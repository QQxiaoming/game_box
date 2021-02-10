#include "m68kcpu.h"

#include "m68kops.h"

/* ======================================================================== */
/* ======================= INSTRUCTION HANDLERS N-Z ======================= */
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


void m68000_nbcd_d(void)
{
	uint *d_dst = &DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(6);
}


void m68000_nbcd_ai(void)
{
	uint ea = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(8+4);
}


void m68000_nbcd_pi(void)
{
	uint ea = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(8+4);
}


void m68000_nbcd_pi7(void)
{
	uint ea = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(8+4);
}


void m68000_nbcd_pd(void)
{
	uint ea = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(8+6);
}


void m68000_nbcd_pd7(void)
{
	uint ea = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(8+6);
}


void m68000_nbcd_di(void)
{
	uint ea = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(8+8);
}


void m68000_nbcd_ix(void)
{
	uint ea = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(8+10);
}


void m68000_nbcd_aw(void)
{
	uint ea = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(8+8);
}


void m68000_nbcd_al(void)
{
	uint ea = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (CPU_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			CPU_NOT_Z = 1;
		CPU_C = CPU_X = 1;
	}
	else
		CPU_C = CPU_X = 0;
	USE_CLKS(8+12);
}


void m68000_neg_d_8(void)
{
	uint *d_dst = &DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(-dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(4);
}


void m68000_neg_ai_8(void)
{
	uint ea = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+4);
}


void m68000_neg_pi_8(void)
{
	uint ea = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+4);
}


void m68000_neg_pi7_8(void)
{
	uint ea = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+4);
}


void m68000_neg_pd_8(void)
{
	uint ea = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+6);
}


void m68000_neg_pd7_8(void)
{
	uint ea = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+6);
}


void m68000_neg_di_8(void)
{
	uint ea = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+8);
}


void m68000_neg_ix_8(void)
{
	uint ea = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+10);
}


void m68000_neg_aw_8(void)
{
	uint ea = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+8);
}


void m68000_neg_al_8(void)
{
	uint ea = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+12);
}


void m68000_neg_d_16(void)
{
	uint *d_dst = &DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(-dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(4);
}


void m68000_neg_ai_16(void)
{
	uint ea = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+4);
}


void m68000_neg_pi_16(void)
{
	uint ea = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+4);
}


void m68000_neg_pd_16(void)
{
	uint ea = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+6);
}


void m68000_neg_di_16(void)
{
	uint ea = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+8);
}


void m68000_neg_ix_16(void)
{
	uint ea = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+10);
}


void m68000_neg_aw_16(void)
{
	uint ea = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+8);
}


void m68000_neg_al_16(void)
{
	uint ea = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(8+12);
}


void m68000_neg_d_32(void)
{
	uint *d_dst = &DY;
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(-dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(6);
}


void m68000_neg_ai_32(void)
{
	uint ea = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(12+8);
}


void m68000_neg_pi_32(void)
{
	uint ea = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(12+8);
}


void m68000_neg_pd_32(void)
{
	uint ea = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(12+10);
}


void m68000_neg_di_32(void)
{
	uint ea = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(12+12);
}


void m68000_neg_ix_32(void)
{
	uint ea = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(12+14);
}


void m68000_neg_aw_32(void)
{
	uint ea = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(12+12);
}


void m68000_neg_al_32(void)
{
	uint ea = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = res != 0;
	USE_CLKS(12+16);
}


void m68000_negx_d_8(void)
{
	uint *d_dst = &DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(4);
}


void m68000_negx_ai_8(void)
{
	uint ea = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(8+4);
}


void m68000_negx_pi_8(void)
{
	uint ea = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(8+4);
}


void m68000_negx_pi7_8(void)
{
	uint ea = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(8+4);
}


void m68000_negx_pd_8(void)
{
	uint ea = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(8+6);
}


void m68000_negx_pd7_8(void)
{
	uint ea = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(8+6);
}


void m68000_negx_di_8(void)
{
	uint ea = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(8+8);
}


void m68000_negx_ix_8(void)
{
	uint ea = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(8+10);
}


void m68000_negx_aw_8(void)
{
	uint ea = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(8+8);
}


void m68000_negx_al_8(void)
{
	uint ea = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_8(dst & res);
	CPU_C = CPU_X = GET_MSB_8(dst | res);
	USE_CLKS(8+12);
}


void m68000_negx_d_16(void)
{
	uint *d_dst = &DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(-dst - (CPU_X != 0));

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = GET_MSB_16(dst | res);
	USE_CLKS(4);
}


void m68000_negx_ai_16(void)
{
	uint ea = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (CPU_X != 0));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = GET_MSB_16(dst | res);
	USE_CLKS(8+4);
}


void m68000_negx_pi_16(void)
{
	uint ea = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (CPU_X != 0));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = GET_MSB_16(dst | res);
	USE_CLKS(8+4);
}


void m68000_negx_pd_16(void)
{
	uint ea = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (CPU_X != 0));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = GET_MSB_16(dst | res);
	USE_CLKS(8+6);
}


void m68000_negx_di_16(void)
{
	uint ea = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (CPU_X != 0));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = GET_MSB_16(dst | res);
	USE_CLKS(8+8);
}


void m68000_negx_ix_16(void)
{
	uint ea = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (CPU_X != 0));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = GET_MSB_16(dst | res);
	USE_CLKS(8+10);
}


void m68000_negx_aw_16(void)
{
	uint ea = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (CPU_X != 0));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = GET_MSB_16(dst | res);
	USE_CLKS(8+8);
}


void m68000_negx_al_16(void)
{
	uint ea = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (CPU_X != 0));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_16(dst & res);
	CPU_C = CPU_X = GET_MSB_16(dst | res);
	USE_CLKS(8+12);
}


void m68000_negx_d_32(void)
{
	uint *d_dst = &DY;
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(-dst - (CPU_X != 0));

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = GET_MSB_32(dst | res);
	USE_CLKS(6);
}


void m68000_negx_ai_32(void)
{
	uint ea = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (CPU_X != 0));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = GET_MSB_32(dst | res);
	USE_CLKS(12+8);
}


void m68000_negx_pi_32(void)
{
	uint ea = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (CPU_X != 0));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = GET_MSB_32(dst | res);
	USE_CLKS(12+8);
}


void m68000_negx_pd_32(void)
{
	uint ea = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (CPU_X != 0));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = GET_MSB_32(dst | res);
	USE_CLKS(12+10);
}


void m68000_negx_di_32(void)
{
	uint ea = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (CPU_X != 0));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = GET_MSB_32(dst | res);
	USE_CLKS(12+12);
}


void m68000_negx_ix_32(void)
{
	uint ea = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (CPU_X != 0));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = GET_MSB_32(dst | res);
	USE_CLKS(12+14);
}


void m68000_negx_aw_32(void)
{
	uint ea = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (CPU_X != 0));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = GET_MSB_32(dst | res);
	USE_CLKS(12+12);
}


