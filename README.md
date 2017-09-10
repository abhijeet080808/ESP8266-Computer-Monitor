# ESP8266-Computer-Monitor
ESP8266 Arduino sketch to monitor Computer reachability

## Introduction

The Arduino [sketch](https://www.arduino.cc/en/Reference/HomePage) is intended to run on a [ESP8266](http://espressif.com/en/products/hardware/esp8266ex/overview) based [ESP-12E](https://github.com/nodemcu/nodemcu-devkit-v1.0) board.

There are 3 components in this project:
  1. Computer that we want to monitor
  2. ESP-12E board
  3. IFTTT enabled smart switch (Eg. [D-Link Smart Plug](https://ifttt.com/dlink_smart_plug)) that is used to power both the computer and the ESP-12E board.

All the three components are connected to internet.

The ESP-12E is used to monitor the computer. When the computer is switched off, the ESP-12E switches off the power to the computer and to itself via the IFTTT enabled smart switch.

The ESP-12E uses periodic ICMP pings to determine if the computer is up. On startup, the ESP-12E periodically polls the computer and once the computer is found to be reachable, it considers itself as armed. In armed state, the ESP-12E will continue to periodically poll the computer and if the computer goes down, it will switch off the power supply by sending a command to a DLink Smart Plug via a IFTTT Webhook.

## Dependencies

  1. [ESP8266 Arduino Core](https://github.com/esp8266/Arduino) - Install in Arduino IDE by going to `Sketch -> Include Library -> Manage Library`.
  2. [TaskScheduler](https://github.com/arkhipenko/TaskScheduler) - Install in Arduino IDE by going to `Sketch -> Include Library -> Manage Library`.
  3. [ESP8266Ping](https://github.com/dancol90/ESP8266Ping) - Download the [ZIP file](https://github.com/dancol90/ESP8266Ping/archive/master.zip) and install in Arduino IDE by going to `Sketch -> Include Library -> Add .ZIP Library`.

## Configuration

Configure the sketch to suit the run environment. Parameters that must be configured by editing the sketch file -
 1. `SSID`
 2. `PASSWORD`
 3. `MONITOR_IP`
 4. `MONITOR_DURATION_MS`
 5. `FAIL_THRESHOLD`
 6. `CONTROL_SERVER`
 7. `OFF_URL`

## Some useful links
  - https://www.arduino.cc/en/Reference/HomePage
  - http://esp8266.github.io/Arduino/versions/2.3.0/doc/libraries.html
