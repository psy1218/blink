#include <Arduino.h>
#include <TaskScheduler.h>
#include <PinChangeInterrupt.h>

const int button1 = 2;
const int button2 = 3;
const int button3 = 4;
const int potPin = A0;
const int redLED = 9;
const int yellowLED = 10;
const int greenLED = 11;

volatile bool redState = LOW;
volatile bool yellowState = LOW;
volatile bool greenState = LOW;

volatile int ledFlag = 0;
volatile int button1flag = 0;
volatile int button2flag = 0;
volatile int button3flag = 0;

// LED 지속 시간  
int RED_TIME    = 2000;
int YELLOW_TIME = 500;
int GREEN_TIME  = 2000;

const int GREEN_BLINK = 1000;
const int GREEN_BLINK_INTERVAL = 166;

const int BLINK = 400;

// 가변저항 값 조절 간격
const int BRIGHTNESS_UPDATE_INTERVAL = 10; 

// LED 상태
bool redOE();
void redOD();
bool yellowOE();
void yellowOD();
bool greenOE();
void greenOD();
void reset();

void Toggle();
bool greenBlinkOE();
void greenBlinkCB();
void greenBlinkOD();

// 버튼 상태
void button1_ISR();
void button2_ISR();
void button3_ISR();

// 가변저항 값 조절
void adjustBrightness();

// send Traffic State
void sendTrafficState();

// 모드 변경
void updateMode(int mode);


Scheduler ts;

Task red(RED_TIME, TASK_ONCE, NULL, &ts, false, redOE, redOD);
Task yellow(YELLOW_TIME, TASK_ONCE, NULL, &ts, false, yellowOE, yellowOD);
Task green(GREEN_TIME, TASK_ONCE, NULL, &ts, false, greenOE, greenOD);
Task greenbasicBlink(GREEN_BLINK_INTERVAL, GREEN_BLINK / (GREEN_BLINK_INTERVAL), greenBlinkCB, &ts, false, greenBlinkOE, greenBlinkOD);

// button2 누르면 모든 LED 깜빡임
Task BlinkAll(BLINK, TASK_FOREVER, &Toggle);

// 가변저항 값 조절
Task adjustBrightnessTask(BRIGHTNESS_UPDATE_INTERVAL, TASK_FOREVER, &adjustBrightness, &ts, true);

// 신호등 상태 전송
Task sendTrafficStateTask(50, TASK_FOREVER, &sendTrafficState, &ts, true);


void setup() {
  Serial.begin(9600);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);

  pinMode(potPin, INPUT);

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  ts.addTask(BlinkAll);

  attachPCINT(digitalPinToPCINT(button1), button1_ISR, FALLING);
  attachPCINT(digitalPinToPCINT(button2), button2_ISR, FALLING);
  attachPCINT(digitalPinToPCINT(button3), button3_ISR, FALLING);

  red.restartDelayed();
}

void loop() {
  ts.execute();
}

// 버튼 1, 2, 3 인터럽트 설정
void button1_ISR() {
  button1flag ^= 1; // 버튼을 누를 때마다 0/1 전환
  Serial.println(button1flag);
  if (button1flag == 1) {
    // LED Task 중단
    red.abort();
    yellow.abort();
    green.abort();
    greenbasicBlink.abort();
    BlinkAll.abort();
    
    Serial.println("abort");
    redState = HIGH;
    yellowState = LOW;
    greenState = LOW;
  } 
  else {
    reset();
    BlinkAll.abort();
    Serial.println("restart");
    red.restartDelayed();
  }
}

void button2_ISR() {
  button2flag ^= 1;
  Serial.println(button2flag);
  if(button2flag == 1){
    // LED Task 중단
    red.abort();
    yellow.abort();
    green.abort();
    greenbasicBlink.abort();

    reset();

    BlinkAll.enable();
  }
  else{
    reset();
    BlinkAll.abort();
    red.restartDelayed();
    Serial.println("restart");
  }
}

