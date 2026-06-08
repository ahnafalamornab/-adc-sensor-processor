#include "stats.h"
#include <math.h>

double compute_mean(double *values, int count) {
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    return sum / count;
}

double compute_min(double *values, int count) {
    double min = values[0];
    for (int i = 1; i < count; i++) {
        if (values[i] < min) {
            min = values[i];
        }
    }
    return min;
}

double compute_max(double *values, int count) {
    double max = values[0];
    for (int i = 1; i < count; i++) {
        if (values[i] > max) {
            max = values[i];
        }
    }
    return max;
}

double compute_stddev(double *values, int count) {
    double mean = compute_mean(values, count);
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        double diff = values[i] - mean;
        sum += diff * diff;
    }
    return sqrt(sum / count);
}