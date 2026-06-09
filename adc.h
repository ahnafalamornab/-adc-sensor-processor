#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#pragma pack(1)
typedef struct {
    float timestamp;
    uint8_t channel_id;
    uint16_t raw_value;
    int16_t temperature;
    uint8_t status_flags;
    uint32_t sequence_number;
    uint8_t reserved[2];
} ADCSample;
#pragma pack()

typedef struct {
    int channel;
    double mean;
    double min;
    double max;
    double stddev;
    int fault_count;
    double voltages[1000];
    int sample_count;
} ChannelStats;

void convert_voltages(ADCSample *samples, int count);
void detect_faults(ADCSample *samples, int count);
void check_sequence(ADCSample *samples, int count);
void analyse_channels(ADCSample *samples, int count);
ChannelStats get_channel_stats(ADCSample *samples, int count, int channel);

#endif
