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
  float turnProportion = 2.0;
  if (speedNeed<50.0) {
    turnProportion = 1.0;
  }
  float maxValue = 1.0 + 1.0/turnProportion;
  int m1_speed = 1.5*(speedNeed + turnNeed/turnProportion)/maxValue;
  int m2_speed = 1.5*(speedNeed - turnNeed/turnProportion)/maxValue;
	StepperControler::setMotorSpeed(0, m1_speed);
	StepperControler::setMotorSpeed(1, m2_speed);
  //Serial.print("M1:");Serial.print(m1_speed);Serial.print(",M2:");Serial.println(m2_speed);
}
