/*
 * ildasm_main.c - Main entry point for "ildasm".
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

#include "ildasm_internal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Table of command-line options.
 */
static ILCmdLineOption const options[] = {
	{"-o", 'o', 1, 0, 0},
	{"-d", 'd', 0, 0, 0},
	{"-r", 'r', 0, 0, 0},
	{"-t", 't', 0, 0, 0},
	{"-q", 'q', 0, 0, 0},
	{"-w", 'w', 0, 0, 0},
	{"-v", 'v', 0, 0, 0},
	{"--output", 'o', 1,
		"--output file    or    -o file",
		"Specify the output file to use.  The default is stdout."},
	{"--dump-sections", 'd', 0,
		"--dump-sections  or    -d",
		"Dump the contents of the IL sections in hexadecimal."},
	{"--real-offsets", 'r', 0,
		"--real-offsets   or    -r",
		"Display real file offsets instead of virtual addresses."},
	{"--show-tokens", 't', 0,
		"--show-tokens    or    -t",
		"Show token codes in output."},
	{"--quote-names", 'q', 0,
		"--quote-names    or    -q",
		"Quote all names in output."},
	{"--whole-file", 'w', 0,
		"--whole-file     or    -w",
		"Dump the contents of the entire file in hexadecimal."},
	{"--version", 'v', 0,
		"--version        or    -v",
		"Print the version of the program"},
	{"--help", 'h', 0,
		"--help",
		"Print this help message."},
	{0, 0, 0, 0, 0}
};

static FILE *outstream;

static void usage(const char *progname);
static void version(void);
static int dumpFile(const char *filename, FILE *stream,
					int closeStream, ILContext *context,
					int dumpSections, int realOffsets);
