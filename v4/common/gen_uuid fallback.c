#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gen_uuid.h"

// Function to generate a random 32-bit unsigned integer
uint32_t generate_random_uint32() { return rand(); }

// Function to generate a version 4 UUID
char *gen_uuid() {
    srand(time(NULL));

    // Generate random numbers for the UUID
    uint32_t r1 = generate_random_uint32();
    uint32_t r2 = generate_random_uint32();
    uint32_t r3 = generate_random_uint32();
    uint32_t r4 = generate_random_uint32();

    // Set the version (bits 12-15 of the time_hi_and_version field to 0100)
    r3 = (r3 & 0x0FFF) | 0x4000;

    // Set the variant (bits 6 and 7 of the clock_seq_hi_and_reserved to 10)
    r4 = (r4 & 0x3FFF) | 0x8000;

    char *uuid = (char *)malloc(37);

    // Format the UUID as a string
    snprintf(uuid, 37, "%08x-%04x-%04x-%04x-%08x%04x", r1, (r2 >> 16) & 0xFFFF,
             r2 & 0xFFFF, (r3 >> 16) & 0xFFFF, r3 & 0xFFFF, r4);
    return uuid;
}
