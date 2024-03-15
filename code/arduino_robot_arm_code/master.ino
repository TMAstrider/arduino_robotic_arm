#define ledPin 13
const int flexPin = A3;
int state = 0;
int potValue = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600); // Default communication rate of the Bluetooth module

  pinMode(flexPin, INPUT);
}

void loop() {
 if(Serial.available() > 0){ // Checks whether data is comming from the serial port
    state = Serial.read(); // Reads the data from the serial port
 }
 // Controlling the LED
 if (state == '1') {
  digitalWrite(ledPin, HIGH); // LED ON
  state = 0;
 }
 else if (state == '0') {
  digitalWrite(ledPin, LOW); // LED ON
  state = 0;
 }
 // Reading the potentiometer
//  potValue = analogRead(A0);
//  int potValueMapped = map(potValue, 0, 1023, 0, 255);
//  Serial.write(potValueMapped); // Sends potValue to servo motor
//  delay(10);
  int flexSensor = analogRead(flexPin);
  flexSensor -= 430;
  flexSensor *= 4;
  int flexMapped = map(flexSensor, 0, 1023, 0, 255);
  Serial.write(flexMapped);
  delay(10);
}