void button3_ISR() {
  button3flag ^= 1;
  if(button3flag == 1){
    // LED Task 중단
    red.abort();
    yellow.abort();
    green.abort();
    greenbasicBlink.abort();
    BlinkAll.abort();

    reset();
  }
  else{
    reset();
    BlinkAll.abort();
    red.restartDelayed();
    Serial.println("restart");
  }
}

// LED 밝기 조절 함수
void adjustBrightness() {
  int sensorValue = analogRead(potPin);
  int brightness = map(sensorValue, 0, 1023, 0, 255);

  analogWrite(redLED, redState == HIGH ? brightness : 0);
  analogWrite(yellowLED, yellowState == HIGH ? brightness : 0);
  analogWrite(greenLED, greenState == HIGH ? brightness : 0);
}

// 신호등 상태 전송
void sendTrafficState() {
  int sensorValue = analogRead(potPin);
  int brightness = map(sensorValue, 0, 1023, 0, 255);
  
  Serial.print(redState);
  Serial.print(",");
  Serial.print(yellowState);
  Serial.print(",");
  Serial.print(greenState);
  Serial.print(",");
  Serial.print(brightness);
  Serial.print(",");
  Serial.println(button1flag + 2 * button2flag + 3 * button3flag);
}

void serialEvent() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    data.trim();

    // 모드 변경 처리 (p5.js → Arduino)
    if (data.startsWith("mode:")) {
      int newMode = data.substring(5).toInt(); // "mode:1" → 1로 변환
      if (newMode >= 0 && newMode <= 3) {
        button1flag = (newMode == 1);
        button2flag = (newMode == 2);
        button3flag = (newMode == 3);
        updateMode(newMode); // 모드 변경 처리
      }
    } 
    else {
      // 기존 슬라이더 데이터 처리
      int times[3];
      int index = 0;
      char *token = strtok(const_cast<char *>(data.c_str()), ",");
      while (token != NULL && index < 3) {
        times[index++] = atoi(token);
        token = strtok(NULL, ",");
      }
      if (index == 3) {
        RED_TIME = times[0];
        YELLOW_TIME = times[1];
        GREEN_TIME = times[2];
        red.setInterval(RED_TIME);
        yellow.setInterval(YELLOW_TIME);
        green.setInterval(GREEN_TIME);
      }
    }
  }
}

// 모드 변경 시 LED 상태 업데이트 (p5.js에서 받은 모드 적용)
void updateMode(int mode) {
  // 기존 실행 중인 Task 중단
  red.abort();
  yellow.abort();
  green.abort();
  greenbasicBlink.abort();
  BlinkAll.abort();

  reset();

  switch (mode) {
    case 1:
      redState = HIGH;
      break;
    case 2:
      BlinkAll.enable();
      break;
    case 3:
      // 모든 LED를 꺼두고 대기
      break;
    default:
      red.restartDelayed();
      break;
  }
}

// LED 제어 함수
bool redOE() {
  redState = HIGH;
  Serial.println("red on");
  return true;
}

void redOD() {
  redState = LOW;
  ledFlag = 1;
  Serial.println("red off");

  yellow.restartDelayed();
}

bool yellowOE() {
  yellowState = HIGH;
  Serial.println("yellow on");
  return true;
}

void yellowOD() {
  yellowState = LOW;
  Serial.println("yellow off");
  if(ledFlag==1){
    green.restartDelayed();
  }
  else{
    red.restartDelayed();
  }
}

bool greenOE() {
  greenState = HIGH;
  Serial.println("green on");
  return true;
}

void greenOD() {
  greenState = LOW;
  Serial.println("green off");
  ledFlag = 0;
  greenbasicBlink.restartDelayed();
}


// basic green blink
bool greenBlinkOE() {
  greenState = LOW;
  return true;
}

void greenBlinkCB()  {
  greenState = greenState ^ 1;
}

void greenBlinkOD() {
  greenState = LOW;
  yellow.restartDelayed();
}
// LED Blink
void Toggle(){
  redState = redState ^ 1;
  yellowState = yellowState ^ 1;
  greenState = greenState ^ 1;
}

void reset(){
  redState = LOW;
  yellowState = LOW;
  greenState = LOW;
}