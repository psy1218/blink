#!/usr/bin/bash
#Raapberry GPIO 핀 번호
pin1=7
pin2=8
pin3=25

# GPIO 핀 output 설정
pinctrl set $pin1 op
pinctrl set $pin2 op
pinctrl set $pin3 op

blink_LED() {
    pin=$1 #인자를 받고 핀 번호 지정
    delay=$2 #인자를 받고 delay 지정

    while true; do
        pinctrl set $pin dl
        sleep $delay
        pinctrl set $pin dh
        sleep $delay
    done
}

# 초기 세팅 - 모든 LED를 끄는 상태로 설정
pinctrl set $pin1 dl
pinctrl set $pin2 dl
pinctrl set $pin3 dl

# 각각의 LED를 백그라운드에서 병렬로 실행
# '&' 기호를 사용하여 백그라운드에서 실행
blink_LED $pin1 1 &
blink_LED $pin2 2 &
blink_LED $pin3 4 &

wait
