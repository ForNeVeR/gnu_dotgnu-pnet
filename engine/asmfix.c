/*
 * asmfix.c - Fix gcc's output when it generates x86 code.
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

/*
The gcc compiler does a pretty poor job of compiling "cvm.c".
It is a little too smart for its own good when it optimizes the
switch loop.  Unfortunately, it isn't really practical to teach
gcc how to generate good interpreter code, and writing an
interpreter by hand in assembly code is simply too hard.

The approach that we take is to let gcc do the best that it
can, and then fix it up afterwards.  This gives us the benefit
of gcc's optimizer, without any of its drawbacks.

This program is hard-wired to optimize interpreter loops, and
in particular the loop in "cvm.c".  It isn't of any use for
optimizing other types of code.
*/

#include "il_system.h"
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * There's no point doing this if the CPU is not x86.
 */
#if defined(__i386) || defined(__i386__)

/*
 * Forward declarations.
 */
static int readInput(void);
static void sortLabels(void);
static int locateLoopTop(void);
static void findInlineLabels(void);
static void optimizeCode(void);

/*
 * Line buffer.
 */
static char **lines = 0;
static unsigned long numLines = 0;
static unsigned long maxLines = 0;
static char *currBuffer = 0;
#define	CURR_BUFFER_MAX		2048
static int currBufferPosn = CURR_BUFFER_MAX;

/*
 * Label buffer.
 */
static unsigned long *labels = 0;
static unsigned long numLabels = 0;
static unsigned long maxLabels = 0;
#define	LABEL_INLINE_BIT	((unsigned long)0x80000000)

/*
 * The labels that sit at the top of the interpreter loop.
 */
static char *topLabel;
static int   topLabelLen;
static char *tableLabel;
static int   tableLabelLen;

int main(int argc, char *argv[])
{
	/* Read the entire assembly stream into memory */
	if(!readInput())
	{
		fputs("virtual memory exhausted\n", stderr);
		return 1;
	}

	/* Sort the label array to make it easier to find labels later */
	sortLabels();

	/* Locate the top of the interpreter loop */
	if(!locateLoopTop())
	{
		unsigned long line;
		fprintf(stderr, "%s: could not locate the interpreter loop\n", argv[0]);
		fprintf(stderr, "(continuing with the original assembly code)\n");
		for(line = 0; line < numLines; ++line)
		{
			fputs(lines[line], stdout);
		}
	}

	/* Look for any labels that seem to be inlining candidates */
	findInlineLabels();

	/* Output the code while performing optimizations */
	optimizeCode();

	/* Done */
	return 0;
}

/*
 * Read the assembly stream from stdin into memory.
 */
static int readInput(void)
{
	char buffer[BUFSIZ];
	int len;
	char *buf;
	while(fgets(buffer, BUFSIZ, stdin))
	{
		/* Skip the line if it contains debug information because
		   the debug info will be useless after optimization */
		if(!strncmp(buffer, ".stab", 5) ||
		   !strncmp(buffer, "\t.stab", 6) ||
		   !strncmp(buffer, ".LM", 3) ||
		   !strncmp(buffer, ".LBB", 4))
		{
			continue;
		}

		/* Copy the line into a permanent buffer */
		len = strlen(buffer) + 1;
		if((currBufferPosn + len) <= CURR_BUFFER_MAX)
		{
			/* Add to the end of the current buffer */
			strcpy(currBuffer + currBufferPosn, buffer);
			buf = currBuffer + currBufferPosn;
			currBufferPosn += len;
		}
		else if(len >= CURR_BUFFER_MAX)
		{
			/* Allocate the entire line in its own buffer */
			buf = (char *)ILMalloc(len);
			if(!buf)
			{
				return 0;
			}
			strcpy(buf, buffer);
		}
		else
		{
			/* Start a new buffer */
			buf = (char *)ILMalloc(CURR_BUFFER_MAX);
			if(!buf)
			{
				return 0;
			}
			strcpy(buf, buffer);
			currBuffer = buf;
			currBufferPosn = len;
		}

		/* Add the permanent buffer to the line index array */
		if(numLines < maxLines)
		{
			lines[numLines++] = buf;
		}
		else
		{
			lines = (char **)ILRealloc(lines, sizeof(char *) *
									   (maxLines + 1024));
			if(!lines)
			{
				return 0;
			}
			maxLines += 1024;
			lines[numLines++] = buf;
		}

		/* Record this line if it is a label */
		if(buf[0] == '.' && buf[1] == 'L')
		{
			if(numLabels < maxLabels)
			{
				labels[numLabels++] = numLines - 1;
			}
			else
			{
				labels = (unsigned long*)ILRealloc
					(labels, sizeof(unsigned long) * (maxLabels + 1024));
				if(!labels)
				{
					return 0;
				}
				maxLabels += 1024;
				labels[numLabels++] = numLines - 1;
			}
		}
	}
	return 1;
}

