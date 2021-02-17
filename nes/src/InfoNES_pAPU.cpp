/*===================================================================*/
/*                                                                   */
/*  InfoNES_pAPU.cpp : InfoNES Sound Emulation Function              */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on DarcNES and NesterJ )     */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"

#include "InfoNES_K6502.h"

/*-------------------------------------------------------------------*/
/*   APU Event resources                                             */
/*-------------------------------------------------------------------*/

struct ApuEvent_t ApuEventQueue[APU_EVENT_MAX];
int cur_event;
uint16_t entertime;

/*-------------------------------------------------------------------*/
/*   APU Register Write Functions                                    */
/*-------------------------------------------------------------------*/

#define APU_WRITEFUNC(name, evtype)                                  \
    void ApuWrite##name(uint16_t addr, uint8_t value) {                     \
        ApuEventQueue[cur_event].time = entertime - g_wPassedClocks; \
        ApuEventQueue[cur_event].type = APUET_W_##evtype;            \
        ApuEventQueue[cur_event].data = value;                       \
        cur_event++;                                                 \
        (void)addr;                                                 \
    }

APU_WRITEFUNC(C1a, C1A);
APU_WRITEFUNC(C1b, C1B);
APU_WRITEFUNC(C1c, C1C);
APU_WRITEFUNC(C1d, C1D);

APU_WRITEFUNC(C2a, C2A);
APU_WRITEFUNC(C2b, C2B);
APU_WRITEFUNC(C2c, C2C);
APU_WRITEFUNC(C2d, C2D);

APU_WRITEFUNC(C3a, C3A);
APU_WRITEFUNC(C3b, C3B);
APU_WRITEFUNC(C3c, C3C);
APU_WRITEFUNC(C3d, C3D);

APU_WRITEFUNC(C4a, C4A);
APU_WRITEFUNC(C4b, C4B);
APU_WRITEFUNC(C4c, C4C);
APU_WRITEFUNC(C4d, C4D);

APU_WRITEFUNC(C5a, C5A);
APU_WRITEFUNC(C5b, C5B);
APU_WRITEFUNC(C5c, C5C);
APU_WRITEFUNC(C5d, C5D);

APU_WRITEFUNC(Control, CTRL);

ApuWritefunc pAPUSoundRegs[20] = {
    ApuWriteC1a, ApuWriteC1b, ApuWriteC1c, ApuWriteC1d, ApuWriteC2a,
    ApuWriteC2b, ApuWriteC2c, ApuWriteC2d, ApuWriteC3a, ApuWriteC3b,
    ApuWriteC3c, ApuWriteC3d, ApuWriteC4a, ApuWriteC4b, ApuWriteC4c,
    ApuWriteC4d, ApuWriteC5a, ApuWriteC5b, ApuWriteC5c, ApuWriteC5d,
};

/*-------------------------------------------------------------------*/
/*   APU resources                                                   */
/*-------------------------------------------------------------------*/

uint8_t wave_buffers[5][735]; /* 44100 / 60 = 735 samples per sync */

uint8_t ApuCtrl;
uint8_t ApuCtrlNew;
uint32_t ApuCntRate;

/*-------------------------------------------------------------------*/
/*   APU Quality resources                                           */
/*-------------------------------------------------------------------*/

int ApuQuality;

uint32_t ApuPulseMagic;
uint32_t ApuTriangleMagic;
uint32_t ApuNoiseMagic;
unsigned int ApuSamplesPerSync;
unsigned int ApuCyclesPerSample;
unsigned int ApuSampleRate;
uint32_t ApuCycleRate;

struct ApuQualityData_t {
    uint32_t pulse_magic;
    uint32_t triangle_magic;
    uint32_t noise_magic;
    unsigned int samples_per_sync;
    unsigned int cycles_per_sample;
    unsigned int sample_rate;
    uint32_t cycle_rate;
} ApuQual[] = {
    {0xA25672A8, 0xA25672A8, 0x00044E80, 183, 164, 11025, 1062658},
    {0x512B3954, 0x512B3954, 0x00089D00, 367, 82, 22050, 531329},
    {0x28959CAA, 0x28959CAA, 0x00113A00, 735, 41, 44100, 265664},
};
//pulse_magic = triangle_magic = (1789773x32x2^24)/(16*samplex2)
//noise_magic = (2^8xsample/10)

