#include <Arduino.h>
#include <PinChangeInterrupt.h>
#include <math.h>

// 수신기 입력 핀
#define CH3_PIN A0  // 제어 값
#define CH5_PIN A1  // On/Off 모드
#define CH6_PIN A2  // 밝기 조절 모드
#define CH9_PIN A3  // 색상 조절 모드

// 출력 핀
#define RED_LED_PIN     3   // On/Off 전용 빨간색 LED
#define YELLOW_LED_PIN  5   // 밝기 조절 전용 노란색 LED
#define RGB_RED_PIN     6   // RGB 모듈
#define RGB_GREEN_PIN   9
#define RGB_BLUE_PIN    10

// PWM 측정용 변수
volatile unsigned long ch3_start = 0, ch5_start = 0, ch6_start = 0, ch9_start = 0;
volatile int ch3_pulse = 1500, ch5_pulse = 1000, ch6_pulse = 1000, ch9_pulse = 1000;
volatile bool new_ch3 = false, new_ch5 = false, new_ch6 = false, new_ch9 = false;

// 제어 상태 변수
bool led_on = false;
int brightness = 255;
int hue = 0;

void ch3_ISR();
void ch5_ISR();
void ch6_ISR();
void ch9_ISR();
void applyHSVColor(int h, float s, float v);

void setup() {
  Serial.begin(115200);

  pinMode(CH3_PIN, INPUT_PULLUP);
  pinMode(CH5_PIN, INPUT_PULLUP);
  pinMode(CH6_PIN, INPUT_PULLUP);
  pinMode(CH9_PIN, INPUT_PULLUP);

  attachPCINT(digitalPinToPCINT(CH3_PIN), ch3_ISR, CHANGE);
  attachPCINT(digitalPinToPCINT(CH5_PIN), ch5_ISR, CHANGE);
  attachPCINT(digitalPinToPCINT(CH6_PIN), ch6_ISR, CHANGE);
  attachPCINT(digitalPinToPCINT(CH9_PIN), ch9_ISR, CHANGE);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
}

void ch3_ISR() {
  if (digitalRead(CH3_PIN) == HIGH) ch3_start = micros();
  else if (ch3_start) {
    ch3_pulse = micros() - ch3_start;
    new_ch3 = true;
    ch3_start = 0;
  }
}
void ch5_ISR() {
  if (digitalRead(CH5_PIN) == HIGH) ch5_start = micros();
  else if (ch5_start) {
    ch5_pulse = micros() - ch5_start;
    new_ch5 = true;
    ch5_start = 0;
  }
}
void ch6_ISR() {
  if (digitalRead(CH6_PIN) == HIGH) ch6_start = micros();
  else if (ch6_start) {
    ch6_pulse = micros() - ch6_start;
    new_ch6 = true;
    ch6_start = 0;
  }
}
void ch9_ISR() {
  if (digitalRead(CH9_PIN) == HIGH) ch9_start = micros();
  else if (ch9_start) {
    ch9_pulse = micros() - ch9_start;
    new_ch9 = true;
    ch9_start = 0;
  }
}

void loop() {
  if (new_ch3) {
    int val = ch3_pulse;

    // On/Off 제어
    if (ch5_pulse > 1500) {
      led_on = (val > 1500);
      digitalWrite(RED_LED_PIN, led_on ? HIGH : LOW);
      Serial.print("[On/Off] PWM:"); Serial.print(val);
      Serial.print(" LED:"); Serial.println(led_on ? "ON" : "OFF");
    } else {
      digitalWrite(RED_LED_PIN, LOW); // 모드 OFF 시 LED OFF
    }

    // 밝기 조절
    if (ch6_pulse > 1500) {
      brightness = constrain(map(val, 1000, 2000, 0, 255), 0, 255);
      analogWrite(YELLOW_LED_PIN, brightness);
      Serial.print("[Brightness] PWM:"); Serial.print(val);
      Serial.print(" Brightness:"); Serial.println(brightness);
    } else {
      analogWrite(YELLOW_LED_PIN, 0); // 모드 OFF 시 밝기 0
    }

    // 색상 조절
    if (ch9_pulse > 1500) {
      hue = constrain(map(val, 1000, 2000, 0, 360), 0, 360);
      applyHSVColor(hue, 1.0, 1.0);
      Serial.print("[Color] PWM:"); Serial.print(val);
      Serial.print(" Hue:"); Serial.println(hue);
    } else {
      // RGB LED OFF
      analogWrite(RGB_RED_PIN, 0);
      analogWrite(RGB_GREEN_PIN, 0);
      analogWrite(RGB_BLUE_PIN, 0);
    }

    new_ch3 = false;
  }
}


void applyHSVColor(int h, float s, float v) {
  float c = v * s;
  float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
  float m = v - c;
  float r = 0, g = 0, b = 0;

  if (h < 60)      { r = c; g = x; b = 0; }
  else if (h < 120){ r = x; g = c; b = 0; }
  else if (h < 180){ r = 0; g = c; b = x; }
  else if (h < 240){ r = 0; g = x; b = c; }
  else if (h < 300){ r = x; g = 0; b = c; }
  else             { r = c; g = 0; b = x; }

  analogWrite(RGB_RED_PIN,   (r + m) * 255);
  analogWrite(RGB_GREEN_PIN, (g + m) * 255);
  analogWrite(RGB_BLUE_PIN,  (b + m) * 255);
}