#ifdef HAVE_QSORT

/*
 * Compare two labels for sorting purposes.
 */
static int compareLabels(const void *e1, const void *e2)
{
	unsigned long posn = *((unsigned long *)e1);
	unsigned long posn2 = *((unsigned long *)e2);
	return strcmp(lines[posn], lines[posn2]);
}

#endif

/*
 * Sort the label array into ascending order.
 */
static void sortLabels(void)
{
#ifdef HAVE_QSORT
	if(numLabels >= 2)
	{
		qsort(labels, numLabels, sizeof(unsigned long), compareLabels);
	}
#else
	unsigned long posn;
	unsigned long posn2;
	unsigned long temp;
	if(numLabels < 2)
	{
		return;
	}
	for(posn = 0; posn < (numLabels - 1); ++posn)
	{
		for(posn2 = (posn + 1); posn2 < numLabels; ++posn2)
		{
			if(strcmp(lines[labels[posn]], lines[labels[posn2]]) > 0)
			{
				temp = labels[posn];
				labels[posn] = labels[posn2];
				labels[posn2] = temp;
			}
		}
	}
#endif
}

/*
 * Locate the top of the interpreter loop, where control
 * always returns after processing an instruction.
 */
static int locateLoopTop(void)
{
	unsigned long line = 0;
	while(line < (numLines - 5))
	{
		if(lines[line][0] == '.' && lines[line][1] == 'L' &&
		   !strcmp(lines[line + 1], "\tmovzbl\t(%esi), %eax\n") &&
		   !strcmp(lines[line + 2], "\tcmpl\t$255, %eax\n") &&
		   !strncmp(lines[line + 3], "\tja\t.L", 6) &&
		   !strncmp(lines[line + 4], "\tjmp\t*.L", 8))
		{
			/* We've found the top of the loop */
			topLabel = ILDupString(lines[line]);
			topLabelLen = 2;
			while(topLabel[topLabelLen] != ':')
			{
				++topLabelLen;
			}
			topLabel[topLabelLen] = '\0';
			tableLabel = ILDupString(lines[line + 4] + 6);
			tableLabelLen = 2;
			while(tableLabel[tableLabelLen] != '(')
			{
				++tableLabelLen;
			}
			tableLabel[tableLabelLen] = '\0';
			return 1;
		}
		++line;
	}
	return 0;
}

/*
 * Find labels that appear to be inlining candidates.
 */
static void findInlineLabels(void)
{
	unsigned long label;
	unsigned long line;
	int ok;
	for(label = 0; label < numLabels; ++label)
	{
		/* Get the starting line for the label's code */
		line = (labels[label] & ~LABEL_INLINE_BIT) + 1;

		/* Look for zero or more inlinable instructions,
		   followed by an unconditional jump */
		ok = 1;
		while(line < numLines && ok)
		{
			if(!strncmp(lines[line], "\tjmp\t", 5))
			{
				break;
			}
			if(!strcmp(lines[line], "\tincl\t%esi\n") ||
			   !strcmp(lines[line], "\taddl\t$2, %esi\n") ||
			   !strcmp(lines[line], "\taddl\t$3, %esi\n") ||
			   !strcmp(lines[line], "\taddl\t$4, %esi\n") ||
			   !strcmp(lines[line], "\taddl\t$5, %esi\n") ||
			   !strcmp(lines[line], "\taddl\t$6, %esi\n") ||
			   !strcmp(lines[line], "\taddl\t$7, %esi\n") ||
			   !strcmp(lines[line], "\taddl\t$8, %esi\n") ||
			   !strcmp(lines[line], "\taddl\t$9, %esi\n") ||
			   !strcmp(lines[line], "\taddl\t$4, %edi\n") ||
			   !strcmp(lines[line], "\taddl\t$8, %edi\n") ||
			   !strcmp(lines[line], "\taddl\t$12, %edi\n") ||
			   !strcmp(lines[line], "\taddl\t$16, %edi\n") ||
			   !strcmp(lines[line], "\taddl\t$-4, %edi\n") ||
			   !strcmp(lines[line], "\taddl\t$-8, %edi\n") ||
			   !strcmp(lines[line], "\taddl\t$-12, %edi\n") ||
			   !strcmp(lines[line], "\taddl\t$-16, %edi\n") ||
			   !strcmp(lines[line], "\tsubl\t$4, %edi\n") ||
			   !strcmp(lines[line], "\tsubl\t$8, %edi\n") ||
			   !strcmp(lines[line], "\tsubl\t$12, %edi\n") ||
			   !strcmp(lines[line], "\tsubl\t$16, %edi\n"))
			{
				ok = 1;
				++line;
			}
			else
			{
				ok = 0;
			}
		}

		/* Mark the label as inlinable if still ok */
		if(ok)
		{
			labels[label] |= LABEL_INLINE_BIT;
		}
	}
}

