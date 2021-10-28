#include <WiFiNINA.h>

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
   10 individual buttons (momentary switches) and 2 integrated with the rotary encoders.
*/
bool buttonState[12];
bool buttonLastState[12];
unsigned long buttonLastDebounceTime[12];

int rotaryLastStateCLK[2];

void setup() {
  // Init buttons
  // we stop at 9, because 10 and 11 are reserved for the buttons for the rotary encoders
  for (int i = 0; i < 10; i++) {
    buttonState[i] = true; // init the state of the button as unpressed (true)
    pinMode(i + PIN_BUTTON_FIRST, INPUT_PULLUP);
  }

  // Init rotary encoders
  for (int i = 0; i < 2; i++) {
    pinMode(PIN_ROTARY_SW + i, INPUT_PULLUP);
    pinMode(PIN_ROTARY_CLK + i, INPUT);
    pinMode(PIN_ROTARY_DT + i, INPUT);
    rotaryLastStateCLK[i] = digitalRead(PIN_ROTARY_CLK + i);
  }

  // Init serial
  Serial.begin(9600);
}

void loop() {
  // Buttons
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

  // Rotary encoders
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
}

void printFormatedData(char componentCode, int componentNumber, bool value) {
  printFormatedDataPrefix(componentCode, componentNumber);
  Serial.print(value);
  Serial.println(";");
}

void printFormatedDataPrefix(char componentCode, int componentNumber) {
  Serial.print(componentCode);
  Serial.print(":");
  Serial.print(componentNumber);
  Serial.print(":");
}