void m68000_negx_al_32(void)
{
	uint ea = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (CPU_X != 0));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_V = GET_MSB_32(dst & res);
	CPU_C = CPU_X = GET_MSB_32(dst | res);
	USE_CLKS(12+16);
}


void m68000_nop(void)
{
	m68ki_add_trace();				   /* auto-disable (see m68kcpu.h) */
	USE_CLKS(4);
}


void m68000_not_d_8(void)
{
	uint *d_dst = &DY;
	uint res = MASK_OUT_ABOVE_8(~*d_dst);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4);
}


void m68000_not_ai_8(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_not_pi_8(void)
{
	uint ea = EA_PI_8;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_not_pi7_8(void)
{
	uint ea = EA_PI7_8;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_not_pd_8(void)
{
	uint ea = EA_PD_8;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_not_pd7_8(void)
{
	uint ea = EA_PD7_8;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_not_di_8(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_not_ix_8(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_not_aw_8(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_not_al_8(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+12);
}


void m68000_not_d_16(void)
{
	uint *d_dst = &DY;
	uint res = MASK_OUT_ABOVE_16(~*d_dst);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4);
}


void m68000_not_ai_16(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_not_pi_16(void)
{
	uint ea = EA_PI_16;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_not_pd_16(void)
{
	uint ea = EA_PD_16;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_not_di_16(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_not_ix_16(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_not_aw_16(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_not_al_16(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+12);
}


void m68000_not_d_32(void)
{
	uint *d_dst = &DY;
	uint res = *d_dst = MASK_OUT_ABOVE_32(~*d_dst);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6);
}


void m68000_not_ai_32(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_not_pi_32(void)
{
	uint ea = EA_PI_32;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_not_pd_32(void)
{
	uint ea = EA_PD_32;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+10);
}


void m68000_not_di_32(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_not_ix_32(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+14);
}


void m68000_not_aw_32(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_not_al_32(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+16);
}


void m68000_or_er_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= MASK_OUT_ABOVE_8(DY)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4);
}


void m68000_or_er_ai_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_AI)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_or_er_pi_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PI_8)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_or_er_pi7_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PI7_8)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_or_er_pd_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PD_8)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+6);
}


void m68000_or_er_pd7_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PD7_8)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+6);
}


void m68000_or_er_di_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_DI)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_or_er_ix_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_IX)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+10);
}


void m68000_or_er_aw_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_AW)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_or_er_al_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_AL)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+12);
}


void m68000_or_er_pcdi_8(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(ea)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_or_er_pcix_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PCIX)));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+10);
}


void m68000_or_er_i_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_imm_8()));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_or_er_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= MASK_OUT_ABOVE_16(DY)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4);
}


void m68000_or_er_ai_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_AI)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_or_er_pi_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_PI_16)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_or_er_pd_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_PD_16)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+6);
}


void m68000_or_er_di_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_DI)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_or_er_ix_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_IX)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+10);
}


void m68000_or_er_aw_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_AW)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_or_er_al_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_AL)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+12);
}


void m68000_or_er_pcdi_16(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(ea)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+8);
}


void m68000_or_er_pcix_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_PCIX)));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+10);
}


void m68000_or_er_i_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_imm_16()));

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(4+4);
}


void m68000_or_er_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= DY));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8);
}


void m68000_or_er_ai_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_AI)));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+8);
}


void m68000_or_er_pi_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_PI_32)));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+8);
}


void m68000_or_er_pd_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_PD_32)));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+10);
}


void m68000_or_er_di_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_DI)));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+12);
}


void m68000_or_er_ix_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_IX)));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+14);
}


void m68000_or_er_aw_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_AW)));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+12);
}


void m68000_or_er_al_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_AL)));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+16);
}


void m68000_or_er_pcdi_32(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(ea)));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+12);
}


void m68000_or_er_pcix_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_PCIX)));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+14);
}


void m68000_or_er_i_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_imm_32()));

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(6+10);
}


void m68000_or_re_ai_8(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_or_re_pi_8(void)
{
	uint ea = EA_PI_8;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_or_re_pi7_8(void)
{
	uint ea = EA_PI7_8;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_or_re_pd_8(void)
{
	uint ea = EA_PD_8;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_or_re_pd7_8(void)
{
	uint ea = EA_PD7_8;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_or_re_di_8(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_or_re_ix_8(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_or_re_aw_8(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_or_re_al_8(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+12);
}


void m68000_or_re_ai_16(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_or_re_pi_16(void)
{
	uint ea = EA_PI_16;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_or_re_pd_16(void)
{
	uint ea = EA_PD_16;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_or_re_di_16(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_or_re_ix_16(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_or_re_aw_16(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_or_re_al_16(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8+12);
}


void m68000_or_re_ai_32(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_or_re_pi_32(void)
{
	uint ea = EA_PI_32;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_or_re_pd_32(void)
{
	uint ea = EA_PD_32;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+10);
}


void m68000_or_re_di_32(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_or_re_ix_32(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+14);
}


void m68000_or_re_aw_32(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_or_re_al_32(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+16);
}


void m68000_ori_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DY |= m68ki_read_imm_8()));

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8);
}


void m68000_ori_ai_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_ori_pi_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PI_8;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_ori_pi7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PI7_8;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_ori_pd_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PD_8;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+6);
}


void m68000_ori_pd7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PD7_8;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+6);
}


void m68000_ori_di_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_ori_ix_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+10);
}


void m68000_ori_aw_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_ori_al_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_ori_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY |= m68ki_read_imm_16());

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(8);
}


void m68000_ori_ai_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_ori_pi_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_PI_16;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+4);
}


void m68000_ori_pd_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_PD_16;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+6);
}


void m68000_ori_di_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_ori_ix_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+10);
}


void m68000_ori_aw_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+8);
}


void m68000_ori_al_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(12+12);
}


void m68000_ori_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY |= m68ki_read_imm_32());

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(16);
}


void m68000_ori_ai_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+8);
}


void m68000_ori_pi_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_PI_32;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+8);
}


void m68000_ori_pd_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_PD_32;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+10);
}


void m68000_ori_di_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+12);
}


void m68000_ori_ix_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+14);
}


void m68000_ori_aw_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+12);
}


void m68000_ori_al_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_V = 0;
	USE_CLKS(20+16);
}


void m68000_ori_to_ccr(void)
{
	m68ki_set_ccr(m68ki_get_ccr() | m68ki_read_imm_16());
	USE_CLKS(20);
}


void m68000_ori_to_sr(void)
{
	uint or_val = m68ki_read_imm_16();

	if (CPU_S)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(m68ki_get_sr() | or_val);
		USE_CLKS(20);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


void m68020_pack_rr(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = DX + m68ki_read_imm_16();

		DY = MASK_OUT_BELOW_8(DY) | ((src >> 4) & 0x00f0) | (src & 0x000f);
		USE_CLKS(6);
		return;
	}
	m68000_illegal();
}


void m68020_pack_mm_ax7(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_16(AX -= 2);

		src = (((src >> 8) & 0x00ff) | ((src << 8) & 0xff00)) + m68ki_read_imm_16();
		m68ki_write_8(CPU_A[7] -= 2, ((src >> 4) & 0xf0) | (src & 0x0f));
		USE_CLKS(13);
		return;
	}
	m68000_illegal();
}


