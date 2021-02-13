#include "mainwindow.h"
#include <sys/time.h>
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
    g_dgenThread->pdwPad1 = 0xf303f;
    g_dgenThread->pdwPad2 = 0xf303f;
    mdscr.data = new unsigned char[336*256*2];
    memset(mdscr.data,0x0,336*256*2);
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

        md::region_info(c, &pal, &hz, nullptr, nullptr, nullptr);
        if ((hz != dgen_hz) || (pal != dgen_pal) ||
            (c != megad.region)) {
            megad.region = (int8_t)c;
            dgen_hz = hz;
            dgen_pal = pal;
            printf("main: reconfiguring for region \"%c\": "
                   "%dHz (%s)\n", c, hz, (pal ? "PAL" : "NTSC"));
            megad.pal = (unsigned int)pal;
            megad.init_pal();
            megad.init_sound();
        }
    }

    uint32_t pdwSystem=0;
    while(!pdwSystem)
    {
        const unsigned int usec_frame = (1000000 / dgen_hz);

        struct timeval tpstart,tpend;
        float timeuse;
        gettimeofday(&tpstart,nullptr);

        g_dgenThread->DGEN_PadState(&megad.pad[0], &megad.pad[1], &pdwSystem);
        megad.one_frame(&mdscr, mdpal, nullptr);
        g_dgenThread->DGEN_LoadFrame(mdscr.data,336*256*2);

        gettimeofday(&tpend,nullptr);
        timeuse=(1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000000.0;
        unsigned int tmp = (usec_frame - (unsigned int)timeuse);
        if(tmp > 1000)
        {
            if (tmp > (1000000 / 50))
                tmp = (1000000 / 50);
            g_dgenThread->DGEN_Wait(tmp);
        }
    }

    // Cleanup
    megad.unplug();
    YM2612Shutdown();

    delete [] mdpal;
    delete [] mdscr.data;
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

void dump_z80ram(unsigned char *z80ram, int size )
{
    Q_UNUSED(z80ram);
    Q_UNUSED(size);
}
