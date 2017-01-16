#include <ZumoMotors.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <NewPing.h>

#define TRIGGER_PIN  2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     3  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


ZumoReflectanceSensorArray reflectanceSensors;
#define NUM_SENSORS 6
unsigned int sensorValues[NUM_SENSORS];
int incomingByte;      // a variable to read incoming serial data into

//Initialise bools, counters and arrays
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

//Setup ultrasonic scanner
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {

  //initialise reflectance sensors
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

  Serial.begin(9600);

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

      //initialise sonar ping variable
      int pingValue;

      for(int i = 0; i < 10; i++){

        if(!corridorObjectFound){

          //get sonar ping value
          pingValue = sonar.ping_cm();

          //if an object is within 20cm of the Zumo
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

            //get time of run and store it in array
            finished=millis();
            elapsed=finished-start;
            runs[totalRuns] = elapsed;

            //add run to room runs array
            roomRunNumbers[totalRoomNum] = totalRuns;

            //increment runs and run rooms
            totalRuns++;
            totalRunRooms++;
          }

          //if zumo is at the end of the track
          if (incomingByte == 'e') {
            foundEnd = true;
            motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
            /*for(int i = 0; i < totalRoomNum; i++){
              Serial.println(rooms[i]);
            }*/
            //Serial.println("+");

            //Add run time to run array
            finished=millis();
            elapsed=finished-start;
            runs[totalRuns] = elapsed;
            totalRuns++;
          }
          
      }

    //if zumo is at a corner
    } else if(foundCorner) {
        
      foundCornerFunction();
      
    } else if(foundRoom){

      foundRoomFunction();
      
    //if at end of track
    } else if(foundEnd){

      endFunction();

    //If an object has been found in the corridor
    } else if(corridorObjectFound){

      corridorObjectFuntion();
      
    }
    
  
}

void foundCornerFunction() {

  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
  
    Serial.println("Robot at corner");
    Serial.println("+");
  
    //turn zumo left
    if (incomingByte == 'a') {
      if(cornerTurn == "null"){
        cornerTurn = "left";
        corners[totalCorners] = "left";
        totalCorners++;
      }
      motors.setLeftSpeed(-100);
      motors.setRightSpeed(100);
    }
  
    //turn zumo right
    if (incomingByte == 'd') {
      if(cornerTurn == "null"){
        cornerTurn = "right";
        corners[totalCorners] = "right";
        totalCorners++;
      }
      motors.setLeftSpeed(100);
      motors.setRightSpeed(-100);
    }
  
    //signal corner turn is complete
    if (incomingByte == 'c') {
      motors.setLeftSpeed(0);
      motors.setRightSpeed(0);
  
      //start next run timer
      start = millis();
  
      //reset global variables for corner turn
      foundCorner = false;
      cornerTurn = "null";
    }
  }
  
}

void foundRoomFunction() {

  //only do on first loop
  if(firstLoop){
    //increment room counter and assign room number
    roomNumber = totalRoomNum + 1;
    totalRoomNum++;
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

    //signal zumo is back in corridor
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

    //signal to scan room
    if (incomingByte == 'b') {
      bool foundPerson = false;
      motors.setLeftSpeed(100);
      motors.setRightSpeed(-100);
      for (int i = 0; i < 25; i++){
        //get ping value
        int pingValue = sonar.ping_cm();
        //if person hasn't already been found
        if(!foundPerson){
          //if person is within 20cm of zumo
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
  
}

void endFunction() {

   //turn zumo around
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

      //If the run has a room in it
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

      //if it doesn't have a room in it
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
  
}

void corridorObjectFuntion() {

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
