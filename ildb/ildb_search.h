/*
 * ildb_search.h - Support for searching for source files.
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

#ifndef	_ILDB_SEARCH_H
#define	_ILDB_SEARCH_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Destroy the source search path.
 */
void ILDbSearchDestroy(ILDb *db);

/*
 * Reset the source search path.
 */
void ILDbSearchReset(ILDb *db);

/*
 * Add a directory to the front of the source search path if
 * it is not already present.
 */
void ILDbSearchAdd(ILDb *db, const char *directory);

/*
 * Print the source search path.
 */
void ILDbSearchPrint(ILDb *db);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILDB_SEARCH_H */
