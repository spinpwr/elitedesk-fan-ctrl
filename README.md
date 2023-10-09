# Mini arudino PWM fan controller
Arduino based mini PWM fan controller originally created for an HP EliteDesk 705 G4 mini.

The mentioned system has a really annoying fan control issue which has never been solved by the vendor.
Basically any hardware change (even unpluging the power brick..) causes the PC to run the fans at full speed at the next boot.  
See details on reddit: https://www.reddit.com/r/homelab/comments/su3qpx/elitedesk_705_g4_mini_fans_running_full_speed/  

Instead of reseting CMOS and taking out the battery everytime I decided to build a fan controller for the PC.

## BoM
- Arduino Pro mini 
(or other small board with at least 1 PWM output)
- NTC or small temp sensor 
(it needs to fit undet the heatsink)
- 4pin JTS connector with 1.5 mm pitch
- some wires

## Principal
The voltage drop of the NTC is measured by an analog pin of the arduino board. The NTC resistance (and also the voltage drop) is inversely proportional to the temperature. We don't need to know the actual resistance, not even the voltage the value from the ADC can be used to set the fan speed.

The PC uses PWM fans (one for the CPU and one for the drives) so the speed can be set by generating 25kHz signal(s). 