void m68020_pack_mm_ay7(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_32(CPU_A[7] -= 4);

		src = (((src >> 16) & 0x00ff) | ((src << 8) & 0xff00)) + m68ki_read_imm_16();
		/* I hate the way Motorola changes where Rx and Ry are */
		m68ki_write_8(--AY, ((src >> 4) & 0xf0) | (src & 0x0f));
		USE_CLKS(13);
		return;
	}
	m68000_illegal();
}


void m68020_pack_mm_axy7(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_32(CPU_A[7] -= 4);

		src = (((src >> 16) & 0x00ff) | ((src << 8) & 0xff00)) + m68ki_read_imm_16();
		m68ki_write_8(CPU_A[7] -= 2, ((src >> 4) & 0xf0) | (src & 0x0f));
		USE_CLKS(13);
		return;
	}
	m68000_illegal();
}


void m68020_pack_mm(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_16(AX -= 2);

		src = (((src >> 8) & 0x00ff) | ((src << 8) & 0xff00)) + m68ki_read_imm_16();
		m68ki_write_8(--AY, ((src >> 4) & 0xf0) | (src & 0x0f));
		USE_CLKS(13);
		return;
	}
	m68000_illegal();
}


void m68000_pea_ai(void)
{
	uint ea = EA_AI;

	m68ki_push_32(ea);
	USE_CLKS(0+12);
}


void m68000_pea_di(void)
{
	uint ea = EA_DI;

	m68ki_push_32(ea);
	USE_CLKS(0+16);
}


void m68000_pea_ix(void)
{
	uint ea = EA_IX;

	m68ki_push_32(ea);
	USE_CLKS(0+20);
}


void m68000_pea_aw(void)
{
	uint ea = EA_AW;

	m68ki_push_32(ea);
	USE_CLKS(0+16);
}


void m68000_pea_al(void)
{
	uint ea = EA_AL;

	m68ki_push_32(ea);
	USE_CLKS(0+20);
}


void m68000_pea_pcdi(void)
{
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));

	m68ki_push_32(ea);
	USE_CLKS(0+16);
}


void m68000_pea_pcix(void)
{
	uint ea = EA_PCIX;

	m68ki_push_32(ea);
	USE_CLKS(0+20);
}


