/*
 * c_scope.h - Scope handling for the C programming language.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#ifndef	_C_SCOPE_H
#define	_C_SCOPE_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Scope data item kinds.  Must not overlap with the values
 * defined in "codegen/cg_scope.h".
 */
#define	C_SCDATA_TYPEDEF			100
#define	C_SCDATA_STRUCT_OR_UNION	101
#define	C_SCDATA_ENUM				102
#define	C_SCDATA_LOCAL_VAR			103
#define	C_SCDATA_GLOBAL_VAR			104
#define	C_SCDATA_FUNCTION			105

/*
 * The current scope.
 */
extern ILScope *CCurrentScope;

/*
 * The global scope.
 */
extern ILScope *CGlobalScope;

/*
 * Initialize the global scope.
 */
void CScopeGlobalInit(void);

/*
 * Look up a name in the current scope.  Returns NULL if not found.
 */
void *CScopeLookup(const char *name);

/*
 * Determine if a name in the current scope is a typedef.
 */
int CScopeIsTypedef(const char *name);

/*
 * Look up a struct or union name in the current scope.
 */
void *CScopeLookupStructOrUnion(const char *name, int structKind);

/*
 * Determine if we already have a struct or union in with a specific
 * name in the current scope.
 */
int CScopeHasStructOrUnion(const char *name, int structKind);

/*
 * Add a type reference for a struct or union to the current scope.
 */
void CScopeAddStructOrUnion(const char *name, int structKind, ILType *type);

/*
 * Look up an enum name in the current scope.
 */
void *CScopeLookupEnum(const char *name);

/*
 * Determine if we already have an enum in with a specific
 * name in the current scope.
 */
int CScopeHasEnum(const char *name);

/*
 * Add a type reference for an enum to the current scope.
 */
void CScopeAddEnum(const char *name, ILType *type);

/*
 * Add a type definition to the current scope.
 */
void CScopeAddTypedef(const char *name, ILType *type, ILNode *node);

/*
 * Get the scope data kind.
 */
int CScopeGetKind(void *data);

/*
 * Get the type information associated with a scope data item.
 */
ILType *CScopeGetType(void *data);

#ifdef	__cplusplus
};
#endif

#endif	/* _C_SCOPE_H */
