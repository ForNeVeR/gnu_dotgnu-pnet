// This sample prints information about the code pages that are present
// in the system library.  Usage as follows:
//
//    ilrun codepage.exe
//       -- List all code pages using a short one line per page form.
//
//    ilrun codepage.exe -v
//       -- List all code pages using a verbose form.
//
//    ilrun codepage.exe nnn
//       -- Print verbose information about code page "nnn".
//
//    ilrun codepage.exe name
//       -- Print verbose information about web encoding "name".

using System;
using System.Text;

class CodePage
{
	// Print information about an encoding.
	private static void PrintEncoding(Encoding enc, bool verbose, bool isdef)
	{
		if(verbose)
		{
			Console.Write("{0}: ", enc.CodePage);
			Console.Write(enc.EncodingName);
			if(isdef)
			{
				Console.Write(" [default encoding]");
			}
			Console.WriteLine();
			Console.Write("\tBodyName=");
			Console.WriteLine(enc.BodyName);
			Console.Write("\tHeaderName=");
			Console.WriteLine(enc.HeaderName);
			Console.Write("\tWebName=");
			Console.WriteLine(enc.WebName);
			Console.Write("\tIsBrowserDisplay=");
			Console.WriteLine(enc.IsBrowserDisplay);
			Console.Write("\tIsBrowserSave=");
			Console.WriteLine(enc.IsBrowserSave);
			Console.Write("\tIsMailNewsDisplay=");
			Console.WriteLine(enc.IsMailNewsDisplay);
			Console.Write("\tIsMailNewsSave=");
			Console.WriteLine(enc.IsMailNewsSave);
			Console.Write("\tWindowsCodePage=");
			Console.WriteLine(enc.WindowsCodePage);
		}
		else
		{
			Console.Write("{0,5}: ", enc.CodePage);
			Console.Write(enc.EncodingName);
			if(isdef)
			{
				Console.Write(" [default encoding]");
			}
			Console.WriteLine();
		}
	}

	// Print all code pages in the system.
	private static void PrintAll(bool verbose)
	{
		int page;
		int defaultPage;
		Encoding enc;
		enc = Encoding.Default;
		defaultPage = enc.CodePage;
		for(page = 1; page < 65536; ++page)
		{
			try
			{
				enc = Encoding.GetEncoding(page);
			}
			catch(NotSupportedException)
			{
				enc = null;
			}
			if(enc != null)
			{
				PrintEncoding(enc, verbose, (page == defaultPage));
				if(verbose)
				{
					Console.WriteLine();
				}
			}
		}
	}

	// Print information about a specific code page.
	private static void PrintPage(int page)
	{
		Encoding enc;
		try
		{
			enc = Encoding.GetEncoding(page);
		}
		catch(NotSupportedException)
		{
			enc = null;
		}
		if(enc != null)
		{
			PrintEncoding(enc, true, false);
		}
		else
		{
			Console.Write(page);
			Console.WriteLine(": unknown code page");
		}
	}

	// Print information about a specific web encoding.
	private static void PrintWebEncoding(String name)
	{
		Encoding enc;
		try
		{
			enc = Encoding.GetEncoding(name);
		}
		catch(NotSupportedException)
		{
			enc = null;
		}
		if(enc != null)
		{
			PrintEncoding(enc, true, false);
		}
		else
		{
			Console.Write(name);
			Console.WriteLine(": unknown encoding name");
		}
	}

	public static void Main(String[] args)
	{
		if(args.Length > 0)
		{
			if(args[0] == "-v")
			{
				PrintAll(true);
			}
			else if(args[0][0] >= '0' && args[0][0] <= '9')
			{
				PrintPage(Int32.Parse(args[0]));
			}
			else
			{
				PrintWebEncoding(args[0]);
			}
		}
		else
		{
			PrintAll(false);
		}
	}
}
