#define light_sensor A0
#define turbidy_light A1

void setup() {
  Serial.begin(9600);
  pinMode(light_sensor, INPUT);
  pinMode(turbidy_light, OUTPUT);
}

void turbidityReading() {
  int Lvalue = analogRead(light_sensor); // read the light
  int mVolt = map(Lvalue, 0, 1023, 0, 5000); // map analogue reading to 5000mV
  float volt = (double)mVolt / 1000; // convert millivolt to volt
  Serial.print(mVolt); // print millivolt
  Serial.print("mV ");
  Serial.print(volt, 3); // print volts with 3 decimal places
  Serial.println("V ");           // Send value to Serial Monitor
}

void loop() {
  analogWrite(turbidy_light, 200);  // turn the LED on (HIGH is the voltage level)
  delay(100);                      // wait for a second
  turbidityReading();
  delay(100);

  analogWrite(turbidy_light, 0);   // turn the LED off by making the voltage LOW
  delay(500); // wait for 1000 milliseconds
}