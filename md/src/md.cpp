// DGen/SDL v1.17+
// Megadrive C++ module

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "md.h"
#include "romload.h"
#include "rc-vars.h"
#include "decode.h"

/**
 * This sets up an array of memory locations for Musashi.
 */
void md::musa_memory_map()
{
    unsigned int rom0_len = romlen;
    unsigned int rom1_sta = 0;
    unsigned int rom1_len = 0;

    m68k_register_memory(NULL, 0);
    if (save_len) {
        DEBUG(("[%06x-%06x] ???? (SAVE)",
               save_start, (save_start + save_len - 1)));
        if (save_start < romlen) {
            /* Punch a hole through the ROM area. */
            rom0_len = save_start;
            /* Add entry for ROM leftovers, if any. */
            if ((save_start + save_len) < romlen) {
                rom1_sta = (save_start + save_len);
                rom1_len = (romlen - rom1_sta);
            }
        }
    }

#ifdef ROM_BYTESWAP
#define S 1
#else
#define S 0
#endif

    const m68k_mem_t mem[3] = {
        // r, w, x, swab, addr, size, mask, mem
        { 1, 0, 1, S, 0x000000, rom0_len, 0x7fffff, rom }, // M68K ROM
        { 1, 1, 1, 1, 0xe00000, 0x200000, 0x00ffff, ram }, // M68K RAM
        { 1, 0, 1, S, rom1_sta, rom1_len, 0x7fffff, &rom[rom1_sta] }
    };
    unsigned int i;
    unsigned int j = 0;

    for (i = 0; ((i < elemof(mem)) && (j < elemof(musa_memory))); ++i) {
        if (mem[i].size == 0)
            continue;
        DEBUG(("[%06x-%06x] %c%c%c%c (%s)",
               mem[i].addr,
               (mem[i].addr + mem[i].size - 1),
               (mem[i].r ? 'r' : '-'),
               (mem[i].w ? 'w' : '-'),
               (mem[i].x ? 'x' : '-'),
               (mem[i].swab ? 's' : '-'),
               (mem[i].w ? "RAM" : "ROM")));
        musa_memory[j] = mem[i];
        ++j;
    }
    if (j)
        m68k_register_memory(musa_memory, j);
    else
        DEBUG(("no memory region defined"));
}

int musa_irq_callback(int level)
{
    (void)level;
    assert(md::md_musa != NULL);
    md::md_musa->m68k_vdp_irq_handler();
    return M68K_INT_ACK_AUTOVECTOR;
}

/**
 * Resets everything (Z80, M68K, VDP, etc).
 * @return 0 on success
 */
int md::reset()
{
    // Clear memory.
    memset(mem, 0, 0x20000);
    // Reset the VDP.
    vdp.reset();
    // Erase CPU states.
    memset(&m68k_state, 0, sizeof(m68k_state));
    memset(&z80_state, 0, sizeof(z80_state));
    m68k_state_restore();
    z80_state_restore();

    md_set_musa(1);
    m68k_pulse_reset();
    md_set_musa(0);

    aoo3_toggle=aoo5_toggle=aoo3_six=aoo5_six
    =aoo3_six_timeout=aoo5_six_timeout
    =coo4=coo5=0;
  pad[0] = MD_PAD_UNTOUCHED;
  pad[1] = MD_PAD_UNTOUCHED;
  memset(pad_com, 0, sizeof(pad_com));

#ifdef WITH_PICO
  // Initialize Pico pen X, Y coordinates
  pico_pen_coords[0] = 0x3c;
  pico_pen_coords[1] = 0x1fc;
#endif

  // Reset FM registers
  fm_reset();
  dac_init();

  memset(&odo, 0, sizeof(odo));
  ras = 0;

  z80_st_running = 0;
  m68k_st_running = 0;
  z80_reset();
  z80_st_busreq = 1;
  z80_st_reset = 1;
  z80_st_irq = 0;
  return 0;
}


/**
 * Initialise the Z80.
 */
void md::z80_init()
{
    z80_st_busreq = 1;
    z80_st_reset = 0;
    z80_bank68k = 0xff8000;
}

/**
 * Reset the Z80.
 */
void md::z80_reset()
{
    z80_bank68k = 0xff8000;
}

/**
 * Initialise sound.
 * @return True when successful.
 */
