#include "Cartesian.h"
#include <Arduino.h>
#include "stepper.h"
#include "uart_reader.h"

state_machine state;
cartesian_state cart_state;

uint32_t move_start_time = 0;
bool x_ramp_active = false;

#define x_speed 25000
#define y_speed 12000
#define conv_speed 1500
#define slow_conv_speed 1900

uint8_t hold_magnet = 0;

#define max_y_pulse 15500
uint16_t getRampSpeed(uint16_t target_speed);
uint32_t dequeue();
void enqueue(uint32_t pulse);

#define x_pick_pulse 9000
#define x_pass_pulse 46000
#define x_fail_pulse 74000

uint32_t x_step_pulse = 0;

#define QUEUE_SIZE 5

uint32_t pulse_queue[QUEUE_SIZE];

uint8_t queue_front = 0;
uint8_t queue_rear = 0;
uint8_t queue_count = 0;


bool prev_first_ir = false;
bool prev_second_ir = false;

bool waiting_before_color = false;

bool color_detection_active = false;
uint32_t color_detection_start = 0;
uint16_t red_count = 0;
uint16_t green_count = 0;

bool machine_busy = false;
void process_color_detection();
void monitor_ir_events() {
  if (first_ir == 1 && prev_first_ir == 0) {
    color_detection_active = true;
    color_detection_start = millis();

    red_count = 0;
    green_count = 0;
  }

  prev_first_ir = first_ir;
  prev_second_ir = second_ir;
}

bool x_limit_reached = false;
bool y_limit_reached = false;
bool y_picked_delay = false;
bool y_placed_delay = false;

bool C = false;

void cartesian_process() {
  uint8_t x_limit_read = digitalRead(A0);  //normally 1 signal->0
  uint8_t y_limit_read = digitalRead(A5);  //normally 1 signal->0

  monitor_ir_events();
  process_color_detection();

  if (hold_magnet == 1) {
    digitalWrite(11, HIGH);
  } else if (hold_magnet == 0) {
    digitalWrite(11, LOW);
  }
  switch (state) {
    case home:
      {
        stepperRun(conv_motor, conv_speed, CW);
        // stepperStop(conv_motor);

        if (!x_limit_reached) {
          if (y_limit_reached && x_limit_read == 1) {
            if (!x_ramp_active) {
              x_ramp_active = true;

              move_start_time = millis();
            }
            stepperRun(x_motor,
                       getRampSpeed(x_speed),
                       CW);
          } else if (x_limit_read == 0) {
            stepperStop(x_motor);
            x_limit_reached = true;
          }
        }

        if (!y_limit_reached) {
          if (y_limit_read == 1) {
            stepperRun(y_motor,
                       y_speed,
                       CW);
          } else {
            stepperStop(y_motor);
            y_limit_reached = true;
          }
        }

        if (x_limit_reached && y_limit_reached) {
          x_motor.pulsecount = 0;
          y_motor.pulsecount = 0;
          x_ramp_active = false;

          stepperStop(x_motor);
          stepperStop(y_motor);

          if (second_ir == 1 && queue_count > 0) {
            state = object_destiny;
          } else {
            state = conveyor_run;
          }
        }
        break;
      }
    case conveyor_run:
      {
        stepperRun(conv_motor, conv_speed, CW);

        if ((second_ir == 1) && (queue_count > 0) && (!machine_busy)) {
          state = home;
        }
        break;
      }

      // case object_color_detection:
      //   {
      //     stepperStop(x_motor);
      //     stepperStop(y_motor);
      //     x_ramp_active = false;
      //     // Serial.println("obj color detection");
      //     static uint16_t red_count = 0;
      //     static uint16_t green_count = 0;
      //     static uint32_t color_detect_time = 0;

      //     // First time entering state
      //     if (!waiting_before_color) {
      //       waiting_before_color = true;
      //       waiting_start_time = millis();
      //     }
      //     // Run conveyor for 0.5 sec
      //     if (millis() - waiting_start_time < 1500) {
      //       stepperRun(conv_motor, conv_speed, CW);
      //     } else {
      //       // stepperStop(conv_motor);
      //       if (color_detect_time == 0) {
      //         color_detect_time = millis();
      //       }
      //       if (millis() - color_detect_time <= 500) {
      //         if (color == 'R') {
      //           red_count++;
      //         } else if (color == 'G') {
      //           green_count++;
      //         }
      //       } else {
      //         if (status == 1) {
      //           if (red_count > green_count) {
      //             enqueue(x_fail_pulse);
      //           } else {
      //             enqueue(x_pass_pulse);
      //           }
      //         } else if (status == 3) {
      //           if (red_count > green_count) {
      //             enqueue(x_pass_pulse);
      //           } else {
      //             enqueue(x_fail_pulse);
      //           }
      //         }
      //         color_detect_time = 0;
      //         red_count = 0;
      //         green_count = 0;
      //         first_ir_reached_slow = 0;
      //         waiting_before_color = false;
      //         waiting_start_time = 0;
      //         if (resume_pick_place) {
      //           resume_pick_place = false;
      //           state = pick_and_place;
      //         } else {
      //           state = conveyor_run;
      //         }
      //       }
      //     }

      //     break;
      //   }

    case object_destiny:
      {
        // stepperStop(conv_motor);

        if (queue_count > 0) {
          x_step_pulse = dequeue();
          machine_busy = true;
          state = pick_and_place;
        }

        break;
      }
    case pick_and_place:
      {
        Serial.print("CART = ");
        if (cart_state == y_pick_down) {
          Serial.println("Y_PICK_DOWN");
        }

        else if (cart_state == y_pick_up) {
          Serial.println("Y_PICK_UP");
        }

        else if (cart_state == x_goes_right) {
          Serial.println("X_GOES_RIGHT");
        }

        else if (cart_state == y_place_down) {
          Serial.println("Y_PLACE_DOWN");
        }
        Serial.println("pick n place");

        if (cart_state == y_pick_down) {
          if (x_motor.pulsecount < x_pick_pulse) {
            if (!x_ramp_active) {
              x_ramp_active = true;
              move_start_time = millis();
            }
            stepperRun(x_motor, getRampSpeed(x_speed), CCW);
            return;
          }
          x_ramp_active = false;
          stepperStop(x_motor);
        }

        switch (cart_state) {

          case y_pick_down:
            {
              hold_magnet = 1;

              static uint32_t wait_delay = 0;
              if (y_motor.pulsecount < max_y_pulse) {
                stepperRun(y_motor, y_speed, CCW);
              } else {
                stepperStop(y_motor);
                if (!y_picked_delay) {
                  y_picked_delay = true;
                  wait_delay = millis();
                }
              }
              if (y_picked_delay && (millis() - wait_delay >= 500)) {
                y_picked_delay = false;
                cart_state = y_pick_up;
              }

              break;
            }
          case y_pick_up:
            {
              hold_magnet = 1;

              if (y_limit_read == 1) {
                stepperRun(y_motor, y_speed, CW);
              } else if (y_limit_read == 0) {

                stepperStop(y_motor);
                cart_state = x_goes_right;
                y_motor.pulsecount = 0;
                x_motor.pulsecount = 0;
              }
              break;
            }
          case x_goes_right:
            {
              hold_magnet = 1;

              if (x_motor.pulsecount < x_step_pulse) {
                if (!x_ramp_active) {
                  x_ramp_active = true;
                  move_start_time = millis();
                }
                stepperRun(x_motor, getRampSpeed(x_speed), CCW);
              } else if (x_motor.pulsecount >= x_step_pulse) {
                x_ramp_active = false;
                stepperStop(x_motor);
                cart_state = y_place_down;
              }
              break;
            }
          case y_place_down:
            {
              static uint32_t waiting_delay = 0;
              if (y_motor.pulsecount < 15000) {
                stepperRun(y_motor, y_speed, CCW);
              } else {
                stepperStop(y_motor);
                hold_magnet = 0;
                if (!y_placed_delay) {
                  y_placed_delay = true;
                  waiting_delay = millis();
                }
              }
              if (y_placed_delay && (millis() - waiting_delay >= 500)) {
                y_placed_delay = false;
                state = reset_flag;
              }
            }
            break;
        }
      }
      break;

    case reset_flag:
      hold_magnet = 0;
      cart_state = y_pick_down;
      x_limit_reached = false;
      y_limit_reached = false;
      x_motor.pulsecount = 0;
      y_motor.pulsecount = 0;
      y_picked_delay = false;
      y_placed_delay = false;
      machine_busy = false;
      state = home;
      break;
  }
  static uint32_t prevPrint = 0;

  if (millis() - prevPrint >= 200) {
    prevPrint = millis();
    Serial.print("STATE = ");
    if (state == home) {
      Serial.println("HOME");
    } else if (state == conveyor_run) {
      Serial.println("CONVEYOR_RUN");
    } else if (state == object_color_detection) {
      Serial.println("COLOR_DETECTION");
    } else if (state == object_destiny) {
      Serial.println("OBJECT_DESTINY");
    } else if (state == reset_flag) {
      Serial.println("RESET_FLAG");
    }
  }
}

