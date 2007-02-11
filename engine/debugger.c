/*
 * debugger.c - debugger support for execution engine.
 *
 * Copyright (C) 2006  Southern Storm Software, Pty Ltd.
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
#include <ctype.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "il_sysio.h"
#include "il_engine.h"
#include "il_debug.h"
#include "il_dumpasm.h"
#include "engine.h"
#include "cvm.h"
#include "debugger.h"

#ifdef IL_USE_JIT
#include <jit/jit-dump.h>
#include "jitc.h"
#endif

#ifdef IL_CONFIG_DEBUGGER

/*
 * Lock the debugger object.
 */
#define	LockDebugger(debugger)		ILMutexLock(debugger->lock);

/*
 * Unlock the debugger object.
 */
#define	UnlockDebugger(debugger)	ILMutexUnlock(debugger->lock);

/*
 * Extern functions.
 */
extern ILInt32 _IL_StackFrame_InternalGetTotalFrames(ILExecThread * _thread);

/*
 * Forward declarations.
 */
static void CommandLoop(ILDebugger *debugger);
static void Help(ILDebugger *debugger, FILE *stream);

/*
 * Print out of memory memory and exit program.
 */
static void ILDbOutOfMemory()
{
	fputs("il_debugger: virtual memory exhausted\n", stderr);
	exit(1);
}

/*
 * Open stream for writing and reading text. Return 0 on failure.
 */
static FILE *OpenStream()
{
	return tmpfile();
}

/*
 * Return contents of stream from start to current position
 * as text allocated with ILMalloc(). Return 0 on error.
 */
static char *ReadStream(FILE *stream)
{
	int len;
	char *result;

	len = (int) ftell(stream);
	result = (char *) ILMalloc(len);
	if(result == 0)
	{
		return 0;
	}
	fseek(stream, 0, SEEK_SET);
	fread((void *) result, 1, len, stream);
	return result;
}

/*
 * Open socket IO.
 * TODO: only supported connection string is tcp://localhost:4571
 */
static int SocketIO_Open(ILDebuggerIO *io, const char *connectionString)
{
	ILSysIOHandle sockfd;
	int port = 4571;

	/*
	 * Create socket descriptor
	 *  adress family = 2 (IL_AF_INET, AddressFamily.InterNetwork
	 *  socket type = 1 (IL_SOCK_STREAM, SocketType.Stream)
	 *  protocol type = 6 (ProtocolType.Tcp)
	 */
	sockfd = ILSysIOSocket(2, 1, 6);
	if(sockfd == ILSysIOHandle_Invalid)
	{
		perror("il_debugger: socket creation failed");
		return 0;
	}

	/* Convert the end point into a sockaddr buffer like in
	 * CombinedToSerialized() in socket.c */
	unsigned char addr[16];
	addr[0] = 2;								/* address family - AF_INET */
	addr[1] = 0; 
	addr[2] = (unsigned char)(port >> 8);		/* port */
	addr[3] = (unsigned char)port;
	addr[4] = 127;								/* end point 127.0.0.1 */
	addr[5] = 0;
	addr[6] = 0;
	addr[7] = 1;
	addr[8] = addr[9] = addr[10] = addr[11] =
											 addr[12] = addr[13] =
											 addr[14]= addr[15] = 0;

	/* Connect, this connects socket for ipv4.
	   TODO: we should do it more correctly (see Socket.cs Connect()) */
	if(!ILSysIOSocketConnect(sockfd, addr, 16))
	{
		perror("il_debugger: connect to socket failed");
		return 0;	
	}

	/* Setup io struct */
	io->sockfd = sockfd;
	io->input = OpenStream();
	if(io->input == 0)
	{
		perror("il_debugger: failed to open input stream");
		return 0;
	}
	io->output = OpenStream();
	if(io->output == 0)
	{
		perror("il_debugger: failed to open output stream");
		return 0;
	}

	return 1;
}

/*
 * Close socket IO.
 */
static void SocketIO_Close(ILDebuggerIO *io)
{
	ILSysIOSocketClose(io->sockfd);
}

/*
 * Recieve input data to input stream using socket IO.
 */
static int SocketIO_Recieve(ILDebuggerIO *io)
{
	static char buffer[128];
	int len;

	/* Rewind input stream */
	fseek(io->input, 0, SEEK_SET);

	/* Read data from socket and write to the input stream */
	do
	{
		len = ILSysIOSocketReceive(io->sockfd, (void *)buffer, 128, 0);
		if(len < 0)
		{
			perror("il_debugger: recieve failed");
			return 0;
		}
		fwrite(buffer, 1, len, io->input);
	} while(len == 128);	/* whole buffer filled - more data are available */

	putc(0, io->input);

	return 1;
}

/*
 * Send data in output stream using socket IO.
 */
static int SocketIO_Send(ILDebuggerIO *io)
{
	int len;
	int count;
	static char buffer[1024];

	/* Terminate output with 0 */
	putc(0, io->output);

	/* Remeber output length */
	len = (int) ftell(io->output);

	/* Rewind output stream */
	fseek(io->output, 0, SEEK_SET);

	/* Read data from output stream and send them to socket */
	do
	{
		if(len > 1024)
		{
			count = 1024;
		}
		else
		{
			count = len;
		}
		if(fread((void *)buffer, 1, count, io->output) != count)
		{
			perror("il_debugger: send failed");
			return 0;
		}
		if(ILSysIOSocketSend(io->sockfd, buffer, count, 0) < 0)
		{
			perror("il_debugger: send failed");
			return 0;
		}
		len -= 1024;
	}
	while(len > 0);

	/* Rewind output stream for next command */
	fseek(io->output, 0, SEEK_SET);

	return 1;
}

/*
 * Check if we are watching given image.
 */
static int IsImageWatched(ILAssemblyWatch *watch, ILImage *image)
{
	if(watch == 0)
	{
		return 1;	/* watch all assemblies by default */
	}
	while(watch)
	{
		if(watch->image == image)
		{
			return 1;
		}
		watch = watch->next;
	}
	return 0;
}

int ILDebuggerIsAssemblyWatched(ILDebugger *debugger, ILMethod *method)
{
	return IsImageWatched(debugger->assemblyWatches,
									ILClassToImage(ILMethod_Owner(method)));
}

/*
 * Get method's location.
 */
const char * GetLocation(ILMethod *method, ILInt32 offset,
												ILUInt32 *line, ILUInt32 *col)
{
	ILDebugContext *dbgc;
	const char *result;

	/* Dump empty location if the method's image
	 * does not have any debug information */
	if(ILDebugPresent(ILProgramItem_Image(method)))
	{
		/* Get the symbol debug information */
		if((dbgc = ILDebugCreate(ILProgramItem_Image(method))) == 0)
		{
			ILDbOutOfMemory();
		}
		result = ILDebugGetLineInfo(dbgc, ILMethod_Token(method),
									(ILUInt32)offset,
									line, col);
		ILDebugDestroy(dbgc);

		if(result)
		{
			/* Everything ok */
			return result;
		}
	}

	/* No debug info available */
	*line = 0;
	*col = 0;
	return 0;
}

/*
 * Dump a string, with XML quoting.
 */
static void DumpString(const char *str, FILE *stream)
{
	int ch;
	if(!str)
	{
		return;
	}
	while((ch = *str++) != '\0')
	{
		if(ch == '<')
		{
			fputs("&lt;", stream);
		}
		else if(ch == '>')
		{
			fputs("&gt;", stream);
		}
		else if(ch == '&')
		{
			fputs("&amp;", stream);
		}
		else if(ch == '"')
		{
			fputs("&quot;", stream);
		}
		else if(ch == '\'')
		{
			fputs("&apos;", stream);
		}
		else
		{
			putc(ch, stream);
		}
	}
}

 /*
 * Indent a line by a specific amount.
 */
static void Indent(FILE *stream, int indent)
{
	static char const spaces[] = "                ";
	while(indent >= 16)
	{
		fwrite(spaces, 1, 16, stream);
		indent -= 16;
	}
	if(indent > 0)
	{
		fwrite(spaces, 1, indent, stream);
	}
}