bool md::init_sound()
{
    if (lock == false)
        return false;
    if (ok_ym2612) {
        YM2612Shutdown();
        ok_ym2612 = false;
    }
    if (ok_sn76496) {
        (void)0;
        ok_sn76496 = false;
    }
    // Initialize two additional chips when MJazz is enabled.
    if (YM2612Init((dgen_mjazz ? 3 : 1),
               (((pal) ? PAL_MCLK : NTSC_MCLK) / 7),
               dgen_soundrate, dgen_mjazz, NULL, NULL))
        return false;
    ok_ym2612 = true;
    if (SN76496_init(0,
             (((pal) ? PAL_MCLK : NTSC_MCLK) / 15),
             dgen_soundrate, 16))
        return false;
    ok_sn76496 = true;
    return true;
}

/**
 * Switch to PAL or NTSC.
 * This method's name is a bit misleading.  This switches to PAL or not
 * depending on "md::pal".
 */
void md::init_pal()
{
    unsigned int hc;

    if (pal) {
        mclk = PAL_MCLK;
        lines = PAL_LINES;
        vhz = PAL_HZ;
    }
    else {
        mclk = NTSC_MCLK;
        lines = NTSC_LINES;
        vhz = NTSC_HZ;
    }
    clk0 = (mclk / 15);
    clk1 = (mclk / 7);
    // Initialize horizontal counter table (Gens style)
    for (hc = 0; (hc < 512); ++hc) {
        // H32
        hc_table[hc][0] = (((hc * 170) / M68K_CYCLES_PER_LINE) - 0x18);
        // H40
        hc_table[hc][1] = (((hc * 205) / M68K_CYCLES_PER_LINE) - 0x1c);
    }
}

bool md::lock = false;

/**
 * MD constructor.
 * @param pal True if we are running the MD in PAL mode.
 * @param region Region to emulate ('J', 'U', or 'E').
 */
md::md(bool pal, char region):
    md_musa_ref(0), md_musa_prev(0),
    pal(pal), ok_ym2612(false), ok_sn76496(false),
    vdp(*this), region(region), plugged(false)
{
    // Only one MD object is allowed to exist at once.
    if (lock)
        return;
    lock = true;

    // PAL or NTSC.
    init_pal();

    // Start up the sound chips.
    if (init_sound() == false)
        goto cleanup;

    romlen = no_rom_size;
    rom = (uint8_t*)no_rom;
  mem=ram=z80ram=saveram=NULL;
  save_start=save_len=save_prot=save_active=0;

  fm_reset();

#ifdef WITH_PICO
    pico_enabled = false;
#endif

  memset(&m68k_state, 0, sizeof(m68k_state));
  memset(&z80_state, 0, sizeof(z80_state));

    ctx_musa = calloc(1, m68k_context_size());
    if (ctx_musa == NULL)
        goto cleanup;
    md_set_musa(1);
    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_register_memory(NULL, 0);
    m68k_set_int_ack_callback(musa_irq_callback);
    md_set_musa(0);

  memset(&cart_head, 0, sizeof(cart_head));

  memset(romname, 0, sizeof(romname));

  ok=0;

  //  Format of pad is: __SA____ UDLRBC__

  rom = (uint8_t*)no_rom;
  romlen = no_rom_size;
  mem=ram=z80ram=NULL;
  mem=(unsigned char *)malloc(0x20008);
    if (mem == NULL)
        goto cleanup;
  memset(mem,0,0x20000);
  ram=   mem+0x00000;
  z80ram=mem+0x10000;
  // Hack for DrZ80 to avoid crashing when PC leaves z80ram.
  z80ram[0x10000] = 0x00; // NOP
  z80ram[0x10001] = 0x00; // NOP
  z80ram[0x10002] = 0x00; // NOP
  z80ram[0x10003] = 0x00; // NOP
  z80ram[0x10004] = 0x00; // NOP
  z80ram[0x10005] = 0xc3; // JP 0x0000
  z80ram[0x10006] = 0x00;
  z80ram[0x10007] = 0x00;

    md_set_musa(1);
    musa_memory_map();
    md_set_musa(0);

    // M68K: 0 = none, 1 = StarScream, 2 = Musashi, 3 = Cyclone
    switch (dgen_emu_m68k) {
    case 2:
        cpu_emu = CPU_EMU_MUSA;
        break;
    default:
        cpu_emu = CPU_EMU_NONE;
        break;
    }
    // Z80: 0 = none, 1 = CZ80, 2 = MZ80, 3 = DrZ80
    switch (dgen_emu_z80) {
    default:
        z80_core = Z80_CORE_NONE;
        break;
    }

    md_set_musa(1);
    m68k_pulse_reset();
    md_set_musa(0);

  z80_init();

  reset(); // reset megadrive

    patch_elem = NULL;

  ok=1;

    return;
cleanup:
    if (ok_ym2612)
        YM2612Shutdown();
    if (ok_sn76496)
        (void)0;
    free(ctx_musa);
    free(mem);
    memset(this, 0, sizeof(*this));
    lock = false;
}

