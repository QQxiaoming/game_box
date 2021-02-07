/*===================================================================*/
/*                                                                   */
/*  InfoNES.h : NES Emulator for Win32, Linux(x86), Linux(PS2)       */
/*                                                                   */
/*  2000/05/14  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_H_INCLUDED
#define InfoNES_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/
#include <stdint.h>

/*-------------------------------------------------------------------*/
/*  NES resources                                                    */
/*-------------------------------------------------------------------*/

#define RAM_SIZE 0x2000
#define SRAM_SIZE 0x2000
#define PPURAM_SIZE 0x4000
#define SPRRAM_SIZE 256

/* RAM */
extern uint8_t RAM[];

/* SRAM */
extern uint8_t SRAM[];

/* ROM */
extern uint8_t *ROM;

/* SRAM BANK ( 8Kb ) */
extern uint8_t *SRAMBANK;

/* ROM BANK ( 8Kb * 4 ) */
extern uint8_t *ROMBANK0;
extern uint8_t *ROMBANK1;
extern uint8_t *ROMBANK2;
extern uint8_t *ROMBANK3;

/*-------------------------------------------------------------------*/
/*  PPU resources                                                    */
/*-------------------------------------------------------------------*/

/* PPU RAM */
extern uint8_t PPURAM[];

/* VROM */
extern uint8_t *VROM;

/* PPU BANK ( 1Kb * 16 ) */
extern uint8_t *PPUBANK[];

#define NAME_TABLE0 8
#define NAME_TABLE1 9
#define NAME_TABLE2 10
#define NAME_TABLE3 11

#define NAME_TABLE_V_MASK 2
#define NAME_TABLE_H_MASK 1

/* Sprite RAM */
extern uint8_t SPRRAM[];

#define SPR_Y 0
#define SPR_CHR 1
#define SPR_ATTR 2
#define SPR_X 3
#define SPR_ATTR_COLOR 0x3
#define SPR_ATTR_V_FLIP 0x80
#define SPR_ATTR_H_FLIP 0x40
#define SPR_ATTR_PRI 0x20

/* PPU Register */
extern uint8_t PPU_R0;
extern uint8_t PPU_R1;
extern uint8_t PPU_R2;
extern uint8_t PPU_R3;
extern uint8_t PPU_R7;

extern uint8_t PPU_Scr_V;
extern uint8_t PPU_Scr_V_Next;
extern uint8_t PPU_Scr_V_Byte;
extern uint8_t PPU_Scr_V_Byte_Next;
extern uint8_t PPU_Scr_V_Bit;
extern uint8_t PPU_Scr_V_Bit_Next;

extern uint8_t PPU_Scr_H;
extern uint8_t PPU_Scr_H_Next;
extern uint8_t PPU_Scr_H_Byte;
extern uint8_t PPU_Scr_H_Byte_Next;
extern uint8_t PPU_Scr_H_Bit;
extern uint8_t PPU_Scr_H_Bit_Next;

extern uint8_t PPU_Latch_Flag;
extern uint16_t PPU_Addr;
extern uint16_t PPU_Temp;
extern uint16_t PPU_Increment;

extern uint8_t PPU_Latch_Flag;
extern uint8_t PPU_UpDown_Clip;

#define R0_NMI_VB 0x80
#define R0_NMI_SP 0x40
#define R0_SP_SIZE 0x20
#define R0_BG_ADDR 0x10
#define R0_SP_ADDR 0x08
#define R0_INC_ADDR 0x04
#define R0_NAME_ADDR 0x03

#define R1_BACKCOLOR 0xe0
#define R1_SHOW_SP 0x10
#define R1_SHOW_SCR 0x08
#define R1_CLIP_SP 0x04
#define R1_CLIP_BG 0x02
#define R1_MONOCHROME 0x01

#define R2_IN_VBLANK 0x80
#define R2_HIT_SP 0x40
#define R2_MAX_SP 0x20
#define R2_WRITE_FLAG 0x10

#define SCAN_TOP_OFF_SCREEN 0
#define SCAN_ON_SCREEN 1
#define SCAN_BOTTOM_OFF_SCREEN 2
#define SCAN_UNKNOWN 3
#define SCAN_VBLANK 4

#define SCAN_TOP_OFF_SCREEN_START 0
#define SCAN_ON_SCREEN_START 8
#define SCAN_BOTTOM_OFF_SCREEN_START 232
#define SCAN_UNKNOWN_START 240
#define SCAN_VBLANK_START 243
#define SCAN_VBLANK_END 262

#define STEP_PER_SCANLINE 112
#define STEP_PER_FRAME 29828

