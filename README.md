# Fuel Monitoring System

An IoT-based fuel monitoring system developed for monitoring
generator fuel levels using an ESP32, JSN-SR04T ultrasonic sensor,
MCP4725 DAC and DSE 7320 MKII generator controller.

## Features

- Non-contact fuel-level measurement
- Ultrasonic distance measurement
- Fuel volume calculation
- Fuel percentage calculation
- Analog voltage generation
- DSE 7320 MKII integration
- Sensor fault detection
- Tank calibration
- ESP32-based control

## System Architecture

[Block diagram]

## Hardware

- ESP32
- JSN-SR04T ultrasonic sensor
- MCP4725 12-bit DAC
- DSE 7320 MKII
- DC-DC buck converter
- Fuel tank

## Working Principle

The JSN-SR04T measures the distance between the sensor
and the fuel surface.

The ESP32 converts the measured distance into fuel level,
fuel percentage and fuel volume.

The calculated fuel level is then converted into an analog
voltage using the MCP4725 DAC.

The analog output is interfaced with the DSE 7320 MKII
generator controller.

## Calibration

Maximum fuel volume: 4016 L

Full-level distance: 23.0 cm

Empty-level distance: 133.8 cm


## Pin Configuration

| Device | Signal | ESP32 |
|---|---|---|
| JSN-SR04T | TRIG | GPIO 18 |
| JSN-SR04T | ECHO | GPIO 19 |
| MCP4725 | SDA | GPIO 21 |
| MCP4725 | SCL | GPIO 22 |

