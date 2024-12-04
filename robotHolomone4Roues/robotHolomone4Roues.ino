//  le programme du robot principal
// 06/11/2024 /sjr


//#define DEBUG
#include "RCControl.h"
//#include "TM1637.h"
#define CMD_MIN_MAX_FILTRE 10 //valeur pour filtrer autour du zero pour av/ar et G/D

//*******************************************************************************//
// Association des entrées du L298N, aux sorties utilisées sur notre Arduino Uno //
//*******************************************************************************//
// moteur avant
#define borneAvENA        11      // On associe la borne "ENA" du L298N à la pin D10 de l'arduino
#define borneAvIN1        12      // On associe la borne "IN1" du L298N à la pin D9 de l'arduino
#define borneAvIN2        10      // On associe la borne "IN2" du L298N à la pin D8 de l'arduino
#define borneAvIN3        7       // On associe la borne "IN3" du L298N à la pin D7 de l'arduino
#define borneAvIN4        8       // On associe la borne "IN4" du L298N à la pin D6 de l'arduino
#define borneAvENB        9       // On associe la borne "ENB" du L298N à la pin D5 de l'arduino

// moteur arriere
#define borneArENA        5       // On associe la borne "ENA" du L298N à la pin D10 de l'arduino
#define borneArIN1        18      // On associe la borne "IN1" du L298N à la pin D9 de l'arduino
#define borneArIN2        17      // On associe la borne "IN2" du L298N à la pin D8 de l'arduino
#define borneArIN3        16      // On associe la borne "IN3" du L298N à la pin D7 de l'arduino
#define borneArIN4        15      // On associe la borne "IN4" du L298N à la pin D6 de l'arduino
#define borneArENB        3      // On associe la borne "ENB" du L298N à la pin D5 de l'arduino

RCControl control;
//Robot r;


float vitesse = 0;
float vitesse_M1 = 0;
float vitesse_M2 = 0;
float vitesse_M3 = 0;
float vitesse_M4 = 0;
float seuil_bas = 25;
float direction = 0;
float direction_lateral = 0;


//int rcCalibrationPin = 2;  //  X sur CNC, à boucler au 5V
//int rcKeyPin = 5;  //X_dir sur CNC, bouclé à la masse

