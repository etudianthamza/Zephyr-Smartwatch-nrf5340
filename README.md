# ⌚ Embedded Smartwatch Based on nRF5340

## 📌 Project Overview

This project is a fully embedded smartwatch system developed using the **Nordic nRF5340-DK** board, inspired by the open-source [ZSWatch project](https://github.com/jakkra/ZSWatch) by Jakob Krantz and Daniel Kampert.

The system runs on **Zephyr RTOS** and integrates real-time environmental sensing, Bluetooth Low Energy communication, a graphical touchscreen interface, and precise timekeeping via an external RTC module.

This project highlights competencies in:
- Embedded firmware development with Zephyr RTOS
- Sensor interfacing (I2C, SPI)
- Real-time GUI design with LVGL
- Bluetooth Low Energy (BLE) communication
- Real-Time Clock (RTC) management

---

## 🎯 Objectives

- Run a real-time operating system (Zephyr) on nRF5340
- Display a smartwatch interface on a 320×240 touchscreen using LVGL
- Read environmental and motion data from onboard sensors
- Exchange data with a smartphone via Bluetooth Low Energy
- Manage accurate timekeeping with an external RTC module (RV-8263-C8)

---

## 🧠 System Architecture

### 🔧 Main Board

- **Nordic nRF5340-DK**
  - MCU: nRF5340 (dual-core ARM Cortex-M33)
  - App Core @ 128 MHz / Net Core @ 64 MHz (BLE stack)
  - RTOS: Zephyr Project

### 🖥️ Display

- **Adafruit 2.8" TFT Touch Shield v2**
  - Resolution: 320 × 240 pixels
  - Interface: SPI
  - Capacitive touchscreen
  - GUI Framework: LVGL + Squareline Studio

### 🌡️ Environmental & Motion Sensors (ST IKS01A3 Shield)

| Measurement                  | Sensor   | Interface |
|------------------------------|----------|-----------|
| Accelerometer & Gyroscope    | LSM6DSO  | I2C       |
| Magnetometer                 | LIS2MDL  | I2C       |
| Temperature & Humidity       | HTS221   | I2C       |
| Barometric Pressure          | LPS22HH  | I2C       |

### 🕐 Real-Time Clock

- **Micro Crystal RV-8263-C8**
  - Interface: I2C
  - Purpose: Accurate date/time tracking independent of MCU

---

## 💻 Software & Technologies

- Embedded C
- Zephyr RTOS
- LVGL — Lightweight graphics library
- Squareline Studio — GUI design tool
- Bluetooth Low Energy (BLE)
- I2C / SPI communication protocols
- Devicetree & Overlays
- Kconfig — Kernel configuration
- west — Build & flash toolchain

---

## 📊 Features

### ⌚ Smartwatch Interface
- Real-time clock display (hours, minutes, seconds)
- Date display
- Touch-based navigation between screens

### 🌍 Environmental Monitoring
- Temperature (°C)
- Humidity (%RH)
- Atmospheric pressure (hPa)

### 🏃 Motion Sensing
- Accelerometer data (LSM6DSO)
- Gyroscope data (LSM6DSO)
- Magnetometer / compass (LIS2MDL)

### ⏱️ Chronometer
- Start / Stop / Reset via touchscreen

### 📡 Bluetooth Low Energy
- Data exchange with a paired smartphone
- BLE advertising and connection management

---

## 🚀 Getting Started

### Requirements

- [nRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/installation.html) (includes Zephyr)
- `west` build tool
- J-Link / nRF5340-DK USB connection

### Installation

```bash
git clone https://github.com/etudianthamza/Zephyr-Smartwatch-nrf5340.git
cd Zephyr-Smartwatch-nrf5340
```

1. Install **nRF Connect SDK** and set up `west`
2. Build the project for the nRF5340-DK
3. Connect the board via USB
4. Flash the firmware

```bash
west build -b nrf5340dk_nrf5340_cpuapp
west flash
```

---

## 📸 Demonstration

<p align="center">
  <img src="Demonstration_Smartwatch_12Mo.gif" width="320"/>
</p>
<p align="center">
  <em>Full smartwatch demonstration — Zephyr RTOS + LVGL interface on nRF5340</em>
</p>

---

## 📚 References

- [ZSWatch — Original Project](https://github.com/jakkra/ZSWatch) by Jakob Krantz & Daniel Kampert
- [Zephyr Project RTOS](https://zephyrproject.org/)
- [LVGL — Light and Versatile Graphics Library](https://lvgl.io/)
- [Nordic nRF5340 Product Page](https://www.nordicsemi.com/Products/nRF5340)
- [ST IKS01A3 Shield](https://www.st.com/en/ecosystems/x-nucleo-iks01a3.html)
- [Micro Crystal RV-8263-C8](https://www.microcrystal.com/en/products/real-time-clock-rtc-modules/rv-8263-c8/)
