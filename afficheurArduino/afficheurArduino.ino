#include <VirtuinoCM.h>

/* Example: Bluetooth HC-05 + Arduino UNO - getting started
 * Created by Ilias Lamprou
 * Modified: Sep/9/2019
 */

#include "TM1637.h"
#include <SoftwareSerial.h>
//tx hc-05 -> arduino RX pin=2  
//rx hc-05 -> arduino TX pin=3   
SoftwareSerial espSerial =  SoftwareSerial(2,3);      
//-------------VirtuinoCM  Library and settings --------------
#include "VirtuinoCM.h"
VirtuinoCM virtuino;               
#define V_memory_count 32   //32       // the size of V memory. You can change it to a number <=255)
float V[V_memory_count];           // This array is synchronized with Virtuino V memory. You can change the type to int, long etc.

boolean debug = false;              // set this variable to false on the finale code to decrease the request time.
#define CLK 14//pins definitions for TM1637 and can be changed to other ports
#define DIO 15
TM1637 tm1637(CLK,DIO);
//============================================================== setup
//==============================================================
void setup() {
    Serial.begin(9600);
  if (debug) {
    while (!Serial) continue;
  }
  espSerial.begin(9600);  
  espSerial.setTimeout(50);
   tm1637.init();
   tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  virtuino.begin(onReceived,onRequested,256);  //Start Virtuino. Set the buffer to 256. With this buffer Virtuino can control about 28 pins (1 command = 9bytes) The T(text) commands with 20 characters need 20+6 bytes
  //virtuino.key="1234";                       //This is the Virtuino password. Only requests the start with this key are accepted from the library
/*  
  pinMode(4, OUTPUT);            // On Virtuino panel add a button to control this pin
  pinMode(13, OUTPUT);            // On Virtuino panel add a button to control this pin
  pinMode(6, INPUT);             // On Virtuino panel add a led to get the state of this pin
  pinMode(7, INPUT);             // On Virtuino panel add a led to get the state of this pin
  */
  }

//============================================================== loop
//==============================================================
void loop() {
  virtuinoRun();        // Necessary function to communicate with Virtuino. Client handler

  // enter your code below. Avoid to use delays on this loop. Instead of the default delay function use the vDelay that is located on the bottom of this code
  // You don't need to add code to read or write to the pins. Just enter the  pinMode of each Pin you want to use on void setup

  int value = (int)V[1];//mémoire utilisée

if ((int)V[1] > 0){

  Serial.println(value);
  tm1637.display(0,0);
  tm1637.display(1,value/100);
  tm1637.display(2,(value%100)/10);
  tm1637.display(3,(value%100)%10);
}
 else{//perte de com avec l appli android. Soit on affiche une valeur par defaut, soit on ne change pas la derniere valeur
  //value = 50;
//    tm1637.display(0,1);
//  tm1637.display(1,value/100);
//  tm1637.display(2,(value%100)/10);
//  tm1637.display(3,(value%100)%10);

 }
  vDelay(100);     // This is an example of the recommended delay function. Remove this if you don't need
 
}









//============================================================== onCommandReceived
//==============================================================
/* This function is called every time Virtuino app sends a request to server to change a Pin value
 * The 'variableType' can be a character like V, T, O  V=Virtual pin  T=Text Pin    O=PWM Pin 
 * The 'variableIndex' is the pin number index of Virtuino app
 * The 'valueAsText' is the value that has sent from the app   */
 void onReceived(char variableType, uint8_t variableIndex, String valueAsText){     
    if (variableType=='V'){
        float value = valueAsText.toFloat();        // convert the value to float. The valueAsText have to be numerical
        if (variableIndex<V_memory_count) V[variableIndex]=value;              // copy the received value to arduino V memory array
    }
}

//==============================================================
/* This function is called every time Virtuino app requests to read a pin value*/
String onRequested(char variableType, uint8_t variableIndex){     
    if (variableType=='V') {
    if (variableIndex<V_memory_count) return  String(V[variableIndex]);   // return the value of the arduino V memory array
  }
  return "";
}

//============================================================== virtuinoRun
  void virtuinoRun(){
    while (espSerial.available()) {
        char tempChar=espSerial.read();
        if (tempChar==CM_START_CHAR) {               // a new command is starting...
              virtuino.readBuffer=CM_START_CHAR;     // copy the new command to the virtuino readBuffer
              virtuino.readBuffer+=espSerial.readStringUntil(CM_END_CHAR);
              virtuino.readBuffer+=CM_END_CHAR;
              if (debug) Serial.println("\nCommand= "+virtuino.readBuffer);
              String* response= virtuino.getResponse();    // get the text that has to be sent to Virtuino as reply. The library will check the inptuBuffer and it will create the response text
              if (debug) Serial.println("Response : "+*response);
              espSerial.print(*response);
              break; 
         }
    }
 }
 

 //============================================================== vDelay
  void vDelay(int delayInMillis){
    long t=millis()+delayInMillis;
    while (millis()<t) virtuinoRun();
    }
