#include <Arduino.h>
#include "stepper.h"
#include "Cartesian.h"
#include "uart_reader.h"

//y_ bottom max 16320 from home
//y_cw -up(toward limit),,ccw-down


// x_ CW->towards limit
// ccw ->away to limit
//x pick 9000
// x_pass->56000
// x_fail ->74000

//xluimit30
//tylimit 20

void setup() {
  Serial.begin(115200);
  stepper_init();
  state = home;
  pinMode(11, OUTPUT);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
}

void loop() {

  uart_parse();
  if (status == 1 || status == 3) {
    cartesian_process();
  }else if(status ==2){
    cartesian_off();
  }
      // cartesian_process();

  // stepperRun(y_motor, 15000, CW);
  // digitalWrite(11,HIGH);
  // stepperRun(x_motor, 10000, CW);
  //   stepperRun(y_motor, 10000, CW);
  // stepperRun(y_motor, 3000, CW);

  //    uint8_t x_limit_read = digitalRead(A0);  //normally 1 signal->0
  //   uint8_t y_limit_read = digitalRead(A5);
  //  if (x_limit_read == 1) {
  //         stepperRun(x_motor, 4000, CW);
  //       } else if (x_limit_read == 0) {
  //         stepperStop(x_motor);
  //         x_limit_reached = true;
  //       }
  //       if (y_limit_read == 1) {
  //         stepperRun(y_motor, 3000, CW);
  //       } else if (y_limit_read == 0) {
  //         stepperStop(y_motor);
  //         y_limit_reached = true;
  //       }
  // static unsigned long prevPrint = 0;
  // if (millis() - prevPrint >= 100) {
  //   prevPrint = millis();

  //   if (state == home) {

  //     Serial.println("HOME");

  //   } else if (state == conveyor_run) {

  //     Serial.println("CONVEYOR_RUN");

  //   } else if (state == object_color_detection) {

  //     Serial.println("OBJECT_COLOR_DETECTION");

  //   } else if (state == object_destiny) {

  //     Serial.println("OBJECT_DESTINY");

  //   } else if (state == pick_and_place) {

  //     Serial.println("PICK_AND_PLACE");

  //   } else if (state == reset_flag) {

  //     Serial.println("RESET_FLAG");
  // if(state==0){
  //   Serial.println("HOME");
  // }else if(state ==)
  // Serial.println(x_motor.pulsecount);
  // Serial.print("X_pulse : ");
  // Serial.print(x_motor.pulsecount);
  // Serial.print("  y_pulse : ");
  // Serial.print(y_motor.pulsecount);

  // Serial.print("  x_stepp : ");
  // Serial.print(x_step_pulse);

  // Serial.print("      x_limit : ");
  // Serial.print(digitalRead(A0));
  // Serial.print("            y_limit  : ");
  // Serial.println(digitalRead(A5));
  //  Serial.print("            magnet  : ");
  //   Serial.println(hold_magnet);
  // Serial.print("      first_ir : ");
  // Serial.print(digitalRead(11));
  // Serial.print("            second_ir  : ");
  // Serial.println(digitalRead(13));
  // Serial.print("        ");
  // Serial.println(state);

  //   Serial.print("       colorr  : ");
  // Serial.println(color);

  //   Serial.print("STATE: ");
  // Serial.print(status);

  // Serial.print("  IR1: ");
  // Serial.print(first_ir);

  // Serial.print("  IR2: ");
  // Serial.print(second_ir);

  // Serial.print("  COLOR: ");
  // Serial.println(color);
}
