/*
 * cvm_format.h - Format information for CVM instruction patterns.
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

#ifndef	_ENGINE_CVM_FORMAT_H
#define	_ENGINE_CVM_FORMAT_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Lengths of CVM instructions that have various kinds of arguments.
 */
#define	CVM_LEN_NONE					1
#define	CVM_LEN_BYTE					2
#define	CVM_LEN_BYTE2					3
#define	CVM_LEN_WORD					5
#define	CVM_LEN_WORD2					9
#define	CVM_LEN_PTR						(1 + sizeof(void *))
#define	CVM_LEN_PTR2					(1 + 2 * sizeof(void *))
#define CVM_LEN_WIDE_SMALL				2
#define	CVM_LEN_WIDE_LARGE				6
#define	CVM_LEN_WIDE_PTR_SMALL			(2 + sizeof(void *))
#define	CVM_LEN_WIDE_PTR_LARGE			(6 + sizeof(void *))
#define	CVM_LEN_DWIDE_SMALL				3
#define	CVM_LEN_DWIDE_LARGE				10
#define	CVM_LEN_DWIDE_PTR_SMALL			(3 + sizeof(void *))
#define	CVM_LEN_DWIDE_PTR_LARGE			(10 + sizeof(void *))
#define	CVM_LEN_LONG					9
#define	CVM_LEN_FLOAT					5
#define	CVM_LEN_DOUBLE					9
#define	CVM_LEN_BRANCH					6
#define	CVM_LEN_TRY						12
#define	CVMP_LEN_NONE					2
#define	CVMP_LEN_BYTE					3
#define	CVMP_LEN_WORD					6
#define	CVMP_LEN_PTR					(2 + sizeof(void *))
#define	CVMP_LEN_WORD_PTR				(6 + sizeof(void *))
#define	CVMP_LEN_WORD2_PTR				(10 + sizeof(void *))

/*
 * Extract particular arguments from CVM instructions.
 */
#define	CVM_ARG_SUB_OPCODE				(pc[1])
#define	CVM_ARG_BYTE					((ILUInt32)(ILUInt8)(pc[1]))
#define	CVM_ARG_SBYTE					((ILInt32)(ILInt8)(pc[1]))
#define	CVM_ARG_BYTE2					((ILUInt32)(ILUInt8)(pc[2]))
#define	CVM_ARG_SBYTE2					((ILInt32)(ILInt8)(pc[2]))
#define	CVM_ARG_WORD					(IL_READ_UINT32(pc + 1))
#define	CVM_ARG_WORD2					(IL_READ_UINT32(pc + 5))
#define	CVM_ARG_PTR(type)				((type)(ReadPointer(pc + 1)))
#define	CVM_ARG_PTR2(type)				\
			((type)(ReadPointer(pc + 1 + sizeof(void *))))
#define	CVM_ARG_WIDE_SMALL				CVM_ARG_BYTE
#define	CVM_ARG_WIDE_LARGE				(IL_READ_UINT32(pc + 2))
#define	CVM_ARG_WIDE_PTR_SMALL(type)	((type)(ReadPointer(pc + 2)))
#define	CVM_ARG_WIDE_PTR_LARGE(type)	((type)(ReadPointer(pc + 6)))
#define	CVM_ARG_DWIDE1_SMALL			CVM_ARG_BYTE
#define	CVM_ARG_DWIDE2_SMALL			CVM_ARG_BYTE2
#define	CVM_ARG_DWIDE1_LARGE			(IL_READ_UINT32(pc + 2))
#define	CVM_ARG_DWIDE2_LARGE			(IL_READ_UINT32(pc + 6))
#define	CVM_ARG_DWIDE_PTR_SMALL(type)	((type)(ReadPointer(pc + 3)))
#define	CVM_ARG_DWIDE_PTR_LARGE(type)	((type)(ReadPointer(pc + 10)))
#define	CVM_ARG_LONG					(IL_READ_INT64(pc + 1))
#define	CVM_ARG_FLOAT					(IL_READ_FLOAT(pc + 1))
#define	CVM_ARG_DOUBLE					(IL_READ_DOUBLE(pc + 1))
#define	CVM_ARG_BRANCH_SHORT			(pc + CVM_ARG_SBYTE)
#define	CVM_ARG_BRANCH_LONG				(pc + IL_READ_INT32(pc + 2))
#define CVM_ARG_JSR_RETURN				(pc + 6)
#define	CVM_ARG_SWITCH_LIMIT			(IL_READ_UINT32(pc + 1))
#define	CVM_ARG_SWITCH_DEFAULT			(pc + IL_READ_INT32(pc + 5))
#define	CVM_ARG_SWITCH_DEST(n)			(pc + IL_READ_INT32(pc + 9 + (n) * 4))
#define	CVM_ARG_CALL_RETURN(pc)			((pc) + sizeof(void *) + 1)
#define	CVM_ARG_CALLV_RETURN_SMALL(pc)	((pc) + 3)
#define	CVM_ARG_CALLV_RETURN_LARGE(pc)	((pc) + 10)
#define	CVM_ARG_CALLI_RETURN_SMALL(pc)	((pc) + 3 + sizeof(void *))
#define	CVM_ARG_CALLI_RETURN_LARGE(pc)	((pc) + 10 + sizeof(void *))
#define	CVM_ARG_TAIL_OPCODE				(pc[2])
#define	CVM_ARG_TAIL_METHOD				((ILMethod *)(ReadPointer(pc + 3)))
#define	CVM_ARG_TRY_START				(IL_READ_UINT32(pc))
#define	CVM_ARG_TRY_END					(IL_READ_UINT32(pc + 4))
#define	CVM_ARG_TRY_LENGTH				(IL_READ_UINT32(pc + 8))
#define	CVMP_ARG_BYTE					((ILUInt32)(ILUInt8)(pc[2]))
#define	CVMP_ARG_SBYTE					((ILInt32)(ILInt8)(pc[2]))
#define	CVMP_ARG_WORD					(IL_READ_UINT32(pc + 2))
#define	CVMP_ARG_WORD2					(IL_READ_UINT32(pc + 6))
#define	CVMP_ARG_PTR(type)				((type)(ReadPointer(pc + 2)))
#define	CVMP_ARG_WORD_PTR(type)			((type)(ReadPointer(pc + 6)))
#define	CVMP_ARG_WORD2_PTR(type)		((type)(ReadPointer(pc + 10)))

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_CVM_FORMAT_H */
