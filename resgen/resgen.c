/*
 * resgen.c - Resource file generator and reader.
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

#include <stdio.h>
#include "il_system.h"
#include "il_image.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Table of command-line options.
 */
static ILCmdLineOption const options[] = {
	{"-t", 't', 0, 0, 0},
	{"-r", 'r', 0, 0, 0},
	{"-i", 'i', 0, 0, 0},
	{"-x", 'x', 0, 0, 0},
	{"-T", 'T', 0, 0, 0},
	{"-R", 'R', 0, 0, 0},
	{"-X", 'X', 0, 0, 0},
	{"-s", 's', 0, 0, 0},
	{"-v", 'v', 0, 0, 0},
	{"--text-input", 't', 0,
		"--text-input  or -t",
		"Input files contain text resources."},
	{"--res-input", 'r', 0,
		"--res-input   or -r",
		"Input files contain binary resources."},
	{"--il-input", 'i', 0,
		"--il-input    or -i",
		"Input files contain IL images."},
	{"--xml-input", 'x', 0,
		"--xml-input   or -x",
		"Input files contain XML resources (not yet implemented)."},
	{"--text-output", 'T', 0,
		"--text-output or -T",
		"Write text resources to the output file."},
	{"--res-output", 'R', 0,
		"--res-output  or -R",
		"Write binary resources to the output file."},
	{"--xml-output", 'X', 0,
		"--xml-output  or -X",
		"Write XML resources to the output file (not yet implemented)."},
	{"--sort-names", 's', 0,
		"--sort-names  or -s",
		"Sort the resources by name before writing text output."},
	{"--version", 'v', 0,
		"--version     or -v",
		"Print the version of the program"},
	{"--help", 'h', 0,
		"--help",
		"Print this help message."},
	{0, 0, 0, 0, 0}
};

/*
 * Input and output formats.
 */
#define	FORMAT_GUESS		0
#define	FORMAT_TEXT			1
#define	FORMAT_RES_BINARY	2
#define	FORMAT_IL			3
#define	FORMAT_XML			4

static void usage(const char *progname);
static void version(void);
static int guessFormat(const char *filename, int isOutput);
static int loadResources(const char *filename, FILE *stream,
						 ILContext *context, int format);