/*
 * Compare two label names.
 */
static int compareLabelNames(char *name1, int len, char *name2)
{
	while(len > 0 && *name2 != ':')
	{
		if(*name1 < *name2)
		{
			return -1;
		}
		else if(*name1 > *name2)
		{
			return 1;
		}
		++name1;
		++name2;
		--len;
	}
	if(len > 0)
	{
		if(*name1 < *name2)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	else if(*name2 != ':')
	{
		if(*name2 > ':')
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

/*
 * Determine if a label is inlinable and get its starting line.
 */
static int canInline(char *name, int len, unsigned long *line)
{
	unsigned long left, right, middle;
	unsigned long temp;
	int cmp;
	left = 0;
	right = numLabels - 1;
	while(left <= right)
	{
		middle = (left + right) / 2;
		temp = labels[middle] & ~LABEL_INLINE_BIT;
		cmp = compareLabelNames(name, len, lines[temp]);
		if(cmp < 0)
		{
			right = middle - 1;
		}
		else if(cmp > 0)
		{
			left = middle + 1;
		}
		else
		{
			if((labels[middle] & LABEL_INLINE_BIT) != 0)
			{
				*line = temp + 1;
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

/*
 * Optimize the assembly code and write it to stdout.
 */
static void optimizeCode(void)
{
	unsigned long line;
	unsigned long startLine;
	int sawLoopTop = 0;
	line = 0;
	while(line < numLines)
	{
		if(!sawLoopTop && lines[line][0] == '.' && lines[line][1] == 'L' &&
		   !strncmp(lines[line], topLabel, topLabelLen) &&
		   lines[line][topLabelLen] == ':')
		{
			/* Output a more efficient table lookup at the loop top */
			fputs(lines[line], stdout);
			printf("\tmovzbl\t(%%esi), %%eax\n");
			printf("\tjmp\t*%s(,%%eax,4)\n", tableLabel);

			/* Skip code until we've passed the original "jmp*" */
			while(line < numLines &&
			      strncmp(lines[line], "\tjmp\t*.L", 8) != 0)
			{
				++line;
			}
			++line;

			/* We don't need to do this again */
			sawLoopTop = 1;
		}
		else if(!strncmp(lines[line], "\tjmp\t.L", 7))
		{
			/* Unconditional jump */
			if(!strncmp(lines[line] + 5, topLabel, topLabelLen) &&
			   lines[line][5 + topLabelLen] == '\n')
			{
				/* Jump to the loop top: inline a table lookup here */
				printf("\tmovzbl\t(%%esi), %%eax\n");
				printf("\tjmp\t*%s(,%%eax,4)\n", tableLabel);
			}
			else if(canInline(lines[line] + 5, strlen(lines[line]) - 6,
							  &startLine))
			{
				/* Inline the code at the jumped-to label */
				for(;;)
				{
					if(!strncmp(lines[startLine], "\tjmp\t.L", 7))
					{
						/* Unconditional jump to another block */
						if(!strncmp(lines[startLine] + 5, topLabel,
									topLabelLen) &&
						   lines[startLine][5 + topLabelLen] == '\n')
						{
							/* Jump to the loop top: inline a table lookup */
							printf("\tmovzbl\t(%%esi), %%eax\n");
							printf("\tjmp\t*%s(,%%eax,4)\n", tableLabel);
							break;
						}
						else if(!canInline(lines[startLine] + 5,
										   strlen(lines[startLine]) - 6,
										   &startLine))
						{
							/* End of code that we can inline */
							fputs(lines[startLine], stdout);
							break;
						}
					}
					else
					{
						/* Ordinary inlinable line */
						fputs(lines[startLine], stdout);
						++startLine;
					}
				}
			}
			else
			{
				/* Jump to somewhere that cannot be inlined */
				fputs(lines[line], stdout);
			}
			++line;
		}
		else
		{
			/* Output an ordinary line of code */
			fputs(lines[line], stdout);
			++line;
		}
	}
}

#else	/* !x86 */

/*
 * We don't know how to optimize the code, so copy it through as-is.
 */
int main(int argc, char *argv[])
{
	char buffer[BUFSIZ];
	while(fgets(buffer, BUFSIZ, stdin))
	{
		fputs(buffer, stdout);
	}
	return 0;
}

#endif	/* !x86 */

#ifdef	__cplusplus
};
#endif
