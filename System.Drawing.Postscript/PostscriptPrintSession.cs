/*
 * PostscriptPrintSession.cs - Implementation of the
 *			"System.Drawing.Toolkit.PostscriptPrintSession" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace System.Drawing.Toolkit
{

using System.Drawing.Printing;

internal sealed class PostscriptPrintSession : IToolkitPrintSession
{
	// Internal state.
	private String printerName;
	private PrintDocument document;

	// Constructor.
	public PostscriptPrintSession(String printerName, PrintDocument document)
			{
				this.printerName = printerName;
				this.document = document;
			}

	// Get or set the document that is associated with this session.
	public PrintDocument Document
			{
				get
				{
					return document;
				}
				set
				{
					document = value;
				}
			}

	// Start the printing session.
	public void StartPrint(PrintEventArgs e)
			{
				// TODO
			}

	// End the printing session.
	public void EndPrint(PrintEventArgs e)
			{
				// TODO
			}

	// Start printing a page, and return a "Graphics" object for it.
	public Graphics StartPage(PrintPageEventArgs e)
			{
				// TODO
				return null;
			}

	// End printing of the current page.
	public void EndPage(PrintPageEventArgs e)
			{
				// TODO
			}

	// Get the preview page information for all pages.  This is only
	// used for preview sessions.  Normal printers should return null.
	public PreviewPageInfo[] GetPreviewPageInfo()
			{
				// Not used.
				return null;
			}

	// Get or set the anti-alias flag for preview operations.
	public bool UseAntiAlias
			{
				get
				{
					// Not used.
					return false;
				}
				set
				{
					// Not used.
				}
			}

}; // class PostscriptPrintSession

}; // namespace System.Drawing.Postscript
