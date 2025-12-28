#ifndef AWS_SERVICE_H
#define AWS_SERVICE_H

#include "alarm_service.h"

void connectToAWS();
bool isAWSConnected();
void publishReportedState();
void changeLampState();
void messageHandler(String &topic, String &payload);
void maintainAWS();

#endif
