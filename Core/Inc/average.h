#ifndef AVERAGE_H
#define AVERAGE_H

#include <stdint.h>

typedef enum
{
    AVG_OK = 0,
    AVG_ERR_NULL = -1,
    AVG_ERR_COUNT = -2
} AverageStatus;

AverageStatus CalculateAverage(
    const int *data,
    int count,
    int *result
);

AverageStatus CalculateADCAverage(
    const uint32_t *data,
    int count,
    uint32_t *result
);

#endif