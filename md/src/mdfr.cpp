// DGen/SDL v1.18+
// Megadrive 1 Frame module
// Many, many thanks to John Stiles for the new structure of this module! :)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "md.h"

int split_screen=0;

#ifdef COMPILE_WITH_STAR
void md::run_to_odo_star(int odo_to)
{
  int odo_start = s68000readOdometer();
  s68000exec(odo_to - odo_start);
}
#endif

#ifdef COMPILE_WITH_MUSA
void md::run_to_odo_musa(int odo_to)
{
  odo += m68k_execute(odo_to - odo);
}
#endif

#ifdef COMPILE_WITH_M68KEM
void md::run_to_odo_m68kem(int odo_to)
{
  odo += m68000_execute(odo_to - odo);
}
#endif

void md::run_to_odo_z80(int odo_to)
{
  if(z80_online)
    {
      odo_to >>= 1;
      int odo_start = mz80GetElapsedTicks(0);
      mz80exec(odo_to - odo_start);
    }
}

#define LINES_PER_FRAME_NTSC 0x106 // Number of scanlines in NTSC (w/ vblank)
#define LINES_PER_FRAME_PAL  0x138 // Number of scanlines in PAL  (w/ vblank)

#define LINES_PER_FRAME (pal? LINES_PER_FRAME_PAL : LINES_PER_FRAME_NTSC)

#define VBLANK_LINE_NTSC 0xE0 // vblank location for NTSC (and PAL 28-cel mode)
#define VBLANK_LINE_PAL  0xF0 // vblank location for PAL 30-cel mode

#define VBLANK_LINE ((pal && (vdp.reg[1] & 8))? \
                     VBLANK_LINE_PAL : VBLANK_LINE_NTSC)

#define scanlength (pal? (7189547/50/0x138) : (8000000/60/0x106))

#ifdef COMPILE_WITH_STAR
int md::one_frame_star(struct bmap *bm, unsigned char retpal[256], struct sndinfo *sndi)
{
  int hints, odom = 0;

  star_mz80_on(); // VERY IMPORTANT! Must call before using star/mz80

  // Clear odos
  s68000tripOdometer();
  mz80GetElapsedTicks(1);

  // Raster zero causes special things to happen :)
  coo4 = 0x37; // Init status register
  if(vdp.reg[12] & 0x2) coo5 ^= 0x10; // Toggle odd/even for interlace
  if(vdp.reg[1]  & 0x40) coo5 &= ~0x88; // Clear vblank and vint
  if(!(vdp.reg[1] & 0x20)) coo5 |= 0x80; // If vint disabled, vint happened
					 // is permanently set
  hints = vdp.reg[10]; // Set hint counter

  // Video display! :D
  for(ras = 0; ras < VBLANK_LINE; ++ras)
    {
      fm_timer_callback(); // update sound timers

      if((vdp.reg[0] & 0x10) && (--hints < 0))
        {
	  // Trigger hint
	  s68000interrupt(4, -1);
	  hints = vdp.reg[10];
	  may_want_to_get_pic(bm, retpal, 1);
	} else
	  may_want_to_get_pic(bm, retpal, 0);

      coo5 |= 4; // hblank comes before, about 36/209 of the whole scanline
      run_to_odo_star(odom + (scanlength * 36/209));
      // Do hdisplay now
      odom += scanlength;
      coo5 &= ~4;
      run_to_odo_star(odom);

      // Do Z80
      run_to_odo_z80(odom);
    }
  // Now we're in vblank, more special things happen :)
  // Blank everything, and trigger vint
  coo5 |= 0x8C;
  if(vdp.reg[1] & 0x20) s68000interrupt(6, -1);
  if(z80_online) mz80int(0);

  // Run the course of vblank
  for(; ras < LINES_PER_FRAME; ++ras)
    {
      fm_timer_callback();

      // No interrupts happen in vblank

      odom += scanlength;
      run_to_odo_star(odom);
      run_to_odo_z80(odom);
    }

  // Fill the sound buffers
  if(sndi) may_want_to_get_sound(sndi);
  
  // Shut off mz80/star - VERY IMPORTANT!
  star_mz80_off();

  return 0;
}
#endif // COMPILE_WITH_STAR

