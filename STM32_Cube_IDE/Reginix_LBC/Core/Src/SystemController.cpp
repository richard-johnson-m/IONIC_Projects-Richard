/*
 * SystemController.cpp
 *
 *  Created on: 20-Mar-2026
 *      Author: tamil
 */

#include "SystemController.h"
#include "main.h"

SystemController::SystemController(Stepper *stepper, DCMotor *motor,
		VacuumPump *vacuumpump) {
	this->stepper = stepper;
	this->motor = motor;
	this->vacuumpump = vacuumpump;
	paused = false;
}

void SystemController::Init() {
	state = SYS_INIT_1;
}

void SystemController::Start() {
	if (state == SYS_READY || state == SYS_DONE)

		state = SYS_RUN_1;
}

void SystemController::Pause() {
	if (!paused) {
		prevState = state;
		state = SYS_PAUSE;
		stepper->pass();
		motor->pass();
		vacuumpump->pass();
		paused = true;
	}
}

void SystemController::Resume() {
	if (paused) {

		state = prevState;
		stepper->resume();
		motor->resume();
		vacuumpump->resume();
		paused = false;
	}
}

void SystemController::Reset() {
	state = SYS_RESET_1;
}

void SystemController::Update() {
	stepper->HandleExtio();
	motor->HandleExtio();
	switch (state) {

	case SYS_INIT_1:
		stepper->MoveEnd(100);
		vacuumpump->Stop();
		state = SYS_INIT_2;
		break;

	case SYS_INIT_2:
		if (stepper->IsDone()) {
			motor->MoveHome(800);
			state = SYS_INIT_3;
		}
		break;

	case SYS_INIT_3:
		if (!motor->IsRunning()) {
			stepper->MoveHome(100);
			state = SYS_INIT_4;
		}
		break;
	case SYS_INIT_4:
		if (stepper->IsDone()) {
			state = SYS_READY;
			stepper->Stop();
			motor->Stop();
			vacuumpump->Stop();
		}
		break;

	case SYS_READY:
		break;

		// ---------- RUN ----------
	case SYS_RUN_1:
		vacuumpump->StartTimed(vacuum_presure, vacuum_time);  // 2 sec vacuum
		state = SYS_RUN_2;


	case SYS_RUN_2:

		if (vacuumpump->IsRunning()) {
			break;
		}

		static uint32_t startTime = 0;
		static bool started = false;

		if (!started) {
			stepper->MoveEnd(20);
			startTime = HAL_GetTick();
			started = true;
		}


		if ((HAL_GetTick() - startTime >= 4000)) {
			stepper->MoveEnd(100);
			state = SYS_RUN_3;
			startTime = 0;
			started = 0;
		}

	case SYS_RUN_3:
//		if (stepper->IsDone()) {
//			motor->MoveEnd(500);
//			state = SYS_RUN_4;
//		}

		static uint32_t motorStartTime = 0;
		static bool motorStarted = false;
		if (stepper->IsDone())
		{
		 if (!motorStarted)
		    {
		        motor->MoveEnd(800);
		        motorStartTime = HAL_GetTick();
		        motorStarted = true;
		    }


		    if ((HAL_GetTick() - motorStartTime) >= 2000)
		    {
		        motor->MoveEnd(400);
		        state = SYS_RUN_4;
		        motorStarted=false;
		        motorStartTime=0;

		    }
		}
		break;

	case SYS_RUN_4:

		if (!started) {
			if (!motor->IsRunning()) {
				stepper->MoveForward(smire_presure, 50);
				started = true;
			}
			startTime = HAL_GetTick();
		}


		if ((HAL_GetTick() - startTime >= smire_time)) {

			state = SYS_RUN_5;
			startTime = 0;
			started = false;
		}
		break;

	case SYS_RUN_5:
		if (stepper->IsDone()) {
			stepper->MoveEnd(100);
			state = SYS_RUN_6;
		}
		break;

	case SYS_RUN_6:
		if (stepper->IsDone()) {
			motor->MoveHome(800);
			state = SYS_RUN_7;
		}
		break;

	case SYS_RUN_7:
		if (!motor->IsRunning()) {
			stepper->MoveHome(100);
			state = SYS_RUN_8;
		}
		break;
	case SYS_RUN_8:
		if (stepper->IsDone()) {
			state = SYS_DONE;
		}
		break;
	case SYS_DONE:
		break;

		// ---------- PAUSE ----------
	case SYS_PAUSE:
		break;

		// ---------- RESET ----------
	case SYS_RESET_1:
		stepper->MoveEnd(100);
		state = SYS_RESET_2;
		break;

	case SYS_RESET_2:
		if (stepper->IsDone()) {
			motor->MoveEnd(800);
			state = SYS_RESET_3;
		}
		break;

	case SYS_RESET_3:
		if (!motor->IsRunning()) {
			state = SYS_POFF;
		}
		break;
	}

}
