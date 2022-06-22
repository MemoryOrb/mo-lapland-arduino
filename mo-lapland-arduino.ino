#define DEBOUNCE_DELAY 50

/*
  We just define the first pin number, all the other button pins follow the first one
*/
#define PIN_BUTTON_FIRST 2

/*
  We just define the pins for one rotary encoder and increment (+1) for the second,
*/
#define PIN_ROTARY_CLK 16 // A2
#define PIN_ROTARY_DT  18 // A4
#define PIN_ROTARY_SW1 0
#define PIN_ROTARY_SW2 12

/*
  Same as rotary encoder, we just define one potentiometer and increment (+1) for
  the other, therefore, 14 (A0) will be 15 (A1)
*/
#define PIN_POTENTIOMETER 14 // A0

/*
  12, because 10 individual buttons (momentary switches) and 2 integrated
  switches with the rotary encoders.
*/
bool buttonState[12];
bool buttonLastState[12];
unsigned long buttonLastDebounceTime[12];

int rotaryLastStateCLK[2];
int potentiometerLastState[2];

/*
  Use to stabilize values. Data is printed/sent only if the change (increase or decrease)
  exceeds the threshold. The constant 8 has been defined empirically with the current hardware.
*/
const int potentiometerValueThreshold = 8;

char buf[8];

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
    pinMode(PIN_ROTARY_CLK + i, INPUT);
    pinMode(PIN_ROTARY_DT + i, INPUT);
    rotaryLastStateCLK[i] = digitalRead(PIN_ROTARY_CLK + i);
  }
  pinMode(PIN_ROTARY_SW1, INPUT_PULLUP);
  buttonState[10] = digitalRead(PIN_ROTARY_SW1);
  pinMode(PIN_ROTARY_SW2, INPUT_PULLUP);
  buttonState[11] = digitalRead(PIN_ROTARY_SW2);

  /*
  ** Init potentiometers
  */
  for (int i = 0; i < 2; i++) {
    pinMode(PIN_POTENTIOMETER + i, INPUT);
    potentiometerLastState[i] = analogRead(PIN_POTENTIOMETER + i) * 100 / 1024;
  }

  /*
  ** Init serial
  */
  Serial.begin(9600);
}

void loop() {
  /*
  ** Buttons
  */
  for (int i = 0; i < 10; i++) {
    checkButton(i + PIN_BUTTON_FIRST, i);
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
  }

  /*
  ** Potentiometers
  */
  for (int i = 0; i < 2; i++) {
    int v = analogRead(PIN_POTENTIOMETER + i);
    if ((v - potentiometerLastState[i]) >= potentiometerValueThreshold || (v - potentiometerLastState[i]) <= -potentiometerValueThreshold) {
      printFormatedData('P', i, v);
      potentiometerLastState[i] = v;
    }
  }
  checkButton(PIN_ROTARY_SW1, 10);
  checkButton(PIN_ROTARY_SW2, 11);
}

void checkButton(int pin, int index) {
  bool b = digitalRead(pin);
  if (b != buttonLastState[index]) {
    buttonLastDebounceTime[index] = millis();
  }
  if ((millis() - buttonLastDebounceTime[index]) > DEBOUNCE_DELAY) {
    if (b != buttonState[index]) {
      buttonState[index] = b;
      printFormatedData('B', index, b);
    }
  }
  buttonLastState[index] = b;
}

void printFormatedData(char componentCode, int componentNumber, bool value) {
  printFormatedDataPrefix(componentCode, componentNumber);

  Serial.print(value);
  Serial.println(";");
}

void printFormatedData(char componentCode, int componentNumber, int value) {
  printFormatedDataPrefix(componentCode, componentNumber);

  Serial.print(value);
  Serial.println(";");
}

void printFormatedDataPrefix(char componentCode, int componentNumber) {
  Serial.print(componentCode);
  Serial.print(":");
  Serial.print(itoa(componentNumber, buf, 10));
  Serial.print(":");
}
