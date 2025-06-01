#include <Arduino.h>
#include <Servo.h>

#define THROTTLE_IN A0
#define THROTTLE_OUT 9

Servo esc;

void setup() {
  esc.attach(THROTTLE_OUT);
  esc.writeMicroseconds(1500);  // 중립 신호
  delay(2000);
}

void loop() {
  int pwm_in = pulseIn(THROTTLE_IN, HIGH, 25000);

  //if (pwm_in >= 1200 && pwm_in <= 2000) {
    int neutral = 1500;
    int reduced_pwm;

    if (pwm_in > neutral) {
      reduced_pwm = neutral + (pwm_in - neutral) / 5; // 전진 1/5 속도
    } else if (pwm_in < neutral) {
      reduced_pwm = neutral - (neutral - pwm_in) / 5; // 후진 1/5 속도
    } else {
      reduced_pwm = neutral; // 정지
    }

    esc.writeMicroseconds(reduced_pwm);
  //}
}