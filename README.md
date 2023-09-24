# Smart Home IoT
This repository contains Arduino sketches for all IoT nodes that are part of the system be part of a smart home or IoT system. Each sketch is designed to run on an ESP8266-based microcontroller (e.g., NodeMCU) and serves a specific purpose within the system.

## NOTE:
**object detection is based on basic https://github.com/tensorflow/examples implementation**

###Nodes and Sketches
1. NodeDoor (Arduino)
   Purpose: Monitors the state of a door sensor and sends door open/close events to a central node.
   Components: ESP8266, Door Sensor
   Features: Sends UDP messages to a central node, provides door status feedback.
2. NodeWindow (Arduino)
   Purpose: Monitors the state of a window sensor and sends window open/close events to a central node.
   Components: ESP8266, Window Sensor
   Features: Sends UDP messages to a central node, provides window status feedback.
3. NodeMobile (Arduino)
   Purpose: Connects to Wi-Fi, listens for UDP packets from a central node, and displays information on an OLED screen.
   Components: ESP8266, OLED Display, Button, Buzzer
   Features: Displays data on an OLED screen, sounds an alarm for alerts, switches between multiple screens.
4. NodeCore (Node.js & Python)
   Purpose: Serves as the central node that receives data from other nodes and manages the IoT system.
   Features: Listens for UDP packets from nodes, processes data, and triggers alerts or actions.
6. NodePir (Arduino)
   Purpose: Monitors motion using a Passive Infrared (PIR) sensor and sends motion detection events to a central node.
   Components: ESP8266, PIR Sensor
   Features: Sends UDP messages to a central node, provides motion detection feedback.