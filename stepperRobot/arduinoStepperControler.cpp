#include "arduinoStepperControler.h"

#define REVERSEM1

//Variables used by ISR

volatile bool motorMoving[2] = {false, false};
volatile long timer_period[2];
volatile bool targetSpeedSatisfied[2] = {false, false};
volatile int16_t targetMotorSpeed[2] = {0, 0};    // Target speed of motors

//End of variables used by ISR

int8_t StepperControler::motorEnable;
int8_t StepperControler::microstepping;
int8_t StepperControler::zeroSpeed;
int8_t StepperControler::maxAccel;
int8_t StepperControler::maxAccellSafe;
int8_t StepperControler::pinMotorDir[2];
int8_t StepperControler::pinMotorStep[2];
float StepperControler::max_accel;
int16_t StepperControler::motorSpeed[2];


// 16 single cycle instructions = 1us at 16Mhz
void delay_1us()
{
	__asm__ __volatile__ (
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop" "\n\t"
			"nop");
}
 
 // TIMER 1 : STEPPER MOTOR1 SPEED CONTROL
ISR(TIMER1_COMPA_vect)
{
	if (motorMoving[0]) // If we are not moving we dont generate a pulse
	{
		digitalWrite (StepperControler::getPinMotorStep(0),HIGH);
		delay_1us();
		digitalWrite (StepperControler::getPinMotorStep(0),LOW);
	}
	OCR1A += timer_period[0];
	if (!targetSpeedSatisfied[0]) {
		StepperControler::setMotorSpeed(0,targetMotorSpeed[0]);
	}
}
// TIMER 3 : STEPPER MOTOR2 SPEED CONTROL
ISR(TIMER1_COMPB_vect)
{
	if (motorMoving[1]) // If we are not moving we dont generate a pulse
	{ 
		digitalWrite (StepperControler::getPinMotorStep(1), HIGH);
		delay_1us();
		digitalWrite (StepperControler::getPinMotorStep(1), LOW);
	}
	OCR1B += timer_period[1];
	if (!targetSpeedSatisfied[1]) {
		StepperControler::setMotorSpeed(1,targetMotorSpeed[1]);
	}
}

ISR(EXTERNAL_INT0_vect)
{

}

StepperControler::StepperControler()
{
	pinMotorDir[0] = 13;
	pinMotorDir[1] = 6;
	pinMotorStep[0] = 12; // motor 1
	pinMotorStep[1] = 3; // motor 2
	motorSpeed[0] = 0;
	motorSpeed[1] = 0;
	
	max_accel = MAX_ACCEL;
	microstepping = MICROSTEPPING;
	
}

void StepperControler::init()
{
	pinMotorDir[0] = 13;
	pinMotorDir[1] = 6;
	pinMotorStep[0] = 12; // motor 1
	pinMotorStep[1] = 3; // motor 2
	motorSpeed[0] = 0;
	motorSpeed[1] = 0;
	
	max_accel = MAX_ACCEL;
	microstepping = MICROSTEPPING;
	
	pinMode(pinMotorStep[0], OUTPUT); // STEP MOTOR 1 
	pinMode(pinMotorDir[0], OUTPUT); // DIR MOTOR 1  
	pinMode(pinMotorStep[1], OUTPUT); // STEP MOTOR 2 
	pinMode(pinMotorDir[1], OUTPUT); // DIR MOTOR 2  

	pinMode(MOTORENABLE, OUTPUT);  // motor enable pin 
	digitalWrite(MOTORENABLE, HIGH);  // disable motors
	
	// MOTOR1 => TIMER1A
	TCCR1A = 0;                       // Timer1 CTC mode 4, OCxA,B outputs disconnected
	// TCCR1B = (1 << WGM12) | (1 << CS11); // Prescaler=8, => 2Mhz
	TCCR1B = (1 << CS11); // normal mode + Prescaler=8, => 2Mhz
	OCR1A = 0;
	motorMoving[0] = false; // motor stopped

	// obo -> TiMER 1B
	OCR1B = 0;
	motorMoving[1] = false; // motor stopped
	TCNT1 = 0;

	// Enable stepper drivers and TIMER interrupts
	digitalWrite(MOTORENABLE, LOW);   // Enable stepper drivers

	// Enable TIMERs interrupts
	TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B); // Enable Timer1 interrupt
	
}

void StepperControler::setMotorSpeed (int i, int16_t tspeed)   // motor = 1 or 2, speed
{
	int16_t speed;
	targetMotorSpeed[i]=tspeed;
#ifdef REVERSEM0
	if (i == 0) tspeed = - tspeed;
#endif
#ifdef REVERSEM1
	if (i == 1) tspeed = - tspeed;
#endif
	// WE LIMIT MAX ACCELERATION of the motors
	if ((motorSpeed[i] - tspeed) > max_accel)
	{
		motorSpeed[i] -= max_accel;
		targetSpeedSatisfied[i] = false;
	}
	else if ((motorSpeed[i] - tspeed) < -max_accel) 
	{
		motorSpeed[i] += max_accel;
		targetSpeedSatisfied[i] = false;
	}	
	else
	{
		motorSpeed[i] = tspeed;
		targetSpeedSatisfied[i] = true;
	}

#ifdef DEBUG		
	Serial.print("MotorSpeed[");Serial.print(i);Serial.print("]=");Serial.println(motorSpeed[i]);
#endif

#if MICROSTEPPING==16
	speed = motorSpeed[i] * 46; // Adjust factor from control output speed to real motor speed in steps/second
#else
	speed = motorSpeed[i] * 23; // 1/8 Microstepping
#endif
	if (speed == 0)
	{
		timer_period[i] = ZERO_SPEED;
		motorMoving[i] = false;
	}
	else if (speed > 0)
	{
		timer_period[i] = 2000000 / speed; // 2Mhz timer
		motorMoving[i] = true;
		digitalWrite (pinMotorDir[i], LOW);
	}
	else
	{
		timer_period[i] = 2000000 / -speed;
		motorMoving[i] = true;
		digitalWrite (pinMotorDir[i], HIGH);
	}
	if (timer_period[i] > ZERO_SPEED)   // Check for minimun speed (maximun period without overflow)
		timer_period[i] = ZERO_SPEED;

	if (i == 0)
		OCR1A += timer_period[0];
	else
		OCR1B += timer_period[1];

}
