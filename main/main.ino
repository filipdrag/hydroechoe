#define NO_INPUTS 2
#define NO_OUTPUTS 3

#define DELAY_MS 100 //TODO calibrate

#define WH_PIN A0
#define WH_THRESHOLD 8
#define LS_LED_PIN A1
#define LS_SENSOR_PIN A2
#define LS_THRESHOLD 420 //TODO turbidity calibration
#define RGB_R A3
#define RGB_G A4
#define RGB_B A5

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

//RGB LED OUTPUT

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

void setColor(int r, int g, int b) {
  analogWrite(RGB_R, r);
  analogWrite(RGB_G, g);
  analogWrite(RGB_B, b);
}

void updateRGB() {
  if ( !ls_isMurky && !wh_isMoving ) {
    setColor(63, 63, 255);
    Serial.print("CLEAN, STILL -> BLUE\n");
  } else if ( !ls_isMurky && wh_isMoving ) {
    setColor(63, 255, 63);
    Serial.print("CLEAN, MOVING -> GREEN\n");
  } else if ( ls_isMurky && !wh_isMoving ) {
    setColor(105, 62, 7);
    Serial.print("DIRTY, STILL -> BROWN\n");
  } else if (ls_isMurky && wh_isMoving) {
    setColor(255, 0, 0);
    Serial.print("DIRTY, MOVING -> RED\n");
  }
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
  
  //RGB OUT
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
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
    Serial.print("RGB ");
    updateRGB();
  } else if (concurrency_ouput == 1) {
    Serial.print("BUZ ");
  } else if (concurency_output == 2) {
    Serial.print("VIB ");
  }

  concurrency_output = (concurrency_output + 1) % NO_OUTPUTS;

  delay(DELAY_MS);
}