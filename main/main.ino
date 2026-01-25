#define NO_INPUTS 2
#define NO_OUTPUTS 3

#define DELAY_MS 100 //TODO calibrate

#define WH_PIN A0
#define WH_THRESHOLD 30
#define LS_LED_PIN A1
#define LS_SENSOR_PIN A2
#define LS_THRESHOLD 170 //TODO turbidity calibration
#define RGB_R 11
#define RGB_G 10
#define RGB_B 9
#define VIB_PIN 3
#define BUZZ_PIN 6

//CONCURRENCY INIT
// 0 water histogram
// 1 light sensor
short int concurrency_input = 0;

// 0 RGB LED
// 1 buzzer
// 2 vibration motor
short int concurrency_output = 0;

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

//VIBRATION OUTPUT
// 0 not vibrating
// 1 low vibration
// 2 high vibration
short int vib_status = 0;
bool vib_on = false;

//BUZZER OUTPUT
bool is_buzzing = false;

void turbidityReading() {
  int Lvalue = analogRead(LS_SENSOR_PIN); // read the light
  int mVolt = map(Lvalue, 0, 1023, 0, 5000); // map analogue reading to 5000mV
  ls_isMurky = mVolt < LS_THRESHOLD;

  Serial.print(mVolt);
  Serial.print(" mV ");
}

void waterLevelReading() {
  int reading = analogRead(WH_PIN);
    int abs_change = abs(wh_lastReading - reading);
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
    setColor(0, 0, 255);
    Serial.print("CLEAN, STILL -> BLUE\n");
  } else if ( !ls_isMurky && wh_isMoving ) {
    setColor(0, 255, 0);
    Serial.print("CLEAN, MOVING -> GREEN\n");
  } else if ( ls_isMurky && !wh_isMoving ) {
    setColor(255, 165, 5);
    Serial.print("DIRTY, STILL -> BROWN\n");
  } else if (ls_isMurky && wh_isMoving) {
    setColor(255, 0, 0);
    Serial.print("DIRTY, MOVING -> RED\n");
  }
  else {
    setColor(255, 255, 255);
    Serial.print("wtf bro");
  }
}

void updateVibration() {
  static unsigned long last_toggle = 0;
  
  if(!wh_isMoving) {
    digitalWrite(VIB_PIN, LOW);
    vib_on = false;
    return;
  }

  unsigned long interval = ls_isMurky ? 70 : 1000;
  unsigned long now = millis();
  if(now - last_toggle >= interval) {
    vib_on = !vib_on;
    digitalWrite(VIB_PIN, vib_on ? HIGH : LOW);
    last_toggle = now;
  }
}

void updateBuzzer() {
  if( ls_isMurky && !is_buzzing ) {
    tone(BUZZ_PIN, 50);
    is_buzzing = true;
  } else if (!ls_isMurky && is_buzzing) {
    noTone(BUZZ_PIN);
    is_buzzing = false;
    }
}

void setup() {
  //WATER HISTOGRAM
  pinMode(WH_PIN, INPUT); 
  Serial.begin(9600);

  wh_start = &wh_measurements_queue[0];
  wh_end = &wh_measurements_queue[9];
  wh_current = wh_start;

  //TURBIDITY SENSOR
  pinMode(LS_SENSOR_PIN, INPUT);
  pinMode(LS_LED_PIN, OUTPUT);
  digitalWrite(LS_LED_PIN, HIGH);
  
  //RGB OUT
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  pinMode(VIB_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
}

void loop() {
  if(concurrency_input == 0) {
    Serial.print("\nWH ");

    waterLevelReading();
    
    //wh_isMoving = true;
    if(wh_isMoving) { Serial.print(" Moving"); }
    else { Serial.print(" Not moving"); }

  } else if (concurrency_input == 1) {
    Serial.print("\nLS ");
    
    turbidityReading();

    if(ls_isMurky) { Serial.print("Murky"); }
    else { Serial.print("Clear"); }
  }

  concurrency_input = (concurrency_input + 1) % NO_INPUTS;

  if(concurrency_output == 0) {
    Serial.print("\nRGB ");
    updateRGB();
  } else if (concurrency_output == 1) {
    Serial.print("\nBUZ ");
    updateBuzzer();
    if (is_buzzing) { Serial.print("Buzzing "); }
    else { Serial.print('Not buzzing'); }
    
  } else if (concurrency_output == 2) {
    Serial.print("\nVIB ");
    updateVibration();
    switch (vib_status) {
      case 0: Serial.print("Not vibrating "); break;
      case 1: Serial.print("Low vibration "); break;
      case 2: Serial.print("Hight vibration "); break;
      default: Serial.print(vib_status);
    }
  }

  concurrency_output = (concurrency_output + 1) % NO_OUTPUTS;

  delay(DELAY_MS);
}