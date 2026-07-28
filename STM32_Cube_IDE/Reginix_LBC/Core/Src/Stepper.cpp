#include "Stepper.h"

Stepper::Stepper(TIM_HandleTypeDef *htim, uint32_t channel,
                 GPIO_TypeDef *dirPort, uint16_t dirPin,
                 GPIO_TypeDef *enPort, uint16_t enPin,
                 GPIO_TypeDef *homePort, uint16_t homePin,
                 GPIO_TypeDef *endPort, uint16_t endPin)
{
    this->htim = htim;
    this->channel = channel;

    this->dirPort = dirPort;
    this->dirPin = dirPin;

    this->enPort = enPort;
    this->enPin = enPin;

    this->homePort = homePort;
    this->homePin = homePin;

    this->endPort = endPort;
    this->endPin = endPin;

    running = false;
    done = true;
    mode = IDLE;
}

void Stepper::Init(uint16_t microstep, uint32_t stepsPerRev)
{
    this->microstep = microstep;
    this->stepsPerRev = stepsPerRev;
}

void Stepper::Enable()
{
    HAL_GPIO_WritePin(enPort, enPin, GPIO_PIN_RESET);
}

void Stepper::Disable()
{
    HAL_GPIO_WritePin(enPort, enPin, GPIO_PIN_SET);
}

//void Stepper::SetSpeed(uint32_t speed)
//{
//    uint32_t timer_clk = HAL_RCC_GetPCLK2Freq();
//    uint32_t arr = (timer_clk / speed) - 1;
//
//    __HAL_TIM_SET_AUTORELOAD(htim, arr);
//    __HAL_TIM_SET_COMPARE(htim, channel, arr / 2);
//}
void Stepper::SetSpeed(float rpm)
{
    if (rpm <= 0) return;

    // Your motor config
    uint32_t stepsPerRevTotal = stepsPerRev * microstep; // 200 * 16 = 3200

    // Convert RPM → frequency (Hz)
    float freq = (rpm * stepsPerRevTotal) / 60.0f;

    // Timer setup (PSC = 71 → 1 MHz timer clock)
    uint32_t timer_clk = 1000000; // 1 MHz after prescaler

    uint32_t arr = (uint32_t)((timer_clk / freq) - 1);

    // Safety limits
    if (arr < 10) arr = 10;
    if (arr > 65535) arr = 65535;

    __HAL_TIM_SET_AUTORELOAD(htim, arr);
    __HAL_TIM_SET_COMPARE(htim, channel, arr / 2);
}
void Stepper::MoveForward(uint32_t pulses, uint32_t speed)
{
    HAL_GPIO_WritePin(dirPort, dirPin, GPIO_PIN_RESET);

    targetPulses = pulses;
    currentPulses = 0;

    this->speed = speed;

    mode = MOVE_PULSE;
    running = true;
    done = false;

    Enable();
    SetSpeed(speed);
    HAL_TIM_PWM_Start_IT(htim, channel);
}

void Stepper::MoveReverse(uint32_t pulses, uint32_t speed)
{
    HAL_GPIO_WritePin(dirPort, dirPin, GPIO_PIN_SET);
    MoveForward(pulses, speed);
}

void Stepper::MoveHome(uint32_t speed)
{
    HAL_GPIO_WritePin(dirPort, dirPin, GPIO_PIN_RESET);

    mode = MOVE_HOME;
    running = true;
    done = false;

    Enable();
    SetSpeed(speed);
    HAL_TIM_PWM_Start_IT(htim, channel);
}

void Stepper::MoveEnd(uint32_t speed)
{
    HAL_GPIO_WritePin(dirPort, dirPin, GPIO_PIN_SET);

    mode = MOVE_END;
    running = true;
    done = false;

    Enable();
    SetSpeed(speed);
    HAL_TIM_PWM_Start_IT(htim, channel);
}

void Stepper::Stop()
{
	Disable();
    HAL_TIM_PWM_Stop_IT(htim, channel);
    running = false;
    done = true;
}
void Stepper::pass()
{
	Disable();
    HAL_TIM_PWM_Stop_IT(htim, channel);

}
void Stepper::resume()
{
	Enable();
    HAL_TIM_PWM_Start_IT(htim, channel);
}

void Stepper::HandlePulse()
{
    currentPulses++;

    if (mode == MOVE_PULSE)
    {
        if (currentPulses >= targetPulses)
        {
            Stop();
        }
    }
}

void Stepper::HandleExti(uint16_t GPIO_Pin)
{
    if (mode == MOVE_HOME && GPIO_Pin == homePin)
    {
        Stop();
    }

    if (mode == MOVE_END && GPIO_Pin == endPin)
    {
        Stop();
    }
}

void Stepper::HandleExtio()
{
    if (mode == MOVE_HOME && HAL_GPIO_ReadPin(homePort, homePin))
    {
        Stop();
    }

    if (mode == MOVE_END && HAL_GPIO_ReadPin(endPort, endPin))
    {
        Stop();
    }
}

bool Stepper::IsBusy()
{
    return running;
}

bool Stepper::IsDone()
{
    return done;
}