static void dumpWholeFile(const char *filename, FILE *stream);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	char *outputFile = 0;
	int dumpSections = 0;
	int wholeFile = 0;
	int flags = 0;
	int sawStdin;
	int state, opt;
	char *param;
	FILE *infile;
	int errors;
	ILContext *context;

	/* Parse the command-line arguments */
	state = 0;
	while((opt = ILCmdLineNextOption(&argc, &argv, &state,
									 options, &param)) != 0)
	{
		switch(opt)
		{
			case 'o':
			{
				outputFile = param;
			}
			break;

			case 'd':
			{
				dumpSections = 1;
			}
			break;

			case 'r':
			{
				flags |= ILDASM_REAL_OFFSETS;
			}
			break;

			case 't':
			{
				flags |= IL_DUMP_SHOW_TOKENS;
			}
			break;

			case 'q':
			{
				flags |= IL_DUMP_QUOTE_NAMES;
			}
			break;

			case 'w':
			{
				wholeFile = 1;
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

	/* We need at least one input file argument */
	if(argc <= 1)
	{
		usage(progname);
		return 1;
	}

	/* Open the output stream */
	if(outputFile && strcmp(outputFile, "-") != 0)
	{
		if((outstream = fopen(outputFile, "w")) == NULL)
		{
			perror(outputFile);
			return 1;
		}
	}
	else
	{
		outstream = stdout;
	}

	/* Create a context to use for image loading */
	context = ILContextCreate();
	if(!context)
	{
		fprintf(stderr, "%s: out of memory\n", progname);
		return 1;
	}

	/* Load and dump the input files */
	sawStdin = 0;
	errors = 0;
	while(argc > 1)
	{
		if(!strcmp(argv[1], "-"))
		{
			/* Dump the contents of stdin, but only once */
			if(!sawStdin)
			{
				if(!wholeFile)
				{
					errors |= dumpFile("stdin", stdin, 0, context,
									   dumpSections, flags);
				}
				else
				{
					dumpWholeFile("stdin", stdin);
				}
				sawStdin = 1;
			}
		}
		else
		{
			/* Dump the contents of a regular file */
			if((infile = fopen(argv[1], "rb")) == NULL)
			{
				/* Try again in case libc did not understand the 'b' */
				if((infile = fopen(argv[1], "r")) == NULL)
				{
					perror(argv[1]);
					errors = 1;
					++argv;
					--argc;
					continue;
				}
			}
			if(!wholeFile)
			{
				errors |= dumpFile(argv[1], infile, 1, context,
								   dumpSections, flags);
			}
			else
			{
				dumpWholeFile(argv[1], infile);
				fclose(infile);
			}
		}
		++argv;
		--argc;
	}

	/* Destroy the context */
	ILContextDestroy(context);
	
	/* Close the output stream */
	if(outstream != stdout)
	{
		fclose(outstream);
	}

	/* Done */
	return errors;
}

static void usage(const char *progname)
{
	fprintf(stderr, "ILDASM " VERSION " - Intermediate Language Disassembler\n");
	fprintf(stderr, "Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s [options] input ...\n", progname);
	fprintf(stderr, "\n");
	ILCmdLineHelp(options);
}

static void version(void)
{

	printf("ILDASM " VERSION " - Intermediate Language Disassembler\n");
	printf("Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	printf("\n");
	printf("ILDASM comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

/*
 * Dump a block of bytes in hex.
 */
static void dumpBlockInHex(unsigned long virtAddr,
						   unsigned char *addr,
						   unsigned long size)
{
	unsigned long offset;
	unsigned long posn;
	int ch;

	offset = 0;
	while(offset < size)
	{
		fprintf(outstream, "%08lx: ", offset + virtAddr);
		for(posn = 0; posn < 16 && (offset + posn) < size; ++posn)
		{
			fprintf(outstream, "%02x ", (int)(addr[posn]));
		}
		while(posn < 16)
		{
			fputs("   ", outstream);
			++posn;
		}
		putc(' ', outstream);
		for(posn = 0; posn < 16 && (offset + posn) < size; ++posn)
		{
			ch = (int)(addr[posn]);
			if(ch >= ' ' && ch < 0x7F)
			{
				putc(ch, outstream);
			}
			else
			{
				putc('.', outstream);
			}
		}
		putc('\n', outstream);
		offset += 16;
		addr += 16;
	}
}

/*
 * Dump section data in hex.
 */
static void dumpSectionInHex(ILImage *image, int section,
							 const char *name, int realOffsets)
{
	void *address;
	unsigned long size;
	unsigned long virtAddr;

	/* Get the section data */
	if(!ILImageGetSection(image, section, &address, &size) || !size)
	{
		fprintf(outstream, "%s: not present\n\n", name);
		return;
	}
	virtAddr = ILImageGetSectionAddr(image, section);
	if(realOffsets)
	{
		virtAddr = ILImageRealOffset(image, virtAddr);
	}

	/* Output the section header */
	fprintf(outstream, "%s (%lu bytes):\n\n", name, size);

	/* Dump the section data in hex */
	dumpBlockInHex(virtAddr, (unsigned char *)address, size);

	/* Output the section footer */
	putc('\n', outstream);
}

/*
 * Dump the metadata section by splitting it into separate blobs.
 */
static void dumpMetadataSection(ILImage *image, int realOffsets)
{
	void *address;
	unsigned long size;
	unsigned long virtAddr;
	unsigned long numEntries;
	unsigned long entry;
	char *name;

	/* Dump the header */
	if(!ILImageGetSection(image, IL_SECTION_METADATA, &address, &size) || !size)
	{
		fprintf(outstream, "Metadata Section is not present\n\n");
		return;
	}
	virtAddr = ILImageGetSectionAddr(image, IL_SECTION_METADATA);
	size = ILImageMetaHeaderSize(image);
	fprintf(outstream, "Metadata Header (%lu bytes):\n\n", size);
	if(realOffsets)
	{
		virtAddr = ILImageRealOffset(image, virtAddr);
	}
	dumpBlockInHex(virtAddr, (unsigned char *)address, size);
	putc('\n', outstream);

	/* Dump each of the entries in turn */
	numEntries = ILImageNumMetaEntries(image);
	for(entry = 0; entry < numEntries; ++entry)
	{
		address = ILImageMetaEntryInfo(image, entry, &name, &virtAddr, &size);
		fprintf(outstream, "Metadata Blob %s (%lu bytes):\n\n", name, size);
		if(realOffsets)
		{
			virtAddr = ILImageRealOffset(image, virtAddr);
		}
		dumpBlockInHex(virtAddr, (unsigned char *)address, size);
		putc('\n', outstream);
	}
}

/*
 * Dump all sections in the image.
 */
static void dumpSectionData(ILImage *image, int realOffsets)
{
	/* Dump the program header */
	dumpSectionInHex(image, IL_SECTION_HEADER, "Header", realOffsets);

	/* Dump the code section */
	dumpSectionInHex(image, IL_SECTION_CODE, "Code Section", realOffsets);

	/* Dump the contents of the metadata section */
	dumpMetadataSection(image, realOffsets);

	/* Dump the misc sections */
	dumpSectionInHex(image, IL_SECTION_RESOURCES,
					 "Resources Section", realOffsets);
	dumpSectionInHex(image, IL_SECTION_STRONG_NAMES,
					 "Strong Name Signature Section", realOffsets);
	dumpSectionInHex(image, IL_SECTION_CODE_MANAGER,
					 "Code Manager Table Section", realOffsets);
	dumpSectionInHex(image, IL_SECTION_DEBUG,
					 "Debug Section", realOffsets);
}

/*
 * Dump the assembly version of an image.
 */
static void dumpAssembly(ILImage *image, int flags)
{
	/* Dump global definitions */
	ILDAsmDumpGlobal(image, outstream, flags);

	/* Dump class definitions */
	ILDAsmDumpClasses(image, outstream, flags);
}

/*
 * Dump the contents of a file.
 */
static int dumpFile(const char *filename, FILE *stream,
					int closeStream, ILContext *context,
					int dumpSections, int flags)
{
	int loadError;
	ILImage *image;
	int imageType;

	/* Attempt to load the image into memory */
	loadError = ILImageLoad(stream, filename, context, &image,
							IL_LOADFLAG_FORCE_32BIT |
							IL_LOADFLAG_PRE_VALIDATE |
							IL_LOADFLAG_NO_RESOLVE |
							(dumpSections ? IL_LOADFLAG_NO_METADATA : 0));
	if(loadError != 0)
	{
		if(closeStream)
		{
			fclose(stream);
		}
		fprintf(stderr, "%s: %s\n", filename, ILImageLoadError(loadError));
		return 1;
	}

	/* Close the input stream, because we don't need it any more */
	if(closeStream)
	{
		fclose(stream);
	}

	/* Dump interesting information about the image in general */
	fprintf(outstream, "// Input: %s\n", filename);
	imageType = ILImageType(image);
	fprintf(outstream, "// Image type: %s\n",
		    (imageType == IL_IMAGETYPE_DLL ? "DLL" :
				(imageType == IL_IMAGETYPE_EXE ? "EXE" :
					(imageType == IL_IMAGETYPE_OBJ ? "OBJ" : "Unknown"))));
	fprintf(outstream, "// Native code present: %s\n",
		    (ILImageHadNative(image) ? "Yes" : "No"));
	fprintf(outstream, "// 32-bit only: %s\n",
		    (ILImageIs32Bit(image) ? "Yes" : "No"));
	fprintf(outstream, "// Length of IL data: %lu\n", ILImageLength(image));
	fprintf(outstream, "\n");

	/* How should we dump the image? */
	if(dumpSections)
	{
		dumpSectionData(image, (flags & ILDASM_REAL_OFFSETS) != 0);
	}
	else
	{
		dumpAssembly(image, flags);
	}

	/* Clean up and exit */
	ILImageDestroy(image);
	return 0;
}

/*
 * Dump the whole contents of a file in hexadecimal.
 */
static void dumpWholeFile(const char *filename, FILE *stream)
{
	unsigned char buffer[1024];
	unsigned size;
	unsigned long addr;
	fprintf(outstream, "// Input: %s\n\n", filename);
	addr = 0;
	while((size = fread(buffer, 1, sizeof(buffer), stream)) != 0)
	{
		dumpBlockInHex(addr, buffer, (unsigned long)size);
		addr += (unsigned long)size;
		if(size != sizeof(buffer))
		{
			break;
		}
	}
}

#ifdef	__cplusplus
};
#endif
