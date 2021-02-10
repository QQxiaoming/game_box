#include "m68kcpu.h"

#include "m68kops.h"

/* ======================================================================== */
/* ======================= INSTRUCTION HANDLERS A-C ======================= */
/* ======================================================================== */
/* Instruction handler function names follow this convention:
 *
 * m68000_NAME_EXTENSIONS(void)
 * where NAME is the name of the opcode it handles and EXTENSIONS are any
 * extensions for special instances of that opcode.
 *
 * Examples:
 *   m68000_add_er_ai_8(): add opcode, from effective address to register,
 *                         using address register indirect, size = byte
 *
 *   m68000_asr_s_8(): arithmetic shift right, static count, size = byte
 *
 *
 * Note: move uses the form m68000_move_DST_SRC_SIZE
 *
 * Common extensions:
 * 8   : size = byte
 * 16  : size = word
 * 32  : size = long
 * rr  : register to register
 * mm  : memory to memory
 * a7  : using a7 register
 * ax7 : using a7 in X part of instruction (....XXX......YYY)
 * ay7 : using a7 in Y part of instruction (....XXX......YYY)
 * axy7: using a7 in both parts of instruction (....XXX......YYY)
 * r   : register
 * s   : static
 * er  : effective address -> register
 * re  : register -> effective address
 * ea  : using effective address mode of operation
 * d   : data register direct
 * a   : address register direct
 * ai  : address register indirect
 * pi  : address register indirect with postincrement
 * pi7 : address register 7 indirect with postincrement
 * pd  : address register indirect with predecrement
 * pd7 : address register 7 indirect with predecrement
 * di  : address register indirect with displacement
 * ix  : address register indirect with index
 * aw  : absolute word
 * al  : absolute long
 * pcdi: program counter with displacement
 * pcix: program counter with index
 */


void m68000_1010(void)
{
	m68ki_exception(EXCEPTION_1010);
	M68K_DO_LOG_EMU((M68K_LOG, "%s at %08x: called 1010 instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
}


void m68000_1111(void)
{
	m68ki_exception(EXCEPTION_1111);
	M68K_DO_LOG_EMU((M68K_LOG, "%s at %08x: called 1111 instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
}


void m68000_abcd_rr(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (CPU_X != 0);

	if (res > 9)
		res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res -= 0xa0;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | MASK_OUT_ABOVE_8(res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(6);
}


void m68000_abcd_mm_ax7(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea = CPU_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (CPU_X != 0);

	if (res > 9)
		res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res -= 0xa0;

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(18);
}


void m68000_abcd_mm_ay7(void)
{
	uint src = m68ki_read_8(CPU_A[7] -= 2);
	uint ea = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (CPU_X != 0);

	if (res > 9)
		res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res -= 0xa0;

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(18);
}


void m68000_abcd_mm_axy7(void)
{
	uint src = m68ki_read_8(CPU_A[7] -= 2);
	uint ea = CPU_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (CPU_X != 0);

	if (res > 9)
		res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res -= 0xa0;

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(18);
}


void m68000_abcd_mm(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (CPU_X != 0);

	if (res > 9)
		res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res -= 0xa0;

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(18);
}


void m68000_add_er_d_8(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4);
}


void m68000_add_er_ai_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_AI);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_add_er_pi_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PI_8);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_add_er_pi7_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PI7_8);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_add_er_pd_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PD_8);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+6);
}


void m68000_add_er_pd7_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PD7_8);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+6);
}


void m68000_add_er_di_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_DI);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_add_er_ix_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_IX);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_add_er_aw_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_AW);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_add_er_al_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_AL);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+12);
}


void m68000_add_er_pcdi_8(void)
{
	uint *d_dst = &DX;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_8(ea);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_add_er_pcix_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PCIX);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_add_er_i_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_imm_8();
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_add_er_d_16(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_add_er_a_16(void)
{
	uint *d_dst = &DX;
	uint src = AY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_add_er_ai_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_AI);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_add_er_pi_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_PI_16);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_add_er_pd_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_PD_16);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+6);
}


void m68000_add_er_di_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_DI);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_add_er_ix_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_IX);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_add_er_aw_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_AW);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_add_er_al_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_AL);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+12);
}


void m68000_add_er_pcdi_16(void)
{
	uint *d_dst = &DX;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_16(ea);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_add_er_pcix_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_PCIX);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_add_er_i_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_imm_16();
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_add_er_d_32(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(8);
}


void m68000_add_er_a_32(void)
{
	uint *d_dst = &DX;
	uint src = AY;
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(8);
}


void m68000_add_er_ai_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_AI);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_add_er_pi_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_PI_32);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_add_er_pd_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_PD_32);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+10);
}


void m68000_add_er_di_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_DI);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_add_er_ix_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_IX);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+14);
}


void m68000_add_er_aw_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_AW);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_add_er_al_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_AL);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+16);
}


void m68000_add_er_pcdi_32(void)
{
	uint *d_dst = &DX;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_32(ea);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_add_er_pcix_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_PCIX);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+14);
}


void m68000_add_er_i_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_imm_32();
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+10);
}


void m68000_add_re_ai_8(void)
{
	uint ea = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_add_re_pi_8(void)
{
	uint ea = EA_PI_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_add_re_pi7_8(void)
{
	uint ea = EA_PI7_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_add_re_pd_8(void)
{
	uint ea = EA_PD_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_add_re_pd7_8(void)
{
	uint ea = EA_PD7_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_add_re_di_8(void)
{
	uint ea = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_add_re_ix_8(void)
{
	uint ea = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_add_re_aw_8(void)
{
	uint ea = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_add_re_al_8(void)
{
	uint ea = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+12);
}


void m68000_add_re_ai_16(void)
{
	uint ea = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_add_re_pi_16(void)
{
	uint ea = EA_PI_16;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_add_re_pd_16(void)
{
	uint ea = EA_PD_16;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_add_re_di_16(void)
{
	uint ea = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_add_re_ix_16(void)
{
	uint ea = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_add_re_aw_16(void)
{
	uint ea = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_add_re_al_16(void)
{
	uint ea = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+12);
}


void m68000_add_re_ai_32(void)
{
	uint ea = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_add_re_pi_32(void)
{
	uint ea = EA_PI_32;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_add_re_pd_32(void)
{
	uint ea = EA_PD_32;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+10);
}


void m68000_add_re_di_32(void)
{
	uint ea = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_add_re_ix_32(void)
{
	uint ea = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+14);
}


void m68000_add_re_aw_32(void)
{
	uint ea = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_add_re_al_32(void)
{
	uint ea = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+16);
}


void m68000_adda_d_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(DY));
	USE_CLKS(8);
}


void m68000_adda_a_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(AY));
	USE_CLKS(8);
}


void m68000_adda_ai_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_AI)));
	USE_CLKS(8+4);
}


void m68000_adda_pi_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_PI_16)));
	USE_CLKS(8+4);
}


void m68000_adda_pd_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_PD_16)));
	USE_CLKS(8+6);
}


void m68000_adda_di_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_DI)));
	USE_CLKS(8+8);
}


void m68000_adda_ix_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_IX)));
	USE_CLKS(8+10);
}


void m68000_adda_aw_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_AW)));
	USE_CLKS(8+8);
}


void m68000_adda_al_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_AL)));
	USE_CLKS(8+12);
}


void m68000_adda_pcdi_16(void)
{
	uint *a_dst = &AX;

	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(ea)));
	USE_CLKS(8+8);
}


void m68000_adda_pcix_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_PCIX)));
	USE_CLKS(8+10);
}


void m68000_adda_i_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_imm_16()));
	USE_CLKS(8+4);
}


void m68000_adda_d_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + DY);
	USE_CLKS(8);
}


void m68000_adda_a_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + AY);
	USE_CLKS(8);
}


void m68000_adda_ai_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_AI));
	USE_CLKS(6+8);
}


void m68000_adda_pi_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_PI_32));
	USE_CLKS(6+8);
}


void m68000_adda_pd_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_PD_32));
	USE_CLKS(6+10);
}


void m68000_adda_di_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_DI));
	USE_CLKS(6+12);
}


void m68000_adda_ix_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_IX));
	USE_CLKS(6+14);
}


void m68000_adda_aw_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_AW));
	USE_CLKS(6+12);
}


void m68000_adda_al_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_AL));
	USE_CLKS(6+16);
}


void m68000_adda_pcdi_32(void)
{
	uint *a_dst = &AX;

	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(ea));
	USE_CLKS(6+12);
}


void m68000_adda_pcix_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_PCIX));
	USE_CLKS(6+14);
}


void m68000_adda_i_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_imm_32());
	USE_CLKS(6+10);
}


void m68000_addi_d_8(void)
{
	uint *d_dst = &DY;
	uint src = m68ki_read_imm_8();
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8);
}


void m68000_addi_ai_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_addi_pi_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_addi_pi7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_addi_pd_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+6);
}


void m68000_addi_pd7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+6);
}


void m68000_addi_di_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_addi_ix_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+10);
}


void m68000_addi_aw_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_addi_al_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_addi_d_16(void)
{
	uint *d_dst = &DY;
	uint src = m68ki_read_imm_16();
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8);
}


void m68000_addi_ai_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_addi_pi_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_addi_pd_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+6);
}


void m68000_addi_di_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_addi_ix_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+10);
}


void m68000_addi_aw_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_addi_al_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_addi_d_32(void)
{
	uint *d_dst = &DY;
	uint src = m68ki_read_imm_32();
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(16);
}


void m68000_addi_ai_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+8);
}


void m68000_addi_pi_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+8);
}


void m68000_addi_pd_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+10);
}


void m68000_addi_di_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+12);
}


void m68000_addi_ix_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+14);
}


void m68000_addi_aw_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+12);
}


void m68000_addi_al_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+16);
}


void m68000_addq_d_8(void)
{
	uint *d_dst = &DY;
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4);
}


void m68000_addq_ai_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_addq_pi_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_addq_pi7_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_addq_pd_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_addq_pd7_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_addq_di_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_addq_ix_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_addq_aw_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_addq_al_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+12);
}


void m68000_addq_d_16(void)
{
	uint *d_dst = &DY;
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_addq_a_16(void)
{
	uint *a_dst = &AY;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + (((CPU_IR >> 9) - 1) & 7) + 1);
	USE_CLKS(4);
}


