# programming-things
Coursework 1 - Robot

This project uses the Arduino Uno and a couple of XBees to get a Zumo robot to navigate its way around a track which is layed out like a building. The Zumo cannot go past the walls so the Zumo Reflectance Sensor is used to check if the Zumo is going out of bounds and then correct its position. The user navigates the Zumo using a GUI made in processing with the library ControlP5. The Zumo has to check rooms for people (Objects) that need saving from a fire. An ultrsonic sensor is used for this that uses the newPing library to sense if there are objects in front of the Zumo. If the Zumo finds people in the rooms it must report back what room the people are in so they can be rescued. When the Zumo gets to the end of the track it must then optimise a return route checking the rooms that it found people in to make sure they were rescued. If they weren't a light on the Zumo will light up so the person can follow it back to safety.

All of the code was written in the Arduino and Processing Development Environments. The code in the Arduino environment is what runs the main elements of the program and Processing is just used for the GUI.
