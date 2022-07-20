#include "mainwindow.h"
#include "game_box_misc.h"
#include <QDebug>

#define IS_MAIN_CPP
#include "md.h"
#include "pd.h"
#include "rc.h"
#include "rc-vars.h"
#include "dgen_system.h"

static DGENThread *g_dgenThread = nullptr;
// This is the struct bmap setup by your implementation.
// It should be 336x240 (or 336x256 in PAL mode), in 8, 12, 15, 16, 24 or 32
// bits-per-pixel.
static struct bmap mdscr;
// Also, you should allocate a 256-char palette array, if need be. Otherwise
// this can be NULL if you don't have a paletted display.
static unsigned char mdpal[256]={0};
// This is the struct sndinfo, also setup by your implementation.
// Note that the buffers pointed to in this struct should ALWAYS be 16-bit
// signed format, regardless of the actual audio format.
static struct sndinfo mdsndi;

void DGEN_start(DGENThread *dgenThread, const char *pszFileName) {
    bool dgen_pal = false;
    char dgen_region = 0;
    uint32_t pdwSystem = 0;

    g_dgenThread = dgenThread;
    g_dgenThread->libVersion = DGEN_VER;
    g_dgenThread->pdwPad1 = 0xf303f;
    g_dgenThread->pdwPad2 = 0xf303f;
    mdscr.data = new unsigned char[336 * 256 * 2];
    memset(mdscr.data, 0x0, 336 * 256 * 2);
    mdscr.h = 256;
    mdscr.w = 336;
    mdscr.bpp = 15;
    mdscr.pitch = mdscr.w * 2;
    mdsndi.len = (44100 / 60);
    mdsndi.lr = new int16_t[mdsndi.len * 2];

    md megad(dgen_pal, dgen_region);
    if (!megad.okay()) {
        return;
    }
    if (megad.load(pszFileName)) {
        return;
    }

    // Set untouched pads
    megad.pad[0] = MD_PAD_UNTOUCHED;
    megad.pad[1] = MD_PAD_UNTOUCHED;

    // Reset
    megad.reset();
    if (!dgen_region) {
        uint8_t c = megad.region_guess();
        int hz;
        int pal;

        md::region_info(c, &pal, &hz, nullptr, nullptr, nullptr);
        if ((hz != dgen_hz) || (pal != dgen_pal) || (c != megad.region)) {
            megad.region = static_cast<int8_t>(c);
            dgen_hz = hz;
            dgen_pal = pal;
            megad.pal = static_cast<unsigned int>(pal);
            megad.init_pal();
        }
    }
    megad.init_sound();
    g_dgenThread->DGEN_SoundInit();
    g_dgenThread->DGEN_SoundOpen(static_cast<int>(mdsndi.len), 44100);

    while (!pdwSystem) {
        DGEN_Wait();
        g_dgenThread->DGEN_PadState(&megad.pad[0], &megad.pad[1], &pdwSystem);
        megad.one_frame(&mdscr, mdpal, &mdsndi);
        g_dgenThread->DGEN_LoadFrame(mdscr.data);
        g_dgenThread->DGEN_SoundOutput(static_cast<int>(mdsndi.len), mdsndi.lr);
    }

    megad.unplug();
    g_dgenThread->DGEN_SoundClose();

    delete[] mdscr.data;
    delete[] mdsndi.lr;
}

void DGEN_Wait(void) {
    const unsigned int usec_frame = (1000000UL / 60);
    struct timeval tpend;
    long timeuse;
    static struct timeval tpstart;

    gettimeofday(&tpend, nullptr);
    timeuse = (1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec);
    unsigned int tmp = (usec_frame - static_cast<unsigned int>(timeuse));
    if (tmp > 1000) {
        if (tmp > (1000000 / 50))
            tmp = (1000000 / 50);
        tmp -= 1000;
        g_dgenThread->DGEN_Wait(tmp);
    }
    gettimeofday(&tpstart, nullptr);
}

uint8_t *load(size_t *file_size, const char *name, size_t max_size) {
    int size = g_dgenThread->DGEN_OpenRom(name);
    if (size == -1 || size > static_cast<int>(max_size))
        return nullptr;
    *file_size = static_cast<size_t>(size);
    uint8_t *rom = new unsigned char[size];
    g_dgenThread->DGEN_ReadRom(rom, static_cast<unsigned int>(size));
    g_dgenThread->DGEN_CloseRom();
    return rom;
}

void unload(uint8_t *data) {
    if (data != nullptr) {
        delete[] data;
    }
}

void dump_z80ram(unsigned char *z80ram, int size) {
    Q_UNUSED(z80ram);
    Q_UNUSED(size);
}
