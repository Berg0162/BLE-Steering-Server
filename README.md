# <img src="/media/ESP32_Icon.png" width="110" height="34" align="bottom" alt="ESP32 Icon"> Open Virtual Steering - BLESteeringServer
The **Romans** knew it already **`"Sterzare Necesse Est"`** (free after Plutarch) and it is still valid in a **virtual cycling world**.<br>
<details><summary>What is means</summary>
    
The original quote **Navigare Necesse Est** ("Navigation is Necessary"), was humourously adapted here, by replacing **Navigare** with the Italian verb for [**steering**](https://en.wiktionary.org/wiki/sterzare), aligning it with the project's theme.

</details>

**Open Virtual Steering (OVS)** is an open-source project that explores interfacing with various **Human-Interface-Devices (HID)** for the **use case of steering control** in **virtual cycling platforms**.
It provides examples of alternative input methods for **navigating and positioning a cycling avatar** in a virtual world.

## 📦 What Is This?

**BLESteeringServer** is an open-source, Arduino-compatible C++ library that provides a **reusable Bluetooth Low Energy (BLE) server** implementation for transmitting **steering data** to virtual cycling platforms, for use with the **Espressif ESP32 SoC's**.

It is designed as a **shared backend** for projects in the **Open Virtual Steering** ecosystem and supports pairing with BLE clients that recognize a known **steering profile** used in commercial platforms.

![Open Virtual Steering Diagram](/media/OpenVirtualSteering_Overview.png)

## 📚 Related OVS Repositories

This project allows users to explore a range of input devices for **steering control in virtual cycling worlds**. The emphasis is on **enhancing interactivity and user experience** through both **accessible** and **innovative** control methods.

| HID Focus | Bluedroid/NimBLE Repository | Bluefruit Repository |
|-----------|-------------------|----------------------|
| Buttons, Joysticks, Rotary Encoders | [`OVS-DiscreteHIDs`](https://github.com/Berg0162/Open-Virtual-Steering-DiscreteHID) | [`OVS-DiscreteHIDs-Bluefruit`](https://github.com/Berg0162/Open-Virtual-Steering-DiscreteHID-Bluefruit) |
| Turn & lean-based steering with MPU6050 | [`OVS-MotionIMU`](https://github.com/Berg0162/Open-Virtual-Steering-MotionIMU) | [`OVS-MotionIMU-Bluefruit`](https://github.com/Berg0162/Open-Virtual-Steering-MotionIMU-Bluefruit) |
| Voice-activated steering with TinyML | [`OVS-VoiceControl`](https://github.com/Berg0162/Open-Virtual-Steering-VoiceControl) | ℹ️ |
> ℹ️ The **VoiceControl** project is tightly coupled to the **XIAO ESP32S3 Sense**, which includes a built-in microphone and supports only **ESP32 BLE-Hosts**. A Bluefruit version is not applicable.

## ESP32 BLE-Hosts

There are two hosts, **ESP-Bluedroid** and **ESP-NimBLE**. The major difference between them is as follows:

+ Although both support Bluetooth LE, **ESP-NimBLE** requires less heap and flash size.

+ **ESP-Bluedroid** supports both Classic Bluetooth and Bluetooth LE, while **ESP-NimBLE** only supports Bluetooth LE.

## ❓ What is ESP-Bluedroid?
**Espressif** has modified a version of the native Android Bluetooth stack: **ESP-Bluedroid**. When you have installed the **Arduino core for ESP32**, the default BLE-Host is **ESP-Bluedroid**. The Bluetooth LE stack in ESP-IDF is a layered architecture that enables Bluetooth functionality on ESP32 chip series. It consists of two layers: the Bluetooth Upper Layer (BTU) and the Bluetooth Transport Controller layer (BTC).<br>
ESP-Bluedroid for ESP32 supports Classic Bluetooth and Bluetooth LE.

+ API references

[Bluetooth® Common](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/bt_common.html)

[Bluetooth LE](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/bt_le.html)

[Bluetooth LE 4.2 Application Examples](https://github.com/espressif/esp-idf/tree/v5.4.1/examples/bluetooth/bluedroid/ble)

## 🔧 How the code works

With the **BLESteeringServer** library installed, developers can easily create applications that act as **dedicated BLE steering controllers** for use with **virtual cycling platforms**.

The user application is responsible for interfacing with a **Human Interface Device (HID)** — such as buttons, a joystick, an IMU, or voice input — and regularly passing steering data to the BLESteeringServer.

Once paired with a BLE client, the application uses `BLESteeringServer::updateSteeringValue()` to transmit real-time **steering angle data** over Bluetooth.

To ensure **interoperability**, the library implements a known **BLE Steering Profile**, which defines a consistent structure of services and characteristics used in commercial BLE steering devices. BLE profiles serve as an **application-layer contract** between the controller (peripheral) and the receiving client (central), ensuring compatibility across devices and platforms.

<details>
<summary>🔍 More Info</summary>
    
[Introduction on BLE profiles, services, characteristics, device roles and network topology](https://embeddedcentric.com/lesson-2-ble-profiles-services-characteristics-device-roles-and-network-topology/)

</details>

## 🧱 Dependencies

+ [Arduino core for ESP32](https://github.com/espressif/arduino-esp32)

+ **Supported MCU's** with **Bluedroid**
    - Espressif: ESP32, ESP32-S3, ESP32-C2, ESP32-C3, ESP32-C6, ESP32-H2

## 🚴‍♂️ Quick start?

+ Install the **BLESteeringServer library** from this repository. Download as `.zip` and extract to `Arduino/libraries` folder, or <br>in <b>Arduino IDE</b> from `Sketch menu` -> `Include library` -> `Add .Zip library`<br>
+ Select one of the _**Related Repositories**_ that apply this **shared backend** and offer you **ready-to-use exemplary access** to a selection of different **Human-Interface-Devices**.

## ⚠️ Disclaimer
<details>
<summary> 💡 <b>Research & Independence</b></summary>
This project is <b>not affiliated with, endorsed by, or associated with any commercial virtual cycling platform or steering device manufacturer</b>. It is a <b>research and interoperability</b> initiative designed to explore <b>alternative human interface methods</b> in the context of indoor cycling. All development is conducted independently for <b>educational and experimental purposes</b>.
</details>
<details>
<summary> ✅ <b>Compliance & Responsibility</b></summary> 
This repository does <b>not include or promote any circumvention of technological protection measures</b>, reverse engineering of proprietary software, or unauthorized access to restricted systems. Users are <b>solely responsible</b> for ensuring that their use of this code complies with <b>local laws, software licenses, and platform terms of service</b>.
</details>
<details>
<summary> 🔍 <b>Copyright & Contact</b></summary>
If you are a <b>rights holder</b> and believe that this project includes content that <b>violates your intellectual property rights</b>, please <b>open an issue</b> on this repository to initiate a respectful review. We are committed to responding promptly and, if appropriate, taking corrective action.
</details>

## 🧪 Testing & Validation
The functionality of **Open Virtual Steering with BLESteeringServer** was tested using **standard BLE debugging tools** to ensure proper communication and responsiveness. One of the primary tools used was **nRF Connect for Mobile**, a widely available application for **scanning, connecting, and interacting with BLE devices**. This allowed for **verification of characteristic read/write operations, response timing, and general stability** of the BLE communication.  

## 🔧 Basic Usage

```cpp
#include <BLESteeringServer.h>

void setup() {
    BLESteeringServer::getInstance().begin();
}

void loop() {
    float angle = getSteeringAngleFromHID();
    BLESteeringServer::getInstance().updateSteeringValue(angle);
}
```
---

<details>
<summary>⚖️ <b>Legal Notice (EU Context)</b></summary>

This project is developed and published in accordance with **EU directives** that recognize the right to study, test, and develop software components for the purpose of achieving **interoperability** (e.g., Directive 2009/24/EC on the legal protection of computer programs, Article 6).  

No part of this project is intended to **infringe upon intellectual property rights** or violate technological protection measures. All content is shared in good faith under the belief that it falls within the bounds of **legitimate research, reverse engineering for interoperability, and fair use under EU law**.  

Users must ensure their own compliance with **national implementations of EU directives**, and are responsible for how they apply or modify this code.

</details>

---
