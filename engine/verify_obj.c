/*
 * verify_obj.c - Verify instructions related to objects.
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

#if defined(IL_VERIFY_GLOBALS)

/* No globals required */

#elif defined(IL_VERIFY_LOCALS)

/* No locals required */

#else /* IL_VERIFY_CODE */

case IL_OP_CPOBJ:
{
	/* TODO */
}
break;

case IL_OP_CASTCLASS:
{
	/* TODO */
}
break;

case IL_OP_ISINST:
{
	/* TODO */
}
break;

case IL_OP_BOX:
{
	/* TODO */
}
break;

case IL_OP_UNBOX:
{
	/* TODO */
}
break;

case IL_OP_LDFLD:
{
	/* TODO */
}
break;

case IL_OP_LDFLDA:
{
	/* TODO */
}
break;

case IL_OP_STFLD:
{
	/* TODO */
}
break;

case IL_OP_LDSFLD:
{
	/* TODO */
}
break;

case IL_OP_LDSFLDA:
{
	/* TODO */
}
break;

case IL_OP_STSFLD:
{
	/* TODO */
}
break;

case IL_OP_REFANYVAL:
{
	/* TODO */
}
break;

case IL_OP_MKREFANY:
{
	/* TODO */
}
break;

case IL_OP_LDTOKEN:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_ARGLIST:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_INITOBJ:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_CPBLK:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_INITBLK:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_SIZEOF:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_REFANYTYPE:
{
	/* TODO */
}
break;

#endif /* IL_VERIFY_CODE */
