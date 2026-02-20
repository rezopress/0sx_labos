const int ETD_ID = 6357133;
const int LED[] = {7, 8, 9, 10};
const int NB_LEDS = 4;
const int BTN_PIN = 2;
const int POT_PIN = A1;
const bool IS_EVEN = (ETD_ID % 2 == 0);

int btnState = HIGH;
int btnPrevious = HIGH;
int lastReading = HIGH;

int value = 0;
int mappedValue = 0;

int currentRange = -1;
int previousRange = -1;

unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 100;


void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NB_LEDS; i++) { pinMode(LED[i], OUTPUT); }
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(POT_PIN, INPUT_PULLUP);
}

void choice() {
  value = analogRead(POT_PIN);
  mappedValue = map(value, 0, 1023, 0, 20);

  // Debounce du bouton
  int reading = digitalRead(BTN_PIN);
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    btnPrevious = btnState;
    btnState = reading;
  }
  lastReading = reading;

  // Plage actuelle
  if (mappedValue <= 5) {
    currentRange = 0;
  } else if (mappedValue <= 10) {
    currentRange = 1;
  } else if (mappedValue <= 15) {
    currentRange = 2;
  } else {
    currentRange = 3;
  }

  if (ETD_ID % 2 == 0) { idIsEven(); }
  else { idIsOdd(); }
}

void idIsEven() {
  if (btnState == LOW) {
    if (btnPrevious == HIGH || currentRange != previousRange) {
      switch (currentRange) {
        case 0: Serial.println("[!!!!!...............] 25%");  break;
        case 1: Serial.println("[!!!!!!!!!!..........] 50%");  break;
        case 2: Serial.println("[!!!!!!!!!!!!!!!.....] 75%");  break;
        case 3: Serial.println("[!!!!!!!!!!!!!!!!!!!!] 100%"); break;
      }
      previousRange = currentRange;
    }
  } else {
    currentRange = -1;
  }

  if (mappedValue <= 5) {
    digitalWrite(LED[0], HIGH);
    digitalWrite(LED[1], LOW);
    digitalWrite(LED[2], LOW);
    digitalWrite(LED[3], LOW);
  } else if (mappedValue <= 10) {
    digitalWrite(LED[0], LOW);
    digitalWrite(LED[1], HIGH);
    digitalWrite(LED[2], LOW);
    digitalWrite(LED[3], LOW);
  } else if (mappedValue <= 15) {
    digitalWrite(LED[0], LOW);
    digitalWrite(LED[1], LOW);
    digitalWrite(LED[2], HIGH);
    digitalWrite(LED[3], LOW);
  } else {
    digitalWrite(LED[0], LOW);
    digitalWrite(LED[1], LOW);
    digitalWrite(LED[2], LOW);
    digitalWrite(LED[3], HIGH);
  }
}

void idIsOdd() {
  if (btnState == LOW) {
    if (btnPrevious == HIGH || currentRange != previousRange) {
      switch (currentRange) {
        case 0: Serial.println("25%  [-----................]");  break;
        case 1: Serial.println("50%  [-----------..........]");  break;
        case 2: Serial.println("75%  [----------------.....]");  break;
        case 3: Serial.println("100% [---------------------]");  break;
      }
      previousRange = currentRange;
    }
  } else {
    previousRange = -1;
  }

  if (mappedValue <= 5) {
    digitalWrite(LED[0], HIGH);
    digitalWrite(LED[1], LOW);
    digitalWrite(LED[2], LOW);
    digitalWrite(LED[3], LOW);
  } else if (mappedValue <= 10) {
    digitalWrite(LED[0], HIGH);
    digitalWrite(LED[1], HIGH);
    digitalWrite(LED[2], LOW);
    digitalWrite(LED[3], LOW);
  } else if (mappedValue <= 15) {
    digitalWrite(LED[0], HIGH);
    digitalWrite(LED[1], HIGH);
    digitalWrite(LED[2], HIGH);
    digitalWrite(LED[3], LOW);
  } else {
    digitalWrite(LED[0], HIGH);
    digitalWrite(LED[1], HIGH);
    digitalWrite(LED[2], HIGH);
    digitalWrite(LED[3], HIGH);
  }
}

void loop() {
  choice();
}