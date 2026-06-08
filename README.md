# ADC Sensor Processor
UGMFGT-15-1 Programming for Engineers - Resit Coursework
University of the West of England, UWE Bristol
Student: Ahnaf Ornab | ID: a2ornab

## What this program does
Reads a binary ADC sensor log file and produces a structured analysis report.
It computes per-channel statistics, detects faults, and checks sequence integrity.

## How to build in CLion
1. Open the project in CLion
2. Click Build > Build Project
3. Run the executable

## How to build with gcc
gcc -o ADC_Sensor_Processor main.c adc.c io.c stats.c -lm

## How to run
./ADC_Sensor_Processor adc_sensor_log.bin

## GitHub Repository
https://github.com/ahnafalamornab/adc-sensor-processor