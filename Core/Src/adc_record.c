#include "adc_record.h"

ADCRecordStatus DecodeADCRecord(
    const uint8_t *bytes,
    size_t length,
    ADCRecord *out
)
{
    if (bytes == NULL || out == NULL)
    {
        return ADC_RECORD_BAD_ARGUMENT;
    }

    if (length != ADC_RECORD_SIZE)
    {
        return ADC_RECORD_BAD_LENGTH;
    }

    int is_empty = 1;

    for (size_t i = 0; i < ADC_RECORD_SIZE; i++)
    {
        if (bytes[i] != 0xFFU)
        {
            is_empty = 0;
            break;
        }
    }

    if (is_empty)
    {
        return ADC_RECORD_EMPTY;
    }

    ADCRecord decoded;

    decoded.raw =
        (uint32_t)bytes[0] |
        ((uint32_t)bytes[1] << 8) |
        ((uint32_t)bytes[2] << 16) |
        ((uint32_t)bytes[3] << 24);

    decoded.avg =
        (uint32_t)bytes[4] |
        ((uint32_t)bytes[5] << 8) |
        ((uint32_t)bytes[6] << 16) |
        ((uint32_t)bytes[7] << 24);

    if (decoded.raw > 4095U || decoded.avg > 4095U)
    {
        return ADC_RECORD_OUT_OF_RANGE;
    }

    *out = decoded;
    return ADC_RECORD_DECODED_UNVERIFIED;
}