/*
 * Dump error message to outgoing buffer.
 */
static void DumpError(const char *message, FILE *stream)
{
	fputs("<ErrorMessage>", stream);
	fputs(message, stream);
	fputs("</ErrorMessage>", stream);
}

/*
 * Dump message to outgoing buffer.
 */
static void DumpMessage(const char *message, FILE *stream)
{
	fputs("<Message>", stream);
	fputs(message, stream);
	fputs("</Message>", stream);
}

/*
 * Dump current location. If debug info not available, dump empty location.
 */
void DumpLocation(FILE *stream, const char *sourceFile, ILUInt32 line,
													ILUInt32 col, int indent)
{
	Indent(stream, indent);
	fprintf(stream,
				 "<Location Linenum=\"%d\" Col=\"%d\"><SourceFile Filename=\"",
				  line, col);
	if(sourceFile)
	{
		fputs(sourceFile, stream);
	}
	fputs("\" /></Location>\n", stream);
}

/*
 * Dump a class name to a stream.
 */
static void DumpClassName(FILE *stream, ILClass *classInfo)
{
	const char *name = ILClass_Name(classInfo);
	const char *namespace = ILClass_Namespace(classInfo);
	if(namespace)
	{
		DumpString(namespace, stream);
		putc('.', stream);
	}
	DumpString(name, stream);
}

/*
 * Generate documentation for a specific method definition.
 */
static void DumpMethod(FILE *stream, ILMethod *method, int indent)
{
	ILType *signature;
	int isConstructor;
	ILUInt32 line;
	ILUInt32 col;
	const char *sourceFile;

	/* Bail out on null */
	if(!method)
	{
		return;
	}

	/* Output the member header */
	Indent(stream, indent);
	fputs("<Member MemberName=\"", stream);
	DumpString(ILMethod_Name(method), stream);
	fputs("\" OwnerName=\"", stream);
	DumpClassName(stream, ILMethod_Owner(method));
	fputs("\">\n", stream);

	/* Is this method a constructor? */
	isConstructor = (!strcmp(ILMethod_Name(method), ".ctor") ||
	   				 !strcmp(ILMethod_Name(method), ".cctor"));

	/* Output the signature in ILASM form */
	Indent(stream, indent + 2);
	fputs("<MemberSignature Language=\"ILASM\" Value=\"", stream);
	if(!isConstructor)
	{
		fputs(".method ", stream);
	}
	ILDumpFlags(stream, ILMethod_Attrs(method), ILMethodDefinitionFlags, 0);
	signature = ILMethod_Signature(method);
	ILDumpMethodType(stream, ILProgramItem_Image(method), signature,
			         IL_DUMP_XML_QUOTING, 0, ILMethod_Name(method), method);
	putc(' ', stream);
	ILDumpFlags(stream, ILMethod_ImplAttrs(method),
				ILMethodImplementationFlags, 0);
	fputs("\"/>\n", stream);

	/* Output the member kind */
	Indent(stream, indent + 2);
	if(isConstructor)
	{
		fputs("<MemberType>Constructor</MemberType>\n", stream);
	}
	else
	{
		fputs("<MemberType>Method</MemberType>\n", stream);
	}

	/* Output members location */
	sourceFile = GetLocation(method, 0, &line, &col);
	if(sourceFile)
	{
		DumpLocation(stream, sourceFile, line, col, indent + 2);
	}

	Indent(stream, indent);
	fputs("</Member>\n", stream);
}

/*
 * Dump position of execution.
 */
void DumpExecPosition(FILE *stream, ILMethod *method,
							ILUInt32 offset, void *pc, const char *sourceFile,
							ILUInt32 line, ILUInt32 col, int indent)
{
	Indent(stream, indent);
	fprintf(stream, "<ExecPosition Offset=\"%d\"", offset);
	if(pc != 0)
	{
		fprintf(stream, " PC=\"%p\"", pc);
	}
	fputs(">\n", stream);
	DumpMethod(stream, method, indent + 2);
	DumpLocation(stream, sourceFile, line, col, indent + 2);
	Indent(stream, indent);
	fputs("</ExecPosition>", stream);
}

/*
 * Dump breakpoint.
 */
void DumpBreakpoint(FILE *stream, ILBreakpoint *breakpoint, int indent)
{
	Indent(stream, indent);
	fprintf(stream, "<Breakpoint Id=\"%d\" >\n", breakpoint->id);
	DumpExecPosition(stream, breakpoint->method,
							breakpoint->offset,
							0,
							breakpoint->sourceFile,
							breakpoint->line,
							breakpoint->col, indent + 2);
	Indent(stream, indent);
	fputs("\n</Breakpoint>", stream);
}

#ifdef IL_USE_CVM
/*
 * Dump call frame (used only with CVM coder.)
 */
static void DumpCallFrame(FILE *stream, ILDebugger *debugger,
											ILCallFrame *frame, int indent)
{
	unsigned char *start;
	ILInt32 offset;
	ILUInt32 line;
	ILUInt32 col;
	const char *sourceFile;

	if(frame->method && frame->pc != IL_INVALID_PC)
	{
		/* Find the start of the frame method */
		start = (unsigned char *)ILMethodGetUserData(frame->method);
		if(ILMethodIsConstructor(frame->method))
		{
			start -= ILCoderCtorOffset(debugger->process->coder);
		}

		offset = (ILInt32)ILCoderGetILOffset
				(debugger->process->coder, (void *)start,
				(ILUInt32)(frame->pc - start), 0);

		/* Read current position from debug info */
		sourceFile = GetLocation(frame->method, offset, &line, &col);
	}
	else
	{
		/* Probably a native method that does not have offsets */
		offset = -1;
		sourceFile = 0;
		line = 0;
		col = 0;
	}

	/* Dump the frame */
	DumpExecPosition(stream, frame->method,
						offset, 0, sourceFile,
						line, col, indent);
}
#endif	// IL_USE_CVM

/*
 * Create new ILDebuggerThreadInfo.
 */
ILDebuggerThreadInfo *ILDebuggerThreadInfo_Create()
{
	ILDebuggerThreadInfo *result;
	result = (ILDebuggerThreadInfo *) ILMalloc(sizeof(ILDebuggerThreadInfo));
	if(result == 0)
	{
		ILDbOutOfMemory();
	}
	memset(result, 0, sizeof(ILDebuggerThreadInfo));
	result->runType = IL_DEBUGGER_RUN_TYPE_STOPPED;
	return result;
}

/*
 * Free thread info list.
 */
 void ILDebuggerThreadInfo_Destroy(ILDebuggerThreadInfo *info)
 {
	ILDebuggerThreadInfo *next;
	
	while(info)
	{
		next = info->next;
#ifdef IL_USE_JIT
		if(info->jitStackTrace)
		{
			jit_stack_trace_free(info->jitStackTrace);
		}
#endif
		ILFree(info);
		info = next;
	}
 }

/*
 * Get debugger info associated with given exec thread.
 * If not found return free info.
 */
ILDebuggerThreadInfo *GetDbThreadInfo(ILDebugger *debugger,
														ILExecThread *thread)
{
	ILDebuggerThreadInfo *info;

	/* Search for existing info */
	info = debugger->dbthread;
	do
	{
		if(info->execThread == thread)
		{
			return info;
		}
		info = info->next;
	} while(info);

	/* Search for free slot */
	info = debugger->dbthread;
	for(;;)
	{
		if(info->runType == IL_DEBUGGER_RUN_TYPE_CONTINUE)
		{
			return info;
		}
		if(info->next == 0)
		{
			/* Append new info to the end of list */
			info->next = ILDebuggerThreadInfo_Create();
			info->next->id = info->id + 1;
			return info->next;
		}
		info = info->next;
	}
}

/*
 * Update info about state of execution thread.
 * This function is called to remeber various info
 * when thread stops debugger.
 */
