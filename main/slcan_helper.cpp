#include "slcan_helper.hpp"

/** \fn hex_digit
 *  \brief
 *       Converts a 4-bit value to its hexadecimal character representation
 * 
 *  \param b
 *       The 4-bit value to convert
 *  \return
 *       Hexadecimal character
 */
char hex_digit(const uint8_t b)
{
    static const char* hex_tbl = "0123456789abcdef";
    return hex_tbl[b & 0x0f];
}

/** \fn hex_write
 *  \brief
 *       Writes hexadecimal representation of data to a character buffer
 * 
 *  \param p
 *       Pointer to the character buffer (output)
 *  \param data
 *       Pointer to the data to be converted to hexadecimal
 *  \param len
 *       Length of the data
 */
void hex_write(char** p, const uint8_t* data, uint8_t len)
{
    unsigned int i;
    for (i = 0; i < len; i++) {
        *(*p)++ = hex_digit(data[i] >> 4);
        *(*p)++ = hex_digit(data[i]);
    }
}

/** \fn hex_val
 *  \brief
 *       Converts a hexadecimal character to its numeric value
 * 
 *  \param c
 *       Hexadecimal character
 *  \return
 *       Numeric value
 */
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

/** \fn hex_to_u32
 *  \brief
 *       Converts a hexadecimal string to a 32-bit unsigned integer
 * 
 *  \param str
 *       Hexadecimal string
 *  \param len
 *       Length of the string
 *  \return
 *       32-bit unsigned integer value
 */
uint32_t hex_to_u32(const char* str, uint8_t len)
{
    uint32_t val = 0;
    unsigned int i;
    for (i = 0; i < len; i++) {
        val = (val << 4) | hex_val(str[i]);
    }
    return val;
}

/** \fn hex_to_u8
 *  \brief
 *       Converts a two-character hexadecimal string to an 8-bit unsigned integer
 * 
 *  \param str
 *       Hexadecimal string
 *  \return
 *       8-bit unsigned integer value
 */
uint8_t hex_to_u8(const char* str)
{
    uint8_t val;
    val = hex_val(*str++);
    val = (val << 4) | hex_val(*str);
    return val;
}


/** \fn hex_to_u8_array
 *  \brief
 *       Converts a hexadecimal string to an array of 8-bit unsigned integers
 * 
 *  \param str
 *       Hexadecimal string
 *  \param buf
 *       Pointer to the buffer for the resulting array
 *  \param len
 *       Length of the array
 */
void hex_to_u8_array(const char* str, uint8_t* buf, size_t len)
{
    while (len-- > 0) {
        *buf++ = hex_to_u8(str);
        str += 2;
    }
}
