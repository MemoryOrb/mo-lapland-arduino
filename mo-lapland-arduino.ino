#include <WiFiNINA.h>

/* 
 * We just define the first pin number, all the other button pins follow 
 * the first one, therefore, starting at 4 up to 13 (for 10 buttons).
 */
#define PIN_BUTTON_FIRST  4
#define DEBOUNCE_DELAY    50

#define PIN_ROTARY_LEFT_DT   A3
#define PIN_ROTARY_LEFT_CLK  A4
#define PIN_ROTARY_RIGHT_DT  A6
#define PIN_ROTARY_RIGHT_CLK A5

bool buttonState[10];
bool buttonLastState[10];
unsigned long buttonLastDebounceTime[10];

int rotaryLeftStateCLK;
int rotaryLeftLastStateCLK;
int rotaryRightStateCLK;
int rotaryRightLastStateCLK;

void setup() {
  // Init buttons
  for (int i = 0; i < 10; i++) {
    buttonState[i] = true; // init the state of the button as unpressed (true)
    pinMode(i + PIN_BUTTON_FIRST, INPUT_PULLUP);
  }

  // Init rotary encoders
  pinMode(PIN_ROTARY_LEFT_CLK, INPUT);
  pinMode(PIN_ROTARY_LEFT_DT, INPUT);
  pinMode(PIN_ROTARY_RIGHT_CLK, INPUT);
  pinMode(PIN_ROTARY_RIGHT_DT, INPUT);
  rotaryLeftLastStateCLK = digitalRead(PIN_ROTARY_LEFT_CLK);
  rotaryRightLastStateCLK = digitalRead(PIN_ROTARY_RIGHT_CLK);

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
        
        printFormatedData('B', i+PIN_BUTTON_FIRST, b);
      }
    }
    
    buttonLastState[i] = b;
  }

  // Rotary encoders
  rotaryLeftStateCLK = digitalRead(PIN_ROTARY_LEFT_CLK);
  if (rotaryLeftStateCLK != rotaryLeftLastStateCLK) {
    printFormatedData('R', 0, (digitalRead(PIN_ROTARY_LEFT_DT) != rotaryLeftStateCLK)); 
  }
  rotaryLeftLastStateCLK = rotaryLeftStateCLK;

  rotaryRightStateCLK = digitalRead(PIN_ROTARY_RIGHT_CLK);
  if (rotaryRightStateCLK != rotaryRightLastStateCLK) {
    printFormatedData('R', 1, (digitalRead(PIN_ROTARY_RIGHT_DT) != rotaryRightStateCLK)); 
  }
  rotaryRightLastStateCLK = rotaryRightStateCLK;
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
