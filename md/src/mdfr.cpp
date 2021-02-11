// DGen/SDL v1.29+
// Megadrive 1 Frame module
// Many, many thanks to John Stiles for the new structure of this module! :)
// And kudos to Gens (Gens/GS) and Genplus (GX) authors -- zamaz

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "md.h"
#include "rc-vars.h"

// Set and unset contexts (Musashi, StarScream, MZ80)

class md* md::md_musa(0);

bool md::md_set_musa(bool set)
{
	if (set) {
		++md_musa_ref;
		if (md_musa == this)
			return true;
		md_musa_prev = md_musa;
		md_musa = this;
		md_set_musa_sync(true);
		return false;
	}
	else {
		if (md_musa != this)
			abort();
		if (--md_musa_ref != 0)
			return true;
		md_set_musa_sync(false);
		md_musa = md_musa_prev;
		md_musa_prev = 0;
		return true;
	}
}

void md::md_set_musa_sync(bool push)
{
	unsigned int i, j;

	if (push) {
		m68k_set_context(ctx_musa);
		for (i = M68K_REG_D0, j = 0; (i <= M68K_REG_D7); ++i, ++j)
			m68k_set_reg((m68k_register_t)i,
				     le2h32(m68k_state.d[j]));
		for (i = M68K_REG_A0, j = 0; (i <= M68K_REG_A7); ++i, ++j)
			m68k_set_reg((m68k_register_t)i,
				     le2h32(m68k_state.a[j]));
		m68k_set_reg(M68K_REG_PC, le2h32(m68k_state.pc));
		m68k_set_reg(M68K_REG_SR, le2h16(m68k_state.sr));
	}
	else {
		for (i = M68K_REG_D0, j = 0; (i <= M68K_REG_D7); ++i, ++j)
			m68k_state.d[j] =
				h2le32(m68k_get_reg(NULL, (m68k_register_t)i));
		for (i = M68K_REG_A0, j = 0; (i <= M68K_REG_A7); ++i, ++j)
			m68k_state.a[j] =
				h2le32(m68k_get_reg(NULL, (m68k_register_t)i));
		m68k_state.pc = h2le32(m68k_get_reg(NULL, M68K_REG_PC));
		m68k_state.sr = h2le16(m68k_get_reg(NULL, M68K_REG_SR));
		m68k_get_context(ctx_musa);
	}
}

// Set/unset contexts
void md::md_set(bool set)
{
	if (cpu_emu == CPU_EMU_MUSA)
		md_set_musa(set);
	else
		(void)0;
}

// Return PC data.
unsigned int md::m68k_read_pc()
{
	static bool rec = false;
	unsigned int pc;

	// Forbid recursion.
	if (rec)
		return h2be16(0xdead);
	rec = true;
	if (cpu_emu == CPU_EMU_MUSA) {
		md_set_musa(1);
		pc = m68k_get_reg(NULL, M68K_REG_PC);
		md_set_musa(0);
	}
	else
		pc = 0;
	pc = misc_readword(pc & 0xffffff);
	rec = false;
	return pc;
}

// Return current M68K odometer
int md::m68k_odo()
{
	if (m68k_st_running) {
		if (cpu_emu == CPU_EMU_MUSA)
			return (odo.m68k + m68k_cycles_run());
	}
	return odo.m68k;
}

// Run M68K to odo.m68k_max
void md::m68k_run()
{
	int cycles = (odo.m68k_max - odo.m68k);

	if (cycles <= 0)
		return;
	m68k_st_running = 1;
	if (cpu_emu == CPU_EMU_MUSA)
		odo.m68k += m68k_execute(cycles);
	else
		odo.m68k += cycles;

	m68k_st_running = 0;
}

// Issue BUSREQ
void md::m68k_busreq_request()
{
	if (z80_st_busreq)
		return;
	z80_st_busreq = 1;
	if (z80_st_reset)
		return;
	z80_sync(0);
}

// Cancel BUSREQ
void md::m68k_busreq_cancel()
{
	if (!z80_st_busreq)
		return;
	z80_st_busreq = 0;
	z80_sync(1);
}

