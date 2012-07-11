/*
 *  VAX translation
 *
 *  Copyright (c) 2012 Pekka Enberg
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "cpu.h"
#include "disas.h"
#include "tcg-op.h"
#include "qemu-log.h"

#include "helper.h"
#define GEN_HELPER 1
#include "helper.h"

/*
 * Opcodes are defined in Appendix A ("Opcode Assignments") of the "VAX
 * Architecture Reference Manual".
 */
enum {
	/*
	 * One byte opcodes:
	 */

	OPC_HALT		= 0x00,
	OPC_NOP			= 0x01,
	OPC_REI			= 0x02,
	OPC_BPT			= 0x03,
	OPC_RET			= 0x04,
	OPC_RSB			= 0x05,
	OPC_LDPCTX		= 0x06,
	OPC_SVPCTX		= 0x07,

	OPC_CVTPS		= 0x08,
	OPC_CVTSP		= 0x09,
	OPC_INDEX		= 0x0A,
	OPC_CRC			= 0x0B,
	OPC_PROBER		= 0x0C,
	OPC_PROBEW		= 0x0D,
	OPC_INSQUE		= 0x0E,
	OPC_REMQUE		= 0x0F,

	OPC_BSBB		= 0x10,
	OPC_BRB			= 0x11,
	OPC_BNEQ		= 0x12,
	OPC_BEQL		= 0x13,
	OPC_BGTR		= 0x14,
	OPC_BLEQ		= 0x15,
	OPC_JSB			= 0x16,
	OPC_JMP			= 0x17,

	OPC_BGEQ		= 0x18,
	OPC_BLSS		= 0x19,
	OPC_BGTRU		= 0x1A,
	OPC_BLEQU		= 0x1B,
	OPC_BVC			= 0x1C,
	OPC_BVS			= 0x1D,
	OPC_BGEQU		= 0x1E,
	OPC_BLSSU		= 0x1F,

	OPC_ADDP4		= 0x20,
	OPC_ADDP6		= 0x21,
	OPC_SUBP4		= 0x22,
	OPC_SUBP6		= 0x23,
	OPC_CVTPT		= 0x24,
	OPC_MULP		= 0x25,
	OPC_CVTTP		= 0x26,
	OPC_DIVP		= 0x27,

	OPC_MOVC3		= 0x28,
	OPC_CMPC3		= 0x29,
	OPC_SCANC		= 0x2A,
	OPC_SPANC		= 0x2B,
	OPC_MOVC5		= 0x2C,
	OPC_CMPC5		= 0x2D,
	OPC_MOVTC		= 0x2E,
	OPC_MOVTUC		= 0x2F,

	OPC_BSBW		= 0x30,
	OPC_BRW			= 0x31,
	OPC_CVTWL		= 0x32,
	OPC_CVTWB		= 0x33,
	OPC_MOVP		= 0x34,
	OPC_CMPP3		= 0x35,
	OPC_CVTPL		= 0x36,
	OPC_CMPP4		= 0x37,

	OPC_EDITPC		= 0x38,
	OPC_MATCHC		= 0x39,
	OPC_LOCC		= 0x3A,
	OPC_SKPC		= 0x3B,
	OPC_MOVZWL		= 0x3C,
	OPC_ACBW		= 0x3D,
	OPC_MOVAW		= 0x3E,
	OPC_PUSHAW		= 0x3F,

	OPC_ADDF2		= 0x40,
	OPC_ADDF3		= 0x41,
	OPC_SUBF2		= 0x42,
	OPC_SUBF3		= 0x43,
	OPC_MULF2		= 0x44,
	OPC_MULF3		= 0x45,
	OPC_DIVF2		= 0x46,
	OPC_DIVF3		= 0x47,

	OPC_CVTFB		= 0x48,
	OPC_CVTFW		= 0x49,
	OPC_CVTFL		= 0x4A,
	OPC_CVTRFL		= 0x4B,
	OPC_CVTBF		= 0x4C,
	OPC_CVTWF		= 0x4D,
	OPC_CVTLF		= 0x4E,
	OPC_ACBF		= 0x4F,

	OPC_MOVF		= 0x50,
	OPC_CMPF		= 0x51,
	OPC_MNEGF		= 0x52,
	OPC_TSTF		= 0x53,
	OPC_EMODF		= 0x54,
	OPC_POLYF		= 0x55,
	OPC_CVTFD		= 0x56,
	OPC_RESERVED_57		= 0x57,

	OPC_ADAWI		= 0x58,
	OPC_RESERVED_59		= 0x59,
	OPC_RESERVED_5A		= 0x5A,
	OPC_RESERVED_5B		= 0x5B,
	OPC_INSQHI		= 0x5C,
	OPC_INSQTI		= 0x5D,
	OPC_REMQHI		= 0x5E,
	OPC_REMQTI		= 0x5F,

	OPC_ADDD2		= 0x60,
	OPC_ADDD3		= 0x61,
	OPC_SUBD2		= 0x62,
	OPC_SUBD3		= 0x63,
	OPC_MULD2		= 0x64,
	OPC_MULD3		= 0x65,
	OPC_DIVD2		= 0x66,
	OPC_DIVD3		= 0x67,

	OPC_CVTDB		= 0x68,
	OPC_CVTDW		= 0x69,
	OPC_CVTDL		= 0x6A,
	OPC_CVTRDL	 	= 0x6B,
	OPC_CVTBD 	 	= 0x6C,
	OPC_CVTWD		= 0x6D,
	OPC_CVTLD		= 0x6E,
	OPC_ACBD 		= 0x6F,

	OPC_MOVD		= 0x70,
	OPC_CMPD		= 0x71,
	OPC_MNEGD		= 0x72,
	OPC_TSTD		= 0x73,
	OPC_EMODD		= 0x74,
	OPC_POLYD		= 0x75,
	OPC_CVTDF		= 0x76,
	OPC_RESERVED_77		= 0x77,

	OPC_ASHL 		= 0x78,
	OPC_ASHQ 	 	= 0x79,
	OPC_EMUL		= 0x7A,
	OPC_EDIV	 	= 0x7B,
	OPC_CLRQ	 	= 0x7C,
	OPC_MOVQ 		= 0x7D,
	OPC_MOVAQ 		= 0x7E,
	OPC_PUSHAQ 		= 0x7F,

	OPC_ADDB2		= 0x80,
	OPC_ADDB3		= 0x81,
	OPC_SUBB2		= 0x82,
	OPC_SUBB3		= 0x83,
	OPC_MULB2		= 0x84,
	OPC_MULB3		= 0x85,
	OPC_DIVB2		= 0x86,
	OPC_DIVB3		= 0x87,

	OPC_BISB2		= 0x88,
	OPC_BISB3		= 0x89,
	OPC_BICB2		= 0x8A,
	OPC_BICB3		= 0x8B,
	OPC_XORB2		= 0x8C,
	OPC_XORB3		= 0x8D,
	OPC_MNEGB		= 0x8E,
	OPC_CASEB		= 0x8F,

	OPC_MOVB		= 0x90,
	OPC_CMPB		= 0x91,
	OPC_MCOMB		= 0x92,
	OPC_BITB		= 0x93,
	OPC_CLRB		= 0x94,
	OPC_TSTB		= 0x95,
	OPC_INCB		= 0x96,
	OPC_DECB		= 0x97,

	OPC_CVTBL		= 0x98,
	OPC_CVTBW		= 0x99,
	OPC_MOVZBL		= 0x9A,
	OPC_MOVZBW		= 0x9B,
	OPC_ROTL		= 0x9C,
	OPC_ACBB		= 0x9D,
	OPC_MOVAB		= 0x9E,
	OPC_PUSHAB		= 0x9F,

	OPC_ADDW2		= 0xA0,
	OPC_ADDW3		= 0xA1,
	OPC_SUBW2		= 0xA2,
	OPC_SUBW3		= 0xA3,
	OPC_MULW2		= 0xA4,
	OPC_MULW3		= 0xA5,
	OPC_DIVW2		= 0xA6,
	OPC_DIVW3		= 0xA7,

	OPC_BISW2		= 0xA8,
	OPC_BISW3		= 0xA9,
	OPC_BICW2		= 0xAA,
	OPC_BICW3		= 0xAB,
	OPC_XORW2		= 0xAC,
	OPC_XORW3		= 0xAD,
	OPC_MNEGW		= 0xAE,
	OPC_CASEW		= 0xAF,

