#include "io.h"
#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

ADCSample *load_binary(const char *filename, int *count) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: could not open file %s\n", filename);
        return NULL;
    }

    uint32_t magic;
    uint16_t version;
    uint16_t channel_count;
    uint32_t record_count;
    uint32_t sample_rate;
    uint8_t reserved[8];

    fread(&magic, sizeof(uint32_t), 1, file);
    if (magic != 0xADC1BEEF) {
        printf("Error: invalid magic number\n");
        fclose(file);
        return NULL;
    }

    fread(&version, sizeof(uint16_t), 1, file);
    fread(&channel_count, sizeof(uint16_t), 1, file);
    fread(&record_count, sizeof(uint32_t), 1, file);
    fread(&sample_rate, sizeof(uint32_t), 1, file);
    fread(reserved, sizeof(uint8_t), 8, file);

    printf("Magic number : 0xADC1BEEF - valid\n");
    printf("Version      : %d\n", version);
    printf("Channels     : %d\n", channel_count);
    printf("Records      : %d\n", record_count);
    printf("Sample rate  : %d Hz\n", sample_rate);

    ADCSample *samples = malloc(record_count * sizeof(ADCSample));
    if (samples == NULL) {
        printf("Error: could not allocate memory\n");
        fclose(file);
        return NULL;
    }

    fread(samples, sizeof(ADCSample), record_count, file);
    fclose(file);
    *count = (int)record_count;
    return samples;
}

void export_results(const char *filename, ADCSample *samples, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: could not create results file\n");
        return;
    }

    fprintf(file, "=== ADC Sensor Analysis Report ===\n\n");
    fprintf(file, "Total records loaded : %d\n", count);
    fprintf(file, "Channels analysed    : 4\n");
    fprintf(file, "Sample rate          : 1000 Hz\n");
    fprintf(file, "Voltage reference    : 3.3 V\n");
    fprintf(file, "ADC resolution       : 12-bit (0 to 4095)\n\n");

    fprintf(file, "--- Per Channel Statistics ---\n");
    int ch;
    for (ch = 0; ch < 4; ch++) {
        int ch_count = 0;
        int overvoltage = 0;
        int undervoltage = 0;
        int sensor_fault = 0;
        double sum = 0.0;
        double min_v = 9999.0;
        double max_v = -9999.0;
        int i;

        for (i = 0; i < count; i++) {
            if (samples[i].channel_id == ch) {
                double v = (samples[i].raw_value / 4095.0) * 3.3;
                sum += v;
                if (v < min_v) min_v = v;
                if (v > max_v) max_v = v;
                ch_count++;
                if (v > 3.0) overvoltage++;
                if (v < 0.3) undervoltage++;
                if (samples[i].status_flags & 0x01) sensor_fault++;
            }
        }

        double mean = sum / ch_count;
        double sq_sum = 0.0;

        for (i = 0; i < count; i++) {
            if (samples[i].channel_id == ch) {
                double v = (samples[i].raw_value / 4095.0) * 3.3;
                double diff = v - mean;
                sq_sum += diff * diff;
            }
        }

        double stddev = sqrt(sq_sum / ch_count);

        fprintf(file, "\nChannel %d:\n", ch);
        fprintf(file, "  Samples       : %d\n", ch_count);
        fprintf(file, "  Mean voltage  : %.4f V\n", mean);
        fprintf(file, "  Min voltage   : %.4f V\n", min_v);
        fprintf(file, "  Max voltage   : %.4f V\n", max_v);
        fprintf(file, "  Std deviation : %.4f V\n", stddev);
        fprintf(file, "  Overvoltage   : %d\n", overvoltage);
        fprintf(file, "  Undervoltage  : %d\n", undervoltage);
        fprintf(file, "  Sensor fault  : %d\n", sensor_fault);
        fprintf(file, "  Total faults  : %d\n", overvoltage + undervoltage + sensor_fault);
    }

    fprintf(file, "\n--- Sequence Integrity ---\n");
    int gaps = 0;
    int i;
    for (i = 1; i < count; i++) {
        if (samples[i].sequence_number != samples[i-1].sequence_number + 1) {
            gaps++;
            fprintf(file, "Gap: missing between seq %d and %d\n",
                    samples[i-1].sequence_number,
                    samples[i].sequence_number);
        }
    }
    if (gaps == 0) {
        fprintf(file, "No sequence gaps detected\n");
    } else {
        fprintf(file, "Total gaps: %d\n", gaps);
    }

    fprintf(file, "\n--- Temperature Anomalies ---\n");
    int temp_anomalies = 0;
    for (i = 0; i < count; i++) {
        double temp = samples[i].temperature / 10.0;
        if (temp > 50.0 || temp < 10.0) {
            temp_anomalies++;
            fprintf(file, "Anomaly: seq %d ch %d temp %.1f C\n",
                    samples[i].sequence_number,
                    samples[i].channel_id,
                    temp);
        }
    }
    if (temp_anomalies == 0) {
        fprintf(file, "No temperature anomalies\n");
    } else {
        fprintf(file, "Total anomalies: %d\n", temp_anomalies);
    }

    fclose(file);
    printf("Results saved to %s\n", filename);
}