void m68000_addq_ai_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_addq_pi_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_addq_pd_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_addq_di_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_addq_ix_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_addq_aw_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_addq_al_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+12);
}


void m68000_addq_d_32(void)
{
	uint *d_dst = &DY;
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(8);
}


void m68000_addq_a_32(void)
{
	uint *a_dst = &AY;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + (((CPU_IR >> 9) - 1) & 7) + 1);
	USE_CLKS(8);
}


void m68000_addq_ai_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_addq_pi_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_addq_pd_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+10);
}


void m68000_addq_di_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_addq_ix_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+14);
}


void m68000_addq_aw_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_addq_al_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+16);
}


void m68000_addx_rr_8(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst + (CPU_X != 0));

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4);
}


void m68000_addx_rr_16(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst + (CPU_X != 0));

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_addx_rr_32(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(src + dst + (CPU_X != 0));

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(8);
}


void m68000_addx_mm_8_ax7(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea = CPU_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst + (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(18);
}


void m68000_addx_mm_8_ay7(void)
{
	uint src = m68ki_read_8(CPU_A[7] -= 2);
	uint ea = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst + (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(18);
}


void m68000_addx_mm_8_axy7(void)
{
	uint src = m68ki_read_8(CPU_A[7] -= 2);
	uint ea = CPU_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst + (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(18);
}


void m68000_addx_mm_8(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst + (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = VFLAG_ADD_8(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(18);
}


void m68000_addx_mm_16(void)
{
	uint src = m68ki_read_16(AY -= 2);
	uint ea = (AX -= 2);
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst + (CPU_X != 0));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = VFLAG_ADD_16(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(18);
}


void m68000_addx_mm_32(void)
{
	uint src = m68ki_read_32(AY -= 4);
	uint ea = (AX -= 4);
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst + (CPU_X != 0));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = VFLAG_ADD_32(src, dst, res);
	CPU_X = CPU_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(30);
}


void m68000_and_er_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (DY | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4);
}


void m68000_and_er_ai_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_AI) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_and_er_pi_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PI_8) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_and_er_pi7_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PI7_8) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_and_er_pd_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PD_8) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+6);
}


void m68000_and_er_pd7_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PD7_8) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+6);
}


void m68000_and_er_di_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_DI) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_and_er_ix_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_IX) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+10);
}


void m68000_and_er_aw_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_AW) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_and_er_al_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_AL) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+12);
}


void m68000_and_er_pcdi_8(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(ea) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_and_er_pcix_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PCIX) | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+10);
}


void m68000_and_er_i_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_imm_8() | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_and_er_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (DY | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4);
}


void m68000_and_er_ai_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_AI) | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_and_er_pi_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_PI_16) | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_and_er_pd_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_PD_16) | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+6);
}


void m68000_and_er_di_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_DI) | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_and_er_ix_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_IX) | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+10);
}


void m68000_and_er_aw_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_AW) | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_and_er_al_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_AL) | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+12);
}


void m68000_and_er_pcdi_16(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(ea) | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_and_er_pcix_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_PCIX) | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+10);
}


void m68000_and_er_i_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_imm_16() | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_and_er_d_32(void)
{
	uint res = DX &= DY;

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8);
}


void m68000_and_er_ai_32(void)
{
	uint res = DX &= m68ki_read_32(EA_AI);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+8);
}


void m68000_and_er_pi_32(void)
{
	uint res = DX &= m68ki_read_32(EA_PI_32);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+8);
}


void m68000_and_er_pd_32(void)
{
	uint res = DX &= m68ki_read_32(EA_PD_32);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+10);
}


void m68000_and_er_di_32(void)
{
	uint res = DX &= m68ki_read_32(EA_DI);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+12);
}


void m68000_and_er_ix_32(void)
{
	uint res = DX &= m68ki_read_32(EA_IX);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+14);
}


void m68000_and_er_aw_32(void)
{
	uint res = DX &= m68ki_read_32(EA_AW);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+12);
}


void m68000_and_er_al_32(void)
{
	uint res = DX &= m68ki_read_32(EA_AL);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+16);
}


void m68000_and_er_pcdi_32(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = DX &= m68ki_read_32(ea);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+12);
}


void m68000_and_er_pcix_32(void)
{
	uint res = DX &= m68ki_read_32(EA_PCIX);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+14);
}


void m68000_and_er_i_32(void)
{
	uint res = DX &= m68ki_read_imm_32();

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+10);
}


void m68000_and_re_ai_8(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_and_re_pi_8(void)
{
	uint ea = EA_PI_8;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_and_re_pi7_8(void)
{
	uint ea = EA_PI7_8;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_and_re_pd_8(void)
{
	uint ea = EA_PD_8;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_and_re_pd7_8(void)
{
	uint ea = EA_PD7_8;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_and_re_di_8(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_and_re_ix_8(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_and_re_aw_8(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_and_re_al_8(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+12);
}


void m68000_and_re_ai_16(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_and_re_pi_16(void)
{
	uint ea = EA_PI_16;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_and_re_pd_16(void)
{
	uint ea = EA_PD_16;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_and_re_di_16(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_and_re_ix_16(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_and_re_aw_16(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_and_re_al_16(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+12);
}


void m68000_and_re_ai_32(void)
{
	uint ea = EA_AI;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_and_re_pi_32(void)
{
	uint ea = EA_PI_32;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_and_re_pd_32(void)
{
	uint ea = EA_PD_32;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+10);
}


void m68000_and_re_di_32(void)
{
	uint ea = EA_DI;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_and_re_ix_32(void)
{
	uint ea = EA_IX;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+14);
}


void m68000_and_re_aw_32(void)
{
	uint ea = EA_AW;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_and_re_al_32(void)
{
	uint ea = EA_AL;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+16);
}


void m68000_andi_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY &= (m68ki_read_imm_8() | 0xffffff00));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8);
}


void m68000_andi_ai_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_AI;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_andi_pi_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PI_8;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_andi_pi7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PI7_8;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_andi_pd_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PD_8;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+6);
}


void m68000_andi_pd7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PD7_8;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+6);
}


void m68000_andi_di_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_DI;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_andi_ix_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_IX;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+10);
}


void m68000_andi_aw_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_AW;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_andi_al_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_AL;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_andi_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY &= (m68ki_read_imm_16() | 0xffff0000));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8);
}


void m68000_andi_ai_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_AI;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_andi_pi_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_PI_16;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_andi_pd_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_PD_16;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+6);
}


void m68000_andi_di_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_DI;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_andi_ix_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_IX;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+10);
}


void m68000_andi_aw_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_AW;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_andi_al_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_AL;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_andi_d_32(void)
{
	uint res = DY &= (m68ki_read_imm_32());

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(14);
}


void m68000_andi_ai_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_AI;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+8);
}


void m68000_andi_pi_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_PI_32;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+8);
}


void m68000_andi_pd_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_PD_32;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+10);
}


void m68000_andi_di_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_DI;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+12);
}


void m68000_andi_ix_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_IX;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+14);
}


void m68000_andi_aw_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_AW;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+12);
}


void m68000_andi_al_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_AL;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+16);
}


void m68000_andi_to_ccr(void)
{
	m68ki_set_ccr(m68ki_get_ccr() & m68ki_read_imm_16());
	USE_CLKS(20);
}


