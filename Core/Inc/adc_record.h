#ifndef ADC_RECORD_H
#define ADC_RECORD_H

#include <stddef.h>
#include <stdint.h>

#define ADC_RECORD_SIZE 8U

typedef struct
{
    uint32_t raw;
    uint32_t avg;
} ADCRecord;

typedef enum
{
    ADC_RECORD_DECODED_UNVERIFIED = 0,
    ADC_RECORD_EMPTY,
    ADC_RECORD_OUT_OF_RANGE,
    ADC_RECORD_BAD_LENGTH,
    ADC_RECORD_BAD_ARGUMENT
} ADCRecordStatus;

/*
 * Decode exactly eight bytes: little-endian uint32_t RAW, then AVG.
 * Check pointers, length, erased state (all FF), then the 0..4095 range.
 * Only DECODED_UNVERIFIED updates *out; every other status leaves it unchanged.
 * The stored format cannot establish integrity or identify a partial write.
 * The caller must provide readable input and writable output storage, and must
 * check the returned status before using the output.
 */
ADCRecordStatus DecodeADCRecord(
    const uint8_t *bytes,
    size_t length,
    ADCRecord *out
);

#endif