	OPC_MOVW		= 0xB0,
	OPC_CMPW		= 0xB1,
	OPC_MCOMW		= 0xB2,
	OPC_BITW		= 0xB3,
	OPC_CLRW		= 0xB4,
	OPC_TSTW		= 0xB5,
	OPC_INCW		= 0xB6,
	OPC_DECW		= 0xB7,

	OPC_BISPSW		= 0xB8,
	OPC_BICPSW		= 0xB9,
	OPC_POPR		= 0xBA,
	OPC_PUSHR		= 0xBB,
	OPC_CHMK		= 0xBC,
	OPC_CHME		= 0xBD,
	OPC_CHMS		= 0xBE,
	OPC_CHMU		= 0xBF,

	OPC_ADDL2		= 0xC0,
	OPC_ADDL3		= 0xC1,
	OPC_SUBL2		= 0xC2,
	OPC_SUBL3		= 0xC3,
	OPC_MULL2		= 0xC4,
	OPC_MULL3		= 0xC5,
	OPC_DIVL2		= 0xC6,
	OPC_DIVL3		= 0xC7,

	OPC_BISL2		= 0xC8,
	OPC_BISL3		= 0xC9,
	OPC_BICL2		= 0xCA,
	OPC_BICL3		= 0xCB,
	OPC_XORL2		= 0xCC,
	OPC_XORL3		= 0xCD,
	OPC_MNEGL		= 0xCE,
	OPC_CASEL		= 0xCF,

	OPC_MOVL		= 0xD0,
	OPC_CMPL		= 0xD1,
	OPC_MCOML		= 0xD2,
	OPC_BITL		= 0xD3,
	OPC_CLRL		= 0xD4,
	OPC_TSTL		= 0xD5,
	OPC_INCL		= 0xD6,
	OPC_DECL		= 0xD7,

	OPC_ADWC		= 0xD8,
	OPC_SBWC		= 0xD9,
	OPC_MTPR		= 0xDA,
	OPC_MFPR		= 0xDB,
	OPC_MOVPSL		= 0xDC,
	OPC_PUSHL		= 0xDD,
	OPC_MOVAL		= 0xDE,
	OPC_PUSHAL		= 0xDF,

	OPC_BBS			= 0xE0,
	OPC_BBC			= 0xE1,
	OPC_BBSS		= 0xE2,
	OPC_BBCS		= 0xE3,
	OPC_BBSC		= 0xE4,
	OPC_BBCC		= 0xE5,
	OPC_BBSSI		= 0xE6,
	OPC_BBCCI		= 0xE7,

	OPC_BLBS		= 0xE8,
	OPC_BLBC		= 0xE9,
	OPC_FFS			= 0xEA,
	OPC_FFC			= 0xEB,
	OPC_CMPV		= 0xEC,
	OPC_CMPZV		= 0xED,
	OPC_EXTV		= 0xEE,
	OPC_EXTZV		= 0xEF,

	OPC_INSV		= 0xF0,
	OPC_ACBL		= 0xF1,
	OPC_AOBLSS		= 0xF2,
	OPC_AOBLEQ		= 0xF3,
	OPC_SOBGEQ		= 0xF4,
	OPC_SOBGTR		= 0xF5,
	OPC_CVTLB		= 0xF6,
	OPC_CVTLW		= 0xF7,

	OPC_ASHP		= 0xF8,
	OPC_CVTLP		= 0xF9,
	OPC_CALLG		= 0xFA,
	OPC_CALLS		= 0xFB,
	OPC_XFC			= 0xFC,

	/*
	 * Two byte opcodes:
	 */

	OPC_CVTDH		= 0x32FD,
	OPC_CVTGF		= 0x33FD,

	OPC_ADDG2		= 0x40FD,
	OPC_ADDG3		= 0x41FD,
	OPC_SUBG2		= 0x42FD,
	OPC_SUBG3		= 0x43FD,
	OPC_MULG2		= 0x44FD,
	OPC_MULG3		= 0x45FD,
	OPC_DIVG2		= 0x46FD,
	OPC_DIVG3		= 0x47FD,

	OPC_CVTGB		= 0x48FD,
	OPC_CVTGW		= 0x49FD,
	OPC_CVTGL		= 0x4AFD,
	OPC_CVTRGL		= 0x4BFD,
	OPC_CVTBG		= 0x4CFD,
	OPC_CVTWG		= 0x4DFD,
	OPC_CVTLG		= 0x4EFD,
	OPC_ACBG		= 0x4FFD,

	OPC_MOVG		= 0x50FD,
	OPC_CMPG		= 0x51FD,
	OPC_MNEGG		= 0x52FD,
	OPC_TSTG		= 0x53FD,
	OPC_EMODG		= 0x54FD,
	OPC_POLYG		= 0x55FD,
	OPC_CVTGH		= 0x56FD,

	OPC_ADDH2		= 0x60FD,
	OPC_ADDH3		= 0x61FD,
	OPC_SUBH2		= 0x62FD,
	OPC_SUBH3		= 0x63FD,
	OPC_MULH2		= 0x64FD,
	OPC_MULH3		= 0x65FD,
	OPC_DIVH2		= 0x66FD,
	OPC_DIVH3		= 0x67FD,

	OPC_CVTHB		= 0x68FD,
	OPC_CVTHW		= 0x69FD,
	OPC_CVTHL		= 0x6AFD,
	OPC_CVTHRL		= 0x6BFD,
	OPC_CVTBH		= 0x6CFD,
	OPC_CVTWH		= 0x6DFD,
	OPC_CVTLH		= 0x6EFD,
	OPC_ACBH		= 0x6FFD,

	OPC_MOVH		= 0x70FD,
	OPC_CMPH		= 0x71FD,
	OPC_MNEGH		= 0x72FD,
	OPC_TSTH		= 0x73FD,
	OPC_EMODH		= 0x74FD,
	OPC_POLYH		= 0x75FD,
	OPC_CVTHG		= 0x76FD,

	OPC_CLRO		= 0x7CFD,
	OPC_MOVO		= 0x7DFD,
	OPC_MOVAO		= 0x7EFD,
	OPC_PUSHAO		= 0x7FFD,

	OPC_CVTFG		= 0x99FD,
	OPC_CVTFH		= 0x98FD,

	OPC_CVTHF		= 0xF6FD,
	OPC_CVTHD		= 0xF7FD,

	OPC_BUGL		= 0xFDFF,
	OPC_BUGW		= 0xFEFF,
};

typedef struct DisasContext {
	CPUVAXState		*env;
	 /* Start of the current instruction. */
	target_ulong		insn_pc;
	target_ulong		pc;
	int			is_jmp;
	int			cc_op;
} DisasContext;

/* TODO: Initialize with tcg_global_mem_new() */
static TCGv			cpu_regs[16];

enum {
	TYPE_BYTE,
	TYPE_WORD,
	TYPE_LONG,
	TYPE_QUAD,
	TYPE_OCTA,
	TYPE_F_FLOATING,
	TYPE_D_FLOATING,
	TYPE_G_FLOATING,
	TYPE_H_FLOATING,
};

enum {
	COND_GTR,
	COND_LEQ,
	COND_NEQ,
	COND_EQL,
	COND_GEQ,
	COND_LSS,
	COND_GTRU,
	COND_LEQU,
	COND_BVC,
	COND_BVS,
	COND_EQU,
	COND_LSSU,
};

/*
 * Addressing modes are defined in Table 2.1 ("Summary of General Register
 * Addressing") of the "VAX Architecture Reference Manual".
 */

enum {
	/* General Register Addressing Mode */
	ADDMODE_INDEXED			= 4,
	ADDMODE_REGISTER		= 5,
	ADDMODE_REGISTER_DEFERRED	= 6,
	ADDMODE_AUTODECREMENT		= 7,
	ADDMODE_AUTOINCREMENT		= 8,
	ADDMODE_AUTOINCREMENT_DEFERRED	= 9,
	ADDMODE_BYTE_DISP		= 10,
	ADDMODE_BYTE_DISP_DEFERRED	= 11,
	ADDMODE_WORD_DISP		= 12,
	ADDMODE_WORD_DISP_DEFERRED	= 13,
	ADDMODE_LONGWORD_DISP		= 14,
	ADDMODE_LONGWORD_DISP_DEFERRED	= 15,
};