/*-------------------------------------------------------------------*/
/*  Rectangle Wave #1 resources                                      */
/*-------------------------------------------------------------------*/
uint8_t ApuC1a, ApuC1b, ApuC1c, ApuC1d;

uint8_t *ApuC1Wave;
uint32_t ApuC1Skip;
uint32_t ApuC1Index;
int32_t ApuC1EnvPhase;
uint8_t ApuC1EnvVol;
uint8_t ApuC1Atl;
int32_t ApuC1SweepPhase;
uint32_t ApuC1Freq;

/*-------------------------------------------------------------------*/
/*  Rectangle Wave #2 resources                                      */
/*-------------------------------------------------------------------*/
uint8_t ApuC2a, ApuC2b, ApuC2c, ApuC2d;

uint8_t *ApuC2Wave;
uint32_t ApuC2Skip;
uint32_t ApuC2Index;
int32_t ApuC2EnvPhase;
uint8_t ApuC2EnvVol;
uint8_t ApuC2Atl;
int32_t ApuC2SweepPhase;
uint32_t ApuC2Freq;

/*-------------------------------------------------------------------*/
/*  Triangle Wave resources                                          */
/*-------------------------------------------------------------------*/
uint8_t ApuC3a, ApuC3b, ApuC3c, ApuC3d;

uint32_t ApuC3Skip;
uint32_t ApuC3Index;
uint8_t ApuC3Atl;
uint32_t ApuC3Llc; /* Linear Length Counter */
uint8_t ApuC3WriteLatency;
uint8_t ApuC3CounterStarted;
uint32_t ApuC3Freq;

/*-------------------------------------------------------------------*/
/*  Noise resources                                                  */
/*-------------------------------------------------------------------*/
uint8_t ApuC4a, ApuC4b, ApuC4c, ApuC4d;

uint32_t ApuC4Sr;  /* Shift register */
uint32_t ApuC4Fdc; /* Frequency divide counter */
uint32_t ApuC4Skip;
uint32_t ApuC4Index;
uint8_t ApuC4Atl;
uint8_t ApuC4EnvVol;
int32_t ApuC4EnvPhase;

/*-------------------------------------------------------------------*/
/*  DPCM resources                                                   */
/*-------------------------------------------------------------------*/
uint8_t ApuC5Reg[4];
uint8_t ApuC5Enable;
uint8_t ApuC5Looping;
uint8_t ApuC5CurByte;
uint8_t ApuC5DpcmValue;

int ApuC5Freq;
int ApuC5Phaseacc;

uint16_t ApuC5Address, ApuC5CacheAddr;
int ApuC5DmaLength, ApuC5CacheDmaLength;

