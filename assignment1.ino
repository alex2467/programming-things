#include <ZumoMotors.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <NewPing.h>

#define TRIGGER_PIN  2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     3  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

//Select for cable, Vart for xbees


ZumoReflectanceSensorArray reflectanceSensors;
#define NUM_SENSORS 6
unsigned int sensorValues[NUM_SENSORS];
int incomingByte;      // a variable to read incoming serial data into
bool foundCorner = false;
bool foundRoom = false;
bool foundEnd = false;
bool firstLoop = true;
bool corridorObjectFound = false;
int totalRoomNum = 0;
int totalCorners = 0;
int totalPeopleCount = 0;
String roomSide = "null";
String cornerTurn = "null";
int roomNumber = 0;
String rooms[20];
String roomSides[20];
String corners[20];
int peopleRooms[20];
int runs[20];
int start;
int finished;
int elapsed;
int totalRuns = 0;
int totalRunRooms = 0;
int roomRunNumbers[20];
bool foundRoomRun = false;

#define LED_PIN 13

ZumoMotors motors;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

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
    //Serial.print(reflectanceSensors.calibratedMinimumOn[i]);
    //Serial.print(' ');
  }
  //Serial.println();
  
  // print the calibration maximum values measured when emitters were on
  for (byte i = 0; i < NUM_SENSORS; i++)
  {
    //Serial.print(reflectanceSensors.calibratedMaximumOn[i]);
    //Serial.print(' ');
  }
  //Serial.println();
  //Serial.println();
  delay(1000);

  start = millis();

}

void loop() {
  // read calibrated sensor values and obtain a measure of the line position.
  // Note: the values returned will be incorrect if the sensors have not been properly
  // calibrated during the calibration phase.
  unsigned int position = reflectanceSensors.readLine(sensorValues);

  // To get raw sensor values instead, call:  
  //reflectanceSensors.read(sensorValues);

    //if the zumo is not at a corner
    if(!foundCorner && !foundRoom && !foundEnd && !corridorObjectFound){
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
        finished=millis();
        elapsed=finished-start;
        runs[totalRuns] = elapsed;
        totalRuns++;
     
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

      int pingValue;

      for(int i = 0; i < 10; i++){

        if(!corridorObjectFound){

          pingValue = sonar.ping_cm();
  
          if(pingValue > 0 && pingValue < 20){
            motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
            if(totalRoomNum == 0){
              if(totalCorners == 0){
                Serial.println("Object found before corner 1");
                Serial.println("+");
                corridorObjectFound = true;
              } else {
                Serial.println("Object found after corner " + String(totalCorners));
                Serial.println("+");
                corridorObjectFound = true;
              }
            } else {
              Serial.println("Object found after room " + String(totalRoomNum) + " and corner " + String(totalCorners));
              Serial.println("+");
              corridorObjectFound = true;
            }
            
          }

        }

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
            finished=millis();
            elapsed=finished-start;
            runs[totalRuns] = elapsed;
            roomRunNumbers[totalRoomNum] = totalRuns;
            totalRuns++;
            totalRunRooms++;
          }

          if (incomingByte == 'e') {
            foundEnd = true;
            motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
            //Serial.println("Start");
            for(int i = 0; i < totalRoomNum; i++){
              Serial.println(rooms[i]);
            }
            Serial.println("+");
            finished=millis();
            elapsed=finished-start;
            runs[totalRuns] = elapsed;
            totalRuns++;
          }
          
      }

    //if zumo is at a corner
    } else if(foundCorner) {
        
      if (Serial.available() > 0) {
        // read the oldest byte in the serial buffer:
        incomingByte = Serial.read();
        if (incomingByte == 'a') {
          if(cornerTurn == "null"){
            cornerTurn = "left";
            corners[totalCorners] = "left";
            totalCorners++;
          }
          motors.setLeftSpeed(-100);
          motors.setRightSpeed(100);
        }
        if (incomingByte == 'd') {
          if(cornerTurn == "null"){
            cornerTurn = "right";
            corners[totalCorners] = "right";
            totalCorners++;
          }
          motors.setLeftSpeed(100);
          motors.setRightSpeed(-100);
        }
        if (incomingByte == 'c') {
          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);
          start = millis(); 
          foundCorner = false;
          cornerTurn = "null";
        }
      }
    } else if(foundRoom){

      if(firstLoop){
        roomNumber = totalRoomNum + 1;
        totalRoomNum++;
        //Serial.println("Reset Room " + String(roomNumber) + " on " + roomSide);
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

        //to stop the zumo
        if (incomingByte == ' ') {
          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);
        }
        
        if (incomingByte == 'c') {
          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);
          Serial.println("Room " + String(roomNumber) + " on " + roomSide);
          Serial.println("+");
          rooms[roomNumber - 1] = "Room " + String(roomNumber) + " on " + roomSide;
          roomSides[roomNumber - 1] = roomSide;
          foundRoom = false;
          firstLoop = true;
          roomSide = "null";
          start = millis();
        }

        if (incomingByte == 'b') {
          bool foundPerson = false;
          motors.setLeftSpeed(100);
          motors.setRightSpeed(-100);
          for (int i = 0; i < 25; i++){
            int pingValue = sonar.ping_cm();
            if(!foundPerson){
              if(pingValue > 0 && pingValue < 20){
                foundPerson = true;
                peopleRooms[totalPeopleCount] = roomNumber;
                totalPeopleCount++;
                Serial.println("Found in room " + String(roomNumber) + " on " + roomSide);
              }
            }
            delay(50);
          }
          
          motors.setLeftSpeed(-100);
          motors.setRightSpeed(100);
          for (int i = 0; i < 50; i++){
            int pingValue = sonar.ping_cm();
            if(!foundPerson){
              if(pingValue > 0 && pingValue < 20){
                foundPerson = true;
                peopleRooms[totalPeopleCount] = roomNumber;
                totalPeopleCount++;
                Serial.println("Found in room " + String(roomNumber) + " on " + roomSide);
              }
            }
            delay(50);
          }

          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);

        }
      }
      
    } else if(foundEnd){

      motors.setLeftSpeed(100);
      motors.setRightSpeed(-100);
      delay(3800);

      while(totalRuns > 0){

        totalRuns--;
        foundCorner = false;

        for(int i = 0; i <= totalRunRooms; i++){
          if(roomRunNumbers[i] == totalRuns){
            foundRoomRun = true;
          }
        }

        if(foundRoomRun){
  
          motors.setLeftSpeed(100);
          motors.setRightSpeed(-100);

          delay(1000);

          motors.setLeftSpeed(50);
          motors.setRightSpeed(50);

          delay(500);

          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);

          foundRoomRun = false;
          
        } else {

          while(!foundCorner){

            unsigned int position = reflectanceSensors.readLine(sensorValues);

            motors.setLeftSpeed(50);
            motors.setRightSpeed(50);
  
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
              motors.setLeftSpeed(-100);
              motors.setRightSpeed(100);
              delay(1700);
              motors.setLeftSpeed(0);
              motors.setRightSpeed(0);
           
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

          }
          
        }
  
        
      
      }

      if(totalRuns == 0){
        foundEnd = false;
        
      }
      
    } else if(corridorObjectFound){

      if (Serial.available() > 0) {
        // read the oldest byte in the serial buffer:
        incomingByte = Serial.read();

        if (incomingByte == 'c') {
          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);
          corridorObjectFound = false;
        }

      }
      
    }
    
  
}