// INITIALIZATION
void setup()
{
  //pinMode(rcCalibrationPin, INPUT_PULLUP);
  //pinMode(rcKeyPin, INPUT_PULLUP);

  Serial.begin(115200); // Serial output to console
  delay(200);
  //Serial.println("Programme Stepper version du 17/03/2023");

  //Serial.println("Départ");
  // STEPPER MOTORS INITIALIZATION
  //Serial.println("Steper motors initialization...");
//  r.init();
//  r.setSmooth(false);
  control.init();
  // Configuration de toutes les pins de l'Arduino en "sortie" (car elles attaquent les entrées du module L298N)
  pinMode(borneAvENA, OUTPUT);
  pinMode(borneAvIN1, OUTPUT);
  pinMode(borneAvIN2, OUTPUT);
  pinMode(borneAvIN3, OUTPUT);
  pinMode(borneAvIN4, OUTPUT);
  pinMode(borneAvENB, OUTPUT);
  
  pinMode(borneArENA, OUTPUT);
  pinMode(borneArIN1, OUTPUT);
  pinMode(borneArIN2, OUTPUT);
  pinMode(borneArIN3, OUTPUT);
  pinMode(borneArIN4, OUTPUT);
  pinMode(borneArENB, OUTPUT);

  analogWrite(borneAvENA, 0);
  analogWrite(borneAvENB, 0);

  analogWrite(borneArENA, 0);
  analogWrite(borneArENB, 0);

//  if (digitalRead(rcCalibrationPin) == HIGH) {
  if (0) {
  
    Serial.println("Beginning calibration process ...");
    Serial.println("Storing zero values ...");
    control.calibrateZero();

//    r.move(10, 1);
//    r.stop(1);

    Serial.println("Computing maximum values ...");
    Serial.println("Move sticks to maximum values ...");
    control.calibrateInputs();

    Serial.println("Calibration process finished.");

    control.writeToEEPROM();


//    r.move(-10, 1);
  }
  else {
    //Serial.println("reading the calibration settings.");
    control.initializeFromEEPROM();
  }
//  r.stop(1);
  delay(1000);
}
/*
void conversion(float in_vitesse, float in_direction, float *out_vitesse_M1, float *out_vitesse_M2)
{
  
  float turnProportion = 4.0;
  if (abs(in_vitesse) < 50.0) {
    turnProportion = 1.0;
  }
  float maxValue = 1.0 + 1.0/turnProportion;

  *out_vitesse_M1 = (in_vitesse + in_direction / turnProportion) / maxValue / 2.0 / 100 * 255 ;
  *out_vitesse_M2 = (in_vitesse - in_direction / turnProportion) / maxValue / 2.0 / 100 * 255;

  *out_vitesse_M1 = (255 / 100 ) * *out_vitesse_M1;
  *out_vitesse_M2 = (255 / 100 ) * *out_vitesse_M2;
  
}
*/
void conversion_4M(float in_vitesse, float in_direction, float *out_vitesse_M1, float *out_vitesse_M2, float *out_vitesse_M3, float *out_vitesse_M4)
{
  
  float turnProportion = 4.0;
  if (abs(in_vitesse) < 50.0) {
    turnProportion = 1.0;
  }
  float maxValue = 1.0 + 1.0/turnProportion;

  *out_vitesse_M1 = (in_vitesse + in_direction / turnProportion) / maxValue / 2.0 / 100 * 255 ;
  *out_vitesse_M2 = (in_vitesse - in_direction / turnProportion) / maxValue / 2.0 / 100 * 255;

  *out_vitesse_M1 = (255 / 100 ) * *out_vitesse_M1 * 60 / 100;
  *out_vitesse_M2 = (255 / 100 ) * *out_vitesse_M2 * 60 / 100;
  *out_vitesse_M3 = *out_vitesse_M1;
  *out_vitesse_M4 = *out_vitesse_M2;
  
}

void conversion_4M_normalise(float in_vitesse, float in_direction, float *out_vitesse_M1, float *out_vitesse_M2, float *out_vitesse_M3, float *out_vitesse_M4)
{
  
  float turnProportion = 1.5;
  float pourcentage_puissance = 0.4;
  float min = 0;
  
  if(-3.0 < in_vitesse && in_vitesse < 3.0) in_vitesse = 0.0;
  if(-3.0 < in_direction && in_direction < 3.0) in_direction = 0.0;
  if(in_vitesse == 0 && in_direction == 0) min = 0;

  *out_vitesse_M1 = (in_vitesse + in_direction / turnProportion);
  *out_vitesse_M2 = (in_vitesse - in_direction / turnProportion);
  Serial.print("A: out_vitesse_M1:"); 
  Serial.print(*out_vitesse_M1);
//  Serial.print(",");
  Serial.print(",out_vitesse_M2:"); 
  Serial.print(*out_vitesse_M2);

  if(abs(*out_vitesse_M1) > 100 || abs(*out_vitesse_M2) > 100)
  {
    float max = max(abs(*out_vitesse_M1), abs(*out_vitesse_M2));
    *out_vitesse_M1 = *out_vitesse_M1 / max * 100;
    *out_vitesse_M2 = *out_vitesse_M2 / max * 100;
    Serial.print(" A: MAX:"); 
    Serial.print(max);  
  }
  Serial.print(" B: out_vitesse_M1:"); 
  Serial.print(*out_vitesse_M1);
//  Serial.print(",");
  Serial.print(",out_vitesse_M2:"); 
  Serial.print(*out_vitesse_M2);

  *out_vitesse_M1 = (255.0 / 100.0 ) * *out_vitesse_M1 * pourcentage_puissance;
  *out_vitesse_M2 = (255.0 / 100.0 ) * *out_vitesse_M2 * pourcentage_puissance;
  //float temp_M1 = (((255.0  - min) / 100.0 ) * abs(*out_vitesse_M1) + min )* pourcentage_puissance;
  //float temp_M2 = (((255.0 - min) / 100.0 ) * abs(*out_vitesse_M2) + min ) * pourcentage_puissance;
  
  //if(*out_vitesse_M1 >= 0) *out_vitesse_M1 = temp_M1;
  //else  *out_vitesse_M1 = -temp_M1;
  //if(*out_vitesse_M2 >= 0) *out_vitesse_M2 = temp_M2;
  //else  *out_vitesse_M2 = -temp_M2;
 
  Serial.print(" C: out_vitesse_M1:"); 
  Serial.print(*out_vitesse_M1);
//  Serial.print(",");
  Serial.print(",out_vitesse_M2:"); 
  Serial.print(*out_vitesse_M2);
  Serial.println("");
  *out_vitesse_M3 = *out_vitesse_M1;
  *out_vitesse_M4 = *out_vitesse_M2;
  
}

