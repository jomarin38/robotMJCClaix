//  le programme du robot principal
// 06/11/2024 /sjr


#include "RCControl.h"
#include "pinsout.h"

//#define CALIBRATION
RCControl control;


float vitesse_M1 = 0;
float vitesse_M2 = 0;
float vitesse_M3 = 0;
float vitesse_M4 = 0;
//float seuil_bas = 25;
float vitesse = 0;
float direction = 0;
float direction_lateral = 0;
float sw5 = 0;
float sw6 = 0;
float sw7 = 0;
float sw8 = 0;


// INITIALIZATION
void setup()
{

  Serial.begin(115200); // Serial output to console
  delay(200);
  //Serial.println("Programme Stepper version du 17/03/2023");

  control.init();
  // Configuration de toutes les pins de l'Arduino en "sortie" (car elles attaquent les entrées des modules L298N)
  pinMode(borneAvENA, OUTPUT);
  pinMode(borneAvIN1, OUTPUT);
  pinMode(borneAvIN2, OUTPUT);

  pinMode(borneAvIN3, OUTPUT);
  pinMode(borneAvIN4, OUTPUT);
  pinMode(borneAvENB, OUTPUT);
  
  pinMode(borneArENA, OUTPUT);
  pinMode(borneArIN1, OUTPUT);
  pinMode(borneArIN2, OUTPUT);
  //pins moteurs 4
//  pinMode(borneArIN3, OUTPUT);
//  pinMode(borneArIN4, OUTPUT);
//  pinMode(borneArENB, OUTPUT);

  analogWrite(borneAvENA, 0);
  analogWrite(borneAvENB, 0);

  analogWrite(borneArENA, 0);
  //pwm moteur 4
// analogWrite(borneArENB, 0);



//GPIO vers pinces 
  pinMode(OUT_CHANNEL_0_1, OUTPUT);
  digitalWrite(OUT_CHANNEL_0_1, 0);

  pinMode(OUT_CHANNEL_0_2, OUTPUT);
  digitalWrite(OUT_CHANNEL_0_2, 0);

  pinMode(OUT_CHANNEL_1, OUTPUT);
  digitalWrite(OUT_CHANNEL_1, 0);

  pinMode(OUT_CHANNEL_2, OUTPUT);
  digitalWrite(OUT_CHANNEL_2, 0);

  // pinMode(OUT_CHANNEL_3, OUTPUT);
  // analogWrite(OUT_CHANNEL_3, 0);


//  if (digitalRead(rcCalibrationPin) == HIGH) {
  #ifdef CALIBRATION
  
    Serial.println("Beginning calibration process ...");
    delay(20);
    Serial.println("Storing zero values ...");
    control.calibrateZero();
    delay(20);
    Serial.println("Move sticks to maximum values ...");
    delay(20);
    control.calibrateInputs();
    delay(20);
    control.debugCalibration();
    delay(20);
    Serial.println("Calibration process finished.");

    control.writeToEEPROM();
    Serial.println("EEPROM backup finished.");
		delay(20000);
#else
    //Serial.println("reading the calibration settings.");
    control.initializeFromEEPROM();
    control.debugCalibration();
    delay(1000);
#endif

  delay(1000);
}

