/*
 * StreamWriter.cs - Implementation of the "System.IO.StreamWriter" class.
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

namespace System.IO
{

using System;
using System.Globalization;
using System.Text;

public class StreamWriter : TextWriter
{
#if !ECMA_COMPAT
	// The null stream writer.
	public new static readonly StreamWriter Null
			= new StreamWriter(Stream.Null);
#endif

	// Default and minimum buffer sizes to use for streams.
	private const int STREAM_BUFSIZ = 1024;
	private const int FILE_BUFSIZ   = 4096;
	private const int MIN_BUFSIZ    = 128;

	// Internal state.
	private Stream 		stream;
	private Encoding	encoding;
	private int    		bufferSize;
	private char[] 		inBuffer;
	private int    		inBufferPosn;
	private int    		inBufferLen;
	private byte[] 		outBuffer;
	private int    		outBufferPosn;
	private int    		outBufferLen;
	private bool   		autoFlush;

	// Constructors that are based on a stream.
	public StreamWriter(Stream stream)
			: this(stream, Encoding.UTF8, STREAM_BUFSIZ) {}
	public StreamWriter(Stream stream, Encoding encoding)
			: this(stream, encoding, STREAM_BUFSIZ) {}
	public StreamWriter(Stream stream, Encoding encoding, int bufferSize)
			{
				// Validate the parameters.
				if(stream == null)
				{
					throw new ArgumentNullException("stream");
				}
				if(encoding == null)
				{
					throw new ArgumentNullException("encoding");
				}
				if(!stream.CanWrite)
				{
					throw new ArgumentException(_("IO_NotSupp_Write"));
				}
				if(bufferSize <= 0)
				{
					throw new ArgumentOutOfRangeException
						("bufferSize", _("ArgRange_BufferSize"));
				}
				if(bufferSize < MIN_BUFSIZ)
				{
					bufferSize = MIN_BUFSIZ;
				}

				// Initialize this object.
				this.stream = stream;
				this.encoding = encoding;
				this.bufferSize = bufferSize;
				this.inBuffer = new char [bufferSize];
				this.inBufferPosn = 0;
				this.inBufferLen = 0;
				this.outBuffer = new byte [bufferSize];
				this.outBufferPosn = 0;
				this.outBufferLen = 0;
				this.autoFlush = false;

				// Write the encoding's preamble.
				WritePreamble();
			}

	// Constructors that are based on a filename.
	public StreamWriter(String path)
			: this(path, false, Encoding.UTF8, STREAM_BUFSIZ) {}
	public StreamWriter(String path, bool append)
			: this(path, append, Encoding.UTF8, STREAM_BUFSIZ) {}
	public StreamWriter(String path, bool append, Encoding encoding)
			: this(path, append, encoding, STREAM_BUFSIZ) {}
	public StreamWriter(String path, bool append,
						Encoding encoding, int bufferSize)
			{
				// Validate the parameters.
				if(path == null)
				{
					throw new ArgumentNullException("path");
				}
				if(encoding == null)
				{
					throw new ArgumentNullException("encoding");
				}
				if(bufferSize <= 0)
				{
					throw new ArgumentOutOfRangeException
						("bufferSize", _("ArgRange_BufferSize"));
				}
				if(bufferSize < MIN_BUFSIZ)
				{
					bufferSize = MIN_BUFSIZ;
				}

				// Attempt to open the file.
				Stream stream = new FileStream(path,
											   (append ? FileMode.Append
											   		   : FileMode.Create),
											   FileAccess.Write,
											   FileShare.Read,
											   FILE_BUFSIZ);

				// Initialize this object.
				this.stream = stream;
				this.encoding = encoding;
				this.bufferSize = bufferSize;
				this.inBuffer = new char [bufferSize];
				this.inBufferPosn = 0;
				this.inBufferLen = 0;
				this.outBuffer = new byte [bufferSize];
				this.outBufferPosn = 0;
				this.outBufferLen = 0;
				this.autoFlush = false;

				// Write the encoding's preamble.
				WritePreamble();
			}

	// Destructor.
	~StreamWriter()
			{
				Dispose(false);
			}

	// Write the encoding's preamble to the output stream.
	private void WritePreamble()
			{
				byte[] preamble = encoding.GetPreamble();
				if(preamble != null)
				{
					stream.Write(preamble, 0, preamble.Length);
				}
			}

	// Close this stream writer.
	public override void Close()
			{
				Dispose(true);
			}

	// Dispose this stream writer.
	protected override void Dispose(bool disposing)
			{
				if(stream != null)
				{
					if(outBufferPosn < outBufferLen)
					{
						stream.Write(outBuffer, outBufferPosn,
									 outBufferLen - outBufferPosn);
					}
					stream.Close();
					stream = null;
				}
				inBuffer = null;
				inBufferPosn = 0;
				inBufferLen = 0;
				outBuffer = null;
				outBufferPosn = 0;
				outBufferLen = 0;
				bufferSize = 0;
				base.Dispose(disposing);
			}

	// Flush the contents of the writer's buffer to the underlying stream.
	public override void Flush()
			{
				if(stream == null)
				{
					throw new ObjectDisposedException(_("IO_StreamClosed"));
				}
				if(outBufferPosn < outBufferLen)
				{
					stream.Write(outBuffer, outBufferPosn,
								 outBufferLen - outBufferPosn);
					outBufferPosn = 0;
					outBufferLen = 0;
				}
				stream.Flush();
			}

	// Write a string to the stream writer.
	public override void Write(String value)
			{
				// TODO
			}

	// Write a buffer of characters to this stream writer.
	public override void Write(char[] buffer, int index, int count)
			{
				if(buffer == null)
				{
					throw new ArgumentNullException("buffer");
				}
				if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", _("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_Array"));
				}
				if((buffer.Length - index) < count)
				{
					throw new ArgumentException
						(_("Arg_InvalidArrayRange"));
				}
				// TODO
			}
	public override void Write(char[] buffer)
			{
				if(buffer == null)
				{
					throw new ArgumentNullException("buffer");
				}
				Write(buffer, 0, buffer.Length);
			}

	// Write a single character to this stream writer.
	public override void Write(char value)
			{
				// TODO
			}

	// Get or set the autoflush state of this stream writer.
	public virtual bool AutoFlush
			{
				get
				{
					return autoFlush;
				}
				set
				{
					autoFlush = value;
				}
			}

	// Get the base stream underlying this stream writer.
	public virtual Stream BaseStream
			{
				get
				{
					return stream;
				}
			}

	// Get the current encoding in use by this stream writer.
	public override Encoding Encoding
			{
				get
				{
					return encoding;
				}
			}

}; // class StreamWriter

}; // namespace System.IO
