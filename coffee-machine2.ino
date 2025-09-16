// LED pins
const int yellowLEDs[] = {2, 3, 4, 5};
const int redLEDs[]    = {6, 7, 8, 9};
const int blueLEDs[]   = {10, 11, 12, 13};

// Button pins
const int yellowButton = A0;
const int redButton    = A1;
const int blueButton   = A2;

// Animation variables
bool animating = false;
int column = -1;
int step = 0;
unsigned long lastTime = 0;
const int stepDelay = 200; // ms per animation step

// Debounce variables
unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 50; // ms

void setup() {
  Serial.begin(9600);

  // Set LED pins as OUTPUT
  for (int i = 0; i < 4; i++) {
    pinMode(yellowLEDs[i], OUTPUT);
    pinMode(redLEDs[i], OUTPUT);
    pinMode(blueLEDs[i], OUTPUT);
  }

  // Set button pins as INPUT
  pinMode(yellowButton, INPUT);
  pinMode(redButton, INPUT);
  pinMode(blueButton, INPUT);
}

void loop() {
  unsigned long now = millis();

  // Only check buttons if not animating
  if (!animating) {

    // Yellow button
    if (digitalRead(yellowButton) == HIGH && now - lastPressTime > debounceDelay) {
      startAnimation(0, "Yellow");
      lastPressTime = now;
    } 
    // Red button
    else if (digitalRead(redButton) == HIGH && now - lastPressTime > debounceDelay) {
      startAnimation(1, "Red");
      lastPressTime = now;
    } 
    // Blue button
    else if (digitalRead(blueButton) == HIGH && now - lastPressTime > debounceDelay) {
      startAnimation(2, "Blue");
      lastPressTime = now;
    }
  }

  // Run animation if active
  if (animating) runAnimation();
}

void startAnimation(int col, const char* name) {
  Serial.print(name);
  Serial.println(" button pressed!");
  
  animating = true;
  column = col;
  step = 0;
  lastTime = millis();

  // Turn off all LEDs in this column at the start
  const int* leds = getLEDs(column);
  for (int i = 0; i < 4; i++) digitalWrite(leds[i], LOW);
}

void runAnimation() {
  const int* leds = getLEDs(column);

  if (millis() - lastTime >= stepDelay) {
    lastTime = millis();

    // Turn off all top 3 LEDs before lighting the current one
    for (int i = 0; i < 3; i++) digitalWrite(leds[i], LOW);

    if (step < 3) {
      digitalWrite(leds[step], HIGH); // Light current step
      step++;
    } else {
      digitalWrite(leds[3], HIGH); // Bottom LED stays on
      animating = false;           // Animation finished
      column = -1;
    }
  }
}

const int* getLEDs(int col) {
  if (col == 0) return yellowLEDs;
  else if (col == 1) return redLEDs;
  else return blueLEDs;
}