// Trigger M68K IRQ
void md::m68k_irq(int i)
{
	if (cpu_emu == CPU_EMU_MUSA)
		m68k_set_irq(i);
	else
		(void)i;
}

// Trigger M68K IRQ or disable them according to VDP status.
void md::m68k_vdp_irq_trigger()
{
	if ((vdp.vint_pending) && (vdp.reg[1] & 0x20))
		m68k_irq(6);
	else if ((vdp.hint_pending) && (vdp.reg[0] & 0x10))
		m68k_irq(4);
	else
		m68k_irq(0);
}

// Called whenever M68K acknowledges an interrupt.
void md::m68k_vdp_irq_handler()
{
	if ((vdp.vint_pending) && (vdp.reg[1] & 0x20)) {
		vdp.vint_pending = false;
		coo5 &= ~0x80;
		if ((vdp.hint_pending) && (vdp.reg[0] & 0x10))
			m68k_irq(4);
		else {
			vdp.hint_pending = false;
			m68k_irq(0);
		}
	}
	else {
		vdp.hint_pending = false;
		m68k_irq(0);
	}
}

// Return current Z80 odometer
int md::z80_odo()
{
	if (z80_st_running) {

	}
	return odo.z80;
}

// Run Z80 to odo.z80_max
void md::z80_run()
{
	int cycles = (odo.z80_max - odo.z80);

	if (cycles <= 0)
		return;
	z80_st_running = 1;

	if (z80_st_busreq | z80_st_reset)
		odo.z80 += cycles;
	else {
			odo.z80 += cycles;
	}

	z80_st_running = 0;
}

// Synchronize Z80 with M68K, don't execute code if fake is nonzero
void md::z80_sync(int fake)
{
	int cycles = (m68k_odo() >> 1);

	if (cycles > odo.z80_max)
		cycles = odo.z80_max;
	cycles -= odo.z80;
	if (cycles <= 0)
		return;
	z80_st_running = 1;

	if (fake)
		odo.z80 += cycles;
	else {
			odo.z80 += cycles;
	}

	z80_st_running = 0;
}

// Trigger Z80 IRQ
void md::z80_irq(int vector)
{
	z80_st_irq = 1;
	z80_irq_vector = vector;
		(void)0;
}

// Clear Z80 IRQ
void md::z80_irq_clear()
{
	z80_st_irq = 0;
	z80_irq_vector = 0;
		(void)0;
}

// Return the number of microseconds spent in current frame
unsigned int md::frame_usecs()
{
	if (z80_st_running)
		return ((z80_odo() * 1000) / (clk0 / 1000));
	return ((m68k_odo() * 1000) / (clk1 / 1000));
}

// Return first line of vblank
unsigned int md::vblank()
{
	return (((pal) && (vdp.reg[1] & 0x08)) ? PAL_VBLANK : NTSC_VBLANK);
}

// 6-button pad status update. Should be called once per displayed line.
void md::pad_update()
{
	// The following code was originally in DGen until at least DGen v1.21
	// (Win32 version) but wasn't in DGen/SDL v1.23, preventing 6-button
	// emulation from working at all until now (v1.31 included).
	// This broke some games (no input at all).

	// Reset 6-button pad toggle after 26? lines
	if (aoo3_six_timeout > 25)
		aoo3_six = 0;
	else
		++aoo3_six_timeout;
	if (aoo5_six_timeout > 25)
		aoo5_six = 0;
	else
		++aoo5_six_timeout;
}

