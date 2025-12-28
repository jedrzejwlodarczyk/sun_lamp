#ifndef LAMP_SERVICE_H
#define LAMP_SERVICE_H

void changeLampState();
bool getLampState();
int getWW();
int getCW();
bool getBoost();
void setWW(int newValue);
void setCW(int newValue);
void setLampState(bool newLampState);
void setBoost(bool newBoost);

#endif