/*
 * md_ppc.c - Machine-dependent definitions for PPC
 *
 * Copyright (C) 2004-2005  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "cvm_config.h"
#include "md_ppc.h"

#ifdef	__cplusplus
extern	"C" {
#endif

#ifdef CVM_PPC

md_inst_ptr _md_ppc_setcc(md_inst_ptr inst, int reg, int cond)
{
	TODO_trap(inst);
	return;
}

md_inst_ptr _md_ppc_setcmp(md_inst_ptr inst, int dreg)
{
	TODO_trap(inst);
	return inst;
}
#endif /* CVM_PPC */

#ifdef	__cplusplus
};
#endif
