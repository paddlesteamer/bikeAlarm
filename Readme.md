# bikeAlarm
Simple motorcycle alarm triggered by a gyroscope. Powered by motorcycle's self 12V battery. When triggered, it sends 'alarm' message to the station listening (433Mhz - connected to raspberryPi) which relays the alarm to the telegram bot. 

## client
It is the main component resides on motorcycle. Contains Dorji DRF020D20 module, 5V relay, L3GD20H Gyroscope, Arduino Nano and a voltage regulator to boost 12-14V from battery down to 5V.

## host
It is the station that listens to 'Alarm' messages from client. It, then, relays them to telegram. It is basically a raspberryPi listens thorough Dorji DRF7020D13 module. A UART to USB converter used for Dorji module to provide mobility - maybe later I may connect it to OTG port of my phone or to another computer
