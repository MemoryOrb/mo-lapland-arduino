/* 
 * We just define the first pin number, all the other button pins follow 
 * the first one, therefore, starting at 4 up to 13 (for 10 buttons).
 */
#define PIN_BUTTON_FIRST 4
#define DEBOUNCE_DELAY 50

bool buttonState[10];
bool buttonLastState[10];
unsigned long buttonLastDebounceTime[10];

void setup() {
  for (int i = 0; i < 10; i++) {
    buttonState[i] = true; // init the state of the button as unpressed (true)
    pinMode(i + PIN_BUTTON_FIRST, INPUT_PULLUP);
  }

  Serial.begin(9600);
}

void loop() {
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
