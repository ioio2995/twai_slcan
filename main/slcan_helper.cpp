#include "slcan_helper.hpp"


char hex_digit(const uint8_t b)
{
    static const char* hex_tbl = "0123456789abcdef";
    return hex_tbl[b & 0x0f];
}

void hex_write(char** p, const uint8_t* data, uint8_t len)
{
    unsigned int i;
    for (i = 0; i < len; i++) {
        *(*p)++ = hex_digit(data[i] >> 4);
        *(*p)++ = hex_digit(data[i]);
    }
}

uint8_t hex_val(char c)
{
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 0xA;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 0xa;
    } else {
        return (c - '0') & 0xf;
    }
}

uint32_t hex_to_u32(const char* str, uint8_t len)
{
    uint32_t val = 0;
    unsigned int i;
    for (i = 0; i < len; i++) {
        val = (val << 4) | hex_val(str[i]);
    }
    return val;
}

uint8_t hex_to_u8(const char* str)
{
    uint8_t val;
    val = hex_val(*str++);
    val = (val << 4) | hex_val(*str);
    return val;
}

void hex_to_u8_array(const char* str, uint8_t* buf, size_t len)
{
    while (len-- > 0) {
        *buf++ = hex_to_u8(str);
        str += 2;
    }
}
