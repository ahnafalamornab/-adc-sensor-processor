#include "adc.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>

void convert_voltages(ADCSample *samples, int count) {
    for (int i = 0; i < count; i++) {
        double voltage = (samples[i].raw_value / 4095.0) * 3.3;
        (void)voltage;
    }
}

void detect_faults(ADCSample *samples, int count) {
    int fault_count[4] = {0, 0, 0, 0};

    for (int i = 0; i < count; i++) {
        double voltage = (samples[i].raw_value / 4095.0) * 3.3;
        int ch = samples[i].channel_id;

        if (voltage > 3.0) {
            fault_count[ch]++;
        }
        if (voltage < 0.3) {
            fault_count[ch]++;
        }
        if (samples[i].status_flags & 0x01) {
            fault_count[ch]++;
        }
    }

    printf("\n--- Fault Detection ---\n");
    for (int ch = 0; ch < 4; ch++) {
        printf("Channel %d: %d faults\n", ch, fault_count[ch]);
    }
}

void check_sequence(ADCSample *samples, int count) {
    int gaps = 0;

    printf("\n--- Sequence Check ---\n");
    for (int i = 1; i < count; i++) {
        if (samples[i].sequence_number != samples[i-1].sequence_number + 1) {
            gaps++;
            printf("Gap found: missing between seq %d and %d\n",
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
    printf("\n--- Per Channel Statistics ---\n");

    for (int ch = 0; ch < 4; ch++) {
        int ch_count = 0;

        for (int i = 0; i < count; i++) {
            if (samples[i].channel_id == ch) {
                ch_count++;
            }
        }

        double *voltages = malloc(ch_count * sizeof(double));
        int idx = 0;

        for (int i = 0; i < count; i++) {
            if (samples[i].channel_id == ch) {
                voltages[idx] = (samples[i].raw_value / 4095.0) * 3.3;
                idx++;
            }
        }

        printf("Channel %d:\n", ch);
        printf("  Samples : %d\n", ch_count);
        printf("  Mean    : %.4f V\n", compute_mean(voltages, ch_count));
        printf("  Min     : %.4f V\n", compute_min(voltages, ch_count));
        printf("  Max     : %.4f V\n", compute_max(voltages, ch_count));
        printf("  Std Dev : %.4f V\n", compute_stddev(voltages, ch_count));

        free(voltages);
    }
}