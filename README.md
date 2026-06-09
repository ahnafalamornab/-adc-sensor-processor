# ADC Sensor Processor
UGMFGT-15-1 Programming for Engineers - Resit Coursework
University of the West of England, UWE Bristol
Student: Ahnaf Ornab | ID: a2ornab | 25043740

## Project Description
A command-line C program that reads a binary ADC sensor log file and
produces a structured analysis report. The program reads 4000 records
from a 4-channel 12-bit ADC sensor running at 1000 Hz.

## What the program does
- Reads and validates the binary file header including magic number check
- Loads all 4000 records using fread with a packed struct
- Converts raw ADC values to voltage using: voltage = (raw_value / 4095.0) * 3.3
- Computes per-channel statistics: mean, min, max, standard deviation
- Detects faults: overvoltage above 3.0V, undervoltage below 0.3V, sensor flags
- Checks sequence number integrity and reports missing records
- Detects temperature anomalies above 50C or below 10C
- Exports a structured results report to results.txt

## File Structure
- main.c       entry point, loads data, calls all analysis functions
- adc.c/adc.h  voltage conversion, statistics, fault detection, sequence check
- io.c/io.h    binary file reading and results export
- stats.c/h    mean, min, max, standard deviation calculations

## How to build
Requires CMake 3.20 and a C99 compiler.

mkdir build
cd build
cmake ..
make
./ADC_Sensor_Processor adc_sensor_log.bin

## How to run in CLion
1. Open project in CLion
2. Set program argument to: adc_sensor_log.bin
3. Set working directory to project root
4. Click the green play button

## Expected output
- 4000 records loaded
- Channel 0: 0 faults
- Channel 1: 6 faults overvoltage
- Channel 2: 7 faults undervoltage
- Channel 3: 7 faults sensor flag
- 2 sequence gaps detected
- 24 temperature anomalies detected

## GitHub Repository
https://github.com/ahnafalamornab/-adc-sensor-processor