void m68000_andi_to_sr(void)
{
	uint and_val = m68ki_read_imm_16();

	if (CPU_S)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(m68ki_get_sr() & and_val);
		USE_CLKS(20);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


void m68000_asr_s_8(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = src >> shift;

	if (GET_MSB_8(src))
		res |= m68k_shift_8_table[shift];

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_X = CPU_C = shift > 7 ? CPU_N : (src >> (shift - 1)) & 1;
	USE_CLKS((shift << 1) + 6);
}


void m68000_asr_s_16(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = src >> shift;

	if (GET_MSB_16(src))
		res |= m68k_shift_16_table[shift];

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_X = CPU_C = (src >> (shift - 1)) & 1;
	USE_CLKS((shift << 1) + 6);
}


void m68000_asr_s_32(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = src >> shift;

	if (GET_MSB_32(src))
		res |= m68k_shift_32_table[shift];

	*d_dst = res;

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_X = CPU_C = (src >> (shift - 1)) & 1;
	USE_CLKS((shift << 1) + 8);
}


void m68000_asr_r_8(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = src >> shift;

	USE_CLKS((shift << 1) + 6);
	if (shift != 0)
	{
		if (shift < 8)
		{
			if (GET_MSB_8(src))
				res |= m68k_shift_8_table[shift];

			*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

			CPU_C = CPU_X = (src >> (shift - 1)) & 1;
			CPU_N = GET_MSB_8(res);
			CPU_NOT_Z = res;
			CPU_V = 0;
			return;
		}

		if (GET_MSB_8(src))
		{
			*d_dst |= 0xff;
			CPU_C = CPU_X = 1;
			CPU_N = 1;
			CPU_NOT_Z = 1;
			CPU_V = 0;
			return;
		}

		*d_dst &= 0xffffff00;
		CPU_C = CPU_X = 0;
		CPU_N = 0;
		CPU_NOT_Z = 0;
		CPU_V = 0;
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_asr_r_16(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = src >> shift;

	USE_CLKS((shift << 1) + 6);
	if (shift != 0)
	{
		if (shift < 16)
		{
			if (GET_MSB_16(src))
				res |= m68k_shift_16_table[shift];

			*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

			CPU_C = CPU_X = (src >> (shift - 1)) & 1;
			CPU_N = GET_MSB_16(res);
			CPU_NOT_Z = res;
			CPU_V = 0;
			return;
		}

		if (GET_MSB_16(src))
		{
			*d_dst |= 0xffff;
			CPU_C = CPU_X = 1;
			CPU_N = 1;
			CPU_NOT_Z = 1;
			CPU_V = 0;
			return;
		}

		*d_dst &= 0xffff0000;
		CPU_C = CPU_X = 0;
		CPU_N = 0;
		CPU_NOT_Z = 0;
		CPU_V = 0;
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_asr_r_32(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = src >> shift;

	USE_CLKS((shift << 1) + 8);
	if (shift != 0)
	{
		if (shift < 32)
		{
			if (GET_MSB_32(src))
				res |= m68k_shift_32_table[shift];

			*d_dst = res;

			CPU_C = CPU_X = (src >> (shift - 1)) & 1;
			CPU_N = GET_MSB_32(res);
			CPU_NOT_Z = res;
			CPU_V = 0;
			return;
		}

		if (GET_MSB_32(src))
		{
			*d_dst = 0xffffffff;
			CPU_C = CPU_X = 1;
			CPU_N = 1;
			CPU_NOT_Z = 1;
			CPU_V = 0;
			return;
		}

		*d_dst = 0;
		CPU_C = CPU_X = 0;
		CPU_N = 0;
		CPU_NOT_Z = 0;
		CPU_V = 0;
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_asr_ea_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_C = CPU_X = src & 1;
	USE_CLKS(8+4);
}


void m68000_asr_ea_pi(void)
{
	uint ea = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_C = CPU_X = src & 1;
	USE_CLKS(8+4);
}


void m68000_asr_ea_pd(void)
{
	uint ea = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_C = CPU_X = src & 1;
	USE_CLKS(8+6);
}


void m68000_asr_ea_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_C = CPU_X = src & 1;
	USE_CLKS(8+8);
}


void m68000_asr_ea_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_C = CPU_X = src & 1;
	USE_CLKS(8+10);
}


void m68000_asr_ea_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_C = CPU_X = src & 1;
	USE_CLKS(8+8);
}


void m68000_asr_ea_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
	CPU_C = CPU_X = src & 1;
	USE_CLKS(8+12);
}


void m68000_asl_s_8(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = MASK_OUT_ABOVE_8(src << shift);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_X = CPU_C = (src >> (8 - shift)) & 1;
	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	src &= m68k_shift_8_table[shift + 1];
	CPU_V = !(src == 0 || (src == m68k_shift_8_table[shift + 1] && shift < 8));

	USE_CLKS((shift << 1) + 6);
}


void m68000_asl_s_16(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = MASK_OUT_ABOVE_16(src << shift);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = (src >> (16 - shift)) & 1;
	src &= m68k_shift_16_table[shift + 1];
	CPU_V = !(src == 0 || src == m68k_shift_16_table[shift + 1]);

	USE_CLKS((shift << 1) + 6);
}


void m68000_asl_s_32(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = *d_dst;
	uint res = MASK_OUT_ABOVE_32(src << shift);

	*d_dst = res;

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = (src >> (32 - shift)) & 1;
	src &= m68k_shift_32_table[shift + 1];
	CPU_V = !(src == 0 || src == m68k_shift_32_table[shift + 1]);

	USE_CLKS((shift << 1) + 8);
}


void m68000_asl_r_8(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = MASK_OUT_ABOVE_8(src << shift);

	USE_CLKS((shift << 1) + 6);
	if (shift != 0)
	{
		if (shift < 8)
		{
			*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;
			CPU_X = CPU_C = (src >> (8 - shift)) & 1;
			CPU_N = GET_MSB_8(res);
			CPU_NOT_Z = res;
			src &= m68k_shift_8_table[shift + 1];
			CPU_V = !(src == 0 || src == m68k_shift_8_table[shift + 1]);
			return;
		}

		*d_dst &= 0xffffff00;
		CPU_X = CPU_C = (shift == 8 ? src & 1 : 0);
		CPU_N = 0;
		CPU_NOT_Z = 0;
		CPU_V = !(src == 0);
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_asl_r_16(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = MASK_OUT_ABOVE_16(src << shift);

	USE_CLKS((shift << 1) + 6);
	if (shift != 0)
	{
		if (shift < 16)
		{
			*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;
			CPU_X = CPU_C = (src >> (16 - shift)) & 1;
			CPU_N = GET_MSB_16(res);
			CPU_NOT_Z = res;
			src &= m68k_shift_16_table[shift + 1];
			CPU_V = !(src == 0 || src == m68k_shift_16_table[shift + 1]);
			return;
		}

		*d_dst &= 0xffff0000;
		CPU_X = CPU_C = (shift == 16 ? src & 1 : 0);
		CPU_N = 0;
		CPU_NOT_Z = 0;
		CPU_V = !(src == 0);
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_asl_r_32(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = *d_dst;
	uint res = MASK_OUT_ABOVE_32(src << shift);

	USE_CLKS((shift << 1) + 8);
	if (shift != 0)
	{
		if (shift < 32)
		{
			*d_dst = res;
			CPU_X = CPU_C = (src >> (32 - shift)) & 1;
			CPU_N = GET_MSB_32(res);
			CPU_NOT_Z = res;
			src &= m68k_shift_32_table[shift + 1];
			CPU_V = !(src == 0 || src == m68k_shift_32_table[shift + 1]);
			return;
		}

		*d_dst = 0;
		CPU_X = CPU_C = (shift == 32 ? src & 1 : 0);
		CPU_N = 0;
		CPU_NOT_Z = 0;
		CPU_V = !(src == 0);
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_asl_ea_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = GET_MSB_16(src);
	src &= 0xc000;
	CPU_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+4);
}


void m68000_asl_ea_pi(void)
{
	uint ea = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = GET_MSB_16(src);
	src &= 0xc000;
	CPU_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+4);
}


void m68000_asl_ea_pd(void)
{
	uint ea = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = GET_MSB_16(src);
	src &= 0xc000;
	CPU_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+6);
}


void m68000_asl_ea_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = GET_MSB_16(src);
	src &= 0xc000;
	CPU_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+8);
}


void m68000_asl_ea_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = GET_MSB_16(src);
	src &= 0xc000;
	CPU_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+10);
}


void m68000_asl_ea_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = GET_MSB_16(src);
	src &= 0xc000;
	CPU_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+8);
}


void m68000_asl_ea_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = GET_MSB_16(src);
	src &= 0xc000;
	CPU_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+12);
}


void m68000_bhi_8(void)
{
	if (CONDITION_HI)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bhi_16(void)
{
	if (CONDITION_HI)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bhi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_HI)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bls_8(void)
{
	if (CONDITION_LS)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bls_16(void)
{
	if (CONDITION_LS)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bls_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LS)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bcc_8(void)
{
	if (CONDITION_CC)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bcc_16(void)
{
	if (CONDITION_CC)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bcc_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_CC)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bcs_8(void)
{
	if (CONDITION_CS)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bcs_16(void)
{
	if (CONDITION_CS)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bcs_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_CS)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bne_8(void)
{
	if (CONDITION_NE)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bne_16(void)
{
	if (CONDITION_NE)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bne_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_NE)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_beq_8(void)
{
	if (CONDITION_EQ)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_beq_16(void)
{
	if (CONDITION_EQ)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_beq_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_EQ)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bvc_8(void)
{
	if (CONDITION_VC)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bvc_16(void)
{
	if (CONDITION_VC)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bvc_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_VC)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bvs_8(void)
{
	if (CONDITION_VS)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bvs_16(void)
{
	if (CONDITION_VS)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bvs_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_VS)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bpl_8(void)
{
	if (CONDITION_PL)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bpl_16(void)
{
	if (CONDITION_PL)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bpl_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_PL)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bmi_8(void)
{
	if (CONDITION_MI)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bmi_16(void)
{
	if (CONDITION_MI)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bmi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_MI)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bge_8(void)
{
	if (CONDITION_GE)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bge_16(void)
{
	if (CONDITION_GE)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bge_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_GE)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_blt_8(void)
{
	if (CONDITION_LT)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_blt_16(void)
{
	if (CONDITION_LT)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_blt_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LT)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bgt_8(void)
{
	if (CONDITION_GT)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_bgt_16(void)
{
	if (CONDITION_GT)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_bgt_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_GT)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_ble_8(void)
{
	if (CONDITION_LE)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}


void m68000_ble_16(void)
{
	if (CONDITION_LE)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(10);
		return;
	}
	CPU_PC += 2;
	USE_CLKS(12);
}


void m68020_ble_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LE)
		{
			m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_32(CPU_PC));
			USE_CLKS(6);
			return;
		}
		CPU_PC += 2;
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bchg_r_d(void)
{
	uint *d_dst = &DY;
	uint mask = 1 << (DX & 0x1f);

	CPU_NOT_Z = *d_dst & mask;
	*d_dst ^= mask;
	USE_CLKS(8);
}


void m68000_bchg_r_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+4);
}


void m68000_bchg_r_pi(void)
{
	uint ea = EA_PI_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+4);
}


void m68000_bchg_r_pi7(void)
{
	uint ea = EA_PI7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+4);
}


void m68000_bchg_r_pd(void)
{
	uint ea = EA_PD_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+6);
}


void m68000_bchg_r_pd7(void)
{
	uint ea = EA_PD7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+6);
}


void m68000_bchg_r_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+8);
}


void m68000_bchg_r_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+10);
}


void m68000_bchg_r_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+8);
}


void m68000_bchg_r_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+12);
}


void m68000_bchg_s_d(void)
{
	uint *d_dst = &DY;
	uint mask = 1 << (m68ki_read_imm_8() & 0x1f);

	CPU_NOT_Z = *d_dst & mask;
	*d_dst ^= mask;
	USE_CLKS(12);
}


void m68000_bchg_s_ai(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_AI;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+4);
}


void m68000_bchg_s_pi(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PI_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+4);
}


void m68000_bchg_s_pi7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PI7_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+4);
}


void m68000_bchg_s_pd(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PD_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+6);
}


void m68000_bchg_s_pd7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PD7_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+6);
}


void m68000_bchg_s_di(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_DI;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+8);
}


void m68000_bchg_s_ix(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_IX;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+10);
}


void m68000_bchg_s_aw(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_AW;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+8);
}


void m68000_bchg_s_al(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_AL;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+12);
}


void m68000_bclr_r_d(void)
{
	uint *d_dst = &DY;
	uint mask = 1 << (DX & 0x1f);

	CPU_NOT_Z = *d_dst & mask;
	*d_dst &= ~mask;
	USE_CLKS(10);
}


void m68000_bclr_r_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+4);
}


void m68000_bclr_r_pi(void)
{
	uint ea = EA_PI_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+4);
}