static void writeTextResources(FILE *stream);
static void writeSortedTextResources(FILE *stream);
static void writeBinaryResources(FILE *stream);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	int inputFormat = FORMAT_GUESS;
	int outputFormat = FORMAT_GUESS;
	int sortNames = 0;
	char *outputFile = 0;
	int currFormat;
	int sawStdin;
	int state, opt;
	char *param;
	FILE *file;
	int errors;
	ILContext *context;

	/* Parse the command-line arguments */
	state = 0;
	while((opt = ILCmdLineNextOption(&argc, &argv, &state,
									 options, &param)) != 0)
	{
		switch(opt)
		{
			case 't':
			{
				inputFormat = FORMAT_TEXT;
			}
			break;

			case 'r':
			{
				inputFormat = FORMAT_RES_BINARY;
			}
			break;

			case 'i':
			{
				inputFormat = FORMAT_IL;
			}
			break;

			case 'x':
			{
				inputFormat = FORMAT_XML;
			}
			break;

			case 'T':
			{
				outputFormat = FORMAT_TEXT;
			}
			break;

			case 'R':
			{
				outputFormat = FORMAT_RES_BINARY;
			}
			break;

			case 'X':
			{
				outputFormat = FORMAT_XML;
			}
			break;

			case 's':
			{
				sortNames = 1;
			}
			break;

			case 'v':
			{
				version();
				return 0;
			}
			/* Not reached */

			default:
			{
				usage(progname);
				return 1;
			}
			/* Not reached */
		}
	}

	/* We need at least one input file and one output file argument */
	if(argc <= 2)
	{
		usage(progname);
		return 1;
	}

	/* Strip the output file from the command-line */
	outputFile = argv[argc - 1];
	--argc;

	/* Create a context to use for image loading */
	context = ILContextCreate();
	if(!context)
	{
		fprintf(stderr, "%s: out of memory\n", progname);
		return 1;
	}

	/* Load resource information from the input files */
	sawStdin = 0;
	errors = 0;
	while(argc > 1)
	{
		if(!strcmp(argv[1], "-"))
		{
			/* Dump the contents of stdin, but only once */
			if(!sawStdin)
			{
				currFormat = ((inputFormat != FORMAT_GUESS)
									? inputFormat : FORMAT_TEXT);
				errors |= loadResources("stdin", stdin, context, currFormat);
				sawStdin = 1;
			}
		}
		else
		{
			/* Dump the contents of a regular file */
			if((file = fopen(argv[1], "rb")) == NULL)
			{
				/* Try again in case libc did not understand the 'b' */
				if((file = fopen(argv[1], "r")) == NULL)
				{
					perror(argv[1]);
					errors = 1;
					++argv;
					--argc;
					continue;
				}
			}
			currFormat = ((inputFormat != FORMAT_GUESS)
								? inputFormat : guessFormat(argv[1], 0));
			errors |= loadResources(argv[1], file, context, currFormat);
			fclose(file);
		}
		++argv;
		--argc;
	}

	/* Destroy the context */
	ILContextDestroy(context);

	/* Bail out if there were errors while parsing the input files */
	if(errors)
	{
		return errors;
	}

	/* Write the resources to the output file */
	if(outputFormat == FORMAT_GUESS)
	{
		if(!strcmp(outputFile, "-"))
		{
			outputFormat = FORMAT_TEXT;
		}
		else
		{
			outputFormat = guessFormat(outputFile, 1);
		}
	}
	switch(outputFormat)
	{
		case FORMAT_TEXT:
		{
			if(!strcmp(outputFile, "-"))
			{
				/* Write text resources to stdout */
				if(sortNames)
				{
					writeSortedTextResources(stdout);
				}
				else
				{
					writeTextResources(stdout);
				}
			}
			else
			{
				/* Write text resources to a specified file */
				if((file = fopen(outputFile, "w")) == NULL)
				{
					perror(outputFile);
					return 1;
				}
				if(sortNames)
				{
					writeSortedTextResources(file);
				}
				else
				{
					writeTextResources(file);
				}
				fclose(file);
			}
		}
		break;

		case FORMAT_RES_BINARY:
		{
			if(!strcmp(outputFile, "-"))
			{
				/* Write binary resources to stdout */
				writeBinaryResources(stdout);
			}
			else
			{
				/* Write binary resources to a specified file */
				if((file = fopen(outputFile, "w")) == NULL)
				{
					perror(outputFile);
					return 1;
				}
				writeBinaryResources(file);
				fclose(file);
			}
		}
		break;

		case FORMAT_XML:
		{
			fprintf(stderr, "%s: XML output format is not supported yet\n",
					outputFile);
			return 1;
		}
		/* Not reached */

		default:
		{
			fprintf(stderr, "%s: unknown output resource format\n",
					outputFile);
			return 1;
		}
		/* Not reached */
	}
	
	/* Done */
	return 0;
}

static void usage(const char *progname)
{
	fprintf(stderr, "RESGEN " VERSION " - IL Resource Generation Utility\n");
	fprintf(stderr, "Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s [options] input ... output\n", progname);
	fprintf(stderr, "\n");
	ILCmdLineHelp(options);
}

