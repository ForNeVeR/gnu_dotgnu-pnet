/*
 * PreviewPrintController.cs - Implementation of the
 *			"System.Drawing.Printing.PreviewPrintController" class.
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

namespace System.Drawing.Printing
{

public class PreviewPrintController : PrintController
{
	// Internal state.
	private bool useAntiAlias;

	// Constructor.
	public PreviewPrintController()
			{
				this.useAntiAlias = false;
			}

	// Get or set the anti-alias property.
	public virtual bool UseAntiAlias
			{
				get
				{
					return useAntiAlias;
				}
				set
				{
					useAntiAlias = value;
				}
			}

	// Get the page preview information for all pages.
	[TODO]
	public PreviewPageInfo[] GetPreviewPageInfo()
			{
				// TODO
				return null;
			}

	// Event that is emitted at the end of a page.
	[TODO]
	public override void OnEndPage
				(PrintDocument document, PrintPageEventArgs e)
			{
				// TODO
				base.OnEndPage(document, e);
			}

	// Event that is emitted at the end of the print process.
	[TODO]
	public override void OnEndPrint
				(PrintDocument document, PrintEventArgs e)
			{
				// TODO
				base.OnEndPrint(document, e);
			}

	// Event that is emitted at the start of a page.
	[TODO]
	public override void OnStartPage
				(PrintDocument document, PrintPageEventArgs e)
			{
				// TODO
				base.OnStartPage(document, e);
			}

	// Event that is emitted at the start of the print process.
	[TODO]
	public override void OnStartPrint
				(PrintDocument document, PrintEventArgs e)
			{
				// TODO
				base.OnStartPrint(document, e);
			}

}; // class PreviewPrintController

}; // namespace System.Drawing.Printing