void conversion_4M_normalise2(float in_vitesse, float in_direction, float direction_lateral, float *out_vitesse_M1, float *out_vitesse_M2, float *out_vitesse_M3, float *out_vitesse_M4)
{
  
  float turnProportion = 2;
  float pourcentage_puissance = 0.6;
  float min = 0;
  
  if(-3.0 < in_vitesse && in_vitesse < 3.0) in_vitesse = 0.0;
  if(-3.0 < in_direction && in_direction < 3.0) in_direction = 0.0;
  if(in_vitesse == 0 && in_direction == 0) min = 0;

  *out_vitesse_M1 = (in_vitesse - in_direction / turnProportion + direction_lateral);
  *out_vitesse_M2 = (in_vitesse + in_direction / turnProportion - direction_lateral);
  *out_vitesse_M3 = (in_vitesse + in_direction / turnProportion + direction_lateral);
  *out_vitesse_M4 = (in_vitesse - in_direction / turnProportion - direction_lateral);
  Serial.print("A: out_vitesse_M1:"); 
  Serial.print(*out_vitesse_M1);
//  Serial.print(",");
  Serial.print(",out_vitesse_M2:"); 
  Serial.print(*out_vitesse_M2);

  if(abs(*out_vitesse_M1) > 100 || abs(*out_vitesse_M2) > 100)
  {
    float max = max(abs(*out_vitesse_M1), abs(*out_vitesse_M2));
    float max2 = max(abs(*out_vitesse_M3), abs(*out_vitesse_M4));
    max = max(abs(max), abs(max2));
    
    *out_vitesse_M1 = *out_vitesse_M1 / max * 100;
    *out_vitesse_M2 = *out_vitesse_M2 / max * 100;
    *out_vitesse_M3 = *out_vitesse_M3 / max * 100;
    *out_vitesse_M4 = *out_vitesse_M4 / max * 100;
    Serial.print(" A: MAX:"); 
    Serial.print(max);  
  }
  Serial.print(" B: out_vitesse_M1:"); 
  Serial.print(*out_vitesse_M1);
//  Serial.print(",");
  Serial.print(",out_vitesse_M2:"); 
  Serial.print(*out_vitesse_M2);

  *out_vitesse_M1 = (255.0 / 100.0 ) * *out_vitesse_M1 * pourcentage_puissance;
  *out_vitesse_M2 = (255.0 / 100.0 ) * *out_vitesse_M2 * pourcentage_puissance;
  *out_vitesse_M3 = (255.0 / 100.0 ) * *out_vitesse_M3 * pourcentage_puissance;
  *out_vitesse_M4 = (255.0 / 100.0 ) * *out_vitesse_M4 * pourcentage_puissance;
  //float temp_M1 = (((255.0  - min) / 100.0 ) * abs(*out_vitesse_M1) + min )* pourcentage_puissance;
  //float temp_M2 = (((255.0 - min) / 100.0 ) * abs(*out_vitesse_M2) + min ) * pourcentage_puissance;
  
  //if(*out_vitesse_M1 >= 0) *out_vitesse_M1 = temp_M1;
  //else  *out_vitesse_M1 = -temp_M1;
  //if(*out_vitesse_M2 >= 0) *out_vitesse_M2 = temp_M2;
  //else  *out_vitesse_M2 = -temp_M2;
 
  Serial.print(" C: out_vitesse_M1:"); 
  Serial.print(*out_vitesse_M1);
//  Serial.print(",");
  Serial.print(",out_vitesse_M2:"); 
  Serial.print(*out_vitesse_M2);
  Serial.println("");
  //analogWrite(borneAvENA, abs(*out_vitesse_M1));
  //analogWrite(borneAvENB, abs(*out_vitesse_M2));

  //analogWrite(borneArENA, abs(*out_vitesse_M3));
  //analogWrite(borneArENB, abs(*out_vitesse_M4));

}