static void UpdateDbThreadInfo(ILDebuggerThreadInfo *info, void *userData,
								ILExecThread *thread, ILMethod *method,
								ILInt32 offset, int type,
								const char *sourceFile,
								ILUInt32 line, ILUInt32 col)
{
	info->userData = userData;
	info->execThread = thread;
	info->method = method;
	info->offset = offset;
	info->type = type;
	info->sourceFile = sourceFile;
	info->line = line;
	info->col = col;
	info->numFrames = _IL_StackFrame_InternalGetTotalFrames(thread);

#ifdef IL_USE_JIT
	/* Free previous jit stack trace */
	if(info->jitStackTrace)
	{
		jit_stack_trace_free(info->jitStackTrace);
	}
	/* Set current jit stack trace */
	info->jitStackTrace = jit_exception_get_stack_trace();
#endif

}

/*
 * Create new breakpoint.
 */
ILBreakpoint *ILBreakpoint_Create()
{
	ILBreakpoint *result;
	result = (ILBreakpoint *) ILMalloc(sizeof(ILBreakpoint));
	if(result == 0)
	{
		ILDbOutOfMemory();
	}
	memset(result, 0, sizeof(ILBreakpoint));
	return result;
}

/*
 * Free breakpoint list.
 */
 void DebuggerBreakpoint_Destroy(ILBreakpoint *breakpoint)
 {
	ILBreakpoint *next;
	
	while(breakpoint)
	{
		next = breakpoint->next;
		ILFree(breakpoint);
		breakpoint = next;
	}
 }

 /*
 * Get breakpoint in given method and given offset interval.
 * If not found return free breakpoint.
 */
ILBreakpoint *GetBreakpoint(ILDebugger *debugger, ILMethod *method,
															ILUInt32 offset)
{
	ILBreakpoint *breakpoint;

	/* Search for existing breakpoint */
	breakpoint = debugger->breakpoint;
	do
	{
		if(breakpoint->method == method && breakpoint->offset == offset)
		{
			return breakpoint;
		}
		breakpoint = breakpoint->next;
	} while(breakpoint);

	/* Search for free slot */
	breakpoint = debugger->breakpoint;
	for(;;)
	{
		if(breakpoint->method == 0)
		{
			return breakpoint;
		}
		if(breakpoint->next == 0)
		{
			/* Append new breakpoint to the end of the list */
			breakpoint->next = ILBreakpoint_Create();
			breakpoint->next->id = breakpoint->id + 1;
			return breakpoint->next;
		}
		breakpoint = breakpoint->next;
	}
}

static void DestroyAssemblyWatches(ILDebugger *debugger)
{
	ILAssemblyWatch *watch;
	while(debugger->assemblyWatches)
	{
		watch = debugger->assemblyWatches;
		debugger->assemblyWatches = debugger->assemblyWatches->next;
		ILFree(watch);
	}
}

/*
 * Find method for given source file and line.
 */
ILMethod *FindMethodByLocation(ILDebugger *debugger, const char *sourceFile,
								ILUInt32 reqLine, ILUInt32 *outLine,
								ILUInt32 *outCol, ILUInt32 *outOffset,
								const char **outFile)
{
	ILContext *context;
	ILImage *image;
	ILDebugContext *dbgc = 0;
	ILMethod *method;
	const char *filename;
	ILUInt32 line;
	ILUInt32 col;
	ILUInt32 offset;
	ILInt32 delta;
	ILInt32 bestDelta;
	ILUInt32 bestOffset;
	ILUInt32 bestLine;
	ILUInt32 bestCol;
	ILMethod *bestMethod;
	ILMethodCode methodCode;

	/* Initialize searching */
	bestDelta = IL_MAX_INT32;
	bestOffset = 0;
	bestMethod = 0;
	bestLine = 0;
	bestCol = 0;
	
	/* Iterate all images */
	context = ILExecProcessGetContext(debugger->process);
	image = 0;
	while((image = ILContextNextImage(context, image)) != 0)
	{
		/* Get debug context for image */
		if(!ILDebugPresent(image))
		{
			continue;
		}
		if((dbgc = ILDebugCreate(image)) == 0)
		{
			continue;
		}
		/* Iterate all methods */
		method = 0;
		while((method = (ILMethod *)ILImageNextToken
					(image, IL_META_TOKEN_METHOD_DEF, (void *)method)) != 0)
		{
			/* Find best matching offset in method */
			if(ILMethodGetCode(method, &methodCode))
			{
				for(offset = 0; offset < methodCode.codeLen; offset++)
				{
					/* Get debug info for current offset */
					filename = ILDebugGetLineInfo(dbgc,
														ILMethod_Token(method),
												 		offset, &line, &col);

					/* There is no line debug info for this offset */
					if(!filename)
					{
						continue;
					}
					/* Try next method if not in requested file */
					if(ILStrICmp(filename, sourceFile) != 0)
					{
						break;
					}
					delta = abs(reqLine - line);
					if(delta >= bestDelta)
					{
						continue;
					}
					/* Update the best debug info */
					bestMethod = method;
					bestDelta = delta;
					bestOffset = offset;
					bestLine = line;
					bestCol = col;
					*outFile = filename;
				}
			}
		}
	}
	/* No method found in given source file */
	if(bestMethod == 0)
	{
		return 0;
	}
	/* Fill output info and return method */
	*outLine = bestLine;
	*outCol = bestCol;
	*outOffset = bestOffset;

	return bestMethod;
}

/*
 * Insert, remove or toggle breakpoint. Actions 0, 1, 2
 */
void HandleBreakpointCommand(ILDebugger *debugger, FILE *stream, int command)
{
	ILUInt32 reqLine;
	ILMethod *method;
	ILUInt32 offset;
	ILUInt32 line;
	ILUInt32 col;
	const char *sourceFile;
	ILBreakpoint *breakpoint;

	/* Validate and parse arguments */
	if(debugger->argCount < 2)
	{
		DumpError("Parameter missing", stream);
		return;
	}
	reqLine = atoi(debugger->args[1]);

	/* Find method for given source file and line */
	method = FindMethodByLocation(debugger, debugger->args[0], reqLine,
											&line, &col, &offset, &sourceFile);

	if(!method)
	{
		DumpError("Location not found, wrong params or debug info missing.",
																	stream);
		return;
	}

	/* Get breakpoint info - empty or existing */
	breakpoint = GetBreakpoint(debugger, method, offset);

	
	if(breakpoint->method)
	{
		/* Breakpoint exists */
		if(command == 0)
		{
			/* On insert */
			DumpError("breakpoint already exists", stream);
		}
		else
		{
			/* On remove or toggle */
			DumpBreakpoint(stream, breakpoint, 2);
			breakpoint->method = 0;
			ILExecProcessUnwatchMethod(debugger->process, method);
			DumpMessage("removed", stream);
		}
	}
	else
	{
		/* Breakpoint does not exist */
		if(command == 1)
		{
			/* On remove */
			DumpError("Breakpoint not found", stream);
		}
		else
		{
			/* Fill info */
			breakpoint->method = method;
			breakpoint->offset = offset;
			breakpoint->sourceFile = sourceFile;
			breakpoint->line = line;
			breakpoint->col = col;

			/* Dump breakpoint - it's usefull because location
			   may not be exactly the the same as user wanted */
			DumpBreakpoint(stream, breakpoint, 2);
			DumpMessage("inserted", stream);
		}
	}
}

/*
 * insert_breakpoint command.
 */
void InsertBreakpoint(ILDebugger *debugger, FILE *stream)
{
	HandleBreakpointCommand(debugger, stream, 0);
}

/*
 * remove_breakpoint command.
 * TODO: remove breakpoint by id
 */
void RemoveBreakpoint(ILDebugger *debugger, FILE *stream)
{
	HandleBreakpointCommand(debugger, stream, 1);
}

/*
 * toggle_breakpoint command.
 */
void ToggleBreakpoint(ILDebugger *debugger, FILE *stream)
{
	HandleBreakpointCommand(debugger, stream, 2);
}

/*
 * break command.
 */
void Break(ILDebugger *debugger, FILE *stream)
{
	/* Set stop flag */
	debugger->breakAll = 1;
}

/*
 * show_locals command.
 */
