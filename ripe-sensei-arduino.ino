#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <Servo.h>


Servo m1;
Servo servo;
#define esc_pin D1                


#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define API_KEY ""
#define DATABASE_URL ""

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

unsigned long sendDataPrevMillis = 0;

void setup(){
  m1.attach(esc_pin);
  m1.writeMicroseconds(1500);
  servo.attach(D2);
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase connection successful");
    signupOK = true;
  } else {
    Serial.printf("Firebase sign-up error: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    
    if (Firebase.RTDB.getInt(&fbdo, "Controls/esc")) {
      if (fbdo.dataType() == "int"){
        int escValue = fbdo.intData();
        escValue = constrain(escValue, 1000, 2000);
        m1.writeMicroseconds(escValue);
      }
      
    } else {
      Serial.println("Failed to read Auto: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.getInt(&fbdo, "Controls/wheel")) {
      if (fbdo.dataType() == "int"){
        int wheelValue = fbdo.intData();
        wheelValue = constrain(wheelValue, 0, 180);
        servo.write(wheelValue);
      }
      
    } else {
      Serial.println("Failed to read Auto: " + fbdo.errorReason());
    }

  }


}



