int lastReading = 0;
bool isMoving = false;
int measurements_queue[10] = {0};
int* start;
int* end;
int* current;
int accumulated_change = 0;

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);

  start = &measurements_queue[0];
  end = &measurements_queue[9];
  current = start;
}

void loop() {
  int reading = analogRead(A0);
  int abs_change = abs(lastReading - reading);

  // rolling sum over 10 samples
  accumulated_change += abs_change - *current;
  *current = abs_change;
  current = (current == end) ? start : current + 1;

  isMoving = accumulated_change > 8;

  Serial.print(reading);
  Serial.print(' ');
  Serial.print(abs_change);
  if (isMoving) Serial.print(" Moving");
  else Serial.print(" Not moving");
  Serial.println();

  lastReading = reading;
  delay(100);
}