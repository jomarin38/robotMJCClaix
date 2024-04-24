//  le programme du robot principal
// 17/04/2024


//#define DEBUG
// relais en 8, 4, 7 et 12
// relais 1 en 4
#include "robot.h"
#include "RCControl.h"
#include "TM1637.h"
//#define CDE_RELAIS   4 // patte arduino branchement realis realis commande lumière
//#define PWM_RELAIS   7 // patte arduino branchement telcommande voie hG
#define CMD_MIN_MAX_FILTRE 5 //valeur pour filtrer autour du zero pour av/ar et G/D

#define COMMUNO1 4  // patte UNo pour comm vers arduino pince avant
#define COMMUNO2 9    // autres pinces valeur à trouver
#define COMMUNO3 6

#define TEMPS_AVANT_DEGUISEMENT 100000

RCControl control;
Robot r;

//X STEP et Z STEP
#define CLK 9//pins definitions for TM1637 and can be changed to other ports
#define DIO 10
TM1637 tm1637(CLK,DIO);

float throttle;
float steering;
float commande_pince1;
float commande_pince2;
float commande_pince3;

int rcCalibrationPin = 2;  //  X sur CNC, à boucler au 5V
//int rcKeyPin = 3;  //Y sur CNC, bouclé à la masse ATTENTION ça perturbe la fonciton moteur !!!
int rcKeyPin = 5;  //X_dir sur CNC, bouclé à la masse

int score = 42;
uint32_t temps_dernier_score = 0;
#define TEMPS_ENTRE_2_AFFICHAGES_MS 300
// INITIALIZATION
void setup()
{
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;displayInt(42);

 
  

  pinMode(rcCalibrationPin, INPUT_PULLUP);
  pinMode(rcKeyPin, INPUT_PULLUP);
//  pinMode(CDE_RELAIS, OUTPUT); // relais eclairage
//  pinMode(PWM_RELAIS, INPUT);// lecture manette RC pour leds
  
  pinMode(COMMUNO1, OUTPUT); // comm avec arduino pinces
  pinMode(COMMUNO2, OUTPUT); // comm avec arduino pinces
  pinMode(COMMUNO3, OUTPUT); // comm avec arduino pinces

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
//  //Pin Y au zero
//  while (digitalRead(rcKeyPin) == HIGH ) {
//    delay(50);
//  }

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

void displayInt(int value) {

  int reste = 0;
  
  int milliers = value/1000;
  reste = value - milliers*1000;
  int centaine = reste/100;
  reste = reste - centaine*100;
  int dixaine = reste/10;
  int unite = reste - dixaine * 10;

  tm1637.display(0,milliers);
  tm1637.display(1,centaine);
  tm1637.display(2,dixaine);
  tm1637.display(3,unite);
  
}

//**********************************************************************
void loop()
{
  //lecture commandes
  throttle = control.getThrottle();
  steering = control.getSteering();
  commande_pince1 = control.getPince1();
  commande_pince2 = control.getPince2();
  commande_pince3 = control.getPince3();

  if (-CMD_MIN_MAX_FILTRE < throttle && throttle < CMD_MIN_MAX_FILTRE) throttle = 0;
  if (-CMD_MIN_MAX_FILTRE < steering && steering < CMD_MIN_MAX_FILTRE) steering = 0;

  //Serial.print("throttle="); Serial.print(throttle);
  //Serial.print("steering="); Serial.println(steering);
  Serial.print("pince1="); Serial.println(commande_pince1);
  Serial.print("pince2="); Serial.println(commande_pince2);
  Serial.print("pince3="); Serial.println(commande_pince3);

  r.setMovingSpeeds(
    throttle,
    steering
  );
    
  uint32_t chrono_ms = micros() / 1000;

  if (commande_pince1 > 1800) 
  {
     digitalWrite(COMMUNO1, LOW);
  }
  else
      digitalWrite(COMMUNO1, HIGH);
  //---------------------------------    
   if (commande_pince2 > 1800) 
    {
     digitalWrite(COMMUNO2, LOW);
  }
  else
      digitalWrite(COMMUNO2, HIGH);

   if (commande_pince3 > 1800) 
    {
     digitalWrite(COMMUNO3, LOW);
  }
  else
      digitalWrite(COMMUNO3, HIGH);


  delay(50);

}
