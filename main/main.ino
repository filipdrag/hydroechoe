#define NO_INPUTS 2
#define NO_OUTPUTS 3

#define WH_PIN A0
#define WH_THRESHOLD 8
#define LS_LED_PIN A1
#define LS_SENSOR_PIN A2
#define LS_THRESHOLD 420 //TODO turbidity calibration

//CONCURRENCY INIT
// 0 water histogram
// 1 light sensor
int concurrency_input = 0;

// 0 RGB LED
// 1 buzzer
// 2 vibration motor
int concurrency_output = 0;

//WATER HISTOGRAM INIT
int wh_lastReading = 0;
bool wh_isMoving = false;
int wh_measurements_queue[10] = {0};
int* wh_start;
int* wh_end;
int* wh_current;
int wh_accumulated_change = 0;

//LIGHT SENSOR INIT
bool ls_isMurky = false;

void turbidityReading() {
  int Lvalue = analogRead(LS_SENSOR_PIN); // read the light
  int mVolt = map(Lvalue, 0, 1023, 0, 5000); // map analogue reading to 5000mV
  ls_isMurky = mVolt > LS_THRESHOLD;

  Serial.print(mVolt);
  Serial.print(" mV ");a
}

void waterLevelReading() {
  int reading = analogRead(WH_PIN);
    int abs_change = abs(lastReading - reading);
    wh_accumulated_change += abs_change - *wh_current;
    *wh_current = abs_change;
    wh_current = (wh_current == wh_end) ? wh_start : wh_current + 1;
    wh_lastReading = reading;
    wh_isMoving = wh_accumulated_change > WH_THRESHOLD;

    Serial.print(reading);
    Serial.print(" ");
    Serial.print(abs_change);
}

void setup() {
  //WATER HISTOGRAM
  pinMode(WH_PIN, INPUT); 
  Serial.begin(9600);

  start = &wh_measurements_queue[0];
  end = &wh_measurements_queue[9];
  wh_current = wh_start;

  //TURBIDITY SENSOR
  pinMode(LS_SENSOR_PIN, INPUT);
  pinMode(LS_LED_PIN, OUTPUT);
  digitalWrite(LS_LED_PIN, HIGH);
}

void loop() {
  if(concurrency_input == 0) {
    Serial.print("WH ");

    waterLevelReading();
    
    if(wh_isMoving) { Serial.print(" Moving"); }
    else { Serial.print(" Not moving"); }

  } else if (concurrency_input == 1) {
    Serial.print("LS ");
    
    turbidity_reading();

    if(ls_isMurky) { Serial.print("Murky"); }
    else { Serial.print("Clear"); }
  }

  concurrency_input = (concurrency_input + 1) % NO_INPUTS;

  if(concurerncy_output == 0) {
    
  }

}