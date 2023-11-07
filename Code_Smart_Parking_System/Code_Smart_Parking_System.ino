#include <FirebaseESP32.h>
#include  <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

#include "RTClib.h"

RTC_DS3231 rtc;


#define FIREBASE_HOST "smart-parking-system-acf8a-default-rtdb.firebaseio.com/"
#define WIFI_SSID "iPhone"
#define WIFI_PASSWORD "12345678"
#define FIREBASE_Authorization_key "um1a4J0tPZjOCsS7TUviCyjDwxwxhBmuoFhHnhOw"

FirebaseData firebaseData;
FirebaseJson json;

unsigned long previousMillis = 0;
const long interval = 200;

Servo myservo;

bool servoClosed = false;
bool servoJustOpened = false;

LiquidCrystal_I2C lcd1(0x27, 16, 2);
LiquidCrystal_I2C lcd2(0x68, 16, 2);

const int ledPins[] = {12, 14, 27, 26};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

const int trigPin1 = 15;
const int echoPin1 = 23;
const int trigPin2 = 18;
const int echoPin2 = 5;
const int trigPin3 = 4;
const int echoPin3 = 2;
const int trigPin4 = 25;
const int echoPin4 = 33;

int threshold = 10;
int openAngle = 0;
int closeAngle = 90;
int pos;

bool slot1;
bool slot2;
bool slot3;
bool slot4;

bool gate;

int buzzerPin = 32;
bool buzzerRinging = false;

int servoPin = 13;


int i = 0;
int count;
int buttonPin = 36;
int irPin =  39;

int buttonState ;
boolean irState = HIGH;

