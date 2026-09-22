# Ultrasonic-Based Real-Time Fuel Level Monitoring System

## Design and Implementation of an Ultrasonic-Based Real-Time Fuel Level Monitoring System Integrated with Deep Sea Electronics (DSE 7320 MKII) Controllers

An embedded fuel monitoring system designed to measure generator day-tank fuel levels using an ultrasonic sensor and integrate the calculated fuel level with a **Deep Sea Electronics (DSE 7320 MKII)** generator controller.

The system uses an **ESP32** to acquire ultrasonic distance measurements, calculate fuel level and volume, and generate a corresponding analog voltage through an **MCP4725 12-bit DAC**. The analog voltage is then supplied to the DSE 7320 MKII, which interprets the voltage according to a configured voltage-to-percentage mapping.

---

## 📌 Project Overview

Accurate fuel-level monitoring is important for generator-based power systems, particularly in telecommunication and industrial infrastructure.

Traditional fuel monitoring methods often depend on manual inspection or bulk-tank measurements. These approaches provide limited information about the fuel level of individual generator day tanks.

This project provides a non-contact and real-time approach using:

* Ultrasonic distance measurement
* ESP32 embedded processing
* Fuel volume and percentage calculation
* MCP4725 digital-to-analog conversion
* Analog interfacing with DSE 7320 MKII
* Configurable fuel-level calibration

The system was developed and tested for generator day-tank monitoring and was deployed across multiple generator tanks.

---

## ⚙️ System Architecture

                    ┌─────────────────────┐
                    │     Fuel Tank       │
                    └──────────┬──────────┘
                               │
                               │ Distance
                               ▼
                    ┌─────────────────────┐
                    │   Ultrasonic Sensor │
                    │      JSN-SR04T      │
                    └──────────┬──────────┘
                               │
                               │ Echo / Trigger
                               ▼
                    ┌─────────────────────┐
                    │        ESP32        │
                    │                     │
                    │ Distance Calculation│
                    │ Fuel Level          │
                    │ Fuel Percentage     │
                    │ Fuel Volume         │
                    └──────────┬──────────┘
                               │
                               │ I²C
                               ▼
                    ┌─────────────────────┐
                    │      MCP4725        │
                    │     12-bit DAC      │
                    └──────────┬──────────┘
                               │
                               │ Analog Voltage
                               │ 0 – 3.3 V
                               ▼
                    ┌─────────────────────┐
                    │    DSE 7320 MKII    │
                    │ Generator Controller│
                    └─────────────────────┘
```

---

# 🔧 Hardware Components

| Component            | Function                                    |
| -------------------- | ------------------------------------------- |
| ESP32                | Main processing and control unit            |
| JSN-SR04T            | Non-contact ultrasonic distance measurement |
| MCP4725              | 12-bit digital-to-analog converter          |
| DSE 7320 MKII        | Generator controller and fuel-level display |
| DC-DC Buck Converter | Provides regulated supply voltage           |
| Generator Day Tank   | Fuel storage and measurement target         |

---

# 📡 Ultrasonic Fuel-Level Measurement

The ultrasonic sensor is mounted above the fuel surface.

The sensor sends an ultrasonic pulse toward the fuel surface and measures the time taken for the reflected signal to return.

The ESP32 uses this measurement to determine the distance between the sensor and the fuel surface.


# 📏 Tank Calibration

The system uses calibrated reference distances corresponding to the full and empty conditions of the tank.

### Calibration Parameters

| Parameter                   |    Value |
| --------------------------- | -------: |
| Maximum calibrated volume   |  4016 L |
| Full-level distance         |  23 cm |
| Empty-level distance        | 132.58 cm |

> **Note:** The ultrasonic sensor has a minimum sensing/blind region. Therefore, the practical full-tank reference is determined by the actual installation and calibration rather than assuming the sensor can measure directly from its physical minimum range.

The calibration values may need to be changed for different tank dimensions or sensor mounting positions.

---

# 🧮 Fuel-Level Calculation

The measured ultrasonic distance is converted into fuel level using the calibrated full and empty reference points.

For a linear tank-level approximation:

```text
Fuel Height = Empty Distance − Measured Distance
```

Fuel percentage is calculated as:

```text
Fuel Percentage =
(Fuel Height / Effective Tank Height) × 100
```

The calculated percentage is constrained to the valid range:

```text
0% ≤ Fuel Percentage ≤ 100%
```

Fuel volume can then be calculated using the calibrated maximum tank volume:

```text
Fuel Volume =
(Fuel Percentage / 100) × Maximum Volume
```

For the calibrated tank:

```text
Maximum Volume = 3977 L
```

---

# 🔌 ESP32 Pin Configuration

## JSN-SR04T → ESP32

| ESP32   | Ultrasonic Sensor |
| ------- | ----------------- |
| GPIO 18 | TRIG              |
| GPIO 19 | ECHO              |
| 5 V     | VCC               |
| GND     | GND               |

The ECHO signal should be interfaced with the ESP32 using an appropriate voltage-divider/level-shifting arrangement where required.

---

## MCP4725 → ESP32

| ESP32   | MCP4725 |
| ------- | ------- |
| GPIO 21 | SDA     |
| GPIO 22 | SCL     |
| 3.3 V   | VCC     |
| GND     | GND     |

The MCP4725 communicates with the ESP32 through the **I²C interface**.

---

# ⚠️ Hardware Installation Notice

**Do not place the DC-DC buck converter close to the ultrasonic sensor.**

During hardware testing, electrical interference/noise from the power-conversion circuitry can affect ultrasonic sensor operation.

Keep the buck converter physically separated from the ultrasonic sensing section and use appropriate wiring and grounding practices.

---

# 🎚️ DSE 7320 MKII Analog Mapping

The ESP32 calculates the fuel percentage and converts it into an analog voltage using the MCP4725 DAC.

The system uses an inverse voltage relationship:

| Fuel Level | Analog Voltage |
| ---------: | -------------: |
|       100% |         0.00 V |
|        80% |         0.66 V |
|        60% |         1.32 V |
|        40% |         1.98 V |
|        20% |         2.64 V |
|         0% |         3.30 V |

Therefore:


The DSE 7320 MKII is configured with the corresponding voltage-to-percentage relationship.

---

# 🔢 MCP4725 DAC

The MCP4725 is a **12-bit DAC**.

It provides:

```text
Resolution = 12 bits

