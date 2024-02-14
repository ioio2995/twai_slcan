#ifndef SLCAN_H
#define SLCAN_H

#include <stddef.h>

#define CR '\r'
#define BELL '\a'

void slcan_rx_spin(void *arg);

void slcan_decode_line(char* line);
int slcan_serial_write(int itf,const char* buf, size_t len);

#endif // SLCAN_H

