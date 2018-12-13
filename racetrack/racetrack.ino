#include <LiquidCrystal.h>
//PINS
const int piezo = 10, c3 = 6, c2 = 7, c1 = 8, go = 9;
int lights[] = {c1, c2, c3, go}; //pins attached to leds

//LCD PINS
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

//TIMES (in ms)
long countdown_start_time = 0; //time the countdown starts
long race_start_time = 0; //time the race starts
long countdown_current_time = 0; //current time in the countdown
long race_current_time = 0; //current time in the race
long car1_finish_time = 0; //car 1 finishing time
long car2_finish_time = 0; //car 2 finishing time

long race_current_time_seconds = 0; //current time in the race in seconds

//FLAGS
int has_race_started = 0; //Has the race started?
int car1_finished = 0; //Has car 1 finished?
int car2_finished = 0; //Has car 2 finished?
int last_displayed_time = 0; //Last displayed time on the LCD display (in seconds)
int count_lights_off = 0; //Have the countdown lights been turned off (when race begins)?
int count3 = 0; //Has the light for count 3 turned on?
int count2 = 0; //Has the light for count 2 turned on?
int count1 = 0; //Has the light for count 1 turned on?

int last_readout_time = 0;

/*
Default Layout

+---+   +---+
|UNO|   |LCD|
| 12|---|RS |
| 11|---|EN |
|  5|---|D4 |
|  4|---|D5 |
|  3|---|D6 |
|  2|---|D7 |
|GND|---|R/W|
|GND|---|VSS|
| 5V|---|VCC|
|  3|---|VO |
|   |   +---+
| 10|---|PZO|
|   |   +---+
|   |   |LED|
|  6|---|3  |
|  7|---|2  |
|  8|---|1  |
|  9|---|GO |
|   |   +---+
| A0|---|LDR|
| A1|---|LDR|
+---+   +---+
*/

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  lcd.begin(16, 2);
  pinMode(c3, OUTPUT);
  pinMode(c2, OUTPUT);
  pinMode(c1, OUTPUT);
  pinMode(go, OUTPUT);
  pinMode(piezo, OUTPUT);
}

void loop() {
  if (!has_race_started && digitalRead(13) == HIGH) {
    //activate readout mode
    pinsOff(lights);
    readoutMode(lcd, analogRead(A0), analogRead(A1));
  }
  
  if (countdown_start_time == 0) {
    countdown_start_time = millis();
  }
  
  countdown_current_time = millis() - countdown_start_time;
  if (countdown_current_time > 0 && !count3) {
    digitalWrite(c3, HIGH);
    lcd.clear();
    lcd.print("3...");
    count3 = 1;
  }
  
  if (countdown_current_time > 1000 && !count2) {
    digitalWrite(c2, HIGH);
    lcd.clear();
    lcd.print("2...");
    count2 = 1;
  }
  
  if (countdown_current_time > 2000 && !count1) {
    digitalWrite(c1, HIGH);
    lcd.clear();
    lcd.print("1...");
    count1 = 1;
  }
  
  if (countdown_current_time > 3000 && !has_race_started) {
    
    has_race_started = 1;
    digitalWrite(go, HIGH);
    lcd.clear();
    lcd.print("GO!!");
    race_start_time = millis();
  }

  race_current_time = (millis() - race_start_time);
  race_current_time_seconds = race_current_time/1000;
  
  if (has_race_started && (analogRead(A0) < 900) && !car1_finished) {
    car1_finish_time = race_current_time;
    car1_finished = 1;
  }
  
  if (has_race_started && (analogRead(A1) < 900) && !car2_finished) {
    car2_finish_time = race_current_time;
    car2_finished = 1;
  }
  
  if ((countdown_current_time % 1000) < 100 && countdown_current_time < 4000) {
    tone(piezo, 1000);
  } else {
    noTone(piezo);
  }
  if (countdown_current_time > 5000 && !count_lights_off) {
    pinsOff(lights);
    count_lights_off = 1;
  }
  
  if (car1_finished && car2_finished) {
    noTone(piezo);
    pinsOff(lights);
    printResult(lcd, car1_finish_time, car2_finish_time);
    while(true){}
  }
  
  if (race_current_time_seconds > last_displayed_time && has_race_started) {
    //Update Display  
    last_displayed_time = race_current_time_seconds;
    lcd.clear();
    lcd.print(race_current_time_seconds);
    printIfOneCarFinished(lcd, car1_finished, car2_finished);
  }
}

void printIfOneCarFinished(LiquidCrystal lcd, int car1, int car2) {
  if (car1) {
    lcd.setCursor(0,1);
    lcd.print("Car 1 Finished");
  }
  if (car2) {
    lcd.setCursor(0,1);
    lcd.print("Car 2 Finished");
  }
}

void printResult(LiquidCrystal lcd, long car1_time, long car2_time) {
  lcd.clear();
  lcd.print("Car 1: ");
  lcd.print(car1_time/1000);
  lcd.print(".");
  lcd.print(car1_time%1000);
  lcd.print("s");
  lcd.setCursor(0, 1);
  lcd.print("Car 2: ");
  lcd.print(car2_time/1000);
  lcd.print(".");
  lcd.print(car2_time%1000);
  lcd.print("s");
}

void readoutMode(LiquidCrystal lcd, int a0, int a1) {
  while(true) {
    lcd.clear();
    lcd.print("Readout Mode");
    lcd.setCursor(0,1);
    lcd.print("A0:");
    lcd.print(a0);
    lcd.print("  ");
    lcd.print("A1:");
    lcd.print(a1);
    delay(1000);
  }
}

int intArrayLength(int intArray[]) {
  return sizeof(intArray)/sizeof(int);
}
void pinsOff(int pins[]) {
  for(int i = 0; i < intArrayLength(pins); i++) {
    digitalWrite(pins[i], LOW);
  }
}

