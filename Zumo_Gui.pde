import processing.serial.*;
import controlP5.*;

Serial myPort;  // Create object from Serial class
String val;     // Data received from the serial port

ControlP5 cp5;

int myColor = color(255);

int c1,c2;

float n,n1;


void setup() {
  
  printArray(Serial.list());
  
  String portName = Serial.list()[2]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);
  
  size(430,670);
  noStroke();
  cp5 = new ControlP5(this);
  
  cp5.addTextfield("Info")
     .setPosition(50,360)
     .setSize(330,30)
     ;
  
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
  
  // and add another 2 buttons
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

  /*PImage[] imgs = {loadImage("button_a.png"),loadImage("button_b.png"),loadImage("button_c.png")};
  cp5.addButton("play")
     .setValue(128)
     .setPosition(140,300)
     .setImages(imgs)
     .updateSize()
     ;
     
  cp5.addButton("playAgain")
     .setValue(128)
     .setPosition(210,300)
     .setImages(imgs)
     .updateSize()
     ;*/

}

void draw() {
  background(myColor);
  myColor = lerpColor(c1,c2,n);
  n += (1-n)* 0.1; 
}

public void controlEvent(ControlEvent theEvent) {
  println(theEvent.getController().getName());
  n = 0;
}

// function colorA will receive changes from 
// controller with name colorA
public void Up() {
  c1 = c2;
  c2 = color(0,160,100);
  myPort.write('w');
  /*if ( myPort.available() > 0) 
  {  // If data is available,
    val = myPort.readStringUntil('\n');         // read it and store it in val
    println(val); //print it out in the console
  }*/
  
}

// function colorB will receive changes from 
// controller with name colorB
public void Left() {
  c1 = c2;
  c2 = color(150,0,0);
  myPort.write('a');
}

// function colorC will receive changes from 
// controller with name colorC
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
    //javax.swing.JOptionPane.showMessageDialog(null, val);
    //println(val); //print it out in the console
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