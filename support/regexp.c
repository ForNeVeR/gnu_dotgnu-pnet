/*
 * regexp.c - Simple regexp support
 * 
 * Copyright (C) 2002  Free Software Foundation, Inc,
 *
 * Contributed by Gopal V
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

#include <stdlib.h>
#include <stdio.h>
#include "il_config.h"
#include "il_utils.h"
#include "il_system.h"
#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#ifdef HAVE_REGEX_H
	#include <regex.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/* 
 * Regexp compile mapping on to regcomp 
 */
ILRegexpHandle ILRegexpCompile(char* pattern,int flags,int *error)
{
#ifdef HAVE_REGCOMP
	regex_t *r;	
  	r = (regex_t *) ILCalloc(1,sizeof(regex_t));
	if(!r)return NULL;
	*error=regcomp(r,pattern,flags);
	return (ILRegexpHandle)(r);
#else
	return NULL;
#endif
}

/*
 * Regexp execute mapping onto regexec
 */
int ILRegexpExec(ILRegexpHandle handle,char* input,int flags,
						ILRegexpMatch **match)
{
#ifdef HAVE_REGEXEC
	size_t no_sub = ((regex_t*)handle)->re_nsub+1; /* groups */
	regmatch_t *result;
	int retval=-1;
	if((result = (regmatch_t *) ILCalloc(no_sub,sizeof(regmatch_t)))==0)
	{
		return -1;
	}
	if((*match = (ILRegexpMatch*)ILCalloc(no_sub,sizeof(ILRegexpMatch)))==0)
	{
		return -1;
	}
	retval=regexec((regex_t*)(handle),input,no_sub,result,0);
	while(--no_sub)
	{
		(*match)[no_sub].start=result[no_sub].rm_so;
		(*match)[no_sub].end=result[no_sub].rm_eo;
	}
	ILFree(result);
	return retval;
#else
	return -1;
#endif
}

/*
 * Regexp error reporting function (auto alloc)
 */
char* ILRegexpError(int errorcode, ILRegexpHandle handle)
{
#ifdef HAVE_REGERROR
	size_t length; 
	char *buffer;
	length = regerror (errorcode, (regex_t*)(handle), NULL, 0);
	buffer = malloc(length);
	if(!buffer)return NULL;
	regerror (errorcode, (regex_t*)(handle), buffer, length);
	return buffer;
#else
	return NULL;
#endif
}

/*
 * Regexp free to free the handle
 */
void ILRegexpFree(ILRegexpHandle handle)
{
#ifdef HAVE_REGFREE
	regfree((regex_t*)(handle));
#else
#ifdef HAVE_REGEX_H
	ILFree((regex_t*)(handle)); /* attempt a normal free */
#endif
#endif
}

#ifdef	__cplusplus
};
#endif
