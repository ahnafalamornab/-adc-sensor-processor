#include "adc.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>

void convert_voltages(ADCSample *samples, int count) {
    int i;
    for (i = 0; i < count; i++) {
        double v = (samples[i].raw_value / 4095.0) * 3.3;
        (void)v;
    }
}

ChannelStats get_channel_stats(ADCSample *samples, int count, int channel) {
    ChannelStats cs;
    int i;
    cs.channel = channel;
    cs.fault_count = 0;
    cs.sample_count = 0;

    for (i = 0; i < count; i++) {
        if (samples[i].channel_id == channel) {
            double voltage = (samples[i].raw_value / 4095.0) * 3.3;
            cs.voltages[cs.sample_count] = voltage;
            cs.sample_count++;

            if (voltage > 3.0) cs.fault_count++;
            if (voltage < 0.3) cs.fault_count++;
            if (samples[i].status_flags & 0x01) cs.fault_count++;
        }
    }

    cs.mean   = compute_mean(cs.voltages, cs.sample_count);
    cs.min    = compute_min(cs.voltages, cs.sample_count);
    cs.max    = compute_max(cs.voltages, cs.sample_count);
    cs.stddev = compute_stddev(cs.voltages, cs.sample_count);

    return cs;
}

void detect_faults(ADCSample *samples, int count) {
    int ch;
    printf("\n--- Fault Detection ---\n");
    for (ch = 0; ch < 4; ch++) {
        int overvoltage = 0;
        int undervoltage = 0;
        int sensor_fault = 0;
        int i;

        for (i = 0; i < count; i++) {
            if (samples[i].channel_id == ch) {
                double voltage = (samples[i].raw_value / 4095.0) * 3.3;
                if (voltage > 3.0) overvoltage++;
                if (voltage < 0.3) undervoltage++;
                if (samples[i].status_flags & 0x01) sensor_fault++;
            }
        }

        printf("Channel %d: overvoltage %d  undervoltage %d  sensor fault %d\n",
               ch, overvoltage, undervoltage, sensor_fault);
    }
}

void check_sequence(ADCSample *samples, int count) {
    int gaps = 0;
    int i;
    printf("\n--- Sequence Integrity Check ---\n");
    for (i = 1; i < count; i++) {
        if (samples[i].sequence_number != samples[i-1].sequence_number + 1) {
            gaps++;
            printf("Gap: missing between seq %d and %d\n",
                   samples[i-1].sequence_number,
                   samples[i].sequence_number);
        }
    }
    if (gaps == 0) {
        printf("No sequence gaps found\n");
    } else {
        printf("Total gaps: %d\n", gaps);
    }
}

void analyse_channels(ADCSample *samples, int count) {
    int ch;
    printf("\n--- Per Channel Statistics ---\n");
    for (ch = 0; ch < 4; ch++) {
        ChannelStats cs = get_channel_stats(samples, count, ch);
        printf("Channel %d:\n", ch);
        printf("  Samples : %d\n", cs.sample_count);
        printf("  Mean    : %.4f V\n", cs.mean);
        printf("  Min     : %.4f V\n", cs.min);
        printf("  Max     : %.4f V\n", cs.max);
        printf("  Std Dev : %.4f V\n", cs.stddev);
        printf("  Faults  : %d\n", cs.fault_count);
    }
}

void check_temperature(ADCSample *samples, int count) {
    int anomalies = 0;
    int i;
    printf("\n--- Temperature Check ---\n");
    for (i = 0; i < count; i++) {
        double temp = samples[i].temperature / 10.0;
        if (temp > 50.0 || temp < 10.0) {
            anomalies++;
            printf("Temp anomaly: seq %d ch %d temp %.1f C\n",
                   samples[i].sequence_number,
                   samples[i].channel_id,
                   temp);
        }
    }
    if (anomalies == 0) {
        printf("No temperature anomalies found\n");
    } else {
        printf("Total anomalies: %d\n", anomalies);
    }
}