enum {
	/* Program Counter Addressing Mode */
	ADDMODE_IMMEDIATE			= 8,
	ADDMODE_ABSOLUTE			= 9,
	ADDMODE_BYTE_RELATIVE			= 10,
	ADDMODE_BYTE_RELATIVE_DEFERRED		= 11,
	ADDMODE_WORD_RELATIVE			= 12,
	ADDMODE_WORD_RELATIVE_DEFERRED		= 13,
	ADDMODE_LONGWORD_BYTE_RELATIVE		= 14,
	ADDMODE_LONGWORD_BYTE_RELATIVE_DEFERRED	= 15,
};

#define REG_MASK		0x0F
#define REG_SHIFT		0

#define ADDMODE_MASK		0xF0
#define ADDMODE_SHIFT		4

static inline TCGv disas_vax_operand(DisasContext *ctx, int opsize)
{
	unsigned char operand, addr_mode, reg_num;
	TCGv ret;

	operand = ldub_code(ctx->pc++);

	reg_num = operand & REG_MASK;

	addr_mode = (operand & ADDMODE_MASK) >> ADDMODE_SHIFT;

	switch (addr_mode) {
	case ADDMODE_INDEXED:
		assert(0);
		break;
	case ADDMODE_REGISTER:
		ret = cpu_regs[reg_num];
		break;
	case ADDMODE_REGISTER_DEFERRED: {
		/* FIXME: Is this correct? */
		switch (opsize) {
		case TYPE_BYTE:
			tcg_gen_qemu_ld8u(ret, cpu_regs[reg_num], 0);
			break;
		case TYPE_WORD:
			tcg_gen_qemu_ld16u(ret, cpu_regs[reg_num], 0);
			break;
		case TYPE_LONG:
			tcg_gen_qemu_ld32u(ret, cpu_regs[reg_num], 0);
			break;
		}
		break;
	}
	case ADDMODE_AUTODECREMENT:
	case ADDMODE_AUTOINCREMENT:
	case ADDMODE_AUTOINCREMENT_DEFERRED:
	case ADDMODE_BYTE_DISP:
	case ADDMODE_BYTE_DISP_DEFERRED:
	case ADDMODE_WORD_DISP:
	case ADDMODE_WORD_DISP_DEFERRED:
	case ADDMODE_LONGWORD_DISP:
	case ADDMODE_LONGWORD_DISP_DEFERRED:
	default:
		assert(0);
		break;
	}

	return ret;
}

#define UPDATE_CC(op)						\
	do {							\
		ctx->cc_op = op;				\
	} while (0)						\

#define EMIT_SET_INTERLOCK()		/* TODO */

#define EMIT_RELEASE_INTERLOCK()	/* TODO */

#define EMIT_INTEGER_OVERFLOW()		/* TODO */

#define EMIT_DIVIDE_BY_ZERO()		/* TODO */

#define EMIT_RESERVED_OPERAND_FAULT()	/* TODO */

#define EMIT_ADAWI()						\
	do {							\
		TCGv tmp, add, sum;				\
		add = disas_vax_operand(ctx, TYPE_WORD);	\
		tmp = add;					\
		EMIT_SET_INTERLOCK();				\
		sum = disas_vax_operand(ctx, TYPE_WORD);	\
		tcg_gen_add_tl(sum, sum, tmp);			\
		EMIT_RELEASE_INTERLOCK();			\
		UPDATE_CC(CC_OP_ADD);				\
		EMIT_INTEGER_OVERFLOW();			\
		EMIT_RESERVED_OPERAND_FAULT();			\
	} while (0)						\

#define EMIT_ADD(sum, add1, add2)				\
	do {							\
		tcg_gen_add_tl(sum, add1, add2);		\
		UPDATE_CC(CC_OP_ADD);				\
		EMIT_INTEGER_OVERFLOW();			\
	} while (0)						\

#define EMIT_ADD2(type)						\
	do {							\
		TCGv add, sum;					\
		add = disas_vax_operand(ctx, type);		\
		sum = disas_vax_operand(ctx, type);		\
		EMIT_ADD(sum, sum, add);			\
	} while (0)						\

#define EMIT_ADD3(type)						\
	do {							\
		TCGv add1, add2, sum;				\
		add1 = disas_vax_operand(ctx, type);		\
		add2 = disas_vax_operand(ctx, type);		\
		sum  = disas_vax_operand(ctx, type);		\
		EMIT_ADD(sum, add1, add2);			\
	} while (0)						\

#define EMIT_ADWC()						\
	do {							\
	} while (0)						\

#define EMIT_ASH(type)						\
	do {							\
	} while (0)						\

#define EMIT_BIC(dst, src, mask)				\
	do {							\
		tcg_gen_not_tl(mask, mask);			\
		tcg_gen_and_tl(dst, src, mask);			\
		UPDATE_CC(CC_OP_LOGICAL_OP);			\
		EMIT_INTEGER_OVERFLOW();			\
	} while (0)						\

#define EMIT_BIC2(type)						\
	do {							\
		TCGv mask, dst;					\
		mask = disas_vax_operand(ctx, type);		\
		dst = disas_vax_operand(ctx, type);		\
		EMIT_BIC(dst, dst, mask);			\
	} while (0)						\

#define EMIT_BIC3(type)						\
	do {							\
		TCGv mask, src, dst;				\
		mask = disas_vax_operand(ctx, type);		\
		src = disas_vax_operand(ctx, type);		\
		dst = disas_vax_operand(ctx, type);		\
		EMIT_BIC(dst, src, mask);			\
	} while (0)						\

#define EMIT_BIS(dst, src, mask)				\
	do {							\
		tcg_gen_or_tl(dst, src, mask);			\
		UPDATE_CC(CC_OP_LOGICAL_OP);			\
		EMIT_INTEGER_OVERFLOW();			\
	} while (0)						\

#define EMIT_BIS2(type)						\
	do {							\
		TCGv mask, dst;					\
		mask = disas_vax_operand(ctx, type);		\
		dst = disas_vax_operand(ctx, type);		\
		EMIT_BIS(dst, dst, mask);			\
	} while (0)						\

#define EMIT_BIS3(type)						\
	do {							\
		TCGv mask, src, dst;				\
		mask = disas_vax_operand(ctx, type);		\
		src = disas_vax_operand(ctx, type);		\
		dst = disas_vax_operand(ctx, type);		\
		EMIT_BIS(dst, src, mask);			\
	} while (0)						\

#define EMIT_BIT(type)						\
	do {							\
	} while (0)						\

#define EMIT_CLR(type)						\
	do {							\
	} while (0)						\

#define EMIT_CMP(type)						\
	do {							\
	} while (0)						\

#define EMIT_CVT(from_type, to_type)				\
	do {							\
	} while (0)						\

#define EMIT_DEC(type)						\
	do {							\
		TCGv dif;					\
		dif = disas_vax_operand(ctx, type);		\
		tcg_gen_subi_tl(dif, dif, 1);			\
		UPDATE_CC(CC_OP_SUB);				\
		EMIT_INTEGER_OVERFLOW();			\
	} while (0)						\

#define EMIT_DIV(quo, divd, divr)				\
	do {							\
		tcg_gen_div_tl(quo, divd, divr);		\
		UPDATE_CC(CC_OP_DIV);				\
		EMIT_INTEGER_OVERFLOW();			\
		EMIT_DIVIDE_BY_ZERO();				\
	} while (0)						\

#define EMIT_DIV2(type)						\
	do {							\
		TCGv divr, quo;					\
		divr = disas_vax_operand(ctx, type);		\
		quo = disas_vax_operand(ctx, type);		\
		EMIT_DIV(quo, quo, divr);			\
	} while (0)						\

#define EMIT_DIV3(type)						\
	do {							\
		TCGv divr, divd, quo;				\
		divr = disas_vax_operand(ctx, type);		\
		divd = disas_vax_operand(ctx, type);		\
		quo = disas_vax_operand(ctx, type);		\
		EMIT_DIV(quo, divd, divr);			\
	} while (0)						\

#define EMIT_EDIV()						\
	do {							\
	} while (0)						\