/*-------------------------------------------------------------------*/
/*  Wave Data                                                        */
/*-------------------------------------------------------------------*/
uint8_t pulse_25[0x20] = {
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t pulse_50[0x20] = {
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t pulse_75[0x20] = {
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t pulse_87[0x20] = {
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00,
};

uint8_t triangle_50[0x20] = {
    0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05,
    0x04, 0x03, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};

uint8_t *pulse_waves[4] = {
    pulse_87,
    pulse_75,
    pulse_50,
    pulse_25,
};

/*-------------------------------------------------------------------*/
/*  Active Time Left Data                                            */
/*-------------------------------------------------------------------*/
uint8_t ApuAtl[0x20] = {
    5, 127, 10, 1, 19, 2,  40, 3,  80, 4,  30, 5,  7, 6,  13, 7,
    6, 8,   12, 9, 24, 10, 48, 11, 96, 12, 36, 13, 8, 14, 16, 15,
};

/*-------------------------------------------------------------------*/
/* Frequency Limit of Rectangle Channels                             */
/*-------------------------------------------------------------------*/
uint16_t ApuFreqLimit[8] = {0x3FF, 0x555, 0x666, 0x71C, 0x787, 0x7C1, 0x7E0, 0x7F0};

/*-------------------------------------------------------------------*/
/* Noise Frequency Lookup Table                                      */
/*-------------------------------------------------------------------*/
uint32_t ApuNoiseFreq[16] = {4,   8,   16,  32,  64,  96,   128,  160,
                          202, 254, 380, 508, 762, 1016, 2034, 4068};

uint32_t ApuNoiseFreqNTSC[16] = {48112, 24056, 12028, 6014, 3007, 2005, 1504, 1203,
                          953, 758, 506, 379, 253, 189, 95, 47};

/*-------------------------------------------------------------------*/
/* DMC Transfer Clocks Table                                          */
/*-------------------------------------------------------------------*/
uint32_t ApuDpcmCycles[16] = {428, 380, 340, 320, 286, 254, 226, 214,
                           190, 160, 142, 128, 106, 85,  72,  54};

/*===================================================================*/
/*                                                                   */
/*      ApuRenderingWave1() : Rendering Rectangular Wave #1          */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/* Write registers of rectangular wave #1                            */
/*-------------------------------------------------------------------*/
#include <QDebug>
int ApuWriteWave1(int cycles, int event) {
    /* APU Reg Write Event */
    while ((event < cur_event) && (ApuEventQueue[event].time < cycles)) {
        if ((ApuEventQueue[event].type & APUET_MASK) == APUET_C1) {
            switch (ApuEventQueue[event].type & 0x03) {
                case 0:
                    ApuC1a = ApuEventQueue[event].data;
                    ApuC1Wave = pulse_waves[ApuC1DutyCycle >> 6];
                    break;

                case 1:
                    ApuC1b = ApuEventQueue[event].data;
                    break;

                case 2:
                    ApuC1c = ApuEventQueue[event].data;
                    ApuC1Freq &= 0xFF00;
                    ApuC1Freq |= (uint16_t)(ApuC1c);
                    //ApuC1Atl = ApuAtl[(ApuC1d >> 3)&0x1f];
                    break;

                case 3:
                    ApuC1d = ApuEventQueue[event].data;
                    ApuC1Freq = (ApuC1Freq & 0xff)|(((uint16_t)(ApuC1d & 0x07)) << 8);
                    ApuC1Atl = ApuAtl[(ApuC1d >> 3)&0x1f];
                    ApuC1EnvVol = 0xf;
                    break;
            }
        } else if (ApuEventQueue[event].type == APUET_W_CTRL) {
            ApuCtrlNew = ApuEventQueue[event].data;

            if (!(ApuEventQueue[event].data & (1 << 0))) {
                ApuC1Atl = 0;
            }
        }
        event++;
    }
    return event;
}

/*-------------------------------------------------------------------*/
/* Rendering rectangular wave #1                                     */
/*-------------------------------------------------------------------*/

void ApuRenderingWave1(void) {
    int cycles = 0;
    int event = 0;

    /* note: 41 CPU cycles occur between increments of i */
    ApuCtrlNew = ApuCtrl;
    for (unsigned int i = 0; i < ApuSamplesPerSync; i++) {
        /* Write registers */
        cycles += ApuCyclesPerSample;
        event = ApuWriteWave1(cycles, event);

        /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
        ApuC1EnvPhase -= 4;
        //while (ApuC1EnvPhase < 0) {
        while (0) {
            ApuC1EnvPhase += ApuC1EnvDelay;

            if (ApuC1EnvVol || ApuC1Hold) {
                ApuC1EnvVol--;
                ApuC1EnvVol = ApuC1EnvVol & 0x0f;
            }
        }

        /*
         * TODO: using a table of max frequencies is not technically
         * clean, but it is fast and (or should be) accurate
         */
        if (ApuC1Freq < 8 ||
            (!ApuC1SweepIncDec && ApuC1Freq > ApuC1FreqLimit)) {
            wave_buffers[0][i] = 0;
            break;
        }

        /* Frequency sweeping at a rate of ( Sweep Delay + 1) / 120 secs */
        if (ApuC1SweepOn) {
            ApuC1SweepPhase -= 2; /* 120/60 */
            //while (ApuC1SweepPhase < 0) {
            while (0) {
                ApuC1SweepPhase += ApuC1SweepDelay;
                if (ApuC1SweepIncDec) /* ramp up */
                {
                    /* Rectangular #1 */
                    ApuC1Freq -= (ApuC1Freq >> ApuC1SweepShifts)-1;
                } else {
                    /* ramp down */
                    ApuC1Freq += (ApuC1Freq >> ApuC1SweepShifts);
                }
            }
        }

        ApuC1Skip = ( ApuPulseMagic << 1 ) / (ApuC1Freq+1);

        /* Wave Rendering */
        if ((ApuCtrlNew & 0x01) && (ApuC1Atl || ApuC1Hold)) {
            ApuC1Index += ApuC1Skip;
            ApuC1Index &= 0x1fffffff;

            if (ApuC1Env) {
                wave_buffers[0][i] =
                    ApuC1Wave[ApuC1Index >> 24] * ApuC1Vol;
            } else {
                wave_buffers[0][i] = ApuC1Wave[ApuC1Index >> 24] * ApuC1EnvVol;
            }
        } else {
            wave_buffers[0][i] = 0;
        }
    }
    if (ApuC1Atl) {
        ApuC1Atl--;
    }
}

/*===================================================================*/
/*                                                                   */
/*      ApuRenderingWave2() : Rendering Rectangular Wave #2          */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/* Write registers of rectangular wave #2                           */
/*-------------------------------------------------------------------*/

int ApuWriteWave2(int cycles, int event) {
    /* APU Reg Write Event */
    while ((event < cur_event) && (ApuEventQueue[event].time < cycles)) {
        if ((ApuEventQueue[event].type & APUET_MASK) == APUET_C2) {
            switch (ApuEventQueue[event].type & 0x03) {
                case 0:
                    ApuC2a = ApuEventQueue[event].data;
                    ApuC2Wave = pulse_waves[ApuC2DutyCycle >> 6];
                    break;

                case 1:
                    ApuC2b = ApuEventQueue[event].data;
                    break;

                case 2:
                    ApuC2c = ApuEventQueue[event].data;
                    ApuC2Freq &= 0xFF00;
                    ApuC2Freq |= (uint16_t)(ApuC2c);
                    //ApuC2Atl = ApuAtl[(ApuC1d >> 3)&0x1f]<<1;
                    break;

                case 3:
                    ApuC2d = ApuEventQueue[event].data;
                    ApuC2Freq = (ApuC2Freq & 0xff)|(((uint16_t)(ApuC2d & 0x07)) << 8);
                    ApuC2Atl = ApuAtl[(ApuC2d >> 3)&0x1f]<<1;
                    ApuC2EnvVol = 0xf;
                    break;
            }
        } else if (ApuEventQueue[event].type == APUET_W_CTRL) {
            ApuCtrlNew = ApuEventQueue[event].data;

            if (!(ApuEventQueue[event].data & (1 << 1))) {
                ApuC2Atl = 0;
            }
        }
        event++;
    }
    return event;
}

/*-------------------------------------------------------------------*/
/* Rendering rectangular wave #2                                     */
/*-------------------------------------------------------------------*/

void ApuRenderingWave2(void) {
    int cycles = 0;
    int event = 0;

    /* note: 41 CPU cycles occur between increments of i */
    ApuCtrlNew = ApuCtrl;
    for (unsigned int i = 0; i < ApuSamplesPerSync; i++) {
        /* Write registers */
        cycles += ApuCyclesPerSample;
        event = ApuWriteWave2(cycles, event);

        /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
        ApuC2EnvPhase -= 4;
        //while (ApuC2EnvPhase < 0) {
        while (0) {
            ApuC2EnvPhase += ApuC2EnvDelay;

            if (ApuC2EnvVol || ApuC2Hold) {
                ApuC2EnvVol--;
                ApuC2EnvVol = ApuC2EnvVol & 0x0f;
            }
        }

        /*
         * TODO: using a table of max frequencies is not technically
         * clean, but it is fast and (or should be) accurate
         */
        if (ApuC2Freq < 8 ||
            (!ApuC2SweepIncDec && ApuC2Freq > ApuC2FreqLimit)) {
            wave_buffers[1][i] = 0;
            break;
        }

        /* Frequency sweeping at a rate of ( Sweep Delay + 1) / 120 secs */
        if (ApuC2SweepOn) {
            ApuC2SweepPhase -= 2; /* 120/60 */
            //while (ApuC2SweepPhase < 0) {
            while (0) {
                ApuC2SweepPhase += ApuC2SweepDelay;
                if (ApuC2SweepIncDec) /* ramp up */
                {
                    /* Rectangular #2 */
                    ApuC2Freq -= (ApuC2Freq >> ApuC2SweepShifts);
                } else {
                    /* ramp down */
                    ApuC2Freq += (ApuC2Freq >> ApuC2SweepShifts);
                }
            }
        }
        ApuC2Skip = ( ApuPulseMagic << 1 ) / (ApuC2Freq+1);

        /* Wave Rendering */
        if ((ApuCtrlNew & 0x02) && (ApuC2Atl || ApuC2Hold)) {
            ApuC2Index += ApuC2Skip;
            ApuC2Index &= 0x1fffffff;

            if (ApuC2Env) {
                wave_buffers[1][i] =
                    ApuC2Wave[ApuC2Index >> 24] * ApuC2Vol;
            } else {
                wave_buffers[1][i] = ApuC2Wave[ApuC2Index >> 24] * (ApuC2EnvVol);
            }
        } else {
            wave_buffers[1][i] = 0;
        }
    }
    if (ApuC2Atl) {
        ApuC2Atl--;
    }
}

/*===================================================================*/
/*                                                                   */
/*      ApuRenderingWave3() : Rendering Triangle Wave                */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/* Write registers of triangle wave #3                              */
/*-------------------------------------------------------------------*/

int ApuWriteWave3(int cycles, int event) {
    /* APU Reg Write Event */
    while ((event < cur_event) && (ApuEventQueue[event].time < cycles)) {
        if ((ApuEventQueue[event].type & APUET_MASK) == APUET_C3) {
            switch (ApuEventQueue[event].type & 3) {
                case 0:
                    ApuC3a = ApuEventQueue[event].data;
                    ApuC3Llc = ApuC3LinearLength;
                    break;

                case 1:
                    ApuC3b = ApuEventQueue[event].data;
                    break;

                case 2:
                    ApuC3c = ApuEventQueue[event].data;
                    ApuC3Freq &= 0xFF00;
                    ApuC3Freq |= (uint16_t)(ApuC3c);
                    break;

                case 3:
                    ApuC3d = ApuEventQueue[event].data;
                    ApuC3Freq = (ApuC3Freq & 0xff)|(((uint16_t)(ApuC3d & 0x07)) << 8);
                    ApuC3Atl = ApuC3LengthCounter;
                    break;
            }
        } else if (ApuEventQueue[event].type == APUET_W_CTRL) {
            ApuCtrlNew = ApuEventQueue[event].data;

            if (!(ApuEventQueue[event].data & (1 << 2))) {
                ApuC3Atl = 0;
                ApuC3Llc = 0;
            }
        }
        event++;
    }
    return event;
}

/*-------------------------------------------------------------------*/
/* Rendering triangle wave #3                                        */
/*-------------------------------------------------------------------*/

void ApuRenderingWave3(void) {
    int cycles = 0;
    int event = 0;

    /* note: 41 CPU cycles occur between increments of i */
    ApuCtrlNew = ApuCtrl;
    for (unsigned int i = 0; i < ApuSamplesPerSync; i++) {
        /* Write registers */
        cycles += ApuCyclesPerSample;
        event = ApuWriteWave3(cycles, event);

        /* Cutting Min Frequency */
        if (ApuC3Freq < 8) {
            wave_buffers[2][i] = 0;
            break;
        }

        /* Counter Control */
        if (ApuC3CounterStarted) {
            if (ApuC3Atl > 0 && !ApuC3Holdnote) {
                ApuC3Atl--;
            }
            if (ApuC3Llc > 0) {
                ApuC3Llc--;
            }
        } else if (!ApuC3Holdnote && ApuC3WriteLatency > 0) {
            if (--ApuC3WriteLatency == 0) {
                ApuC3CounterStarted = 0x01;
            }
        }

        ApuC3Skip = ( ApuTriangleMagic ) / (ApuC3Freq+1);

        /* Wave Rendering */
        if ((ApuCtrlNew & 0x04) &&
            ((ApuC3Atl > 0 || ApuC3Holdnote) && ApuC3Llc > 0)) {
            ApuC3Index += ApuC3Skip;
            ApuC3Index &= 0x1fffffff;
            wave_buffers[2][i] = triangle_50[ApuC3Index >> 24]*0x11;
        } else {
            wave_buffers[2][i] = 0;
        }
    }
}

/*===================================================================*/
/*                                                                   */
/*      ApuRenderingWave4() : Rendering Noise                        */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/* Write registers of noise channel #4                              */
/*-------------------------------------------------------------------*/

int ApuWriteWave4(int cycles, int event) {
    /* APU Reg Write Event */
    while ((event < cur_event) && (ApuEventQueue[event].time < cycles)) {
        if ((ApuEventQueue[event].type & APUET_MASK) == APUET_C4) {
            switch (ApuEventQueue[event].type & 3) {
                case 0:
                    ApuC4a = ApuEventQueue[event].data;
                    break;

                case 1:
                    ApuC4b = ApuEventQueue[event].data;
                    break;

                case 2:
                    ApuC4c = ApuEventQueue[event].data;
                    break;

                case 3:
                    ApuC4d = ApuEventQueue[event].data;

                    /* Frequency */
                    if (ApuC4Freq) {
                        ApuC4Skip = ApuNoiseMagic / ApuC4Freq;
                    } else {
                        ApuC4Skip = 0;
                    }
                    ApuC4Atl = ApuC4LengthCounter;
                    ApuC4EnvVol = 0xf;
            }
        } else if (ApuEventQueue[event].type == APUET_W_CTRL) {
            ApuCtrlNew = ApuEventQueue[event].data;

            if (!(ApuEventQueue[event].data & (1 << 3))) {
                ApuC4Atl = 0;
            }
        }
        event++;
    }
    return event;
}

/*-------------------------------------------------------------------*/
/* Rendering noise channel #4                                        */
/*-------------------------------------------------------------------*/

void ApuRenderingWave4(void) {
    int cycles = 0;
    int event = 0;

    ApuCtrlNew = ApuCtrl;
    for (unsigned int i = 0; i < ApuSamplesPerSync; i++) {
        /* Write registers */
        cycles += ApuCyclesPerSample;
        event = ApuWriteWave4(cycles, event);

        /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
        ApuC4EnvPhase -= 4;
        //while (ApuC4EnvPhase < 0) {
        while (0) {
            ApuC4EnvPhase += ApuC4EnvDelay;

            if (ApuC4EnvVol || ApuC4Hold) {
                ApuC4EnvVol--;
                ApuC4EnvVol = ApuC4EnvVol & 0x0f;
            }
        }

        /* Wave Rendering */
        if ((ApuCtrlNew & 0x08) && (ApuC4Atl || ApuC4Hold)) {
            ApuC4Index += ApuC4Skip;
            if (ApuC4Index > 0xff) {
                unsigned int intNewBit0 = 0x0;
                if (ApuC4Small)
                {
                    // 93 bit mode - new bit 0 is XOR of bits E and 8
                    intNewBit0 = ((ApuC4Sr & 0x0004) >> 14) ^ ((ApuC4Sr & 0x0100) >> 8);
                } else {
		            // 32k mode - new bit 0 is XOR of bits E and D
					intNewBit0 = ((ApuC4Sr & 0x4000) >> 14) ^ ((ApuC4Sr & 0x2000) >> 13);
                }
                ApuC4Sr = (ApuC4Sr << 1) | intNewBit0;
                ApuC4Sr&=0x7fff;
            }
            ApuC4Index &= 0xff;

            if (ApuC4Atl && (!((ApuC4Sr & 0x4000) >> 14))) {
                if (!ApuC4Env) {
                    wave_buffers[3][i] = 0x11*ApuC4Vol;
                } else {
                    wave_buffers[3][i] = 0x11*ApuC4EnvVol;
                }
            } else {
                wave_buffers[3][i] = 0;
            }
        } else {
            wave_buffers[3][i] = 0;
        }
    }
    if (ApuC4Atl) {
        ApuC4Atl--;
    }
}

/*===================================================================*/
/*                                                                   */
/*      ApuRenderingWave5() : Rendering DPCM channel #5              */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/* Write registers of DPCM channel #5                               */
/*-------------------------------------------------------------------*/

int ApuWriteWave5(int cycles, int event) {
    /* APU Reg Write Event */
    while ((event < cur_event) && (ApuEventQueue[event].time < cycles)) {
        if ((ApuEventQueue[event].type & APUET_MASK) == APUET_C5) {
            ApuC5Reg[ApuEventQueue[event].type & 3] = ApuEventQueue[event].data;

            switch (ApuEventQueue[event].type & 3) {
                case 0:
                    ApuC5Freq =
                        ApuDpcmCycles[(ApuEventQueue[event].data & 0x0F)] << 16;
                    ApuC5Looping = ApuEventQueue[event].data & 0x40;
                    break;
                case 1:
                    ApuC5DpcmValue = (ApuEventQueue[event].data & 0x7F) >> 1;
                    break;
                case 2:
                    ApuC5CacheAddr =
                        0xC000 + (uint16_t)(ApuEventQueue[event].data << 6);
                    break;
                case 3:
                    ApuC5CacheDmaLength = ((ApuEventQueue[event].data << 4) + 1)
                                          << 3;
                    break;
            }
        } else if (ApuEventQueue[event].type == APUET_W_CTRL) {
            ApuCtrlNew = ApuEventQueue[event].data;

            if (!(ApuEventQueue[event].data & (1 << 4))) {
                ApuC5Enable = 0;
                ApuC5DmaLength = 0;
            } else {
                ApuC5Enable = 0xFF;
                if (!ApuC5DmaLength) {
                    ApuC5Address = ApuC5CacheAddr;
                    ApuC5DmaLength = ApuC5CacheDmaLength;
                }
            }
        }
        event++;
    }
    return event;
}

/*-------------------------------------------------------------------*/
/* Rendering DPCM channel #5                                         */
/*-------------------------------------------------------------------*/

void ApuRenderingWave5(void) {
    int cycles = 0;
    int event = 0;

    ApuCtrlNew = ApuCtrl;
    for (unsigned int i = 0; i < ApuSamplesPerSync; i++) {
        /* Write registers */
        cycles += ApuCyclesPerSample;
        event = ApuWriteWave5(cycles, event);

        if (ApuC5DmaLength) {
            ApuC5Phaseacc -= ApuCycleRate;

            while (ApuC5Phaseacc < 0) {
                ApuC5Phaseacc += ApuC5Freq;
                if (!(ApuC5DmaLength & 7)) {
                    ApuC5CurByte = K6502_Read(ApuC5Address);
                    if (0xFFFF == ApuC5Address)
                        ApuC5Address = 0x8000;
                    else
                        ApuC5Address++;
                }
                if (!(--ApuC5DmaLength)) {
                    if (ApuC5Looping) {
                        ApuC5Address = ApuC5CacheAddr;
                        ApuC5DmaLength = ApuC5CacheDmaLength;
                    } else {
                        ApuC5Enable = 0;
                        break;
                    }
                }

                // positive delta
                if (ApuC5CurByte & (1 << ((ApuC5DmaLength & 7) ^ 7))) {
                    if (ApuC5DpcmValue < 0x3F) ApuC5DpcmValue += 1;
                } else {
                    // negative delta
                    if (ApuC5DpcmValue > 1) ApuC5DpcmValue -= 1;
                }
            }
        }

        /* Wave Rendering */
        if (ApuCtrlNew & 0x10) {
            wave_buffers[4][i] = (ApuC5Reg[1] & 0x01) + (ApuC5DpcmValue << 1);
        }
    }
}

/*===================================================================*/
/*                                                                   */
/*     InfoNES_pApuVsync() : Callback Function per Vsync             */
/*                                                                   */
/*===================================================================*/
void InfoNES_pAPUVsync(void) {
    ApuRenderingWave1();
    ApuRenderingWave2();
    ApuRenderingWave3();
    ApuRenderingWave4();
    ApuRenderingWave5();

    ApuCtrl = ApuCtrlNew;

    InfoNES_SoundOutput(ApuSamplesPerSync, wave_buffers[0], wave_buffers[1],
                        wave_buffers[2], wave_buffers[3], wave_buffers[4]);

    entertime = g_wPassedClocks;
    cur_event = 0;
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_pApuInit() : Initialize pApu                   */
/*                                                                   */
/*===================================================================*/

void InfoNES_pAPUInit(void) {
    /* Sound Hardware Init */
    InfoNES_SoundInit();

    ApuQuality = pAPU_QUALITY - 1;  // 1: 22050, 2: 44100 [samples/sec]

    ApuPulseMagic = ApuQual[ApuQuality].pulse_magic;
    ApuTriangleMagic = ApuQual[ApuQuality].triangle_magic;
    ApuNoiseMagic = ApuQual[ApuQuality].noise_magic;
    ApuSamplesPerSync = ApuQual[ApuQuality].samples_per_sync;
    ApuCyclesPerSample = ApuQual[ApuQuality].cycles_per_sample;
    ApuSampleRate = ApuQual[ApuQuality].sample_rate;
    ApuCycleRate = ApuQual[ApuQuality].cycle_rate;

    InfoNES_SoundOpen(ApuSamplesPerSync, ApuSampleRate);

    /*-------------------------------------------------------------------*/
    /* Initialize Rectangular, Noise Wave's Regs                         */
    /*-------------------------------------------------------------------*/
    ApuCtrl = ApuCtrlNew = 0;
    ApuCntRate = 5;
    ApuC1Wave = pulse_50;
    ApuC2Wave = pulse_50;

    ApuC1a = ApuC1b = ApuC1c = ApuC1d = 0;
    ApuC2a = ApuC2b = ApuC2c = ApuC2d = 0;
    ApuC4a = ApuC4b = ApuC4c = ApuC4d = 0;

    ApuC1Skip = ApuC2Skip = ApuC4Skip = 0;
    ApuC1Index = ApuC2Index = ApuC4Index = 0;
    ApuC1EnvPhase = ApuC2EnvPhase = ApuC4EnvPhase = 0;
    ApuC1EnvVol = ApuC2EnvVol = ApuC4EnvVol = 0;
    ApuC1Atl = ApuC2Atl = ApuC4Atl = 0;
    ApuC1SweepPhase = ApuC2SweepPhase = 0;
    ApuC1Freq = ApuC2Freq = ApuC3Freq = ApuC4Freq = 0;
    ApuC4Fdc = 0;
    ApuC4Sr = 1;

    /*-------------------------------------------------------------------*/
    /*   Initialize Triangle Wave's Regs                                 */
    /*-------------------------------------------------------------------*/
    ApuC3a = ApuC3b = ApuC3c = ApuC3d = 0;
    ApuC3Atl = ApuC3Llc = 0;
    ApuC3WriteLatency = 3; /* Magic Number */
    ApuC3CounterStarted = 0x00;

    /*-------------------------------------------------------------------*/
    /*   Initialize DPCM's Regs                                          */
    /*-------------------------------------------------------------------*/
    ApuC5Reg[0] = ApuC5Reg[1] = ApuC5Reg[2] = ApuC5Reg[3] = 0;
    ApuC5Enable = ApuC5Looping = ApuC5CurByte = ApuC5DpcmValue = 0;
    ApuC5Freq = ApuC5Phaseacc;
    ApuC5Address = ApuC5CacheAddr = 0;
    ApuC5DmaLength = ApuC5CacheDmaLength = 0;

    /*-------------------------------------------------------------------*/
    /*   Initialize Wave Buffers                                         */
    /*-------------------------------------------------------------------*/
    InfoNES_MemorySet((void *)wave_buffers[0], 0, 735);
    InfoNES_MemorySet((void *)wave_buffers[1], 0, 735);
    InfoNES_MemorySet((void *)wave_buffers[2], 0, 735);
    InfoNES_MemorySet((void *)wave_buffers[3], 0, 735);
    InfoNES_MemorySet((void *)wave_buffers[4], 0, 735);

    entertime = g_wPassedClocks;
    cur_event = 0;
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_pApuDone() : Finalize pApu                     */
/*                                                                   */
/*===================================================================*/

void InfoNES_pAPUDone(void) { InfoNES_SoundClose(); }

/*
 * End of InfoNES_pAPU.cpp
 */