void m68000_rst(void)
{
	if (CPU_S)
	{
		m68ki_output_reset();		   /* auto-disable (see m68kcpu.h) */
		USE_CLKS(132);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


void m68000_ror_s_8(void)
{
	uint *d_dst = &DY;
	uint orig_shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = ROR_8(src, shift);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = (src >> ((shift - 1) & 7)) & 1;
	CPU_V = 0;
	USE_CLKS((orig_shift << 1) + 6);
}


void m68000_ror_s_16(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = ROR_16(src, shift);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = (src >> (shift - 1)) & 1;
	CPU_V = 0;
	USE_CLKS((shift << 1) + 6);
}


void m68000_ror_s_32(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = ROR_32(src, shift);

	*d_dst = res;

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = (src >> (shift - 1)) & 1;
	CPU_V = 0;
	USE_CLKS((shift << 1) + 8);
}


void m68000_ror_r_8(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = ROR_8(src, shift);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;
		CPU_C = (src >> ((shift - 1) & 7)) & 1;
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = 0;
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_ror_r_16(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 15;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = ROR_16(src, shift);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;
		CPU_C = (src >> ((shift - 1) & 15)) & 1;
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = 0;
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_ror_r_32(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 31;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = ROR_32(src, shift);

	USE_CLKS((orig_shift << 1) + 8);
	if (orig_shift != 0)
	{
		*d_dst = res;
		CPU_C = (src >> ((shift - 1) & 31)) & 1;
		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = 0;
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_ror_ea_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = src & 1;
	CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_ror_ea_pi(void)
{
	uint ea = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = src & 1;
	CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_ror_ea_pd(void)
{
	uint ea = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = src & 1;
	CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_ror_ea_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = src & 1;
	CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_ror_ea_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = src & 1;
	CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_ror_ea_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = src & 1;
	CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_ror_ea_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = src & 1;
	CPU_V = 0;
	USE_CLKS(8+12);
}


void m68000_rol_s_8(void)
{
	uint *d_dst = &DY;
	uint orig_shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = MASK_OUT_ABOVE_8(ROL_8(src, shift));

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = (src >> (8 - orig_shift)) & 1;
	CPU_V = 0;
	USE_CLKS((orig_shift << 1) + 6);
}


void m68000_rol_s_16(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, shift));

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = (src >> (16 - shift)) & 1;
	CPU_V = 0;
	USE_CLKS((shift << 1) + 6);
}


void m68000_rol_s_32(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = MASK_OUT_ABOVE_32(ROL_32(src, shift));

	*d_dst = res;

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_C = (src >> (32 - shift)) & 1;
	CPU_V = 0;
	USE_CLKS((shift << 1) + 8);
}


void m68000_rol_r_8(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = MASK_OUT_ABOVE_8(ROL_8(src, shift));

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		if (shift != 0)
		{
			*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;
			CPU_C = (src >> (8 - shift)) & 1;
			CPU_N = GET_MSB_8(res);
			CPU_NOT_Z = res;
			CPU_V = 0;
			return;
		}
		CPU_C = src & 1;
		CPU_N = GET_MSB_8(src);
		CPU_NOT_Z = src;
		CPU_V = 0;
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_rol_r_16(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 15;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, shift));

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		if (shift != 0)
		{
			*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;
			CPU_C = (src >> (16 - shift)) & 1;
			CPU_N = GET_MSB_16(res);
			CPU_NOT_Z = res;
			CPU_V = 0;
			return;
		}
		CPU_C = src & 1;
		CPU_N = GET_MSB_16(src);
		CPU_NOT_Z = src;
		CPU_V = 0;
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_rol_r_32(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 31;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = MASK_OUT_ABOVE_32(ROL_32(src, shift));

	USE_CLKS((orig_shift << 1) + 8);
	if (orig_shift != 0)
	{
		*d_dst = res;

		CPU_C = (src >> (32 - shift)) & 1;
		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = 0;
		return;
	}

	CPU_C = 0;
	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_rol_ea_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = GET_MSB_16(src);
	CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_rol_ea_pi(void)
{
	uint ea = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = GET_MSB_16(src);
	CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_rol_ea_pd(void)
{
	uint ea = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = GET_MSB_16(src);
	CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_rol_ea_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = GET_MSB_16(src);
	CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_rol_ea_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = GET_MSB_16(src);
	CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_rol_ea_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = GET_MSB_16(src);
	CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_rol_ea_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = GET_MSB_16(src);
	CPU_V = 0;
	USE_CLKS(8+12);
}


void m68000_roxr_s_8(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint tmp = ROR_9(src | ((CPU_X != 0) << 8), shift);
	uint res = MASK_OUT_ABOVE_8(tmp);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_9(tmp);
	CPU_V = 0;
	USE_CLKS((shift << 1) + 6);
}


void m68000_roxr_s_16(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint tmp = ROR_17(src | ((CPU_X != 0) << 16), shift);
	uint res = MASK_OUT_ABOVE_16(tmp);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS((shift << 1) + 6);
}


void m68000_roxr_s_32(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = MASK_OUT_ABOVE_32((ROR_33(src, shift) & ~(1 << (32 - shift))) | ((CPU_X != 0) << (32 - shift)));
	uint new_x_flag = src & (1 << (shift - 1));

	*d_dst = res;

	CPU_C = CPU_X = new_x_flag;
	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = 0;

	USE_CLKS((shift << 1) + 8);
}


void m68000_roxr_r_8(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 9;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint tmp = ROR_9(src | ((CPU_X != 0) << 8), shift);
	uint res = MASK_OUT_ABOVE_8(tmp);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;
		CPU_C = CPU_X = GET_MSB_9(tmp);
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = 0;
		return;
	}

	CPU_C = CPU_X;
	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_roxr_r_16(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 17;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint tmp = ROR_17(src | ((CPU_X != 0) << 16), shift);
	uint res = MASK_OUT_ABOVE_16(tmp);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;
		CPU_C = CPU_X = GET_MSB_17(tmp);
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = 0;
		return;
	}

	CPU_C = CPU_X;
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_roxr_r_32(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 33;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = MASK_OUT_ABOVE_32((ROR_33(src, shift) & ~(1 << (32 - shift))) | ((CPU_X != 0) << (32 - shift)));
	uint new_x_flag = src & (1 << (shift - 1));

	USE_CLKS((orig_shift << 1) + 8);
	if (shift != 0)
	{
		*d_dst = res;
		CPU_X = new_x_flag;
	}
	else
		res = src;
	CPU_C = CPU_X;
	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_roxr_ea_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_roxr_ea_pi(void)
{
	uint ea = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_roxr_ea_pd(void)
{
	uint ea = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_roxr_ea_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_roxr_ea_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_roxr_ea_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_roxr_ea_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+12);
}


void m68000_roxl_s_8(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint tmp = ROL_9(src | ((CPU_X != 0) << 8), shift);
	uint res = MASK_OUT_ABOVE_8(tmp);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_9(tmp);
	CPU_V = 0;
	USE_CLKS((shift << 1) + 6);
}


void m68000_roxl_s_16(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint tmp = ROL_17(src | ((CPU_X != 0) << 16), shift);
	uint res = MASK_OUT_ABOVE_16(tmp);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS((shift << 1) + 6);
}


void m68000_roxl_s_32(void)
{
	uint *d_dst = &DY;
	uint shift = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = MASK_OUT_ABOVE_32((ROL_33(src, shift) & ~(1 << (shift - 1))) | ((CPU_X != 0) << (shift - 1)));
	uint new_x_flag = src & (1 << (32 - shift));

	*d_dst = res;

	CPU_C = CPU_X = new_x_flag;
	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = 0;

	USE_CLKS((shift << 1) + 6);
}


void m68000_roxl_r_8(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 9;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint tmp = ROL_9(src | ((CPU_X != 0) << 8), shift);
	uint res = MASK_OUT_ABOVE_8(tmp);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;
		CPU_C = CPU_X = GET_MSB_9(tmp);
		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = 0;
		return;
	}

	CPU_C = CPU_X;
	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_roxl_r_16(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 17;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint tmp = ROL_17(src | ((CPU_X != 0) << 16), shift);
	uint res = MASK_OUT_ABOVE_16(tmp);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;
		CPU_C = CPU_X = GET_MSB_17(tmp);
		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = 0;
		return;
	}

	CPU_C = CPU_X;
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_roxl_r_32(void)
{
	uint *d_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 33;
	uint src = MASK_OUT_ABOVE_32(*d_dst);
	uint res = MASK_OUT_ABOVE_32((ROL_33(src, shift) & ~(1 << (shift - 1))) | ((CPU_X != 0) << (shift - 1)));
	uint new_x_flag = src & (1 << (32 - shift));

	USE_CLKS((orig_shift << 1) + 8);
	if (shift != 0)
	{
		*d_dst = res;
		CPU_X = new_x_flag;
	}
	else
		res = src;
	CPU_C = CPU_X;
	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = 0;
}


void m68000_roxl_ea_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_roxl_ea_pi(void)
{
	uint ea = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+4);
}


void m68000_roxl_ea_pd(void)
{
	uint ea = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+6);
}


void m68000_roxl_ea_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_roxl_ea_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+10);
}


void m68000_roxl_ea_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+8);
}


void m68000_roxl_ea_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((CPU_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_C = CPU_X = GET_MSB_17(tmp);
	CPU_V = 0;
	USE_CLKS(8+12);
}


void m68010_rtd(void)
{
	if (CPU_MODE & CPU_MODE_010_PLUS)
	{
		uint new_pc = m68ki_pull_32();

		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_A[7] += MAKE_INT_16(m68ki_read_imm_16());
		m68ki_branch_long(new_pc);
		USE_CLKS(16);
		return;
	}
	m68000_illegal();
}


void m68000_rte(void)
{
	uint new_sr;
	uint new_pc;
	uint format_word;

	if (CPU_S)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		new_sr = m68ki_pull_16();
		new_pc = m68ki_pull_32();
		m68ki_branch_long(new_pc);
		m68ki_set_sr(new_sr);
		if (!(CPU_MODE & CPU_MODE_010_PLUS))
		{
			USE_CLKS(20);
			return;
		}
		format_word = (m68ki_pull_16() >> 12) & 0xf;
		/* I'm ignoring code 8 (bus error and address error) */
        if (format_word != 0)
			/* Generate a new program counter from the format error vector */
			m68ki_set_pc(m68ki_read_32((EXCEPTION_FORMAT_ERROR<<2)+CPU_VBR));
		USE_CLKS(20);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


void m68020_rtm(void)
{
	if (CPU_MODE & CPU_MODE_020)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		M68K_DO_LOG((M68K_LOG, "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68k_cpu_names[CPU_MODE], ADDRESS_68K(CPU_PC - 2), CPU_IR,
					 m68k_disassemble_quick(ADDRESS_68K(CPU_PC - 2))));
		USE_CLKS(19);
		return;
	}
	m68000_illegal();
}


void m68000_rtr(void)
{
	m68ki_add_trace();				   /* auto-disable (see m68kcpu.h) */
	m68ki_set_ccr(m68ki_pull_16());
	m68ki_branch_long(m68ki_pull_32());
	USE_CLKS(20);
}


void m68000_rts(void)
{
	m68ki_add_trace();				   /* auto-disable (see m68kcpu.h) */
	m68ki_branch_long(m68ki_pull_32());
	USE_CLKS(16);
}


void m68000_sbcd_rr(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (CPU_X != 0);

	if (res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res += 0xa0;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | MASK_OUT_ABOVE_8(res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(6);
}


void m68000_sbcd_mm_ax7(void)
{
	uint src = m68ki_read_8(--(AY));
	uint ea = CPU_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (CPU_X != 0);

	if (res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res += 0xa0;

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(18);
}


void m68000_sbcd_mm_ay7(void)
{
	uint src = m68ki_read_8(CPU_A[7] -= 2);
	uint ea = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (CPU_X != 0);

	if (res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res += 0xa0;

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(18);
}


void m68000_sbcd_mm_axy7(void)
{
	uint src = m68ki_read_8(CPU_A[7] -= 2);
	uint ea = CPU_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (CPU_X != 0);

	if (res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res += 0xa0;

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(18);
}


void m68000_sbcd_mm(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (CPU_X != 0);

	if (res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((CPU_X = CPU_C = res > 0x99) != 0)
		res += 0xa0;

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		CPU_NOT_Z = 1;
	USE_CLKS(18);
}


void m68000_st_d(void)
{
	DY |= 0xff;
	USE_CLKS(6);
}


void m68000_st_ai(void)
{
	m68ki_write_8(EA_AI, 0xff);
	USE_CLKS(8+4);
}


void m68000_st_pi(void)
{
	m68ki_write_8(EA_PI_8, 0xff);
	USE_CLKS(8+4);
}


void m68000_st_pi7(void)
{
	m68ki_write_8(EA_PI7_8, 0xff);
	USE_CLKS(8+4);
}


void m68000_st_pd(void)
{
	m68ki_write_8(EA_PD_8, 0xff);
	USE_CLKS(8+6);
}


void m68000_st_pd7(void)
{
	m68ki_write_8(EA_PD7_8, 0xff);
	USE_CLKS(8+6);
}


void m68000_st_di(void)
{
	m68ki_write_8(EA_DI, 0xff);
	USE_CLKS(8+8);
}


void m68000_st_ix(void)
{
	m68ki_write_8(EA_IX, 0xff);
	USE_CLKS(8+10);
}


void m68000_st_aw(void)
{
	m68ki_write_8(EA_AW, 0xff);
	USE_CLKS(8+8);
}


void m68000_st_al(void)
{
	m68ki_write_8(EA_AL, 0xff);
	USE_CLKS(8+12);
}


void m68000_sf_d(void)
{
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_sf_ai(void)
{
	m68ki_write_8(EA_AI, 0);
	USE_CLKS(8+4);
}


void m68000_sf_pi(void)
{
	m68ki_write_8(EA_PI_8, 0);
	USE_CLKS(8+4);
}


void m68000_sf_pi7(void)
{
	m68ki_write_8(EA_PI7_8, 0);
	USE_CLKS(8+4);
}


void m68000_sf_pd(void)
{
	m68ki_write_8(EA_PD_8, 0);
	USE_CLKS(8+6);
}


void m68000_sf_pd7(void)
{
	m68ki_write_8(EA_PD7_8, 0);
	USE_CLKS(8+6);
}


void m68000_sf_di(void)
{
	m68ki_write_8(EA_DI, 0);
	USE_CLKS(8+8);
}


void m68000_sf_ix(void)
{
	m68ki_write_8(EA_IX, 0);
	USE_CLKS(8+10);
}


void m68000_sf_aw(void)
{
	m68ki_write_8(EA_AW, 0);
	USE_CLKS(8+8);
}


void m68000_sf_al(void)
{
	m68ki_write_8(EA_AL, 0);
	USE_CLKS(8+12);
}


void m68000_shi_d(void)
{
	if (CONDITION_HI)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_shi_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_shi_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_shi_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_shi_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_shi_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_shi_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_shi_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_shi_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_shi_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_sls_d(void)
{
	if (CONDITION_LS)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_sls_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sls_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sls_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sls_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sls_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sls_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sls_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_sls_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sls_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_scc_d(void)
{
	if (CONDITION_CC)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_scc_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_scc_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_scc_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_scc_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_scc_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_scc_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_scc_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_scc_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_scc_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_scs_d(void)
{
	if (CONDITION_CS)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_scs_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_scs_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_scs_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_scs_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_scs_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_scs_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_scs_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_scs_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_scs_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_sne_d(void)
{
	if (CONDITION_NE)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_sne_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sne_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sne_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sne_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sne_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sne_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sne_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_sne_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sne_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_seq_d(void)
{
	if (CONDITION_EQ)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_seq_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_seq_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_seq_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_seq_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_seq_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_seq_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_seq_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_seq_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_seq_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_svc_d(void)
{
	if (CONDITION_VC)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_svc_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_svc_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_svc_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_svc_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_svc_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_svc_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_svc_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_svc_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_svc_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_svs_d(void)
{
	if (CONDITION_VS)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_svs_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_svs_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_svs_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_svs_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_svs_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_svs_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_svs_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_svs_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_svs_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_spl_d(void)
{
	if (CONDITION_PL)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_spl_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_spl_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_spl_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_spl_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_spl_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_spl_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_spl_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_spl_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_spl_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_smi_d(void)
{
	if (CONDITION_MI)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_smi_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_smi_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_smi_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_smi_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_smi_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_smi_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_smi_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_smi_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_smi_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_sge_d(void)
{
	if (CONDITION_GE)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_sge_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sge_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sge_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sge_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sge_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sge_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sge_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_sge_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sge_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_slt_d(void)
{
	if (CONDITION_LT)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_slt_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_slt_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_slt_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_slt_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_slt_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_slt_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_slt_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_slt_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_slt_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_sgt_d(void)
{
	if (CONDITION_GT)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_sgt_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sgt_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sgt_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sgt_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sgt_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sgt_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sgt_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_sgt_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sgt_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_sle_d(void)
{
	if (CONDITION_LE)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


void m68000_sle_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sle_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sle_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+4);
}


void m68000_sle_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sle_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+6);
}


void m68000_sle_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sle_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+10);
}


void m68000_sle_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+8);
}


void m68000_sle_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+12);
}


void m68000_stop(void)
{
	uint new_sr = m68ki_read_imm_16();

	if (CPU_S)
	{
		m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
		CPU_STOPPED = 1;
		m68ki_set_sr(new_sr);
		m68k_clks_left = 0;
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


void m68000_sub_er_d_8(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4);
}


void m68000_sub_er_ai_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_AI);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_sub_er_pi_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PI_8);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_sub_er_pi7_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PI7_8);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_sub_er_pd_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PD_8);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+6);
}


void m68000_sub_er_pd7_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PD7_8);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+6);
}


void m68000_sub_er_di_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_DI);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_sub_er_ix_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_IX);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_sub_er_aw_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_AW);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_sub_er_al_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_AL);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+12);
}