void ShowLocals(ILDebugger *debugger, FILE *stream)
{
#ifdef IL_USE_CVM
	CVMWord *p;
	ILExecThread *thread = debugger->dbthread->execThread;
	fputs("<LocalVariables>\n",stream);
	for(p = thread->frame; p < thread->stackTop; p++)
	{
		fprintf(stream, "  <LocalVariable Value=\"%d\" />\n", p->intValue);
	}
	fputs("</LocalVariables>",stream);
#endif

#ifdef IL_USE_JIT
	ILExecThread *thread;
	ILMethod *method;
	ILMethodCode code;
	ILType *signature;
	ILUInt32 num;
	ILUInt32 current;
	ILUInt32 i;
	ILWatch *watch;
	ILType *type;
	char *str;
	char ch[8];
	int len;
	int j;

	/* Current thread and method */
	thread = debugger->dbthread->execThread;
	method = debugger->dbthread->method;

	/* Get local variables info */
	if(!ILMethodGetCode(method, &code))
	{
		DumpError("Unable to get method code", stream);
		return;
	}
	if(code.localVarSig)
	{
		signature = ILStandAloneSigGetType(code.localVarSig);
		num = ILTypeNumLocals(signature);
	}
	else
	{
		signature = 0;
		num = 0;
	}

	fputs("<LocalVariables>\n", stream);

	/* Dump local variables in current frame */
	current = 0;
	for(i = 0; i < thread->numWatches && num > 0; i++)
	{
		watch = &(thread->watchStack[i]);
		if(watch->frame != thread->frame)
		{
			continue;	/* Skip if variable is not in current frame */
		}

		/* TODO: dump local variable name */
		fprintf(stream, "<LocalVariable Name=\"var%d\" Value=\"", i);

		type = ILTypeGetLocal(signature, current);

		switch((unsigned long)type)
		{
			case (unsigned long)ILType_Boolean:
			{
				if(*((ILBool *)(watch->addr)) != 0)
				{
					fputs("true", stream);
				}
				else
				{
					fputs("false", stream);
				}
			}
			break;
			case (unsigned long)ILType_Int8:
			{
				fprintf(stream, "%d", *((ILInt8 *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_UInt8:
			{
				fprintf(stream, "%u", *((ILUInt8 *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_Int16:
			{
				fprintf(stream, "%hd", *((ILInt16 *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_UInt16:
			{
				fprintf(stream, "%hu", *((ILUInt16 *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_Int32:
			{
				fprintf(stream, "%d", *((ILInt32 *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_UInt32:
			{
				fprintf(stream, "%u", *((ILUInt32 *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_Int64:
			{
				fprintf(stream, "%lld", *((ILInt64 *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_UInt64:
			{
				fprintf(stream, "%llu", *((ILUInt64 *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_Float32:
			{
				fprintf(stream, "%f", *((ILFloat *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_Float64:
			{
				fprintf(stream, "%f", *((ILDouble *)(watch->addr)));
			}
			break;
			case (unsigned long)ILType_Char:
			{
				len = ILUTF8WriteChar(ch, *((ILUInt16 *)(watch->addr)));
				for(j = 0; j < len; j++)
				{
					putc(ch[j], stream);
				}
			}
			break;
			default:
			{
				if(ILTypeIsStringClass(type))
				{
					str = ILStringToUTF8(thread, *((ILString **)(watch->addr)));
					DumpString(str, stream);
				}
				else
				{
					fprintf(stream, "%p", *((void **)(watch->addr)));
				}
			}
			break;
		}
		fputs("\" >", stream);

		str = ILTypeToName(type);
		fputs("<Type Name=\"", stream);
		DumpString(str, stream);
		fputs("\" /></LocalVariable>\n", stream);
		ILFree(str);

		current++;
		num--;
	}

	fputs("</LocalVariables>",stream);

#endif
}

/*
 * Update location in current thread.
 * Line and sourceFile are set to 0 if no debug info available.
 */
void UpdateLocation(ILDebugger *debugger)
{
	debugger->dbthread->sourceFile = GetLocation(debugger->dbthread->method,
										debugger->dbthread->offset,
										&(debugger->dbthread->line),
										&(debugger->dbthread->col));
	
}

/*
 * Dump current location. If debug info not available, dump empty location.
 */
void DumpCurrentLocation(ILDebugger *debugger, FILE *stream, int indent)
{
	/* Update location in current thread info */
	UpdateLocation(debugger);

	DumpLocation(stream,
					 debugger->dbthread->sourceFile,
					 debugger->dbthread->line,
					 debugger->dbthread->col,
					 indent);
}

/*
 * show_position command.
 */
void ShowPosition(ILDebugger *debugger, FILE *stream)
{
	UpdateLocation(debugger);
	DumpExecPosition(stream, debugger->dbthread->method,
								debugger->dbthread->offset,
								0,
								debugger->dbthread->sourceFile,
								debugger->dbthread->line,
								debugger->dbthread->col,
								0);
}

/*
 * show_stack_trace command.
 */
void ShowStackTrace(ILDebugger *debugger, FILE *stream)
{
#ifdef IL_USE_CVM
	ILCallFrame *frame;
	ILExecThread *thread = debugger->dbthread->execThread;

	/* Push the current frame data onto the stack temporarily
		so that we can dump it */
	if(thread->numFrames < thread->maxFrames)
	{
		frame = &(thread->frameStack[(thread->numFrames)++]);
	}
	else if((frame = _ILAllocCallFrame(thread)) == 0)
	{
		/* We ran out of memory trying to push the frame */
		DumpError("out of memory while getting stack trace", stream);
		return;
	}
	frame->method = thread->method;
	frame->pc = thread->pc;
	frame->frame = thread->frame;
	frame->exceptHeight = thread->exceptHeight;
	frame->permissions = 0;

	fputs("  <StackTrace>\n", stream);
	frame = _ILGetCallFrame(thread, 0);

	while(frame != 0)
	{
		/* Dump the frame */
		DumpCallFrame(stream, debugger, frame, 4);
		putc('\n', stream);

		/* Next frame */
		frame = _ILGetNextCallFrame(thread, frame);
	}

	/* Pop current frame, that was previously pushed for dumping */
	--(thread->numFrames);

	/* Dump xml footer */
	fputs("  </StackTrace>", stream);
#endif	// IL_USE_CVM

#ifdef IL_USE_JIT
	jit_stack_trace_t stackTrace;
	ILUInt32 size;
	ILUInt32 current;
	jit_context_t jitContext;
	ILJitFunction jitFunction;
	ILMethod *method;
	ILUInt32 offset;
	ILUInt32 line;
	ILUInt32 col;
	const char *sourceFile;
	void *pc;

	/* Get jit stack trace */
	stackTrace = debugger->dbthread->jitStackTrace;
	if(stackTrace == 0)
	{
		DumpError("error getting stack trace", stream);
		return;
	}

	/* Dump xml header */
	fputs("  <StackTrace>\n", stream);

	/* Get info for reading stack trace */
	size = jit_stack_trace_get_size(stackTrace);
	jitContext = ILJitGetContext(debugger->process->coder);

	/* Walk the stack and dump managed frames */
	for(current = 0; current < size; ++current)
	{
		jitFunction = jit_stack_trace_get_function(jitContext,
										stackTrace, current);
		if(jitFunction)
		{
			if((method = jit_function_get_meta
									(jitFunction, IL_JIT_META_METHOD)) != 0)
			{
				/* Get IL offset */
				offset = jit_stack_trace_get_offset(jitContext,
													stackTrace,
													current);

				/* Read current position from debug info */
				sourceFile = GetLocation(method, offset, &line, &col);
				pc = jit_stack_trace_get_pc(stackTrace, current);

				/* Dump the frame */
				DumpExecPosition(stream, method, offset, pc,
												sourceFile, line, col, 4);
			}
		}
	}

	/* Dump xml footer */
	fputs("\n  </StackTrace>", stream);
#endif	// IL_USE_JIT
}

/*
 * watch_assembly command.
 */
void WatchAssembly(ILDebugger *debugger, FILE *stream)
{
	ILContext *context;
	ILImage *image;
	ILAssemblyWatch *watch;
	int found = 0;

	if(debugger->argCount < 1)
	{
		DumpError("Parameter missing", stream);
		return;
	}

	/* Search for image with given assembly name */
	context = ILExecProcessGetContext(debugger->process);
	image = 0;
	while((image = ILContextNextImage(context, image)) != 0)
	{
		if(strcmp(ILImageGetAssemblyName(image), debugger->args[0]) == 0)
		{
			found = 1;
			break;
		}
	}
	
	if(!found)
	{
		DumpError("assembly not found", stream);
		return;
	}
	
	/* Search the watch list for the image */
	watch = debugger->assemblyWatches;
	while(watch != 0)
	{
		if(watch->image == image)
		{
			++(watch->count);
			return;
		}
		watch = watch->next;
	}
	
	/* Add the image to the watch list in the thread safe way */
	if((watch = (ILAssemblyWatch *)ILMalloc(sizeof(ILAssemblyWatch))) == 0)
	{
		DumpError("out of memory", stream);
		return;
	}

	watch->image = image;
	watch->count = 1;
	watch->next = debugger->assemblyWatches;
	debugger->assemblyWatches = watch;

	DumpMessage("ok", stream);
}

/*
 * unwatch_all_assemblies command.
 */
void UnwatchAllAssemblies(ILDebugger *debugger, FILE *stream)
{
	DestroyAssemblyWatches(debugger);
	DumpMessage("ok", stream);
}

/*
 * suspend_all command.
 */
void SuspendAll(ILDebugger *debugger, FILE *stream)
{
	ILExecThread *thread = debugger->process->firstThread;
	while(thread)
	{
		_ILExecThreadSuspendThread(thread, thread->supportThread);
		thread = thread->nextThread;
	}
	
	/* Report ok and continue execution */
	DumpMessage("ok", stream);
}

void ResumeAll(ILDebugger *debugger, FILE *stream)
{
	ILExecThread *thread = debugger->process->firstThread;
	while(thread)
	{
		_ILExecThreadResumeThread(thread, thread->supportThread);
		thread = thread->nextThread;
	}

	/* Report ok and continue execution */
	DumpMessage("ok", stream);
}

/*
 * show_threads command.
 */
void ShowThreads(ILDebugger *debugger, FILE *stream)
{
	ILDebuggerThreadInfo *info;

	fputs("<DebuggerThreads>\n", stream);

	info = debugger->dbthread;
	while(info)
	{
		if(info->runType != IL_DEBUGGER_RUN_TYPE_STOPPED)
		{
			continue;
		}
		Indent(stream, 2);
		fprintf(stream, "<DebuggerThread Id=\"%d\" Address=\"%p\" RunType=\"%d\" Current=\"%d\" />\n",
									info->id, info->execThread,
									info->runType, info == debugger->dbthread);
		info = info->next;
	}
	
	fputs("</DebuggerThreads>", stream);
}

/*
 * show_breakpoints command.
 */
void ShowBreakpoints(ILDebugger *debugger, FILE *stream)
{
	ILBreakpoint *breakpoint;

	fputs("<DebuggerBreakpoints>\n", stream);

	breakpoint = debugger->breakpoint;
	while(breakpoint)
	{
		/* Skip removed breakpoints */
		if(breakpoint->method == 0)
		{
			continue;
		}

		/* Dump the breakpoint and move to next */
		DumpBreakpoint(stream, breakpoint, 2);
		breakpoint = breakpoint->next;
	}

	fputs("</DebuggerBreakpoints>", stream);
}

/*
 * Return directory where image was linked.
 */
static const char * GetLinkDir(ILImage *image)
{
	ILDebugContext *dbgc;
	ILToken token;
	ILDebugIter iter;
	ILMetaDataRead reader;
	const char *result = 0;

	/* Dump empty location if the method's image 
	 * does not have any debug information */
	if(ILDebugPresent(image))
	{
		/* Get the symbol debug information */
		if((dbgc = ILDebugCreate(image)) == 0)
		{
			ILDbOutOfMemory();
		}
		token = ILDebugGetPseudo("LDIR");
		ILDebugIterInit(&iter, dbgc, token);
		if(ILDebugIterNext(&iter))
		{
			/* Read the LDIR string */
			reader.data = iter.data;
			reader.len = iter.length;
			reader.error = 0;
			result = ILDebugGetString(dbgc, ILMetaUncompressData(&reader));
			if(reader.error)
			{
				result = 0;
			}
		}
		ILDebugDestroy(dbgc);
	}
	return result;
}

/*
 * Definitions for hashtable of source files.
 */
static unsigned long StringHashFunc(const void *elem)
{
	return ILHashString(0, ((const char *)(elem)), -1);
}

static int StringMatchFunc(const void *elem, const void *key)
{
	return !strcmp(((const char *)(elem)), (const char *)key);
}

static void StringFreeFunc(void *elem)
{
	/* Nothing to do here */
}

/*
 * show_libraries command - dumps also members - it's not currently needed.
 */
void ShowLibraries(ILDebugger *debugger, FILE *stream)
{
	ILContext *context;
	ILImage *image;
	const char *linkDir;
	ILMember *member;
	ILClass *classInfo;
	ILHashTable *table;
	ILHashIter iter;
	ILUInt32 line;
	ILUInt32 col;
	const char *sourceFile;
	int error = 0;
	int dumpMembers;

	/* Check if we should also dump members */
	if(debugger->argCount > 0 && debugger->args[0][0] == '1')
	{
		dumpMembers = 1;
	}
	else
	{
		dumpMembers = 0;
	}

	/* Create source file hashtable */
	table = ILHashCreate(0, StringHashFunc,
							StringHashFunc,
							StringMatchFunc,
							StringFreeFunc);
	if(table == 0)
	{
		DumpError("Out of memory", stream);
		return;
	}

	/* Dump xml header */
	fputs("<Libraries>\n", stream);

	/* Iterate all images */
	context = ILExecProcessGetContext(debugger->process);
	image = 0;
	while((image = ILContextNextImage(context, image)) != 0)
	{
		/* Skip assemblies that are not watched */
		if(!IsImageWatched(debugger->assemblyWatches, image))
		{
			continue;
		}
		/* Dump image xml header */
		fputs("  <Types Library=\"", stream);
		DumpString(ILImageGetAssemblyName(image), stream);
		linkDir = GetLinkDir(image);
		if(linkDir != 0)
		{
			fputs("\" LinkDir=\"", stream);
			DumpString(linkDir, stream);
		}
		fputs("\">\n", stream);
		/* Scan the TypeDef table and dump each top-level type */
		classInfo = 0;
		while((classInfo = (ILClass *)ILImageNextToken
					(image, IL_META_TOKEN_TYPE_DEF, classInfo)) != 0)
		{
			if(dumpMembers)
			{
				Indent(stream, 4);
				fputs("<Type Name=\"", stream);
				DumpString(ILClass_Name(classInfo), stream);
				fputs("\" >\n", stream);
				Indent(stream, 6);
				fputs("<Members>\n", stream);
			}
			member = 0;
			while((member = ILClassNextMember(classInfo, member)) != 0)
			{
				switch(ILMemberGetKind(member))
				{
					case IL_META_MEMBERKIND_METHOD:
					{
						if(dumpMembers)
						{
							DumpMethod(stream, (ILMethod *) member, 8);
						}
						/* Get source file of method from debug info */
						sourceFile = GetLocation((ILMethod *) member,
															0, &line, &col);

						/* Skip if no debug info 
						 * or if source file was already added */
						if(sourceFile == 0 || ILHashFind(table, sourceFile))
						{
							break;
						}
						if(!ILHashAdd(table, (void *)sourceFile))
						{
							error = 1;
						}
					}
					break;
				}
			}

			if(dumpMembers)
			{
				Indent(stream, 6);
				fputs("</Members>\n", stream);
				Indent(stream, 4);
				fputs("</Type>\n", stream);
			}
		}
		/* Dump files in table and clear the table */
		Indent(stream, 6);
		fputs("<SourceFiles>\n", stream);

		ILHashIterInit(&iter, table);
		while((sourceFile = ILHashIterNext(&iter)) != 0)
		{
			Indent(stream, 8);
			fputs("<SourceFile Filename=\"", stream);
			DumpString(sourceFile, stream);
			fputs("\" />\n", stream);

			/* Clearing hashtable */
			ILHashRemove(table, (void *) sourceFile, 0);
		}		
		Indent(stream, 6);
		fputs("</SourceFiles>\n", stream);

		/* Dump image xml footer */
		Indent(stream, 2);
		fputs("</Types>\n", stream);
	}
	/* Output the library footer information */
	fputs("</Libraries>\n", stream);
}

/*
 * Resume current thread with given runType. Other stopped threads
 * are resumend with IL_DEBUGGER_RUN_TYPE_CONTINUE.
 */
void Resume(ILDebugger *debugger, int runType)
{
	ILDebuggerThreadInfo *info;

	/* Handle situation after break command */
	if(debugger->breakAll)
	{
		debugger->breakAll = 0;
	}

	info = debugger->dbthread;
	do
	{
		if(info->runType == IL_DEBUGGER_RUN_TYPE_STOPPED)
		{
			/* Set status */
			if(info == debugger->dbthread)
			{
				info->runType = runType;
			}
			else
			{
				info->runType = IL_DEBUGGER_RUN_TYPE_CONTINUE;
			}
	
			/* Resume exec thread if it was suspended */
			if(debugger->commandThread)
			{
				_ILExecThreadResumeThread(info->execThread,
											info->execThread->supportThread);
			}
		}
		info = info->next;
	} while(info);
}

/*
 * c command (continue).
 */
void Continue(ILDebugger *debugger, FILE *stream)
{
	/* Set run type, report ok and continue execution */
	Resume(debugger, IL_DEBUGGER_RUN_TYPE_CONTINUE);
	DumpMessage("ok", stream);
}

void Step(ILDebugger *debugger, FILE *stream)
{
	/* Set run type, report ok and continue execution */
	Resume(debugger, IL_DEBUGGER_RUN_TYPE_STEP);
	DumpMessage("ok", stream);
}

void Next(ILDebugger *debugger, FILE *stream)
{
	/* Set run type, report ok and continue execution */
	Resume(debugger, IL_DEBUGGER_RUN_TYPE_NEXT);
	DumpMessage("ok", stream);
}

void Until(ILDebugger *debugger, FILE *stream)
{
	/* Set run type, report ok and continue execution */
	Resume(debugger, IL_DEBUGGER_RUN_TYPE_UNTIL);
	DumpMessage("ok", stream);
}

void IsStopped(ILDebugger *debugger, FILE *stream)
{
	if(debugger->dbthread->runType == IL_DEBUGGER_RUN_TYPE_STOPPED)
	{
		DumpMessage("yes", stream);
	}
	else
	{
		DumpMessage("no", stream);
	}
}

/*
 * show_dasm command.
 */
void ShowDasm(ILDebugger *debugger, FILE *stream)
{
	char *str;
	long pos;

	/* Remeber current position before dump */
	pos = ftell(stream);

#ifdef IL_USE_JIT
	ILJitFunction func = ILJitFunctionFromILMethod(debugger->dbthread->method);
	jit_dump_function(stream, func, ILMethod_Name(debugger->dbthread->method));
#else
	/* TODO: segfaults and dumps just one insn */
	_ILDumpCVMInsn(stream, debugger->dbthread->method,
										debugger->dbthread->execThread->pc);
#endif

	/* Read stream to memory so that we can dump with xml quoting */
	str = ReadStream(stream);

	/* Restore position */
	fseek(stream, pos, SEEK_SET);

	if(!str)
	{
		DumpError("Out of memory", stream);
		return;
	}

	fputs("<Dasm>\n", stream);
	fputs("<Text>\n", stream);
	DumpString(str + pos, stream);
	fputs("</Text>\n", stream);
	fputs("</Dasm>\n", stream);

	ILFree(str);
}

/*
 * show_ildasm command.
 */
void ShowIldasm(ILDebugger *debugger, FILE *stream)
{
	ILImage *image;

	char *str;
	long pos;

	/* Remeber current position before dump */
	pos = ftell(stream);

	image = ILClassToImage(ILMethod_Owner(debugger->dbthread->method));
	ILDAsmDumpMethod(image, stream, debugger->dbthread->method, 0, 0, 1);

	/* Read stream to memory so that we can dump with xml quoting */
	str = ReadStream(stream);

	/* Restore position */
	fseek(stream, pos, SEEK_SET);

	if(!str)
	{
		DumpError("Out of memory", stream);
		return;
	}

	fputs("<ILDasm>\n", stream);
	fputs("<Text>\n", stream);
	DumpString(str + pos, stream);
	fputs("</Text>\n", stream);
	fputs("</ILDasm>\n", stream);

	ILFree(str);
}

/*
 * Table of commands.
 */
typedef struct
{
	const char *name;
	int nameSize;
	int argCount;
	void (*func)(ILDebugger *debugger, FILE *stream);
	char *helpName;
	char *helpMsg;
	char *desc;

} Cmd;

static Cmd const commands[] = {
	{ "watch_assembly",				14,	1,	WatchAssembly,
		"watch_assembly [assembly_name]",
		"Watch assembly.",
		"By default debugger breaks in every assembly. This can be slow, so you can specify just assemblies you are interested in (debugger can't break in other assemblies.) Only functions, that were not compiled yet are affected."
	},
	
	{ "unwatch_all_assemblies",		22,	0,	UnwatchAllAssemblies,
		"unwatch_all_assemblies",
		"Remove all assembly watches.",
		"Remove all assembly watches that were previously set with watch_assembly. This causes that debugger can break in any assembly again. Only functions, that were not compiled yet are affected."
	},

	{ "insert_breakpoint",			17,	2,	InsertBreakpoint,
		"insert_breakpoint [source_file] [line]",
		"Break when given location is reached.",
		"Break when given location is reached."
	},

	{ "remove_breakpoint",			17,	2,	RemoveBreakpoint,
		"remove_breakpoint [source_file/breakpoint_id] [line]",
		"Remove breakpoint.",
		"Remove breakpoint."
	},

	{ "toggle_breakpoint",			17,	2,	ToggleBreakpoint,
		"toggle_breakpoint [source_file/breakpoint_id] [line]",
		"Toggle breakpoint.",
		"Toggle breakpoint."
	},

	{ "break",						5,	0,	Break,
		"break",
		"Break execution.",
		"Break execution of all threads."
	},

	{ "continue",					8,	0,	Continue,
		"continue",
		"Continue running your program.",
		"Continue running your program."
	},

	{ "step",						4,	0,	Step,
		"step",
		"Step (into).",
		"Step program until it reaches a different source line (steps into functions)."
	},

	{ "next",						4,	0,	Next,
		"next",
		"Next (step over).",
		"Step program, over function calls."
	},

	{ "until",						5,	0,	Until,
		"until",
		"Until.",
		"Execute until the program reaches a source line greater than the current."
	},

	{ "is_stopped",					10,	0,	IsStopped,
		"is_stopped",
		"Report if execution is stopped.",
		"Execution is stopped e.g when breakpoint is reached or break command is issued."
	},

	{ "show_threads",				12,	0,	ShowThreads,
		"show_threads",
		"List threads stopped in debugger.",
		"List threads stopped in debugger."
	},

	{ "show_breakpoints",			16,	0,	ShowBreakpoints,
		"show_breakpoints",
		"Show breakpoints.",
		"Show breakpoints."
	},

	{ "show_locals",				11,	0,	ShowLocals,
		"print_locals",
		"Print local variables.",
		"Print local variables."
	},

	{ "show_position",				13,	0,	ShowPosition,
		"show_position",
		"Print position where execution stopped.",
		"Print info about breakpoint where execution stopped."
	},

	{ "show_libraries",				14,	1,	ShowLibraries,
		"show_libraries [dump_members]",
		"Print information about watched assemblies.",
		"Specify 1 as first parameter to get all members dumped"
	},

	{ "show_stack_trace",			16,	0,	ShowStackTrace,
		"show_stack_trace",
		"Print stacktrace for current thread.",
		"Print stacktrace for current thread."
	},

	{ "show_dasm",					9,	0,	ShowDasm,
		"show_dasm",
		"Dissassemble current function in engine's internal format.",
		"Dissassemble current function in engine's internal format."
	},

	{ "show_ildasm",				11,	0,	ShowIldasm,
		"show_ildasm",
		"Dissassemble current function in IL.",
		"Dissassemble current function in IL."
	},

	{ "help",						4,	0,	Help,
		"help [command]",
		"Display this information or command help.",
		"Display this information or command specific help."
	}
};

#define	num_commands	(sizeof(commands) / sizeof(commands[0]))

/*
 * Check if commands are ok.
 */
static int CheckCommands()
{
 	int i;
	for(i = 0; i < num_commands; ++i)
	{
		/* Compute nameSize if needed */
		if(commands[i].nameSize != strlen(commands[i].name))
		{
			fprintf(stderr, "il_debugger: invalid nameSize for command %s\n",
															commands[i].name);
			return 0;
		}
		if(commands[i].argCount > IL_DEBUGGER_COMMAND_MAX_ARG_COUNT)
		{
			fprintf(stderr, "il_debugger: arg count for command %s exceeded\n",
			 												commands[i].name);
			return 0;	
		}
	}
	return 1;
}

/*
 * Parse command from input text.
 * Set argCount and return command index or error code.
 * -1 command not found
 * -2 too many parameters for command
 */
int ParseCommand(ILDebugger *debugger, char *text, int textLen)
{
	int i, j;
	debugger->argCount = 0;
	for(i = 0; i < num_commands; ++i)
	{
		/* Text length is too small */
		if(textLen < commands[i].nameSize)
		{
			continue;
		}
		
		/* Lenghts match */
		if(textLen == commands[i].nameSize)
		{
			if(strncmp(text, commands[i].name, commands[i].nameSize) == 0)
			{
				return i;
			}
			continue;
		}
		
		/* Text is longer, check if there is whitespace after command */
		if(isspace(text[commands[i].nameSize]))
		{
			if(strncmp(text, commands[i].name, commands[i].nameSize) == 0)
			{
				if(commands[i].argCount > 0)
				{
					/* Parse parameters */
					debugger->argCount = 0;
					for(j = commands[i].nameSize; j < textLen; j++)
					{
						/* replace whitespace with nul
						 * so that arguments are well terminated */
						if(isspace(text[j]))
						{
							text[j] = 0;
							continue;
						}
						
						/* whitespace followed by word */
						if(text[j-1] == 0)
						{
							debugger->args[debugger->argCount++] = &text[j];
						}
					}

					if(debugger->argCount > commands[i].argCount)
					{
						/* Too many parameters */
						return -2;
					}
				}
				return i;
			}
		}
	}
	return -1;
}

/*
 * Parse command from input text and call command function.
 */
void ParseAndExecuteCommand(ILDebugger *debugger, char *text, int textLen,
															FILE *outputStream)
{
	int cmdIndex = ParseCommand(debugger, text, textLen);
	if(cmdIndex >= 0)
	{
		/* Execute command */
		(*(commands[cmdIndex].func))(debugger, outputStream);
		
	}
	else if(cmdIndex == -1)
	{
		DumpError("Unknown command", outputStream);
	}
	else if(cmdIndex == -2)
	{
		DumpError("Too many parameters for command", outputStream);
	}
 }

/*
 * Run command loop.
 */
static void CommandLoop(ILDebugger *debugger)
{
	char *cmd;
	int nextCommand;
	do
	{
		/* Recieve command */
		if(!(debugger->io->recieve(debugger->io)))
		{
			debugger->abort = 1;
			break;
		}

		/* Read command text from input stream */
		cmd = ReadStream(debugger->io->input);
		if(cmd == 0)
		{
			debugger->abort = 1;
			break;
		}

		/* Dump response header */
		fputs("<DebuggerResponse>\n", debugger->io->output);

		/* Lock down the debugger */
		LockDebugger(debugger);
		
		/* Execute command */
		ParseAndExecuteCommand(debugger, cmd, strlen(cmd),
														debugger->io->output);

		/* Next command flag - true until debugger is stopped */
		nextCommand =
				(debugger->dbthread->runType == IL_DEBUGGER_RUN_TYPE_STOPPED);

		/* Unlock the debugger */
		UnlockDebugger(debugger);

		/* Dump response footer */
		fputs("\n</DebuggerResponse>\n", debugger->io->output);

		/* Free command text, since it was allocated on head */
		ILFree(cmd);

		/* Send response to client */
		if(!(debugger->io->send(debugger->io)))
		{
			debugger->abort = 1;
			break;
		}
	}
	while(nextCommand);
}

/*
 * Help command.
 */
static void Help(ILDebugger *debugger, FILE *stream)
{
	int i;
	char *msg;
	int size, maxSize; 

	/* Print the help header */
	fputs("Usage: command [arguments]\n", stream);
	fputs("Commands:\n", stream);

	/* Scan the command table to determine the width of the tab column */
	maxSize = 0;
	for(i = 0; i < num_commands; ++i)
	{
		msg = commands[i].helpName;
		if(!msg)
		{
			continue;
		}
		size = strlen(msg);
		if(size > maxSize)
		{
			maxSize = size;
		}
	}

	/* Dump the help messages in the command table */
	for(i = 0; i < num_commands; ++i)
	{
		if(i > 0)
		{
			putc('\n', stream);
		}
		msg = commands[i].helpName;
		if(!msg)
		{
			continue;
		}
		msg = msg;
		size = 0;
		fputs(msg, stream);
		size = strlen(msg);
		while(size < maxSize)
		{
			putc(' ', stream);
			++size;
		}
		putc(' ', stream);
		putc(' ', stream);
		msg = commands[i].helpMsg;
		fputs(msg, stream);
	}
}

/*
 * Do CommandLoop() until abort is requested.
 * This function executing in command thread.
 * When abort requested, all thread have to
 * be resumed, so that they can terminate.
 */
static void CommandThreadFn(void *arg)
{
	ILDebugger *debugger = (ILDebugger *) arg;
	do
	{
		CommandLoop(debugger);
	}
	while(debugger->abort == 0);
	
	/* Resume all threads so that they can terminate
	 * by returning IL_HOOK_ABORT */
	Resume(debugger, IL_DEBUGGER_RUN_TYPE_CONTINUE);
}

/*
 * Start command-loop in separate thread.
 */
int StartCommandThread(ILDebugger *debugger)
{
	if(!ILHasThreads())
	{
		return 0;
	}

	/* Create command loop thread  */
	debugger->commandThread = ILThreadCreate(CommandThreadFn,
														(void *) debugger);

	if(debugger->commandThread == 0)
	{
		return 0;
	}

	/* Start command loop thread */
	if(!ILThreadStart(debugger->commandThread))
	{
		ILThreadDestroy(debugger->commandThread);
		debugger->commandThread = 0;
		return 0;
	}

	return 1;
}

/*
 * Debug hook.
 */
static int DebugHook(void *userData, ILExecThread *thread, ILMethod *method,
					 ILInt32 offset, int type)
{
	int stop;
	ILUInt32 line;
	ILUInt32 col;
	const char *sourceFile;
	ILDebugger *debugger;
	ILDebuggerThreadInfo *info;
	ILBreakpoint *breakpoint;

	/* Get debugger attached to thread's process */
	debugger = _ILExecThreadProcess(thread)->debugger;

	/* Dont break in methods whose images are not watched */
	if(!ILDebuggerIsAssemblyWatched(debugger, method))
	{
		return IL_HOOK_CONTINUE;
	}

	/* Read current position from debug info */
	sourceFile = GetLocation(method, offset, &line, &col);

	/* The most important printf if you want to debug this debugger */
	/* fprintf(stderr, "DebugHook method=%s.%s offset=%d sourceFile=%s line=%d\n",
							ILClass_Name(ILMethod_Owner(method)),
							ILMethod_Name(method),
							offset, sourceFile, line); */

	/* Continue if no debug info available */
	if(sourceFile == 0)
	{
		return IL_HOOK_CONTINUE;
	}

	/* Lock down the debugger */
	LockDebugger(debugger);

	/* Get or create debugger info about thread */
	info = GetDbThreadInfo(debugger, thread);

	//fprintf(stderr, " run type %d\n", info->runType);

	if(debugger->breakAll)
	{
		/* Always stop when break requested (by break command) */
		stop = 1;
	}
	else
	{
		/* Determine if we should stop */
		switch(info->runType)
		{
			case IL_DEBUGGER_RUN_TYPE_CONTINUE:
			{
				/* Stop if we have valid breakpoint
				 * for this method and offset */
				breakpoint = GetBreakpoint(debugger, method, offset);
				stop = (breakpoint->method != 0);
			}
			break;
	
			case IL_DEBUGGER_RUN_TYPE_STEP:
			{
				/* Stop if current method or location changed */
				stop =  (info->method != method) ||
					(line != debugger->dbthread->line) ||
					(col != debugger->dbthread->col);
			}
			break;
	
			case IL_DEBUGGER_RUN_TYPE_NEXT:
			{
				/* Stop in the same method when location changed
				or if current stack trace height is smaller then before. */
				if(method == info->method)
				{
					stop = ((line != info->line) ||
						(col != info->col));
				}
				else
				{
					stop = (_IL_StackFrame_InternalGetTotalFrames(thread) <
															info->numFrames);
				}
			}
			break;
			
			case IL_DEBUGGER_RUN_TYPE_UNTIL:
			{
				/* Stop in the same method when location moves further
				or if current stack trace height is smaller then before. */
				if(method == debugger->dbthread->method)
				{
					stop = ((line > info->line) ||
						(col > info->col));
				}
				else
				{
					stop = (_IL_StackFrame_InternalGetTotalFrames(thread) <
															info->numFrames);
				}
			}
			break;
			
			case IL_DEBUGGER_RUN_TYPE_FINISH:
			{
				/* Stop on the last instrucion of current method. TODO */
				stop = 1;
			}
			break;
			
			default:
			{
				stop = 1;
			}
			break;
		}
	}

	/* Continue execution, if we dont need to stop */
	if(!stop)
	{
		/* Unlock the debugger */
		UnlockDebugger(debugger);

		return IL_HOOK_CONTINUE;
	}

	/* Update information about current exec thread state */
	UpdateDbThreadInfo(info, userData, thread, method, offset, type,
													sourceFile, line, col);

	/* Mark thread as stopped */
	info->runType = IL_DEBUGGER_RUN_TYPE_STOPPED;

	/* If we are the first thread that stopped, make this thread current */
	if(debugger->dbthread->runType != IL_DEBUGGER_RUN_TYPE_STOPPED)
	{
		debugger->dbthread = info;
	}

	/* Unlock the debugger */
	UnlockDebugger(debugger);

	/* Check command loop thread */
	if(debugger->commandThread || StartCommandThread(debugger))
	{
		/* Suspend until resumed by continue, step or similar command */
		_ILExecThreadSuspendThread(thread, thread->supportThread);
	}
	else
	{
		/* Do commands in current thread */
		CommandLoop(debugger);
	}

	/* Return hook result */
	if(debugger->abort)
	{
		return IL_HOOK_ABORT;
	}
	else
	{
		return IL_HOOK_CONTINUE;
	}
}

/*
 * Destroy debugger IO.
 */
static void ILDebuggerIODestroy(ILDebuggerIO *io)
{
	/* Close and destroy implementation specific stuff */
	io->close(io);

	/* Destroy common stuff */
	if(io->input)
	{
		fclose(io->input);
	}
	if(io->output)
	{
		fclose(io->output);
	}
	ILFree(io);
}

void ILDebuggerSetIO(ILDebugger *debugger, ILDebuggerIO *io)
{
	debugger->io = io;
}

int ILDebuggerConnect(ILDebugger *debugger, char *connectionString)
{
	/* Check if custom IO was previously set */
	if(debugger->io == 0)
	{
		/* Only supported connection string is tcp://localhost:4571
		 * return error for anything else for now */
		if(strcmp(connectionString, "tcp://localhost:4571") != 0)
		{
			fprintf(stderr, "il_debugger: connection string not supported\n");
			return 0;
		}
	
		/* Create debugger IO from known connectionString scheme */
		debugger->io = (ILDebuggerIO *) ILMalloc(sizeof(ILDebuggerIO));
		if(debugger->io == 0)
		{
			return 0;
		}

		/*
		 * Only supported connection string is tcp://localhost:4571
		 * so setup members for socket based IO
		 */
		debugger->io->open = &SocketIO_Open;
		debugger->io->close = &SocketIO_Close;
		debugger->io->recieve = &SocketIO_Recieve;
		debugger->io->send = &SocketIO_Send;
	}

	/* Try to connect to debugger client */
	return debugger->io->open(debugger->io, connectionString);
}

int ILDebuggerIsAttached(ILExecProcess *process)
{
	return process->debugger != 0;
}

ILDebugger *ILDebuggerFromProcess(ILExecProcess *process)
{
	return process->debugger;
}

void ILDebuggerDestroy(ILDebugger *debugger)
{
	/* Destroy initialized members */
	if(debugger->commandThread)
	{
		ILThreadDestroy(debugger->commandThread);
	}
	if(debugger->lock)
	{
		ILMutexDestroy(debugger->lock);
	}
	if(debugger->dbthread)
	{
		ILDebuggerThreadInfo_Destroy(debugger->dbthread);
	}
	/* Destroy assembly watch list */
	DestroyAssemblyWatches(debugger);

	if(debugger->io)
	{
		ILDebuggerIODestroy(debugger->io);
	}
	/* Unregister hook function */
	ILExecProcessDebugHook(debugger->process, 0, 0);

	/* Detach debugger from process */
	if(debugger->process)
	{
		debugger->process->debugger = 0;
	}
	ILFree(debugger);
}

ILDebugger *ILDebuggerCreate(ILExecProcess *process)
{
	ILDebugger *debugger;

	/* Check if command table is ok - useful for for debuging */
	if(!CheckCommands())
	{
		return 0;
	}

	/* Allocate and initialize debugger structure */
	debugger = (ILDebugger *) ILMalloc(sizeof(ILDebugger));
	if(debugger == 0)
	{
		return 0;
	}
	memset(debugger, 0, sizeof(ILDebugger));

	/* Assign reference to exec process */
	debugger->process = process;

	/* Create mutex for locking down debugger */
	debugger->lock = ILMutexCreate();
	if(!(debugger->lock))
	{
		fprintf(stderr, "il_debugger: failed to create mutex\n");
		ILDebuggerDestroy(debugger);
		return 0;
	}

	/* Assembly watch list */
	debugger->assemblyWatches = 0;

	/* Default thread info */
	debugger->dbthread = ILDebuggerThreadInfo_Create();
	debugger->dbthread->runType = IL_DEBUGGER_RUN_TYPE_STEP;

	/* Set breakAll so that we stop on first program instruction */
	debugger->breakAll = 1;

	/* Initialize breakpoint list */
	debugger->breakpoint = ILBreakpoint_Create();

	/* Initialize command thread */
	debugger->commandThread = 0;

	/* Register debug hook function */
	if(ILExecProcessDebugHook(process, DebugHook, 0) == 0)
	{
		ILDebuggerDestroy(debugger);
		fprintf(stderr, "il_debugger: the runtime engine does not support debugging.\n");
		return 0;
	}

	/* This will call DebugHook()
	 * before the first program instruction executes */
	ILExecProcessWatchAll(process, 1);

	/* Attach debugger to process */
	process->debugger = debugger;

	return debugger;
}

#endif	/* IL_CONFIG_DEBUGGER */
