# Carmunicate

**Hacktronix 2025 | Sri Sai Ram Engineering College**  
**Theme: Vehicle-to-Vehicle (V2V) Communication**  
**Category: Hardware**

## Overview

**Carmunicate** is a real-time V2V (Vehicle-to-Vehicle) communication simulation developed for the **Hacktronix 24-Hour Hackathon**. The goal is to enhance road safety through immediate alerts between nearby vehicles in critical situations like:

- Sudden Braking
- Emergency Situations
- Overspeeding

Our system simulates communication between two cars both in a **web environment** and on **real hardware** using ESP8266 modules.

---

## Project Components

### Simulation (Frontend)

A browser-based visualization using:
- **HTML**
- **CSS**
- **JavaScript**

Simulates two cars where events such as braking or overspeeding trigger V2V communication animations or alerts.

### Hardware Prototype

Built using:
- **2 × ESP8266 Modules**
- **Motor Driver**
- **LCD Display**
- **Accelerator & Brake Buttons**

Communication is achieved via **ESP-NOW**, enabling peer-to-peer messaging between the vehicles. The LCD displays status messages, and the system reacts to speed or brake inputs.

---

## Features

- **Real-time Alerts** to nearby vehicles.
- **2-Way Communication** using ESP-NOW.
- **Emergency Detection** with status displays.
- **Simulation + Hardware Sync** for demonstration purposes.

---

## Installation

### Simulation

1. Clone the repo:
   ```bash
   git clone https://github.com/KumarCySec/Hacktronix
