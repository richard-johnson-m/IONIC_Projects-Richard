#include "servo.h"

Servo myservo1;
Servo myservo2;
Servo myservo3;

uint32_t servo_time = 0;
int angle = 0;
bool direction = true;

void servo_init() {
  myservo1.attach(2);
  myservo2.attach(3);
  myservo3.attach(4);
}

void servo_control(device_state state) {
  switch (state) {
    case off:
      myservo1.write(0);
      myservo2.write(0);
      myservo3.write(0);
      break;

    case on:
      if (millis() - servo_time >= 10) {
        servo_time = millis();

        myservo1.write(angle);
        myservo2.write(angle);
        myservo3.write(angle);

        if(direction==true){
          angle++;
          if(angle>=90){
            direction=false;
          }
        } else if(direction==false){
          angle--;
          if(angle<=0){
            direction=true;
          }
        }
      }
      break;

  }
}