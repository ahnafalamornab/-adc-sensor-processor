#include "adc.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>

/* converts raw ADC values to voltage using formula voltage = (raw / 4095.0) * 3.3 */
void convert_voltages(ADCSample *samples, int count) {
    for (int i = 0; i < count; i++) {
        double v = (samples[i].raw_value / 4095.0) * 3.3;
        (void)v;
    }
}

/* calculates mean min max stddev and fault count for one channel */
ChannelStats get_channel_stats(ADCSample *samples, int count, int channel) {
    ChannelStats cs;
    cs.channel = channel;
    cs.fault_count = 0;
    cs.sample_count = 0;

    for (int i = 0; i < count; i++) {
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

/* checks each sample for overvoltage undervoltage and sensor flag faults */
void detect_faults(ADCSample *samples, int count) {
    printf("\n--- Fault Detection ---\n");
    for (int ch = 0; ch < 4; ch++) {
        ChannelStats cs = get_channel_stats(samples, count, ch);
        printf("Channel %d: %d faults\n", ch, cs.fault_count);
    }
}

/* verifies sequence numbers increment by 1 and reports any gaps */
void check_sequence(ADCSample *samples, int count) {
    int gaps = 0;
    printf("\n--- Sequence Integrity Check ---\n");
    for (int i = 1; i < count; i++) {
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

/* prints per channel statistics to the console */
void analyse_channels(ADCSample *samples, int count) {
    printf("\n--- Per Channel Statistics ---\n");
    for (int ch = 0; ch < 4; ch++) {
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

/* detects temperature readings above 50C or below 10C */
void check_temperature(ADCSample *samples, int count) {
    printf("\n--- Temperature Check ---\n");
    int anomalies = 0;
    for (int i = 0; i < count; i++) {
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