#define EMIT_EMUL()						\
	do {							\
	} while (0)						\

#define EMIT_INC(type)						\
	do {							\
		TCGv sum;					\
		sum = disas_vax_operand(ctx, type);		\
		tcg_gen_addi_tl(sum, sum, 1);			\
		UPDATE_CC(CC_OP_ADD);				\
		EMIT_INTEGER_OVERFLOW();			\
	} while (0)						\

#define EMIT_MCOM(type)						\
	do {							\
	} while (0)						\

#define EMIT_MNEG(type)						\
	do {							\
	} while (0)						\

#define EMIT_MOV(type)						\
	do {							\
	} while (0)						\

#define EMIT_MOVZ(from_type, to_type)				\
	do {							\
	} while (0)						\

#define EMIT_MUL(prod, muld, mulr)				\
	do {							\
		tcg_gen_mul_tl(prod, muld, mulr);		\
		UPDATE_CC(CC_OP_MUL);				\
		EMIT_INTEGER_OVERFLOW();			\
	} while (0)						\

#define EMIT_MUL2(type)						\
	do {							\
		TCGv mulr, prod;				\
		mulr = disas_vax_operand(ctx, type);		\
		prod = disas_vax_operand(ctx, type);		\
		EMIT_MUL(prod, prod, mulr);			\
	} while (0)						\

#define EMIT_MUL3(type)						\
	do {							\
		TCGv mulr, muld, prod;				\
		mulr = disas_vax_operand(ctx, type);		\
		muld = disas_vax_operand(ctx, type);		\
		prod = disas_vax_operand(ctx, type);		\
		EMIT_MUL(prod, muld, mulr);			\
	} while (0)						\

#define EMIT_PUSHL()						\
	do {							\
	} while (0)						\

#define EMIT_ROTL()						\
	do {							\
	} while (0)						\

#define EMIT_SBWC()						\
	do {							\
	} while (0)						\

#define EMIT_SUB(dif, min, sub)					\
	do {							\
		tcg_gen_sub_tl(dif, min, sub);			\
		UPDATE_CC(CC_OP_SUB);				\
		EMIT_INTEGER_OVERFLOW();			\
	} while (0)						\

#define EMIT_SUB2(type)						\
	do {							\
		TCGv sub, dif;					\
		sub = disas_vax_operand(ctx, type);		\
		dif = disas_vax_operand(ctx, type);		\
		EMIT_SUB(dif, dif, sub);			\
	} while (0)						\

#define EMIT_SUB3(type)						\
	do {							\
		TCGv sub, min, dif;				\
		sub = disas_vax_operand(ctx, type);		\
		min = disas_vax_operand(ctx, type);		\
		dif = disas_vax_operand(ctx, type);		\
		EMIT_SUB(dif, min, sub);			\
	} while (0)						\

#define EMIT_TST(type)						\
	do {							\
	} while (0)						\

#define EMIT_XOR(dst, src, mask)				\
	do {							\
		tcg_gen_xor_tl(dst, src, mask);			\
		UPDATE_CC(CC_OP_LOGICAL_OP);			\
		EMIT_INTEGER_OVERFLOW();			\
	} while (0)						\

#define EMIT_XOR2(type)						\
	do {							\
		TCGv mask, dst;					\
		mask = disas_vax_operand(ctx, type);		\
		dst = disas_vax_operand(ctx, type);		\
		EMIT_XOR(dst, dst, mask);			\
	} while (0)						\

#define EMIT_XOR3(type)						\
	do {							\
		TCGv mask, src, dst;				\
		mask = disas_vax_operand(ctx, type);		\
		src = disas_vax_operand(ctx, type);		\
		dst = disas_vax_operand(ctx, type);		\
		EMIT_XOR(dst, src, mask);			\
	} while (0)						\

#define EMIT_MOVA(type)						\
	do {							\
	} while (0)						\

#define EMIT_PUSHA(type)					\
	do {							\
	} while (0)						\

#define EMIT_CMPV()						\
	do {							\
	} while (0)						\

#define EMIT_CMPZV()						\
	do {							\
	} while (0)						\

#define EMIT_EXTV()						\
	do {							\
	} while (0)						\

#define EMIT_EXTZV()						\
	do {							\
	} while (0)						\

#define EMIT_FFC()						\
	do {							\
	} while (0)						\

#define EMIT_FFS()						\
	do {							\
	} while (0)						\

#define EMIT_INSV()						\
	do {							\
	} while (0)						\

#define EMIT_ACB(type)						\
	do {							\
	} while (0)						\

#define EMIT_AOBLEQ()						\
	do {							\
	} while (0)						\

#define EMIT_AOBLSS()						\
	do {							\
	} while (0)						\

#define EMIT_B(cond)						\
	do {							\
	} while (0)						\

#define EMIT_BBS()						\
	do {							\
	} while (0)						\

#define EMIT_BBSS()						\
	do {							\
	} while (0)						\

#define EMIT_BBCS()						\
	do {							\
	} while (0)						\

#define EMIT_BBSC()						\
	do {							\
	} while (0)						\

#define EMIT_BBCC()						\
	do {							\
	} while (0)						\

#define EMIT_BBSSI()						\
	do {							\
	} while (0)						\

#define EMIT_BBCCI()						\
	do {							\
	} while (0)						\

#define EMIT_BBC()						\
	do {							\
	} while (0)						\

#define EMIT_BLBS()						\
	do {							\
	} while (0)						\

#define EMIT_BLBC()						\
	do {							\
	} while (0)						\

#define EMIT_BR(type)						\
	do {							\
	} while (0)						\

#define EMIT_BSB(type)						\
	do {							\
	} while (0)						\

#define EMIT_CASE(type)						\
	do {							\
	} while (0)						\

#define EMIT_JMP()						\
	do {							\
	} while (0)						\

#define EMIT_JSB()						\
	do {							\
	} while (0)						\

#define EMIT_RSB()						\
	do {							\
	} while (0)						\

#define EMIT_SOBGEQ()						\
	do {							\
	} while (0)						\

#define EMIT_SOBGTR()						\
	do {							\
	} while (0)						\

#define EMIT_CALLG()						\
	do {							\
	} while (0)						\

#define EMIT_CALLS()						\
	do {							\
	} while (0)						\

#define EMIT_RET()						\
	do {							\
	} while (0)						\

#define EMIT_BICPSW()						\
	do {							\
	} while (0)						\

#define EMIT_BISPSW()						\
	do {							\
	} while (0)						\

#define EMIT_BPT()						\
	do {							\
	} while (0)						\

#define EMIT_BUG(type)						\
	do {							\
	} while (0)						\

#define EMIT_HALT()						\
	do {							\
	} while (0)						\

#define EMIT_INDEX()						\
	do {							\
	} while (0)						\

#define EMIT_MOVPSL()						\
	do {							\
	} while (0)						\

#define EMIT_NOP()						\
	do {							\
	} while (0)						\

#define EMIT_POPR()						\
	do {							\
	} while (0)						\

#define EMIT_PUSHR()						\
	do {							\
	} while (0)						\

#define EMIT_XFC()						\
	do {							\
	} while (0)						\

#define EMIT_INSQHI()						\
	do {							\
	} while (0)						\

#define EMIT_INSQTI()						\
	do {							\
	} while (0)						\

#define EMIT_INSQUE()						\
	do {							\
	} while (0)						\

#define EMIT_REMQHI()						\
	do {							\
	} while (0)						\

#define EMIT_REMQTI()						\
	do {							\
	} while (0)						\

#define EMIT_REMQUE()						\
	do {							\
	} while (0)						\

#define EMIT_FADD2(type)					\
	do {							\
	} while (0)						\

#define EMIT_FADD3(type)					\
	do {							\
	} while (0)						\

#define EMIT_FCMP(type)						\
	do {							\
	} while (0)						\

#define EMIT_FCVT(from_type, to_type)				\
	do {							\
	} while (0)						\

#define EMIT_FCVTR(from_type, to_type)				\
	do {							\
	} while (0)						\

#define EMIT_FDIV2(type)					\
	do {							\
	} while (0)						\

#define EMIT_FDIV3(type)					\
	do {							\
	} while (0)						\

#define EMIT_FEMOD(type)					\
	do {							\
	} while (0)						\