void m68000_sub_er_pcdi_8(void)
{
	uint *d_dst = &DX;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_8(ea);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_sub_er_pcix_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_8(EA_PCIX);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_sub_er_i_8(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_imm_8();
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_sub_er_d_16(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_sub_er_a_16(void)
{
	uint *d_dst = &DX;
	uint src = AY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_sub_er_ai_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_AI);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_sub_er_pi_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_PI_16);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_sub_er_pd_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_PD_16);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+6);
}


void m68000_sub_er_di_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_DI);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_sub_er_ix_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_IX);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_sub_er_aw_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_AW);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_sub_er_al_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_AL);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+12);
}


void m68000_sub_er_pcdi_16(void)
{
	uint *d_dst = &DX;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_16(ea);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


void m68000_sub_er_pcix_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_PCIX);
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+10);
}


void m68000_sub_er_i_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_imm_16();
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


void m68000_sub_er_d_32(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(8);
}


void m68000_sub_er_a_32(void)
{
	uint *d_dst = &DX;
	uint src = AY;
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(8);
}


void m68000_sub_er_ai_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_AI);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_sub_er_pi_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_PI_32);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


void m68000_sub_er_pd_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_PD_32);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


void m68000_sub_er_di_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_DI);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_sub_er_ix_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_IX);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


