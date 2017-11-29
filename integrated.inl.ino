/**
 * Code for Operation Intercontinental Bear-listic Missile:
 * 
 * 1) generates random shooting times
 * 2) controls reloading mechanism
 * 3) controls shooting mechanism
 * 
 * Written by: Bair Cannon Man (Grant Kim)
 */



// variables for cannon system
// --> look to change if you're changing to a queue
const int MAX_SHOOTINGS = 30; // the max number of firing times the device will ever perform in a day
const int motorPin = 3; // pin for reloading motor
const int sprinklerPin = 4; // pin for valve
int base_num_shootings = 10; // dictates what num_shootings is (changed with buttons)
int num_shootings = 10; // # of shots taken in a day
int shots_fired = 0; // # of shots already taken in the day

// variables for timing mechanism
double start_time; // reference time for start of day
double current_time;
// --> look to change if you're changing to a queue
int randomTimes[MAX_SHOOTINGS]; // day's shooting times
double test_window = .1; // length of the "day" in hours

// variables for button system --> beware adjustment of pin positions
const int led3 = 10; // pin for led (3 feeedings)
const int led4 = 11;
const int buttonUp = 8; // pin for up button
const int buttonDown = 9;
boolean lastButtonUp = LOW; // previous up button state
boolean currentButtonUp = LOW; // current up button state
boolean lastButtonDown = LOW;
boolean currentButtonDown = LOW;
int currentFeedings = 3; // translated to num_shootings
const int minFeedings = 3; // smallest possible value of currentFeedings
const int maxFeedings = 4;
int lastFeedings = 4;

void setup() {
  pinMode(motorPin, OUTPUT);
  pinMode(sprinklerPin, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);
  Serial.begin(9600);

  randomizeTimes(0, 0, test_window, 0);
  for(int i = 0; i < base_num_shootings; i++) {
      //Serial.printf("random time %0.0f: %0.0f", i+1, randomTimes[i]);
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
  // checks buttons which affects base_num_shootings
  checkButtons();
  
  // if end of the day is reached
  if(current_time - start_time > test_window * 60 * 60) {
    Serial.println("DAY HAS RESTARTED");
    start_time = seconds();
    shots_fired = 0;

    // test print
    Serial.print("start time: ");
    Serial.println(start_time);

    Serial.print("number of shootings for the day: ");
    Serial.println(base_num_shootings);
    // allows base_num_shootings to change without affecting existing num_shootings during day
    num_shootings = base_num_shootings;
    
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

// checks buttons to change base_num_shootings, which is applied at beginning of day
void checkButtons() {
  currentButtonUp = digitalRead(buttonUp);
  if(lastButtonUp == LOW && currentButtonUp == HIGH) {
    if(currentFeedings != maxFeedings) {
      lastFeedings = currentFeedings;
      currentFeedings++;
      base_num_shootings = (int)(currentFeedings * (20.0 / 3.0));
    }
    Serial.println(currentFeedings);
    //Serial.println("current feedings: " + currentFeedings);
  }
  lastButtonUp = currentButtonUp;

  currentButtonDown = digitalRead(buttonDown);
  if(lastButtonDown == LOW && currentButtonDown == HIGH) {
    if(currentFeedings != minFeedings) {
      lastFeedings = currentFeedings;
      currentFeedings--;
      base_num_shootings = (int)(currentFeedings * (20.0 / 3.0));
    }
    Serial.println(currentFeedings);
    //Serial.println("current feedings: " + currentFeedings);
  }
  lastButtonDown = currentButtonDown;
  
  // may have to change this if pins change
  digitalWrite(currentFeedings + 7, true);
  digitalWrite(lastFeedings + 7, false);
}

// returns random shooting times; input number of shootings/day, start and end times of the shooting window (minutes)
// good
void randomizeTimes(double start_hours, double start_mins, double end_hours, double end_mins){
   double start_window = (start_hours*60 + start_mins) * 60;
   double end_window = (end_hours*60 + end_mins) * 60;
   double window_length = end_window - start_window;
   double interval = window_length / num_shootings;
   for(int i = 0; i < num_shootings; i++) {
     randomTimes[i] = random(interval*i, interval*i+interval*(5.0/6.0));
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
  Serial.println("random times after removal:");
  for(int i = 0; i < num_shootings - shots_fired - 1; i++) {
    Serial.println(randomTimes[i]);
  }
}

// converts millis() into a seconds function
// good
long seconds() {
  return millis() / 1000.0;
}

