/**
 * @file
 *
 * @ingroup ppc_exc
 *
 * @brief PowerPC Exceptions implementation.
 */

/*
 * Copyright (C) 1999 Eric Valette (valette@crf.canon.fr)
 *                    Canon Centre Recherche France.
 *
 * Copyright (C) 2009-2011 embedded brains GmbH.
 *
 * Enhanced by Jay Kulpinski <jskulpin@eng01.gdds.com>
 * to support 603, 603e, 604, 604e exceptions
 *
 * Moved to "libcpu/powerpc/new-exceptions" and consolidated
 * by Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>
 * to be common for all PPCs with new exceptions.
 *
 * Derived from file "libcpu/powerpc/new-exceptions/raw_exception.c".
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <bsp/vectors.h>

#define PPC_BASIC_VECS_WO_SYS \
  [ASM_RESET_VECTOR] = PPC_EXC_CLASSIC, \
  [ASM_MACH_VECTOR] = PPC_EXC_CLASSIC, \
  [ASM_PROT_VECTOR] = PPC_EXC_CLASSIC, \
  [ASM_ISI_VECTOR] = PPC_EXC_CLASSIC, \
  [ASM_EXT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC, \
  [ASM_ALIGN_VECTOR] = PPC_EXC_CLASSIC, \
  [ASM_PROG_VECTOR] = PPC_EXC_CLASSIC, \
  [ASM_FLOAT_VECTOR] = PPC_EXC_CLASSIC, \
  [ASM_DEC_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC, \
  [ASM_TRACE_VECTOR] = PPC_EXC_CLASSIC

#define PPC_BASIC_VECS \
	PPC_BASIC_VECS_WO_SYS, \
	[ASM_SYS_VECTOR] = PPC_EXC_CLASSIC

static const ppc_exc_categories ppc_405_category_table = {
  [ASM_BOOKE_CRIT_VECTOR] = PPC_EXC_405_CRITICAL | PPC_EXC_ASYNC,
  [ASM_MACH_VECTOR] = PPC_EXC_405_CRITICAL,
  [ASM_PROT_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_ISI_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_EXT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_ALIGN_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_PROG_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_FLOAT_VECTOR] = PPC_EXC_CLASSIC,

  [ASM_PPC405_APU_UNAVAIL_VECTOR] = PPC_EXC_CLASSIC,

  [ASM_SYS_VECTOR] = PPC_EXC_CLASSIC,

  [ASM_BOOKE_DEC_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_BOOKE_FIT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_BOOKE_WDOG_VECTOR] = PPC_EXC_405_CRITICAL | PPC_EXC_ASYNC,
  [ASM_BOOKE_DTLBMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_BOOKE_ITLBMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_TRACE_VECTOR] = PPC_EXC_405_CRITICAL,
};

static const ppc_exc_categories mpc_5xx_category_table = {
  [ASM_RESET_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_MACH_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_EXT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_ALIGN_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_PROG_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_FLOAT_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_DEC_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_SYS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_TRACE_VECTOR] = PPC_EXC_CLASSIC,

  [ASM_5XX_FLOATASSIST_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_5XX_SOFTEMUL_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_5XX_IPROT_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_5XX_DPROT_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_5XX_DBREAK_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_5XX_IBREAK_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_5XX_MEBREAK_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_5XX_NMEBREAK_VECTOR] = PPC_EXC_CLASSIC,
};

static const ppc_exc_categories mpc_603_category_table = {
  PPC_BASIC_VECS,

  [ASM_60X_SYSMGMT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_60X_IMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_DLMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_DSMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_ADDR_VECTOR] = PPC_EXC_CLASSIC,
};

static const ppc_exc_categories mpc_604_category_table = {
  PPC_BASIC_VECS,

  [ASM_60X_PERFMON_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_SYSMGMT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_60X_ADDR_VECTOR] = PPC_EXC_CLASSIC,
};

static const ppc_exc_categories mpc_604_altivec_category_table = {
  PPC_BASIC_VECS,

  [ASM_60X_PERFMON_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_SYSMGMT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_60X_ADDR_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_VEC_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_VEC_ASSIST_VECTOR] = PPC_EXC_CLASSIC,
};

static const ppc_exc_categories mpc_750_category_table = {
  PPC_BASIC_VECS,

  [ASM_60X_SYSMGMT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_60X_ADDR_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_ITM_VECTOR] = PPC_EXC_CLASSIC,
};

static const ppc_exc_categories mpc_750_altivec_category_table = {
  PPC_BASIC_VECS,

  [ASM_60X_SYSMGMT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_60X_ADDR_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_ITM_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_VEC_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_VEC_ASSIST_VECTOR] = PPC_EXC_CLASSIC,
};

static const ppc_exc_categories mpc_860_category_table = {
  PPC_BASIC_VECS,

  [ASM_8XX_FLOATASSIST_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_8XX_SOFTEMUL_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_8XX_ITLBMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_8XX_DTLBMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_8XX_ITLBERROR_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_8XX_DTLBERROR_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_8XX_DBREAK_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_8XX_IBREAK_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_8XX_PERIFBREAK_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_8XX_DEVPORT_VECTOR] = PPC_EXC_CLASSIC,
};

static const ppc_exc_categories e300_category_table = {
  [ASM_RESET_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_MACH_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_PROT_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_ISI_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_EXT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_ALIGN_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_PROG_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_FLOAT_VECTOR] = PPC_EXC_NAKED,
  [ASM_DEC_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_SYS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_TRACE_VECTOR] = PPC_EXC_CLASSIC,

  [ASM_E300_CRIT_VECTOR] = PPC_EXC_BOOKE_CRITICAL | PPC_EXC_ASYNC,
  [ASM_E300_PERFMON_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E300_IMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E300_DLMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E300_DSMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E300_ADDR_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E300_SYSMGMT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
};

static const ppc_exc_categories e200_category_table = {
  [ASM_BOOKE_CRIT_VECTOR] = PPC_EXC_BOOKE_CRITICAL | PPC_EXC_ASYNC,
  [ASM_MACH_VECTOR] = PPC_EXC_BOOKE_CRITICAL,
  [ASM_PROT_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_ISI_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_EXT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_ALIGN_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_PROG_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_FLOAT_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_SYS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_BOOKE_DEC_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_BOOKE_FIT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_BOOKE_WDOG_VECTOR] = PPC_EXC_BOOKE_CRITICAL | PPC_EXC_ASYNC,
  [ASM_BOOKE_DTLBMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_BOOKE_ITLBMISS_VECTOR] = PPC_EXC_CLASSIC,

  /* FIXME: Depending on HDI0 [DAPUEN] this is a critical or debug exception */
  [ASM_BOOKE_DEBUG_VECTOR] = PPC_EXC_BOOKE_CRITICAL,

  [ASM_E500_SPE_UNAVAILABLE_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E500_EMB_FP_DATA_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E500_EMB_FP_ROUND_VECTOR] = PPC_EXC_CLASSIC
};

