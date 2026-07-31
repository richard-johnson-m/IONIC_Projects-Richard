#include"stepper.h"
#include <avr/interrupt.h>

Stepper x_motor = {&PORTD, (1 << 2), 5, false, false, 0, 0, 0};

Stepper y_motor = {&PORTD, (1 << 3), 6, false, false, 0, 0, 0};

Stepper conv_motor = {&PORTD, (1 << 4), 7, false, false, 0, 0, 0};

ISR(TIMER1_COMPA_vect)
{
  stepperUpdate(x_motor);
  stepperUpdate(y_motor);
  stepperUpdate(conv_motor);
}

void stepper_init() {
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(3, OUTPUT);
  pinMode(6, OUTPUT);

  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);


  pinMode(8, OUTPUT);
  digitalWrite(8,LOW);

  // pinMode(9, INPUT_PULLUP);   //xlimit
  // pinMode(10, INPUT_PULLUP);  //y limit

  cli();

  TCCR1A = 0;
  TCCR1B = 0;

  OCR1A = 31;

  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11);

  TIMSK1 |= (1 << OCIE1A);

  sei();
  
}


void stepperRun(Stepper &motor, int speed, bool direction)
{
  digitalWrite(motor.dirPin, direction);
digitalWrite(8, LOW); 
  if(speed <= 0)
  {
    motor.enabled = false;
    return;
  }

  uint16_t newTicks = 62500UL / speed;

  if(newTicks == 0)
    newTicks = 1;

  if(!motor.enabled)
  {
    motor.counter = newTicks;
  }

  motor.ticks = newTicks;
  motor.enabled = true;
}
void stepperStop(Stepper &motor)
{
  motor.enabled = false;

  motor.stepState = LOW;
  //  digitalWrite(8, HIGH);   // Disable drivers
  *motor.stepPort &= ~motor.stepBitMask;
}

void stepperUpdate(Stepper &motor)
{
  if (!motor.enabled)
    return;

  if (motor.counter > 0)
    motor.counter--;

  if (motor.counter == 0)
  {
    *motor.stepPort |= motor.stepBitMask;
    *motor.stepPort &= ~motor.stepBitMask;

    motor.pulsecount++;

    motor.counter = motor.ticks;
  }
}
