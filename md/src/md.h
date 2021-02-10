// DGen

#ifndef __MD_H__
#define __MD_H__

#define VER "1.23"


#ifndef M68K__HEADER
extern "C"
{
#include "m68k.h"
}
#endif

#ifndef	_MZ80_H_
#include "mz80.h"
#endif

//#define BUILD_YM2612
extern "C" {
#include "fm.h"
}

extern "C" {
int SN76496_init(int chip,int clock,int sample_rate,int sample_bits);
void SN76496Write(int chip,int data);
void SN76496Update_16(int chip,void *buffer,int length);
}

extern "C" int test_ctv(unsigned char *dest, int len);
extern "C" int blur_bitmap_16(unsigned char *dest, int len);
extern "C" int blur_bitmap_15(unsigned char *dest, int len);

struct bmap { unsigned char *data; int w,h; int pitch; int bpp; };

// Info passed to render the screen (possibly line by line)
struct dgen_sinfo { unsigned char *vram,*cram,*vsram,*vdp_reg; };
// New struct, happily encapsulates all the sound info
struct sndinfo { signed short *l, *r; int len; };

int draw_md_graphics
  (struct bmap *bm,unsigned char passpal[256],struct dgen_sinfo *si,
     int ymin, int ymax, int layer_sel);
int get_md_palette(unsigned char pal[256],unsigned char *cram);

class md;
class md_vdp
{
public:
  // Next three lines are the state of the VDP
  // They have to be public so we can save and load states
  // and draw the screen.
  unsigned char *mem,*vram,*cram,*vsram;
  unsigned char reg[0x20];
  int rw_mode,rw_addr,rw_dma;
private:
  int poke_vram (int addr,unsigned char d);
  int poke_cram (int addr,unsigned char d);
  int poke_vsram(int addr,unsigned char d);
  int dma_len();
  int dma_addr();
  unsigned char dma_mem_read(int addr);
  int putword(unsigned short d);
  int putbyte(unsigned char d);
  int ok;
  // Used by draw_scanline to render the different display components
  void draw_tile1(int which, int line, unsigned char *where);
  void draw_tile1_solid(int which, int line, unsigned char *where);
  void draw_tile2(int which, int line, unsigned char *where);
  void draw_tile2_solid(int which, int line, unsigned char *where);
/* FIXME: do 3 bytes/pixel */
  void draw_tile3(int which, int line, unsigned char *where);
  void draw_tile3_solid(int which, int line, unsigned char *where);
  void draw_tile4(int which, int line, unsigned char *where);
  void draw_tile4_solid(int which, int line, unsigned char *where);
  void draw_window(int line, int front);
  void draw_sprites(int line, int front);
  void draw_plane_back0(int line);
  void draw_plane_back1(int line);
  void draw_plane_front0(int line);
  void draw_plane_front1(int line);
  // Working variables for the above
  unsigned char sprite_order[0x101], *sprite_base;
  unsigned sprite_count, Bpp, Bpp_times8;
  unsigned char *dest;
public:
  md_vdp(); ~md_vdp();
  md *belongs;
// These are called by MEM.CPP
  int command(unsigned int cmd);
  unsigned short readword();
  unsigned char readbyte();
  int writeword(unsigned short d);
  int writebyte(unsigned char d);

  int okay() {return ok;}
  unsigned char *dirt; // Bitfield: what has changed VRAM/CRAM/VSRAM/Reg
  int get_screen_info(struct dgen_sinfo *si); // For Graph
  int reset();

  unsigned int *highpal;
  // Draw a scanline
  void draw_scanline(struct bmap *bits, int line);
};

class md
{
private:
  int romlen;
  int ok; 
  unsigned char *mem,*rom,*ram,*z80ram;
  // Saveram stuff:
  unsigned char *saveram; // The actual saveram buffer
  unsigned save_start, save_len; // Start address and length
  int save_prot, save_active; // Flags set from $A130F1
public:
  md_vdp vdp;
private:
  struct mz80context z80;
  int star_mz80_on();
  int star_mz80_off();

  int z80_bank68k; // 9 bits
  int z80_online;
  int odo; //,odo_frame_max; // value of odo now/at end of frame
  int odo_line_start,odo_line_end,odo_line_len,ras,frame;
  int hint_countdown;
  int z80_extra_cycles;

  // Note order is (0) Vblank end -------- Vblank Start -- (HIGH)
  // So int6 happens in the middle of the count

  int coo_waiting; // after 04 write which is 0x4xxx or 0x8xxx
  unsigned int coo_cmd; // The complete command
  int aoo3_toggle,aoo5_toggle,aoo3_six,aoo5_six;
  int aoo3_six_timeout, aoo5_six_timeout;
  int calculate_hcoord();
  unsigned char  calculate_coo8();
  unsigned char  calculate_coo9();
  int may_want_to_get_pic(struct bmap *bm,unsigned char retpal[256],int mark);
  int may_want_to_get_sound(struct sndinfo *sndi);
  int z80_int_pending;
  