static const ppc_exc_categories e500_category_table = {
  [ASM_BOOKE_CRIT_VECTOR] = PPC_EXC_BOOKE_CRITICAL | PPC_EXC_ASYNC,
  [ASM_MACH_VECTOR] = PPC_EXC_E500_MACHCHK,
  [ASM_PROT_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_ISI_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_EXT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_ALIGN_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_PROG_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_FLOAT_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_SYS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_BOOKE_APU_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_BOOKE_DEC_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_BOOKE_FIT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_BOOKE_WDOG_VECTOR] = PPC_EXC_BOOKE_CRITICAL | PPC_EXC_ASYNC,
  [ASM_BOOKE_DTLBMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_BOOKE_ITLBMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_BOOKE_DEBUG_VECTOR] = PPC_EXC_BOOKE_CRITICAL,
  [ASM_E500_SPE_UNAVAILABLE_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E500_EMB_FP_DATA_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E500_EMB_FP_ROUND_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_E500_PERFMON_VECTOR] = PPC_EXC_CLASSIC
};

static const ppc_exc_categories psim_category_table = {
  PPC_BASIC_VECS_WO_SYS,

  [ASM_60X_SYSMGMT_VECTOR] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ASM_60X_IMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_DLMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_DSMISS_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_ADDR_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_VEC_VECTOR] = PPC_EXC_CLASSIC,
  [ASM_60X_VEC_ASSIST_VECTOR] = PPC_EXC_CLASSIC,
};

const ppc_exc_categories *ppc_exc_categories_for_cpu(ppc_cpu_id_t cpu)
{
  if (ppc_cpu_has_altivec()) {
    switch (cpu) {
      case PPC_7400:
        return &mpc_750_altivec_category_table;
      case PPC_7455:
      case PPC_7457:
        return &mpc_604_altivec_category_table;
      default:
        break;
    }
  }

  switch (cpu) {
    case PPC_7400:
    case PPC_750:
      return &mpc_750_category_table;
    case PPC_7455:
    case PPC_7457:
    case PPC_604:
    case PPC_604e:
    case PPC_604r:
      return &mpc_604_category_table;
    case PPC_603:
    case PPC_603e:
    case PPC_603le:
    case PPC_603ev:
    /* case PPC_8240: same value as 8260 */
    case PPC_8260:
    case PPC_8245:
      return &mpc_603_category_table;
    case PPC_e300c1:
    case PPC_e300c2:
    case PPC_e300c3:
      return &e300_category_table;
    case PPC_PSIM:
      return &psim_category_table;
    case PPC_8540:
    case PPC_e500v2:
      return &e500_category_table;
    case PPC_e200z0:
    case PPC_e200z1:
    case PPC_e200z6:
      return &e200_category_table;
    case PPC_5XX:
      return &mpc_5xx_category_table;
    case PPC_860:
      return &mpc_860_category_table;
    case PPC_405:
    case PPC_405GP:
    case PPC_405EX:
      return &ppc_405_category_table;
    default:
      break;
  }

  return NULL;
}

ppc_exc_category ppc_exc_category_for_vector(const ppc_exc_categories *categories, unsigned vector)
{
  if (vector <= LAST_VALID_EXC) {
    return (*categories) [vector];
  } else {
    return PPC_EXC_INVALID;
  }
}
