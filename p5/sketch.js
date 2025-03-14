let port;
let redState = 0, yellowState = 0, greenState = 0, brightness = 0, mode = 0;
let redSlider, yellowSlider, greenSlider;
let connectBtn;
let modeButtons = [];

function setup() {
  createCanvas(500, 380);
  background(220);

  // 시리얼 포트 객체 생성
  port = createSerial(); // Web Serial 컨트롤 객체

  // 이전 연결 정보를 통해 자동으로 연결
  let usedPorts = usedSerialPorts();
  if (usedPorts.length > 0) {
    port.open(usedPorts[0], 9600); // 과거에 사용했던 포트에 9600 baud rate로 연결
  }

  // Web Serial 연결 버튼 생성
  connectBtn = createButton("Connect to Arduino");
  connectBtn.position(180, 10);
  connectBtn.mousePressed(connectBtnClick);

    // 모드 변경 버튼 추가
  let modeLabels = ["Normal", "Button1", "Button2", "Button3"];
  for (let i = 0; i < 4; i++) {
    modeButtons[i] = createButton(modeLabels[i]);
    modeButtons[i].position(60 + i * 100, 340);
    modeButtons[i].mousePressed(() => toggleMode(i)); // 두 번 클릭하면 Normal로 돌아가도록 수정
  }
  
  // 슬라이더 생성 (LED 지속 시간 조절)
  redSlider = createSlider(0, 4000, 2000);
  redSlider.position(35, 220);
  redSlider.input(sendData);

  yellowSlider = createSlider(0, 4000, 500);
  yellowSlider.position(180, 220);
  yellowSlider.input(sendData);

  greenSlider = createSlider(0, 4000, 2000);
  greenSlider.position(330, 220);
  greenSlider.input(sendData);
}

function draw() {
  background(220);

  // 🟢 시리얼 데이터 수신 (draw() 내부에서 처리)
  if (port.available()) {
    let data = port.readUntil("\n"); // 한 줄 단위로 읽기
    processSerialData(data.trim()); // 개행문자 제거 후 처리
  }
  
  // 신호등 LED 상태 표시 (실시간 반영)
  drawTrafficLights();
  
  // 신호 길이 슬라이더 및 값 표시
  textSize(14);
  fill(0);
  text("0s", 40, 250);
  text("4s", 160, 250);
  text("0s", 190, 250);
  text("4s", 310, 250);
  text("0s", 340, 250);
  text("4s", 460, 250);

  drawBrightnessGauge();
  drawModeIndicator();
}

// 🟢 신호등 LED 상태를 실시간 반영하여 표시하는 함수
function drawTrafficLights() {
  noStroke();

  let redAlpha = redState ? brightness : 0;    
  let yellowAlpha = yellowState ? brightness : 0;
  let greenAlpha = greenState ? brightness : 0;

  fill(255, 0, 0, redAlpha);
  ellipse(100, 150, 50, 50);
  fill(0);
  textSize(20);
  textAlign(CENTER, CENTER);
  text("R", 100, 150);

  fill(255, 255, 0, yellowAlpha);
  ellipse(245, 150, 50, 50);
  fill(0);
  text("Y", 245, 150);

  fill(0, 255, 0, greenAlpha);
  ellipse(390, 150, 50, 50);
  fill(0);
  text("G", 390, 150);
}


// 신호등 모드 인디케이터
function drawModeIndicator() {
  let labels = ["Normal", "Button1", "Button2", "Button3"];
  let colors = ['gray', 'gray', 'gray', 'gray'];
  colors[mode] = 'blue';

  for (let i = 0; i < 4; i++) {
    fill(colors[i]);
    strokeWeight(1);
    stroke(0, 0, 0); 
    rect(50 + i * 100, 290, 80, 40);

    fill(255);
    textSize(14);
    textAlign(CENTER, CENTER);
    text(labels[i], 90 + i * 100, 310);
  }
}

// 밝기 인디케이터 (원형 게이지)
function drawBrightnessGauge() {
  fill(255);
  stroke(0);
  strokeWeight(2);
  ellipse(245, 80, 60, 60); // 원형 게이지 배경

  noFill();
  stroke(0, 0, 255);
  strokeWeight(6);
  let angle = map(brightness, 0, 255, 0, TWO_PI);
  arc(245, 80, 50, 50, -HALF_PI, -HALF_PI + angle);
}

// 🟢 시리얼 데이터 처리 함수
function processSerialData(data) {
  let values = data.split(",");
  if (values.length === 5) {
    redState = int(values[0]);
    yellowState = int(values[1]);
    greenState = int(values[2]);
    brightness = int(values[3]);
    mode = int(values[4]);
  }
}

// 슬라이더 값 전송 (아두이노로)
function sendData() {
  if (port.opened()) {
    let msg = redSlider.value() + "," + yellowSlider.value() + "," + greenSlider.value() + "\n";
    port.write(msg);
  }
}

// 🟢 모드 변경 버튼을 두 번 누르면 Normal(0)로 복귀
function toggleMode(selectedMode) {
  if (mode === selectedMode) {
    mode = 0; // 현재 모드와 같으면 Normal(0)로 복귀
  } else {
    mode = selectedMode; // 새로운 모드 적용
  }
  sendModeToArduino(mode);
}

function sendModeToArduino(selectedMode) {
  if (port.opened()) {
    port.write("mode:" + selectedMode + "\n");
  }
}

// Arduino 연결/해제 버튼 동작
function connectBtnClick() {
  if (!port.opened()) {
    port.open(9600); // 9600 baudRate
  } else {
    port.close();
  }
}

