/*
 * VAX virtual CPU header
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

#ifndef CPU_VAX_H
#define CPU_VAX_H

#include <assert.h> /* FIXME */

#define TARGET_LONG_BITS 32

#define CPUArchState struct CPUVAXState

#include "config.h"
#include "qemu-common.h"
#include "cpu-defs.h"

#include "softfloat.h"

#define TARGET_HAS_ICE 1

#define ELF_MACHINE EM_NONE

#define TARGET_PAGE_BITS 9

#define TARGET_PHYS_ADDR_SPACE_BITS 32
#define TARGET_VIRT_ADDR_SPACE_BITS 32

/* Rn */
enum {
	R_PC			= 15,
	R_SP			= 14,
	R_FP			= 13,
	R_AP			= 12,
};

/*
 * PSL bitmasks are defined in Table 1.2 ("Processor Status Longword Fields")
 * of the "VAX Architecture Reference Manual".
 */
enum {
	PSL_CM_BIT		= 31,
	PSL_TP_BIT		= 30,
	PSL_FPD_BIT		= 27,
	PSL_IS_BIT		= 26,
	PSL_CUR_MOD_BIT		= 24,
	PSL_PRV_MOD_BIT		= 23,
	PSL_IPL_BIT		= 16,
	PSL_DV_BIT		= 7,
	PSL_FU_BIT		= 6,
	PSL_IV_BIT		= 5,
	PSL_T_BIT		= 4,
	PSL_N_BIT		= 3,
	PSL_Z_BIT		= 2,
	PSL_V_BIT		= 1,
	PSL_C_BIT		= 0,
};

/*
 * IPRs are defined in Table 8.1 ("Architecturally Defined Internal Processor
 * Registers.") of the "VAX Architecture Reference Manual".
 */
enum {
	IPR_KSP			= 0,
	IPR_ESP			= 1,
	IPR_SSP			= 2,
	IPR_USP			= 3,
	IPR_ISP			= 4,
	IPR_P0BR		= 8,
	IPR_P0LR		= 9,
	IPR_P1BR		= 10,
	IPR_P1LR		= 11,
	IPR_SBR			= 12,
	IPR_SLR			= 13,
	IPR_PCBB		= 16,
	IPR_SCBB		= 17,
	IPR_IPL			= 18,
	IPR_ASTLVL		= 19,
	IPR_SIRR		= 20,
	IPR_SISR		= 21,
	IPR_ICCS		= 24,
	IPR_NICR		= 25,
	IPR_ICR			= 26,
	IPR_TODR		= 27,
	IPR_RXCS		= 32,
	IPR_RXDB		= 33,
	IPR_TXCS		= 34,
	IPR_TXDB		= 35,
	IPR_MAPEN		= 56,
	IPR_TBIA		= 57,
	IPR_TBIS		= 58,
	IPR_PME			= 61,
	IPR_SID			= 62,
	IPR_TBCHK		= 63,
};

/*
 * Arithmetic Exception Type Codes are defined in Table 5.1 of the "VAX
 * Architecture Reference Manual". 
 */
enum {
	SS_INTOVF		= 1,
	SS_INTDIV		= 2,
	SS_FLTOVF		= 3,
	SS_FLTDIV		= 4,
	SS_FLTUND		= 5,
	SS_DECOF		= 6,
	SS_SUBRNG		= 7,

	SS_FLTOVF_F		= 8,
	SS_FLTDIV_F		= 9,
	SS_FLTUND_F		= 10,
};

enum {
	/*
	 * N <- sum LSS 0;
	 * Z <- sum EQL 0;
	 * V <- {integer overflow};
	 * C <- {carry from most significant bit};
	 */
	CC_OP_ADD,

	/*
	 * N <- quo LSS 0;
	 * Z <- quo EQL 0;
	 * V <- {integer overflow} OR {divr EQL 0};
	 * C <- 0;
	 */
	CC_OP_DIV,

	/*
	 * N <- dst LESS 0;
	 * Z <- dst EQL 0;
	 * V <- 0;
	 * C <- C;
	 */
	CC_OP_LOGICAL_OP,

	/*
	 * N <- prod LSS 0;
	 * Z <- prod EQL 0;
	 * V <- {integer overflow};
	 * C <- 0;
	 */
	CC_OP_MUL,

	/*
	 * N <- dif LSS 0;
	 * Z <- dif EQL 0;
	 * V <- {integer overflow};
	 * C <- {borrow into most significant bit};
	 */
	CC_OP_SUB,
};

#define NB_MMU_MODES 2

typedef struct CPUVAXState {
	/* General-purpose registers */
	target_ulong		Rn[16];

	/* Processor status longword */
	target_ulong		PSL;

	/* Internal processor registers */
	target_ulong		IPR[64];

	CPU_COMMON
} CPUVAXState;

#include "cpu-qom.h"

int cpu_vax_exec(CPUVAXState *);
void do_interrupt(CPUVAXState *);
void switch_mode(CPUVAXState *, int);

static inline int cpu_mmu_index(CPUVAXState *env)
{
	assert(0);

	return -1;
}

#include "cpu-all.h"

static inline void
cpu_get_tb_cpu_state(CPUVAXState *env, target_ulong *pc, target_ulong *cs_base, int *flags)
{
	assert(0);
}

static inline bool cpu_has_work(CPUVAXState *env)
{
	assert(0);

	return false;
}

static inline CPUVAXState *cpu_init(const char *cpu_model)
{
	assert(0);

	return NULL;
}

#define cpu_handle_mmu_fault(a, b, c, d) \
	do {			\
		assert(0);	\
	} while (0);		\

#include "exec-all.h"

static inline void cpu_pc_from_tb(CPUVAXState *env, TranslationBlock *tb)
{
	env->Rn[R_PC] = tb->pc;
}

#define cpu_exec cpu_vax_exec

#endif
