/*
 * StreamReader.cs - Implementation of the "System.IO.StreamReader" class.
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

public class StreamReader : TextReader
{
#if !ECMA_COMPAT
	// The null stream reader.
	public new static readonly StreamReader Null
			= new StreamReader(Stream.Null);
#endif

	// Default and minimum buffer sizes to use for streams.
	private const int STREAM_BUFSIZ = 1024;
	private const int FILE_BUFSIZ   = 4096;
	private const int MIN_BUFSIZ    = 128;

	// Internal state.
	private Stream 		stream;
	private Encoding	encoding;
	private int    		bufferSize;
	private byte[] 		inBuffer;
	private int    		inBufferPosn;
	private int    		inBufferLen;
	private char[] 		outBuffer;
	private int    		outBufferPosn;
	private int    		outBufferLen;
	private bool   		sawEOF;

	// Constructors that are based on a stream.
	public StreamReader(Stream stream)
			: this(stream, Encoding.UTF8, true, STREAM_BUFSIZ) {}
	public StreamReader(Stream stream, bool detectEncodingFromByteOrderMarks)
			: this(stream, Encoding.UTF8,
				   detectEncodingFromByteOrderMarks, STREAM_BUFSIZ) {}
	public StreamReader(Stream stream, Encoding encoding)
			: this(stream, encoding, true, STREAM_BUFSIZ) {}
	public StreamReader(Stream stream, Encoding encoding,
						bool detectEncodingFromByteOrderMarks)
			: this(stream, encoding,
				   detectEncodingFromByteOrderMarks, STREAM_BUFSIZ) {}
	public StreamReader(Stream stream, Encoding encoding,
						bool detectEncodingFromByteOrderMarks,
						int bufferSize)
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
				if(!stream.CanRead)
				{
					throw new ArgumentException(_("IO_NotSupp_Read"));
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
				this.inBuffer = new byte [bufferSize];
				this.inBufferPosn = 0;
				this.inBufferLen = 0;
				this.outBuffer = new char [bufferSize];
				this.outBufferPosn = 0;
				this.outBufferLen = 0;
				this.sawEOF = false;

				// Should we change encodings based on a byte order mark?
				if(detectEncodingFromByteOrderMarks)
				{
					DetectByteOrder();
				}
			}

	// Constructors that are based on a filename.
	public StreamReader(String path)
			: this(path, Encoding.UTF8, true, STREAM_BUFSIZ) {}
	public StreamReader(String path, bool detectEncodingFromByteOrderMarks)
			: this(path, Encoding.UTF8,
				   detectEncodingFromByteOrderMarks, STREAM_BUFSIZ) {}
	public StreamReader(String path, Encoding encoding)
			: this(path, encoding, true, STREAM_BUFSIZ) {}
	public StreamReader(String path, Encoding encoding,
						bool detectEncodingFromByteOrderMarks)
			: this(path, encoding,
				   detectEncodingFromByteOrderMarks, STREAM_BUFSIZ) {}
	public StreamReader(String path, Encoding encoding,
						bool detectEncodingFromByteOrderMarks,
						int bufferSize)
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
				Stream stream = new FileStream(path, FileMode.Open,
											   FileAccess.Read,
											   FileShare.Read,
											   FILE_BUFSIZ);

				// Initialize this object.
				this.stream = stream;
				this.encoding = encoding;
				this.bufferSize = bufferSize;
				this.inBuffer = new byte [bufferSize];
				this.inBufferPosn = 0;
				this.inBufferLen = 0;
				this.outBuffer = new char [bufferSize];
				this.outBufferPosn = 0;
				this.outBufferLen = 0;
				this.sawEOF = false;

				// Should we change encodings based on a byte order mark?
				if(detectEncodingFromByteOrderMarks)
				{
					DetectByteOrder();
				}
			}

	// Destructor.
	~StreamReader()
			{
				Dispose(false);
			}

	// Detect the byte order by inspecting the first few bytes.
	private void DetectByteOrder()
			{
				// Pre-read the first full buffer of input data.
				inBufferLen = stream.Read(inBuffer, 0, bufferSize);
				if(inBufferLen <= 0)
				{
					sawEOF = true;
					inBufferLen = 0;
				}

				// Check for recognized byte order marks.
				if(inBufferLen >= 2 &&
				   inBuffer[0] == 0xFF &&
				   inBuffer[1] == 0xFE)
				{
					// Little-endian UTF-16.
					encoding = Encoding.Unicode;
					inBufferPosn = 2;
				}
				else if(inBufferLen >= 2 &&
				        inBuffer[0] == 0xFE &&
				        inBuffer[1] == 0xFF)
				{
					// Big-endian UTF-16.
					encoding = Encoding.BigEndianUnicode;
					inBufferPosn = 2;
				}
				else if(inBufferLen >= 3 &&
						inBuffer[0] == 0xEF &&
						inBuffer[1] == 0xBB &&
						inBuffer[2] == 0xBF)
				{
					// UTF-8.
					encoding = Encoding.UTF8;
					inBufferPosn = 3;
				}
			}

	// Close this stream reader.
	public override void Close()
			{
				Dispose(true);
			}

	// Discard any data that was buffered by this stream reader.
	public void DiscardBufferedData()
			{
				inBufferPosn = 0;
				inBufferLen = 0;
				outBufferPosn = 0;
				outBufferLen = 0;
				sawEOF = false;
			}

	// Dispose this stream reader.
	protected override void Dispose(bool disposing)
			{
				if(stream != null)
				{
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
				sawEOF = true;
				base.Dispose(disposing);
			}

	// Peek at the next character from this stream reader.
	public override int Peek()
			{
				// TODO
				return -1;
			}

	// Read a single character from this stream reader.
	public override int Read()
			{
				// TODO
				return -1;
			}

	// Read a buffer of characters from this stream reader.
	public override int Read(char[] buffer, int index, int count)
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
				return 0;
			}

	// Read a line of characters from this stream reader.
	public override String ReadLine()
			{
				// TODO
				return base.ReadLine();
			}

	// Read the entire contents of this stream reader until EOF.
	public override String ReadToEnd()
			{
				// TODO
				return base.ReadToEnd();
			}

	// Get the base stream underlying this stream reader.
	public virtual Stream BaseStream
			{
				get
				{
					return stream;
				}
			}

	// Get the current encoding in use by this stream reader.
	public virtual Encoding CurrentEncoding
			{
				get
				{
					return encoding;
				}
			}

}; // class StreamReader

}; // namespace System.IO
