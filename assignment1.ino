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
bool foundRoom = false;
bool firstLoop = true;
int totalRoomNum = 0;
String roomSide = "null";
int roomNumber = 0;

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

    //if the zumo is not at a corner
    if(!foundCorner && !foundRoom){
      //check middle 2 sensors to see if arrived at a corner
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
     
      //check left 2 sensors to see if zumo is going out of bounds on the left side
      } else if(sensorValues[0] > 300 || sensorValues[1] > 300){
        motors.setLeftSpeed(100);
        motors.setRightSpeed(-100);
        delay(500);
        motors.setLeftSpeed(50);
        motors.setRightSpeed(50);
        delay(500);
        motors.setLeftSpeed(0);
        motors.setRightSpeed(0);

      //check right 2 sensors to see if zumo is going out of bounds on the right side
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

          //to move the zumo forward
          if (incomingByte == 'w') {
            motors.setLeftSpeed(50);
            motors.setRightSpeed(50);
          }
          //to move the zumo left
          if (incomingByte == 'a') {
            motors.setLeftSpeed(-100);
            motors.setRightSpeed(100);
          }
          //to move the zumo back
          if (incomingByte == 's') {
            motors.setLeftSpeed(-50);
            motors.setRightSpeed(-50);
          }
          //to move the zumo right
          if (incomingByte == 'd') {
            motors.setLeftSpeed(100);
            motors.setRightSpeed(-100);
          }
          //to stop the zumo
          if (incomingByte == ' ') {
            motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
          }

          //if there is a room near
          if (incomingByte == 'r') {
            foundRoom = true;
            motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
          }
          
      }

    //if zumo is at a corner
    } else if(foundCorner) {
        
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
    } else if(foundRoom){

      if(firstLoop){
        roomNumber = totalRoomNum + 1;
        totalRoomNum++;
        Serial.println("Reset Room " + String(roomNumber) + " on " + roomSide);
        firstLoop = false;
      }
            
      if (Serial.available() > 0) {
        // read the oldest byte in the serial buffer:
        incomingByte = Serial.read();

        //to move the zumo forward
        if (incomingByte == 'w') {
          motors.setLeftSpeed(50);
          motors.setRightSpeed(50);
        }
        //to move the zumo left
        if (incomingByte == 'a') {
          if(roomSide == "null"){
            roomSide = "left";
          }
          motors.setLeftSpeed(-100);
          motors.setRightSpeed(100);
        }
        //to move the zumo back
        if (incomingByte == 's') {
          motors.setLeftSpeed(-50);
          motors.setRightSpeed(-50);
        }
        //to move the zumo right
        if (incomingByte == 'd') {
          if(roomSide == "null"){
            roomSide = "right";
          }
          motors.setLeftSpeed(100);
          motors.setRightSpeed(-100);
        }
        
        if (incomingByte == 'c') {
          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);
          Serial.println("Room " + String(roomNumber) + " on " + roomSide);
          foundRoom = false;
          firstLoop = true;
          roomSide = "null";
        }
      }
      
    }
    
  
  /*Serial.print("    ");
  Serial.println(position);*/

  
  
  //delay(250);
}