md::~md()
{
    assert(rom != NULL);
    if (rom != no_rom)
        unplug();

  free(mem);
  rom=mem=ram=z80ram=NULL;

    free(ctx_musa);

    if (ok_ym2612)
        YM2612Shutdown();
    if (ok_sn76496)
        (void)0;
    ok=0;
    memset(this, 0, sizeof(*this));
    lock = false;
}

#ifdef ROM_BYTESWAP
/**
 * Byteswaps memory.
 * @param[in] start Byte array of cart memory.
 * @param len How many bytes to byteswap.
 * @return 0 on success (always 0).
 */
// Byteswaps memory
int byteswap_memory(unsigned char *start,int len)
{ int i; unsigned char tmp;
  for (i=0;i<len;i+=2)
  { tmp=start[i+0]; start[i+0]=start[i+1]; start[i+1]=tmp; }
  return 0;
}
#endif

/**
 * Plug a cart into the MD.
 * @param[in] cart Cart's memory as a byte array.
 * @param len Length of the cart.
 * @return 0 on success.
 */
int md::plug_in(unsigned char *cart,int len)
{
  // Plug in the cartridge specified by the uchar *
  // NB - The megadrive will free() it if unplug() is called, or it exits
  // So it must be a single piece of malloced data
  if (cart==NULL)
      return 1;
  if (len<=0)
      return 1;
#ifdef ROM_BYTESWAP
  byteswap_memory(cart,len); // for starscream
#endif
  romlen=len;
  rom=cart;
  // Get saveram start, length (remember byteswapping)
  // First check magic, if there is saveram
  if(rom[ROM_ADDR(0x1b0)] == 'R' && rom[ROM_ADDR(0x1b1)] == 'A')
    {
      save_start = rom[ROM_ADDR(0x1b4)] << 24 | rom[ROM_ADDR(0x1b5)] << 16 |
                   rom[ROM_ADDR(0x1b6)] << 8  | rom[ROM_ADDR(0x1b7)];
      save_len = rom[ROM_ADDR(0x1b8)] << 24 | rom[ROM_ADDR(0x1b9)] << 16 |
                 rom[ROM_ADDR(0x1ba)] << 8  | rom[ROM_ADDR(0x1bb)];
      // Make sure start is even, end is odd, for alignment
// A ROM that I came across had the start and end bytes of
// the save ram the same and wouldn't work.  Fix this as seen
// fit, I know it could probably use some work. [PKH]
      if(save_start != save_len) {
        if(save_start & 1) --save_start;
        if(!(save_len & 1)) ++save_len;
        save_len -= (save_start - 1);
        saveram = (unsigned char*)calloc(1, save_len);
    if (saveram == NULL) {
      save_len = 0;
      save_start = 0;
    }
    // If save RAM does not overlap main ROM, set it active by default since
    // a few games can't manage to properly switch it on/off.
    if(save_start >= (unsigned int)romlen)
      save_active = 1;
      }
      else {
        save_start = save_len = 0;
        saveram = NULL;
      }
    }
  else
    {
      save_start = save_len = 0;
      saveram = NULL;
    }
    md_set_musa(1);
    musa_memory_map();
    md_set_musa(0);
  reset(); // Reset megadrive
  return 0;
}

/**
 * Region to emulate according to dgen_region_order and ROM header.
 * @return Region identifier ('J', 'U' or 'E').
 */
uint8_t md::region_guess()
{
    char const* order = dgen_region_order.val;
    char const* avail = this->cart_head.countries;
    size_t r;
    size_t i;

    assert(order != NULL);
    assert(avail != NULL);
    for (r = 0; (order[r] != '\0'); ++r)
        for (i = 0; (i != sizeof(this->cart_head.countries)); ++i)
            if ((isprint(order[r])) &&
                (toupper(order[r]) == toupper(avail[i])))
                return toupper(order[r]);
    // Use default region.
    return dgen_region;
}

/**
 * Unplug a cart from the system.
 * @return 0 on success.
 */
int md::unplug()
{
  assert(rom != NULL);
  assert(romlen != 0);
  if (rom == no_rom) return 1;
  unload_rom(rom);
  rom = (uint8_t*)no_rom;
  romlen = no_rom_size;
  free(saveram);
  saveram = NULL;
  save_start = save_len = 0;
    md_set_musa(1);
    musa_memory_map();
    md_set_musa(0);
  memset(romname, 0, sizeof(romname));
  memset(&cart_head, 0, sizeof(cart_head));
  reset();

    while (patch_elem != NULL) {
        struct patch_elem *next = patch_elem->next;

        free(patch_elem);
        patch_elem = next;
    }
    plugged = false;

  return 0;
}

