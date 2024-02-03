//Input : pins 2 to 6
//Output : pin13, LOW means no cross detected, HIGH means a cross is detected


#define PINOFFSET 2

int sensorValues[5];


void setup() {
  //start serial connection
  Serial.begin(9600);

  for (int i=0;i<5;i++) {
    sensorValues[i] = 0;
    pinMode(i+PINOFFSET, INPUT);
  }

  pinMode(13, OUTPUT);
}

void loop() {
  //read sensor values
  for (int i=0;i<5;i++) {
    sensorValues[i] = digitalRead(i+PINOFFSET);
  }

  if (checkCross()) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
}

bool checkCross() {
  bool res = false;
  bool lineFound = false;
  bool lineEnded = false;
  for (int i=0;i<5;i++) {
    if (!lineFound) {
      lineFound = sensorValues[i]==HIGH;
    }
    else {
      if (!lineEnded) {
        lineEnded = sensorValues[i]==LOW;
      }
      else {
        if (sensorValues[i]==HIGH) {
          return true;
        }
      }
    }
  }
  return false;
}
