#include <Arduino.h>

static int ww;
static int cw;

static bool lampState;
static bool boost;

void changeLampState(){
    if (lampState == false){
        lampState = true;
    }else{
        lampState = false;
    }
}

bool getLampState(){
    return lampState;
}

int getWW(){
    return ww;
}

int getCW(){
    return cw;
}

bool getBoost(){
    return boost;
}

void setWW(int newValue){
    ww = newValue;
}

void setCW(int newValue){
    cw = newValue;
}

void setLampState(bool newLampState){
    lampState = newLampState;
}

void setBoost(bool newBoost){
    boost = newBoost;
}