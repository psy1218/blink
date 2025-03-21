# 🚦 신호등 제어 시스템 (Arduino + p5.js)

이 프로젝트는 **아두이노(Arduino)와 p5.js를 활용하여 신호등을 제어**하는 시스템입니다.  
사용자는 **버튼 입력 및 웹 인터페이스를 통해 신호등을 조작**할 수 있으며,  
LED 밝기 및 지속 시간을 조절할 수 있습니다.

---
## 📷 **시연 영상**
[![프로젝트 시연 영상](images/UI.png)](https://youtu.be/BZsCrywXoNk)


## 🛠 **기능 구현**
### ✅ 1. **실시간 LED 상태 표시 (Arduino → p5.js)**
- 아두이노에서 **각 LED(Red, Yellow, Green)의 상태**를 p5.js에 실시간 전송
- 가변저항(Potentiometer) 값을 읽어 LED **밝기를 조절**
- p5.js에서는 **투명도(alpha 값)를 이용하여 LED 밝기 표현**

### ✅ 2. **LED 지속 시간 조절 (p5.js → Arduino)**
- 웹에서 **슬라이더**를 사용하여 **각 LED(빨강, 노랑, 초록)의 지속 시간(0~4초)** 설정 가능
- 설정된 값은 **시리얼 통신을 통해 아두이노에 전송**
- 아두이노에서 **전달받은 지속 시간에 따라 LED 점등 시간 변경**

### ✅ 3. **신호등 모드 변경 (Arduino ↔ p5.js)**
- 아두이노 버튼 또는 **p5.js의 버튼 UI를 통해 신호등 모드 변경**
- 총 **4가지 모드 지원**  
  - **Normal Mode (기본 신호등 동작)**
  - **Button 1 Mode (빨간불 고정)**
  - **Button 2 Mode (모든 LED 깜빡임)**
  - **Button 3 Mode (LED OFF)**
- p5.js에서 **모드 버튼 클릭 시 Arduino에도 실시간 반영 (양방향 통신)**

### ✅ 4. **신호등 밝기 인디케이터 (Arduino → p5.js)**
- 가변저항(Potentiometer) 값(0~1023)을 0~255로 매핑하여 **LED 밝기 조절**
- p5.js에서 **원형 게이지(arc)를 사용하여 밝기 표시**
- 사용자가 가변저항을 조작하면 **실시간으로 밝기 게이지 업데이트**

### ✅ 5. **p5.js에서 신호등 모드 버튼을 두 번 클릭하면 원래 상태로 복귀**
- 아두이노처럼 **모드 버튼을 다시 누르면 원래 상태(Normal)로 복귀**하는 기능 구현
- **토글 방식 지원**
  - 첫 번째 클릭: 해당 모드 활성화
  - 두 번째 클릭: 원래 상태 (Normal)로 복귀

---

## 🔗 **사용 기술**
### ✅ **하드웨어**
- **Arduino Uno**  
- **LED x3 (빨강, 노랑, 초록)**  
- **푸시 버튼 x3**  
- **가변저항 (Potentiometer) x1**  
- **저항 (330Ω ~ 1kΩ) x3**  
- **브레드보드 & 점퍼 와이어**  

### ✅ **소프트웨어**
- **Arduino C++ (TaskScheduler 활용)**
- **p5.js (Web Serial API 사용)**
- **HTML / JavaScript**

---

## 📌 **회로 구성**
### **📍 핀 연결**
| 부품 | 아두이노 핀 번호 |
|------|--------------|
| **버튼 1 (Button1)** | `D2` |
| **버튼 2 (Button2)** | `D3` |
| **버튼 3 (Button3)** | `D4` |
| **빨간 LED (Red LED)** | `D9` |
| **노란 LED (Yellow LED)** | `D10` |
| **초록 LED (Green LED)** | `D11` |
| **가변저항 (Potentiometer)** | `A0` (아날로그 입력) |

### **📍 하드웨어 연결**
#### 🟢 **가변저항 (Potentiometer)**
- **왼쪽 핀** → `GND` 연결  
- **오른쪽 핀** → `5V` 연결  
- **가운데 핀** → `A0` (아날로그 입력) 연결

#### 🟢 **LED (Red, Yellow, Green)**
- **LED (+) 핀** → 아두이노 `D9`, `D10`, `D11` 연결  
- **LED (-) 핀** → **330Ω ~ 1kΩ 저항을 거쳐 GND 연결**

#### 🟢 **버튼 (Push Button)**
- **버튼의 한쪽(+)** → `D2, D3, D4` (각 버튼별)  
- **버튼의 반대쪽(-)** → `GND` 연결

#### 🟢 **전원**
- **아두이노 `5V`** → 회로 내 부품(가변저항, 버튼, 센서 등)에 전원 공급  
- **아두이노 `GND`** → 모든 부품과 연결하여 회로 완성  

---


## 📷 **시연 이미지**

### 🖼 **웹 UI**
![웹 UI](images/UI.png)

### 🖼 **회로도**
![회로 연결도](images/회로연결도.png)


