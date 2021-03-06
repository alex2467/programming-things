//Import serial and controlP5 libraries
import processing.serial.*;
import controlP5.*;

Serial myPort;  // Create object from Serial class
String val;     // Data received from the serial port

ControlP5 cp5;

int myColor = color(255);

int c1,c2;

float n,n1;


void setup() {
  
  //printArray(Serial.list());
  
  //Connect to XBee port
  String portName = Serial.list()[2]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);
  
  //define size of window
  size(430,670);
  noStroke();
  
  //create ControlP5 Object
  cp5 = new ControlP5(this);
  
  //Add a text field to gui to get serial text outputs
  cp5.addTextfield("Info")
     .setPosition(50,360)
     .setSize(330,30)
     ;
  
  //Add buttons to gui to control the Zumo
  cp5.addButton("End")
     .setValue(0)
     .setPosition(220,265)
     .setSize(160,30)
     ;
     
  cp5.addButton("Scan")
     .setValue(0)
     .setPosition(220,305)
     .setSize(160,30)
     ;
     
  cp5.addButton("Up")
     .setValue(0)
     .setPosition(190,100)
     .setSize(50,50)
     ;
  
  cp5.addButton("Left")
     .setValue(100)
     .setPosition(135,155)
     .setSize(50,50)
     ;
     
  cp5.addButton("Down")
     .setPosition(190,155)
     .setSize(50,50)
     .setValue(0)
     ;
     
  cp5.addButton("Right")
     .setPosition(245,155)
     .setSize(50,50)
     .setValue(0)
     ;
     
  cp5.addButton("Stop")
     .setPosition(135,215)
     .setSize(160,30)
     .setValue(0)
     ;
     
  cp5.addButton("Complete")
     .setPosition(50,265)
     .setSize(160,30)
     .setValue(0)
     ;
     
  cp5.addButton("Room")
     .setPosition(50,305)
     .setSize(160,30)
     .setValue(0)
     ;

}

void draw() {
  //Set the background colour
  background(myColor);
  myColor = lerpColor(c1,c2,n);
  n += (1-n)* 0.1;
}

public void controlEvent(ControlEvent theEvent) {
  //println(theEvent.getController().getName());
  n = 0;
}

public void Up() {
  c1 = c2;
  c2 = color(0,160,100);
  myPort.write('w'); 
}

public void Left() {
  c1 = c2;
  c2 = color(150,0,0);
  myPort.write('a');
}

public void Down() {
  c1 = c2;
  c2 = color(255,255,0);
  myPort.write('s');
}

public void Right() {
  c1 = c2;
  c2 = color(0,0,0);
  myPort.write('d');
}

public void Stop() {
  myPort.write(' ');
}

public void Complete() {
  myPort.write('c');
  delay(1000);
  if ( myPort.available() > 0) 
  {  // If data is available,
    val = myPort.readStringUntil('+');         // read it and store it in val
    if(val != null){
      cp5.get(Textfield.class, "Info").setText(val);
    }
  }
}

public void Room() {
  myPort.write('r');
}

public void Scan() {
  myPort.write('b');
  delay(6000);
  if ( myPort.available() > 0) 
  {  // If data is available,
    val = myPort.readStringUntil('+');         // read it and store it in val
    if(val != null){
      cp5.get(Textfield.class, "Info").setText(val);
    }
  }
}

public void End() {
  myPort.write('e');
  delay(1000);
  if ( myPort.available() > 0) 
  {  // If data is available,
    val = myPort.readStringUntil('+');         // read it and store it in val
    if(val != null){
      cp5.get(Textfield.class, "Info").setText(val);
    }
  }
}

//Get key inputs and call the correct functions
void keyPressed() {
  if (key == 'w') {
    Up();
  }
  if (key == 'a') {
    Left();
  }
  if (key == 's') {
    Down();
  }
  if (key == 'd') {
    Right();
  }
  if (key == ' ') {
    Stop();
  }
  if (key == 'c') {
    Complete();
    
  }
  if (key == 'r') {
    Room();
  }
  if (key == 'b') {
    Scan();
  }
  if (key == 'e') {
    End();
  }
}