// Generate one frame
int md::one_frame(struct bmap *bm, unsigned char retpal[256],
		  struct sndinfo *sndi)
{
	int hints;
	int m68k_max, z80_max;
	unsigned int vblank = md::vblank();

#ifdef WITH_DEBUG_VDP
	/*
	 * If the user is disabling planes for debugging, then we
	 * paint the screen black before blitting a new frame. This
	 * stops crap from earlier frames from junking up the display.
	 */
	if ((bm != NULL) &&
	    (dgen_vdp_hide_plane_b | dgen_vdp_hide_plane_a |
	     dgen_vdp_hide_plane_w | dgen_vdp_hide_sprites))
		memset(bm->data, 0, (bm->pitch * bm->h));
#endif
	md_set(1);
	// Reset odometers
	memset(&odo, 0, sizeof(odo));
	// Reset FM tickers
	fm_ticker[1] = 0;
	fm_ticker[3] = 0;
	// Raster zero causes special things to happen :)
	// Init status register with fifo always empty (FIXME)
	coo4 = (0x34 | 0x02); // 00110100b | 00000010b
	if (vdp.reg[12] & 0x2)
		coo5 ^= 0x10; // Toggle odd/even for interlace
	coo5 &= ~0x08; // Clear vblank
	coo5 |= !!pal;
	// Clear sprite overflow bit (d6).
	coo5 &= ~0x40;
	// Clear sprite collision bit (d5).
	coo5 &= ~0x20;
	// Is permanently set
	hints = vdp.reg[10]; // Set hint counter
	// Reset sprite overflow line
	vdp.sprite_overflow_line = INT_MIN;
	// Video display! :D
	for (ras = 0; ((unsigned int)ras < vblank); ++ras) {
		pad_update(); // Update 6-button pads
		fm_timer_callback(); // Update sound timers
		if (--hints < 0) {
			// Trigger hint
			hints = vdp.reg[10];
			vdp.hint_pending = true;
			m68k_vdp_irq_trigger();
			may_want_to_get_pic(bm, retpal, 1);
		}
		else
			may_want_to_get_pic(bm, retpal, 0);
		// Enable h-blank
		coo5 |= 0x04;
		// H-blank comes before, about 36/209 of the whole scanline
		m68k_max = (odo.m68k_max + M68K_CYCLES_PER_LINE);
		odo.m68k_max += M68K_CYCLES_HBLANK;
		z80_max = (odo.z80_max + Z80_CYCLES_PER_LINE);
		odo.z80_max += Z80_CYCLES_HBLANK;
		m68k_run();
		z80_run();
		// Disable h-blank
		coo5 &= ~0x04;
		// Do hdisplay now
		odo.m68k_max = m68k_max;
		odo.z80_max = z80_max;
		m68k_run();
		z80_run();
	}
	// Now we're in vblank, more special things happen :)
	// The following was roughly adapted from Genplus GX
	// Enable v-blank
	coo5 |= 0x08;
	if (--hints < 0) {
		// Trigger hint
		vdp.hint_pending = true;
		m68k_vdp_irq_trigger();
	}
	// Save m68k_max and z80_max
	m68k_max = (odo.m68k_max + M68K_CYCLES_PER_LINE);
	z80_max = (odo.z80_max + Z80_CYCLES_PER_LINE);
	// Delay between vint and vint flag
	odo.m68k_max += M68K_CYCLES_HBLANK;
	odo.z80_max += Z80_CYCLES_HBLANK;
	// Enable h-blank
	coo5 |= 0x04;
	m68k_run();
	z80_run();
	// Disable h-blank
	coo5 &= ~0x04;
	// Toggle vint flag
	coo5 |= 0x80;
	// Delay between v-blank and vint
	odo.m68k_max += (M68K_CYCLES_VDELAY - M68K_CYCLES_HBLANK);
	odo.z80_max += (Z80_CYCLES_VDELAY - Z80_CYCLES_HBLANK);
	m68k_run();
	z80_run();
	// Restore m68k_max and z80_max
	odo.m68k_max = m68k_max;
	odo.z80_max = z80_max;
	// Blank everything and trigger vint
	vdp.vint_pending = true;
	m68k_vdp_irq_trigger();
	if (!z80_st_reset)
		z80_irq(0);
	fm_timer_callback();
	// Run remaining cycles
	m68k_run();
	z80_run();
	++ras;
	// Run the course of vblank
	pad_update();
	fm_timer_callback();
	// Usual h-blank stuff
	coo5 |= 0x04;
	m68k_max = (odo.m68k_max + M68K_CYCLES_PER_LINE);
	odo.m68k_max += M68K_CYCLES_HBLANK;
	z80_max = (odo.z80_max + Z80_CYCLES_PER_LINE);
	odo.z80_max += Z80_CYCLES_HBLANK;
	m68k_run();
	z80_run();
	coo5 &= ~0x04;
	odo.m68k_max = m68k_max;
	odo.z80_max = z80_max;
	m68k_run();
	z80_run();
	// Clear Z80 interrupt
	if (z80_st_irq)
		z80_irq_clear();
	++ras;
	// Remaining lines
	while ((unsigned int)ras < lines) {
		pad_update();
		fm_timer_callback();
		// Enable h-blank
		coo5 |= 0x04;
		m68k_max = (odo.m68k_max + M68K_CYCLES_PER_LINE);
		odo.m68k_max += M68K_CYCLES_HBLANK;
		z80_max = (odo.z80_max + Z80_CYCLES_PER_LINE);
		odo.z80_max += Z80_CYCLES_HBLANK;
		m68k_run();
		z80_run();
		// Disable h-blank
		coo5 &= ~0x04;
		odo.m68k_max = m68k_max;
		odo.z80_max = z80_max;
		m68k_run();
		z80_run();
		++ras;
	}
	// Fill the sound buffers
	if (sndi)
		may_want_to_get_sound(sndi);
	fm_timer_callback();
	md_set(0);
	return 0;
}

