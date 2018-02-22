//
// Copyright 2015 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// FirebaseDemo_ESP32 is a sample that demo the different functions
// of the FirebaseArduino API.

#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ESP32_Servo.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'a'},
  {'4', '5', '6', 'b'},
  {'7', '8', '9', 'c'},
  {'*', '0', '#', 'd'}

};
String uid = "";
String uid2 = "";
boolean detect = false;
int type = 0;
int can = 0, bottle = 0;
int modeUID = 0;
int modeCounter = 1;
int modePush = 0;
int modeSelectCoin = 0;
int CouponValue = 1;
float bottleValue = 0.03;
float canValue = 0.56;
int left = 155, right = 10, mid = 60;
int servoPin = 2;
byte rowPins[ROWS] = {13, 12, 14, 27}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 25, 33, 32}; //connect to the column pinouts of the keypad
Servo myservo;
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
int cur = 0;
// Set these to run example.
#define FIREBASE_HOST "https://hackathon-8f517.firebaseio.com"
#define FIREBASE_AUTH "WIZM7kbUtJcFSKh4uqyDQyJcrKVtAtq1ieTgidZq"
#define WIFI_SSID ".@SUT_4_1"
#define WIFI_PASSWORD ""
boolean statusAuth = false;
void setup() {
  myservo.attach(servoPin);
  Serial.begin(9600);
  pinMode(A0, INPUT);
  // pinMode(19, OUTPUT);
  digitalWrite(19, 0);
  lcd.begin(21, 22);
  lcd.backlight();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  lcd.setCursor(0, 0);
  lcd.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    lcd.print(".");
    delay(500);
  }
  Serial.println();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected!!");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--------ready------- ");
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  myservo.write(mid);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  keypad.addEventListener(keypadEvent);
}
int n = 0;
void keypadEvent(KeypadEvent key) {
  switch (keypad.getState()) {
    case PRESSED:
      if (key == '#') {
        Serial.println("#");
        //  digitalWrite(ledPin,!digitalRead(ledPin));
        //  ledPin_state = digitalRead(ledPin);        // Remember LED state, lit or unlit.
      }
      break;

    case RELEASED:
      if (key == '*') {
        if (!modeUID) {
          //mode uid
          modeCounter = 0;
          lcd.clear();
          lcd.print("UID:");
          uid = "";
          lcd.setCursor(0, 3);
          lcd.print("Finish * ,Clear #");
          lcd.setCursor(4, 0);
          modeUID = 1;
        }
        else {

          //next from uid
          modeUID = 0;
          lcd.clear();
          Serial.println(uid);
          modePush = 1;
          //modeCounter = 1;
        }
      } else if (key == '#' && modeUID == 1) {
        //  modeUID = 0;
        // modeCounter = 1;
        //clear uid
        uid = "";
        lcd.clear();
        lcd.print("UID:");
        lcd.setCursor(0, 3);
        lcd.print("Finish * ,Clear #");
        lcd.setCursor(4, 0);
      } else if (key == '1' && modeSelectCoin == 1 || key == '2' && modeSelectCoin == 1) {
        Serial.println("select mode");
        if (key == '1') { //coupon
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Please wait..");
          float getCoupon =  Firebase.getFloat("coupon/" + uid2 + "/coupon");
          Firebase.setFloat("coupon/" + uid2 + "/coupon", getCoupon + can + bottle);
          if (Firebase.failed()) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("error");
            return;
          }
          lcd.clear();
          lcd.print("Success!!");

          lcd.setCursor(0, 1);
          float getCoupon2 =  Firebase.getFloat("coupon/" + uid2 + "/coupon");
          lcd.print("Your coupon : ");
          lcd.print(getCoupon2);
          delay(2000);
          lcd.clear();
          lcd.setCursor(0, 0);
          modeUID = 0;
          modeCounter = 1;
          modePush = 0;
          modeSelectCoin = 0;
          lcd.print("--------ready------- ");
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Please wait..");
          float getCoupon =  Firebase.getFloat("coupon/" + uid2 + "/coin");
          Firebase.setFloat("coupon/" + uid2 + "/coin", getCoupon + (can * canValue + bottle * bottleValue));
          if (Firebase.failed()) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("error");
            return;
          }
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("success");

          lcd.setCursor(0, 1);
          float getCoupon2 =  Firebase.getFloat("coupon/" + uid2 + "/coin");
          lcd.print("Your coin : ");
          lcd.print(getCoupon2);
          delay(2000);
          lcd.clear();
          lcd.setCursor(0, 0);
          modeUID = 0;
          modeCounter = 1;
          modePush = 0;
          modeSelectCoin = 0;

          lcd.print("--------ready------- ");
        }
        uid = "";
        can = 0;
        bottle = 0;
      }
      else {
        uid += key;
        Serial.println(uid);
      }
      break;

    case HOLD:
      if (key == '*') {
        //  blink = true;    // Blink the LED when holding the * key.
        Serial.println("*");
      } else if (key == '#') {
        lcd.clear();
        modeCounter = 1;
        modeUID = 0;
        //back
      }
      break;
  }
}
void loop() {
  // lcd.print("Status:ok");
  /*String data = "abctest123";
    myservo.write(100);
    delay(1000);
    myservo.write(20);
    delay(1000);*/
  /* Firebase.setFloat(data + "/value", 42.0);
    if (!Firebase.failed() && statusAuth == false) {
     statusAuth = true;
     lcd.clear();
     lcd.setCursor(0, 0);
     Serial.print("Success!!");
     lcd.print("Success!!");
     // Serial.println(Firebase.error());
    }*/

  char key = keypad.getKey();
  int sensor = analogRead(A0);
  if (modeSelectCoin) {
    lcd.setCursor(0, 0);
    lcd.print("Please Select...");
    lcd.setCursor(0, 1);
    lcd.print("[1]Coupon :");
    lcd.setCursor(11, 1);
    lcd.print(can + bottle);
    lcd.setCursor(0, 2);
    lcd.print("[2]Coins :");
    lcd.setCursor(11, 2);
    lcd.print(bottle * bottleValue + can * canValue);

  }
  if (modePush) {

    lcd.setCursor(0, 0);
    lcd.print("Your ID:");
    lcd.setCursor(8, 0);
    lcd.print(uid);
    Serial.print("number: ");
    lcd.setCursor(0, 1);
    if (Firebase.getFloat("coupon/" + uid + "/status") == 1.00) { //check uid
      lcd.print("UID Correct"); // have row
      uid2 = uid;
      delay(2000);
      modePush = 0;
      modeSelectCoin = 1;
      lcd.clear();
    } else {
      lcd.print("UID Incorrect");
      delay(2000);
      modePush = 0;

      //mode uid
      modeCounter = 0;
      lcd.clear();
      lcd.print("UID:");
      uid = "";
      lcd.setCursor(0, 3);
      lcd.print("Finish * ,Clear #");
      lcd.setCursor(4, 0);
      modeUID = 1;
    }
    //delay(1000);

  }
  Serial.println(sensor);
  // digitalWrite(19, 1);
  if (sensor > 10) {
    detect = true;
  }

  if (detect) {
    delay(500);
    if (analogRead(A0) > 2500) {
      type = 1; //ป๋อง
    } else {
      type = 2; //ขวด = 0 or <3500

    }

  }
  if (modeUID) {
    if (key) {
      lcd.print(key);
    }
  }


  if (type == 1) {
    Serial.println("ป๋อง");
    can++;
    myservo.write(right);
    delay(1000);
    myservo.write(mid);
    delay(1000);
    type = 0;
    detect = false;
    lcd.clear();
    if (modeCounter) {
      lcd.setCursor(0, 0);
      lcd.print("Can:");
      lcd.print(can);
      lcd.setCursor(0, 1);
      lcd.print("Bottle:");
      lcd.print(bottle);
      lcd.setCursor(0, 3);
      lcd.print("Finish Press Key '*'");
    }
  } else if (type == 2) {
    Serial.println("ขวด");
    bottle++;
    myservo.write(left);
    delay(1000);
    myservo.write(mid);
    delay(1000);
    type = 0;
    detect = false;
    lcd.clear();
    if (modeCounter) {
      lcd.setCursor(0, 0);
      lcd.print("Can:");
      lcd.print(can);
      lcd.setCursor(0, 1);
      lcd.print("Bottle:");
      lcd.print(bottle);
      lcd.setCursor(0, 3);
      lcd.print("Finish Press Key '*'");
    }
  }
  /*  if (statusAuth) {
      // Serial.print("\r\n aaaaaaaaaaaaaaaaaaaa \r\n");
    }*/
}