/**
 * Load a ROM.
 * @param[in] name File name of cart to load.
 * @return 0 on success.
 */
int md::load(const char *name)
{
    uint8_t *temp;
    size_t size;

    temp = load_rom(&size, name);
    if (temp == NULL)
        return 1;

    // Fill the header with ROM info (god this is ugly)
    memcpy((void*)cart_head.system_name,  (void*)(temp + 0x100), 0x10);
    memcpy((void*)cart_head.copyright,    (void*)(temp + 0x110), 0x10);
    memcpy((void*)cart_head.domestic_name,(void*)(temp + 0x120), 0x30);
    memcpy((void*)cart_head.overseas_name,(void*)(temp + 0x150), 0x30);
    memcpy((void*)cart_head.product_no,   (void*)(temp + 0x180), 0x0e);
    cart_head.checksum = temp[0x18e]<<8 | temp[0x18f]; // ugly, but endian-neutral
    memcpy((void*)cart_head.control_data, (void*)(temp + 0x190), 0x10);
    cart_head.rom_start  = temp[0x1a0]<<24 | temp[0x1a1]<<16 | temp[0x1a2]<<8 | temp[0x1a3];
    cart_head.rom_end    = temp[0x1a4]<<24 | temp[0x1a5]<<16 | temp[0x1a6]<<8 | temp[0x1a7];
    cart_head.ram_start  = temp[0x1a8]<<24 | temp[0x1a9]<<16 | temp[0x1aa]<<8 | temp[0x1ab];
    cart_head.ram_end    = temp[0x1ac]<<24 | temp[0x1ad]<<16 | temp[0x1ae]<<8 | temp[0x1af];
    cart_head.save_magic = temp[0x1b0]<<8 | temp[0x1b1];
    cart_head.save_flags = temp[0x1b2]<<8 | temp[0x1b3];
    cart_head.save_start = temp[0x1b4]<<24 | temp[0x1b5]<<16 | temp[0x1b6]<<8 | temp[0x1b7];
    cart_head.save_end   = temp[0x1b8]<<24 | temp[0x1b9]<<16 | temp[0x1ba]<<8 | temp[0x1bb];
    memcpy((void*)cart_head.memo,       (void*)(temp + 0x1c8), 0x28);
    memcpy((void*)cart_head.countries,  (void*)(temp + 0x1f0), 0x10);

#ifdef WITH_PICO
    // Check if cartridge inserted is intended for Sega Pico.
    // If it is, the Sega Pico I/O area will be enabled, and the
    // Megadrive I/O area will be disabled.
    if ((!strncmp(cart_head.system_name, "SEGA PICO", 9)) ||
        (!strncmp(cart_head.system_name, "SEGATOYS PICO", 13)))
        pico_enabled = true;
    else
        pico_enabled = false;
#endif
    // Plug it into the memory map
    plug_in(temp, size); // md then deallocates it when it's done
    plugged = true;
    return 0;
}

/**
 * Cycle through Z80 CPU implementations.
 */
void md::cycle_z80()
{
    z80_state_dump();
    z80_core = (enum z80_core)((z80_core + 1) % Z80_CORE_TOTAL);
    z80_state_restore();
}

/**
 * Cycle between M68K CPU implementations.
 */
void md::cycle_cpu()
{
    m68k_state_dump();
    cpu_emu = (enum cpu_emu)((cpu_emu + 1) % CPU_EMU_TOTAL);
    m68k_state_restore();
}

/**
 * Dump Z80 ram to a file named "dgz80ram".
 * @return Always returns 0.
 */
int md::z80dump()
{
  dump_z80ram(z80ram,0x10000);
  return 0;
}

/**
 * This takes a comma or whitespace-separated list of Game Genie and/or hex
 * codes to patch the ROM with.
 * @param[in] list List of codes separated by '\\t', '\\n', or ','.
 * @param[out] errors Number of codes that failed to apply.
 * @param[out] applied Number of codes that applied correctly.
 * @param[out] reverted Number of codes that were reverted.
 * @return 0 on success.
 */
