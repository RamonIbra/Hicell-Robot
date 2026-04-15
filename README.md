Description
This project is a custom-built shoulder robot inspired by Death Stranding. It uses 3D-printed mechanical parts (arms and base) controlled by an Arduino. It moves servos based on voice commands and activates different LEDs and a laser.

Hardware Components
Arduino: Controls the main logic.
PCA9685 Driver: Controls three servos.
Speech Module: Listens for the "Hicell" wake word and sends command IDs (1, 4, 19, 20, 21) via serial.
3D Printed Parts: Custom-designed base and arms.
Light Outputs: Blue LED, Green LED, Red LED, and a Red Laser.

How the Code Works
Communication: The Arduino uses SoftwareSerial to talk to the voice module and I2C to talk to the servo driver.
Movement: A "smooth" function is used instead of direct movement. It moves the servos 1 unit at a time with a 5ms delay so the 3D-printed arms do not snap or shake.
Inversion: Channel 15 is programmed to move in the opposite direction of the other channels to account for the physical mounting of the servos.
Logic: A switch statement checks the incoming command ID. It first turns off all lights (Reset), then sets the new LED and moves the servos to the target position.

Command List

**1** 

*"Turn on the light"* | Blue LED ON | **Search Mode** 

**20***"Mode 1"* | Red Laser + Red LED | **BT Detection** 

**21** *"Mode 2"* | Green LED ON | **Safe Zone** 

**4** *"Pause"* | Red LED + Servo 200 | **Idle / Retract** 

**19** *"Start"* | Clear LEDs + Servo 400 | **Deploy / Ready** 

Wiring
Pins 2, 3: Voice Module.
Pins 4, 5, 6, 7: LEDs and Laser.
I2C (A4/A5): PCA9685 Servo Driver.
