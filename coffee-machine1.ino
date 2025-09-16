// LED pins
const int yellowLEDs[] = {2, 3, 4, 5};
const int redLEDs[]    = {6, 7, 8, 9};
const int blueLEDs[]   = {10, 11, 12, 13};

// Button pins (use internal pull-ups)
const int yellowButton = A0;
const int redButton    = A1;
const int blueButton   = A2;

// Potentiometer pin
const int potPin = A3;

// Animation variables
bool animating = false;
int column = -1;
int step = 0;
unsigned long lastTime = 0;
const int stepDelay = 200; // ms per animation step

// Coffee ready blink variables
bool coffeeReadyBlink = false;
int blinkStep = 0;
unsigned long lastBlinkTime = 0;
const int blinkDelay = 200; // ms per blink
const int totalBlinks = 3;

// Debounce variables per button
bool lastYellowState = HIGH;
bool lastRedState    = HIGH;
bool lastBlueState   = HIGH;
unsigned long lastYellowDebounce = 0;
unsigned long lastRedDebounce    = 0;
unsigned long lastBlueDebounce   = 0;
const unsigned long debounceDelay = 50; // ms

void setup() {
  Serial.begin(9600);

  // Set LED pins as OUTPUT
  for (int i = 0; i < 4; i++) {
    pinMode(yellowLEDs[i], OUTPUT);
    pinMode(redLEDs[i], OUTPUT);
    pinMode(blueLEDs[i], OUTPUT);
  }

  // Set button pins as INPUT_PULLUP
  pinMode(yellowButton, INPUT_PULLUP);
  pinMode(redButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
}

void loop() {
  unsigned long now = millis();

  // Check buttons with robust debounce
  checkButton(yellowButton, &lastYellowState, &lastYellowDebounce, 0, "Yellow", now);
  checkButton(redButton, &lastRedState, &lastRedDebounce, 1, "Red", now);
  checkButton(blueButton, &lastBlueState, &lastBlueDebounce, 2, "Blue", now);

  // Run animation if active
  if (animating) runAnimation();

  // Run coffee ready blink if active
  if (coffeeReadyBlink) runCoffeeReadyBlink();

  // Potentiometer controls bottom LED brightness
  if (column != -1) {
    const int* leds = getLEDs(column);
    int potValue = analogRead(potPin);
    int brightness = map(potValue, 0, 1023, 0, 255);
    analogWrite(leds[3], brightness);
  }
}

// Button debounce and trigger
void checkButton(int pin, bool* lastState, unsigned long* lastDebounce, int col, const char* name, unsigned long now) {
  bool reading = digitalRead(pin);

  if (reading != *lastState) {
    *lastDebounce = now; // reset debounce timer
  }

  if ((now - *lastDebounce) > debounceDelay) {
    // Button is LOW when pressed due to INPUT_PULLUP
    if (reading == LOW && !animating && !coffeeReadyBlink) {
      resetPreviousBottomLED();
      startAnimation(col, name);
    }
  }

  *lastState = reading;
}

// Reset previous bottom LED
void resetPreviousBottomLED() {
  if (column != -1) {
    const int* leds = getLEDs(column);
    digitalWrite(leds[3], LOW);
  }
}

// Start the drop animation
void startAnimation(int col, const char* name) {
  Serial.print(name);
  Serial.println(" button pressed!");

  animating = true;
  column = col;
  step = 0;
  lastTime = millis();

  const int* leds = getLEDs(column);
  for (int i = 0; i < 4; i++) digitalWrite(leds[i], LOW);
}

// Non-blocking animation of top 3 LEDs
void runAnimation() {
  const int* leds = getLEDs(column);

  if (millis() - lastTime >= stepDelay) {
    lastTime = millis();

    // Turn off all top 3 LEDs
    for (int i = 0; i < 3; i++) digitalWrite(leds[i], LOW);

    if (step < 3) {
      digitalWrite(leds[step], HIGH);
      step++;
    } else {
      digitalWrite(leds[3], HIGH); // bottom LED stays on
      animating = false;
      coffeeReadyBlink = true;      // start coffee ready blink
      blinkStep = 0;
      lastBlinkTime = millis();
    }
  }
}

// Coffee ready blink animation
void runCoffeeReadyBlink() {
  const int* leds = getLEDs(column);

  if (millis() - lastBlinkTime >= blinkDelay) {
    lastBlinkTime = millis();

    // Toggle bottom LED
    if (blinkStep % 2 == 0) digitalWrite(leds[3], LOW);
    else digitalWrite(leds[3], HIGH);

    blinkStep++;
    if (blinkStep >= totalBlinks * 2) {
      coffeeReadyBlink = false;
      blinkStep = 0;
      digitalWrite(leds[3], HIGH); // leave bottom LED on
    }
  }
}

// Return LED column array
const int* getLEDs(int col) {
  if (col == 0) return yellowLEDs;
  else if (col == 1) return redLEDs;
  else return blueLEDs;
}
