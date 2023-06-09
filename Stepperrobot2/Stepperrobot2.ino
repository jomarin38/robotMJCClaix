// attention servotimer2 commande de 750 à 2250 !!!!!!
//#define DEBUG
// relais en 8, 4, 7 et 12
// relais 1 en 4
#include "robot.h"
#include "RCControl.h"
#include "TM1637.h"
#include "ServoTimer2.h" 
#define CDE_RELAIS   4 // patte arduino branchement realis realis commande lumière
#define PWM_RELAIS   7 // patte arduino branchement telcommande voie hG
#define CMD_MIN_MAX_FILTRE 5 //valeur pour filtrer autour du zero pour av/ar et G/D

#define TEMPS_AVANT_DEGUISEMENT 20000

RCControl control;
Robot r;

//X STEP et Z STEP
#define CLK 9//pins definitions for TM1637 and can be changed to other ports
#define DIO 10
TM1637 tm1637(CLK,DIO);
//Servo myservo;  // create servo object to control a servo
ServoTimer2 myservo;
float throttle;
float steering;
float commande_score;
int rcCalibrationPin = 2;  //  X sur CNC, à boucler au 5V
//int rcKeyPin = 3;  //Y sur CNC, bouclé à la masse ATTENTION ça perturbe la fonciton moteur !!!
int rcKeyPin = 5;  //X_dir sur CNC, bouclé à la masse
int pos = 0;    // variable to store the servo position
int score = 42;
uint32_t temps_dernier_score = 0;
#define TEMPS_ENTRE_2_AFFICHAGES_MS 300
// INITIALIZATION
void setup()
{
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;displayInt(42);
myservo.attach(9);  // attaches the servo on pin 9 to the servo object
delay(200);
 

 myservo.write(1250);              // 1250 position neutre, 


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
    // in steps of 1 degree

      for (pos = 1250; pos >= 750; pos-=2) { // goes from 0 degrees to 180 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(1);                       // waits 15ms for the servo to reach the position
  }

delay(500) ;
  
  //for (pos = 0; pos <= 88; pos++) { // goes from 180 degrees to 0 degrees
  //  myservo.write(pos);              // tell servo to go to position in variable 'pos'
   // delay(15);                       // waits 15ms for the servo to reach the position
  //}


  
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
  allumerDeguissement(false);

   r.move(10, 13);  // a ajuster pour distance
    r.stop(1);
  for (pos = 0; pos <= 88; pos++) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}
//*****************************************************
void allumerDeguissement(bool allumer){
  if(allumer == true){
    digitalWrite(CDE_RELAIS, HIGH);  }
  else{
    digitalWrite(CDE_RELAIS, LOW);
  }
}
//******************************************************************
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
  
    
  uint32_t chrono_ms = micros() / 1000;

  if(chrono_ms >= TEMPS_AVANT_DEGUISEMENT ){
    allumerDeguissement(true);
  }  




}
