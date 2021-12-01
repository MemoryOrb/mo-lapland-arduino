#include <WiFiNINA.h>
#include "credentials.h"

#define DEBOUNCE_DELAY 50

/*
  We just define the first pin number, all the other button pins follow
  the first one, therefore, starting at 4 up to 13 (for 10 buttons).
*/
#define PIN_BUTTON_FIRST 4

/*
  We just define the pins for one rotary encoder and increment (+1) for the second,
  therefore, 0, A2 and A4 will be 1, A3 and A5
*/
#define PIN_ROTARY_CLK 16 // A2
#define PIN_ROTARY_DT  18 // A4
#define PIN_ROTARY_SW  0 // TX

/*
  Same as rotary encoder, we just define one potentiometer and increment (+1) for
  the other, therefore, 14 (A0) will be 15 (A1)
*/
#define PIN_POTENTIOMETER 14 // A0

/*
  Same as the potentiometer, we just define one vibrating motor and incremet (+1) for the other
*/
#define PIN_VIBRATING_MOTOR 2

/*
  12, because 10 individual buttons (momentary switches) and 2 integrated
  switches with the rotary encoders.
*/
bool buttonState[12];
bool buttonLastState[12];
unsigned long buttonLastDebounceTime[12];


int rotaryLastStateCLK[2];
int potentiometerLastState[2];

unsigned long vibratingMotorLastActivation[2];
const float vibratingMaxPeriod = 5000.0f; // 5s

/*
  Network, define custom SECRET_SSID, SECRET_PASS, IP1, IP2, IP3 and IP4 in credentials.h
*/
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
IPAddress ipRemote(IP1, IP2, IP3, IP4);

unsigned int localPort = 55666;
int status = WL_IDLE_STATUS;
WiFiClient client;

String dataSend = String("");
String dataReceived = String("");


void setup() {
  /*
  ** Init buttons
  */
  // we stop at 9, because 10 and 11 are reserved for the buttons for the rotary encoders
  for (int i = 0; i < 10; i++) {
    pinMode(PIN_BUTTON_FIRST + i, INPUT_PULLUP);
    buttonState[i] = digitalRead(PIN_BUTTON_FIRST + i);
  }

  /*
  ** Init rotary encoders
  */
  for (int i = 0; i < 2; i++) {
    pinMode(PIN_ROTARY_SW + i, INPUT_PULLUP);
    pinMode(PIN_ROTARY_CLK + i, INPUT);
    pinMode(PIN_ROTARY_DT + i, INPUT);
    rotaryLastStateCLK[i] = digitalRead(PIN_ROTARY_CLK + i);
    buttonState[i + 10] = digitalRead(PIN_ROTARY_SW + i);
  }

  /*
  ** Init potentiometers
  */
  for (int i = 0; i < 2; i++) {
    pinMode(PIN_POTENTIOMETER + i, INPUT);
    potentiometerLastState[i] = analogRead(PIN_POTENTIOMETER + i) * 100 / 1024;
  }

  /*
  ** Init vibrating motors
  */
  for (int i = 0; i < 2; i++) {
    pinMode(PIN_VIBRATING_MOTOR + i, OUTPUT);
  }

  /*
  ** Init serial
  */
  Serial.begin(9600);

  /*
  ** Init network
  */
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 5 seconds for connection:
    delay(5000);
  }

  // print WiFi's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (client.connect(ipRemote, localPort)) {
    dataSend = "c:" + ipToString(WiFi.localIP()) + ";\r";
    client.print(dataSend);
    Serial.println("Connected");
  }
}

void loop() {
  /*
  ** Buttons
  */
  for (int i = 0; i < 10; i++) {
    bool b = digitalRead(i + PIN_BUTTON_FIRST);
    if (b != buttonLastState[i]) {
      buttonLastDebounceTime[i] = millis();
    }
    if ((millis() - buttonLastDebounceTime[i]) > DEBOUNCE_DELAY) {
      if (b != buttonState[i]) {
        buttonState[i] = b;
        printFormatedData('B', i, b);
      }
    }
    buttonLastState[i] = b;
  }

  /*
  ** Rotary encoders
  */
  for (int i = 0; i < 2; i++) {
    bool b = digitalRead(PIN_ROTARY_CLK + i);
    if (b != rotaryLastStateCLK[i]) {
      printFormatedData('R', i, (digitalRead(PIN_ROTARY_DT + i) != b));
    }
    rotaryLastStateCLK[i] = b;

    // integrated button
    bool b2 = digitalRead(PIN_ROTARY_SW + i);
    if (b2 != buttonLastState[i + 10]) {
      buttonLastDebounceTime[i + 10] = millis();
    }
    if ((millis() - buttonLastDebounceTime[i + 10]) > DEBOUNCE_DELAY) {
      if (b2 != buttonState[i + 10]) {
        buttonState[i + 10] = b2;
        printFormatedData('B', i + 10, b2);
      }
    }
    buttonLastState[i + 10] = b2;
  }

  /*
  ** Potentiometers
  */
  for (int i = 0; i < 2; i++) {
    int v = analogRead(PIN_POTENTIOMETER + i) * 100 / 1024;
    if (v != potentiometerLastState[i]) {
      printFormatedData('P', i, v);
    }
    potentiometerLastState[i] = v;
  }

  /*
  ** Vibrating motors
  */
  if (client.available()) {
    char c;
    do {
      c = client.read();
      dataReceived += c;
      if (c == ';') {
        if (dataReceived.charAt(0) == 'V') {
          int i = dataReceived.substring(2, 3).toInt();
          vibratingMotorLastActivation[i] = millis();
          int v = dataReceived.substring(4).toInt();
          digitalWrite(PIN_VIBRATING_MOTOR + i, v);
        }
        Serial.println(dataReceived);
        dataReceived = "";
      }
    } while (c != ';');
  }
  for (int i = 0; i < 2; i++) {
    if ((millis() - vibratingMotorLastActivation[i]) > vibratingMaxPeriod) {
      digitalWrite(PIN_VIBRATING_MOTOR + i, LOW);
    }
  }
}

void printFormatedData(char componentCode, int componentNumber, bool value) {
  printFormatedDataPrefix(componentCode, componentNumber);
  
  Serial.print(value);
  Serial.println(";");
  
  dataSend.concat(value);
  dataSend.concat(";\r");
  client.print(dataSend);
}

void printFormatedData(char componentCode, int componentNumber, int value) {
  printFormatedDataPrefix(componentCode, componentNumber);
  
  Serial.print(value);
  Serial.println(";");
  
  dataSend.concat(value);
  dataSend.concat(";\r");
  client.print(dataSend);
}

void printFormatedDataPrefix(char componentCode, int componentNumber) {
  Serial.print(componentCode);
  Serial.print(":");
  Serial.print(componentNumber);
  Serial.print(":");

  dataSend = "";
  dataSend.concat(componentCode);
  dataSend.concat(":");
  dataSend.concat(componentNumber);
  dataSend.concat(":");
}

String ipToString(const IPAddress& address) {
  return String(address[0]) + "." + address[1] + "." + address[2] + "." + address[3];
}