void m68000_bclr_r_pi7(void)
{
	uint ea = EA_PI7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+4);
}


void m68000_bclr_r_pd(void)
{
	uint ea = EA_PD_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+6);
}


void m68000_bclr_r_pd7(void)
{
	uint ea = EA_PD7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+6);
}


void m68000_bclr_r_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+8);
}


void m68000_bclr_r_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+10);
}


void m68000_bclr_r_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+8);
}


void m68000_bclr_r_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+12);
}


void m68000_bclr_s_d(void)
{
	uint *d_dst = &DY;
	uint mask = 1 << (m68ki_read_imm_8() & 0x1f);

	CPU_NOT_Z = *d_dst & mask;
	*d_dst &= ~mask;
	USE_CLKS(14);
}


void m68000_bclr_s_ai(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_AI;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+4);
}


void m68000_bclr_s_pi(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PI_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+4);
}


void m68000_bclr_s_pi7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PI7_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+4);
}


void m68000_bclr_s_pd(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PD_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+6);
}


void m68000_bclr_s_pd7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PD7_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+6);
}


void m68000_bclr_s_di(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_DI;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+8);
}


void m68000_bclr_s_ix(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_IX;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+10);
}


void m68000_bclr_s_aw(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_AW;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+8);
}


void m68000_bclr_s_al(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_AL;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+12);
}


void m68020_bfchg_d(void)
{
	uint word2 = m68ki_read_imm_16();
	uint offset = (BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : word2 >> 6) & 31;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ROL_32(DY, offset) >> (32 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	DY ^= mask;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(12);
}


void m68020_bfchg_ai(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) ^ (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) ^ (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+8);
}


void m68020_bfchg_di(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_DI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) ^ (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) ^ (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+12);
}


void m68020_bfchg_ix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_IX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) ^ (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) ^ (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+14);
}


void m68020_bfchg_aw(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AW + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) ^ (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) ^ (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+12);
}


void m68020_bfchg_al(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AL + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) ^ (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) ^ (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+16);
}


void m68020_bfclr_d(void)
{
	uint word2 = m68ki_read_imm_16();
	uint offset = (BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : word2 >> 6) & 31;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ROL_32(DY, offset) >> (32 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	DY &= ~mask;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(12);
}


void m68020_bfclr_ai(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+8);
}


void m68020_bfclr_di(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_DI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+12);
}


void m68020_bfclr_ix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_IX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+14);
}


void m68020_bfclr_aw(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AW + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+12);
}


void m68020_bfclr_al(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AL + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+16);
}


void m68020_bfexts_d(void)
{
	uint word2 = m68ki_read_imm_16();
	uint offset = (BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : word2 >> 6) & 31;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ROL_32(DY, offset) >> (32 - width);

	if ((CPU_N = (data >> (width - 1)) & 1))
		data |= MASK_OUT_ABOVE_32(0xffffffff << width);
	CPU_NOT_Z = data;

	CPU_D[(word2 >> 12) & 7] = data;
	USE_CLKS(8);
}


void m68020_bfexts_ai(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	if ((CPU_N = (data >> (width - 1)) & 1))
		data |= MASK_OUT_ABOVE_32(0xffffffff << width);
	CPU_NOT_Z = data;

	CPU_D[(word2 >> 12) & 7] = data;
	USE_CLKS(15+8);
}


void m68020_bfexts_di(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_DI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	if ((CPU_N = (data >> (width - 1)) & 1))
		data |= MASK_OUT_ABOVE_32(0xffffffff << width);
	CPU_NOT_Z = data;

	CPU_D[(word2 >> 12) & 7] = data;
	USE_CLKS(15+12);
}


void m68020_bfexts_ix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_IX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	if ((CPU_N = (data >> (width - 1)) & 1))
		data |= MASK_OUT_ABOVE_32(0xffffffff << width);
	CPU_NOT_Z = data;

	CPU_D[(word2 >> 12) & 7] = data;
	USE_CLKS(15+14);
}


void m68020_bfexts_aw(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AW + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	if ((CPU_N = (data >> (width - 1)) & 1))
		data |= MASK_OUT_ABOVE_32(0xffffffff << width);
	CPU_NOT_Z = data;

	CPU_D[(word2 >> 12) & 7] = data;
	USE_CLKS(15+12);
}


void m68020_bfexts_al(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AL + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	if ((CPU_N = (data >> (width - 1)) & 1))
		data |= MASK_OUT_ABOVE_32(0xffffffff << width);
	CPU_NOT_Z = data;

	CPU_D[(word2 >> 12) & 7] = data;
	USE_CLKS(15+16);
}


void m68020_bfexts_pcdi(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint base = ea + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	if ((CPU_N = (data >> (width - 1)) & 1))
		data |= MASK_OUT_ABOVE_32(0xffffffff << width);
	CPU_NOT_Z = data;

	CPU_D[(word2 >> 12) & 7] = data;
	USE_CLKS(15+12);
}


void m68020_bfexts_pcix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_PCIX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	if ((CPU_N = (data >> (width - 1)) & 1))
		data |= MASK_OUT_ABOVE_32(0xffffffff << width);
	CPU_NOT_Z = data;

	CPU_D[(word2 >> 12) & 7] = data;
	USE_CLKS(15+14);
}


void m68020_bfextu_d(void)
{
	uint word2 = m68ki_read_imm_16();
	uint offset = (BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : word2 >> 6) & 31;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ROL_32(DY, offset) >> (32 - width);

	CPU_D[(word2 >> 12) & 7] = data;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(8);
}


void m68020_bfextu_ai(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_D[(word2 >> 12) & 7] = data;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(15+8);
}


void m68020_bfextu_di(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_DI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_D[(word2 >> 12) & 7] = data;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(15+12);
}


void m68020_bfextu_ix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_IX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_D[(word2 >> 12) & 7] = data;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(15+14);
}


void m68020_bfextu_aw(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AW + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_D[(word2 >> 12) & 7] = data;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(15+12);
}


void m68020_bfextu_al(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AL + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_D[(word2 >> 12) & 7] = data;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(15+16);
}


void m68020_bfextu_pcdi(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint base = ea + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_D[(word2 >> 12) & 7] = data;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(15+12);
}


void m68020_bfextu_pcix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_PCIX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_D[(word2 >> 12) & 7] = data;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(15+14);
}


void m68020_bfffo_d(void)
{
	uint word2 = m68ki_read_imm_16();
	uint offset = (BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : word2 >> 6) & 31;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ROL_32(DY, offset) >> (32 - width);
	uint mask = 1 << (width - 1);

	for (; mask && !(data & mask); mask >>= 1, offset++)
		;
	CPU_D[(word2 >> 12) & 7] = offset;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(18);
}


void m68020_bfffo_ai(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = 1 << (width - 1);

	for (; mask && !(data & mask); mask >>= 1, full_offset++)
		;
	CPU_D[(word2 >> 12) & 7] = full_offset;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(28+8);
}


void m68020_bfffo_di(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_DI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = 1 << (width - 1);

	for (; mask && !(data & mask); mask >>= 1, full_offset++)
		;
	CPU_D[(word2 >> 12) & 7] = full_offset;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(28+12);
}


void m68020_bfffo_ix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_IX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = 1 << (width - 1);

	for (; mask && !(data & mask); mask >>= 1, full_offset++)
		;
	CPU_D[(word2 >> 12) & 7] = full_offset;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(28+14);
}


void m68020_bfffo_aw(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AW + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = 1 << (width - 1);

	for (; mask && !(data & mask); mask >>= 1, full_offset++)
		;
	CPU_D[(word2 >> 12) & 7] = full_offset;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(28+12);
}


void m68020_bfffo_al(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AL + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = 1 << (width - 1);

	for (; mask && !(data & mask); mask >>= 1, full_offset++)
		;
	CPU_D[(word2 >> 12) & 7] = full_offset;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(28+16);
}


void m68020_bfffo_pcdi(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint base = ea + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = 1 << (width - 1);

	for (; mask && !(data & mask); mask >>= 1, full_offset++)
		;
	CPU_D[(word2 >> 12) & 7] = full_offset;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(28+12);
}


void m68020_bfffo_pcix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_PCIX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = 1 << (width - 1);

	for (; mask && !(data & mask); mask >>= 1, full_offset++)
		;
	CPU_D[(word2 >> 12) & 7] = full_offset;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(28+14);
}


void m68020_bfins_d(void)
{
	uint word2 = m68ki_read_imm_16();
	uint offset = (BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : word2 >> 6) & 31;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint insert = MASK_OUT_ABOVE_32(CPU_D[(word2 >> 12) & 7] << (32 - width));
	uint orig_insert = insert >> (32 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	insert = ROR_32(insert, offset);
	mask = ~ROR_32(mask, offset);

	DY &= mask;
	DY |= insert;

	CPU_N = orig_insert >> (width - 1);
	CPU_NOT_Z = orig_insert;
	USE_CLKS(10);
}


void m68020_bfins_ai(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint insert = MASK_OUT_ABOVE_32(CPU_D[(word2 >> 12) & 7] << (32 - width));
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)) | (insert >> offset));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))) | MASK_OUT_ABOVE_8(insert << (8 - offset)));

	CPU_N = GET_MSB_32(insert);
	CPU_NOT_Z = insert;
	USE_CLKS(17+8);
}


void m68020_bfins_di(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_DI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint insert = MASK_OUT_ABOVE_32(CPU_D[(word2 >> 12) & 7] << (32 - width));
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)) | (insert >> offset));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))) | MASK_OUT_ABOVE_8(insert << (8 - offset)));

	CPU_N = GET_MSB_32(insert);
	CPU_NOT_Z = insert;
	USE_CLKS(17+12);
}


void m68020_bfins_ix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_IX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint insert = MASK_OUT_ABOVE_32(CPU_D[(word2 >> 12) & 7] << (32 - width));
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)) | (insert >> offset));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))) | MASK_OUT_ABOVE_8(insert << (8 - offset)));

	CPU_N = GET_MSB_32(insert);
	CPU_NOT_Z = insert;
	USE_CLKS(17+14);
}


