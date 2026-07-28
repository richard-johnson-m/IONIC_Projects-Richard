/*
 * SystemController.h
 *
 *  Created on: 20-Mar-2026
 *      Author: tamil
 */

#ifndef INC_SYSTEMCONTROLLER_H_
#define INC_SYSTEMCONTROLLER_H_



#include "Stepper.h"
#include "DCMotor.h"
#include "VacuumPump.h"
class SystemController
{
public:
    SystemController(Stepper* stepper, DCMotor* motor,VacuumPump* vacuumpump);
    uint32_t vacuum_presure;
    uint32_t smire_presure;
    uint32_t vacuum_time;
    uint32_t smire_time;

    void Init();
    void Update(); // call in while(1)

    void Start();
    void Pause();
    void Resume();
    void Reset();
    enum State
    {
        SYS_INIT_1,
        SYS_INIT_2,
        SYS_INIT_3,
		SYS_INIT_4,
        SYS_READY,

		SYS_RUN_1,
        SYS_RUN_2,
        SYS_RUN_3,
        SYS_RUN_4,
        SYS_RUN_5,
        SYS_RUN_6,
		SYS_RUN_7,
		SYS_RUN_8,
		SYS_RUN_9,
		SYS_RUN_10,
        SYS_DONE,

        SYS_PAUSE,
        SYS_RESET_1,
        SYS_RESET_2,
        SYS_RESET_3,
		SYS_POFF,
    } state;

    State prevState;

private:
    Stepper* stepper;
    DCMotor* motor;
    VacuumPump* vacuumpump;


    bool paused;
};

#endif /* INC_SYSTEMCONTROLLER_H_ */
//SystemController system(&stepper, &motor);
//
//int main(void)
//{
//    HAL_Init();
//
//    stepper.Init(16, 200);
//    stepper.SetAcceleration(500, 500);
//
//    motor.Init();
//
//    system.Init();
//
//    while (1)
//    {
//        system.Update();
//
//        // BUTTON HANDLING (example)
//        if (START_BTN == PRESSED)
//            system.Start();
//
//        if (PAUSE_BTN == PRESSED)
//            system.Pause();
//
//        if (RESUME_BTN == PRESSED)
//            system.Resume();
//
//        if (RESET_BTN == PRESSED)
//            system.Reset();
//    }
//}
