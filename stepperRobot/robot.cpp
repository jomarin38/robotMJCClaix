#include "robot.h"
#include "arduinoStepperControler.h"

void Robot::init() {
	StepperControler::init();
}

void Robot::move(int16_t speed, int16_t duration) {
	
	if (smooth) {
		moveSmooth(speed, duration, accdur);
	}
	else {
		moveAgressive(speed, duration*1000);
	}
	
}

void Robot::moveAgressive(int16_t speed, int16_t duration) {
	
	StepperControler::setMotorSpeed(0, speed);
	StepperControler::setMotorSpeed(1, speed);
	delay(duration);
	
}

void Robot::turn(int16_t speed, int16_t duration) {
	
	StepperControler::setMotorSpeed(0, speed);
	StepperControler::setMotorSpeed(1, -speed);
	delay(duration*1000);
	
}

void Robot::stop(int16_t duration) {
	
	int16_t speed = 0;
	
	StepperControler::setMotorSpeed(0, speed);
	StepperControler::setMotorSpeed(1, speed);
	delay(duration*1000);
	
}

void Robot::moveSmooth(int16_t speed, int16_t duration, int16_t accdur) {
	
	uint16_t dur_millis = duration * 1000;
	
	if (dur_millis>2*accdur) {
		uint16_t tempo=100;
		
		for (uint16_t k = 0; k < accdur/tempo; k++)
		{
			moveAgressive((float)k/(float)accdur*tempo*speed, tempo);
		}
		
		moveAgressive(speed,dur_millis-2*accdur);
		
		for (uint16_t k = accdur/tempo; k > 0; k--)
		{
			moveAgressive((float)k/(float)accdur*tempo*speed, tempo);
		}
	}
	else {
		moveAgressive(speed, duration*1000);
	}
	
}

void Robot::setMovingSpeeds(float speedNeed, float turnNeed)
{
	StepperControler::setMotorSpeed(0, speedNeed + turnNeed/2.0);
	StepperControler::setMotorSpeed(1, speedNeed - turnNeed/2.0);
}