void m68020_bfins_aw(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AW + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint insert = MASK_OUT_ABOVE_32(CPU_D[(word2 >> 12) & 7] << (32 - width));
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)) | (insert >> offset));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))) | MASK_OUT_ABOVE_8(insert << (8 - offset)));

	CPU_N = GET_MSB_32(insert);
	CPU_NOT_Z = insert;
	USE_CLKS(17+12);
}


void m68020_bfins_al(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AL + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint insert = MASK_OUT_ABOVE_32(CPU_D[(word2 >> 12) & 7] << (32 - width));
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) & ~(mask >> offset)) | (insert >> offset));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) & ~(mask << (8 - offset))) | MASK_OUT_ABOVE_8(insert << (8 - offset)));

	CPU_N = GET_MSB_32(insert);
	CPU_NOT_Z = insert;
	USE_CLKS(17+16);
}


void m68020_bfset_d(void)
{
	uint word2 = m68ki_read_imm_16();
	uint offset = (BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : word2 >> 6) & 31;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ROL_32(DY, offset) >> (32 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	DY |= mask;

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(12);
}


void m68020_bfset_ai(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) | (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) | (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+8);
}


void m68020_bfset_di(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_DI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) | (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) | (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+12);
}


void m68020_bfset_ix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_IX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) | (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) | (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+14);
}


void m68020_bfset_aw(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AW + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) | (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) | (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+12);
}


void m68020_bfset_al(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AL + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);
	uint mask = MASK_OUT_ABOVE_32(0xffffffff << (32 - width));

	m68ki_write_32(base, (m68ki_read_32(base) | (mask >> offset)));
	if ((width + offset) > 32)
		m68ki_write_8(base + 4, (m68ki_read_8(base + 4) | (mask << (8 - offset))));

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(20+16);
}


void m68020_bftst_d(void)
{
	uint word2 = m68ki_read_imm_16();
	uint offset = (BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : word2 >> 6) & 31;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ROL_32(DY, offset) >> (32 - width);

	/* if offset + width > 32, wraps around in register */

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(6);
}


void m68020_bftst_ai(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(13+8);
}


void m68020_bftst_di(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_DI + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(13+12);
}


void m68020_bftst_ix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_IX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(13+14);
}


void m68020_bftst_aw(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AW + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(13+12);
}


void m68020_bftst_al(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_AL + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(13+16);
}


void m68020_bftst_pcdi(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint base = ea + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(13+12);
}


void m68020_bftst_pcix(void)
{
	uint word2 = m68ki_read_imm_16();
	uint full_offset = BIT_B(word2) ? MAKE_INT_32(CPU_D[(word2 >> 6) & 7]) : (word2 >> 6) & 31;
	uint base = EA_PCIX + (full_offset >> 3);
	uint offset = full_offset & 7;
	uint width = (((BIT_5(word2) ? CPU_D[word2 & 7] : word2) - 1) & 31) + 1;
	uint data = ((m68ki_read_32(base) << offset) | ((offset + width > 32) ? m68ki_read_8(base + 4) >> (8 - offset) : 0)) >> (32
																															 - width);

	CPU_N = (data >> (width - 1)) & 1;
	CPU_NOT_Z = data;
	USE_CLKS(13+14);
}


void m68010_bkpt(void)
{
	if (CPU_MODE & CPU_MODE_010_PLUS)
	{
		m68ki_bkpt_ack(CPU_MODE & CPU_MODE_020_PLUS ? CPU_IR & 7 : 0);	/* auto-disable (see m68kcpu.h) */
		USE_CLKS(11);
	}
	m68000_illegal();
}


void m68000_bra_8(void)
{
	m68ki_add_trace();				   /* auto-disable (see m68kcpu.h) */
	m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
	USE_CLKS(10);
}


void m68000_bra_16(void)
{
	m68ki_add_trace();				   /* auto-disable (see m68kcpu.h) */
	m68ki_branch_word(m68ki_read_16(CPU_PC));
	USE_CLKS(10);
}


void m68020_bra_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_32(CPU_PC));
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68000_bset_r_d(void)
{
	uint *d_dst = &DY;
	uint mask = 1 << (DX & 0x1f);

	CPU_NOT_Z = *d_dst & mask;
	*d_dst |= mask;
	USE_CLKS(8);
}


void m68000_bset_r_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+4);
}


void m68000_bset_r_pi(void)
{
	uint ea = EA_PI_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+4);
}


void m68000_bset_r_pi7(void)
{
	uint ea = EA_PI7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+4);
}


void m68000_bset_r_pd(void)
{
	uint ea = EA_PD_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+6);
}


void m68000_bset_r_pd7(void)
{
	uint ea = EA_PD7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+6);
}


void m68000_bset_r_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+8);
}


void m68000_bset_r_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+10);
}


void m68000_bset_r_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+8);
}


void m68000_bset_r_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+12);
}


void m68000_bset_s_d(void)
{
	uint *d_dst = &DY;
	uint mask = 1 << (m68ki_read_imm_8() & 0x1f);

	CPU_NOT_Z = *d_dst & mask;
	*d_dst |= mask;
	USE_CLKS(12);
}


void m68000_bset_s_ai(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_AI;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+4);
}


void m68000_bset_s_pi(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PI_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+4);
}


void m68000_bset_s_pi7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PI7_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+4);
}


void m68000_bset_s_pd(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PD_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+6);
}


void m68000_bset_s_pd7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_PD7_8;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+6);
}


void m68000_bset_s_di(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_DI;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+8);
}


void m68000_bset_s_ix(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_IX;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+10);
}


void m68000_bset_s_aw(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_AW;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+8);
}


void m68000_bset_s_al(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea = EA_AL;
	uint src = m68ki_read_8(ea);

	CPU_NOT_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+12);
}


void m68000_bsr_8(void)
{
	m68ki_add_trace();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(CPU_PC);
	m68ki_branch_byte(MASK_OUT_ABOVE_8(CPU_IR));
	USE_CLKS(18);
}


void m68000_bsr_16(void)
{
	m68ki_add_trace();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(CPU_PC + 2);
	m68ki_branch_word(m68ki_read_16(CPU_PC));
	USE_CLKS(18);
}


void m68020_bsr_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_push_32(CPU_PC + 2);
		m68ki_branch_word(m68ki_read_16(CPU_PC));
		USE_CLKS(7);
		return;
	}
	m68000_illegal();
}


void m68000_btst_r_d(void)
{
	CPU_NOT_Z = DY & (1 << (DX & 0x1f));
	USE_CLKS(6);
}


void m68000_btst_r_ai(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_AI) & (1 << (DX & 7));
	USE_CLKS(4+4);
}


void m68000_btst_r_pi(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_PI_8) & (1 << (DX & 7));
	USE_CLKS(4+4);
}


void m68000_btst_r_pi7(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_PI7_8) & (1 << (DX & 7));
	USE_CLKS(4+4);
}


void m68000_btst_r_pd(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_PD_8) & (1 << (DX & 7));
	USE_CLKS(4+6);
}


void m68000_btst_r_pd7(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_PD7_8) & (1 << (DX & 7));
	USE_CLKS(4+6);
}


void m68000_btst_r_di(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_DI) & (1 << (DX & 7));
	USE_CLKS(4+8);
}


void m68000_btst_r_ix(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_IX) & (1 << (DX & 7));
	USE_CLKS(4+10);
}


void m68000_btst_r_aw(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_AW) & (1 << (DX & 7));
	USE_CLKS(4+8);
}


void m68000_btst_r_al(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_AL) & (1 << (DX & 7));
	USE_CLKS(4+12);
}


void m68000_btst_r_pcdi(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	CPU_NOT_Z = m68ki_read_8(ea) & (1 << (DX & 7));
	USE_CLKS(4+8);
}


void m68000_btst_r_pcix(void)
{
	CPU_NOT_Z = m68ki_read_8(EA_PCIX) & (1 << (DX & 7));
	USE_CLKS(4+10);
}


void m68000_btst_r_i(void)
{
	CPU_NOT_Z = m68ki_read_imm_8() & (1 << (DX & 7));
	USE_CLKS(4+4);
}


void m68000_btst_s_d(void)
{
	CPU_NOT_Z = DY & (1 << (m68ki_read_imm_8() & 0x1f));
	USE_CLKS(10);
}


void m68000_btst_s_ai(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_AI) & (1 << bit);
	USE_CLKS(8+4);
}


void m68000_btst_s_pi(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_PI_8) & (1 << bit);
	USE_CLKS(8+4);
}


void m68000_btst_s_pi7(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_PI7_8) & (1 << bit);
	USE_CLKS(8+4);
}


void m68000_btst_s_pd(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_PD_8) & (1 << bit);
	USE_CLKS(8+6);
}


void m68000_btst_s_pd7(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_PD7_8) & (1 << bit);
	USE_CLKS(8+6);
}


void m68000_btst_s_di(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_DI) & (1 << bit);
	USE_CLKS(8+8);
}


void m68000_btst_s_ix(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_IX) & (1 << bit);
	USE_CLKS(8+10);
}


void m68000_btst_s_aw(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_AW) & (1 << bit);
	USE_CLKS(8+8);
}


void m68000_btst_s_al(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_AL) & (1 << bit);
	USE_CLKS(8+12);
}


void m68000_btst_s_pcdi(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	CPU_NOT_Z = m68ki_read_8(ea) & (1 << bit);
	USE_CLKS(8+8);
}


void m68000_btst_s_pcix(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	CPU_NOT_Z = m68ki_read_8(EA_PCIX) & (1 << bit);
	USE_CLKS(8+10);
}


void m68020_callm_ai(void)
{
	if (CPU_MODE & CPU_MODE_020)
	{
		uint ea = EA_AI;

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_PC += 2;
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		USE_CLKS(30+8);
		return;
	}
	m68000_illegal();
}


void m68020_callm_di(void)
{
	if (CPU_MODE & CPU_MODE_020)
	{
		uint ea = EA_DI;

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_PC += 2;
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		USE_CLKS(30+12);
		return;
	}
	m68000_illegal();
}


void m68020_callm_ix(void)
{
	if (CPU_MODE & CPU_MODE_020)
	{
		uint ea = EA_IX;

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_PC += 2;
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		USE_CLKS(30+14);
		return;
	}
	m68000_illegal();
}