int md::patch(const char *list, unsigned int *errors,
          unsigned int *applied, unsigned int *reverted)
{
  static const char delims[] = " \t\n,";
  char *worklist, *tok;
  struct patch p;
  int ret = 0;
  size_t wl_sz;

  if (errors != NULL)
    *errors = 0;
  if (applied != NULL)
    *applied = 0;
  if (reverted != NULL)
    *reverted = 0;

  // Copy the given list to a working list so we can strtok it
  wl_sz = strlen(list) + 1;
  worklist = (char *)malloc(wl_sz);
  if (worklist == NULL)
    return -1;
  memcpy(worklist, list, wl_sz);

  for(tok = strtok(worklist, delims); tok; tok = strtok(NULL, delims))
    {
      struct patch_elem *elem = patch_elem;
      struct patch_elem *prev = NULL;
      uint8_t *dest = rom;
      size_t mask = ~(size_t)0;
      int rev = 0;
      bool swap = true;

      // If it's empty, toss it
      if(*tok == '\0') continue;
      // Decode it
      decode(tok, &p);
      // Discard it if it was bad code
      if (((signed)p.addr == -1) || (p.addr >= (size_t)(romlen - 1))) {
        if ((p.addr < 0xff0000) || (p.addr >= 0xffffff)) {
            printf("Bad patch \"%s\"\n", tok);
            if (errors != NULL)
                ++(*errors);
            ret = -1;
            continue;
        }
        // This is a RAM patch.
        dest = ram;
        mask = 0xffff;
      }
      if (dest == no_rom) {
          printf("Cannot patch this ROM\n");
          continue;
      }
#ifndef ROM_BYTESWAP
      if (dest == rom)
          swap = false;
#endif
      // Put it into dest (remember byteswapping)
      while (elem != NULL) {
    if (elem->addr == p.addr) {
      // Revert a previous patch.
      p.data = elem->data;
      if (prev != NULL)
        prev->next = elem->next;
      else
        patch_elem = NULL;
      free(elem);
      rev = 1;
      break;
    }
    prev = elem;
    elem = elem->next;
      }
      if (rev) {
        printf("Reverting patch \"%s\" -> %06X\n", tok, p.addr);
    if (reverted != NULL)
      ++(*reverted);
      }
      else {
    printf("Patch \"%s\" -> %06X:%04X\n", tok, p.addr, p.data);
    if (applied != NULL)
      ++(*applied);
    if ((elem = (struct patch_elem *)malloc(sizeof(*elem))) != NULL) {
      elem->next = patch_elem;
      elem->addr = p.addr;
      elem->data = ((dest[((p.addr + 0) ^ swap) & mask] << 8) |
            (dest[((p.addr + 1) ^ swap) & mask]));
      patch_elem = elem;
    }
      }
      dest[((p.addr + 0) ^ swap) & mask] = (uint8_t)(p.data >> 8);
      dest[((p.addr + 1) ^ swap) & mask] = (uint8_t)(p.data & 0xff);
    }
  // Done!
  free(worklist);
  return ret;
}

/**
 * Get saveram from FILE*.
 * @param from File to read from.
 * @return 0 on success.
 */
int md::get_save_ram(FILE *from)
{
    return !fread((void*)saveram, save_len, 1, from);
}

/**
 * Write a saveram to FILE*.
 * @param into File to write to.
 * @return 0 on success.
 */
int md::put_save_ram(FILE *into)
{
    return !fwrite((void*)saveram, save_len, 1, into);
}

/**
 * Calculates a ROM's checksum.
 * @param rom ROM memory area.
 * @param len ROM size.
 * @return Checksum.
 */
static unsigned short calculate_checksum(unsigned char *rom,int len)
{
  unsigned short checksum=0;
  int i;
  for (i=512;i<=(len-2);i+=2)
  {
    checksum+=(rom[ROM_ADDR(i+0)]<<8);
    checksum+=rom[ROM_ADDR(i+1)];
  }
  return checksum;
}

/**
 * Replace the in-memory ROM checksum with a calculated checksum.
 */
void md::fix_rom_checksum()
{
  unsigned short cs; cs=calculate_checksum(rom,romlen);
  if (romlen>=0x190) { rom[ROM_ADDR(0x18e)]=cs>>8; rom[ROM_ADDR(0x18f)]=cs&255; }
}

/**
 * This is the default ROM, used when nothing is loaded.
 */
#ifdef ROM_BYTESWAP

const uint8_t md::no_rom[] = {
    // Note: everything is byte swapped.
    "\x72\x4e" "\xff\xff" // stop #0xffff
    "\x71\x4e"            // nop
    "\x71\x4e"            // nop
    "\xf6\x60"            // bra.b 0
};

#else

const uint8_t md::no_rom[] = {
    "\x4e\x72" "\xff\xff" // stop #0xffff
    "\x4e\x71"            // nop
    "\x4e\x71"            // nop
    "\x60\xf6"            // bra.b 0
};

#endif

const size_t md::no_rom_size = sizeof(no_rom);
