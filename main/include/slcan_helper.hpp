#ifndef SLCAN_HELPER_H
#define SLCAN_HELPER_H

#include <stdint.h>
#include <stddef.h>

#define SLC_STD_ID_LEN 3
#define SLC_EXT_ID_LEN 8

/* maximum rx buffer len: extended CAN frame with timestamp */
#define MAX_FRAME_LEN (sizeof("T1111222281122334455667788EA5F\r") + 1)

char hex_digit(const uint8_t b);
void hex_write(char** p, const uint8_t* data, uint8_t len);
uint8_t hex_val(char c);
uint32_t hex_to_u32(const char* str, uint8_t len);
uint8_t hex_to_u8(const char* str);
void hex_to_u8_array(const char* str, uint8_t* buf, size_t len);

#endif //SLCAN_HELPER_H