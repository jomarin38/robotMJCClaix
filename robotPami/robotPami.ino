#include "robot_fr.h"
#include "PinChangeInt.h" // for RC reciver
//#include <Arduino.h>

RobotFR r;

int detections = 0;

// INITIALIZATION
int pinDetecteurGauche;
int pinDetecteurDroite;

volatile  bool valDetecteurDroite;
volatile  bool valDetecteurGauche;

void appelGaucheEventDown(){
//  Serial.println("Left event down");
  valDetecteurGauche=LOW;
}
void appelGaucheEventUp(){
//  Serial.println("Left event up");
  valDetecteurGauche=HIGH;
}

void appelDroiteEventDown(){
//  Serial.println("Right event down");
  valDetecteurDroite=LOW;
}

void appelDroiteEventUp(){
//  Serial.println("Right event up");
  valDetecteurDroite=HIGH;
}
void setup()
{
  pinDetecteurGauche=14;
  pinDetecteurDroite=15;

  pinMode(pinDetecteurGauche,INPUT);
  pinMode( pinDetecteurDroite,INPUT);


  Serial.begin(115200); // Serial output to console
  delay(200);
  Serial.println("Programme Robot Autonome en Francais");
  
  // Initialisation des moteurs
  Serial.println("Initialisation des moteurs..");
  r.initialiser_robot();
  // r.initialiser_capteurs();
  
  Serial.println("Debut du programme :");
  PCintPort::attachInterrupt((pinDetecteurGauche), appelGaucheEventUp,RISING);
  PCintPort::attachInterrupt((pinDetecteurDroite), appelDroiteEventDown,FALLING);
  PCintPort::attachInterrupt((pinDetecteurGauche), appelGaucheEventUp,RISING);
  PCintPort::attachInterrupt((pinDetecteurDroite), appelDroiteEventDown,CHANGE);
}

void loop()
{
  //code à exécuter en boucle

  delay(10);

//  Serial.print("detection:");Serial.print(detections); 
//  Serial.print("| valGauche:");Serial.print(valDetecteurGauche);
//  Serial.print("| valDroite:");Serial.println(valDetecteurDroite);

  if ((valDetecteurGauche == valDetecteurDroite)) {
    r.bouger(-40, 0);
    Serial.println("avance");
  }
  else { 
    if (((valDetecteurGauche==HIGH)&&(valDetecteurDroite==LOW))){
      Serial.print("Gauche");
      r.bouger(-30, 50);
      detections = detections + 1;
    }
    if ((valDetecteurGauche==LOW)&&(valDetecteurDroite==HIGH)) {
      Serial.print("droite");
      r.bouger(-30, -50);
      detections = detections + 1;
    }
  }  
}
//**********************************************************************
//void loop()
//{
//  bool valDetecteurDroite;
//  bool valDetecteurGauche;
//  //code à exécuter en boucle
//
//  delay(10);
//
//
//  valDetecteurGauche=digitalRead(pinDetecteurGauche);
//  valDetecteurDroite=digitalRead(pinDetecteurDroite);
//  //int distance = r.mesurer_distance();
//
////  Serial.print("detection:");Serial.print(detections); 
////  Serial.print("| valGauche:");Serial.print(valDetecteurGauche);
////  Serial.print("| valDroite:");Serial.println(valDetecteurDroite);
//
//  if ((valDetecteurGauche == valDetecteurDroite)) {
//    r.avancer(20,5);
//    Serial.println("avance");
//  }
//  else { 
//    if (((valDetecteurGauche==HIGH)&&(valDetecteurDroite==LOW))){
//      Serial.print("Gauche");
//      r.tourner_gauche(20,5);
//      detections = detections + 1;
//    }
//    if ((valDetecteurGauche==LOW)&&(valDetecteurDroite==HIGH)) {
//      Serial.print("droite");
//      r.tourner_droite(20,5);
//      detections = detections + 1;
//    }
//  }
//}