void m68020_callm_aw(void)
{
	if (CPU_MODE & CPU_MODE_020)
	{
		uint ea = EA_AW;

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_PC += 2;
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		USE_CLKS(30+12);
		return;
	}
	m68000_illegal();
}


void m68020_callm_al(void)
{
	if (CPU_MODE & CPU_MODE_020)
	{
		uint ea = EA_AL;

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_PC += 2;
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		USE_CLKS(30+16);
		return;
	}
	m68000_illegal();
}


void m68020_callm_pcdi(void)
{
	if (CPU_MODE & CPU_MODE_020)
	{
		uint old_pc = (CPU_PC+=2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_PC += 2;
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		USE_CLKS(30+12);
		return;
	}
	m68000_illegal();
}


void m68020_callm_pcix(void)
{
	if (CPU_MODE & CPU_MODE_020)
	{
		uint ea = EA_PCIX;

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_PC += 2;
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		USE_CLKS(30+14);
		return;
	}
	m68000_illegal();
}


void m68020_cas_ai_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_AI;
		uint dst = m68ki_read_8(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_8(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(*d_src, dst, res);
		CPU_C = CFLAG_SUB_8(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_8(*d_src) | dst;
		else
			m68ki_write_8(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+4);
		return;
	}
	m68000_illegal();
}


void m68020_cas_pi_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_PI_8;
		uint dst = m68ki_read_8(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_8(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(*d_src, dst, res);
		CPU_C = CFLAG_SUB_8(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_8(*d_src) | dst;
		else
			m68ki_write_8(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+4);
		return;
	}
	m68000_illegal();
}


void m68020_cas_pi7_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_PI7_8;
		uint dst = m68ki_read_8(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_8(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(*d_src, dst, res);
		CPU_C = CFLAG_SUB_8(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_8(*d_src) | dst;
		else
			m68ki_write_8(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+4);
		return;
	}
	m68000_illegal();
}


void m68020_cas_pd_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_PD_8;
		uint dst = m68ki_read_8(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_8(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(*d_src, dst, res);
		CPU_C = CFLAG_SUB_8(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_8(*d_src) | dst;
		else
			m68ki_write_8(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+6);
		return;
	}
	m68000_illegal();
}


void m68020_cas_pd7_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_PD7_8;
		uint dst = m68ki_read_8(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_8(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(*d_src, dst, res);
		CPU_C = CFLAG_SUB_8(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_8(*d_src) | dst;
		else
			m68ki_write_8(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+6);
		return;
	}
	m68000_illegal();
}


void m68020_cas_di_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_DI;
		uint dst = m68ki_read_8(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_8(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(*d_src, dst, res);
		CPU_C = CFLAG_SUB_8(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_8(*d_src) | dst;
		else
			m68ki_write_8(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+8);
		return;
	}
	m68000_illegal();
}


void m68020_cas_ix_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_IX;
		uint dst = m68ki_read_8(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_8(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(*d_src, dst, res);
		CPU_C = CFLAG_SUB_8(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_8(*d_src) | dst;
		else
			m68ki_write_8(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+10);
		return;
	}
	m68000_illegal();
}


void m68020_cas_aw_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_AW;
		uint dst = m68ki_read_8(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_8(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(*d_src, dst, res);
		CPU_C = CFLAG_SUB_8(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_8(*d_src) | dst;
		else
			m68ki_write_8(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+8);
		return;
	}
	m68000_illegal();
}


void m68020_cas_al_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_AL;
		uint dst = m68ki_read_8(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_8(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(*d_src, dst, res);
		CPU_C = CFLAG_SUB_8(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_8(*d_src) | dst;
		else
			m68ki_write_8(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+12);
		return;
	}
	m68000_illegal();
}


void m68020_cas_ai_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_AI;
		uint dst = m68ki_read_16(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_16(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_16(*d_src, dst, res);
		CPU_C = CFLAG_SUB_16(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_16(*d_src) | dst;
		else
			m68ki_write_16(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+4);
		return;
	}
	m68000_illegal();
}


void m68020_cas_pi_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_PI_16;
		uint dst = m68ki_read_16(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_16(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_16(*d_src, dst, res);
		CPU_C = CFLAG_SUB_16(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_16(*d_src) | dst;
		else
			m68ki_write_16(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+4);
		return;
	}
	m68000_illegal();
}


void m68020_cas_pd_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_PD_16;
		uint dst = m68ki_read_16(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_16(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_16(*d_src, dst, res);
		CPU_C = CFLAG_SUB_16(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_16(*d_src) | dst;
		else
			m68ki_write_16(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+6);
		return;
	}
	m68000_illegal();
}


void m68020_cas_di_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_DI;
		uint dst = m68ki_read_16(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_16(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_16(*d_src, dst, res);
		CPU_C = CFLAG_SUB_16(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_16(*d_src) | dst;
		else
			m68ki_write_16(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+8);
		return;
	}
	m68000_illegal();
}


void m68020_cas_ix_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_IX;
		uint dst = m68ki_read_16(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_16(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_16(*d_src, dst, res);
		CPU_C = CFLAG_SUB_16(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_16(*d_src) | dst;
		else
			m68ki_write_16(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+10);
		return;
	}
	m68000_illegal();
}


void m68020_cas_aw_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_AW;
		uint dst = m68ki_read_16(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_16(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_16(*d_src, dst, res);
		CPU_C = CFLAG_SUB_16(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_16(*d_src) | dst;
		else
			m68ki_write_16(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+8);
		return;
	}
	m68000_illegal();
}


void m68020_cas_al_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_AL;
		uint dst = m68ki_read_16(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_16(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_16(*d_src, dst, res);
		CPU_C = CFLAG_SUB_16(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = MASK_OUT_BELOW_16(*d_src) | dst;
		else
			m68ki_write_16(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+12);
		return;
	}
	m68000_illegal();
}


void m68020_cas_ai_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_AI;
		uint dst = m68ki_read_32(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_32(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_32(*d_src, dst, res);
		CPU_C = CFLAG_SUB_32(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = dst;
		else
			m68ki_write_32(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+8);
		return;
	}
	m68000_illegal();
}


void m68020_cas_pi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_PI_32;
		uint dst = m68ki_read_32(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_32(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_32(*d_src, dst, res);
		CPU_C = CFLAG_SUB_32(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = dst;
		else
			m68ki_write_32(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+8);
		return;
	}
	m68000_illegal();
}


void m68020_cas_pd_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_PD_32;
		uint dst = m68ki_read_32(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_32(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_32(*d_src, dst, res);
		CPU_C = CFLAG_SUB_32(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = dst;
		else
			m68ki_write_32(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+10);
		return;
	}
	m68000_illegal();
}


void m68020_cas_di_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_DI;
		uint dst = m68ki_read_32(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_32(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_32(*d_src, dst, res);
		CPU_C = CFLAG_SUB_32(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = dst;
		else
			m68ki_write_32(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+12);
		return;
	}
	m68000_illegal();
}


void m68020_cas_ix_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_IX;
		uint dst = m68ki_read_32(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_32(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_32(*d_src, dst, res);
		CPU_C = CFLAG_SUB_32(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = dst;
		else
			m68ki_write_32(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+14);
		return;
	}
	m68000_illegal();
}


void m68020_cas_aw_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_AW;
		uint dst = m68ki_read_32(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_32(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_32(*d_src, dst, res);
		CPU_C = CFLAG_SUB_32(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = dst;
		else
			m68ki_write_32(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+12);
		return;
	}
	m68000_illegal();
}


void m68020_cas_al_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint ea = EA_AL;
		uint dst = m68ki_read_32(ea);
		uint *d_src = &CPU_D[word2 & 7];
		uint res = MASK_OUT_ABOVE_32(dst - *d_src);

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_32(*d_src, dst, res);
		CPU_C = CFLAG_SUB_32(*d_src, dst, res);

		if (CPU_NOT_Z)
			*d_src = dst;
		else
			m68ki_write_32(ea, CPU_D[(word2 >> 6) & 7]);
		USE_CLKS(15+16);
		return;
	}
	m68000_illegal();
}


void m68020_cas2_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_32();
		uint *r_src1 = &CPU_D[(word2 >> 16) & 7];
		uint ea1 = m68k_cpu_dar[word2 >> 31][(word2 >> 28) & 7];
		uint dst1 = m68ki_read_16(ea1);
		uint res1 = MASK_OUT_ABOVE_16(dst1 - *r_src1);
		uint *r_src2 = &CPU_D[word2 & 7];
		uint ea2 = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint dst2 = m68ki_read_16(ea2);
		uint res2;

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_16(res1);
		CPU_NOT_Z = res1;
		CPU_V = VFLAG_SUB_16(*r_src1, dst1, res1);
		CPU_C = CFLAG_SUB_16(*r_src1, dst1, res1);

		if (!CPU_NOT_Z)
		{
			res2 = MASK_OUT_ABOVE_16(dst2 - *r_src2);

			CPU_N = GET_MSB_16(res2);
			CPU_NOT_Z = res2;
			CPU_V = VFLAG_SUB_16(*r_src2, dst2, res2);
			CPU_C = CFLAG_SUB_16(*r_src2, dst2, res2);

			if (!CPU_NOT_Z)
			{
				m68ki_write_16(ea1, CPU_D[(word2 >> 22) & 7]);
				m68ki_write_16(ea2, CPU_D[(word2 >> 6) & 7]);
				USE_CLKS(22);
				return;
			}
		}
		*r_src1 = BIT_1F(word2) ? MAKE_INT_16(dst1) : MASK_OUT_ABOVE_16(*r_src1) | dst1;
		*r_src2 = BIT_F(word2) ? MAKE_INT_16(dst2) : MASK_OUT_ABOVE_16(*r_src2) | dst2;
		USE_CLKS(25);
		return;
	}
	m68000_illegal();
}


void m68020_cas2_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_32();
		uint *r_src1 = &CPU_D[(word2 >> 16) & 7];
		uint ea1 = m68k_cpu_dar[word2 >> 31][(word2 >> 28) & 7];
		uint dst1 = m68ki_read_32(ea1);
		uint res1 = MASK_OUT_ABOVE_32(dst1 - *r_src1);
		uint *r_src2 = &CPU_D[word2 & 7];
		uint ea2 = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint dst2 = m68ki_read_32(ea2);
		uint res2;

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_N = GET_MSB_32(res1);
		CPU_NOT_Z = res1;
		CPU_V = VFLAG_SUB_32(*r_src1, dst1, res1);
		CPU_C = CFLAG_SUB_32(*r_src1, dst1, res1);

		if (!CPU_NOT_Z)
		{
			res2 = MASK_OUT_ABOVE_32(dst2 - *r_src2);

			CPU_N = GET_MSB_32(res2);
			CPU_NOT_Z = res2;
			CPU_V = VFLAG_SUB_32(*r_src2, dst2, res2);
			CPU_C = CFLAG_SUB_32(*r_src2, dst2, res2);

			if (!CPU_NOT_Z)
			{
				m68ki_write_32(ea1, CPU_D[(word2 >> 22) & 7]);
				m68ki_write_32(ea2, CPU_D[(word2 >> 6) & 7]);
				USE_CLKS(22);
				return;
			}
		}
		*r_src1 = dst1;
		*r_src2 = dst2;
		USE_CLKS(25);
		return;
	}
	m68000_illegal();
}


void m68000_chk_d_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(DY);

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_ai_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_AI));

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+4);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_pi_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_PI_16));

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+4);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_pd_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_PD_16));

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+6);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_di_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_DI));

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+8);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_ix_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_IX));

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+10);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_aw_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_AW));

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+8);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_al_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_AL));

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+12);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_pcdi_16(void)
{
	int src = MAKE_INT_16(DX);
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	int bound = MAKE_INT_16(m68ki_read_16(ea));

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+8);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_pcix_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_PCIX));

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+10);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68000_chk_i_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_imm_16());

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+4);
		return;
	}
	CPU_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