void m68000_sub_er_aw_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_AW);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_sub_er_al_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_AL);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+16);
}


void m68000_sub_er_pcdi_32(void)
{
	uint *d_dst = &DX;
	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_32(ea);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


void m68000_sub_er_pcix_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_32(EA_PCIX);
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


void m68000_sub_er_i_32(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_imm_32();
	uint dst = *d_dst;
	uint res = *d_dst = MASK_OUT_ABOVE_32(dst - src);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


void m68000_sub_re_ai_8(void)
{
	uint ea = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_sub_re_pi_8(void)
{
	uint ea = EA_PI_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_sub_re_pi7_8(void)
{
	uint ea = EA_PI7_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_sub_re_pd_8(void)
{
	uint ea = EA_PD_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_sub_re_pd7_8(void)
{
	uint ea = EA_PD7_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_sub_re_di_8(void)
{
	uint ea = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_sub_re_ix_8(void)
{
	uint ea = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_sub_re_aw_8(void)
{
	uint ea = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_sub_re_al_8(void)
{
	uint ea = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+12);
}


void m68000_sub_re_ai_16(void)
{
	uint ea = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_sub_re_pi_16(void)
{
	uint ea = EA_PI_16;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_sub_re_pd_16(void)
{
	uint ea = EA_PD_16;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_sub_re_di_16(void)
{
	uint ea = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_sub_re_ix_16(void)
{
	uint ea = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_sub_re_aw_16(void)
{
	uint ea = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_sub_re_al_16(void)
{
	uint ea = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+12);
}


void m68000_sub_re_ai_32(void)
{
	uint ea = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_sub_re_pi_32(void)
{
	uint ea = EA_PI_32;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_sub_re_pd_32(void)
{
	uint ea = EA_PD_32;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+10);
}


void m68000_sub_re_di_32(void)
{
	uint ea = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_sub_re_ix_32(void)
{
	uint ea = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+14);
}


void m68000_sub_re_aw_32(void)
{
	uint ea = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_sub_re_al_32(void)
{
	uint ea = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+16);
}


void m68000_suba_d_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(DY));
	USE_CLKS(8);
}


void m68000_suba_a_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(AY));
	USE_CLKS(8);
}


void m68000_suba_ai_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_AI)));
	USE_CLKS(8+4);
}


void m68000_suba_pi_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_PI_16)));
	USE_CLKS(8+4);
}


void m68000_suba_pd_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_PD_16)));
	USE_CLKS(8+6);
}


void m68000_suba_di_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_DI)));
	USE_CLKS(8+8);
}


void m68000_suba_ix_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_IX)));
	USE_CLKS(8+10);
}


void m68000_suba_aw_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_AW)));
	USE_CLKS(8+8);
}


void m68000_suba_al_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_AL)));
	USE_CLKS(8+12);
}


void m68000_suba_pcdi_16(void)
{
	uint *a_dst = &AX;

	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(ea)));
	USE_CLKS(8+8);
}


void m68000_suba_pcix_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_PCIX)));
	USE_CLKS(8+10);
}


void m68000_suba_i_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_imm_16()));
	USE_CLKS(8+4);
}


void m68000_suba_d_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - DY);
	USE_CLKS(8);
}


void m68000_suba_a_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - AY);
	USE_CLKS(8);
}


void m68000_suba_ai_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_AI));
	USE_CLKS(6+8);
}


void m68000_suba_pi_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_PI_32));
	USE_CLKS(6+8);
}


void m68000_suba_pd_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_PD_32));
	USE_CLKS(6+10);
}


void m68000_suba_di_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_DI));
	USE_CLKS(6+12);
}


void m68000_suba_ix_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_IX));
	USE_CLKS(6+14);
}


void m68000_suba_aw_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_AW));
	USE_CLKS(6+12);
}


void m68000_suba_al_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_AL));
	USE_CLKS(6+16);
}


void m68000_suba_pcdi_32(void)
{
	uint *a_dst = &AX;

	uint old_pc = (CPU_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(ea));
	USE_CLKS(6+12);
}


void m68000_suba_pcix_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_PCIX));
	USE_CLKS(6+14);
}


void m68000_suba_i_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_imm_32());
	USE_CLKS(6+10);
}


void m68000_subi_d_8(void)
{
	uint *d_dst = &DY;
	uint src = m68ki_read_imm_8();
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8);
}


void m68000_subi_ai_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_subi_pi_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_subi_pi7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_subi_pd_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+6);
}


void m68000_subi_pd7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+6);
}


void m68000_subi_di_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_subi_ix_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+10);
}


void m68000_subi_aw_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_subi_al_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_subi_d_16(void)
{
	uint *d_dst = &DY;
	uint src = m68ki_read_imm_16();
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8);
}


void m68000_subi_ai_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_subi_pi_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+4);
}


void m68000_subi_pd_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+6);
}


void m68000_subi_di_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_subi_ix_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+10);
}


void m68000_subi_aw_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_subi_al_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_subi_d_32(void)
{
	uint *d_dst = &DY;
	uint src = m68ki_read_imm_32();
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	*d_dst = res;

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(16);
}


void m68000_subi_ai_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+8);
}


void m68000_subi_pi_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+8);
}


void m68000_subi_pd_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+10);
}


void m68000_subi_di_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+12);
}


void m68000_subi_ix_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+14);
}


void m68000_subi_aw_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+12);
}


void m68000_subi_al_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+16);
}


void m68000_subq_d_8(void)
{
	uint *d_dst = &DY;
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4);
}


void m68000_subq_ai_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_subq_pi_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_subq_pi7_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_subq_pd_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_subq_pd7_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_subq_di_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_subq_ix_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_subq_aw_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_subq_al_8(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+12);
}


void m68000_subq_d_16(void)
{
	uint *d_dst = &DY;
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_subq_a_16(void)
{
	uint *a_dst = &AY;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - ((((CPU_IR >> 9) - 1) & 7) + 1));
	USE_CLKS(8);
}


void m68000_subq_ai_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_subq_pi_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


void m68000_subq_pd_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+6);
}


void m68000_subq_di_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_subq_ix_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+10);
}


void m68000_subq_aw_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


void m68000_subq_al_16(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+12);
}


void m68000_subq_d_32(void)
{
	uint *d_dst = &DY;
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	*d_dst = res;

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(8);
}


