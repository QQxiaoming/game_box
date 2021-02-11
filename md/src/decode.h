/* Header file for decode functions */
#ifndef __GENIE_DECODE_H__
#define __GENIE_DECODE_H__

struct patch { unsigned int addr, data; };

void genie_decode(const char *code, struct patch *result);
void hex_decode(const char *code, struct patch *result);
void decode(const char *code, struct patch *result);

#endif // __GENIE_DECODE_H__