/* Develop Scroll Registers */
#define InfoNES_SetupScr()                                 \
    {                                                      \
        /* V-Scroll Register */                            \
        PPU_Scr_V_Next = (uint8_t)(PPU_Addr & 0x001f);        \
        PPU_Scr_V_Byte_Next = PPU_Scr_V_Next >> 3;         \
        PPU_Scr_V_Bit_Next = PPU_Scr_V_Next & 0x07;        \
                                                           \
        /* H-Scroll Register */                            \
        PPU_Scr_H_Next = (uint8_t)((PPU_Addr & 0x03e0) >> 5); \
        PPU_Scr_H_Byte_Next = PPU_Scr_H_Next >> 3;         \
        PPU_Scr_H_Bit_Next = PPU_Scr_H_Next & 0x07;        \
    }

/* Current Scanline */
extern uint16_t PPU_Scanline;

/* Scanline Table */
extern uint8_t PPU_ScanTable[];

/* Name Table Bank */
extern uint8_t PPU_NameTableBank;

/* BG Base Address */
extern uint8_t *PPU_BG_Base;

/* Sprite Base Address */
extern uint8_t *PPU_SP_Base;

/* Sprite Height */
extern uint16_t PPU_SP_Height;

/* NES display size */
#define NES_DISP_WIDTH 256
#define NES_DISP_HEIGHT 240

/* VRAM Write Enable ( 0: Disable, 1: Enable ) */
extern uint8_t byVramWriteEnable;

/* Frame IRQ ( 0: Disabled, 1: Enabled )*/
extern uint8_t FrameIRQ_Enable;
extern uint16_t FrameStep;

/*-------------------------------------------------------------------*/
/*  Display and Others resouces                                      */
/*-------------------------------------------------------------------*/

/* Frame Skip */
extern uint16_t FrameSkip;
extern uint16_t FrameCnt;
extern uint16_t FrameWait;

extern uint16_t *WorkFrame;

extern uint8_t ChrBuf[];

extern uint8_t ChrBufUpdate;

extern uint16_t PalTable[];

/*-------------------------------------------------------------------*/
/*  APU and Pad resources                                            */
/*-------------------------------------------------------------------*/

extern uint8_t APU_Reg[];
extern int APU_Mute;

extern uint32_t PAD1_Latch;
extern uint32_t PAD2_Latch;
extern uint32_t PAD_System;
extern uint32_t PAD1_Bit;
extern uint32_t PAD2_Bit;

#define PAD_SYS_QUIT 1
#define PAD_SYS_OK 2
#define PAD_SYS_CANCEL 4
#define PAD_SYS_UP 8
#define PAD_SYS_DOWN 0x10
#define PAD_SYS_LEFT 0x20
#define PAD_SYS_RIGHT 0x40

#define PAD_PUSH(a, b) (((a) & (b)) != 0)

/*-------------------------------------------------------------------*/
/*  Mapper Function                                                  */
/*-------------------------------------------------------------------*/

/* Initialize Mapper */
extern void (*MapperInit)();
/* Write to Mapper */
extern void (*MapperWrite)(uint16_t wAddr, uint8_t byData);
/* Write to SRAM */
extern void (*MapperSram)(uint16_t wAddr, uint8_t byData);
/* Write to APU */
extern void (*MapperApu)(uint16_t wAddr, uint8_t byData);
/* Read from Apu */
extern uint8_t (*MapperReadApu)(uint16_t wAddr);
/* Callback at VSync */
extern void (*MapperVSync)(void);
/* Callback at HSync */
extern void (*MapperHSync)(void);
/* Callback at PPU read/write */
extern void (*MapperPPU)(uint16_t wAddr);
/* Callback at Rendering Screen 1:BG, 0:Sprite */
extern void (*MapperRenderScreen)(uint8_t byMode);

/*-------------------------------------------------------------------*/
/*  ROM information                                                  */
/*-------------------------------------------------------------------*/

/* .nes File Header */
struct NesHeader_tag {
    uint8_t byID[4];
    uint8_t byRomSize;
    uint8_t byVRomSize;
    uint8_t byInfo1;
    uint8_t byInfo2;
    uint8_t byReserve[8];
};

/* .nes File Header */
extern struct NesHeader_tag NesHeader;

/* Mapper No. */
extern uint8_t MapperNo;

/* Other */
extern uint8_t ROM_Mirroring;
extern uint8_t ROM_SRAM;
extern uint8_t ROM_Trainer;
extern uint8_t ROM_FourScr;

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

/* Initialize InfoNES */
void InfoNES_Init(void);

/* Completion treatment */
void InfoNES_Fin(void);

/* Load a cassette */
int InfoNES_Load(const char *pszFileName);

/* Reset InfoNES */
int InfoNES_Reset(void);

/* Initialize PPU */
void InfoNES_SetupPPU(void);

/* Set up a Mirroring of Name Table */
void InfoNES_Mirroring(int nType);

/* The main loop of InfoNES */
void InfoNES_Main(void);

/* The loop of emulation */
void InfoNES_Cycle(void);

/* A function in H-Sync */
int InfoNES_HSync(void);

/* Render a scanline */
void InfoNES_DrawLine(void);

/* Get a position of scanline hits sprite #0 */
void InfoNES_GetSprHitY(void);

/* Develop character data */
void InfoNES_SetupChr(void);

#endif /* !InfoNES_H_INCLUDED */
