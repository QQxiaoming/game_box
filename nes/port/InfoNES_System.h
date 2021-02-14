/*===================================================================*/
/*                                                                   */
/*  InfoNES_System.h : The function which depends on a system        */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_SYSTEM_H_INCLUDED
#define InfoNES_SYSTEM_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/
#include <stdint.h>

/*-------------------------------------------------------------------*/
/*  Palette data                                                     */
/*-------------------------------------------------------------------*/
extern uint16_t NesPalette[];

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

/* Menu screen */
int InfoNES_Menu(void);

/* Read ROM image file */
int InfoNES_ReadRom(const char *pszFileName);

/* Release a memory for ROM */
void InfoNES_ReleaseRom(void);

/* Transfer the contents of work frame on the screen */
void InfoNES_LoadFrame(void);

/* Get a joypad state */
void InfoNES_PadState(uint32_t *pdwPad1, uint32_t *pdwPad2, uint32_t *pdwSystem);

/* memcpy */
void *InfoNES_MemoryCopy(void *dest, const void *src, int count);

/* memset */
void *InfoNES_MemorySet(void *dest, int c, int count);

/* Wait */
void InfoNES_Wait(void);

/* Sound Initialize */
void InfoNES_SoundInit(void);

/* Sound Open */
int InfoNES_SoundOpen(int samples_per_sync, int sample_rate);

/* Sound Close */
void InfoNES_SoundClose(void);

/* Sound Output 5 Waves - 2 Pulse, 1 Triangle, 1 Noise, 1 DPCM */
void InfoNES_SoundOutput(int samples, uint8_t *wave1, uint8_t *wave2, uint8_t *wave3,
                         uint8_t *wave4, uint8_t *wave5);

/* Print system message */
void InfoNES_MessageBox(const char *pszMsg, ...);

#endif /* !InfoNES_SYSTEM_H_INCLUDED */
