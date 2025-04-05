#!/usr/bin/bash
#Raapberry GPIO 핀 번호
pin1=17
pin2=18
pin3=19
pin4=20

# GPIO 핀 output 설정
pinctrl set $pin1 op
pinctrl set $pin2 op
pinctrl set $pin3 op
pinctrl set $pin4 op

# 각 LED를 키는 함수
TurnOnOneLED() {
    case $1 in
    1) pinctrl set $pin1 dh ;;
    2) pinctrl set $pin2 dh ;;
    3) pinctrl set $pin3 dh ;;
    4) pinctrl set $pin4 dh ;;
    esac
}

# 각 LED를 끄는 함수
TurnOffOneLED() {
    case $1 in
    1) pinctrl set $pin1 dl ;;
    2) pinctrl set $pin2 dl ;;
    3) pinctrl set $pin3 dl ;;
    4) pinctrl set $pin4 dl ;;
    esac
}

# 초기 세팅 - 모든 LED를 끄는 상태로 설정
# 이전의 값 초기화 
pinctrl set $pin1 dl
pinctrl set $pin2 dl        
pinctrl set $pin3 dl
pinctrl set $pin4 dl

# LED를 순차적으로 켜고 끄는 루프  
while true; do
    for i in {1..4}; do
        TurnOnOneLED $i
        sleep 1
        TurnOffOneLED $i
    done
done
