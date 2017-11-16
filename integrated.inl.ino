/**
 * Code for Operation Intercontinental Bear-listic Missile:
 * 
 * 1) generates random shooting times
 * 2) controls reloading mechanism
 * 3) controls shooting mechanism
 * 
 * Written by: Bair Cannon Man
 */

int motorPin = 3;
int sprinklerPin = 4; // pin for testing with LED
double start_time;; // reference time (start of the day)
double current_time; // time in the day
int num_shootings = 20;
const int MAX_SHOOTINGS = 30; // the max number of firing times the device will ever perform in a day
int randomTimes[MAX_SHOOTINGS]; // day's shooting times
int shots_fired = 0; // when shots fired = num shootings, then stop reading from time array
double test_window = .5; // length of the "day" in hours

void setup() {
  pinMode(motorPin, OUTPUT);
  pinMode(sprinklerPin, OUTPUT);
  Serial.begin(9600);
  
  randomizeTimes(0, 0, test_window, 0);
  for(int i = 0; i < num_shootings; i++) {
      Serial.print("random time ");
      Serial.print(i+1);
      Serial.print(": ");
      Serial.println(randomTimes[i]);
  }
  start_time = seconds();
  Serial.print("start_time: ");
  Serial.println(start_time);
  current_time = seconds();
  Serial.print("current length of day (hours): ");
  Serial.println(test_window);
}

void loop() {
  // if end of the day is reached
  if(current_time - start_time > test_window * 60 * 60) {
    Serial.println("DAY HAS RESTARTED");
    start_time = seconds();
    shots_fired = 0;

    // test print
    Serial.print("start time: ");
    Serial.println(start_time);
    
    randomizeTimes(0, 0, test_window, 0);

    // test print
    for(int i = 0; i < num_shootings; i++) {
      Serial.print("random time ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(randomTimes[i]);
    }
  }
  
  // if one of the shooting times is reached
  if(current_time - start_time > randomTimes[0]) {
    // shoots x amount of times per feeding time
    if(shots_fired  < num_shootings) {
      Serial.println("SHOOTING TIME HAS BEEN REACHED");
      reload(1000);
      delay(2000);
      shoot();
      removeTime();
      shots_fired = shots_fired + 1;
      Serial.print("shots fired: ");
      Serial.println(shots_fired);
    }
  }

  current_time = seconds();
  Serial.print("current time: ");
  Serial.println(current_time);
  delay(5000);
}

// returns random shooting times; input number of shootings/day, start and end times of the shooting window (minutes)
// good
void randomizeTimes(double start_hours, double start_mins, double end_hours, double end_mins){
  if(num_shootings == 20) {
    double start_window = (start_hours*60 + start_mins) * 60;
    double end_window = (end_hours*60 + end_mins) * 60;
    double window_length = end_window - start_window;
    double interval = window_length / 20;
    for(int i = 0; i < 20; i++) {
      randomTimes[i] = random(interval*i, interval*i+interval*(5.0/6.0));
    }
  }
}

// turns the reloading mechanism; input reload time in terms of seconds
// good
void reload(int reloadTime){
  double pseudo_start_time = millis(); // gives reference for reloading time 
  current_time = millis();
  while(current_time - pseudo_start_time < reloadTime*1000) {
    digitalWrite(motorPin, HIGH);
    current_time = millis();
  }
  digitalWrite(motorPin, LOW);
}

// opens solenoid valve
// good
void shoot() {
  digitalWrite(sprinklerPin, LOW);
  delay(3000);
  digitalWrite(sprinklerPin, HIGH);
  delay(3000);
  digitalWrite(sprinklerPin, LOW);
}

// removes shooting time from array when fulfilled
void removeTime() {
  int tempTimes[MAX_SHOOTINGS];
  for(int i = 0; i < num_shootings - shots_fired; i++)
  {
    tempTimes[i] = randomTimes[i + 1];
  }
  for(int i = num_shootings - shots_fired; i < MAX_SHOOTINGS; i++)
  {
    tempTimes[i] = 0;
  }
  for(int i = 0; i < MAX_SHOOTINGS; i++) {
    randomTimes[i] = tempTimes[i];
  }

  // test printing
  Serial.println("random time after removal:");
  for(int i = 0; i < num_shootings - shots_fired; i++) {
    Serial.println(randomTimes[i]);
  }
}

// converts millis() into a seconds function
// good
long seconds() {
  return millis() / 1000.0;
}

