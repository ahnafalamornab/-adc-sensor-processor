#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "io.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ADC_Sensor_Processor <filename>\n");
        return 1;
    }

    int count = 0;
    ADCSample *samples = load_binary(argv[1], &count);

    if (samples == NULL) {
        printf("Failed to load file\n");
        return 1;
    }

    printf("File loaded successfully\n");
    printf("Total records: %d\n", count);

    analyse_channels(samples, count);
    detect_faults(samples, count);
    check_sequence(samples, count);

    export_results("results.txt", samples, count);

    free(samples);
    return 0;
}