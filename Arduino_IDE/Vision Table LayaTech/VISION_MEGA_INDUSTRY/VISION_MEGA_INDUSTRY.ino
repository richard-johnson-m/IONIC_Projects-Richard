#include <Arduino.h>
#include "device.h"
#include "servo.h"
#include "cartesian.h"
#include "color.h"
#include "dwin.h"
#include <SoftwareSerial.h>

SoftwareSerial dwinserial(10, 11);  //10->rx,11->tx

#define esp_serial Serial1        //19->rx,18->tx
#define roarm_serial Serial2      //17->rx,16->tx
#define cartesian_serial Serial3  //15-rx,14->tx

#define obj_ir_pin 22
#define end_ir_pin 23
#define roarm_tower 6
#define cartesian_tower 5
#define ground_relay 27
#define status_pin 30

String buffer = "";
bool recieving;
uint8_t d_id, d_state;

/////set the default state
device_state servo_state = off;
device_state roarm_state = on;
device_state cartesian_state =on;

unsigned long prevTime = 0;
bool relayState = false;
uint32_t relay_test = 0;

void setup() {

  Serial.begin(115200);
  esp_serial.begin(9600);      // For esp to mega
  roarm_serial.begin(115200);  // from mega to roarm uart
  cartesian_serial.begin(115200);
  dwinserial.begin(115200);

  servo_init();
  color_init();

  pinMode(obj_ir_pin, INPUT);  ///frist ir
  pinMode(end_ir_pin, INPUT);  ///second ir

  pinMode(roarm_tower, OUTPUT);      //roarm tower lamp relay
  pinMode(cartesian_tower, OUTPUT);  //cartesian tower lamp relay

  // pinMode(30, INPUT);   //status pin from uno but no t used yet
  pinMode(29, OUTPUT);  //electromagnet pin but controlled with uno pin
  pinMode(27, OUTPUT);  // Relay for humidity 5v
  // pinMode(24,OUTPUT); //BUZER
  // digitalWrite(24, LOW);
  relay_test = millis();
  // Serial.println("MEGA READ");
}

void loop() {
  // digitalWrite(24, LOW);

  uart_parse();  //get data from esp

  ir_check();
  servo_control(servo_state);
  roarm_status(roarm_state);
  cartesian_control(cartesian_state);
  display(cartesian_state);

  // digitalWrite(27, HIGH);
  // delay(500);
  // digitalWrite(27, LOW);
  // delay(500);
  // Serial.print("color : ");
  // Serial.println(color);

  // Serial.print("count : ");
  // Serial.println(count);

  Serial.print("Servo : ");
  Serial.print(servo_state);

  Serial.print("   RoArm : ");
  Serial.print(roarm_state);

  Serial.print("   cartesian : ");
  Serial.println(cartesian_state);
  // Serial.print("   cartesian : ");
  // Serial.println(cartesian_state);
  // }
}
void uart_parse() {
  if (esp_serial.available() > 0) {
    char c = esp_serial.read();
    if (c == '#') {
      buffer = "";
      recieving = true;
    } else if (c == '$') {
      recieving = false;
      process_command(buffer);
    } else if (recieving) {
      buffer += c;
    }
  }
}

void process_command(String buffer) {
  if (buffer.length() < 3) return;
  if (buffer.charAt(1) != ',') return;

  d_id = buffer.charAt(0) - '0';
  d_state = buffer.charAt(2) - '0';

  device_id id = (device_id)d_id;
  device_state state = (device_state)d_state;

  device_control(id, state);
}
