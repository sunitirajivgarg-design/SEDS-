#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>

// 1. Initialize the display engine mapping strictly at address 32 (0x20)
Adafruit_LiquidCrystal lcd(32); 

// 2. Hardware Pin Layout Channel Specifications
const int TRIG_PIN   = 12;
const int ECHO_PIN   = 11;
const int LIGHT_PIN  = A0;
const int BUTTON_PIN = 2;
const int BUZZER_PIN = 8;
const int LED_PIN    = 7;

// 3. Finite State Machine Enumeration Bounds
enum State { OPEN_SEA, ANCHOR_DROPPED, STORM, CHARYBDIS, WRECKED };
State currentState = OPEN_SEA;
State savedState   = OPEN_SEA; 

// 4. Global Time Registers & Debounce Flags
unsigned long stateStartTime = 0;
int lastButtonState = 1; 
unsigned long lastBlinkTime = 0;
bool ledState = false;

// Forward declaration of visual output routing block
void updateDisplay();

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Safe internal pullup anchor switch
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Clean initialization parameters for standard 16x2 visual dimensions
  lcd.begin(16, 2);
  lcd.setBacklight(1); 
  
  Serial.begin(9600);
  Serial.println("--- SYSTEM ONLINE ---");
  
  updateDisplay();
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duration == 0) return 400; // Returns out-of-bounds safety variable if unplugged
  return duration * 0.034 / 2;
}

void loop() {
  // A. Sample Live Environment Metrics
  long distance = getDistance();
  int lightValue = analogRead(LIGHT_PIN); 
  int currentButtonState = digitalRead(BUTTON_PIN);
  
  // B. Parse Live Threshold Boolean Flags
  bool stormActive = (lightValue < 512);     // Light drops below half intensity bounds
  bool charybdisActive = (distance < 100);   // Threat wall within 100cm baseline

  // C. Input Edge-Trigger Button Evaluation
  if (currentButtonState == 0 && lastButtonState == 1) {
    if (currentState != WRECKED) {
      if (currentState == ANCHOR_DROPPED) {
        currentState = savedState;
        if (currentState == STORM || currentState == CHARYBDIS) {
          stateStartTime = millis(); // Reset 5s countdown clock upon entering danger
        }
      } else {
        savedState = currentState;
        currentState = ANCHOR_DROPPED;
      }
      updateDisplay();
    }
    delay(150); // Software debouncing stabilizer
  }
  lastButtonState = currentButtonState; 

  // D. State Machine Switch Transitions & Priority Actions
  switch (currentState) {
    
    case OPEN_SEA:
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);

      // PROMPT PRIORITY LOCK: First matching branch triggers, locking focus!
      if (stormActive) {
        currentState = STORM;
        stateStartTime = millis(); 
        updateDisplay();
      } else if (charybdisActive) {
        currentState = CHARYBDIS;
        stateStartTime = millis(); 
        updateDisplay();
      }
      break;

    case STORM:
      noTone(BUZZER_PIN); // Whirlpool alarm is muted due to original priority lock!
      
      // Asynchronous LED blinking sequence
      if (millis() - lastBlinkTime >= 250) {
        lastBlinkTime = millis();
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
      }

      if (!stormActive) {
        currentState = OPEN_SEA; 
        updateDisplay();
      } else if (millis() - stateStartTime >= 5000) {
        currentState = WRECKED; 
        updateDisplay();
      }
      break;

    case CHARYBDIS:
      digitalWrite(LED_PIN, LOW); // Storm alert is muted due to original priority lock!
      tone(BUZZER_PIN, 440);      // Continuously sound whistle tone alert

      if (!charybdisActive) {
        currentState = OPEN_SEA; 
        updateDisplay();
      } else if (millis() - stateStartTime >= 5000) {
        currentState = WRECKED; 
        updateDisplay();
      }
      break;

    case ANCHOR_DROPPED:
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
      break;

    case WRECKED:
      digitalWrite(LED_PIN, HIGH);
      tone(BUZZER_PIN, 150); // Deep continuous hum drone layout
      break;
  }

  delay(20); 
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  switch (currentState) {
    case OPEN_SEA:       lcd.print("STATE: OPEN SEA");   Serial.println("STATE: OPEN SEA");   break;
    case ANCHOR_DROPPED: lcd.print("STATE: ANCHOR");     Serial.println("STATE: ANCHOR");     break;
    case STORM:          lcd.print("STATE: STORM!");    Serial.println("STATE: STORM!");    break;
    case CHARYBDIS:      lcd.print("STATE: CHARYBDIS");  Serial.println("STATE: CHARYBDIS");  break;
    case WRECKED:        lcd.print("STATE: WRECKED");    Serial.println("STATE: WRECKED");    break;
  }
}