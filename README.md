# StairAlert

*This project is still a Work-in-Progress. The current version has the bare minimum to test the core functionality.*

## Purpose
Got tired of getting heart attacks every time someone sneaks up on me when I have my headset
on. These are the design files for an ESP8266 microcontroller which alerts me if somebody
is approaching me via the stairs the microcontroller is keeping tabs on.

## General description
An ESP8266 is used to detect motion in a stairwell using a PIR sensor. The ESP8266 is
connected to a router using onboard WiFi. The computer acts as a WebSocket server that the
ESP8266 connects to. Once the computer receives a trigger signal over the WebSocket
interface from the ESP8266, the computer plays an alert tone.

The board used for this project is a WEMOS D1 Mini Clone bought from a Chinese eBay seller.
Should work fine with other ESP8266 boards, though pins might have to be re-mapped in the
code.

## Instructions
1. File `config_template.h` in `ESP8266_main` should be renamed to `config.h`, and the
credentials in it should be changed accordingly.
2. Open and upload `ESP8266_main/ESP8266_main.ino` to ESP8266 in Arduino IDE.
3. Run file `main.py` on the computer.
