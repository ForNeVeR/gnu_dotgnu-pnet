/*
 * DefaultThemePainter.cs - Implementation of the
 *			"System.Windows.Forms.Themes.DefaultThemePainter" class.
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

namespace System.Windows.Forms.Themes
{

using System.Drawing;
using System.Drawing.Drawing2D;

// This class implements the default theme functionality for the
// System.Windows.Forms controls.  Other themes can inherit from
// this to provide new themes, while deferring most things to the
// base class to be handled.

internal class DefaultThemePainter : IThemePainter
{
	// Draw a simple button border.
	public virtual void DrawBorder
				(Graphics graphics, Rectangle bounds,
				 Color color, ButtonBorderStyle style)
			{
				Pen pen;
				switch(style)
				{
					case ButtonBorderStyle.Dotted:
					{
						pen = new Pen(color, 1.0f);
						pen.EndCap = LineCap.Square;
						pen.DashStyle = DashStyle.Dot;
						graphics.DrawRectangle(pen, bounds);
						pen.Dispose();
					}
					break;

					case ButtonBorderStyle.Dashed:
					{
						pen = new Pen(color, 1.0f);
						pen.EndCap = LineCap.Square;
						pen.DashStyle = DashStyle.Dash;
						graphics.DrawRectangle(pen, bounds);
						pen.Dispose();
					}
					break;

					case ButtonBorderStyle.Solid:
					{
						pen = new Pen(color, 1.0f);
						pen.EndCap = LineCap.Square;
						graphics.DrawRectangle(pen, bounds);
						pen.Dispose();
					}
					break;

					case ButtonBorderStyle.Inset:
					{
						pen = new Pen(ControlPaint.DarkDark(color), 1.0f);
						pen.EndCap = LineCap.Square;
						graphics.DrawLine(pen, bounds.X,
										  bounds.Y + bounds.Height - 1,
										  bounds.X, bounds.Y);
						graphics.DrawLine(pen, bounds.X + 1, bounds.Y,
										  bounds.X + bounds.Width - 1,
										  bounds.Y);
						pen.Color = ControlPaint.LightLight(color);
						graphics.DrawLine(pen, bounds.X + 1,
										  bounds.Y + bounds.Height - 1,
										  bounds.X + bounds.Width - 1,
										  bounds.Y + bounds.Height - 1);
						graphics.DrawLine(pen, bounds.X + bounds.Width - 1,
										  bounds.Y + bounds.Height - 2,
										  bounds.X + bounds.Width - 1,
										  bounds.Y + 1);
						pen.Color = ControlPaint.Light(color);
						graphics.DrawLine(pen, bounds.X + 1,
										  bounds.Y + bounds.Height - 2,
										  bounds.X + 1, bounds.Y + 1);
						graphics.DrawLine(pen, bounds.X + 2, bounds.Y + 1,
										  bounds.X + bounds.Width - 2,
										  bounds.Y + 1);
						if(color.ToKnownColor() == KnownColor.Control)
						{
							pen.Color = SystemColors.ControlLight;
							graphics.DrawLine(pen, bounds.X + 1,
											  bounds.Y + bounds.Height - 2,
											  bounds.X + bounds.Width - 2,
											  bounds.Y + bounds.Height - 2);
							graphics.DrawLine(pen, bounds.X + bounds.Width - 2,
											  bounds.Y + bounds.Height - 3,
											  bounds.X + bounds.Width - 2,
											  bounds.Y + 1);
						}
						pen.Dispose();
					}
					break;

					case ButtonBorderStyle.Outset:
					{
						pen = new Pen(ControlPaint.LightLight(color), 1.0f);
						pen.EndCap = LineCap.Square;
						graphics.DrawLine(pen, bounds.X,
										  bounds.Y + bounds.Height - 2,
										  bounds.X, bounds.Y);
						graphics.DrawLine(pen, bounds.X + 1, bounds.Y,
										  bounds.X + bounds.Width - 2,
										  bounds.Y);
						pen.Color = ControlPaint.DarkDark(color);
						graphics.DrawLine(pen, bounds.X,
										  bounds.Y + bounds.Height - 1,
										  bounds.X + bounds.Width - 1,
										  bounds.Y + bounds.Height - 1);
						graphics.DrawLine(pen, bounds.X + bounds.Width - 1,
										  bounds.Y + bounds.Height - 2,
										  bounds.X + bounds.Width - 1,
										  bounds.Y);
						pen.Color = ControlPaint.Light(color);
						graphics.DrawLine(pen, bounds.X + 1,
										  bounds.Y + bounds.Height - 3,
										  bounds.X + 1, bounds.Y + 1);
						graphics.DrawLine(pen, bounds.X + 1, bounds.Y + 1,
										  bounds.X + bounds.Width - 3,
										  bounds.Y + 1);
						pen.Color = ControlPaint.Dark(color);
						graphics.DrawLine(pen, bounds.X + 1,
										  bounds.Y + bounds.Height - 2,
										  bounds.X + bounds.Width - 2,
										  bounds.Y + bounds.Height - 2);
						graphics.DrawLine(pen, bounds.X + bounds.Width - 2,
										  bounds.Y + bounds.Height - 3,
										  bounds.X + bounds.Width - 2,
										  bounds.Y + 1);
						pen.Dispose();
					}
					break;
				}
			}
	public virtual void DrawBorder
				(Graphics graphics, Rectangle bounds, Color leftColor,
			     int leftWidth, ButtonBorderStyle leftStyle, Color topColor,
				 int topWidth, ButtonBorderStyle topStyle, Color rightColor,
				 int rightWidth, ButtonBorderStyle rightStyle,
				 Color bottomColor, int bottomWidth,
				 ButtonBorderStyle bottomStyle)
			{
				Pen pen;
				float percent, change;

				// Paint the left side of the border.
				if(leftWidth > 0)
				{
					switch(leftStyle)
					{
						case ButtonBorderStyle.Dotted:
						{
							pen = new Pen(leftColor, 1.0f);
							pen.EndCap = LineCap.Square;
							pen.DashStyle = DashStyle.Dot;
							while(leftWidth > 0)
							{
								--leftWidth;
								graphics.DrawLine
									(pen, bounds.X + leftWidth,
									 bounds.Y + leftWidth + 1,
									 bounds.X + leftWidth,
									 bounds.Y + bounds.Height - leftWidth - 2);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Dashed:
						{
							pen = new Pen(leftColor, 1.0f);
							pen.EndCap = LineCap.Square;
							pen.DashStyle = DashStyle.Dash;
							while(leftWidth > 0)
							{
								--leftWidth;
								graphics.DrawLine
									(pen, bounds.X + leftWidth,
									 bounds.Y + leftWidth + 1,
									 bounds.X + leftWidth,
									 bounds.Y + bounds.Height - leftWidth - 2);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Solid:
						{
							pen = new Pen(leftColor, 1.0f);
							pen.EndCap = LineCap.Square;
							while(leftWidth > 0)
							{
								--leftWidth;
								graphics.DrawLine
									(pen, bounds.X + leftWidth,
									 bounds.Y + leftWidth + 1,
									 bounds.X + leftWidth,
									 bounds.Y + bounds.Height - leftWidth - 2);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Inset:
						{
							change = (1.0f / leftWidth);
							percent = 1.0f;
							while(leftWidth > 0)
							{
								--leftWidth;
								pen = new Pen(ControlPaint.Dark
												(leftColor, percent), 1.0f);
								pen.EndCap = LineCap.Square;
								graphics.DrawLine
									(pen, bounds.X + leftWidth,
									 bounds.Y + leftWidth + 1,
									 bounds.X + leftWidth,
									 bounds.Y + bounds.Height - leftWidth - 2);
								pen.Dispose();
								percent -= change;
							}
						}
						break;

						case ButtonBorderStyle.Outset:
						{
							change = (1.0f / leftWidth);
							percent = change;
							while(leftWidth > 0)
							{
								--leftWidth;
								pen = new Pen(ControlPaint.Light
												(leftColor, percent), 1.0f);
								pen.EndCap = LineCap.Square;
								graphics.DrawLine
									(pen, bounds.X + leftWidth,
									 bounds.Y + leftWidth + 1,
									 bounds.X + leftWidth,
									 bounds.Y + bounds.Height - leftWidth - 2);
								pen.Dispose();
								percent += change;
							}
						}
						break;
					}
				}

				// Paint the top side of the border.
				if(topWidth > 0)
				{
					switch(topStyle)
					{
						case ButtonBorderStyle.Dotted:
						{
							pen = new Pen(topColor, 1.0f);
							pen.EndCap = LineCap.Square;
							pen.DashStyle = DashStyle.Dot;
							while(topWidth > 0)
							{
								--topWidth;
								graphics.DrawLine
									(pen, bounds.X + topWidth,
									 bounds.Y + topWidth,
									 bounds.X + bounds.Width - 1 - topWidth,
									 bounds.Y + topWidth);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Dashed:
						{
							pen = new Pen(topColor, 1.0f);
							pen.EndCap = LineCap.Square;
							pen.DashStyle = DashStyle.Dash;
							while(topWidth > 0)
							{
								--topWidth;
								graphics.DrawLine
									(pen, bounds.X + topWidth,
									 bounds.Y + topWidth,
									 bounds.X + bounds.Width - 1 - topWidth,
									 bounds.Y + topWidth);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Solid:
						{
							pen = new Pen(topColor, 1.0f);
							pen.EndCap = LineCap.Square;
							while(topWidth > 0)
							{
								--topWidth;
								graphics.DrawLine
									(pen, bounds.X + topWidth,
									 bounds.Y + topWidth,
									 bounds.X + bounds.Width - 1 - topWidth,
									 bounds.Y + topWidth);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Inset:
						{
							change = (1.0f / topWidth);
							percent = 1.0f;
							while(topWidth > 0)
							{
								--topWidth;
								pen = new Pen(ControlPaint.Dark
												(leftColor, percent), 1.0f);
								pen.EndCap = LineCap.Square;
								graphics.DrawLine
									(pen, bounds.X + topWidth,
									 bounds.Y + topWidth,
									 bounds.X + bounds.Width - 1 - topWidth,
									 bounds.Y + topWidth);
								pen.Dispose();
								percent -= change;
							}
						}
						break;

						case ButtonBorderStyle.Outset:
						{
							change = (1.0f / topWidth);
							percent = change;
							while(topWidth > 0)
							{
								--topWidth;
								pen = new Pen(ControlPaint.Light
												(leftColor, percent), 1.0f);
								pen.EndCap = LineCap.Square;
								graphics.DrawLine
									(pen, bounds.X + topWidth,
									 bounds.Y + topWidth,
									 bounds.X + bounds.Width - 1 - topWidth,
									 bounds.Y + topWidth);
								pen.Dispose();
								percent += change;
							}
						}
						break;
					}
				}

				// Paint the right side of the border.
				if(rightWidth > 0)
				{
					switch(rightStyle)
					{
						case ButtonBorderStyle.Dotted:
						{
							pen = new Pen(rightColor, 1.0f);
							pen.EndCap = LineCap.Square;
							pen.DashStyle = DashStyle.Dot;
							while(rightWidth > 0)
							{
								--rightWidth;
								graphics.DrawLine
									(pen,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + rightWidth,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + bounds.Height - 1 - rightWidth);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Dashed:
						{
							pen = new Pen(rightColor, 1.0f);
							pen.EndCap = LineCap.Square;
							pen.DashStyle = DashStyle.Dash;
							while(rightWidth > 0)
							{
								--rightWidth;
								graphics.DrawLine
									(pen,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + rightWidth,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + bounds.Height - 1 - rightWidth);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Solid:
						{
							pen = new Pen(rightColor, 1.0f);
							pen.EndCap = LineCap.Square;
							while(rightWidth > 0)
							{
								--rightWidth;
								graphics.DrawLine
									(pen,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + rightWidth,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + bounds.Height - 1 - rightWidth);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Inset:
						{
							change = (1.0f / rightWidth);
							percent = 1.0f;
							while(rightWidth > 0)
							{
								--rightWidth;
								pen = new Pen(ControlPaint.Dark
												(leftColor, percent), 1.0f);
								pen.EndCap = LineCap.Square;
								graphics.DrawLine
									(pen,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + rightWidth,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + bounds.Height - 1 - rightWidth);
								pen.Dispose();
								percent -= change;
							}
						}
						break;

						case ButtonBorderStyle.Outset:
						{
							change = (1.0f / rightWidth);
							percent = change;
							while(rightWidth > 0)
							{
								--rightWidth;
								pen = new Pen(ControlPaint.Light
												(leftColor, percent), 1.0f);
								pen.EndCap = LineCap.Square;
								graphics.DrawLine
									(pen,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + rightWidth,
									 bounds.X + bounds.Width - 1 - rightWidth,
									 bounds.Y + bounds.Height - 1 - rightWidth);
								pen.Dispose();
								percent += change;
							}
						}
						break;
					}
				}

				// Paint the bottom side of the border.
				if(bottomWidth > 0)
				{
					switch(bottomStyle)
					{
						case ButtonBorderStyle.Dotted:
						{
							pen = new Pen(bottomColor, 1.0f);
							pen.EndCap = LineCap.Square;
							pen.DashStyle = DashStyle.Dot;
							while(bottomWidth > 0)
							{
								--bottomWidth;
								graphics.DrawLine
									(pen,
									 bounds.X + bottomWidth,
									 bounds.Y + bounds.Height - 1 - bottomWidth,
									 bounds.X + bounds.Width - 2 - bottomWidth,
									 bounds.Y +
									 	bounds.Height - 1 - bottomWidth);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Dashed:
						{
							pen = new Pen(bottomColor, 1.0f);
							pen.EndCap = LineCap.Square;
							pen.DashStyle = DashStyle.Dash;
							while(bottomWidth > 0)
							{
								--bottomWidth;
								graphics.DrawLine
									(pen,
									 bounds.X + bottomWidth,
									 bounds.Y + bounds.Height - 1 - bottomWidth,
									 bounds.X + bounds.Width - 2 - bottomWidth,
									 bounds.Y +
									 	bounds.Height - 1 - bottomWidth);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Solid:
						{
							pen = new Pen(bottomColor, 1.0f);
							pen.EndCap = LineCap.Square;
							while(bottomWidth > 0)
							{
								--bottomWidth;
								graphics.DrawLine
									(pen,
									 bounds.X + bottomWidth,
									 bounds.Y + bounds.Height - 1 - bottomWidth,
									 bounds.X + bounds.Width - 2 - bottomWidth,
									 bounds.Y +
									 	bounds.Height - 1 - bottomWidth);
							}
							pen.Dispose();
						}
						break;

						case ButtonBorderStyle.Inset:
						{
							change = (1.0f / bottomWidth);
							percent = 1.0f;
							while(bottomWidth > 0)
							{
								--bottomWidth;
								pen = new Pen(ControlPaint.Dark
												(leftColor, percent), 1.0f);
								pen.EndCap = LineCap.Square;
								graphics.DrawLine
									(pen,
									 bounds.X + bottomWidth,
									 bounds.Y + bounds.Height - 1 - bottomWidth,
									 bounds.X + bounds.Width - 2 - bottomWidth,
									 bounds.Y +
									 	bounds.Height - 1 - bottomWidth);
								pen.Dispose();
								percent -= change;
							}
						}
						break;

						case ButtonBorderStyle.Outset:
						{
							change = (1.0f / bottomWidth);
							percent = change;
							while(bottomWidth > 0)
							{
								--bottomWidth;
								pen = new Pen(ControlPaint.Light
												(leftColor, percent), 1.0f);
								pen.EndCap = LineCap.Square;
								graphics.DrawLine
									(pen,
									 bounds.X + bottomWidth,
									 bounds.Y + bounds.Height - 1 - bottomWidth,
									 bounds.X + bounds.Width - 2 - bottomWidth,
									 bounds.Y +
									 	bounds.Height - 1 - bottomWidth);
								pen.Dispose();
								percent += change;
							}
						}
						break;
					}
				}
			}

	// Draw a 3D border within a rectangle.
	public virtual void DrawBorder3D(Graphics graphics, int x, int y,
									 int width, int height,
									 Color foreColor,
									 Color backColor,
									 Border3DStyle style,
									 Border3DSide sides)
			{
				Color dark, light, darkdark, lightlight;
				Color top1, top2 , left1, left2;
				Color bottom1, bottom2, right1, right2;
				Pen pen;
				bool doubleBorder=false;
				top2 = left2 = bottom2 = right2 = Color.Black;

				dark=ControlPaint.Dark(backColor);
				light=ControlPaint.Light(backColor);
				darkdark=ControlPaint.DarkDark(backColor);
				lightlight=ControlPaint.LightLight(backColor);
								
				switch(style)
				{
					case Border3DStyle.Flat:
						top1=dark;
						left1=dark;
						right1=dark;
						bottom1=dark;
						doubleBorder=false;
						break;
					case Border3DStyle.Raised:
						top1=light;
						top2=lightlight;
						left1=light;
						left2=lightlight;
						bottom1=dark;
						bottom2=darkdark;
						right1=dark;
						right2=darkdark;
						doubleBorder=true;
						break;
					case Border3DStyle.RaisedInner:
						top1=lightlight;
						left1=lightlight;
						bottom1=dark;
						right1=dark;
						doubleBorder=false;
						break;
					case Border3DStyle.RaisedOuter:
						top1=light;
						left1=light;
						bottom1=darkdark;
						right1=darkdark;
						doubleBorder=false;
						break;
					case Border3DStyle.Etched:
						top1=dark;
						top2=lightlight;
						left1=dark;
						left2=lightlight;
						bottom1=dark;
						bottom2=lightlight;
						right1=dark;
						right2=lightlight;
						doubleBorder=true;
						break;
					case Border3DStyle.Sunken:
						top1=dark;
						top2=darkdark;
						left1=dark;
						left2=darkdark;
						right1=light;
						right2=lightlight;
						bottom1=light;
						bottom2=lightlight;
						doubleBorder=true;
						break;
					case Border3DStyle.SunkenOuter:
						top1=dark;
						left1=dark;
						bottom1=lightlight;
						right1=lightlight;
						doubleBorder=false;
						break;
					case Border3DStyle.SunkenInner:
						top1=dark;
						left1=dark;
						bottom1=light;
						right1=light;
						doubleBorder=false;
						break;					
					case Border3DStyle.Bump:
						top1=light;
						top2=darkdark;
						left1=light;
						left2=darkdark;
						right1=light;
						right2=darkdark;
						bottom1=light;
						bottom2=darkdark;
						doubleBorder=true;
						break;
					default:
						top1=foreColor;
						bottom1=foreColor;
						left1=foreColor;
						right1=foreColor;
						doubleBorder=false;
						break;
				}

				if(width >= 2 && height >= 2)
				{
					pen = new Pen(foreColor, 1.0f);
					pen.EndCap = LineCap.Square;
					graphics.DrawRectangle(pen, x, y, width, height);
					pen.Dispose();
					++x;
					++y;
					width -= 2;
					height -= 2;
				}
				if(doubleBorder && (width >= 4 && height >= 4))
				{
					pen = new Pen(left1, 1.0f);
					pen.EndCap = LineCap.Square;
					if((sides & Border3DSide.Left )!=0)
					{
						graphics.DrawLine(pen, x, y + height - 2, x, y);
						pen.Color = left2;
						graphics.DrawLine(pen, x + 1, y + height - 3,
									  x + 1, y + 1);
					}
					if((sides & Border3DSide.Right )!=0)
					{
						pen.Color = right1;
						graphics.DrawLine(pen, x + width - 2, y + 1,
									  x + width - 2, y + height - 2);
						pen.Color = right2;
						graphics.DrawLine(pen, x + width - 1, y,
									  x + width - 1, y + height - 1);
					}
					if((sides & Border3DSide.Top)!=0)
					{
						pen.Color = top1;
						graphics.DrawLine(pen, x + 1, y, x + width - 2, y);
						pen.Color = top2;
						graphics.DrawLine(pen, x + 2, y + 1,
									  x + width - 3, y + 1);
					}
					if((sides & Border3DSide.Bottom)!=0)
					{
						pen.Color = bottom1;
						graphics.DrawLine(pen, x + width - 3, y + height - 2,
									  x + 1, y + height - 2);
						pen.Color = bottom2;
						graphics.DrawLine(pen, x + width - 2, y + height - 1,
									  x, y + height - 1);
					}
					pen.Dispose();
					x += 2;
					y += 2;
					width -= 4;
					height -= 4;
				}
				else if(!doubleBorder && width >= 2 && height >= 2)
				{
					pen = new Pen(left1, 1.0f);
					pen.EndCap = LineCap.Square;
					if((sides & Border3DSide.Left )!=0)
					{
						graphics.DrawLine(pen, x, y + height - 2, x, y);
					}
					if((sides & Border3DSide.Right )!=0)
					{
						pen.Color = right1;
						graphics.DrawLine(pen, x + width - 1, y,
									  x + width - 1, y + height - 1);
					}
					if((sides & Border3DSide.Top)!=0)
					{
						pen.Color = top1;
						graphics.DrawLine(pen, x + 1, y, x + width - 2, y);
					}
					if((sides & Border3DSide.Bottom)!=0)
					{
						pen.Color = bottom1;
						graphics.DrawLine(pen, x + width - 2, y + height - 1,
									  x, y + height - 1);
					}
					pen.Dispose();
					x+=1;
					y+=1;
					width -=2;
					height -=2;
				}
			}

	// Draw a button control.
	public virtual void DrawButton
				(Graphics graphics, int x, int y, int width, int height,
				 ButtonState state, Color foreColor, Color backColor,
				 bool isDefault)
			{
				Color light, lightlight, dark, darkdark;
				Pen pen;
				Brush brush;

				// Draw the border around the edges of the button.
				if((state & ButtonState.Flat) == 0)
				{
					if((state & ButtonState.Pushed) != 0)
					{
						if(isDefault)
						{
							lightlight = ControlPaint.Dark(backColor);
							darkdark = lightlight;
							light = backColor;
							dark = backColor;
						}
						else
						{
							lightlight = ControlPaint.DarkDark(backColor);
							darkdark = ControlPaint.LightLight(backColor);
							light = ControlPaint.Dark(backColor);
							dark = ControlPaint.Light(backColor);
						}
					}
					else
					{
						lightlight = ControlPaint.LightLight(backColor);
						darkdark = ControlPaint.DarkDark(backColor);
						light = ControlPaint.Light(backColor);
						dark = ControlPaint.Dark(backColor);
					}
					if(isDefault && width >= 2 && height >= 2)
					{
						pen = new Pen(foreColor, 1.0f);
						pen.EndCap = LineCap.Square;
						graphics.DrawRectangle(pen, x, y, width, height);
						pen.Dispose();
						++x;
						++y;
						width -= 2;
						height -= 2;
					}
					if(width >= 4 && height >= 4)
					{
						pen = new Pen(lightlight, 1.0f);
						pen.EndCap = LineCap.Square;
						graphics.DrawLine(pen, x, y + height - 2, x, y);
						graphics.DrawLine(pen, x + 1, y, x + width - 2, y);
						pen.Color = darkdark;
						graphics.DrawLine(pen, x + width - 1, y,
										  x + width - 1, y + height - 1);
						graphics.DrawLine(pen, x + width - 2, y + height - 1,
										  x, y + height - 1);
						pen.Color = light;
						graphics.DrawLine(pen, x + 1, y + height - 3,
										  x + 1, y + 1);
						graphics.DrawLine(pen, x + 2, y + 1,
										  x + width - 3, y + 1);
						pen.Color = dark;
						graphics.DrawLine(pen, x + width - 2, y + 1,
										  x + width - 2, y + height - 2);
						graphics.DrawLine(pen, x + width - 3, y + height - 2,
										  x + 1, y + height - 2);
						pen.Dispose();
						x += 2;
						y += 2;
						width -= 4;
						height -= 4;
					}
				}

				// Fill the button contents with the background color.
				if(width > 0 && height > 0)
				{
					brush = new SolidBrush(backColor);
					graphics.FillRectangle(brush, x, y, width, height);
					brush.Dispose();
				}
			}

	// Draw a caption button control.
	[TODO]
	public virtual void DrawCaptionButton
				(Graphics graphics, int x, int y, int width, int height,
				 CaptionButton button, ButtonState state)
			{
				// TODO
			}

	// Draw a progress bar control.
	[TODO]
	public virtual void DrawProgressBar(Graphics graphics, int x, int y,
										int width, int height, 
										int steps, int step,
										int value, bool enabled)
			{
				// TODO : draw disabled mode progressbar
				// TODO : handle large no of blocks ie merge cases
				int blockWidth, blockHeight, xSpacing, ySpacing;
				DrawBorder3D(graphics,x,y,width,height, 
							 SystemColors.InactiveBorder,
							 SystemColors.Control,
							 Border3DStyle.SunkenInner,
							 Border3DSide.All);
				width-=4;
				height-=4;
				x+=2;
				y+=2;

				xSpacing=2;
				ySpacing=2;
				width=width-((steps-1)*xSpacing);
				blockWidth=width/steps;
				blockHeight=height-ySpacing-1;
						
				x+=xSpacing;

				for(int i=0;i<steps;i++)
				{
					if((i*step) < value)
					{
						ControlPaint.DrawBlock(graphics, x, y+ySpacing, 
											blockWidth,
											blockHeight,
											enabled ? SystemColors.Highlight
													: SystemColors.ControlDark);
					}
					x+=blockWidth+xSpacing;
				}
			}

	// Draw a progress bar block.
	public virtual void DrawBlock(Graphics graphics, int x, int y,
								  int width, int height,
								  Color color)
			{
				Brush brush;
				Pen pen;
				brush=new SolidBrush(color);
				graphics.FillRectangle(brush, x, y, width, height);
				brush.Dispose();
			}

	// Draw a check box control.
	[TODO]
	public virtual void DrawCheckBox(Graphics graphics, int x, int y,
								     int width, int height, ButtonState state)
			{
				// TODO
			}

	// Draw a combo box's drop down button control.
	[TODO]
	public virtual void DrawComboButton(Graphics graphics, int x, int y,
								        int width, int height,
									    ButtonState state)
			{
				// TODO
			}

	// Draw a container grab handle.
	public virtual void DrawContainerGrabHandle
				(Graphics graphics, Rectangle rectangle)
			{
				Pen black = new Pen(Color.Black, 1.0f);
				black.EndCap = LineCap.Square;
				graphics.FillRectangle(Brushes.White, rectangle);
				graphics.DrawRectangle(black, rectangle);
				graphics.DrawLine
					(black,
					 rectangle.X + rectangle.Width / 2,
					 rectangle.Y + 2,
					 rectangle.X + rectangle.Width / 2,
					 rectangle.Y + rectangle.Height - 3);
				graphics.DrawLine
					(black,
					 rectangle.X + 2,
					 rectangle.Y + rectangle.Height / 2,
					 rectangle.X + rectangle.Width - 3,
					 rectangle.Y + rectangle.Height / 2);
				graphics.DrawLine
					(black,
					 rectangle.X + rectangle.Width / 2 - 1,
					 rectangle.Y + 3,
					 rectangle.X + rectangle.Width / 2 + 1,
					 rectangle.Y + 3);
				graphics.DrawLine
					(black,
					 rectangle.X + rectangle.Width / 2 - 1,
					 rectangle.Y + rectangle.Height - 4,
					 rectangle.X + rectangle.Width / 2 + 1,
					 rectangle.Y + rectangle.Height - 4);
				graphics.DrawLine
					(black,
					 rectangle.X + 3,
					 rectangle.Y + rectangle.Height / 2 - 1,
					 rectangle.X + 3,
					 rectangle.Y + rectangle.Height / 2 + 1);
				graphics.DrawLine
					(black,
					 rectangle.X + rectangle.Width - 4,
					 rectangle.Y + rectangle.Height / 2 - 1,
					 rectangle.X + rectangle.Width - 4,
					 rectangle.Y + rectangle.Height / 2 + 1);
				black.Dispose();
			}

	// Draw a focus rectangle.
	public virtual void DrawFocusRectangle
				(Graphics graphics, Rectangle rectangle,
				 Color foreColor, Color backColor)
			{
				Pen pen = new Pen(foreColor, 1.0f);
				pen.EndCap = LineCap.Square;
				pen.DashStyle = DashStyle.Dot;
				graphics.DrawRectangle(pen, rectangle);
				pen.Dispose();
			}

	// Draw a grab handle.
	public virtual void DrawGrabHandle
				(Graphics graphics, Rectangle rectangle,
				 bool primary, bool enabled)
			{
				Brush background;
				Pen border;
				if(primary)
				{
					if(enabled)
					{
						background = Brushes.White;
						border = Pens.Black;
					}
					else
					{
						background = SystemBrushes.Control;
						border = Pens.Black;
					}
				}
				else
				{
					if(enabled)
					{
						background = Brushes.Black;
						border = Pens.White;
					}
					else
					{
						background = SystemBrushes.Control;
						border = Pens.White;
					}
				}
				graphics.FillRectangle(background, rectangle);
				graphics.DrawRectangle(border, rectangle);
			}

	// Draw a grid of dots.
	[TODO]
	public virtual void DrawGrid(Graphics graphics, Rectangle area,
								 Size pixelsBetweenDots, Color backColor)
			{
				// TODO
			}

	// Draw an image in its disabled state.
	[TODO]
	public virtual void DrawImageDisabled
				(Graphics graphics, Image image,
				 int x, int y, Color background)
			{
				// TODO
			}

	// Draw a locked selection frame.
	public virtual void DrawLockedFrame
				(Graphics graphics, Rectangle rectangle, bool primary)
			{
				Pen outer;
				Pen inner;
				if(primary)
				{
					outer = Pens.White;
					inner = Pens.Black;
				}
				else
				{
					outer = Pens.Black;
					inner = Pens.White;
				}
				graphics.DrawRectangle(outer, rectangle);
				graphics.DrawRectangle
					(outer, rectangle.X + 1, rectangle.Y + 1,
					 rectangle.Width - 2, rectangle.Height - 2);
				graphics.DrawRectangle
					(inner, rectangle.X + 2, rectangle.Y + 2,
					 rectangle.Width - 4, rectangle.Height - 4);
			}

	// Draw a menu glyph.
	[TODO]
	public virtual void DrawMenuGlyph
				(Graphics graphics, int x, int y, int width,
				 int height, MenuGlyph glyph)
			{
				// TODO
			}

	// Draw a three-state check box control.
	[TODO]
	public virtual void DrawMixedCheckBox
				(Graphics graphics, int x, int y, int width,
				 int height, ButtonState state)
			{
				// TODO
			}

	// Draw a radio button control.
	[TODO]
	public virtual void DrawRadioButton
				(Graphics graphics, int x, int y, int width,
				 int height, ButtonState state)
			{
				// TODO
			}

	// Draw a reversible frame.
	[TODO]
	public virtual void DrawReversibleFrame
				(Rectangle rectangle, Color backColor, FrameStyle style)
			{
				// TODO
			}

	// Draw a reversible line.
	[TODO]
	public virtual void DrawReversibleLine
				(Point start, Point end, Color backColor)
			{
				// TODO
			}

	// Draw a scroll button control.
	[TODO]
	public virtual void DrawScrollButton
				(Graphics graphics, int x, int y, int width, int height,
				 ScrollButton button, ButtonState state)
			{
				// TODO
			}

	// Draw a selection frame.
	[TODO]
	public virtual void DrawSelectionFrame
				(Graphics graphics, bool active, Rectangle outsideRect,
				 Rectangle insideRect, Color backColor)
			{
				// TODO
			}

	// Draw a size grip.
	[TODO]
	public virtual void DrawSizeGrip
				(Graphics graphics, Color backColor,
				 int x, int y, int width, int height)
			{
				// TODO
			}

	// Draw a disabled string.
	public virtual void DrawStringDisabled
				(Graphics graphics, String s, Font font,
			     Color color, RectangleF layoutRectangle,
				 StringFormat format)
			{
				SolidBrush brush = new SolidBrush(ControlPaint.Light(color));
				layoutRectangle.Offset(1.0f, 1.0f);
				graphics.DrawString(s, font, brush, layoutRectangle, format);
				brush.Color = ControlPaint.Dark(color);
				layoutRectangle.Offset(-1.0f, -1.0f);
				graphics.DrawString(s, font, brush, layoutRectangle, format);
				brush.Dispose();
			}

	// Draw a filled reversible rectangle.
	[TODO]
	public virtual void FillReversibleRectangle
				(Rectangle rectangle, Color backColor)
			{
				// TODO
			}

}; // class DefaultThemePainter

}; // namespace System.Windows.Forms.Themes
