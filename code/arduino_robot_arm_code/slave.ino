#include <Servo.h>
#define button 2

Servo myServo;
int state = 20;
int buttonState = 0;

void setup() {
  pinMode(button, INPUT);
  myServo.attach(9);
  Serial.begin(9600); // Default communication rate of the Bluetooth module
}

void loop() {
 if(Serial.available() > 0){ // Checks whether data is comming from the serial port
    state = Serial.read(); // Reads the data from the serial port
 }
 // Controlling the servo motor
 myServo.write(state);
 delay(10);
 
 // Reading the button
 buttonState = digitalRead(button);
 if (buttonState == HIGH) {
   Serial.write('1'); // Sends '1' to the master to turn on LED
   Serial.println('1');
 }
 else {
   Serial.write('0');
   Serial.println('0');
 }  
}