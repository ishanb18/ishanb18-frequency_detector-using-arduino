
# Frequency Detector using Arduino

## ✅ Project Overview

This project detects and displays the frequency of an input digital signal using an Arduino. It counts input pulses over a fixed time interval to calculate and report the signal frequency in Hertz (Hz).

## ⚙️ Features

- Detects frequency of square wave or digital pulses
- Simple and efficient code using interrupts or polling
- Frequency output via Serial Monitor

## 🧰 Hardware Requirements

- Arduino Uno, Nano, or compatible board
- Signal source (function generator or 555 timer)
- Jumper wires
- USB cable

## 📁 Project Files

- `freq_detector.ino` – Main Arduino sketch

## 🔧 How It Works

1. Connect the signal to a designated digital input pin (e.g., D2).
2. Arduino counts pulses over a set time (e.g., 1 second).
3. Frequency is calculated as:
   ```c
   frequency = pulseCount / timeInterval;
   ```
4. Frequency is printed to the Serial Monitor.

## 🛠️ Setup Instructions

1. Open `freq_detector.ino` using the Arduino IDE.
2. Upload the code to your board.
3. Connect an input signal (0–5V square wave) to pin D2.
4. Open Serial Monitor (9600 baud) to view frequency output.

## 📜 License

Open source and free for academic or hobby use.
