//#define DEBUG
// relais en 8, 4, 7 et 12
// relais 1 en 4
#include "robot.h"
#include "RCControl.h"
#define CDE_RELAIS   4 // patte arduino branchement realis realis commande lumière
#define PWM_RELAIS   7 // patte arduino branchement telcommande voie hG
RCControl control;
Robot r;

float throttle;
float steering;
float commande_led;
int rcCalibrationPin = 2;  //  X sur CNC, à boucler au 5V
//int rcKeyPin = 3;  //Y sur CNC, bouclé à la masse ATTENTION ça perturbe la fonciton moteur !!!
int rcKeyPin = 5;  //X_dir sur CNC, bouclé à la masse

// INITIALIZATION
void setup()
{

  //  pinMode(rcCalibrationPin, INPUT_PULLUP);
  pinMode(rcKeyPin, INPUT_PULLUP);
  pinMode(CDE_RELAIS, OUTPUT); // relais eclairage
  pinMode(PWM_RELAIS, INPUT);// lecture manette RC pour leds

  /*  pinMode(4, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(12, OUTPUT);

    digitalWrite(4,HIGH);
    digitalWrite(7,HIGH);
    digitalWrite(8,HIGH);
    digitalWrite(12,HIGH);
  */

  Serial.begin(115200); // Serial output to console
  delay(200);
  Serial.println("Programme Stepper version du 17/03/2023");
  Serial.println("attente arrachement de la clé...\n");
  //Pin Y au zero
  while (digitalRead(rcKeyPin) == HIGH ) {
    delay(50);
  }

  Serial.println("Départ");
  // STEPPER MOTORS INITIALIZATION
  Serial.println("Steper motors initialization...");
  r.init();
  r.setSmooth(false);
  control.init();

  if (digitalRead(rcCalibrationPin) == HIGH) {
    Serial.println("Beginning calibration process ...");
    Serial.println("Storing zero values ...");
    control.calibrateZero();

    r.move(10, 1);
    r.stop(1);

    Serial.println("Computing maximum values ...");
    Serial.println("Move sticks to maximum values ...");
    control.calibrateInputs();

    Serial.println("Calibration process finished.");

    control.writeToEEPROM();

    r.move(-10, 1);
  }
  else {
    Serial.println("reading the calibration settings.");
    control.initializeFromEEPROM();
  }
  r.stop(1);
}
//**********************************************************************
void loop()
{
  //lecture commandes
  throttle = control.getThrottle();
  steering = control.getSteering();
  commande_led = control.getRelais();

  //commandes
  if (commande_led > 1800)digitalWrite(CDE_RELAIS, HIGH);
  if (commande_led < 1100)digitalWrite(CDE_RELAIS, LOW);

  r.setMovingSpeeds(
    throttle,
    steering
  );
  Serial.print("throttle="); Serial.print(throttle);
  Serial.print("steering="); Serial.println(steering);
  Serial.print("commande led="); Serial.println(commande_led);
}
