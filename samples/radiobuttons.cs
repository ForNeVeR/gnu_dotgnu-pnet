/*
 * radiobuttons.cs - Sample radio buttons program.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * This program is free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

public class RadioButtons : Form
{
	private RadioButton[] buttons = new RadioButton[6];
	
	private RadioButtons()
	{
		// Force the entire form to repaint when it is resized.
		SetStyle(ControlStyles.ResizeRedraw, true);

		buttons[0] = new RadioButton();
		buttons[0].Location = new Point(10,10);
		buttons[0].Size = new Size(300,20);
		buttons[0].Text = "Appearance Normal , No FlatStyle";
		Controls.Add(buttons[0]);

		buttons[1] = new RadioButton();
		buttons[1].Location = new Point(10,40);
		buttons[1].Size = new Size(300,20);
		buttons[1].Text = "Apearance Normal , FlatStyle.Popup";
		buttons[1].FlatStyle=FlatStyle.Popup;
		Controls.Add(buttons[1]);

		buttons[2] = new RadioButton();
		buttons[2].Location = new Point(10,70);
		buttons[2].Size = new Size(300,20);
		buttons[2].Text = "Appearance Normal , FlatStyle.Flat";
		buttons[2].FlatStyle=FlatStyle.Flat;
		Controls.Add(buttons[2]);

		buttons[3] = new RadioButton();
		buttons[3].Location = new Point(10,100);
		buttons[3].Size = new Size(300,20);
		buttons[3].Text = "Apearance Button , FlatStyle.Popup";
		buttons[3].Appearance=Appearance.Button;
		buttons[3].FlatStyle=FlatStyle.Popup;
		Controls.Add(buttons[3]);

		buttons[4] = new RadioButton();
		buttons[4].Location = new Point(10,130);
		buttons[4].Size = new Size(300,20);
		buttons[4].Text = "Appearance Button , FlatStyle.Flat";
		buttons[4].Appearance=Appearance.Button;
		buttons[4].FlatStyle=FlatStyle.Flat;
		Controls.Add(buttons[4]);

		buttons[5] = new RadioButton();
		buttons[5].Location = new Point(10,160);
		buttons[5].Size = new Size(300,20);
		buttons[5].Text = "Appearance Button , FlatStyle.Standard";
		buttons[5].Appearance=Appearance.Button;
		buttons[5].FlatStyle=FlatStyle.Standard;
		Controls.Add(buttons[5]);
	}

	public static void Main(String[] args)
	{
		RadioButtons form = new RadioButtons();
		form.Width = 400;
		form.Height = 200;
		form.Text = "RadioButtons example";
		Application.Run(form);
	}
}
