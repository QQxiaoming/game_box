#include "mainwindow.h"
#include <QDebug>


#define IS_MAIN_CPP
#include "md.h"
#include "pd.h"
#include "rc.h"
#include "rc-vars.h"


static DGENThread *g_dgenThread = nullptr;
struct bmap mdscr;
unsigned char *mdpal;
void DGEN_start(DGENThread *dgenThread,const char *pszFileName)
{
    bool dgen_pal = false;
    char dgen_region = 0;

    g_dgenThread = dgenThread;
    mdscr.data = (unsigned char *)g_dgenThread->workFrame;
    mdscr.h = 256;
    mdscr.w = 336;
    mdscr.bpp = 15;
    mdscr.pitch = mdscr.w * 2;
    mdpal = new unsigned char[256];

    md megad(dgen_pal,dgen_region);
    if(!megad.okay())
    {
        return;
    }
    if(megad.load(pszFileName))
    {
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

        md::region_info(c, &pal, &hz, 0, 0, 0);
        if ((hz != dgen_hz) || (pal != dgen_pal) ||
            (c != megad.region)) {
            megad.region = c;
            dgen_hz = hz;
            dgen_pal = pal;
            printf("main: reconfiguring for region \"%c\": "
                   "%dHz (%s)\n", c, hz, (pal ? "PAL" : "NTSC"));
            megad.pal = pal;
            megad.init_pal();
            megad.init_sound();
        }
    }

    uint32_t pdwPad1=0,pdwPad2=0,pdwSystem=0;
    while(!pdwSystem)
    {
        g_dgenThread->DGEN_PadState(&pdwPad1, &pdwPad2, &pdwSystem);
        megad.one_frame(&mdscr, mdpal, nullptr);
        g_dgenThread->DGEN_Wait();
    }

    // Cleanup
    megad.unplug();
    YM2612Shutdown();

    delete [] mdpal;
}


uint8_t *load(size_t *file_size, const char *name, size_t max_size)
{
    int size = g_dgenThread->DGEN_OpenRom(name);
    if(size == -1 || size > (int)max_size) return nullptr;
    *file_size =  (size_t)size;
    uint8_t *rom = new unsigned char[size];
    g_dgenThread->DGEN_ReadRom(rom,(unsigned int)size);
    g_dgenThread->DGEN_CloseRom();
    return rom;
}


void unload(uint8_t *data)
{
    if(data != nullptr)
    {
        delete [] data;
    }
}

void dump_z80ram(  unsigned char *z80ram, int size )
{
    Q_UNUSED(z80ram);
    Q_UNUSED(size);
}
