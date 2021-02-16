#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InfoNES.h"
#include "InfoNES_Mapper.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"
#include "InfoNES_K6502.h"
#include "mainwindow.h"
#include "game_box_misc.h"

static NESThread *g_nesThread;

void InfoNES_start(NESThread *nesThread, const char *pszFileName)
{
    g_nesThread = nesThread;
    g_nesThread->libVersion = INFONES_VER;
    WorkFrame = new unsigned short[256 * 240 * 2];
    memset(WorkFrame, 0x0, 256 * 240 * 2);

    if (0 != InfoNES_Load(pszFileName))
    {
        return;
    }

    InfoNES_Main();

    delete[] WorkFrame;
}

// Palette data
uint16_t NesPalette[64] = {
    0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20, 0x20a0,
    0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000, 0x5ef7, 0x01dd,
    0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521, 0x45c0, 0x0240, 0x02a0,
    0x0247, 0x0211, 0x0000, 0x0000, 0x0000, 0x7fff, 0x1eff, 0x2e5f, 0x223f,
    0x79ff, 0x7dd6, 0x7dcc, 0x7e67, 0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb,
    0x0000, 0x0000, 0x0000, 0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b,
    0x7ef6, 0x7f75, 0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000,
    0x0000

};

/*===================================================================*/
/*                                                                   */
/*                  InfoNES_Menu() : Menu screen                     */
/*                                                                   */
/*===================================================================*/
int InfoNES_Menu(void)
{
    if (PAD_PUSH(PAD_System, PAD_SYS_QUIT))
        return -1;

    return 0;
}

/*===================================================================*/
/*                                                                   */
/*               InfoNES_ReadRom() : Read ROM image file             */
/*                                                                   */
/*===================================================================*/
int InfoNES_ReadRom(const char *pszFileName)
{

    if (-1 == g_nesThread->InfoNES_OpenRom(pszFileName))
    {
        return -1;
    }
    g_nesThread->InfoNES_ReadRom(&NesHeader, sizeof(NesHeader));

    if (memcmp(NesHeader.byID, "NES\x1a", 4) != 0)
    {
        return -1;
    }

    /* Clear SRAM */
    memset(SRAM, 0, SRAM_SIZE);

    if (NesHeader.byInfo1 & 4)
    {
        g_nesThread->InfoNES_ReadRom(&SRAM[0x1000], 512);
    }

    /* Allocate Memory for ROM Image */
    ROM = static_cast<uint8_t *>(malloc(NesHeader.byRomSize * 0x4000));

    /* Read ROM Image */
    g_nesThread->InfoNES_ReadRom(ROM, 0x4000 * NesHeader.byRomSize);

    if (NesHeader.byVRomSize > 0)
    {
        /* Allocate Memory for VROM Image */
        VROM = static_cast<uint8_t *>(malloc(NesHeader.byVRomSize * 0x2000));

        /* Read VROM Image */
        g_nesThread->InfoNES_ReadRom(VROM, 0x2000 * NesHeader.byVRomSize);
    }

    g_nesThread->InfoNES_CloseRom();
    /* Successful */
    return 0;
}

/*===================================================================*/
/*                                                                   */
/*           InfoNES_ReleaseRom() : Release a memory for ROM         */
/*                                                                   */
/*===================================================================*/
void InfoNES_ReleaseRom(void)
{
    if (ROM)
    {
        free(ROM);
        ROM = nullptr;
    }

    if (VROM)
    {
        free(VROM);
        VROM = nullptr;
    }
}

/*===================================================================*/
/*                                                                   */
/*      InfoNES_LoadFrame() :                                        */
/*           Transfer the contents of work frame on the screen       */
/*                                                                   */
/*===================================================================*/
void InfoNES_LoadFrame(void)
{
    g_nesThread->InfoNES_LoadFrame(WorkFrame, 256 * 240 * 2);
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_PadState() : Get a joypad state               */
/*                                                                   */
/*===================================================================*/
void InfoNES_PadState(uint32_t *pdwPad1, uint32_t *pdwPad2, uint32_t *pdwSystem)
{
    g_nesThread->InfoNES_PadState(pdwPad1, pdwPad2, pdwSystem);
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemoryCopy() : memcpy                         */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemoryCopy(void *dest, const void *src, int count)
{
    memcpy(dest, src, static_cast<size_t>(count));
    return dest;
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemorySet() : memset                          */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemorySet(void *dest, int c, int count)
{
    memset(dest, c, static_cast<size_t>(count));
    return dest;
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundInit() : Sound Emulation Initialize           */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundInit(void)
{
    g_nesThread->InfoNES_SoundInit();
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundOpen() : Sound Open                           */
/*                                                                   */
/*===================================================================*/
int InfoNES_SoundOpen(int samples_per_sync, int sample_rate)
{
    return g_nesThread->InfoNES_SoundOpen(samples_per_sync, sample_rate);
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundClose() : Sound Close                         */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundClose(void)
{
    g_nesThread->InfoNES_SoundClose();
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_SoundOutput() : Sound Output 5 Waves           */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundOutput(int samples, uint8_t *wave1, uint8_t *wave2, uint8_t *wave3,
                         uint8_t *wave4, uint8_t *wave5)
{
    g_nesThread->InfoNES_SoundOutput(samples, wave1, wave2, wave3, wave4, wave5);
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_Wait() : Wait Emulation if required            */
/*                                                                   */
/*===================================================================*/
void InfoNES_Wait(void)
{
    const unsigned int usec_frame = (1000000UL / 60);
    struct timeval tpend;
    long timeuse;
    static struct timeval tpstart;

    gettimeofday(&tpend, nullptr);
    timeuse = (1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec);
    unsigned int tmp = (usec_frame - static_cast<unsigned int>(timeuse));
    if (tmp > 1000)
    {
        if (tmp > (1000000 / 50))
            tmp = (1000000 / 50);
        tmp -= 1000;
        g_nesThread->InfoNES_Wait(tmp);
    }
    gettimeofday(&tpstart, nullptr);
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_MessageBox() : Print System Message            */
/*                                                                   */
/*===================================================================*/
void InfoNES_MessageBox(const char *pszMsg, ...)
{
    char buf[8192];
    va_list args;
    va_start(args, pszMsg);
    vsprintf(buf, pszMsg, args);
    va_end(args);
    g_nesThread->InfoNES_MessageBox(buf);
}
