// RGB LED pins (adjust based on your wiring)
const int RED_PIN = 11;      // PWM pin
const int GREEN_PIN = 10;   // PWM pin
const int BLUE_PIN = 9;    // PWM pin

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void setColor(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}
 
void loop() {
  setColor(255, 0, 0);      // Red
  delay(1000);
  
  setColor(255, 64, 0);    // Orange
  delay(1000);
  
  setColor(255, 128, 0);    // Yellow
  delay(1000);
  
  setColor(0, 255, 0);      // Green
  delay(1000);
  
  setColor(0, 255, 255);    // Cyan
  delay(1000);
  
  setColor(0, 0, 255);      // Blue
  delay(1000);
  
  setColor(255, 0, 255);    // Magenta
  delay(1000);
}
