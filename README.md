# WemosServoMotorControl
## Controlling a Tower Pro SG90 Micro Servo Motor over a network using a Wemos D1 Mini

This project shows how one can control a commonly available micro-servo motor, the Tower Pro SG90, remotely, over a network, using a Wemos D1 Mini.

The Wemos connects to Wi-Fi using the credentials specified in the code. Once connected, the system is in one of two modes - 

1. Light mode (default)
2. Normal mode

In the "Light" mode, a Light Dependent Resistor (LDR) is used as a light sensor and the varying resistance is  converted to a varying voltage and fed to the analog input of the Wemos. The analog value is then used to set the servo-motor's angular position, thus the motor's position indicates the ambient light intensity.

A physical switch is used to switch between the "Light" mode and "Normal" mode.

Once the switch is flipped to "Normal" mode, the motor's angle can be set by calling a URL like the following:

```
http://<IP address of Wemos>/deg=60
```

Note the absence of the "?". It is not a typo!


Apart from the above URL, there are other URLs that can be used to set various parameters, like the 'step' size, which is the speed with which the motor's angular position changes.

## Wiring Diagram


![Wemos Servo Motor Control](https://github.com/ajithvasudevan/WemosServoMotorControl/raw/master/WemosServoMotorControl.png "Wemos Servo Motor Control")

  
  
