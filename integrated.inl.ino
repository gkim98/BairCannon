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
const int MAX_SHOOTINGS = 30;     // the max number of firing times the device will ever perform in a day
const int motorPin = 6;           // pin for reloading motor
const int sprinklerPin = 8;       // pin for valve
int base_num_shootings = 10;      // dictates what num_shootings is (changed with buttons)
int num_shootings = 10;           // # of shots taken in a day
int shots_fired = 0;              // # of shots already taken in the day
  
// variables for timing mechanism
double start_time;                // reference time for start of day
double current_time;
int randomTimes[MAX_SHOOTINGS];   // day's shooting times
double test_window = .1;          // length of the "day" in hours

// variables for button system
const int led3 = 10;              // pin for led (3 feeedings)
const int led4 = 11;
const int led5 = 12;
const int buttonUp = 3;           // pin for up button
const int buttonDown = 2;
boolean lastButtonUp = LOW;       // previous up button state
boolean currentButtonUp = LOW;    // current up button state
boolean lastButtonDown = LOW;
boolean currentButtonDown = LOW;
int currentFeedings = 3;          // translated to num_shootings
const int minFeedings = 3;        // smallest possible value of currentFeedings
const int maxFeedings = 5;
int lastFeedings = 4;

void setup() {
  pinMode(motorPin, OUTPUT);
  pinMode(sprinklerPin, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);
  Serial.begin(9600);

  // randomizeTimes called in setup because it's only called after day's end
  randomizeTimes(0, 0, test_window, 0);

 // configures time variables on device startup
  start_time = seconds();
  Serial.print("start_time: ");
  Serial.println(start_time);
  current_time = seconds();
  Serial.print("current length of day (hours): ");
  Serial.println(test_window);
}

void loop() {
  
  // checks the buttons that change base_num_shootings
  checkButtons();
  
  // if end of the day is reached
  if(current_time - start_time > test_window * 60 * 60) {
    Serial.println("DAY HAS RESTARTED");
    start_time = seconds();
    shots_fired = 0;
    
    // allows base_num_shootings to change without affecting existing num_shootings during day
    num_shootings = base_num_shootings;

    // produces random shooting times for the next day
    randomizeTimes(0, 0, test_window, 0);

    // displays shooting times
    for(int i = 0; i < num_shootings; i++) {
      Serial.print("random time ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(randomTimes[i]);
    }
  }
  
  // if one of the shooting times is reached
  if(current_time - start_time > randomTimes[shots_fired]) {
    // reloads and shoots device
    if(shots_fired < num_shootings) {
      Serial.println("SHOOTING TIME HAS BEEN REACHED");
      reload(1000);
      delay(2000);
      shoot();
      shots_fired = shots_fired + 1;
      Serial.print("shots fired: ");
      Serial.println(shots_fired);
    }
  }

  // updates current time to compare to day's reference start time
  current_time = seconds();
  Serial.print("current time: ");
  Serial.println(current_time);
}



/// HELPER METHODS

// changes base_num_shootings on button presses --> applied at beginning of day
void checkButtons() {
  // two buttons for incrementing/decrementing shootings
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
  }
  lastButtonDown = currentButtonDown;
  
  // may have to change if pins change
  digitalWrite(currentFeedings + 7, true);
  digitalWrite(lastFeedings + 7, false);
}

// returns random shooting times; input the starting and ending time of shooting window in military time
void randomizeTimes(double start_hours, double start_mins, double end_hours, double end_mins){
   double start_window = (start_hours*60 + start_mins) * 60;
   double end_window = (end_hours*60 + end_mins) * 60;
   double window_length = end_window - start_window;
   double interval = window_length / num_shootings;
   for(int i = 0; i < num_shootings; i++) {
      // absolutely avoids overlaps by "padding" the end of shooting intervals
     randomTimes[i] = random(interval*i, interval*i+interval*(5.0/6.0));
   }
}

// turns the reloading mechanism; input reloadTime in terms of seconds
void reload(int reloadTime){
  double pseudo_start_time = millis(); // gives time reference for beginning of reloading
  current_time = millis();
  while(current_time - pseudo_start_time < 10000) {
    digitalWrite(motorPin, HIGH);
    current_time = millis();
  }
  digitalWrite(motorPin, LOW);
}

// opens and closes solenoid valve to release air pressure
void shoot() {
  digitalWrite(sprinklerPin, LOW);
  delay(1000);
  digitalWrite(sprinklerPin, HIGH);
  delay(1000);
  digitalWrite(sprinklerPin, LOW);
}

// converts millis() into a seconds function
long seconds() {
  return millis() / 1000.0;
}
