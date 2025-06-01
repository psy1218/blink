#include <Arduino.h>
#include <Servo.h>

// === 핀 정의 ===
#define THROTTLE_IN A0      // 수신기에서 속도 PWM 입력
#define STEER_IN    A1      // 수신기에서 조향 PWM 입력
#define THROTTLE_OUT 9      // ESC 출력
#define STEER_OUT    6      // 서보 출력


Servo esc;
Servo steer_servo;

void setup() {
  esc.attach(THROTTLE_OUT);
  steer_servo.attach(STEER_OUT);

  esc.writeMicroseconds(1500);       // ESC 중립
  steer_servo.write(90);             // 서보 중립 (90도)
  delay(2000);
}

void loop() {
  // === 속도 제어 ===
  int pwm_throttle = pulseIn(THROTTLE_IN, HIGH, 25000);
  int neutral = 1500;
  int reduced_pwm;

  if (pwm_throttle > neutral) {
    reduced_pwm = neutral + (pwm_throttle - neutral) / 5;
  } else if (pwm_throttle < neutral) {
    reduced_pwm = neutral - (neutral - pwm_throttle) / 5;
  } else {
    reduced_pwm = neutral;
  }
  esc.writeMicroseconds(reduced_pwm);

  // === 조향 제어 ===
  int pwm_steer = pulseIn(STEER_IN, HIGH, 25000);  // 예: 1000~2000us

  // 수신기 PWM 입력을 서보 각도(0~180도)로 변환
  int angle = map(pwm_steer, 1000, 2000, 40, 140);  // 중립 90도, 제한 범위
  angle = constrain(angle, 40, 140);


  steer_servo.write(angle);
}
