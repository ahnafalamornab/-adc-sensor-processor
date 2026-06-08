#include "io.h"
#include <stdio.h>
#include <stdlib.h>

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
        printf("Error: wrong magic number\n");
        fclose(file);
        return NULL;
    }

    fread(&version, sizeof(uint16_t), 1, file);
    fread(&channel_count, sizeof(uint16_t), 1, file);
    fread(&record_count, sizeof(uint32_t), 1, file);
    fread(&sample_rate, sizeof(uint32_t), 1, file);
    fread(reserved, sizeof(uint8_t), 8, file);

    ADCSample *samples = malloc(record_count * sizeof(ADCSample));
    if (samples == NULL) {
        printf("Error: could not allocate memory\n");
        fclose(file);
        return NULL;
    }

    fread(samples, sizeof(ADCSample), record_count, file);

    fclose(file);
    *count = record_count;
    return samples;
}

void export_results(const char *filename, ADCSample *samples, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: could not create results file\n");
        return;
    }

    fprintf(file, "--- ADC Sensor Analysis Report ---\n\n");
    fprintf(file, "Total records: %d\n\n", count);

    for (int ch = 0; ch < 4; ch++) {
        int ch_count = 0;
        int fault_count = 0;

        for (int i = 0; i < count; i++) {
            if (samples[i].channel_id == ch) {
                ch_count++;
                double voltage = (samples[i].raw_value / 4095.0) * 3.3;
                if (voltage > 3.0) fault_count++;
                if (voltage < 0.3) fault_count++;
                if (samples[i].status_flags & 0x01) fault_count++;
            }
        }

        fprintf(file, "Channel %d: %d samples, %d faults\n", ch, ch_count, fault_count);
    }

    fprintf(file, "\n");

    int gaps = 0;
    for (int i = 1; i < count; i++) {
        if (samples[i].sequence_number != samples[i-1].sequence_number + 1) {
            gaps++;
            fprintf(file, "Sequence gap: missing between %d and %d\n",
                    samples[i-1].sequence_number, samples[i].sequence_number);
        }
    }

    if (gaps == 0) {
        fprintf(file, "No sequence gaps found\n");
    }

    fclose(file);
    printf("Results saved to %s\n", filename);
}