void commande( float throttle_M1, float throttle_M2, float throttle_M3, float throttle_M4 )
{
  throttle_M1 = throttle_M1;
  throttle_M2 = -throttle_M2;
  throttle_M3 = -throttle_M3;
  throttle_M4 = throttle_M4;
  if(throttle_M1 > 0)
  {
    digitalWrite(borneAvIN1, HIGH);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneAvIN2, LOW);                  // L'entrée IN2 doit être au niveau bas

  } else {
    digitalWrite(borneAvIN1, LOW);                  // L'entrée IN1 doit être au niveau bas
    digitalWrite(borneAvIN2, HIGH);                 // L'entrée IN2 doit être au niveau haut

  }
  if(throttle_M2 > 0)
  {
    digitalWrite(borneAvIN3, HIGH);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneAvIN4, LOW);                  // L'entrée IN2 doit être au niveau bas

  } else {
    digitalWrite(borneAvIN3, LOW);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneAvIN4, HIGH);                  // L'entrée IN2 doit être au niveau bas
  }
  if(throttle_M3 > 0)
  {
    digitalWrite(borneArIN1, HIGH);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneArIN2, LOW);                  // L'entrée IN2 doit être au niveau bas
  } else {
    digitalWrite(borneArIN1, LOW);                  // L'entrée IN1 doit être au niveau bas
    digitalWrite(borneArIN2, HIGH);                 // L'entrée IN2 doit être au niveau haut
  }

  if(throttle_M4 > 0)
  {
    digitalWrite(borneArIN3, HIGH);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneArIN4, LOW);                  // L'entrée IN2 doit être au niveau bas

  } else {
    digitalWrite(borneArIN3, LOW);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneArIN4, HIGH);                  // L'entrée IN2 doit être au niveau bas
  }

  analogWrite(borneAvENA, abs(throttle_M1));
  analogWrite(borneAvENB, abs(throttle_M2));

  analogWrite(borneArENA, abs(throttle_M3));
  analogWrite(borneArENB, abs(throttle_M4));

}


void loop()
{
  //lecture commandes
  vitesse = control.getThrottle();
  direction = control.getSteering();
  direction_lateral = control.getLatSteering();
  //conversion_4M_normalise(vitesse, -direction, &vitesse_M1, &vitesse_M2, &vitesse_M3, &vitesse_M4 );
  conversion_4M_normalise2(vitesse, direction, -direction_lateral, &vitesse_M1, &vitesse_M2, &vitesse_M3, &vitesse_M4 );

 commande(vitesse_M1, vitesse_M2, vitesse_M3, vitesse_M4);



  Serial.print("vitesse:"); 
  Serial.print(vitesse);
//  Serial.print(",");
  Serial.print(",direction:"); 
  Serial.print(direction);
  Serial.print(",direction_lateral:"); 
  Serial.print(direction_lateral);
//  Serial.print(",");
  Serial.print(",vitesse_M1:"); 
  Serial.print(vitesse_M1);
  //Serial.print(",");
  Serial.print(",vitesse_M2:"); 
  Serial.println(vitesse_M2);
  delay(200);

}


