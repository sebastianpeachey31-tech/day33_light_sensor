#include "adc_record.h"

#include <stdio.h>
#include <string.h>

static int CheckDecode(
    const char *name,
    const uint8_t *bytes,
    size_t length,
    int null_output,
    ADCRecordStatus expected_status,
    uint32_t expected_raw,
    uint32_t expected_avg
)
{
    ADCRecord out = {123U, 456U};
    ADCRecordStatus status = DecodeADCRecord(
        bytes, length, null_output ? NULL : &out
    );

    if (status != expected_status ||
        out.raw != expected_raw ||
        out.avg != expected_avg)
    {
        fprintf(stderr, "FAIL: %s (status=%d, raw=%lu, avg=%lu)\n",
                name, (int)status,
                (unsigned long)out.raw, (unsigned long)out.avg);
        return 1;
    }

    return 0;
}

int main(void)
{
    static const struct
    {
        const char *name;
        uint8_t bytes[ADC_RECORD_SIZE];
        ADCRecordStatus status;
        uint32_t raw;
        uint32_t avg;
    } cases[] = {
        {"normal", {0xA5, 0x0C, 0, 0, 0xA3, 0x0C, 0, 0},
         ADC_RECORD_DECODED_UNVERIFIED, 3237U, 3235U},
        {"zero is not empty", {0},
         ADC_RECORD_DECODED_UNVERIFIED, 0U, 0U},
        {"maximum ADC values", {0xFF, 0x0F, 0, 0, 0xFF, 0x0F, 0, 0},
         ADC_RECORD_DECODED_UNVERIFIED, 4095U, 4095U},
        {"little endian and field order", {1, 0, 0, 0, 2, 0, 0, 0},
         ADC_RECORD_DECODED_UNVERIFIED, 1U, 2U},
        {"some FF bytes are not empty", {0xFF, 0, 0, 0, 1, 0, 0, 0},
         ADC_RECORD_DECODED_UNVERIFIED, 255U, 1U},
        {"erased record", {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
         ADC_RECORD_EMPTY, 123U, 456U},
        {"RAW 4096", {0, 0x10, 0, 0, 0, 0, 0, 0},
         ADC_RECORD_OUT_OF_RANGE, 123U, 456U},
        {"AVG 4096", {0, 0, 0, 0, 0, 0x10, 0, 0},
         ADC_RECORD_OUT_OF_RANGE, 123U, 456U},
        {"RAW 65536", {0, 0, 1, 0, 0, 0, 0, 0},
         ADC_RECORD_OUT_OF_RANGE, 123U, 456U},
        {"RAW high bit", {0, 0, 0, 0x80, 0, 0, 0, 0},
         ADC_RECORD_OUT_OF_RANGE, 123U, 456U},
        {"RAW 1 and AVG all FF", {1, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF},
         ADC_RECORD_OUT_OF_RANGE, 123U, 456U}
    };

    int failures = 0;
    size_t tests = 0;

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
    {
        uint8_t input[ADC_RECORD_SIZE];
        memcpy(input, cases[i].bytes, sizeof(input));

        failures += CheckDecode(
            cases[i].name, input, sizeof(input), 0,
            cases[i].status, cases[i].raw, cases[i].avg
        );

        if (memcmp(input, cases[i].bytes, sizeof(input)) != 0)
        {
            fprintf(stderr, "FAIL: input changed in %s\n", cases[i].name);
            failures++;
        }
        tests++;
    }

    static const uint8_t short_input[7] = {0};
    static const uint8_t long_input[9] = {0};
    static const size_t bad_lengths[] = {0U, 9U, SIZE_MAX};

    failures += CheckDecode(
        "short input", short_input, sizeof(short_input), 0,
        ADC_RECORD_BAD_LENGTH, 123U, 456U
    );
    tests++;

    for (size_t i = 0; i < sizeof(bad_lengths) / sizeof(bad_lengths[0]); i++)
    {
        failures += CheckDecode(
            "bad length", long_input, bad_lengths[i], 0,
            ADC_RECORD_BAD_LENGTH, 123U, 456U
        );
        tests++;
    }

    failures += CheckDecode(
        "null input", NULL, ADC_RECORD_SIZE, 0,
        ADC_RECORD_BAD_ARGUMENT, 123U, 456U
    );
    failures += CheckDecode(
        "null output", cases[0].bytes, ADC_RECORD_SIZE, 1,
        ADC_RECORD_BAD_ARGUMENT, 123U, 456U
    );
    failures += CheckDecode(
        "null pointers take precedence over length", NULL, 0U, 1,
        ADC_RECORD_BAD_ARGUMENT, 123U, 456U
    );
    tests += 3U;

    /* Repeat with a byte-offset input; no uint32_t alignment is required. */
    uint8_t offset_input[ADC_RECORD_SIZE + 1U] = {0};
    memcpy(offset_input + 1, cases[0].bytes, ADC_RECORD_SIZE);
    failures += CheckDecode(
        "byte-offset input", offset_input + 1, ADC_RECORD_SIZE, 0,
        ADC_RECORD_DECODED_UNVERIFIED, 3237U, 3235U
    );
    tests++;

    printf("DecodeADCRecord: %lu tests, %d failures\n",
           (unsigned long)tests, failures);
    return failures == 0 ? 0 : 1;
}
