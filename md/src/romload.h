#ifndef ROMLOAD_H_
#define ROMLOAD_H_

#include <stddef.h>
#include <stdint.h>

extern uint8_t *load_rom(size_t *rom_size, const char *name);
extern void unload_rom(uint8_t *rom);
extern void set_rom_path(const char *path);

#endif /* ROMLOAD_H_ */