void setup() {
  Serial.begin(9600);

  WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_Authorization_key);

  lcd1.init();
  lcd1.backlight();

  lcd2.init();
  lcd2.backlight();

  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT);

  pinMode(buzzerPin, OUTPUT);


  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
  myservo.write(openAngle);
  delay(300);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  pinMode(buttonPin, INPUT);
  pinMode(irPin,  INPUT);


}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void loop() {

  unsigned long currentMillis = millis();

  long distance1 = getDistance(trigPin1, echoPin1);
  long distance2 = getDistance(trigPin2, echoPin2);
  long distance3 = getDistance(trigPin3, echoPin3);
  long distance4 = getDistance(trigPin4, echoPin4);


  DateTime now = rtc.now();



  count = now.second();





  if ( (distance1 > threshold) && (distance2 > threshold) && (distance3 > threshold) && (distance4 > threshold) ) {
    slot1 = 0;
    slot2 = 0;
    slot3 = 0;
    slot4 = 0;

    gate = 0;
  }

  if ( (distance1 < threshold) && (distance2 > threshold) && (distance3 > threshold) && (distance4 > threshold) ) {
    slot1 = 1;
    slot2 = 0;
    slot3 = 0;
    slot4 = 0;

    gate = 0;
  }

  if ( (distance1 > threshold) && (distance2 < threshold) && (distance3 > threshold) && (distance4 > threshold) ) {
    slot1 = 0;
    slot2 = 1;
    slot3 = 0;
    slot4 = 0;

    gate = 0;
  }

  if ( (distance1 > threshold) && (distance2 > threshold) && (distance3 < threshold) && (distance4 > threshold) ) {
    slot1 = 0;
    slot2 = 0;
    slot3 = 1;
    slot4 = 0;

    gate = 0;
  }

  if ( (distance1 > threshold) && (distance2 > threshold) && (distance3 > threshold) && (distance4 < threshold) ) {
    slot1 = 0;
    slot2 = 0;
    slot3 = 0;
    slot4 = 1;

    gate = 0;
  }

  if ( (distance1 < threshold) && (distance2 < threshold) && (distance3 < threshold) && (distance4 < threshold) ) {
    slot1 = 1;
    slot2 = 1;
    slot3 = 1;
    slot4 = 1;



    gate = 1;
  }

  if ( (distance1 < threshold) && (distance2 < threshold) && (distance3 > threshold) && (distance4 > threshold) ) {
    slot1 = 1;
    slot2 = 1;
    slot3 = 0;
    slot4 = 0;

    gate = 0;
  }

  if ( (distance1 > threshold) && (distance2 < threshold) && (distance3 < threshold) && (distance4 > threshold) ) {
    slot1 = 0;
    slot2 = 1;
    slot3 = 1;
    slot4 = 0;

    gate = 0;
  }

  if ( (distance1 > threshold) && (distance2 > threshold) && (distance3 < threshold) && (distance4 < threshold) ) {
    slot1 = 0;
    slot2 = 0;
    slot3 = 1;
    slot4 = 1;

    gate = 0;
  }

  if ( (distance1 < threshold) && (distance2 > threshold) && (distance3 < threshold) && (distance4 > threshold) ) {
    slot1 = 1;
    slot2 = 0;
    slot3 = 1;
    slot4 = 0;

    gate = 0;
  }

  if ( (distance1 < threshold) && (distance2 > threshold) && (distance3 > threshold) && (distance4 < threshold) ) {
    slot1 = 1;
    slot2 = 0;
    slot3 = 0;
    slot4 = 1;

    gate = 0;
  }

  if ( (distance1 > threshold) && (distance2 < threshold) && (distance3 > threshold) && (distance4 < threshold) ) {
    slot1 = 0;
    slot2 = 1;
    slot3 = 0;
    slot4 = 1;

    gate = 0;
  }

  if ( (distance1 > threshold) && (distance2 < threshold) && (distance3 < threshold) && (distance4 < threshold) ) {
    slot1 = 0;
    slot2 = 1;
    slot3 = 1;
    slot4 = 1;

    gate = 0;
  }

  if ( (distance1 < threshold) && (distance2 > threshold) && (distance3 < threshold) && (distance4 < threshold) ) {
    slot1 = 1;
    slot2 = 0;
    slot3 = 1;
    slot4 = 1;

    gate = 0;
  }

  if ( (distance1 < threshold) && (distance2 < threshold) && (distance3 < threshold) && (distance4 > threshold) ) {
    slot1 = 1;
    slot2 = 1;
    slot3 = 1;
    slot4 = 0;

    gate = 0;
  }

  if ( (distance1 < threshold) && (distance2 < threshold) && (distance3 > threshold) && (distance4 < threshold) ) {
    slot1 = 1;
    slot2 = 1;
    slot3 = 0;
    slot4 = 1;

    gate = 0;
  }

  if (gate == 0) {
    if (buzzerRinging) {
      noTone(buzzerPin);
      buzzerRinging = false;
    }

    if (servoClosed) {
      myservo.write(openAngle);
      servoClosed = false;
    }
  } else {
    if (!buzzerRinging) {
      for (int i = 1; i <= 3; i++) {
        tone(buzzerPin, 900);
        delay(100);
        noTone(buzzerPin);
        delay(100);
      }
      buzzerRinging = true;
    }

    if (!servoClosed) {
      myservo.write(closeAngle);
      servoClosed = true;
    }
  }


  digitalWrite(ledPins[0], slot1);
  digitalWrite(ledPins[1], slot2);
  digitalWrite(ledPins[2], slot3);
  digitalWrite(ledPins[3], slot4);


  Serial.print("Slot 1 : ");
  Serial.print(slot1);
  Serial.print("\t");

  Serial.print("Slot 2: ");
  Serial.print(slot2);
  Serial.print("\t");

  Serial.print("Slot 3: ");
  Serial.print(slot3);
  Serial.print("\t");

  Serial.print("Slot 4: ");
  Serial.print(slot4);
  Serial.print("\t");

  Serial.print("Seconds : ");
  Serial.print(count);
  Serial.print("\t");
  Serial.print("Vehicle Count: ");
  Serial.println(i);

  buttonState = digitalRead(buttonPin);
  if ( buttonState == 1) {

    if (count == 0) {
      i = 0;
    }

    else {

      if (debounceButton(irState) == LOW && irState == HIGH)
      {
        i++;
        irState = LOW;
      }
      else if (debounceButton(irState) == HIGH && irState == LOW)
      {
        irState = HIGH;
      }

    }

    lcd1.setCursor(0, 0);
    lcd1.print("Slot1:");
    lcd1.print(slot1);

    lcd1.setCursor(0, 1);
    lcd1.print("Slot2:");
    lcd1.print(slot2);

    lcd1.setCursor(9, 0);
    lcd1.print("Slot3:");
    lcd1.print(slot3);

    lcd1.setCursor(9, 1);
    lcd1.print("Slot4:");
    lcd1.print(slot4);

    lcd2.setCursor(1, 0);
    lcd2.print("Vehicle Count:");
    lcd2.setCursor(6, 1);
    lcd2.print(i);

  }

  else {

    if (count == 0) {
      i = 0;
    }

    else {

      if (debounceButton(irState) == LOW && irState == HIGH)
      {
        i++;
        irState = LOW;
      }
      else if (debounceButton(irState) == HIGH && irState == LOW)
      {
        irState = HIGH;
      }

    }

    lcd1.setCursor(0, 0);
    lcd1.print("Slot1:");
    lcd1.print(slot1);

    lcd1.setCursor(0, 1);
    lcd1.print("Slot2:");
    lcd1.print(slot2);

    lcd1.setCursor(9, 0);
    lcd1.print("Slot3:");
    lcd1.print(slot3);

    lcd1.setCursor(9, 1);
    lcd1.print("Slot4:");
    lcd1.print(slot4);

    lcd2.setCursor(1, 0);
    lcd2.print("Vehicle Count:");
    lcd2.setCursor(6, 1);
    lcd2.print(i);

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      
            Firebase.setFloat(firebaseData, "/Parking_Slots/Slot_01", slot1);
            Firebase.setFloat(firebaseData, "/Parking_Slots/Slot_02", slot2);
            Firebase.setFloat(firebaseData, "/Parking_Slots/Slot_03", slot3);
            Firebase.setFloat(firebaseData, "/Parking_Slots/Slot_04", slot4);

            Firebase.setFloat(firebaseData, "/Gate_Status/Gate_01", gate);
            Firebase.setFloat(firebaseData, "/Gate_Status/Gate_02", gate);

             Firebase.setFloat(firebaseData, "/Vehicle_Count/Count", i);

      

    }
  }

}

boolean debounceButton(boolean state)
{
  boolean stateNow = digitalRead(irPin);
  if (state != stateNow)
  {
    delay(10);
    stateNow = digitalRead(irPin);
  }
  return stateNow;

}
