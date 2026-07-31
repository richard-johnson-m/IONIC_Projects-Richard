#include <Arduino.h>
#include "uart_reader.h"

char data[30];

int status;
int first_ir;
int second_ir;
char color;


void uart_parse() {

  if (Serial.available()) {

    int len = Serial.readBytesUntil('\n', data, sizeof(data) - 1);
    data[len] = '\0';

    sscanf(data, "%d,%d,%d,%c",
           &status,
           &first_ir,
           &second_ir,
           &color);

    // Serial.print("Status: ");
    // Serial.print(status);
    // Serial.print(" IR1: ");
    // Serial.print(first_ir);
    // Serial.print(" IR2: ");
    // Serial.print(second_ir);
    // Serial.print(" Color: ");
    // Serial.println(color);
  }
}