/*
 * FileDialog.cs - Implementation of the
 *			"System.Windows.Forms.FileDialog" class.
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

namespace System.Windows.Forms
{

using System.IO;
using System.Drawing;
using System.ComponentModel;

public abstract class FileDialog : CommonDialog
{
	// Internal state.
	private bool addExtension;
	internal bool checkFileExists;
	private bool checkPathExists;
	private bool dereferenceLinks;
	private String defaultExt;
	private String fileName;
	private String[] fileNames;
	private String filter;
	private int filterIndex;
	private String initialDirectory;
	private bool restoreDirectory;
	private bool showHelp;
	private bool validateNames;
	private String title;
	private FileDialogForm form;

	// Event identifier for the "FileOk" event.
	protected static readonly object EventFileOk = new Object();

	// Constructor.
	internal FileDialog()
			{
				// Make sure that the dialog fields have their default values.
				Reset();
			}

	// Get or set this object's properties.
	public bool AddExtension
			{
				get
				{
					return addExtension;
				}
				set
				{
					addExtension = value;
				}
			}
	public virtual bool CheckFileExists
			{
				get
				{
					return checkFileExists;
				}
				set
				{
					checkFileExists = value;
				}
			}
	public bool CheckPathExists
			{
				get
				{
					return checkPathExists;
				}
				set
				{
					checkPathExists = value;
				}
			}
	public String DefaultExt
			{
				get
				{
					return defaultExt;
				}
				set
				{
					defaultExt = value;
				}
			}
	public bool DereferenceLinks
			{
				get
				{
					return dereferenceLinks;
				}
				set
				{
					dereferenceLinks = value;
				}
			}
	[TODO]
	public String FileName
			{
				get
				{
					return fileName;
				}
				set
				{
					// TODO: update the dialog box to match.
					fileName = value;
					fileNames = null;
				}
			}
	public String[] FileNames
			{
				get
				{
					if(fileNames == null)
					{
						if(fileName != null && fileName.Length > 0)
						{
							fileNames = new String [] {fileName};
							return fileNames;
						}
						else
						{
							return new String [0];
						}
					}
					else
					{
						return fileNames;
					}
				}
			}
	[TODO]
	public String Filter
			{
				get
				{
					return filter;
				}
				set
				{
					// TODO: validate the filter string.
					// TODO: update the dialog box to match.
					filter = value;
				}
			}
	[TODO]
	public int FilterIndex
			{
				get
				{
					return filterIndex;
				}
				set
				{
					// TODO: validate the filter index.
					// TODO: update the dialog box to match.
					filterIndex = value;
				}
			}
	public String InitialDirectory
			{
				get
				{
					return initialDirectory;
				}
				set
				{
					initialDirectory = value;
				}
			}
	protected virtual IntPtr Instance
			{
				get
				{
					// Not used in this implementation.
					return IntPtr.Zero;
				}
			}
	protected int Options
			{
				get
				{
					// Not used in this implementation.
					return 0;
				}
			}
	public bool RestoreDirectory
			{
				get
				{
					return restoreDirectory;
				}
				set
				{
					restoreDirectory = value;
				}
			}
	public bool ShowHelp
			{
				get
				{
					return showHelp;
				}
				set
				{
					showHelp = value;
				}
			}
	public String Title
			{
				get
				{
					if(title == null || title == String.Empty)
					{
						return DefaultTitle;
					}
					return title;
				}
				set
				{
					title = value;
					if(form != null)
					{
						if(value == null)
						{
							form.Text = String.Empty;
						}
						else
						{
							form.Text = value;
						}
					}
				}
			}
	internal virtual String DefaultTitle
			{
				get
				{
					// Overridden by subclasses to supply "Open" or "Save As".
					return String.Empty;
				}
			}
	public bool ValidateNames
			{
				get
				{
					return validateNames;
				}
				set
				{
					validateNames = value;
				}
			}
	internal virtual String OkButtonName
			{
				get
				{
					return S._("SWF_FileDialog_OpenButton", "Open");
				}
			}

	// Hook procedure - not used in this implementation.
	protected override IntPtr HookProc(IntPtr hWnd, int msg,
									   IntPtr wparam, IntPtr lparam)
			{
				return IntPtr.Zero;
			}

	// Raise the "FileOk" event.
	protected void OnFileOk(CancelEventArgs e)
			{
				if(FileOk != null)
				{
					FileOk(this, e);
				}
			}

	// Reset the dialog box controls to their default values.
	[TODO]
	public override void Reset()
			{
				addExtension = true;
				checkFileExists = false;
				checkPathExists = true;
				dereferenceLinks = true;
				defaultExt = String.Empty;
				fileName = String.Empty;
				fileNames = null;
				filter = "All files (*.*)|*.*";
				filterIndex = 1;
				initialDirectory = String.Empty;
				restoreDirectory = false;
				showHelp = false;
				validateNames = true;
				title = String.Empty;
				// TODO: update an active dialog box form to match.
			}

	// Run the dialog box.
	protected override bool RunDialog(IntPtr hWndOwner)
			{
				// This version is not used in this implementation.
				return false;
			}
	internal override DialogResult RunDialog(IWin32Window owner)
			{
				// If the dialog is already visible, then bail out.
				if(form != null)
				{
					return DialogResult.Cancel;
				}

				// Construct the file dialog form.
				form = new FileDialogForm(this);

				// Run the dialog and get its result.
				DialogResult result;
				try
				{
					result = form.ShowDialog(owner);
				}
				finally
				{
					form.DisposeDialog();
					form = null;
				}
				return result;
			}

	// Convert this object into a string.
	public override string ToString()
			{
				return base.ToString() + ": Title: " + Title +
					   ", FileName: " + FileName;
			}

	// Event that is raised to check that a file is OK.
	public event CancelEventHandler FileOk;

	// Icon codes.
	private const short IconCode_Directory = 0;
	private const short IconCode_Drive     = 1;
	private const short IconCode_File      = 2;
	private const short IconCode_Link      = 3;
	private const short IconCode_App       = 4;
	private const short IconCode_Dll       = 5;
	private const short IconCode_Text      = 6;
	private const short IconCode_Num       = 7;

	// Icon sizes and item spacing.
	private const int IconWidth				= 16;
	private const int IconHeight			= 16;
	private const int IconSpacing			= 4;
	private const int ColumnSpacing			= 32;
	private const int TextSelectOverlap		= 2;

	// Information that is stored for a filesystem entry.
	private sealed class FilesystemEntry : IComparable
	{
		public String name;				// Name to display in dialog box.
		public String fullName;			// Full pathname of the entry.
		public bool isDirectory;		// True if a directory.
		public bool isSymlink;			// True if a symbolic link.
		public short iconCode;			// Icon to display with the item.

		// Compare two entries.  Directories always sort before files.
		public int CompareTo(Object obj)
				{
					FilesystemEntry other = (obj as FilesystemEntry);
					if(other != null)
					{
						if(isDirectory)
						{
							if(!(other.isDirectory))
							{
								return -1;
							}
						}
						else if(other.isDirectory)
						{
							return 1;
						}
						return String.Compare(name, other.name, true);
					}
					else
					{
						return 1;
					}
				}

		// Resolve this entry as a symbolic link.
		public void ResolveSymlinks()
				{
				#if __CSCC__
					int count = 20;
					String path = fullName;
					String link;
					do
					{
						try
						{
							link = SymbolicLinks.ReadLink(path);
							if(link == null)
							{
								// We've found the link destination.
								fullName = path;
								if(Directory.Exists(path))
								{
									isDirectory = true;
									iconCode = IconCode_Directory;
								}
								else
								{
									iconCode = IconCode_File;
								}
								return;
							}
							else
							{
								path = Path.Combine(path, link);
							}
						}
						catch(Exception)
						{
							// The path doesn't exist, so the link is
							// pointing at nothing.  Bail out without
							// resolving it.
							return;
						}
					}
					while(--count > 0);
				#endif
				}

		// Set the icon code based on the file's extension.
		public void SetIconCode()
				{
					if(iconCode != IconCode_File)
					{
						return;
					}
					switch(Path.GetExtension(name).ToLower())
					{
						case ".txt": case ".text": case ".c": case ".cs":
						case ".h": case ".cc": case ".cpp": case ".hpp":
						case ".doc": case ".rtf": case ".xml": case ".texi":
						case ".tex": case ".html":
						{
							iconCode = IconCode_Text;
						}
						break;

						case ".exe":
						{
							iconCode = IconCode_App;
						}
						break;

						case ".dll":
						{
							iconCode = IconCode_Dll;
						}
						break;

						case ".out":
						{
							if(String.Compare(name, "a.out", true) == 0)
							{
								iconCode = IconCode_App;
							}
						}
						break;

						case "":
						{
							if(String.Compare(name, "README", true) == 0)
							{
								iconCode = IconCode_Text;
							}
						}
						break;
					}
				}

		// Determine if this entry's name starts with a particular character.
		// The "ch" value is assumed to be lower case.
		public bool StartsWith(char ch)
				{
					if(name.Length > 0 && Char.ToLower(name[0]) == ch)
					{
						return true;
					}
					else
					{
						return false;
					}
				}

	}; // class FilesystemEntry

	// Determine if we appear to be running on Windows.
	private static bool IsWindows()
			{
			#if !ECMA_COMPAT
				return (Environment.OSVersion.Platform != (PlatformID)128);
			#else
				return (Path.DirectorySeparatorChar == '\\');
			#endif
			}

	// Determine if a pathname ends in ".lnk".
	private static bool EndsInLnk(String pathname)
			{
				int len = pathname.Length;
				if(len < 4)
				{
					return false;
				}
				if(pathname[len - 4] == '.' &&
				   (pathname[len - 3] == 'l' || pathname[len - 3] == 'L') &&
				   (pathname[len - 2] == 'n' || pathname[len - 2] == 'N') &&
				   (pathname[len - 1] == 'k' || pathname[len - 1] == 'K'))
				{
					return true;
				}
				return false;
			}

	// Scan a directory and collect up all of the filesystem entries.
	private static FilesystemEntry[] ScanDirectory
				(String directory, String pattern, bool derefLinks)
			{
				String[] dirs;
				String[] files;
				String[] links;
				FilesystemEntry[] entries;

				// Convert the directory name into a full pathname.
				directory = Path.GetFullPath(directory);

				// Get all sub-directories in the specified directory,
				// irrespective of whether they match the pattern or not.
				try
				{
					dirs = Directory.GetDirectories(directory);
				}
				catch(Exception)
				{
					// An error occurred while trying to scan the directory,
					// so return an empty list of entries.
					return new FilesystemEntry [0];
				}

				// Get all files and Windows shortcut link files that match
				// the pattern in the directory.
				if(pattern == null || pattern == "*" || pattern == "*.*")
				{
					files = Directory.GetFiles(directory);
					links = null;
				}
				else if(!EndsInLnk(pattern) && IsWindows())
				{
					files = Directory.GetFiles(directory, pattern);
					links = Directory.GetFiles(directory, pattern + ".lnk");
				}
				else
				{
					files = Directory.GetFiles(directory, pattern);
					links = null;
				}

				// Combine the three lists and populate the information.
				entries = new FilesystemEntry
					[dirs.Length + files.Length +
					 (links != null ? links.Length : 0)];
				int posn = 0;
				FilesystemEntry entry;
				bool resolveSymlinks = false;
				foreach(String dir in dirs)
				{
					entry = new FilesystemEntry();
					entry.name = Path.GetFileName(dir);
					entry.fullName = dir;
					entry.isDirectory = true;
					entry.isSymlink = false;
					entry.iconCode = IconCode_Directory;
					entries[posn++] = entry;
				}
				foreach(String file in files)
				{
					entry = new FilesystemEntry();
					entry.name = Path.GetFileName(file);
					entry.fullName = file;
					entry.isDirectory = false;
					entry.iconCode = IconCode_File;
				#if __CSCC__
					entry.isSymlink = SymbolicLinks.IsSymbolicLink(file);
					if(entry.isSymlink)
					{
						resolveSymlinks = true;
						entry.iconCode = IconCode_Link;
						if(EndsInLnk(file) && IsWindows())
						{
							// Strip ".lnk" from the end of the filename.
							entry.name = Path.GetFileNameWithoutExtension(file);
						}
					}
				#else
					entry.isSymlink = false;
				#endif
					entries[posn++] = entry;
					entry.SetIconCode();
				}
				if(links != null)
				{
					// We have an extra list of files that end in ".lnk".
					foreach(String link in links)
					{
						entry = new FilesystemEntry();
						entry.name = Path.GetFileNameWithoutExtension(link);
						entry.fullName = link;
						entry.isDirectory = false;
						entry.isSymlink = true;
						entry.iconCode = IconCode_Link;
						entries[posn++] = entry;
						resolveSymlinks = true;
					}
				}

				// Resolve symbolic links to the underlying file or directory.
				if(resolveSymlinks && derefLinks)
				{
					for(posn = 0; posn < entries.Length; ++posn)
					{
						entry = entries[posn];
						if(entry.isSymlink)
						{
							entry.ResolveSymlinks();
							entry.SetIconCode();
						}
					}
				}

				// Sort the entry list and return it.
				Array.Sort(entries);
				return entries;
			}

	// List box like control that manages a group of file icons.
	private sealed class FileIconListBox : Control
	{
		// Internal state.
		private FileDialogForm dialog;
		private HScrollBar scrollBar;
		private FilesystemEntry[] entries;
		private int numEntries;
		private int columns;
		private int rows;
		private int leftMostColumn;
		private int columnWidth;
		private int textHeight;
		private int rowHeight;
		private int selected;
		private Icon[] icons;

		// Constructor.
		public FileIconListBox(FileDialogForm dialog)
				{
					// Set the appropriate style properties for this control.
					this.dialog = dialog;
					ForeColor = SystemColors.WindowText;
					BackColor = SystemColors.Window;
					BorderStyleInternal = BorderStyle.Fixed3D;
					ResizeRedraw = true;
					TabStop = true;

					// Create the horizontal scroll bar and position it.
					scrollBar = new HScrollBar();
					scrollBar.ForeColor = SystemColors.ControlText;
					scrollBar.BackColor = SystemColors.Control;
					scrollBar.Visible = false;
					scrollBar.Dock = DockStyle.Bottom;
					scrollBar.TabStop = false;
					Controls.Add(scrollBar);

					// Load the icons.  We need icons for drives and links.
					icons = new Icon [IconCode_Num];
					icons[0] = new Icon(typeof(FileDialog), "small_folder.ico");
					icons[1] = new Icon(typeof(FileDialog), "small_folder.ico");
					icons[2] = new Icon
						(typeof(FileDialog), "small_document.ico");
					icons[3] = new Icon
						(typeof(FileDialog), "small_document.ico");
					icons[4] = new Icon
						(typeof(FileDialog), "small_application.ico");
					icons[5] = new Icon
						(typeof(FileDialog), "small_dll.ico");
					icons[6] = new Icon
						(typeof(FileDialog), "small_text.ico");
				}

		// Destroy the handle associated with the control.
		protected override void DestroyHandle()
				{
					base.DestroyHandle();
					if(icons != null)
					{
						foreach(Icon icon in icons)
						{
							icon.Dispose();
						}
						icons = null;
					}
				}

		// Get or set the entries in this icon box.
		public FilesystemEntry[] Entries
				{
					get
					{
						return entries;
					}
					set
					{
						entries = value;
						if(entries != null)
						{
							numEntries = entries.Length;
						}
						else
						{
							numEntries = 0;
						}
						LayoutControl();
						Invalidate();
					}
				}

		// Lay out the control after a change in contents.
		private void LayoutControl()
				{
					// Bail out early if there are no entries to display.
					if(numEntries == 0)
					{
						rows = 1;
						columns = 1;
						columnWidth = 1;
						textHeight = 1;
						rowHeight = 1;
						leftMostColumn = 0;
						selected = -1;
						scrollBar.Visible = false;
						return;
					}

					// Measure all of the entries and get the maximum
					// column width and row height values.
					SizeF size;
					Font font = Font;
					int width;
					using(Graphics graphics = CreateGraphics())
					{
						textHeight = (int)(font.GetHeight(graphics));
						rowHeight = textHeight;
						if(rowHeight < (IconHeight + 1))
						{
							rowHeight = IconHeight + 1;
						}
						columnWidth = IconWidth + IconSpacing + ColumnSpacing;
						foreach(FilesystemEntry entry in entries)
						{
							size = graphics.MeasureString(entry.name, font);
							width = (IconWidth + IconSpacing + ColumnSpacing) +
									(int)(size.Width);
							if(width > columnWidth)
							{
								columnWidth = width;
							}
						}
					}

					// Determine if the entire list will fit in the
					// client area or if we need to add a scroll bar.
					Rectangle client = ClientRectangle;
					if(columnWidth > client.Width)
					{
						columnWidth = client.Width;
					}
					if(columnWidth < 1)
					{
						columnWidth = 1;
					}
					rows = client.Height / rowHeight;
					if(rows < 1)
					{
						rows = 1;
					}
					columns = (numEntries + rows - 1) / rows;
					int visibleColumns =
						(client.Width + columnWidth - 1) / columnWidth;
					leftMostColumn = 0;
					selected = -1;
					if(columns <= visibleColumns)
					{
						// We won't need a scroll bar to display the contents.
						scrollBar.Visible = false;
					}
					else
					{
						// We need a scroll bar to display the contents.
						rows = (client.Height - scrollBar.Height) / rowHeight;
						if(rows < 1)
						{
							rows = 1;
						}
						columns = (numEntries + rows - 1) / rows;
						scrollBar.Value = 0;
						scrollBar.Maximum = columns - 1;
						scrollBar.SmallChange = 1;
						scrollBar.LargeChange = 1;
						scrollBar.Visible = true;
					}
				}

		// Handle a layout request from the parent class.
		protected override void OnLayout(LayoutEventArgs e)
				{
					base.OnLayout(e);
					LayoutControl();
				}

		// Draw a particular entry at a specified location.
		private void DrawEntry(Graphics graphics, int entry,
							   int x, int y, Font font,
							   bool clearBackground)
				{
					// Bail out if the entry index is out of range.
					if(entry < 0 || entry >= numEntries)
					{
						return;
					}

					// Compute the bounding box of the entry's text area.
					String name = entries[entry].name;
					SizeF size = graphics.MeasureString(name, font);
					int width = ((int)(size.Width)) + TextSelectOverlap * 2;
					int height = textHeight + 2;
					if(height > rowHeight)
					{
						height = rowHeight;
					}
					Rectangle textBounds = new Rectangle
						(x + (IconWidth + IconSpacing - TextSelectOverlap),
						 y + (rowHeight - height) / 2, width, height);

					// Draw the icon to the left of the text.
					graphics.DrawIcon
						(icons[entries[entry].iconCode],
						 x, y + (rowHeight - IconHeight) / 2);

					// Fill the background behind the text and get
					// the foreground color to draw the text with.
					Brush foreground;
					if(selected == entry)
					{
						if(Focused)
						{
							graphics.FillRectangle
								(SystemBrushes.Highlight, textBounds);
							foreground = SystemBrushes.HighlightText;
						}
						else
						{
							graphics.FillRectangle
								(SystemBrushes.Control, textBounds);
							foreground = SystemBrushes.ControlText;
						}
					}
					else if(clearBackground)
					{
						graphics.FillRectangle
							(SystemBrushes.Window, textBounds);
						foreground = SystemBrushes.WindowText;
					}
					else
					{
						foreground = SystemBrushes.WindowText;
					}

					// Draw the text within the specified text bounds.
					graphics.DrawString
							(name, font, foreground,
							 (float)(textBounds.X + TextSelectOverlap),
							 (float)(y + (rowHeight - textHeight) / 2));
				}

		// Draw a particular entry at its proper location.
		private void DrawEntry(int entry)
				{
					// Bail out if the entry index is out of range.
					if(entry < 0 || entry >= numEntries)
					{
						return;
					}

					// Compute the location of the entry.
					int row, column;
					int x, y;
					column = (entry / rows);
					row = entry - column * rows;
					x = (column - leftMostColumn) * columnWidth;
					y = row * rowHeight;

					// Draw the entry.
					using(Graphics graphics = CreateGraphics())
					{
						DrawEntry(graphics, entry, x, y, Font, true);
					}
				}

		// Change the selection.
		private void ChangeSelection(int entry)
				{
					if(entry < 0 || entry >= numEntries)
					{
						entry = -1;
					}
					if(selected != entry)
					{
						int oldSelected = selected;
						selected = entry;
						DrawEntry(oldSelected);
						DrawEntry(entry);
						// TODO: scroll the entry into view if necessary.
					}
				}

		// Handle a paint request from the parent class.
		protected override void OnPaint(PaintEventArgs e)
				{
					Graphics graphics = e.Graphics;
					int row, column;
					int visibleColumns =
						(columnWidth != 0 ?
							((ClientSize.Width + columnWidth - 1)
								/ columnWidth) : 0);
					Font font = Font;
					for(row = 0; row < rows; ++row)
					{
						for(column = leftMostColumn;
							column < columns &&
							column < (leftMostColumn + visibleColumns);
							++column)
						{
							DrawEntry(graphics, column * rows + row,
									  (column - leftMostColumn) * columnWidth,
									  row * rowHeight, font, false);
						}
					}
				}

		// Handle a "focus enter" event.
		protected override void OnEnter(EventArgs e)
				{
					base.OnEnter(e);
					if(selected == -1)
					{
						// Set the focus to the first entry by default.
						selected = 0;
					}
					DrawEntry(selected);
				}

		// Handle a "focus leave" event.
		protected override void OnLeave(EventArgs e)
				{
					base.OnLeave(e);
					if(selected != -1)
					{
						DrawEntry(selected);
					}
				}

		// Determine if a character is recognized by a control as an input char.
		protected override bool IsInputChar(char c)
				{
					if(Char.IsLetterOrDigit(c))
					{
						return false;
					}
					else
					{
						return base.IsInputChar(c);
					}
				}

		// Process a dialog character.  We use letters and digits to
		// select the next file starting with that character.
		protected override bool ProcessDialogChar(char charCode)
				{
					if(Char.IsLetterOrDigit(charCode))
					{
						int current = selected;
						int count = numEntries;
						charCode = Char.ToLower(charCode);
						while(count > 0)
						{
							++current;
							if(current >= numEntries)
							{
								current = 0;
							}
							if(entries[current].StartsWith(charCode))
							{
								ChangeSelection(current);
								break;
							}
							--count;
						}
						return true;
					}
					else
					{
						return base.ProcessDialogChar(charCode);
					}
				}

		// Process a dialog key.
		protected override bool ProcessDialogKey(Keys keyData)
				{
					int next = selected;
					int visibleColumns;
					switch(keyData)
					{
						case Keys.Left:
						{
							if(next >= rows)
							{
								next -= rows;
							}
						}
						break;

						case Keys.Right:
						{
							if(next == -1)
							{
								next = 0;
							}
							else
							{
								next += rows;
								if(next >= numEntries)
								{
									next = numEntries - 1;
								}
							}
						}
						break;

						case Keys.Up:
						{
							if(next > 0)
							{
								--next;
							}
						}
						break;

						case Keys.Down:
						{
							++next;
							if(next >= numEntries)
							{
								next = numEntries - 1;
							}
						}
						break;

						case Keys.Home:
						{
							next = 0;
						}
						break;

						case Keys.End:
						{
							next = numEntries - 1;
						}
						break;

						case Keys.PageUp:
						{
							visibleColumns =
								(columnWidth != 0 ?
									((ClientSize.Width + columnWidth - 1)
										/ columnWidth) : 0);
							next -= rows * visibleColumns;
							if(next < 0)
							{
								next = 0;
							}
						}
						break;

						case Keys.PageDown:
						{
							visibleColumns =
								(columnWidth != 0 ?
									((ClientSize.Width + columnWidth - 1)
										/ columnWidth) : 0);
							next += rows * visibleColumns;
							if(next >= numEntries)
							{
								next = numEntries - 1;
							}
						}
						break;

						case Keys.Enter:
						{
							ActivateSelected();
							return true;
						}
						// Not reached.

						default: return base.ProcessDialogKey(keyData);
					}
					ChangeSelection(next);
					return true;
				}

		// Activate the selected item.
		private void ActivateSelected()
				{
					// Bail out if nothing is selected.
					if(selected < 0 || selected >= numEntries)
					{
						return;
					}

					// Get the selected entry and check its type.
					FilesystemEntry entry = entries[selected];
					if(entry.isDirectory)
					{
						// This is a sub-directory which we should enter.
						dialog.ChangeDirectory(entry.fullName);
					}
					else
					{
						// The user has selected a file: end the dialog.
						// TODO
					}
				}

	}; // class FileIconListBox

	// Form that defines the UI of a file dialog.
	private sealed class FileDialogForm : Form
	{
		// Internal state.
		private FileDialog fileDialogParent;
		private VBoxLayout vbox;
		private HBoxLayout hbox;
		private GridLayout grid;
		private FileIconListBox listBox;
		private ComboBox directory;
		private Button upButton;
		private Button newDirButton;
		private Button homeButton;
		private Label nameLabel;
		private TextBox name;
		private Label typeLabel;
		private ComboBox type;
		private Button okButton;
		private Button cancelButton;
		private String currentDirectory;

		// Constructor.
		public FileDialogForm(FileDialog fileDialogParent)
				{
					// Record the parent for later access.
					this.fileDialogParent = fileDialogParent;

					// Set the caption to that specified in the parent.
					Text = fileDialogParent.Title;

					// Construct the layout boxes for the file dialog.
					vbox = new VBoxLayout();
					vbox.Dock = DockStyle.Fill;
					hbox = new HBoxLayout();
					listBox = new FileIconListBox(this);
					grid = new GridLayout(3, 2);
					grid.StretchColumn = 1;
					vbox.Controls.Add(hbox);
					vbox.Controls.Add(listBox);
					vbox.Controls.Add(grid);
					vbox.StretchControl = listBox;

					// Add the top line (directory name and up button).
					directory = new ComboBox();
					upButton = new Button();
					upButton.FlatStyle = FlatStyle.Popup;
					upButton.Text = "Up";	// TODO: change to an image.
					newDirButton = new Button();
					newDirButton.FlatStyle = FlatStyle.Popup;
					newDirButton.Text = "New";	// TODO: change to an image.
					homeButton = new Button();
					homeButton.FlatStyle = FlatStyle.Popup;
					homeButton.Text = "Home";	// TODO: change to an image.
					hbox.StretchControl = directory;
					hbox.Controls.Add(directory);
					hbox.Controls.Add(upButton);
					hbox.Controls.Add(newDirButton);
					hbox.Controls.Add(homeButton);

					// The second line is "listBox", already created above.

					// Add the third line (file name entry fields).
					nameLabel = new Label();
					nameLabel.Text = "File name:";	// TODO: translate.
					name = new TextBox();
					okButton = new Button();
					okButton.Text = fileDialogParent.OkButtonName;
					grid.SetControl(0, 0, nameLabel);
					grid.SetControl(1, 0, name);
					grid.SetControl(2, 0, okButton);

					// Add the fourth line (file type entry fields).
					typeLabel = new Label();
					typeLabel.Text = "Files of type:";	// TODO: translate.
					type = new ComboBox();
					cancelButton = new Button();
					cancelButton.Text = S._("SWF_MessageBox_Cancel", "Cancel");
					grid.SetControl(0, 1, typeLabel);
					grid.SetControl(1, 1, type);
					grid.SetControl(2, 1, cancelButton);

					// Add the top-level vbox to the dialog and set the size.
					Controls.Add(vbox);
					Size size = vbox.RecommendedSize;
					if(size.Width < 500)
					{
						size.Width = 500;
					}
					if(size.Height < 300)
					{
						size.Height = 300;
					}
					ClientSize = size;
					MinimumSize = size;

					// Hook up interesting events.
					upButton.Click += new EventHandler(UpOneLevel);
					homeButton.Click += new EventHandler(Home);
					directory.SelectedIndexChanged +=
						new EventHandler(DirectorySelectionChanged);

					// Scan the initial directory.
					String dir = fileDialogParent.InitialDirectory;
					if(dir == null || dir.Length == 0)
					{
						dir = Directory.GetCurrentDirectory();
					}
					ChangeDirectory(dir);
				}

		// Dispose of this dialog.
		public void DisposeDialog()
				{
					Dispose(true);
				}

		// Change to a new directory.
		public void ChangeDirectory(String dir)
				{
					// Record the current directory.
					currentDirectory = dir;

					// Scan the current directory and display the entries.
					FilesystemEntry[] entries;
					entries = ScanDirectory
						(dir, "*.*", fileDialogParent.DereferenceLinks);
					listBox.Entries = entries;

					// Update the directory combo box with the list.
					directory.BeginUpdate();
					ComboBox.ObjectCollection items = directory.Items;
					items.Clear();
					String d = currentDirectory;
					while(d != Path.GetPathRoot(d))
					{
						items.Add(Path.GetFileName(d));
						d = Path.GetDirectoryName(d);
					}
					items.Add(d);
					directory.Text = currentDirectory;
					directory.EndUpdate();
				}

		// Reload the current view to match the current settings.
		public void Reload()
				{
					ChangeDirectory(currentDirectory);
				}

		// Go up one level.
		public void UpOneLevel()
				{
					if(currentDirectory != Path.GetPathRoot(currentDirectory))
					{
						ChangeDirectory
							(Path.GetDirectoryName(currentDirectory));
					}
				}
		private void UpOneLevel(Object sender, EventArgs e)
				{
					UpOneLevel();
				}

		// Go to the home directory.
		public void Home()
				{
					String home = Environment.GetEnvironmentVariable("HOME");
					if(home != null && home.Length > 0)
					{
						ChangeDirectory(home);
					}
				}
		private void Home(Object sender, EventArgs e)
				{
					Home();
				}

		// Process a dialog key.
		protected override bool ProcessDialogKey(Keys keyData)
				{
					if(keyData == Keys.F4)
					{
						directory.Focus();
						directory.DroppedDown = true;
						return true;
					}
					else if(keyData == Keys.F5)
					{
						Reload();
						return true;
					}
					else if(keyData == (Keys.Alt | Keys.Home))
					{
						Home();
						return true;
					}
					return base.ProcessDialogKey(keyData);
				}

		// Process a change of directory selection in a combo box.
		private void DirectorySelectionChanged(Object sender, EventArgs e)
				{
					int index = directory.SelectedIndex;
					String d = currentDirectory;
					while(d != Path.GetPathRoot(d))
					{
						if(index <= 0)
						{
							break;
						}
						--index;
						d = Path.GetDirectoryName(d);
					}
					if(d != currentDirectory)
					{
						ChangeDirectory(d);
					}
				}

	}; // class FileDialogForm

}; // class FileDialog

}; // namespace System.Windows.Forms
