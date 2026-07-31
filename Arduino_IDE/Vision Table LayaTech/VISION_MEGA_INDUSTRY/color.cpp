#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "color.h"
#include "cartesian.h"

#define redpin 150
#define greenpin 130
#define bluepin 112

#define commonAnode true

byte gammatable[256];

Adafruit_TCS34725 tcs =
Adafruit_TCS34725(
TCS34725_INTEGRATIONTIME_50MS,
TCS34725_GAIN_4X
);

void color_init() {

  if (tcs.begin()) {

  } else {

    while (1);
  }

#if defined(ARDUINO_ARCH_ESP32)

  ledcAttach(redpin, 12000, 8);
  ledcAttach(greenpin, 12000, 8);
  ledcAttach(bluepin, 12000, 8);

#else

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

#endif

  for (int i = 0; i < 256; i++) {

    float x = i;

    x /= 255;

    x = pow(x, 2.5);

    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    }

    else {
      gammatable[i] = x;
    }
  }
}

const char* color_read() {

  static char color[2];

  float red, green, blue;

  tcs.setInterrupt(false);

  tcs.getRGB(&red, &green, &blue);

  tcs.setInterrupt(true);

#if defined(ARDUINO_ARCH_ESP32)

  ledcWrite(1, gammatable[(int)red]);
  ledcWrite(2, gammatable[(int)green]);
  ledcWrite(3, gammatable[(int)blue]);

#else

  analogWrite(redpin, gammatable[(int)red]);
  analogWrite(greenpin, gammatable[(int)green]);
  analogWrite(bluepin, gammatable[(int)blue]);

#endif

  if (red > green + 20) {

    strcpy(color, "R");
    // red_count++;
  }

  else if (green > red + 20) {

    strcpy(color, "G");
    // green_count++; 
  }
else{
  strcpy(color, "G");
}
  return color;
}