// Return V counter (Gens/GS style)
uint8_t md::calculate_coo8()
{
	unsigned int id;
	unsigned int hc, vc;
	uint8_t bl, bh;

	id = m68k_odo();
	/*
	  FIXME
	  Using "(ras - 1)" instead of "ras" here seems to solve horizon
	  issues in Road Rash and Mickey Mania (Moose Chase level).
	*/
	if (ras)
		id -= ((ras - 1) * M68K_CYCLES_PER_LINE);
	id &= 0x1ff;
	if (vdp.reg[4] & 0x81) {
		hc = hc_table[id][1];
		bl = 0xa4;
	}
	else {
		hc = hc_table[id][0];
		bl = 0x84;
	}
	bh = (hc <= 0xe0);
	bl = (hc >= bl);
	bl &= bh;
	vc = ras;
	vc += (bl != 0);
	if (pal) {
		if (vc >= 0x103)
			vc -= 56;
	}
	else {
		if (vc >= 0xeb)
			vc -= 6;
	}
	return vc;
}

// Return H counter (Gens/GS style)
uint8_t md::calculate_coo9()
{
	unsigned int id;

	id = m68k_odo();
	if (ras)
		id -= ((ras - 1) * M68K_CYCLES_PER_LINE);
	id &= 0x1ff;
	if (vdp.reg[4] & 0x81)
		return hc_table[id][1];
	return hc_table[id][0];
}

// *************************************
//       May want to get pic or sound
// *************************************

inline int md::may_want_to_get_pic(struct bmap *bm,unsigned char retpal[256],int/*mark*/)
{
  if (bm==NULL) return 0;

  if (ras>=0 && (unsigned int)ras<vblank())
    vdp.draw_scanline(bm, ras);
  if(retpal && ras == 100) get_md_palette(retpal, vdp.cram);
  return 0;
}

int md::may_want_to_get_sound(struct sndinfo *sndi)
{
  extern intptr_t dgen_volume;
  unsigned int i, len = sndi->len;

  // Get the PSG
  SN76496Update_16_2(0, sndi->lr, len);

	if (dac_len) {
		unsigned int ratio = ((sndi->len << 10) / elemof(dac_data));

		// Stretch the DAC to fit the real length.
		for (i = 0; (i != sndi->len); ++i) {
			unsigned int index = ((i << 10) / ratio);
			uint16_t data;

			if (index >= dac_len)
				data = dac_data[dac_len - 1];
			else
				data = dac_data[index];
			data = ((data - 0x80) << 6);
			sndi->lr[i << 1] += data;
			sndi->lr[(i << 1) ^ 1] += data;
		}
		// Clear the DAC for next frame.
		dac_len = 0;
#ifndef NDEBUG
		memset(dac_data, 0xff, sizeof(dac_data));
#endif
	}

  // Add in the stereo FM buffer
  YM2612UpdateOne(0, sndi->lr, len, dgen_volume, 1);
  if (dgen_mjazz) {
    YM2612UpdateOne(1, sndi->lr, len, dgen_volume, 0);
    YM2612UpdateOne(2, sndi->lr, len, dgen_volume, 0);
  }
  return 0;
}

