#include <ZumoMotors.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
//#include <NewPing.h>


//Select for cable, Vart for xbees


ZumoReflectanceSensorArray reflectanceSensors;
#define NUM_SENSORS 6
unsigned int sensorValues[NUM_SENSORS];
int incomingByte;      // a variable to read incoming serial data into
bool foundCorner = false;

#define LED_PIN 13

ZumoMotors motors;

void setup() {
  // put your setup code here, to run once:
  reflectanceSensors.init();

  delay(500);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);        // turn on LED to indicate we are in calibration mode
  
  unsigned long startTime = millis();
  while(millis() - startTime < 10000)   // make the calibration take 10 seconds
  {
    reflectanceSensors.calibrate();
  }
  digitalWrite(13, LOW);         // turn off LED to indicate we are through with calibration

  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
  for (byte i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(reflectanceSensors.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  
  // print the calibration maximum values measured when emitters were on
  for (byte i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(reflectanceSensors.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
   // read calibrated sensor values and obtain a measure of the line position.
  // Note: the values returned will be incorrect if the sensors have not been properly
  // calibrated during the calibration phase.
  unsigned int position = reflectanceSensors.readLine(sensorValues);

  // To get raw sensor values instead, call:  
  //reflectanceSensors.read(sensorValues);


    //Serial.print(sensorValues[i]);
    //Serial.print(' ');
    
    if(!foundCorner){
      if(sensorValues[2] > 200 || sensorValues[3] > 200){
        motors.setLeftSpeed(-100);
        motors.setRightSpeed(-100);
        delay(100);
        motors.setLeftSpeed(0);
        motors.setRightSpeed(0);
        //motors.setLeftSpeed(100);
        //motors.setRightSpeed(-100);
        //delay(1500);
        foundCorner = true;
      } else if(sensorValues[0] > 300 || sensorValues[1] > 300){
        motors.setLeftSpeed(100);
        motors.setRightSpeed(-100);
        delay(500);
        motors.setLeftSpeed(50);
        motors.setRightSpeed(50);
        delay(500);
        motors.setLeftSpeed(0);
        motors.setRightSpeed(0);
      } else if(sensorValues[4] > 300 || sensorValues[5] > 300){
        motors.setLeftSpeed(-100);
        motors.setRightSpeed(100);
        delay(500);
        motors.setLeftSpeed(50);
        motors.setRightSpeed(50);
        delay(500);
        motors.setLeftSpeed(0);
        motors.setRightSpeed(0);
      } 
      if (Serial.available() > 0) {
          // read the oldest byte in the serial buffer:
          incomingByte = Serial.read();
      
          if (incomingByte == 'w') {
            motors.setLeftSpeed(50);
            motors.setRightSpeed(50);
          }
          if (incomingByte == 'a') {
            motors.setLeftSpeed(-100);
            motors.setRightSpeed(100);
          }
          if (incomingByte == 's') {
            motors.setLeftSpeed(-50);
            motors.setRightSpeed(-50);
          }
          if (incomingByte == 'd') {
            motors.setLeftSpeed(100);
            motors.setRightSpeed(-100);
          }
      
          if (incomingByte == ' ') {
            motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
          }
      }
    } else {
        
      if (Serial.available() > 0) {
        // read the oldest byte in the serial buffer:
        incomingByte = Serial.read();
        if (incomingByte == 'a') {
          motors.setLeftSpeed(-100);
          motors.setRightSpeed(100);
        }
        if (incomingByte == 'd') {
          motors.setLeftSpeed(100);
          motors.setRightSpeed(-100);
        }
        if (incomingByte == 'c') {
          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);
          foundCorner = false;
        }
      }
    }
    
  
  /*Serial.print("    ");
  Serial.println(position);*/

  
  
  //delay(250);
}