void conversion_3M_holonome(float in_vitesse, float in_direction, float in_slide, float *out_vitesse_M1, float *out_vitesse_M2, float *out_vitesse_M3)
{
  
  *out_vitesse_M1 = -0.33 * in_slide + 0.58 * in_vitesse + 0.33 * in_direction;
  *out_vitesse_M2 = -0.33 * in_slide - 0.58 * in_vitesse + 0.33 * in_direction;
  *out_vitesse_M3 =  0.67 * in_slide                     + 0.33 * in_direction;

  *out_vitesse_M1 = *out_vitesse_M1 * 255.0/58.0;
  *out_vitesse_M2 = *out_vitesse_M2 * 255.0/58.0;
  *out_vitesse_M3 = *out_vitesse_M3 * 255.0/58.0;
  
  float max_val = max(max(abs(*out_vitesse_M1), abs(*out_vitesse_M2)), abs(*out_vitesse_M3));
  float coeff = 1;
  if (max_val>255) {
    coeff = 255.0/max_val;
  }
  //normalisation
  *out_vitesse_M1 = *out_vitesse_M1 * coeff;
  *out_vitesse_M2 = *out_vitesse_M2 * coeff;
  *out_vitesse_M3 = *out_vitesse_M3 * coeff;
  
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

  // if(throttle_M4 > 0)
  // {
  //   digitalWrite(borneArIN3, HIGH);                 // L'entrée IN1 doit être au niveau haut
  //   digitalWrite(borneArIN4, LOW);                  // L'entrée IN2 doit être au niveau bas

  // } else {
  //   digitalWrite(borneArIN3, LOW);                 // L'entrée IN1 doit être au niveau haut
  //   digitalWrite(borneArIN4, HIGH);                  // L'entrée IN2 doit être au niveau bas
  // }

  analogWrite(borneAvENA, abs(throttle_M1));
  analogWrite(borneAvENB, abs(throttle_M2));
  analogWrite(borneArENA, abs(throttle_M3));
//  analogWrite(borneArENB, abs(throttle_M4));
}


void loop()
{
  //lecture commandes
  vitesse           = control.getThrottle();
  direction         = control.getSteering();
  direction_lateral = control.getLatSteering();
  sw5 = control.getSW5();
  sw6 = control.getSW6();
  sw7 = control.getSW7();
  sw8 = control.getSW8();

  conversion_3M_holonome(vitesse, -direction, direction_lateral, &vitesse_M1, &vitesse_M2, &vitesse_M3 );
  vitesse_M4 = 0;
  commande(vitesse_M1, vitesse_M2, vitesse_M3, vitesse_M4);


  
  if(sw5 == -100){
      digitalWrite(OUT_CHANNEL_0_1, LOW);
      digitalWrite(OUT_CHANNEL_0_2, LOW);
  } else if (sw5 == 0) {
      digitalWrite(OUT_CHANNEL_0_1, HIGH);
      digitalWrite(OUT_CHANNEL_0_2, LOW);
  } 
  else {
      digitalWrite(OUT_CHANNEL_0_1, HIGH);
      digitalWrite(OUT_CHANNEL_0_2, HIGH);
  }

  if(sw6 == -100){
      digitalWrite(OUT_CHANNEL_1, LOW);
  }
  else {
      digitalWrite(OUT_CHANNEL_1, HIGH);
  }

  if(sw7 == -100){
      digitalWrite(OUT_CHANNEL_2, LOW);
  }
  else {
      digitalWrite(OUT_CHANNEL_2, HIGH);
  }
  if(sw8 == -100){
      digitalWrite(OUT_CHANNEL_3_1, LOW);
      digitalWrite(OUT_CHANNEL_3_2, LOW);
  } else if (sw8 == 0){
      digitalWrite(OUT_CHANNEL_3_1, LOW);
      digitalWrite(OUT_CHANNEL_3_2, HIGH);
  }
  else {
      digitalWrite(OUT_CHANNEL_3_1, HIGH);
      digitalWrite(OUT_CHANNEL_3_2, HIGH);
  }

  control.debugInterrupt();
  Serial.println(sw8);
 // Serial.print(sw5); Serial.print("\t\t| ");Serial.print(sw6);Serial.print("\t\t| "); Serial.print(sw7);Serial.print("\t\t| "); Serial.println(sw8);

  control.debugInterrupt();
  //debogue les valeurs envoye aux moteurs
  // Serial.print(",vitesse_M1: \t| "); Serial.print("vitesse_M2: \t| "); Serial.println("vitesse_M3: \t|"); 
  // Serial.print(vitesse_M1); Serial.print("\t\t| ");Serial.print(vitesse_M2);Serial.print("\t\t| "); Serial.println(vitesse_M3);
 delay(200);

}