void m68000_subq_a_32(void)
{
	uint *a_dst = &AY;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - ((((CPU_IR >> 9) - 1) & 7) + 1));
	USE_CLKS(8);
}


void m68000_subq_ai_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_subq_pi_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


void m68000_subq_pd_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+10);
}


void m68000_subq_di_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_subq_ix_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+14);
}


void m68000_subq_aw_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


void m68000_subq_al_32(void)
{
	uint src = (((CPU_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+16);
}


void m68000_subx_rr_8(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src - (CPU_X != 0));

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4);
}


void m68000_subx_rr_16(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src - (CPU_X != 0));

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


void m68000_subx_rr_32(void)
{
	uint *d_dst = &DX;
	uint src = DY;
	uint dst = *d_dst;
	uint res = MASK_OUT_ABOVE_32(dst - src - (CPU_X != 0));

	*d_dst = res;

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(8);
}


void m68000_subx_mm_8_ax7(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea = CPU_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(18);
}


void m68000_subx_mm_8_ay7(void)
{
	uint src = m68ki_read_8(CPU_A[7] -= 2);
	uint ea = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(18);
}


void m68000_subx_mm_8_axy7(void)
{
	uint src = m68ki_read_8(CPU_A[7] -= 2);
	uint ea = CPU_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(18);
}


void m68000_subx_mm_8(void)
{
	uint src = m68ki_read_8(--(AY));
	uint ea = --(AX);
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src - (CPU_X != 0));

	m68ki_write_8(ea, res);

	CPU_N = GET_MSB_8(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_X = CPU_C = CFLAG_SUB_8(src, dst, res);
	CPU_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(18);
}


void m68000_subx_mm_16(void)
{
	uint src = m68ki_read_16(AY -= 2);
	uint ea = (AX -= 2);
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src - (CPU_X != 0));

	m68ki_write_16(ea, res);

	CPU_N = GET_MSB_16(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_X = CPU_C = CFLAG_SUB_16(src, dst, res);
	CPU_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(18);
}


void m68000_subx_mm_32(void)
{
	uint src = m68ki_read_32(AY -= 4);
	uint ea = (AX -= 4);
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src - (CPU_X != 0));

	m68ki_write_32(ea, res);

	CPU_N = GET_MSB_32(res);
	if (res != 0)
		CPU_NOT_Z = 1;
	CPU_X = CPU_C = CFLAG_SUB_32(src, dst, res);
	CPU_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(30);
}


void m68000_swap(void)
{
	uint *d_dst = &DY;

	*d_dst ^= (*d_dst >> 16) & 0x0000ffff;
	*d_dst ^= (*d_dst << 16) & 0xffff0000;
	*d_dst ^= (*d_dst >> 16) & 0x0000ffff;

	CPU_N = GET_MSB_32(*d_dst);
	CPU_NOT_Z = *d_dst;
	CPU_C = CPU_V = 0;
	USE_CLKS(4);
}


void m68000_tas_d(void)
{
	uint *d_dst = &DY;

	CPU_NOT_Z = MASK_OUT_ABOVE_8(*d_dst);
	CPU_N = GET_MSB_8(*d_dst);
	CPU_V = CPU_C = 0;
	*d_dst |= 0x80;
	USE_CLKS(4);
}


void m68000_tas_ai(void)
{
	uint ea = EA_AI;
	uint dst = m68ki_read_8(ea);

	CPU_NOT_Z = dst;
	CPU_N = GET_MSB_8(dst);
	CPU_V = CPU_C = 0;
	m68ki_write_8(ea, dst | 0x80);
	USE_CLKS(14+4);
}


void m68000_tas_pi(void)
{
	uint ea = EA_PI_8;
	uint dst = m68ki_read_8(ea);

	CPU_NOT_Z = dst;
	CPU_N = GET_MSB_8(dst);
	CPU_V = CPU_C = 0;
	m68ki_write_8(ea, dst | 0x80);
	USE_CLKS(14+4);
}


void m68000_tas_pi7(void)
{
	uint ea = EA_PI7_8;
	uint dst = m68ki_read_8(ea);

	CPU_NOT_Z = dst;
	CPU_N = GET_MSB_8(dst);
	CPU_V = CPU_C = 0;
	m68ki_write_8(ea, dst | 0x80);
	USE_CLKS(14+4);
}


void m68000_tas_pd(void)
{
	uint ea = EA_PD_8;
	uint dst = m68ki_read_8(ea);

	CPU_NOT_Z = dst;
	CPU_N = GET_MSB_8(dst);
	CPU_V = CPU_C = 0;
	m68ki_write_8(ea, dst | 0x80);
	USE_CLKS(14+6);
}


void m68000_tas_pd7(void)
{
	uint ea = EA_PD7_8;
	uint dst = m68ki_read_8(ea);

	CPU_NOT_Z = dst;
	CPU_N = GET_MSB_8(dst);
	CPU_V = CPU_C = 0;
	m68ki_write_8(ea, dst | 0x80);
	USE_CLKS(14+6);
}


void m68000_tas_di(void)
{
	uint ea = EA_DI;
	uint dst = m68ki_read_8(ea);

	CPU_NOT_Z = dst;
	CPU_N = GET_MSB_8(dst);
	CPU_V = CPU_C = 0;
	m68ki_write_8(ea, dst | 0x80);
	USE_CLKS(14+8);
}


void m68000_tas_ix(void)
{
	uint ea = EA_IX;
	uint dst = m68ki_read_8(ea);

	CPU_NOT_Z = dst;
	CPU_N = GET_MSB_8(dst);
	CPU_V = CPU_C = 0;
	m68ki_write_8(ea, dst | 0x80);
	USE_CLKS(14+10);
}


void m68000_tas_aw(void)
{
	uint ea = EA_AW;
	uint dst = m68ki_read_8(ea);

	CPU_NOT_Z = dst;
	CPU_N = GET_MSB_8(dst);
	CPU_V = CPU_C = 0;
	m68ki_write_8(ea, dst | 0x80);
	USE_CLKS(14+8);
}


void m68000_tas_al(void)
{
	uint ea = EA_AL;
	uint dst = m68ki_read_8(ea);

	CPU_NOT_Z = dst;
	CPU_N = GET_MSB_8(dst);
	CPU_V = CPU_C = 0;
	m68ki_write_8(ea, dst | 0x80);
	USE_CLKS(14+12);
}


void m68000_trap(void)
{
	m68ki_interrupt(EXCEPTION_TRAP_BASE + (CPU_IR & 0xf));	/* HJB 990403 */
}


void m68020_trapt_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68000_illegal();
}


void m68020_trapt_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68000_illegal();
}


void m68020_trapt_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68000_illegal();
}


