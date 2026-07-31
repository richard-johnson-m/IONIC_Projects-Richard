#include "Arduino.h"
#include "roarm.h"

#define roarm_serial Serial2

void roarm_status(device_state state) {


  switch (state) {

      //////// STOP ////////
    case off:
      // Serial.println("RoArm stop ");
      // digitalWrite(roarm_tower,HIGH);
      digitalWrite(ground_relay, LOW);
       digitalWrite(27, LOW); //humidity
      // roarm_serial.println("{\"T\":210}");
      break;
case on:

  roarm_serial.println("{\"T\":114,\"led\":255,\"r\":255,\"g\":255,\"b\":255}");

  digitalWrite(roarm_tower, LOW);
  digitalWrite(27, HIGH);

  if (millis() - lastTime >= 1000)
  {
    lastTime = millis();

    switch(stepIndex)
    {
        case 0:   // CENTER
          roarm_serial.println("{\"T\":104,\"x\":209,\"y\":0,\"z\":18,\"t\":3.14,\"spd\":1.5}");
          break;

        case 1:   // LEFT
          roarm_serial.println("{\"T\":104,\"x\":209,\"y\":80,\"z\":18,\"t\":3.14,\"spd\":1.5}");
          break;

        case 2:   // RIGHT
          roarm_serial.println("{\"T\":104,\"x\":209,\"y\":-80,\"z\":18,\"t\":3.14,\"spd\":1.5}");
          break;

        case 3:   // CENTER
          roarm_serial.println("{\"T\":104,\"x\":209,\"y\":0,\"z\":18,\"t\":3.14,\"spd\":1.5}");
          break;

        case 4:   // UP
          roarm_serial.println("{\"T\":101,\"joint\":3,\"rad\":2.25,\"spd\":0,\"acc\":10}");
          break;

        case 5:   // DOWN
          roarm_serial.println("{\"T\":101,\"joint\":3,\"rad\":2.65,\"spd\":0,\"acc\":10}");
          break;

    
    }

    stepIndex++;

    if(stepIndex > 5)
    {
      stepIndex = 0;
    }
  }

  break;
    //////// ERRATIC ////////
 case erratic:

  digitalWrite(roarm_tower, HIGH);

  if (millis() - lastTime >= 650) {

    lastTime = millis();

    switch (stepIndex) {

      case 0:

        roarm_serial.println("{\"T\":121,\"joint\":1,\"angle\":-60,\"spd\":55,\"acc\":10}");
        break;

      case 1:

        roarm_serial.println("{\"T\":121,\"joint\":3,\"angle\":120,\"spd\":50,\"acc\":10}");
        break;

      case 2:

        roarm_serial.println("{\"T\":121,\"joint\":1,\"angle\":70,\"spd\":55,\"acc\":10}");
        break;

      case 3:

        roarm_serial.println("{\"T\":121,\"joint\":4,\"angle\":20,\"spd\":45,\"acc\":10}");
        break;

      case 4:

        roarm_serial.println("{\"T\":121,\"joint\":3,\"angle\":80,\"spd\":50,\"acc\":10}");
        break;

      case 5:

        roarm_serial.println("{\"T\":121,\"joint\":1,\"angle\":-30,\"spd\":55,\"acc\":10}");
        break;

      case 6:

        roarm_serial.println("{\"T\":121,\"joint\":4,\"angle\":50,\"spd\":45,\"acc\":10}");
        break;

      case 7:

        roarm_serial.println("{\"T\":121,\"joint\":2,\"angle\":10,\"spd\":35,\"acc\":10}");
        break;

      case 8:

        roarm_serial.println("{\"T\":121,\"joint\":2,\"angle\":-10,\"spd\":35,\"acc\":10}");
        break;
    }

    stepIndex++;

    if (stepIndex > 8) {
      stepIndex = 0;
    }
  }

  break;
  }
}