void m68020_chk_d_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = DY;

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_ai_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = m68ki_read_32(EA_AI);

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+4);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_pi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = m68ki_read_32(EA_PI_16);

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+4);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_pd_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = m68ki_read_32(EA_PD_16);

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+6);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_di_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = m68ki_read_32(EA_DI);

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+8);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_ix_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = m68ki_read_32(EA_IX);

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+10);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_aw_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = m68ki_read_32(EA_AW);

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+8);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_al_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = m68ki_read_32(EA_AL);

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+12);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_pcdi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		uint old_pc = (CPU_PC+=2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		int bound = m68ki_read_32(ea);

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+8);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_pcix_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = m68ki_read_32(EA_PCIX);

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+10);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk_i_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		int src = DX;
		int bound = m68ki_read_imm_16();

		if (src >= 0 && src <= bound)
		{
			USE_CLKS(8+4);
			return;
		}
		CPU_N = src < 0;
		m68ki_interrupt(EXCEPTION_CHK);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_ai_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_AI;
		uint lower_bound = m68ki_read_8(ea);
		uint upper_bound = m68ki_read_8(ea + 1);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_8(lower_bound);
			upper_bound = MAKE_INT_8(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_8(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+4);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_di_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_DI;
		uint lower_bound = m68ki_read_8(ea);
		uint upper_bound = m68ki_read_8(ea + 1);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_8(lower_bound);
			upper_bound = MAKE_INT_8(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_8(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+8);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_ix_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_IX;
		uint lower_bound = m68ki_read_8(ea);
		uint upper_bound = m68ki_read_8(ea + 1);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_8(lower_bound);
			upper_bound = MAKE_INT_8(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_8(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+10);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_aw_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_AW;
		uint lower_bound = m68ki_read_8(ea);
		uint upper_bound = m68ki_read_8(ea + 1);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_8(lower_bound);
			upper_bound = MAKE_INT_8(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_8(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+8);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_al_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_AL;
		uint lower_bound = m68ki_read_8(ea);
		uint upper_bound = m68ki_read_8(ea + 1);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_8(lower_bound);
			upper_bound = MAKE_INT_8(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_8(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+12);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_pcdi_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint old_pc = (CPU_PC+=2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		uint lower_bound = m68ki_read_8(ea);
		uint upper_bound = m68ki_read_8(ea + 1);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_8(lower_bound);
			upper_bound = MAKE_INT_8(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_8(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+8);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_pcix_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_PCIX;
		uint lower_bound = m68ki_read_8(ea);
		uint upper_bound = m68ki_read_8(ea + 1);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_8(lower_bound);
			upper_bound = MAKE_INT_8(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_8(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+10);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_ai_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_AI;
		uint lower_bound = m68ki_read_16(ea);
		uint upper_bound = m68ki_read_16(ea + 2);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_16(lower_bound);
			upper_bound = MAKE_INT_16(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_16(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+4);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_di_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_DI;
		uint lower_bound = m68ki_read_16(ea);
		uint upper_bound = m68ki_read_16(ea + 2);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_16(lower_bound);
			upper_bound = MAKE_INT_16(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_16(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+8);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_ix_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_IX;
		uint lower_bound = m68ki_read_16(ea);
		uint upper_bound = m68ki_read_16(ea + 2);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_16(lower_bound);
			upper_bound = MAKE_INT_16(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_16(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+10);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_aw_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_AW;
		uint lower_bound = m68ki_read_16(ea);
		uint upper_bound = m68ki_read_16(ea + 2);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_16(lower_bound);
			upper_bound = MAKE_INT_16(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_16(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+8);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_al_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_AL;
		uint lower_bound = m68ki_read_16(ea);
		uint upper_bound = m68ki_read_16(ea + 2);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_16(lower_bound);
			upper_bound = MAKE_INT_16(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_16(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+12);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_pcdi_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint old_pc = (CPU_PC+=2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		uint lower_bound = m68ki_read_16(ea);
		uint upper_bound = m68ki_read_16(ea + 2);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_16(lower_bound);
			upper_bound = MAKE_INT_16(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_16(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+8);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_pcix_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_PCIX;
		uint lower_bound = m68ki_read_16(ea);
		uint upper_bound = m68ki_read_16(ea + 2);

		if (BIT_F(word2))
		{
			lower_bound = MAKE_INT_16(lower_bound);
			upper_bound = MAKE_INT_16(upper_bound);
		}
		else
			src = MASK_OUT_ABOVE_16(src);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+10);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_ai_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_AI;
		uint lower_bound = m68ki_read_32(ea);
		uint upper_bound = m68ki_read_32(ea + 4);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+8);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_di_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_DI;
		uint lower_bound = m68ki_read_32(ea);
		uint upper_bound = m68ki_read_32(ea + 4);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+12);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_ix_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_IX;
		uint lower_bound = m68ki_read_32(ea);
		uint upper_bound = m68ki_read_32(ea + 4);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+14);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_aw_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_AW;
		uint lower_bound = m68ki_read_32(ea);
		uint upper_bound = m68ki_read_32(ea + 4);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+12);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_al_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_AL;
		uint lower_bound = m68ki_read_32(ea);
		uint upper_bound = m68ki_read_32(ea + 4);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+16);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_pcdi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint old_pc = (CPU_PC+=2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		uint lower_bound = m68ki_read_32(ea);
		uint upper_bound = m68ki_read_32(ea + 4);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+12);
		return;
	}
	m68000_illegal();
}


void m68020_chk2_cmp2_pcix_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint word2 = m68ki_read_imm_16();
		uint src = m68k_cpu_dar[word2 >> 15][(word2 >> 12) & 7];
		uint ea = EA_PCIX;
		uint lower_bound = m68ki_read_32(ea);
		uint upper_bound = m68ki_read_32(ea + 4);

		CPU_NOT_Z = !(src == lower_bound || src == upper_bound);
		CPU_C = src < lower_bound || src > upper_bound;

		if (CPU_C && BIT_B(word2))	   /* chk2 */
			m68ki_interrupt(EXCEPTION_CHK);

		USE_CLKS(18+14);
		return;
	}
	m68000_illegal();
}


void m68000_clr_d_8(void)
{
	DY &= 0xffffff00;

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(4);
}


void m68000_clr_ai_8(void)
{
	m68ki_write_8(EA_AI, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+4);
}


void m68000_clr_pi_8(void)
{
	m68ki_write_8(EA_PI_8, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+4);
}


void m68000_clr_pi7_8(void)
{
	m68ki_write_8(EA_PI7_8, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+4);
}


void m68000_clr_pd_8(void)
{
	m68ki_write_8(EA_PD_8, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+6);
}


void m68000_clr_pd7_8(void)
{
	m68ki_write_8(EA_PD7_8, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+6);
}


void m68000_clr_di_8(void)
{
	m68ki_write_8(EA_DI, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+8);
}


void m68000_clr_ix_8(void)
{
	m68ki_write_8(EA_IX, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+10);
}


void m68000_clr_aw_8(void)
{
	m68ki_write_8(EA_AW, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+8);
}


void m68000_clr_al_8(void)
{
	m68ki_write_8(EA_AL, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+12);
}


void m68000_clr_d_16(void)
{
	DY &= 0xffff0000;

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(4);
}


void m68000_clr_ai_16(void)
{
	m68ki_write_16(EA_AI, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+4);
}


void m68000_clr_pi_16(void)
{
	m68ki_write_16(EA_PI_16, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+4);
}


void m68000_clr_pd_16(void)
{
	m68ki_write_16(EA_PD_16, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+6);
}


void m68000_clr_di_16(void)
{
	m68ki_write_16(EA_DI, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+8);
}


void m68000_clr_ix_16(void)
{
	m68ki_write_16(EA_IX, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+10);
}


void m68000_clr_aw_16(void)
{
	m68ki_write_16(EA_AW, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+8);
}


void m68000_clr_al_16(void)
{
	m68ki_write_16(EA_AL, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(8+12);
}


void m68000_clr_d_32(void)
{
	DY = 0;

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(6);
}


void m68000_clr_ai_32(void)
{
	m68ki_write_32(EA_AI, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(12+8);
}


void m68000_clr_pi_32(void)
{
	m68ki_write_32(EA_PI_32, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(12+8);
}


void m68000_clr_pd_32(void)
{
	m68ki_write_32(EA_PD_32, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(12+10);
}


void m68000_clr_di_32(void)
{
	m68ki_write_32(EA_DI, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(12+12);
}


void m68000_clr_ix_32(void)
{
	m68ki_write_32(EA_IX, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(12+14);
}


void m68000_clr_aw_32(void)
{
	m68ki_write_32(EA_AW, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(12+12);
}


void m68000_clr_al_32(void)
{
	m68ki_write_32(EA_AL, 0);

	CPU_N = CPU_V = CPU_C = 0;
	CPU_NOT_Z = 0;
	USE_CLKS(12+16);
}


void m68000_cmp_d_8(void)
{
	uint src = DY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4);
}


void m68000_cmp_ai_8(void)
{
	uint src = m68ki_read_8(EA_AI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_cmp_pi_8(void)
{
	uint src = m68ki_read_8(EA_PI_8);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_cmp_pi7_8(void)
{
	uint src = m68ki_read_8(EA_PI7_8);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_cmp_pd_8(void)
{
	uint src = m68ki_read_8(EA_PD_8);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+6);
}


void m68000_cmp_pd7_8(void)
{
	uint src = m68ki_read_8(EA_PD7_8);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+6);
}


void m68000_cmp_di_8(void)
{
	uint src = m68ki_read_8(EA_DI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_cmp_ix_8(void)
{
	uint src = m68ki_read_8(EA_IX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_cmp_aw_8(void)
{
	uint src = m68ki_read_8(EA_AW);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_cmp_al_8(void)
{
	uint src = m68ki_read_8(EA_AL);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+12);
}


void m68000_cmp_pcdi_8(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_8(ea);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_cmp_pcix_8(void)
{
	uint src = m68ki_read_8(EA_PCIX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_cmp_i_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_cmp_d_16(void)
{
	uint src = DY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_cmp_a_16(void)
{
	uint src = AY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_cmp_ai_16(void)
{
	uint src = m68ki_read_16(EA_AI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_cmp_pi_16(void)
{
	uint src = m68ki_read_16(EA_PI_16);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_cmp_pd_16(void)
{
	uint src = m68ki_read_16(EA_PD_16);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+6);
}


void m68000_cmp_di_16(void)
{
	uint src = m68ki_read_16(EA_DI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_cmp_ix_16(void)
{
	uint src = m68ki_read_16(EA_IX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_cmp_aw_16(void)
{
	uint src = m68ki_read_16(EA_AW);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_cmp_al_16(void)
{
	uint src = m68ki_read_16(EA_AL);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+12);
}


void m68000_cmp_pcdi_16(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_16(ea);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_cmp_pcix_16(void)
{
	uint src = m68ki_read_16(EA_PCIX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_cmp_i_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_cmp_d_32(void)
{
	uint src = DY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


void m68000_cmp_a_32(void)
{
	uint src = AY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


void m68000_cmp_ai_32(void)
{
	uint src = m68ki_read_32(EA_AI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_cmp_pi_32(void)
{
	uint src = m68ki_read_32(EA_PI_32);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_cmp_pd_32(void)
{
	uint src = m68ki_read_32(EA_PD_32);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


void m68000_cmp_di_32(void)
{
	uint src = m68ki_read_32(EA_DI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_cmp_ix_32(void)
{
	uint src = m68ki_read_32(EA_IX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


void m68000_cmp_aw_32(void)
{
	uint src = m68ki_read_32(EA_AW);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_cmp_al_32(void)
{
	uint src = m68ki_read_32(EA_AL);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+16);
}


void m68000_cmp_pcdi_32(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_32(ea);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_cmp_pcix_32(void)
{
	uint src = m68ki_read_32(EA_PCIX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


void m68000_cmp_i_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_cmpa_d_16(void)
{
	uint src = MAKE_INT_16(DY);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


void m68000_cmpa_a_16(void)
{
	uint src = MAKE_INT_16(AY);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


void m68000_cmpa_ai_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_AI));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+4);
}


void m68000_cmpa_pi_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_PI_16));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+4);
}


void m68000_cmpa_pd_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_PD_16));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+6);
}


void m68000_cmpa_di_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_DI));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_cmpa_ix_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_IX));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


void m68000_cmpa_aw_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_AW));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_cmpa_al_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_AL));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_cmpa_pcdi_16(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = MAKE_INT_16(m68ki_read_16(ea));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_cmpa_pcix_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_PCIX));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


void m68000_cmpa_i_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_imm_16());
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+4);
}


void m68000_cmpa_d_32(void)
{
	uint src = DY;
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


void m68000_cmpa_a_32(void)
{
	uint src = AY;
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


void m68000_cmpa_ai_32(void)
{
	uint src = m68ki_read_32(EA_AI);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_cmpa_pi_32(void)
{
	uint src = m68ki_read_32(EA_PI_32);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_cmpa_pd_32(void)
{
	uint src = m68ki_read_32(EA_PD_32);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


void m68000_cmpa_di_32(void)
{
	uint src = m68ki_read_32(EA_DI);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_cmpa_ix_32(void)
{
	uint src = m68ki_read_32(EA_IX);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


void m68000_cmpa_aw_32(void)
{
	uint src = m68ki_read_32(EA_AW);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_cmpa_al_32(void)
{
	uint src = m68ki_read_32(EA_AL);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+16);
}


void m68000_cmpa_pcdi_32(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_32(ea);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_cmpa_pcix_32(void)
{
	uint src = m68ki_read_32(EA_PCIX);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


void m68000_cmpa_i_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_cmpi_d_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = DY;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8);
}


void m68000_cmpi_ai_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_AI);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_cmpi_pi_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_PI_8);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_cmpi_pi7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_PI7_8);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_cmpi_pd_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_PD_8);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_cmpi_pd7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_PD7_8);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_cmpi_di_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_DI);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_cmpi_ix_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_IX);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_cmpi_aw_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_AW);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_cmpi_al_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_AL);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+12);
}


void m68020_cmpi_pcdi_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_imm_8();
		uint old_pc = (CPU_PC += 2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		uint dst = m68ki_read_8(ea);
		uint res = MASK_OUT_ABOVE_8(dst - src);

		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(src, dst, res);
		CPU_C = CFLAG_SUB_8(src, dst, res);
		USE_CLKS(8);
		return;
	}
	m68000_illegal();
}


void m68020_cmpi_pcix_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_imm_8();
		uint dst = m68ki_read_8(EA_PCIX);
		uint res = MASK_OUT_ABOVE_8(dst - src);

		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_8(src, dst, res);
		CPU_C = CFLAG_SUB_8(src, dst, res);
		USE_CLKS(8);
		return;
	}
	m68000_illegal();
}


void m68000_cmpi_d_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = DY;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8);
}


void m68000_cmpi_ai_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_AI);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_cmpi_pi_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_PI_16);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_cmpi_pd_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_PD_16);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_cmpi_di_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_DI);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_cmpi_ix_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_IX);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_cmpi_aw_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_AW);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_cmpi_al_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_AL);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+12);
}


void m68020_cmpi_pcdi_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_imm_16();
		uint old_pc = (CPU_PC += 2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		uint dst = m68ki_read_16(ea);
		uint res = MASK_OUT_ABOVE_16(dst - src);

		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_16(src, dst, res);
		CPU_C = CFLAG_SUB_16(src, dst, res);
		USE_CLKS(8);
		return;
	}
	m68000_illegal();
}


void m68020_cmpi_pcix_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_imm_16();
		uint dst = m68ki_read_16(EA_PCIX);
		uint res = MASK_OUT_ABOVE_16(dst - src);

		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_16(src, dst, res);
		CPU_C = CFLAG_SUB_16(src, dst, res);
		USE_CLKS(8);
		return;
	}
	m68000_illegal();
}


void m68000_cmpi_d_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = DY;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(14);
}


