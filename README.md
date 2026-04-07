# With You – Personal Safety and Activity Monitoring Device
This project was developed for the course “Applications of Telecommunication Devices” at the Department of Electrical and Computer Engineering, Aristotle University of Thessaloniki, in 2021.

## Description
With You is an Arduino-based wearable system designed to improve personal safety and track daily activity. The device monitors heart rate (BPM) and distance traveled, provides notifications for reaching daily goals, and includes a safety alert system for potential danger.

## Features
- Heart rate monitoring using Pulse Sensor SEN 11574  
- Distance tracking with HC-SR04 ultrasonic sensors  
- Safety alerts with LED notifications and buzzer activation  
- Wireless communication to a base station for real-time monitoring  
- Multi-node setup using ALOHA-based packet transmission  
- Reward notifications for goal achievement  

## Hardware Components
- Arduino Uno  
- Pulse sensor (SEN 11574)  
- Ultrasonic distance sensors (HC-SR04)  
- LEDs and buzzers  

## Software
- Arduino `.ino` sketches for sensor reading, data transmission, and alert handling  
- Simulation of location data using `rand()` for prototype testing  
- Wireless network setup for multiple transmitters and a single receiver  

## Implementation Details
- Transmitter nodes collect sensor data and send it to the receiver  
- Receiver node displays the measurements, identifies which node sent the data, and triggers alerts  
- Safety alert triggers buzzer and sends location data in real-time  
- Sliding window logic and timing calculations ensure consistent updates  

## Network Analysis 
- ALOHA protocol used to handle multiple transmitters  
- Packet size: 400 bits; Transmission rate: 125 kbps; Packet duration: 3.2 ms  
- Traffic calculations ensure the network can handle 100 users with minimal collisions  
- Coverage analysis ensures 95% probability of successful reception  

## Usage
1. Upload the transmitter `Aloha_Tx.ino`, `Aloha_Tx_2.ino`  sketch to the wearable Arduino unit  
2. Upload the receiver `Aloha_Rx.ino` sketch to the base station Arduino  
3. Power the devices and monitor heart rate, distance, and safety alerts in real time  
4. Press the alert button on the transmitter to simulate danger  

## Future Improvements
- Integrate GPS for real location tracking  
- Miniaturize sensors into a wristband or smartwatch  
- Optimize network for battery efficiency  
- Combine with a secondary device for improved coverage of ultrasonic sensors  

## Goal
The project demonstrates practical experience with embedded systems, sensor integration, IoT communication, and real-time alert mechanisms, providing a solid foundation for wearable safety technology development.
