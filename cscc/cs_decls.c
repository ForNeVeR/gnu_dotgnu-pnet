/*
 * cs_decls.c - Type gathering and semantic analysis for C# declaration types.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

#include "cs_internal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Perform semantic analysis for the "using namespace" directive.
 */
DECLARE_SEM(UsingNamespace)
{
}

/*
 * Perform semantic analysis before visiting a class definition.
 */
DECLARE_SEM(ClassDefn_Before)
{
}

/*
 * Perform semantic analysis after visiting a class definition.
 */
DECLARE_SEM(ClassDefn_After)
{
}

/*
 * Ignore a "GenDiscard" request on a node.
 */
static void Ignore_GenDiscard(ILNode *node, ILGenInfo *info)
{
	/* Nothing to be done here */
}

/*
 * Node class for "using namespace" statements.
 */
ILNodeClass const ILNodeClass_UsingNamespace = {
	"using_namespace",
	ILNodeNotImpGetType,
	Ignore_GenDiscard,
	ILNodeNotImpGenValue,
	ILNodeNotImpGenThen,
	ILNodeNotImpGenElse,
	ILNodeNotImpPrepare,
	ILNodeNotImpGetAndPrepare,
	ILNodeNotImpStore,
	ILNodeNotImpEvalConst,
};

#ifdef	__cplusplus
};
#endif