  void run_to_odo_star(int odo_to);
  void run_to_odo_musa(int odo_to);
  void run_to_odo_m68kem(int odo_to);
  void run_to_odo_z80(int odo_to);

  int fm_timer_callback();
  int myfm_read(int a);
  int mysn_write(int v);
  int fm_sel[2],fm_tover[2],ras_fm_ticker[4];
  signed short fm_reg[2][0x100]; // All of them (-1 = not def'd yet)

  int dac_data[0x138], dac_enabled, dac_last;
  // Since dac values are normally shifted << 6, 1 is used to mark unchanged
  // values.
  void dac_init() { dac_last = 1; dac_enabled = 0; dac_clear(); }
  void dac_clear() {
    dac_data[0] = dac_last;
    for(int i = 1; i < 0x138; ++i)
      dac_data[i] = 1;
  }
  void dac_submit(int d) { 
    dac_last = (d - 0x80) << 6; if(dac_enabled) dac_data[ras] = dac_last;
  }
  void dac_enable(int d) {
    dac_enabled = d & 0x80;
    dac_data[ras] = (dac_enabled? dac_last : 1);
  }
  
public:
  int myfm_write(int a,int v,int md);
  int flush_fm_to_mame();
  int pause, layer_sel;
  // public struct, full with data from the cartridge header
  struct _carthead_ {
    char system_name[0x10];           // "SEGA GENESIS    ", "SEGA MEGA DRIVE  "
    char copyright[0x10];             // "(C)SEGA 1988.JUL"
    char domestic_name[0x30];         // Domestic game name
    char overseas_name[0x30];         // Overseas game name
    char product_no[0xe];             // "GM XXXXXXXX-XX" or "GM MK-XXXX -00"
    unsigned short checksum;          // ROM checksum
    char control_data[0x10];          // I/O use data (usually only joystick)
    unsigned rom_start, rom_end;      // ROM start & end addresses
    unsigned ram_start, ram_end;      // RAM start & end addresses
    unsigned short save_magic;        // 0x5241("RA") if saveram, else 0x2020
    unsigned short save_flags;        // Misc saveram info
    unsigned save_start, save_end;    // Saveram start & end
    unsigned short modem_magic;       // 0x4d4f("MO") if uses modem, else 0x2020
    char modem_firm[4];               // Modem firm name (should be same as (c))
    char modem_ver[4];                // yy.z, yy=ID number, z=version
    char memo[0x28];                  // Extra data
    char countries[0x10];             // Country code
  } cart_head;
  int snd_mute; // &1=fm &2=psg &4=dac
  int country_ver;
  int pal; // 0 = NTSC 1 = PAL
  int one_frame_star(struct bmap *bm,unsigned char retpal[256],struct sndinfo *sndi);
  int one_frame_musa(struct bmap *bm,unsigned char retpal[256],struct sndinfo *sndi);
  int one_frame_m68kem(struct bmap *bm,unsigned char retpal[256],struct sndinfo *sndi);
  int one_frame(struct bmap *bm,unsigned char retpal[256],struct sndinfo *sndi);

  int pad[2];
// c000004 bit 1 write fifo empty, bit 0 write fifo full (???)
// c000005 vint happened, (sprover, coll, oddinint)
// invblank, inhblank, dma busy, pal
  unsigned char coo4,coo5;
  int okay() {return ok;}
  md();
  ~md();
  int plug_in(unsigned char *cart,int len);
  int unplug();
  int load(char *name);

  int reset();

  unsigned misc_readbyte(unsigned a);
  void misc_writebyte(unsigned a,unsigned d);
  unsigned misc_readword(unsigned a);
  void misc_writeword(unsigned a,unsigned d);

  int z80_init();
  unsigned char z80_read(unsigned int a);
  void z80_write(unsigned int a,unsigned char v);
  unsigned short z80_port_read(unsigned short a);
  void z80_port_write(unsigned short a,unsigned char v);

  int cpu_emu;// OK to read it but call change_cpu_emu to change it
  int change_cpu_emu(int to);
  mz80context&   z80_context() {return z80;}
  int load(void *hand,char *desc);
  int save(void *hand,char *desc);
  int import_gst(void *hand);
  int export_gst(void *hand);

  char romfilename[256]; // If known

  int get_screen_info(struct dgen_sinfo *si)
  { return vdp.get_screen_info(si); }
  int z80dump();

  // Fix ROM checksum
  //void fix_rom_checksum();QQM

  // Added by Joe Groff:
  // Patch the ROM code, using Game Genie/Hex codes
  //void patch(const char *list);QQM
  // Get/put the battery save RAM
  int has_save_ram();
  //void get_save_ram(void *from);QQM
  //void put_save_ram(void *into);QQM
  
  // Added by Phillip K. Hornung <redx@pknet.com>
  // Linux joystick initialization and handling routines
  void init_joysticks();
  void read_joysticks();
};

inline int md::has_save_ram()
{
  return save_len;
}

#endif // __MD_H__