#define EMIT_FMNEG(type)					\
	do {							\
	} while (0)						\

#define EMIT_FMOV(type)						\
	do {							\
	} while (0)						\

#define EMIT_FMUL2(type)					\
	do {							\
	} while (0)						\

#define EMIT_FMUL3(type)					\
	do {							\
	} while (0)						\

#define EMIT_FPOLY(type)					\
	do {							\
	} while (0)						\

#define EMIT_FSUB2(type)					\
	do {							\
	} while (0)						\

#define EMIT_FSUB3(type)					\
	do {							\
	} while (0)						\

#define EMIT_FTST(type)						\
	do {							\
	} while (0)						\

#define EMIT_CMPC3()						\
	do {							\
	} while (0)						\

#define EMIT_CMPC5()						\
	do {							\
	} while (0)						\

#define EMIT_LOCC()						\
	do {							\
	} while (0)						\

#define EMIT_MATCHC()						\
	do {							\
	} while (0)						\

#define EMIT_MOVC3()						\
	do {							\
	} while (0)						\

#define EMIT_MOVC5()						\
	do {							\
	} while (0)						\

#define EMIT_MOVTC()						\
	do {							\
	} while (0)						\

#define EMIT_MOVTUC()						\
	do {							\
	} while (0)						\

#define EMIT_SCANC()						\
	do {							\
	} while (0)						\

#define EMIT_SKPC()						\
	do {							\
	} while (0)						\

#define EMIT_SPANC()						\
	do {							\
	} while (0)						\

#define EMIT_CRC()						\
	do {							\
	} while (0)						\

#define EMIT_ADDP4()						\
	do {							\
	} while (0)						\

#define EMIT_ADDP6()						\
	do {							\
	} while (0)						\

#define EMIT_ASHP()						\
	do {							\
	} while (0)						\

#define EMIT_CMPP3()						\
	do {							\
	} while (0)						\

#define EMIT_CMPP4()						\
	do {							\
	} while (0)						\

#define EMIT_CVTLP()						\
	do {							\
	} while (0)						\

#define EMIT_CVTPL()						\
	do {							\
	} while (0)						\

#define EMIT_CVTPS()						\
	do {							\
	} while (0)						\

#define EMIT_CVTPT()						\
	do {							\
	} while (0)						\

#define EMIT_CVTSP()						\
	do {							\
	} while (0)						\

#define EMIT_CVTTP()						\
	do {							\
	} while (0)						\

#define EMIT_DIVP()						\
	do {							\
	} while (0)						\

#define EMIT_MOVP()						\
	do {							\
	} while (0)						\

#define EMIT_MULP()						\
	do {							\
	} while (0)						\

#define EMIT_SUBP4()						\
	do {							\
	} while (0)						\

#define EMIT_SUBP6()						\
	do {							\
	} while (0)						\

#define EMIT_EDITPC()						\
	do {							\
	} while (0)						\

#define EMIT_PROBER()						\
	do {							\
	} while (0)						\

#define EMIT_PROBEW()						\
	do {							\
	} while (0)						\

#define EMIT_REI()						\
	do {							\
	} while (0)						\

#define EMIT_CHMK()						\
	do {							\
	} while (0)						\

#define EMIT_CHME()						\
	do {							\
	} while (0)						\

#define EMIT_CHMS()						\
	do {							\
	} while (0)						\

#define EMIT_CHMU()						\
	do {							\
	} while (0)						\

#define EMIT_LDPCTX()						\
	do {							\
	} while (0)						\

#define EMIT_SVPCTX()						\
	do {							\
	} while (0)						\

#define EMIT_MFPR()						\
	do {							\
	} while (0)						\

#define EMIT_MTPR()						\
	do {							\
	} while (0)						\

