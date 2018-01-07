# WemosServoMotorControl
## Controlling a Tower Pro SG90 Micro Servo Motor over a network using a Wemos D1 Mini

This project shows how one can control a commonly available micro-servo motor, the Tower Pro SG90, remotely, over a network, using a Wemos D1 Mini.

The motor's angle can be set by calling a URL like the following:

```
http://<IP address of Wemos>/deg=60
```

Note the absense of the "?". It is not a typo!

The project has a "Light" mode where a Light Dependent Resistor (LDR) is used as a light sensor and the varying resistance is  converted to a varying voltage and fed to the analog input of the Wemos. The analog value is then used to set the servo-motor's angular position, thus the motor's position indicates the ambient light intensity.

A physical switch is used to enable or disable the "Light" mode.

Apart from the above URL, there are other URLs that can be used to set various parameters, like the 'step' size, which is the speed with which the motor's angular position changes.


  
  
