#include "robot_fr.h"
#include "arduinoStepperControler.h"


		void tourner_gauche(int16_t vitesse, int16_t duree);
    void tourner_droite(int16_t vitesse, int16_t duree);
    
		void attendre(int16_t duree);

		void bouger(float vitesse, float rotation);


void RobotFR::initialiser_robot() {
	Robot::init();
}

void RobotFR::initialiser_capteurs() {
	pinMode(US_TRIG_PIN, OUTPUT);
  pinMode(US_ECHO_PIN, INPUT);
}

void RobotFR::avancer(int16_t vitesse, int16_t duree) {
	
  Robot::move(vitesse, duree);
	
}

void RobotFR::reculer(int16_t vitesse, int16_t duree) {
	
  Robot::move(-vitesse, duree);
	
}

void RobotFR::tourner_gauche(int16_t vitesse, int16_t duree) {
	
  Robot::turn(vitesse, duree);
	
}

void RobotFR::tourner_droite(int16_t vitesse, int16_t duree) {
	
  Robot::turn(-vitesse, duree);
	
}

void RobotFR::attendre(int16_t duree) {
	
  Robot::stop(duree);
	
}

void RobotFR::bouger(float vitesse, float rotation) {
	
  Robot::setMovingSpeeds(vitesse, rotation);
	
}

int RobotFR::mesurer_distance() {
  digitalWrite(US_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIG_PIN, LOW);
  const unsigned long duration= pulseIn(US_ECHO_PIN, HIGH);
  int distance= duration/29/2;
  return distance;
}