static void disas_vax_insn(CPUVAXState *env, DisasContext *ctx)
{
	uint16_t opc;

	opc = ldub_code(ctx->pc++);

	if (opc >= 0xFD && opc <= 0xFF)
		opc = ldub_code(ctx->pc++) | (opc << 8);

	switch (opc) {

	/*
	 * Integer arithmetic and logical instructions:
	 */

	case OPC_ADAWI:		EMIT_ADAWI();		break;

	case OPC_ADDB2:		EMIT_ADD2(TYPE_BYTE);	break;
	case OPC_ADDB3:		EMIT_ADD3(TYPE_BYTE);	break;
	case OPC_ADDW2:		EMIT_ADD2(TYPE_WORD);	break;
	case OPC_ADDW3:		EMIT_ADD3(TYPE_WORD);	break;
	case OPC_ADDL2:		EMIT_ADD2(TYPE_LONG);	break;
	case OPC_ADDL3:		EMIT_ADD3(TYPE_LONG);	break;

	case OPC_ADWC:		EMIT_ADWC();		break;

	case OPC_ASHL:		EMIT_ASH(TYPE_LONG);	break;
	case OPC_ASHQ:		EMIT_ASH(TYPE_QUAD);	break;

	case OPC_BICB2:		EMIT_BIC2(TYPE_BYTE);	break;
	case OPC_BICB3:		EMIT_BIC3(TYPE_BYTE);	break;
	case OPC_BICW2:		EMIT_BIC2(TYPE_WORD);	break;
	case OPC_BICW3:		EMIT_BIC3(TYPE_WORD);	break;
	case OPC_BICL2:		EMIT_BIC2(TYPE_LONG);	break;
	case OPC_BICL3:		EMIT_BIC3(TYPE_LONG);	break;

	case OPC_BISB2:		EMIT_BIS2(TYPE_BYTE);	break;
	case OPC_BISB3:		EMIT_BIS3(TYPE_BYTE);	break;
	case OPC_BISW2:		EMIT_BIS2(TYPE_WORD);	break;
	case OPC_BISW3:		EMIT_BIS3(TYPE_WORD);	break;
	case OPC_BISL2:		EMIT_BIS2(TYPE_LONG);	break;
	case OPC_BISL3:		EMIT_BIS3(TYPE_LONG);	break;

	case OPC_BITB:		EMIT_BIT(TYPE_BYTE);	break;
	case OPC_BITW:		EMIT_BIT(TYPE_WORD);	break;
	case OPC_BITL:		EMIT_BIT(TYPE_LONG);	break;

	case OPC_CLRB:		EMIT_CLR(TYPE_BYTE);	break;
	case OPC_CLRW:		EMIT_CLR(TYPE_WORD);	break;
	case OPC_CLRL:		EMIT_CLR(TYPE_LONG);	break;
	case OPC_CLRQ:		EMIT_CLR(TYPE_QUAD);	break;
	case OPC_CLRO:		EMIT_CLR(TYPE_OCTA);	break;

	case OPC_CMPB:		EMIT_CMP(TYPE_BYTE);	break;
	case OPC_CMPW:		EMIT_CMP(TYPE_WORD);	break;
	case OPC_CMPL:		EMIT_CMP(TYPE_LONG);	break;

	case OPC_CVTBW:		EMIT_CVT(TYPE_BYTE, TYPE_WORD);	break;
	case OPC_CVTBL:		EMIT_CVT(TYPE_BYTE, TYPE_LONG); break;
	case OPC_CVTWB:		EMIT_CVT(TYPE_WORD, TYPE_BYTE);	break;
	case OPC_CVTWL:		EMIT_CVT(TYPE_WORD, TYPE_LONG);	break;
	case OPC_CVTLB:		EMIT_CVT(TYPE_LONG, TYPE_BYTE); break;
	case OPC_CVTLW:		EMIT_CVT(TYPE_LONG, TYPE_WORD);	break;

	case OPC_DECB:		EMIT_DEC(TYPE_BYTE);	break;
	case OPC_DECW:		EMIT_DEC(TYPE_WORD);	break;
	case OPC_DECL:		EMIT_DEC(TYPE_LONG);	break;

	case OPC_DIVB2:		EMIT_DIV2(TYPE_BYTE);	break;
	case OPC_DIVB3:		EMIT_DIV3(TYPE_BYTE);	break;
	case OPC_DIVW2:		EMIT_DIV2(TYPE_WORD);	break;
	case OPC_DIVW3:		EMIT_DIV3(TYPE_WORD);	break;
	case OPC_DIVL2:		EMIT_DIV2(TYPE_LONG);	break;
	case OPC_DIVL3:		EMIT_DIV3(TYPE_LONG);	break;

	case OPC_EDIV:		EMIT_EDIV();		break;

	case OPC_EMUL:		EMIT_EMUL();		break;

	case OPC_INCB:		EMIT_INC(TYPE_BYTE);	break;
	case OPC_INCW:		EMIT_INC(TYPE_WORD);	break;
	case OPC_INCL:		EMIT_INC(TYPE_LONG);	break;

	case OPC_MCOMB:		EMIT_MCOM(TYPE_BYTE);	break;
	case OPC_MCOMW:		EMIT_MCOM(TYPE_WORD);	break;
	case OPC_MCOML:		EMIT_MCOM(TYPE_LONG);	break;

	case OPC_MNEGB:		EMIT_MNEG(TYPE_BYTE);	break;
	case OPC_MNEGW:		EMIT_MNEG(TYPE_WORD);	break;
	case OPC_MNEGL:		EMIT_MNEG(TYPE_LONG);	break;

	case OPC_MOVB:		EMIT_MOV(TYPE_BYTE);	break;
	case OPC_MOVW:		EMIT_MOV(TYPE_WORD);	break;
	case OPC_MOVL:		EMIT_MOV(TYPE_LONG);	break;
	case OPC_MOVQ:		EMIT_MOV(TYPE_QUAD);	break;
	case OPC_MOVO:		EMIT_MOV(TYPE_OCTA);	break;

	case OPC_MOVZBL:	EMIT_MOVZ(TYPE_BYTE, TYPE_LONG); break;
	case OPC_MOVZBW:	EMIT_MOVZ(TYPE_BYTE, TYPE_WORD); break;
	case OPC_MOVZWL:	EMIT_MOVZ(TYPE_WORD, TYPE_LONG); break;

	case OPC_MULB2:		EMIT_MUL2(TYPE_BYTE);	break;
	case OPC_MULB3:		EMIT_MUL3(TYPE_BYTE);	break;
	case OPC_MULW2:		EMIT_MUL2(TYPE_WORD);	break;
	case OPC_MULW3:		EMIT_MUL3(TYPE_WORD);	break;
	case OPC_MULL2:		EMIT_MUL2(TYPE_LONG);	break;
	case OPC_MULL3:		EMIT_MUL3(TYPE_LONG);	break;

	case OPC_PUSHL:		EMIT_PUSHL();		break;

	case OPC_ROTL:		EMIT_ROTL();		break;

	case OPC_SBWC:		EMIT_SBWC();		break;

	case OPC_SUBB2:		EMIT_SUB2(TYPE_BYTE);	break;
	case OPC_SUBB3:		EMIT_SUB3(TYPE_BYTE);	break;
	case OPC_SUBW2:		EMIT_SUB2(TYPE_WORD);	break;
	case OPC_SUBW3:		EMIT_SUB3(TYPE_WORD);	break;
	case OPC_SUBL2:		EMIT_SUB2(TYPE_LONG);	break;
	case OPC_SUBL3:		EMIT_SUB3(TYPE_LONG);	break;

	case OPC_TSTB:		EMIT_TST(TYPE_BYTE);	break;
	case OPC_TSTW:		EMIT_TST(TYPE_WORD);	break;
	case OPC_TSTL:		EMIT_TST(TYPE_LONG);	break;

	case OPC_XORB2:		EMIT_XOR2(TYPE_BYTE);	break;
	case OPC_XORB3:		EMIT_XOR3(TYPE_BYTE);	break;
	case OPC_XORW2:		EMIT_XOR2(TYPE_WORD);	break;
	case OPC_XORW3:		EMIT_XOR3(TYPE_WORD);	break;
	case OPC_XORL2:		EMIT_XOR2(TYPE_LONG);	break;
	case OPC_XORL3:		EMIT_XOR3(TYPE_LONG);	break;

	/*
	 * Address instructions:
	 */

	case OPC_MOVAB:		EMIT_MOVA(TYPE_BYTE);	break;
	case OPC_MOVAW:		EMIT_MOVA(TYPE_WORD);	break;
	case OPC_MOVAL:		EMIT_MOVA(TYPE_LONG);	break;
	case OPC_MOVAQ:		EMIT_MOVA(TYPE_QUAD);	break;
	case OPC_MOVAO:		EMIT_MOVA(TYPE_OCTA);	break;

	case OPC_PUSHAB:	EMIT_PUSHA(TYPE_BYTE);	break;
	case OPC_PUSHAW:	EMIT_PUSHA(TYPE_WORD);	break;
	case OPC_PUSHAL:	EMIT_PUSHA(TYPE_LONG);	break;
	case OPC_PUSHAQ:	EMIT_PUSHA(TYPE_QUAD);	break;
	case OPC_PUSHAO:	EMIT_PUSHA(TYPE_OCTA);	break;

	/*
	 * Variable-length bit field instructions:
	 */

	case OPC_CMPV:		EMIT_CMPV();		break;
	case OPC_CMPZV:		EMIT_CMPZV();		break;

	case OPC_EXTV:		EMIT_EXTV();		break;
	case OPC_EXTZV:		EMIT_EXTZV();		break;

	case OPC_FFC:		EMIT_FFC();		break;
	case OPC_FFS:		EMIT_FFS();		break;

	case OPC_INSV:		EMIT_INSV();		break;

	/*
	 * Control instructions:
	 */

	case OPC_ACBB:		EMIT_ACB(TYPE_BYTE);	break;
	case OPC_ACBW:		EMIT_ACB(TYPE_WORD);	break;
	case OPC_ACBL:		EMIT_ACB(TYPE_LONG);	break;
	case OPC_ACBF:		EMIT_ACB(TYPE_F_FLOATING); break;
	case OPC_ACBD:		EMIT_ACB(TYPE_D_FLOATING); break;
	case OPC_ACBG:		EMIT_ACB(TYPE_G_FLOATING); break;
	case OPC_ACBH:		EMIT_ACB(TYPE_H_FLOATING); break;

	case OPC_AOBLEQ:	EMIT_AOBLEQ();		break;
	case OPC_AOBLSS:	EMIT_AOBLSS();		break;

	case OPC_BGTR:		EMIT_B(COND_GTR);	break;
	case OPC_BLEQ:		EMIT_B(COND_LEQ);	break;
	case OPC_BNEQ:		EMIT_B(COND_NEQ);	break;
	case OPC_BEQL:		EMIT_B(COND_EQL);	break;
	case OPC_BGEQ:		EMIT_B(COND_GEQ);	break;
	case OPC_BLSS:		EMIT_B(COND_LSS);	break;
	case OPC_BGTRU:		EMIT_B(COND_GTRU);	break;
	case OPC_BLEQU:		EMIT_B(COND_LEQU);	break;
	case OPC_BVC:		EMIT_B(COND_BVC);	break;
	case OPC_BVS:		EMIT_B(COND_BVS);	break;
	case OPC_BGEQU:		EMIT_B(COND_EQU);	break;
	case OPC_BLSSU:		EMIT_B(COND_LSSU);	break;

	case OPC_BBS:		EMIT_BBS();		break;
	case OPC_BBC:		EMIT_BBC();		break;

	case OPC_BBSS:		EMIT_BBSS();		break;
	case OPC_BBCS:		EMIT_BBCS();		break;
	case OPC_BBSC:		EMIT_BBSC();		break;
	case OPC_BBCC:		EMIT_BBCC();		break;

	case OPC_BBSSI:		EMIT_BBSSI();		break;
	case OPC_BBCCI:		EMIT_BBCCI();		break;

	case OPC_BLBS:		EMIT_BLBS();		break;
	case OPC_BLBC:		EMIT_BLBC();		break;

	case OPC_BRB:		EMIT_BR(TYPE_BYTE);	break;
	case OPC_BRW:		EMIT_BR(TYPE_WORD);	break;

	case OPC_BSBB:		EMIT_BSB(TYPE_BYTE);	break;
	case OPC_BSBW:		EMIT_BSB(TYPE_WORD);	break;

	case OPC_CASEB:		EMIT_CASE(TYPE_BYTE);	break;
	case OPC_CASEW:		EMIT_CASE(TYPE_WORD);	break;
	case OPC_CASEL:		EMIT_CASE(TYPE_LONG);	break;

	case OPC_JMP:		EMIT_JMP();		break;

	case OPC_JSB:		EMIT_JSB();		break;

	case OPC_RSB:		EMIT_RSB();		break;

	case OPC_SOBGEQ:	EMIT_SOBGEQ();		break;

	case OPC_SOBGTR:	EMIT_SOBGTR();		break;

	/*
	 * Procedure call instructions:
	 */

	case OPC_CALLG:		EMIT_CALLG();		break;

	case OPC_CALLS:		EMIT_CALLS();		break;

	case OPC_RET:		EMIT_RET();		break;

	/*
	 * Miscellaneous instructions:
	 */

	case OPC_BICPSW:	EMIT_BICPSW();		break;

	case OPC_BISPSW:	EMIT_BISPSW();		break;

	case OPC_BPT:		EMIT_BPT();		break;

	case OPC_BUGW:		EMIT_BUG(TYPE_WORD);	break;
	case OPC_BUGL:		EMIT_BUG(TYPE_LONG);	break;

	case OPC_HALT:		EMIT_HALT();		break;

	case OPC_INDEX:		EMIT_INDEX();		break;

	case OPC_MOVPSL:	EMIT_MOVPSL();		break;

	case OPC_NOP:		EMIT_NOP();		break;

	case OPC_POPR:		EMIT_POPR();		break;

	case OPC_PUSHR:		EMIT_PUSHR();		break;

	case OPC_XFC:		EMIT_XFC();		break;

	/*
	 * Queue instructions:
	 */

	case OPC_INSQHI:	EMIT_INSQHI();		break;

	case OPC_INSQTI:	EMIT_INSQTI();		break;

	case OPC_INSQUE:	EMIT_INSQUE();		break;

	case OPC_REMQHI:	EMIT_REMQHI();		break;

	case OPC_REMQTI:	EMIT_REMQTI();		break;

	case OPC_REMQUE:	EMIT_REMQUE();		break;

	/*
	 * Floating point instructions:
	 */

	case OPC_ADDF2:		EMIT_FADD2(TYPE_F_FLOATING); break;
	case OPC_ADDF3:		EMIT_FADD3(TYPE_F_FLOATING); break;
	case OPC_ADDD2:		EMIT_FADD2(TYPE_D_FLOATING); break;
	case OPC_ADDD3:		EMIT_FADD3(TYPE_D_FLOATING); break;
	case OPC_ADDG2:		EMIT_FADD2(TYPE_G_FLOATING); break;
	case OPC_ADDG3:		EMIT_FADD3(TYPE_G_FLOATING); break;
	case OPC_ADDH2:		EMIT_FADD2(TYPE_H_FLOATING); break;
	case OPC_ADDH3:		EMIT_FADD3(TYPE_H_FLOATING); break;

	case OPC_CMPF:		EMIT_FCMP(TYPE_F_FLOATING); break;
	case OPC_CMPD:		EMIT_FCMP(TYPE_D_FLOATING); break;
	case OPC_CMPG:		EMIT_FCMP(TYPE_G_FLOATING); break;
	case OPC_CMPH:		EMIT_FCMP(TYPE_H_FLOATING); break;
 
	case OPC_CVTBF:		EMIT_FCVT(TYPE_BYTE, TYPE_F_FLOATING); break;
	case OPC_CVTWF:		EMIT_FCVT(TYPE_WORD, TYPE_F_FLOATING); break;
	case OPC_CVTLF:		EMIT_FCVT(TYPE_LONG, TYPE_F_FLOATING); break;
	case OPC_CVTBD:		EMIT_FCVT(TYPE_BYTE, TYPE_D_FLOATING); break;
	case OPC_CVTWD:		EMIT_FCVT(TYPE_WORD, TYPE_D_FLOATING); break;
	case OPC_CVTLD:		EMIT_FCVT(TYPE_LONG, TYPE_D_FLOATING); break;
	case OPC_CVTBG:		EMIT_FCVT(TYPE_BYTE, TYPE_G_FLOATING); break;
	case OPC_CVTWG:		EMIT_FCVT(TYPE_WORD, TYPE_G_FLOATING); break;
	case OPC_CVTLG:		EMIT_FCVT(TYPE_LONG, TYPE_G_FLOATING); break;
	case OPC_CVTBH:		EMIT_FCVT(TYPE_BYTE, TYPE_H_FLOATING); break;
	case OPC_CVTWH:		EMIT_FCVT(TYPE_WORD, TYPE_H_FLOATING); break;
	case OPC_CVTLH:		EMIT_FCVT(TYPE_LONG, TYPE_H_FLOATING); break;
	case OPC_CVTFB:		EMIT_FCVT(TYPE_F_FLOATING, TYPE_BYTE); break;
	case OPC_CVTFW:		EMIT_FCVT(TYPE_F_FLOATING, TYPE_WORD); break;
	case OPC_CVTFL:		EMIT_FCVT(TYPE_F_FLOATING, TYPE_LONG); break;
	case OPC_CVTRFL:	EMIT_FCVTR(TYPE_F_FLOATING, TYPE_LONG); break;
	case OPC_CVTDB:		EMIT_FCVT(TYPE_D_FLOATING, TYPE_BYTE); break;
	case OPC_CVTDW:		EMIT_FCVT(TYPE_D_FLOATING, TYPE_WORD); break;
	case OPC_CVTDL:		EMIT_FCVT(TYPE_D_FLOATING, TYPE_LONG); break;
	case OPC_CVTRDL:	EMIT_FCVTR(TYPE_D_FLOATING, TYPE_LONG); break;
	case OPC_CVTGB:		EMIT_FCVT(TYPE_G_FLOATING, TYPE_BYTE); break;
	case OPC_CVTGW:		EMIT_FCVT(TYPE_G_FLOATING, TYPE_WORD); break;
	case OPC_CVTGL:		EMIT_FCVT(TYPE_G_FLOATING, TYPE_LONG); break;
	case OPC_CVTRGL:	EMIT_FCVTR(TYPE_G_FLOATING, TYPE_LONG); break;
	case OPC_CVTHB:		EMIT_FCVT(TYPE_H_FLOATING, TYPE_BYTE); break;
	case OPC_CVTHW:		EMIT_FCVT(TYPE_H_FLOATING, TYPE_WORD); break;
	case OPC_CVTHL:		EMIT_FCVT(TYPE_H_FLOATING, TYPE_LONG); break;
	case OPC_CVTHRL:	EMIT_FCVTR(TYPE_H_FLOATING, TYPE_LONG); break;
	case OPC_CVTFD:		EMIT_FCVT(TYPE_F_FLOATING, TYPE_D_FLOATING); break;
	case OPC_CVTFG:		EMIT_FCVT(TYPE_F_FLOATING, TYPE_G_FLOATING); break;
	case OPC_CVTFH:		EMIT_FCVT(TYPE_F_FLOATING, TYPE_H_FLOATING); break;
	case OPC_CVTDF:		EMIT_FCVT(TYPE_D_FLOATING, TYPE_F_FLOATING); break;
	case OPC_CVTDH:		EMIT_FCVT(TYPE_D_FLOATING, TYPE_H_FLOATING); break;
	case OPC_CVTGF:		EMIT_FCVT(TYPE_G_FLOATING, TYPE_F_FLOATING); break;
	case OPC_CVTGH:		EMIT_FCVT(TYPE_G_FLOATING, TYPE_H_FLOATING); break;
	case OPC_CVTHF:		EMIT_FCVT(TYPE_H_FLOATING, TYPE_F_FLOATING); break;
	case OPC_CVTHD:		EMIT_FCVT(TYPE_H_FLOATING, TYPE_D_FLOATING); break;
	case OPC_CVTHG:		EMIT_FCVT(TYPE_H_FLOATING, TYPE_G_FLOATING); break;

	case OPC_DIVF2:		EMIT_FDIV2(TYPE_F_FLOATING); break;
	case OPC_DIVF3:		EMIT_FDIV3(TYPE_F_FLOATING); break;
	case OPC_DIVD2:		EMIT_FDIV2(TYPE_D_FLOATING); break;
	case OPC_DIVD3:		EMIT_FDIV3(TYPE_D_FLOATING); break;
	case OPC_DIVG2:		EMIT_FDIV2(TYPE_G_FLOATING); break;
	case OPC_DIVG3:		EMIT_FDIV3(TYPE_G_FLOATING); break;
	case OPC_DIVH2:		EMIT_FDIV2(TYPE_H_FLOATING); break;
	case OPC_DIVH3:		EMIT_FDIV3(TYPE_H_FLOATING); break;

	case OPC_EMODF:		EMIT_FEMOD(TYPE_F_FLOATING); break;
	case OPC_EMODD:		EMIT_FEMOD(TYPE_D_FLOATING); break;
	case OPC_EMODG:		EMIT_FEMOD(TYPE_G_FLOATING); break;
	case OPC_EMODH:		EMIT_FEMOD(TYPE_H_FLOATING); break;

	case OPC_MNEGF:		EMIT_FMNEG(TYPE_F_FLOATING); break;
	case OPC_MNEGD:		EMIT_FMNEG(TYPE_D_FLOATING); break;
	case OPC_MNEGG:		EMIT_FMNEG(TYPE_G_FLOATING); break;
	case OPC_MNEGH:		EMIT_FMNEG(TYPE_H_FLOATING); break;

	case OPC_MOVF:		EMIT_FMOV(TYPE_F_FLOATING); break;
	case OPC_MOVD:		EMIT_FMOV(TYPE_D_FLOATING); break;
	case OPC_MOVG:		EMIT_FMOV(TYPE_G_FLOATING); break;
	case OPC_MOVH:		EMIT_FMOV(TYPE_H_FLOATING); break;

	case OPC_MULF2:		EMIT_FMUL2(TYPE_F_FLOATING); break;
	case OPC_MULF3:		EMIT_FMUL3(TYPE_F_FLOATING); break;
	case OPC_MULD2:		EMIT_FMUL2(TYPE_D_FLOATING); break;
	case OPC_MULD3:		EMIT_FMUL3(TYPE_D_FLOATING); break;
	case OPC_MULG2:		EMIT_FMUL2(TYPE_G_FLOATING); break;
	case OPC_MULG3:		EMIT_FMUL3(TYPE_G_FLOATING); break;
	case OPC_MULH2:		EMIT_FMUL2(TYPE_H_FLOATING); break;
	case OPC_MULH3:		EMIT_FMUL3(TYPE_H_FLOATING); break;

	case OPC_POLYF:		EMIT_FPOLY(TYPE_F_FLOATING); break;
	case OPC_POLYD:		EMIT_FPOLY(TYPE_D_FLOATING); break;
	case OPC_POLYG:		EMIT_FPOLY(TYPE_G_FLOATING); break;
	case OPC_POLYH:		EMIT_FPOLY(TYPE_H_FLOATING); break;

	case OPC_SUBF2:		EMIT_FSUB2(TYPE_F_FLOATING); break;
	case OPC_SUBF3:		EMIT_FSUB3(TYPE_F_FLOATING); break;
	case OPC_SUBD2:		EMIT_FSUB2(TYPE_D_FLOATING); break;
	case OPC_SUBD3:		EMIT_FSUB3(TYPE_D_FLOATING); break;
	case OPC_SUBG2:		EMIT_FSUB2(TYPE_G_FLOATING); break;
	case OPC_SUBG3:		EMIT_FSUB3(TYPE_G_FLOATING); break;
	case OPC_SUBH2:		EMIT_FSUB2(TYPE_H_FLOATING); break;
	case OPC_SUBH3:		EMIT_FSUB3(TYPE_H_FLOATING); break;

	case OPC_TSTF:		EMIT_FTST(TYPE_F_FLOATING); break;
	case OPC_TSTD:		EMIT_FTST(TYPE_D_FLOATING); break;
	case OPC_TSTG:		EMIT_FTST(TYPE_G_FLOATING); break;
	case OPC_TSTH:		EMIT_FTST(TYPE_H_FLOATING); break;

	/*
	 * Character-string instructions
	 */

	case OPC_CMPC3:		EMIT_CMPC3();	break;
	case OPC_CMPC5:		EMIT_CMPC5();	break;

	case OPC_LOCC:		EMIT_LOCC();	break;

	case OPC_MATCHC:	EMIT_MATCHC();	break;

	case OPC_MOVC3:		EMIT_MOVC3();	break;
	case OPC_MOVC5:		EMIT_MOVC5();	break;

	case OPC_MOVTC:		EMIT_MOVTC();	break;

	case OPC_MOVTUC:	EMIT_MOVTUC();	break;

	case OPC_SCANC:		EMIT_SCANC();	break;

	case OPC_SKPC:		EMIT_SKPC();	break;

	case OPC_SPANC:		EMIT_SPANC();	break;

	/*
	 * Cyclic redundancy check instruction:
	 */

	case OPC_CRC:		EMIT_CRC();	break;

	/*
	 * Decimal-string instructions:
	 */

	case OPC_ADDP4:		EMIT_ADDP4();	break;
	case OPC_ADDP6:		EMIT_ADDP6();	break;

	case OPC_ASHP:		EMIT_ASHP();	break;

	case OPC_CMPP3:		EMIT_CMPP3();	break;
	case OPC_CMPP4:		EMIT_CMPP4();	break;

	case OPC_CVTLP:		EMIT_CVTLP();	break;

	case OPC_CVTPL:		EMIT_CVTPL();	break;

	case OPC_CVTPS:		EMIT_CVTPS();	break;

	case OPC_CVTPT:		EMIT_CVTPT();	break;

	case OPC_CVTSP:		EMIT_CVTSP();	break;

	case OPC_CVTTP:		EMIT_CVTTP();	break;

	case OPC_DIVP:		EMIT_DIVP();	break;

	case OPC_MOVP:		EMIT_MOVP();	break;

	case OPC_MULP:		EMIT_MULP();	break;

	case OPC_SUBP4:		EMIT_SUBP4();	break;
	case OPC_SUBP6:		EMIT_SUBP6();	break;

	/*
	 * Edit instruction:
	 */

	case OPC_EDITPC:	EMIT_EDITPC();	break;

	/*
	 * Privileged services and argument validation:
	 */

	case OPC_PROBER:	EMIT_PROBER();	break;
	case OPC_PROBEW:	EMIT_PROBEW();	break;

	/*
	 * Instructions related to exceptions and interrupts:
	 */

	case OPC_REI:		EMIT_REI();	break;

	case OPC_CHMK:		EMIT_CHMK();	break;
	case OPC_CHME:		EMIT_CHME();	break;
	case OPC_CHMS:		EMIT_CHMS();	break;
	case OPC_CHMU:		EMIT_CHMU();	break;

	/*
	 * Process structure instructions:
	 */

	case OPC_LDPCTX:	EMIT_LDPCTX();	break;

	case OPC_SVPCTX:	EMIT_SVPCTX();	break;

	/*
	 * MTRP and MFPR instructions:
	 */

	case OPC_MFPR:		EMIT_MFPR();	break;

	case OPC_MTPR:		EMIT_MTPR();	break;

	default:
		goto invalid_opcode;
	}

invalid_opcode:
	qemu_log("INVALID(pc = %08x)\n", ctx->pc);
	/* TODO: generate exception */
}

static void gen_intermediate_code_internal(
	CPUVAXState *env, TranslationBlock *tb, int search_pc)
{
	uint16_t *gen_opc_end = gen_opc_buf + OPC_MAX_SIZE;
	target_ulong pc_start;
	DisasContext ctx;

	/* generate intermediate code */
	pc_start = tb->pc;

	ctx.env		= env;
	ctx.pc		= pc_start;

	do {
		disas_vax_insn(env, &ctx);
	} while (!ctx.is_jmp && gen_opc_ptr < gen_opc_end);
}

void gen_intermediate_code(CPUVAXState *env, TranslationBlock *tb)
{
	gen_intermediate_code_internal(env, tb, 0);
}

void gen_intermediate_code_pc(CPUVAXState *env, TranslationBlock *tb)
{
	gen_intermediate_code_internal(env, tb, 1);
}

void cpu_dump_state(CPUVAXState *env, FILE *f, fprintf_function cpu_fprintf,
		    int flags)
{
	assert(0);
}

void restore_state_to_opc(CPUVAXState *env, TranslationBlock *tb, int pc_pos)
{
	assert(0);
}