static void version(void)
{

	printf("RESGEN " VERSION " - IL Resource Generation Utility\n");
	printf("Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	printf("\n");
	printf("RESGEN comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

static void outOfMemory(void)
{
	fputs("virtual memory exhausted\n", stderr);
	exit(1);
}

/*
 * Guess the format of a file from its extension.  Returns
 * FORMAT_GUESS if we cannot determine the format.
 */
static int guessFormat(const char *filename, int isOutput)
{
	int len = strlen(filename);
	while(len > 0 && filename[len - 1] != '.')
	{
		--len;
	}
	if(len <= 0)
	{
		return FORMAT_GUESS;
	}
	filename += len;
	if(!ILStrICmp(filename, "txt") ||
	   !ILStrICmp(filename, "text"))
	{
		return FORMAT_TEXT;
	}
	if(!ILStrICmp(filename, "resx") ||
	   !ILStrICmp(filename, "xml"))
	{
		return FORMAT_XML;
	}
	else if(!ILStrICmp(filename, "exe") ||
	        !ILStrICmp(filename, "dll") ||
			!ILStrICmp(filename, "obj") ||
			!ILStrICmp(filename, "o"))
	{
		return (isOutput ? FORMAT_GUESS : FORMAT_IL);
	}
	else if(!ILStrICmp(filename, "resources"))
	{
		return FORMAT_RES_BINARY;
	}
	else
	{
		return FORMAT_GUESS;
	}
}

/*
 * Global hash table, that holds all of the input strings.
 */
typedef struct _tagHashEntry HashEntry;
struct _tagHashEntry
{
	HashEntry   *next;
	int			 nameLen;
	int			 valueLen;
	const char  *filename;
	long		 linenum;
	long		 offset;
	char		 data[1];

};
#define	HASH_TABLE_SIZE		4096
static HashEntry *hashTable[HASH_TABLE_SIZE];
static unsigned long numStrings = 0;

/*
 * Add a resource to the global hash table.
 */
static int AddResource(const char *filename, long linenum,
					   const char *name, int nameLen,
					   const char *value, int valueLen)
{
	unsigned long hash;
	HashEntry *entry;
	int error;

	/* Is the name already in the hash table? */
	hash = (ILHashString(0, name, nameLen) & (HASH_TABLE_SIZE - 1));
	entry = hashTable[hash];
	while(entry != 0)
	{
		if(entry->nameLen == nameLen &&
		   !strncmp(entry->data, name, nameLen))
		{
			/* The value must be identical, or we report an error */
			if(entry->valueLen != valueLen ||
			   (valueLen != 0 &&
			    strncmp(entry->data + entry->nameLen, value, valueLen) != 0))
			{
				error = 1;
			}
			else
			{
				error = 0;
			}

			/* Print the message as either an error or a warning */
			if(linenum > 0)
			{
				fprintf(stderr, "%s:%ld", filename, linenum);
			}
			else
			{
				fputs(filename, stderr);
			}
			if(!error)
			{
				fputs(": warning", stderr);
			}
			fputs(": duplicate definition for `", stderr);
			fwrite(name, 1, nameLen, stderr);
			fputs("'\n", stderr);
			if(entry->linenum > 0)
			{
				fprintf(stderr, "%s:%ld", entry->filename, entry->linenum);
			}
			else
			{
				fputs(entry->filename, stderr);
			}
			fputs(": original definition here\n", stderr);

			/* Return the error state to the caller */
			return error;
		}
		entry = entry->next;
	}

	/* Create a new hash entry */
	entry = (HashEntry *)ILMalloc(sizeof(HashEntry) + nameLen + valueLen - 1);
	if(!entry)
	{
		outOfMemory();
	}
	entry->next = hashTable[hash];
	hashTable[hash] = entry;
	entry->nameLen = nameLen;
	entry->valueLen = valueLen;
	entry->filename = filename;
	entry->linenum = linenum;
	entry->offset = 0;
	if(nameLen)
	{
		ILMemCpy(entry->data, name, nameLen);
	}
	if(valueLen)
	{
		ILMemCpy(entry->data + nameLen, value, valueLen);
	}
	++numStrings;

	/* Done */
	return 0;
}

/*
 * Parse a hexadecimal escape.
 */
static unsigned long ParseHex(const char *inbuf, int inlen,
							  int hexlen, int *parsedlen)
{
	char tempch;
	unsigned long ch = 0;
	*parsedlen = 0;
	while(inlen > 0 && hexlen > 0)
	{
		tempch = *inbuf++;
		--inlen;
		if(tempch >= '0' && tempch <= '9')
		{
			ch = (ch * 16) + (tempch - '0');
		}
		else if(tempch >= 'A' && tempch <= 'F')
		{
			ch = (ch * 16) + (tempch - 'A' + 10);
		}
		else if(tempch >= 'a' && tempch <= 'f')
		{
			ch = (ch * 16) + (tempch - 'a' + 10);
		}
		else
		{
			break;
		}
		++(*parsedlen);
		--hexlen;
	}
	return ch;
}

/*
 * Convert escape sequences within a buffer into real characters.
 */
static int ConvertEscapes(const char *inbuf, int inlen, char *outbuf)
{
	int outlen = 0;
	unsigned long ch;
	int templen;

	while(inlen > 0)
	{
		if(*inbuf == '\\')
		{
			++inbuf;
			--inlen;
			if(!inlen)
			{
				break;
			}
			switch(*inbuf)
			{
				case 'n':
				{
					*outbuf++ = '\n';
					++outlen;
					++inbuf;
					--inlen;
				}
				break;

				case 'r':
				{
					*outbuf++ = '\r';
					++outlen;
					++inbuf;
					--inlen;
				}
				break;

				case 't':
				{
					*outbuf++ = '\t';
					++outlen;
					++inbuf;
					--inlen;
				}
				break;

				case 'f':
				{
					*outbuf++ = '\f';
					++outlen;
					++inbuf;
					--inlen;
				}
				break;

				case 'v':
				{
					*outbuf++ = '\v';
					++outlen;
					++inbuf;
					--inlen;
				}
				break;

				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				{
					/* Parse an octal character sequence */
					ch = (*inbuf++ - '0');
					--inlen;
					while(inlen > 0 && *inbuf >= '0' && *inbuf <= '7')
					{
						ch = (ch * 8) + (*inbuf++ - '0');
						--inlen;
					}

					/* Write out the character in UTF-8 form */
				writeUTF8:
					if(ch < 0x80)
					{
						*outbuf++ = (char)ch;
						++outlen;
					}
					else if(ch < (1 << 11))
					{
						*outbuf++ = (char)(0xC0 | ((ch >> 6) & 0x1F));
						*outbuf++ = (char)(0x80 | (ch & 0x3F));
						outlen += 2;
					}
					else if(ch < (1 << 16))
					{
						*outbuf++ = (char)(0xE0 | ((ch >> 12) & 0x0F));
						*outbuf++ = (char)(0x80 | ((ch >> 6) & 0x3F));
						*outbuf++ = (char)(0x80 | (ch & 0x3F));
						outlen += 3;
					}
					else if(ch < (1 << 21))
					{
						*outbuf++ = (char)(0xF0 | ((ch >> 18) & 0x07));
						*outbuf++ = (char)(0x80 | ((ch >> 12) & 0x3F));
						*outbuf++ = (char)(0x80 | ((ch >> 6) & 0x3F));
						*outbuf++ = (char)(0x80 | (ch & 0x3F));
						outlen += 4;
					}
					else if(ch < (1 << 26))
					{
						*outbuf++ = (char)(0xF8 | ((ch >> 24) & 0x03));
						*outbuf++ = (char)(0x80 | ((ch >> 18) & 0x3F));
						*outbuf++ = (char)(0x80 | ((ch >> 12) & 0x3F));
						*outbuf++ = (char)(0x80 | ((ch >> 6) & 0x3F));
						*outbuf++ = (char)(0x80 | (ch & 0x3F));
						outlen += 5;
					}
					else
					{
						*outbuf++ = (char)(0xFC | ((ch >> 30) & 0x03));
						*outbuf++ = (char)(0x80 | ((ch >> 24) & 0x3F));
						*outbuf++ = (char)(0x80 | ((ch >> 18) & 0x3F));
						*outbuf++ = (char)(0x80 | ((ch >> 12) & 0x3F));
						*outbuf++ = (char)(0x80 | ((ch >> 6) & 0x3F));
						*outbuf++ = (char)(0x80 | (ch & 0x3F));
						outlen += 6;
					}
				}
				break;

				case 'x': case 'X':
				{
					/* Parse a 2-digit hexadecimal character sequence */
					++inbuf;
					--inlen;
					ch = ParseHex(inbuf, inlen, 2, &templen);
					inbuf += templen;
					inlen -= templen;
					goto writeUTF8;
				}
				/* Not reached */

				case 'u':
				{
					/* Parse a 4-digit hexadecimal character sequence */
					++inbuf;
					--inlen;
					ch = ParseHex(inbuf, inlen, 4, &templen);
					inbuf += templen;
					inlen -= templen;
					goto writeUTF8;
				}
				/* Not reached */

				case 'U':
				{
					/* Parse an 8-digit hexadecimal character sequence */
					++inbuf;
					--inlen;
					ch = ParseHex(inbuf, inlen, 8, &templen);
					inbuf += templen;
					inlen -= templen;
					goto writeUTF8;
				}
				/* Not reached */

				default:
				{
					/* Regular character that has been escaped */
					*outbuf++ = *inbuf++;
					--inlen;
				}
				break;
			}
		}
		else
		{
			*outbuf++ = *inbuf++;
			++outlen;
			--inlen;
		}
	}
	return outlen;
}

/*
 * Determine if a character is white space.
 */
#define	IsWhiteSpace(ch)	\
			((ch) == '\n' || (ch) == '\r' || (ch) == '\t' || \
			 (ch) == '\f' || (ch) == '\v' || (ch) == ' ' || \
			 (ch) == 0x1A)

/*
 * Load text resources from an input file.
 */
static int loadTextResources(const char *filename, FILE *stream)
{
	char buffer[4096];
	char buffer2[4096];
	int posn, nameLen, valueLen;
	const char *name;
	const char *value;
	long linenum = 0;
	int error = 0;

	while(fgets(buffer, sizeof(buffer), stream))
	{
		/* Advance the line counter */
		++linenum;

		/* Strip trailing end of line characters */
		posn = strlen(buffer);
		while(posn > 0 &&
		      (buffer[posn - 1] == '\n' || buffer[posn - 1] == '\r'))
		{
			--posn;
		}
		buffer[posn] = '\0';

		/* Skip leading white space */
		posn = 0;
		while(buffer[posn] != '\0' && IsWhiteSpace(buffer[posn]))
		{
			++posn;
		}

		/* Ignore this line if it is empty or a comment */
		if(buffer[posn] == '\0' || buffer[posn] == '#')
		{
			continue;
		}

		/* Parse the line into "name=value" */
		name = buffer + posn;
		nameLen = 0;
		while(buffer[posn] != '\0' && buffer[posn] != '=')
		{
			++nameLen;
			++posn;
		}
		if(buffer[posn] == '\0' || nameLen == 0)
		{
			fprintf(stderr, "%s:%ld: invalid resource format\n",
					filename, linenum);
			error = 1;
			continue;
		}
		++posn;
		value = buffer + posn;
		valueLen = strlen(buffer + posn);

		/* Convert escape sequences in the value into real characters */
		valueLen = ConvertEscapes(value, valueLen, buffer2);
		value = buffer2;

		/* Add the resource to the hash table */
		error |= AddResource(filename, linenum, name, nameLen,
							 value, valueLen);
	}

	return error;
}

/*
 * Read a string from binary resource data.
 * Returns zero if the data is badly formatted.
 */
static int readString(const char *filename,
					  unsigned char **data, unsigned long *size,
					  char **str, int *len)
{
	unsigned char *d = *data;
	unsigned long s = *size;
	unsigned char ch;
	int shift;
	if(s != 0)
	{
		ch = *d++;
		--s;
		*len = (int)(ch & 0x7F);
		shift = 7;
		while((ch & (unsigned char)0x80) != 0)
		{
			if(s == 0)
			{
				goto invalid;
			}
			ch = *d++;
			--s;
			*len |= (((int)(ch & 0x7F)) << shift);
			shift += 7;
		}
		*str = (char *)d;
		if(((unsigned long)(*len)) <= s)
		{
			*data = d + *len;
			*size = s - (unsigned long)(*len);
			return 1;
		}
	}
invalid:
	fprintf(stderr, "%s: invalid binary resource string\n", filename);
	return 0;
}

/*
 * Parse binary resource data from a buffer.
 */
static int parseBinaryResources(const char *filename, unsigned char *data,
								unsigned long size)
{
	unsigned long numStrings;
	unsigned long stringNum;
	char *str, *str2;
	int len, len2;
	unsigned char *tempData;
	unsigned long tempSize;
	unsigned long tempNum;
	unsigned char *valueData;
	unsigned long valueSize;
	int error;
	static char const className1[] = "System.Resources.ResourceReader";

	/* Parse the name of the resource class */
	if(!readString(filename, &data, &size, &str, &len))
	{
		return 1;
	}
	if(len != ((int)(strlen(className1))) ||
	   ILMemCmp(str, className1, len) != 0)
	{
		/* This isn't a set of string resources, so skip it */
		return 0;
	}

	/* Parse the header */
	if(size < 8)
	{
		fprintf(stderr, "%s: truncated resource data\n", filename);
		return 1;
	}
	else if(IL_READ_UINT32(data) != 4)
	{
		fprintf(stderr, "%s: invalid resource version\n", filename);
		return 1;
	}
	numStrings = IL_READ_UINT32(data + 4);
	data += 8;
	size -= 8;

	/* Find the end of the string name table */
	tempData = data;
	tempSize = size;
	for(stringNum = 0; stringNum < numStrings; ++stringNum)
	{
		if(!readString(filename, &tempData, &tempSize, &str, &len))
		{
			return 1;
		}
		if(tempSize < 4)
		{
			fprintf(stderr,"%s: truncated resource data\n", filename);
			return 1;
		}
		tempData += 4;
		tempSize -= 4;
	}

	/* Skip the representation table */
	if(tempSize < 4)
	{
		fprintf(stderr,"%s: truncated resource data\n", filename);
		return 1;
	}
	tempNum = IL_READ_UINT32(tempData);
	tempData += 4;
	tempSize -= 4;
	while(tempNum > 0)
	{
		if(!readString(filename, &tempData, &tempSize, &str, &len))
		{
			return 1;
		}
		--tempNum;
	}
	valueData = tempData;
	valueSize = tempSize;

	/* Parse the name table again to extract the strings */
	error = 0;
	for(stringNum = 0; stringNum < numStrings; ++stringNum)
	{
		readString(filename, &data, &size, &str, &len);
		tempNum = IL_READ_UINT32(data) + 4;
		if(tempNum >= size)
		{
			fprintf(stderr, "%s: invalid string offset\n", filename);
			return 1;
		}
		tempData = valueData + tempNum;
		tempSize = valueSize - tempNum;
		if(!readString(filename, &tempData, &tempSize, &str2, &len2))
		{
			return 1;
		}
		error |= AddResource(filename, 0, str, len, str2, len2);
		data += 4;
		size -= 4;
	}

	/* Done */
	return error;
}

/*
 * Load binary resources from an input file.
 */
static int loadBinaryResources(const char *filename, FILE *stream)
{
	char buffer[BUFSIZ];
	unsigned char *data = 0;
	unsigned long dataLen = 0;
	unsigned long dataMax = 0;
	unsigned len;
	int error;

	/* Load the entire contents of the input file into memory */
	while((len = fread(buffer, 1, sizeof(buffer), stream)) > 0)
	{
		if((dataLen + len) > dataMax)
		{
			dataMax = (dataLen + len + 4095) & ~4095;
			if((data = (unsigned char *)ILRealloc(data, dataMax)) == 0)
			{
				outOfMemory();
			}
		}
		ILMemCpy(data + dataLen, buffer, len);
		dataLen += len;
		if(len < sizeof(buffer))
		{
			break;
		}
	}

	/* Check for the magic number */
	if(dataLen < 4 || IL_READ_UINT32(data) != (unsigned long)0xBEEFCACE)
	{
		fprintf(stderr, "%s: not a binary resource file\n", filename);
		if(data != 0)
		{
			ILFree(data);
		}
		return 1;
	}

	/* Parse the resources */
	error = parseBinaryResources(filename, data + 4, dataLen - 4);

	/* Free the memory that was used for the loaded data */
	ILFree(data);

	/* Done */
	return error;
}

/*
 * Load all string resources from the resource section
 * of an IL binary.
 */
static int loadResourceSection(const char *filename, unsigned char *address,
							   unsigned long size)
{
	unsigned long len;
	unsigned long magic;
	while(size >= 8)
	{
		/* Read the header for the next resource sub-section */
		len = IL_READ_UINT32(address);
		magic = IL_READ_UINT32(address + 4);
		if(len > (size - 4))
		{
			fprintf(stderr, "%s: badly formatted resources\n", filename);
			return 1;
		}
		address += 8;
		size -= 8;
		len -= 4;

		/* Skip if not string resources */
		if(magic != (unsigned long)0xBEEFCACE)
		{
			address += len;
			size -= len;
			continue;
		}

		/* Parse the binary resource data */
		if(parseBinaryResources(filename, address, len))
		{
			return 1;
		}
		address += len;
		size -= len;
	}
	return 0;
}

/*
 * Load the resources from a file that is in a specific format.
 */
static int loadResources(const char *filename, FILE *stream,
						 ILContext *context, int format)
{
	switch(format)
	{
		case FORMAT_TEXT:
		{
			return loadTextResources(filename, stream);
		}
		/* Not reached */

		case FORMAT_RES_BINARY:
		{
			return loadBinaryResources(filename, stream);
		}
		/* Not reached */

		case FORMAT_IL:
		{
			ILImage *image;
			int loadError;
			void *address;
			unsigned long size;

			/* Attempt to load the IL image */
			loadError = ILImageLoad(stream, filename, context, &image,
									IL_LOADFLAG_FORCE_32BIT |
									IL_LOADFLAG_NO_METADATA);
			if(loadError != 0)
			{
				fprintf(stderr, "%s: %s\n", filename,
						ILImageLoadError(loadError));
				return 1;
			}

			/* Extract and parse the resource section */
			if(ILImageGetSection(image, IL_SECTION_RESOURCES, &address, &size))
			{
				loadError = loadResourceSection
								(filename, (unsigned char *)address, size);
			}
			else
			{
				loadError = 0;
			}

			/* Clean up and exit */
			ILImageDestroy(image);
			return loadError;
		}
		/* Not reached */

		case FORMAT_XML:
		{
			fprintf(stderr, "%s: XML input resources are not supported yet\n",
					filename);
			return 1;
		}
		/* Not reached */
	}

	fprintf(stderr, "%s: unknown input resource format\n", filename);
	return 1;
}

/*
 * Write a single hash entry to an output stream.
 */
static void writeEntry(FILE *stream, HashEntry *entry)
{
	const char *str;
	int len, posn;
	unsigned long ch;
	int needEscape;

	fwrite(entry->data, 1, entry->nameLen, stream);
	putc('=', stream);
	str = entry->data + entry->nameLen;
	len = entry->valueLen;
	posn = 0;
	needEscape = 0;
	while(posn < len)
	{
		ch = ILUTF8ReadChar(str, len, &posn);
		switch(ch)
		{
			case '\n':	fputs("\\n", stream); needEscape = 0; break;
			case '\r':	fputs("\\r", stream); needEscape = 0; break;
			case '\t':	fputs("\\t", stream); needEscape = 0; break;
			case '\f':	fputs("\\f", stream); needEscape = 0; break;
			case '\v':	fputs("\\v", stream); needEscape = 0; break;
			case '\\':	fputs("\\\\", stream); needEscape = 0; break;
			case '\0':	fputs("\\0", stream); needEscape = 1; break;

			default:
			{
				if(ch < 0x20 || (ch >= 0x7F && ch <= 0xFF))
				{
					fprintf(stream, "\\x%02lX", ch);
					needEscape = 0;
				}
				else if(ch < 0x80)
				{
					if(needEscape && ch >= '0' && ch <= '7')
					{
						putc('\\', stream);
					}
					putc((int)ch, stream);
					needEscape = 0;
				}
				else if(ch < (unsigned long)0x10000)
				{
					fprintf(stream, "\\u%04lX", ch);
					needEscape = 0;
				}
				else
				{
					fprintf(stream, "\\U%08lX", ch);
					needEscape = 0;
				}
			}
			break;
		}
	}
	putc('\n', stream);
}

/*
 * Write text resources to an output stream.
 */
static void writeTextResources(FILE *stream)
{
	int hash;
	HashEntry *entry;
	for(hash = 0; hash < HASH_TABLE_SIZE; ++hash)
	{
		entry = hashTable[hash];
		while(entry != 0)
		{
			writeEntry(stream, entry);
			entry = entry->next;
		}
	}
}

/*
 * Compare the names of two hash entries.
 */
static int NameCompare(const void *e1, const void *e2)
{
	HashEntry *entry1 = *((HashEntry **)e1);
	HashEntry *entry2 = *((HashEntry **)e2);
	int cmp;
	if(entry1->nameLen == entry2->nameLen)
	{
		return strncmp(entry1->data, entry2->data, entry1->nameLen);
	}
	else if(entry1->nameLen < entry2->nameLen)
	{
		cmp = strncmp(entry1->data, entry2->data, entry1->nameLen);
		if(cmp != 0)
		{
			return cmp;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		cmp = strncmp(entry1->data, entry2->data, entry2->nameLen);
		if(cmp != 0)
		{
			return cmp;
		}
		else
		{
			return 1;
		}
	}
}

/*
 * Write text resources to an output stream, sorted by name.
 */
static void writeSortedTextResources(FILE *stream)
{
	int hash;
	HashEntry *entry;
	HashEntry **table;
	unsigned long posn;

	/* Bail out if there are no strings in the hash table */
	if(!numStrings)
	{
		return;
	}

	/* Allocate space for the string table */
	if((table = (HashEntry **)ILMalloc(sizeof(HashEntry *) * numStrings)) == 0)
	{
		outOfMemory();
	}

	/* Populate the table with the hash entries */
	posn = 0;
	for(hash = 0; hash < HASH_TABLE_SIZE; ++hash)
	{
		entry = hashTable[hash];
		while(entry != 0)
		{
			table[posn++] = entry;
			entry = entry->next;
		}
	}

	/* Sort the table into ascending order by name */
#if HAVE_QSORT
	qsort(table, numStrings, sizeof(HashEntry *), NameCompare);
#else
	{
		unsigned long posn2;
		for(posn = 0; posn < (numStrings - 1); ++posn)
		{
			for(posn2 = posn + 1; posn2 < numStrings; ++posn2)
			{
				if(NameCompare(&(table[posn]), &(table[posn2])) > 0)
				{
					entry = table[posn];
					table[posn] = table[posn2];
					table[posn2] = entry;
				}
			}
		}
	}
#endif

	/* Write out the entries */
	for(posn = 0; posn < numStrings; ++posn)
	{
		writeEntry(stream, table[posn]);
	}

	/* Free the table */
	ILFree(table);
}

/*
 * Determine the number of bytes that are necessary to represent a length.
 */
static int LengthSize(int len)
{
	int size = 1;
	while(len >= 0x80)
	{
		++size;
		len >>= 7;
	}
	return size;
}

/*
 * Write a 32-bit word to a binary output stream.
 */
static void writeWord(FILE *stream, unsigned long word)
{
	putc((word & 0xFF), stream);
	putc(((word >> 8) & 0xFF), stream);
	putc(((word >> 16) & 0xFF), stream);
	putc(((word >> 24) & 0xFF), stream);
}

/*
 * Write a length-specified string to a binary output stream.
 */
static void writeString(FILE *stream, const char *str, int len)
{
	int templen;

	/* Write out the length */
	templen = len;
	while(templen >= 0x80)
	{
		putc(((templen & 0x7F) | 0x80), stream);
		templen >>= 7;
	}
	putc(templen, stream);

	/* Write out the string data */
	if(len)
	{
		fwrite(str, 1, len, stream);
	}
}

static void writeBinaryResources(FILE *stream)
{
	int hash;
	long offset;
	long numStrings;
	HashEntry *entry;
	static char const className1[] = "System.Resources.ResourceReader";
	static char const className2[] = "System.String";

	/* Calculate the offsets of all strings, and the size
	   of the index table */
	offset = 0;
	numStrings = 0;
	for(hash = 0; hash < HASH_TABLE_SIZE; ++hash)
	{
		entry = hashTable[hash];
		while(entry != 0)
		{
			entry->offset = offset;
			offset += 4 + LengthSize(entry->valueLen) + entry->valueLen;
			++numStrings;
			entry = entry->next;
		}
	}

	/* Write out the header */
	writeWord(stream, (unsigned long)0xBEEFCACE);
	writeString(stream, className1, strlen(className1));
	writeWord(stream, (unsigned long)4);
	writeWord(stream, (unsigned long)numStrings);

	/* Write out the resource names */
	for(hash = 0; hash < HASH_TABLE_SIZE; ++hash)
	{
		entry = hashTable[hash];
		while(entry != 0)
		{
			writeString(stream, entry->data, entry->nameLen);
			writeWord(stream, (unsigned long)(entry->offset));
			entry = entry->next;
		}
	}

	/* Write the list of representation class names */
	writeWord(stream, (unsigned long)1);
	writeString(stream, className2, strlen(className2));

	/* Write out the resource values */
	for(hash = 0; hash < HASH_TABLE_SIZE; ++hash)
	{
		entry = hashTable[hash];
		while(entry != 0)
		{
			writeWord(stream, (unsigned long)0);
			writeString(stream, entry->data + entry->nameLen, entry->valueLen);
			entry = entry->next;
		}
	}
}

#ifdef	__cplusplus
};
#endif
