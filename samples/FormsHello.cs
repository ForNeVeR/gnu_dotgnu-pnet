/*
 * FormsHello.cs - Sample program for Forms.
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

public class FormsHello : Form
{
	
	private FormsHello()
	{
		SetStyle(ControlStyles.ResizeRedraw, true);
	}

	private static void HandlePaint(Object sender, PaintEventArgs e)
	{
		Graphics graphics = e.Graphics;
		Form form = (sender as Form);

		Pen pen = new Pen(Color.Black, 1.0f);
		graphics.DrawLine(pen, 0, 0, form.Width, form.Height);
		pen.Dispose();

		pen = new Pen(Color.Red, 2.0f);
		graphics.DrawRectangle(pen, 10, 10, form.Width - 20, form.Height - 20);
		pen.Dispose();

		Brush brush = new SolidBrush(Color.Yellow);
		graphics.FillPie(brush, 20, 20, 60, 60, 30.0f, 70.0f);
		brush.Dispose();

		Font font = new Font("Arial", 12);
		brush = new SolidBrush(Color.Blue);
		graphics.DrawString("Hello", font, brush, 30, 100);
		brush.Dispose();
		font.Dispose();

		brush = new HatchBrush
			(HatchStyle.BackwardDiagonal, Color.Black, Color.White);
		graphics.FillEllipse(brush, 200, 40, 100, 100);
		brush.Dispose();
	}

	public static void Main(String[] args)
	{
		Form form = new FormsHello();
		form.Width = 400;
		form.Height = 250;
		form.Text = "Forms Hello";
		form.Paint += new PaintEventHandler(HandlePaint);
		Application.Run(form);
	}
}
