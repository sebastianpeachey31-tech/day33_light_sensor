#include "average.h"
#include <stddef.h>

AverageStatus CalculateAverage(
    const int *data,
    int count,
    int *result
)
{
    if (data == NULL || result == NULL)
    {
        return AVG_ERR_NULL;
    }

    if (count <= 0)
    {
        return AVG_ERR_COUNT;
    }

    int64_t sum = 0;

    for (int i = 0; i < count; i++)
    {
        sum += data[i];
    }

    *result = (int)(sum / count);

    return AVG_OK;
}

AverageStatus CalculateADCAverage(
    const uint32_t *data,
    int count,
    uint32_t *result
)
{
    if (data == NULL || result == NULL)
    {
        return AVG_ERR_NULL;
    }

    if (count <= 0)
    {
        return AVG_ERR_COUNT;
    }

    uint64_t sum = 0;

    for (int i = 0; i < count; i++)
    {
        sum += data[i];
    }

    *result = (uint32_t)(sum / (uint64_t)count);

    return AVG_OK;
}