uint16_t getRampSpeed(uint16_t target_speed) {
  uint32_t elapsed = millis() - move_start_time;

  if (elapsed >= 1000) {
    return target_speed;
  }
  return map(elapsed, 0, 1000, target_speed / 5, target_speed);
}

void cartesian_off()
{
    stepperStop(conv_motor);
    stepperStop(x_motor);
    stepperStop(y_motor);

    x_ramp_active = false;
    move_start_time = 0;

    digitalWrite(8, HIGH);
}
void enqueue(uint32_t pulse) {
  if (queue_count < QUEUE_SIZE) {
    pulse_queue[queue_rear] = pulse;
    queue_rear++;
    if (queue_rear >= QUEUE_SIZE) {
      queue_rear = 0;
    }
    queue_count++;
  }
}
uint32_t dequeue() {
  uint32_t value = 0;

  if (queue_count > 0) {
    value = pulse_queue[queue_front];
    queue_front++;
    if (queue_front >= QUEUE_SIZE) {
      queue_front = 0;
    }
    queue_count--;
  }
  return value;
}

void process_color_detection() {
  if (!color_detection_active)
    return;

  uint32_t elapsed =
    millis() - color_detection_start;

  // wait 1 second for object to reach sensor

  if (elapsed < 1000)
    return;

  // sample for next 500ms

  if (elapsed < 1500) {
    if (color == 'R')
      red_count++;

    else if (color == 'G')
      green_count++;

    return;
  }

  // result

  if (status == 1) {
    if (red_count > green_count)
      enqueue(x_fail_pulse);
    else
      enqueue(x_pass_pulse);
  } else if (status == 3) {
    if (red_count > green_count)
      enqueue(x_pass_pulse);
    else
      enqueue(x_fail_pulse);
  }

  color_detection_active = false;
}