#include "robot_fr.h"
#include "PinChangeInt.h" // for RC reciver
//#include <Arduino.h>

RobotFR r;

#define ETAT_INDEFINI 0
#define ETAT_TOURNER_GAUCHE 1
#define ETAT_TOURNER_DROITE 2
#define ETAT_ALLER_TOUT_DROIT 3

unsigned char etat_courant;
unsigned char etat_precedent; 
int detections = 0;

// INITIALIZATION
int pinDetecteurGauche;
int pinDetecteurDroite;

volatile  bool valDetecteurDroite;
volatile  bool valDetecteurGauche;

//void appelGaucheEventDown(){
//  Serial.println("Left event down");
//  valDetecteurGauche=LOW;
//}
//void appelGaucheEventUp(){
//  Serial.println("Left event up");
//  valDetecteurGauche=HIGH;
//}
//
//void appelDroiteEventDown(){
//  Serial.println("Right event down");
//  valDetecteurDroite=LOW;
//}
//
//void appelDroiteEventUp(){
//  Serial.println("Right event up");
//  valDetecteurDroite=HIGH;
//}

void determiner_direction(){
    if ((valDetecteurGauche == valDetecteurDroite)) {
    etat_courant = ETAT_ALLER_TOUT_DROIT;
  }
  else { 
    if (((valDetecteurGauche==HIGH)&&(valDetecteurDroite==LOW))){
      etat_courant = ETAT_TOURNER_GAUCHE;
    }
    if ((valDetecteurGauche==LOW)&&(valDetecteurDroite==HIGH)) {
      etat_courant = ETAT_TOURNER_DROITE;
    }
  }  
//  switch (etat_courant){
//    case(ETAT_TOURNER_GAUCHE):
//      Serial.println("nouvelle direction gauche");
//      break;
//
//    case(ETAT_TOURNER_DROITE):
//      Serial.println("nouvelle direction droite");
//      break;
//    case(ETAT_ALLER_TOUT_DROIT):
//          Serial.println("nouvelle direction tout droit");
//      break;
//    default:
//      Serial.println("nouvelle direction erreur");
//      break;
//  }
}
void appelGaucheEvent(){
  valDetecteurGauche=digitalRead(pinDetecteurGauche);;
//  valDetecteurDroite=digitalRead(pinDetecteurDroite);;
  //determiner_direction();
  //Serial.print("Left event : ");Serial.println(valDetecteurGauche);
}

void appelDroiteEvent(){
//  valDetecteurGauche=digitalRead(pinDetecteurGauche);;
  valDetecteurDroite=digitalRead(pinDetecteurDroite);;
 // determiner_direction();
 // Serial.print("Right event : ");Serial.println(valDetecteurDroite);
}
void setup()
{
  etat_courant = ETAT_ALLER_TOUT_DROIT;
  etat_precedent = ETAT_INDEFINI; 

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

//  PCintPort::attachInterrupt((pinDetecteurGauche), appelGaucheEvent,CHANGE);
//  PCintPort::attachInterrupt((pinDetecteurDroite), appelDroiteEvent,CHANGE);
}

void loop()
{
  //code à exécuter en boucle
  bool etat_changement = false;
//  delay(1);

//  Serial.print("detection:");Serial.print(detections); 
//  Serial.print("State:");Serial.print(etat_courant);
//  Serial.print("| valGauche:");Serial.print(valDetecteurGauche);
//  Serial.print("| valDroite:");Serial.println(valDetecteurDroite);

  valDetecteurGauche=digitalRead(pinDetecteurGauche);
  valDetecteurDroite=digitalRead(pinDetecteurDroite);
  determiner_direction();
  if(etat_courant != etat_precedent)
  {
    etat_precedent = etat_courant;  

    if ((etat_courant == ETAT_ALLER_TOUT_DROIT)) {
      r.bouger(-40, 0);
      Serial.println("avance");
      detections = detections + 1;

    }
    else { 
      if (etat_courant == ETAT_TOURNER_GAUCHE){
        Serial.println("Gauche");
        r.bouger(-30, 50);
        detections = detections + 1;
      }
      if (etat_courant == ETAT_TOURNER_DROITE) {
        Serial.println("droite");
        r.bouger(-30, -50);
        detections = detections + 1;
      }
    }
  }

}



//void loop()
//{
//  //code à exécuter en boucle
//  bool etat_changement = false;
//  delay(1);
//
////  Serial.print("detection:");Serial.print(detections); 
////  Serial.print("State:");Serial.print(etat_courant);
////  Serial.print("| valGauche:");Serial.print(valDetecteurGauche);
////  Serial.print("| valDroite:");Serial.println(valDetecteurDroite);
//
// determiner_direction();
//  if(etat_courant != etat_precedent)
//  {
//    etat_precedent = etat_courant;  
//
//    if ((etat_courant == ETAT_ALLER_TOUT_DROIT)) {
//      r.bouger(-40, 0);
//      Serial.println("avance");
//      detections = detections + 1;
//
//    }
//    else { 
//      if (etat_courant == ETAT_TOURNER_GAUCHE){
//        Serial.println("Gauche");
//        r.bouger(-30, 50);
//        detections = detections + 1;
//      }
//      if (etat_courant == ETAT_TOURNER_DROITE) {
//        Serial.println("droite");
//        r.bouger(-30, -50);
//        detections = detections + 1;
//      }
//    }
//  }
//
//}
//
//void loop()
//{
//  //code à exécuter en boucle
//
//  delay(100);
//
////  Serial.print("detection:");Serial.print(detections); 
////  Serial.print("| valGauche:");Serial.print(valDetecteurGauche);
////  Serial.print("| valDroite:");Serial.println(valDetecteurDroite);
//
//  if ((valDetecteurGauche == valDetecteurDroite)) {
//    r.bouger(-40, 0);
//    Serial.println("avance");
//  }
//  else { 
//    if (((valDetecteurGauche==HIGH)&&(valDetecteurDroite==LOW))){
//      Serial.print("Gauche");
//      r.bouger(-30, 50);
//      detections = detections + 1;
//    }
//    if ((valDetecteurGauche==LOW)&&(valDetecteurDroite==HIGH)) {
//      Serial.print("droite");
//      r.bouger(-30, -50);
//      detections = detections + 1;
//    }
//  }  
//}
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