void m68000_cmpi_ai_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_AI);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_cmpi_pi_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_PI_32);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_cmpi_pd_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_PD_32);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+10);
}


void m68000_cmpi_di_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_DI);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_cmpi_ix_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_IX);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+14);
}


void m68000_cmpi_aw_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_AW);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_cmpi_al_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_AL);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+16);
}


void m68020_cmpi_pcdi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_imm_32();
		uint old_pc = (CPU_PC += 2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		uint dst = m68ki_read_32(ea);
		uint res = MASK_OUT_ABOVE_32(dst - src);

		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_32(src, dst, res);
		CPU_C = CFLAG_SUB_32(src, dst, res);
		USE_CLKS(8);
		return;
	}
	m68000_illegal();
}


void m68020_cmpi_pcix_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_imm_32();
		uint dst = m68ki_read_32(EA_PCIX);
		uint res = MASK_OUT_ABOVE_32(dst - src);

		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = VFLAG_SUB_32(src, dst, res);
		CPU_C = CFLAG_SUB_32(src, dst, res);
		USE_CLKS(8);
		return;
	}
	m68000_illegal();
}


void m68000_cmpm_8_ax7(void)
{
	uint src = m68ki_read_8(AY++);
	uint dst = m68ki_read_8((CPU_A[7] += 2) - 2);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(12);
}


void m68000_cmpm_8_ay7(void)
{
	uint src = m68ki_read_8((CPU_A[7] += 2) - 2);
	uint dst = m68ki_read_8(AX++);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(12);
}


void m68000_cmpm_8_axy7(void)
{
	uint src = m68ki_read_8((CPU_A[7] += 2) - 2);
	uint dst = m68ki_read_8((CPU_A[7] += 2) - 2);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(12);
}


void m68000_cmpm_8(void)
{
	uint src = m68ki_read_8(AY++);
	uint dst = m68ki_read_8(AX++);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_8(src, dst, res);
	CPU_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(12);
}


void m68000_cmpm_16(void)
{
	uint src = m68ki_read_16((AY += 2) - 2);
	uint dst = m68ki_read_16((AX += 2) - 2);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_16(src, dst, res);
	CPU_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(12);
}


void m68000_cmpm_32(void)
{
	uint src = m68ki_read_32((AY += 4) - 4);
	uint dst = m68ki_read_32((AX += 4) - 4);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = VFLAG_SUB_32(src, dst, res);
	CPU_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(20);
}


void m68020_cpbcc(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		return;
	}
	m68000_1111();
}


void m68020_cpdbcc(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		return;
	}
	m68000_1111();
}


void m68020_cpgen(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		return;
	}
	m68000_1111();
}


void m68020_cpscc(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		return;
	}
	m68000_1111();
}


void m68020_cptrapcc(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		return;
	}
	m68000_1111();
}


