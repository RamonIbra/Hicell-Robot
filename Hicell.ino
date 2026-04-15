#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
SoftwareSerial voiceSerial(2, 3); // RX, TX pins

// Pin Definitions for LEDs and Lasers
#define BLUE_LED_PIN   4
#define RED_LASER_PIN  5
#define GREEN_LED_PIN  6
#define RED_LED_PIN    7 

// Servo configuration
const int servoChannels[] = {1, 2, 15};
int currentPWM[3] = {400, 400, 400};

void setup() {
  Serial.begin(9600);
  voiceSerial.begin(9600);

  pwm.begin();
  pwm.setPWMFreq(50); // Standard frequency for servos

  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(RED_LASER_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT); 

  resetOutputs();   // Turn everything off at start
  setServoAll(400); // Set servos to the default "Ready" position
}

void loop() {
  // Check if the voice module has sent a number
  if (voiceSerial.available()) {
    int cmd = voiceSerial.read();
    Serial.print("Command Received: ");
    Serial.println(cmd);

    handleCommand(cmd);
  }
}

void handleCommand(int cmd) {
  resetOutputs(); // Reset all lights before activating the new mode

  switch (cmd) {
    case 1: // Voice command for "ON"
      digitalWrite(BLUE_LED_PIN, HIGH);
      Serial.println("Mode: Blue LED");
      break;

    case 20: // Voice command for "Defence Mode"
      digitalWrite(RED_LASER_PIN, HIGH);
      digitalWrite(RED_LED_PIN, HIGH);
      Serial.println("Mode: Laser + Red LED");
      break;

    case 21: // Voice command for "Standby"
      digitalWrite(GREEN_LED_PIN, HIGH);
      Serial.println("Mode: Green LED");
      break;

    case 4: // Voice command for "Pause"
      digitalWrite(RED_LED_PIN, HIGH);
      setServoAllSmooth(200); // Move servos slowly to position 200
      Serial.println("Mode: Idle (Servo 200)");
      break;

    case 19: // Voice command for "Start"
      setServoAllSmooth(400); // Move servos slowly to position 400
      Serial.println("Mode: Ready (Servo 400)");
      break;

    default:
      Serial.println("Unknown command received");
      break;
  }
}

// Simple helper to turn off all pins
void resetOutputs() {
  digitalWrite(BLUE_LED_PIN, LOW);
  digitalWrite(RED_LASER_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
}

// Jumps all servos immediately to a position
void setServoAll(int pwmVal) {
  for (int i = 0; i < 3; i++) {
    pwm.setPWM(servoChannels[i], 0, pwmVal);
    currentPWM[i] = pwmVal;
  }
}

// Moves servos step-by-step to avoid jerky motion
void setServoAllSmooth(int targetPWM) {
  int startPWM[3];
  int step[3];
  int finalTargets[3];
  int maxSteps = 0;

  // 1. Preparation Phase: Calculate distances for each servo
  for (int i = 0; i < 3; i++) {
    startPWM[i] = currentPWM[i];
    
    // Check if this is the inverted channel (15)
    int actualTarget;
    if (servoChannels[i] == 15) {
      actualTarget = invertPWM(targetPWM);
    } else {
      actualTarget = targetPWM;
    }
    finalTargets[i] = actualTarget;

    // Determine if we need to count up or count down
    if (startPWM[i] < actualTarget) {
      step[i] = 1; 
    } else {
      step[i] = -1;
    }

    // Find the total distance to travel
    int distance = abs(startPWM[i] - actualTarget);
    if (distance > maxSteps) {
      maxSteps = distance;
    }
  }

  // 2. Execution Phase: Move all servos simultaneously one step at a time
  for (int s = 0; s <= maxSteps; s++) {
    for (int i = 0; i < 3; i++) {
      int currentPos = startPWM[i] + (step[i] * s);
      
      // Ensure we don't move past the target
      if (step[i] == 1) { // We are increasing
        if (currentPos <= finalTargets[i]) {
          pwm.setPWM(servoChannels[i], 0, currentPos);
        }
      } else { // We are decreasing
        if (currentPos >= finalTargets[i]) {
          pwm.setPWM(servoChannels[i], 0, currentPos);
        }
      }
    }
    delay(5); // Wait 5ms between steps to control speed
  }

  // 3. Update Phase: Save the new positions as the current ones
  for (int i = 0; i < 3; i++) {
    currentPWM[i] = finalTargets[i];
  }
}

// Math to flip the value for the inverted servo
int invertPWM(int val) {
  return 600 - (val - 150);
}