Digital range:
0 – 4095

Number of levels:
4096
```

For a 3.3 V reference/output range, the theoretical voltage step is approximately:

```text
3.3 / 4095 ≈ 0.806 mV
```

This allows the ESP32 to generate a fine-resolution analog voltage for the DSE controller.

---

# 💻 Firmware

The ESP32 firmware performs the following operations:

```text
1. Trigger ultrasonic sensor
          ↓
2. Measure echo time
          ↓
3. Calculate distance
          ↓
4. Validate sensor reading
          ↓
5. Calculate fuel level
          ↓
6. Calculate fuel percentage
          ↓
7. Calculate fuel volume
          ↓
8. Convert fuel percentage to DAC value
          ↓
9. Generate analog voltage
          ↓
10. Send voltage to DSE 7320 MKII
```



# 🛠️ Software and Tools

* Arduino IDE
* ESP32 Arduino Core
* C/C++
* I²C communication
* MCP4725 DAC library
* Serial Monitor

---

# 🧪 Testing

The system was tested through several stages.

### 1. Ultrasonic Sensor Testing

The ultrasonic sensor was tested independently to verify:

* Trigger operation
* Echo response
* Distance measurement
* Sensor fault conditions
* Measurement stability

### 2. Fuel-Level Calculation

Measured distance values were compared with calibrated tank levels.

### 3. DAC Testing

The MCP4725 output was tested across the required voltage range.

### 4. DSE Integration

The generated analog voltage was connected to the configured DSE 7320 MKII analog input and verified against the corresponding fuel percentage.

### 5. Field Testing

The complete system was tested under practical generator operating conditions.

---

# 📊 System Performance

The system is designed for near real-time fuel monitoring.

The usable sensing range depends on the physical installation and tank geometry. For the calibrated implementation, the system operates between the defined full and empty reference distances.

The MCP4725 provides 4096 discrete DAC levels, allowing smooth analog voltage generation.

The system also includes boundary handling to prevent calculated fuel values and DAC outputs from exceeding their valid operating ranges.

---

# 🚨 Sensor Fault Handling

The firmware can identify abnormal ultrasonic measurements such as:

* No echo received
* Invalid distance
* Distance outside the calibrated range
* Sensor disconnection
* Abnormal measurements

This prevents invalid measurements from directly producing uncontrolled output values.

---
# 🔬 Applications

The system can be adapted for:

* Generator day-tank monitoring
* Telecommunication infrastructure
* Industrial fuel monitoring
* Backup power systems
* Remote fuel-level monitoring
* IoT-based generator monitoring
* Industrial retrofit applications

---

# 🚀 Future Improvements

Potential future improvements include:

* Remote IoT data transmission
* Web-based monitoring dashboard
* Historical fuel-consumption analysis
* Low-fuel alerts
* Automated fuel-consumption calculation
* Cloud database integration
* Multiple-generator monitoring
* Improved ultrasonic filtering
* Temperature compensation
* Additional industrial communication interfaces

---

# ⚠️ Disclaimer

This repository is intended for educational, engineering, and development purposes.

When connecting the system to industrial generator controllers, verify voltage levels, grounding, isolation, wiring, and controller configuration before applying the signal to operational equipment.

Site-specific configurations, credentials, network information, and confidential infrastructure details should not be published in this repository.

---

# 👨‍💻 Author

**Thisara Siriwardhana**

Computer Engineering Undergraduate

Sri Lanka

---
