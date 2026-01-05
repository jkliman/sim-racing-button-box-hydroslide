#include <Joystick.h>


// Create Joystick instance with 9 buttons (0-8), no hat switch, no axes
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
  9, 0,                  // 9 buttons, 0 hat switches
  false, false, false,   // No X, Y, Z axes
  false, false, false,   // No Rx, Ry, Rz
  false, false,          // No rudder, throttle
  false, false, false);  // No accelerator, brake, steering

const int ON = LOW;//HIGH;
const int OFF = HIGH;//LOW;

// Piezo vibration motor pin (use a PWM-capable pin: 3,5,6,9,10 on Pro Micro)
const int motorPin = 9;
bool engineStarting = false;
unsigned long engineStartTime = 0;

// Engine start sound effect - simulates cranking then starting
void playEngineStart() {
  // Phase 1: Slow cranking (starter motor engaging)
  for (int i = 0; i < 4; i++) {
    digitalWrite(motorPin, HIGH);
    delay(80);
    digitalWrite(motorPin, LOW);
    delay(120);
  }

  // Phase 2: Cranking speeds up (engine catching)
  for (int i = 0; i < 6; i++) {
    digitalWrite(motorPin, HIGH);
    delay(60 - i * 5);
    digitalWrite(motorPin, LOW);
    delay(80 - i * 8);
  }

  // Phase 3: Engine catches and revs up
  for (int i = 0; i < 10; i++) {
    digitalWrite(motorPin, HIGH);
    delay(30);
    digitalWrite(motorPin, LOW);
    delay(20 - i);
  }

  // Phase 4: High rev burst
  digitalWrite(motorPin, HIGH);
  delay(400);

  // Phase 5: Settle to idle (pulsing vibration)
  for (int i = 0; i < 6; i++) {
    digitalWrite(motorPin, HIGH);
    delay(50);
    digitalWrite(motorPin, LOW);
    delay(50);
  }

  // Turn off motor completely
  digitalWrite(motorPin, LOW);
}

class ModeSwitch {
  private:
    int modePin1;
    int modePin2;
    char lastMode=' ';
    //bool lastPinState1;
    //bool lastPinState2;

    char checkMode(){
      int pinState1 = digitalRead(modePin1);
      int pinState2 = digitalRead(modePin2);
      if ((pinState1==OFF &&pinState2==OFF) || (pinState1==ON && pinState2==ON))
        return 'B';
      else if (pinState1==ON)
        return 'A';
      else
        return 'C';
    }

  public:
    ModeSwitch(int pin1=-1, int pin2=-1) 
      : modePin1(pin1), modePin2(pin2) {}

    void doSetup(){
      pinMode(modePin1, INPUT_PULLUP);
      pinMode(modePin2, INPUT_PULLUP);
    }

    bool enabled(){
      return (modePin1!=-1);
    }

    char getMode(){
      char mode = checkMode();
      if (mode!=lastMode){
        Serial.print("Mode Switched from ");
        Serial.print(lastMode);
        Serial.print(" to ");
        Serial.println(mode);
        lastMode=mode;        
      }
      return mode;
    }

};

// Define the Button class
class Button {
  private:
    int buttonIndex;
    int secondaryButtonIndex;
    int pin;
    bool currentPinState;
    bool lastPinState;
    bool currentButtonState;
    unsigned long lastChangeTime;
    const unsigned long pushTimeout = 250;  // Adjust this as needed (500 ms here)
    char mode;  // 'A', 'B', or 'C'
    ModeSwitch buttonModeSwitch;

    // This function would be replaced by your actual setButton implementation
    void setButton(bool state, int index) {
      // Logic for setting button state (to be implemented)
      Serial.print("Button ");
      Serial.print(index);
      Serial.println(state ? " On" : " Off");
      currentButtonState=state;
      Joystick.setButton(index, state);
    }

  int getButtonToPress(){
    return (mode == 'C' && currentPinState != ON)? secondaryButtonIndex: buttonIndex;
  }

  public:
    Button(int pinNumber, char buttonMode, int index, int secondaryIndex = -1) 
      : buttonIndex(index), secondaryButtonIndex(secondaryIndex), pin(pinNumber), mode(buttonMode), 
        currentPinState(OFF), lastPinState(OFF), lastChangeTime(0) {}

    Button(int pinNumber, ModeSwitch modeSwitch, int index, int secondaryIndex = -1) 
      : buttonIndex(index), secondaryButtonIndex(secondaryIndex), pin(pinNumber), buttonModeSwitch(modeSwitch), 
        currentPinState(OFF), lastPinState(OFF), lastChangeTime(0) {}

    // Reads the pin and checks if the state has changed
    bool readButton() {
      if (buttonModeSwitch.enabled()){
        //Serial.println("Getting mode from switch");
        mode= buttonModeSwitch.getMode();
      }

      currentPinState = digitalRead(pin);

      if (currentPinState != lastPinState) {
        lastChangeTime = millis();
        lastPinState = currentPinState;

        if (mode == 'A') {
          // Mode A: Simply call setButton based on ON/OFF state
          setButton(currentPinState == ON, buttonIndex);
        } else {
          // Mode B: Only act on OFF -> ON transitions and start the timer
          int buttonToPress = getButtonToPress();          
          setButton(true, buttonToPress);  // Button press        
       
        }
        return true;  // State changed
      } else {
        // Handle Mode B and C pushTimeout logic
        if (mode == 'B' || mode == 'C') {
          if (currentButtonState == true && millis() - lastChangeTime >= pushTimeout) {
            int buttonToPress = getButtonToPress();
            setButton(false, buttonToPress);  // Timeout expired for button press
          }
        }
      }
      return false;  // No state change
    }
  
  int getPin(){
    return pin;
  }

  void doSetup(){
    pinMode(getPin(), INPUT_PULLUP);
    buttonModeSwitch.doSetup();
  }
};

const int bypassPin=8;
// Setup each of our buttons
Button butIgnition = Button(2,ModeSwitch(20,21),0,5);
Button butStart = Button(3,'A',1);
Button butAcc1 = Button(4,ModeSwitch(18,19),2,6);
Button butAcc2 = Button(5,ModeSwitch(14,15),3,7);
Button butAcc3 = Button(6,ModeSwitch(10,16),4,8);
Button buttons[] = {
  butIgnition,
  butStart,
  butAcc1,
  butAcc2,
  butAcc3
};

const int numButtons = sizeof(buttons) / sizeof(buttons[0]);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  pinMode(bypassPin, INPUT_PULLUP);
  pinMode(motorPin, OUTPUT);

  digitalWrite(motorPin, LOW);  // Ensure motor is off at start
  Serial.println("Ready!");

  // Initialize Joystick Library
  Joystick.begin();

  Serial.println("setup!");
  // Set the button pins as inputs
  for (int i = 0; i < numButtons; i++) {
    buttons[i].doSetup();
  }
  
}

// Track start button state for engine effect
bool lastStartState = OFF;

void loop() {

  bool byPass = (digitalRead(bypassPin) == ON);

  if (!byPass){
    // Check if start button was just pressed (for engine sound)
    bool currentStartState = digitalRead(3);  // Start button pin
    if (currentStartState == ON && lastStartState == OFF) {
      Serial.println("Engine starting!");
      playEngineStart();
    }
    lastStartState = currentStartState;

    // Evaluate each button in the array using readButton()
    for (int i = 0; i < numButtons; i++) {
      buttons[i].readButton();
    }
  }
  // Add a delay to avoid overwhelming the serial monitor
  delay(100);
}