#ifdef COMPILE_WITH_MUSA
int md::one_frame_musa(struct bmap *bm, unsigned char retpal[256], struct sndinfo *sndi)
{
  int hints, odom = 0;

  star_mz80_on(); // VERY IMPORTANT! Must call before using star/mz80

  // Clear odos
  odo = 0;
  mz80GetElapsedTicks(1);

  // Raster zero causes special things to happen :)
  coo4 = 0x37; // Init status register
  if(vdp.reg[12] & 0x2) coo5 ^= 0x10; // Toggle odd/even for interlace
  if(vdp.reg[1]  & 0x40) coo5 &= ~0x88; // Clear vblank and vint
  if(!(vdp.reg[1] & 0x20)) coo5 |= 0x80; // If vint disabled, vint happened
					 // is permanently set
  hints = vdp.reg[10]; // Set hint counter

  // Video display! :D
  for(ras = 0; ras < VBLANK_LINE; ++ras)
    {
      fm_timer_callback(); // update sound timers

      if((vdp.reg[0] & 0x10) && (--hints < 0))
        {
	  // Trigger hint
	  m68k_assert_irq(4); m68k_clear_irq(4);
	  hints = vdp.reg[10];
	  may_want_to_get_pic(bm, retpal, 1);
	} else may_want_to_get_pic(bm, retpal, 0);
      coo5 |= 4; // hblank comes before, about 36/209 of the whole scanline
      run_to_odo_musa(odom + (scanlength * 36/209));
      // Do hdisplay now
      odom += scanlength;
      coo5 &= ~4;
      run_to_odo_musa(odom);

      // Do Z80
      run_to_odo_z80(odom);
    }
  // Now we're in vblank, more special things happen :)
  // Blank everything, and trigger vint
  coo5 |= 0x8C;
  if(vdp.reg[1] & 0x20) { m68k_assert_irq(6); m68k_clear_irq(6); }
  if(z80_online) mz80int(0);

  // Run the course of vblank
  for(; ras < LINES_PER_FRAME; ++ras)
    {
      fm_timer_callback();

      // No interrupts happen in vblank :)

      odom += scanlength;
      run_to_odo_musa(odom);
      run_to_odo_z80(odom);
    }

  // Fill the sound buffers
  if(sndi) may_want_to_get_sound(sndi);
  
  // Shut off mz80/star - VERY IMPORTANT!
  star_mz80_off();

  return 0;
}
#endif // COMPILE_WITH_MUSA

// FIXME: I'm not going to do this until I figure out 68kem better
#ifdef COMPILE_WITH_M68KEM
int md::one_frame_m68kem(struct bmap *bm, unsigned char retpal[256], struct sndinfo *sndi)
{}
#endif

int md::one_frame(
  struct bmap *bm,unsigned char retpal[256],
  struct sndinfo *sndi)
{
  ++frame;
  switch(cpu_emu)
    {
#ifdef COMPILE_WITH_STAR
      case 0: return one_frame_star(bm, retpal, sndi);
#endif
#ifdef COMPILE_WITH_MUSA
      case 1: return one_frame_musa(bm, retpal, sndi);
#endif
#ifdef COMPILE_WITH_M68KEM
      case 2: return one_frame_m68kem(bm, retpal, sndi);
#endif
      // Something's screwy here...
      default: return 1;
    }
}

int md::calculate_hcoord()
{
  int x=0,hcoord;
  // c00009 is the H counter (I believe it's (h-coord>>1)&0xff)
#ifdef COMPILE_WITH_STAR
  if (cpu_emu==0) x = s68000readOdometer() - (ras * scanlength);
#endif
#if defined(COMPILE_WITH_MUSA) || defined(COMPILE_WITH_M68KEM)
  if (cpu_emu==1) x = odo - (ras * scanlength);
#endif

  // hcoord=-56 (inclusive) to 364 (inclusive) in 40 column mode

  hcoord=(x*416)/scanlength;
  hcoord-=56;

  return hcoord;
}

unsigned char md::calculate_coo8()
{
  int hvcount;
  hvcount=ras;

  // c00008 is the V counter
  if (calculate_hcoord()>=330) hvcount++;
    // v counter seems to be a bit ahead of where h counter wraps

  if (hvcount>(VBLANK_LINE+0xa)) hvcount-=6; // vcounter E5-EA appears twice!

  hvcount&=0xff;
  return hvcount;
}

unsigned char md::calculate_coo9()
{
  int coo9;
  coo9=(calculate_hcoord()>>1)&0xff;

  return coo9;
}


// *************************************
//       May want to get pic or sound
// *************************************

inline int md::may_want_to_get_pic(struct bmap *bm,unsigned char retpal[256],int/*mark*/)
{
  if (bm==NULL) return 0;

  if (ras>=0 && ras<VBLANK_LINE )
    vdp.draw_scanline(bm, ras);
  if(retpal && ras == 100) get_md_palette(retpal, vdp.cram);
  return 0;
}

int md::may_want_to_get_sound(struct sndinfo *sndi)
{
  int i, len = sndi->len;
  int in_dac, cur_dac = 0, acc_dac = len;
  int *dac = dac_data - 1;

  // Get the PSG
  SN76496Update_16(0, sndi->l, len);

  // We bring in the dac, but stretch it out to fit the real length.
  for(i = 0; i < len; ++i)
    {
      acc_dac += LINES_PER_FRAME;
      if(acc_dac >= len)
	{
	  acc_dac -= len;
	  in_dac = *(++dac);
	  if(in_dac != 1) cur_dac = in_dac;
	}
      sndi->l[i] += cur_dac;
    }
  // Copy mono signal to center channel
  memcpy(sndi->r, sndi->l, len * sizeof(short));

  // Add in the stereo FM buffer
  FMSAMPLE *buf[2];
  buf[0] = (FMSAMPLE*) sndi->l;
  buf[1] = (FMSAMPLE*) sndi->r;
  YM2612UpdateOne(0, (void**)buf, len);

  // Clear the dac for next frame
  dac_clear();
  return 0;
}