void m68020_trapf_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapf_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapf_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_traphi_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_HI)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_traphi_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_HI)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_traphi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_HI)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapls_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LS)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapls_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LS)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapls_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LS)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapcc_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_CC)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapcc_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_CC)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapcc_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_CC)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapcs_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_CS)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapcs_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_CS)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapcs_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_CS)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapne_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_NE)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapne_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_NE)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapne_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_NE)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapeq_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_EQ)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapeq_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_EQ)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapeq_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_EQ)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapvc_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_VC)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapvc_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_VC)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapvc_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_VC)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapvs_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_VS)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapvs_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_VS)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapvs_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_VS)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trappl_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_PL)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trappl_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_PL)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trappl_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_PL)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapmi_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_MI)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapmi_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_MI)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapmi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_MI)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapge_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_GE)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapge_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_GE)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapge_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_GE)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_traplt_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LT)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_traplt_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LT)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_traplt_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LT)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_trapgt_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_GT)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_trapgt_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_GT)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_trapgt_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_GT)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68020_traple_0(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LE)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_traple_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LE)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(6);
		CPU_PC += 2;
		return;
	}
	m68000_illegal();
}


void m68020_traple_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		if (CONDITION_LE)
			m68ki_interrupt(EXCEPTION_TRAPV);	/* HJB 990403 */
		else
			USE_CLKS(8);
		CPU_PC += 4;
		return;
	}
	m68000_illegal();
}


void m68000_trapv(void)
{
	if (!CPU_V)
	{
		USE_CLKS(4);
		return;
	}
	m68ki_interrupt(EXCEPTION_TRAPV);  /* HJB 990403 */
}


void m68000_tst_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4);
}


void m68000_tst_ai_8(void)
{
	uint ea = EA_AI;
	uint res = m68ki_read_8(ea);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+4);
}


void m68000_tst_pi_8(void)
{
	uint ea = EA_PI_8;
	uint res = m68ki_read_8(ea);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+4);
}


void m68000_tst_pi7_8(void)
{
	uint ea = EA_PI7_8;
	uint res = m68ki_read_8(ea);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+4);
}


void m68000_tst_pd_8(void)
{
	uint ea = EA_PD_8;
	uint res = m68ki_read_8(ea);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+6);
}


void m68000_tst_pd7_8(void)
{
	uint ea = EA_PD7_8;
	uint res = m68ki_read_8(ea);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+6);
}


void m68000_tst_di_8(void)
{
	uint ea = EA_DI;
	uint res = m68ki_read_8(ea);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+8);
}


void m68000_tst_ix_8(void)
{
	uint ea = EA_IX;
	uint res = m68ki_read_8(ea);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+10);
}


void m68000_tst_aw_8(void)
{
	uint ea = EA_AW;
	uint res = m68ki_read_8(ea);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+8);
}


void m68000_tst_al_8(void)
{
	uint ea = EA_AL;
	uint res = m68ki_read_8(ea);

	CPU_N = GET_MSB_8(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+12);
}


void m68020_tst_pcdi_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint old_pc = (CPU_PC += 2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		uint res = m68ki_read_8(ea);

		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_tst_pcix_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint res = m68ki_read_8(EA_PCIX);

		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_tst_imm_8(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint res = m68ki_read_imm_8();

		CPU_N = GET_MSB_8(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68000_tst_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4);
}


void m68020_tst_a_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint res = MASK_OUT_ABOVE_16(AY);

		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68000_tst_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+4);
}


void m68000_tst_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+4);
}


void m68000_tst_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+6);
}


void m68000_tst_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+8);
}


void m68000_tst_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+10);
}


void m68000_tst_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+8);
}


void m68000_tst_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);

	CPU_N = GET_MSB_16(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+12);
}


void m68020_tst_pcdi_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint old_pc = (CPU_PC += 2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		uint res = m68ki_read_16(ea);

		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_tst_pcix_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint res = m68ki_read_16(EA_PCIX);

		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_tst_imm_16(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint res = m68ki_read_imm_16();

		CPU_N = GET_MSB_16(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68000_tst_d_32(void)
{
	uint res = DY;

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4);
}


void m68020_tst_a_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint res = AY;

		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68000_tst_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+8);
}


void m68000_tst_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+8);
}


void m68000_tst_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+10);
}


void m68000_tst_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+12);
}


void m68000_tst_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+14);
}


void m68000_tst_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+12);
}


void m68000_tst_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);

	CPU_N = GET_MSB_32(res);
	CPU_NOT_Z = res;
	CPU_V = CPU_C = 0;
	USE_CLKS(4+16);
}


void m68020_tst_pcdi_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint old_pc = (CPU_PC += 2) - 2;
		uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
		uint res = m68ki_read_32(ea);

		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_tst_pcix_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint res = m68ki_read_32(EA_PCIX);

		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68020_tst_imm_32(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint res = m68ki_read_imm_32();

		CPU_N = GET_MSB_32(res);
		CPU_NOT_Z = res;
		CPU_V = CPU_C = 0;
		USE_CLKS(4);
		return;
	}
	m68000_illegal();
}


void m68000_unlk_a7(void)
{
	CPU_A[7] = m68ki_read_32(CPU_A[7]);
	USE_CLKS(12);
}


void m68000_unlk(void)
{
	uint *a_dst = &AY;

	CPU_A[7] = *a_dst;
	*a_dst = m68ki_pull_32();
	USE_CLKS(12);
}


void m68020_unpk_rr(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = DX;

		DY = MASK_OUT_BELOW_16(DY) | (((((src << 4) & 0x0f00) | (src & 0x000f)) + m68ki_read_imm_16()) & 0xffff);
		USE_CLKS(8);
		return;
	}
	m68000_illegal();
}


void m68020_unpk_mm_ax7(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_8(--AX);

		src = (((src << 4) & 0x0f00) | (src & 0x000f)) + m68ki_read_imm_16();
		m68ki_write_8(CPU_A[7] -= 2, (src >> 8) & 0xff);
		m68ki_write_8(CPU_A[7] -= 2, src & 0xff);
		USE_CLKS(13);
		return;
	}
	m68000_illegal();
}


void m68020_unpk_mm_ay7(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_8(CPU_A[7] -= 2);

		src = (((src << 4) & 0x0f00) | (src & 0x000f)) + m68ki_read_imm_16();
		m68ki_write_8(--AY, (src >> 8) & 0xff);
		m68ki_write_8(--AY, src & 0xff);
		USE_CLKS(13);
		return;
	}
	m68000_illegal();
}


void m68020_unpk_mm_axy7(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_8(CPU_A[7] -= 2);

		src = (((src << 4) & 0x0f00) | (src & 0x000f)) + m68ki_read_imm_16();
		m68ki_write_8(CPU_A[7] -= 2, (src >> 8) & 0xff);
		m68ki_write_8(CPU_A[7] -= 2, src & 0xff);
		USE_CLKS(13);
		return;
	}
	m68000_illegal();
}


void m68020_unpk_mm(void)
{
	if (CPU_MODE & CPU_MODE_020_PLUS)
	{
		uint src = m68ki_read_8(--AX);

		src = (((src << 4) & 0x0f00) | (src & 0x000f)) + m68ki_read_imm_16();
		m68ki_write_8(--AY, (src >> 8) & 0xff);
		m68ki_write_8(--AY, src & 0xff);
		USE_CLKS(13);
		return;
	}
	m68000_illegal();
}


