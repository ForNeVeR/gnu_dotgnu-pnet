/*
 * ControlPaint.cs - Implementation of the
 *			"System.Windows.Forms.ControlPaint" class.
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

using System.Drawing;
using System.Drawing.Drawing2D;

public sealed class ControlPaint
{
	// This class cannot be instantiated.
	private ControlPaint() {}

	// Get the ControlDark color, taking contrast into account.
	public static Color ContrastControlDark
			{
				get
				{
					return SystemColors.ControlDark;
				}
			}

	// Convert HSB values into an RGB value.  Algorithm based on:
	// http://www.cs.rit.edu/~ncs/color/t_convert.html
	private static Color FromHSB(float hue, float saturation, float brightness)
			{
				float f, p, q, t, r, g, b;
				int temp;
				if(saturation == 0.0f)
				{
					temp = (int)(brightness * 255.0f);
					return Color.FromArgb(temp, temp, temp);
				}
				else
				{
					hue /= 60.0f;
					temp = (int)hue;
					f = hue - (float)temp;
					p = brightness * (1.0f - saturation);
					q = brightness * (1.0f - saturation * f);
					t = brightness * (1.0f - saturation * (1.0f - f));
					switch(temp)
					{
						case 0:
							r = brightness;
							g = t;
							b = p;
							break;
						case 1:
							r = q;
							g = brightness;
							b = p;
							break;
						case 2:
							r = p;
							g = brightness;
							b = t;
							break;
						case 3:
							r = p;
							g = q;
							b = brightness;
							break;
						case 4:
							r = t;
							g = p;
							b = brightness;
							break;
						default:
							r = brightness;
							g = p;
							b = q;
							break;
					}
					return Color.FromArgb
						((int)(r / 255.0f), (int)(g / 255.0f),
						 (int)(b / 255.0f));
				}
			}

	// Get the "dark" version of a color.
	public static Color Dark(Color baseColor)
			{
				return Dark(baseColor, 0.5f);
			}
	public static Color DarkDark(Color baseColor)
			{
				return Dark(baseColor, 1.0f);
			}
	public static Color Dark(Color baseColor, float percOfDarkDark)
			{
				if(baseColor.ToKnownColor() == KnownColor.Control)
				{
					if(percOfDarkDark <= 0.0f)
					{
						return SystemColors.ControlDark;
					}
					else if(percOfDarkDark >= 1.0f)
					{
						return SystemColors.ControlDarkDark;
					}
					else
					{
						Color dark = SystemColors.ControlDark;
						Color darkdark = SystemColors.ControlDarkDark;
						int redDiff = darkdark.R - dark.R;
						int greenDiff = darkdark.G - dark.G;
						int blueDiff = darkdark.B - dark.B;
						return Color.FromArgb
							(dark.R + (int)(redDiff * percOfDarkDark),
							 dark.G + (int)(greenDiff * percOfDarkDark),
							 dark.B + (int)(blueDiff * percOfDarkDark));
					}
				}
				float hue = baseColor.GetHue();
				float saturation = baseColor.GetSaturation();
				float brightness = baseColor.GetBrightness();
				brightness -= percOfDarkDark * 0.666f;
				if(brightness < 0.0f)
				{
					brightness = 0.0f;
				}
				return FromHSB(hue, saturation, brightness);
			}

	// Get the "light" version of a color.
	public static Color Light(Color baseColor)
			{
				return Light(baseColor, 0.5f);
			}
	public static Color LightLight(Color baseColor)
			{
				return Light(baseColor, 1.0f);
			}
	public static Color Light(Color baseColor, float percOfLightLight)
			{
				if(baseColor.ToKnownColor() == KnownColor.Control)
				{
					if(percOfLightLight <= 0.0f)
					{
						return SystemColors.ControlLight;
					}
					else if(percOfLightLight >= 1.0f)
					{
						return SystemColors.ControlLightLight;
					}
					else
					{
						Color light = SystemColors.ControlLight;
						Color lightlight = SystemColors.ControlLightLight;
						int redDiff = lightlight.R - light.R;
						int greenDiff = lightlight.G - light.G;
						int blueDiff = lightlight.B - light.B;
						return Color.FromArgb
							(light.R + (int)(redDiff * percOfLightLight),
							 light.G + (int)(greenDiff * percOfLightLight),
							 light.B + (int)(blueDiff * percOfLightLight));
					}
				}
				float hue = baseColor.GetHue();
				float saturation = baseColor.GetSaturation();
				float brightness = baseColor.GetBrightness();
				brightness += percOfLightLight * 0.666f;
				if(brightness > 1.0f)
				{
					brightness = 1.0f;
				}
				return FromHSB(hue, saturation, brightness);
			}

	// Draw a simple button border.
	public static void DrawBorder
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
						pen = new Pen(DarkDark(color), 1.0f);
						pen.EndCap = LineCap.Square;
						graphics.DrawLine(pen, bounds.X,
										  bounds.Y + bounds.Height - 1,
										  bounds.X, bounds.Y);
						graphics.DrawLine(pen, bounds.X + 1, bounds.Y,
										  bounds.X + bounds.Width - 1,
										  bounds.Y);
						pen.Color = LightLight(color);
						graphics.DrawLine(pen, bounds.X + 1,
										  bounds.Y + bounds.Height - 1,
										  bounds.X + bounds.Width - 1,
										  bounds.Y + bounds.Height - 1);
						graphics.DrawLine(pen, bounds.X + bounds.Width - 1,
										  bounds.Y + bounds.Height - 2,
										  bounds.X + bounds.Width - 1,
										  bounds.Y + 1);
						pen.Color = Light(color);
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
						pen = new Pen(LightLight(color), 1.0f);
						pen.EndCap = LineCap.Square;
						graphics.DrawLine(pen, bounds.X,
										  bounds.Y + bounds.Height - 2,
										  bounds.X, bounds.Y);
						graphics.DrawLine(pen, bounds.X + 1, bounds.Y,
										  bounds.X + bounds.Width - 2,
										  bounds.Y);
						pen.Color = DarkDark(color);
						graphics.DrawLine(pen, bounds.X,
										  bounds.Y + bounds.Height - 1,
										  bounds.X + bounds.Width - 1,
										  bounds.Y + bounds.Height - 1);
						graphics.DrawLine(pen, bounds.X + bounds.Width - 1,
										  bounds.Y + bounds.Height - 2,
										  bounds.X + bounds.Width - 1,
										  bounds.Y);
						pen.Color = Light(color);
						graphics.DrawLine(pen, bounds.X + 1,
										  bounds.Y + bounds.Height - 3,
										  bounds.X + 1, bounds.Y + 1);
						graphics.DrawLine(pen, bounds.X + 1, bounds.Y + 1,
										  bounds.X + bounds.Width - 3,
										  bounds.Y + 1);
						pen.Color = Dark(color);
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
	[TODO]
	public static void DrawBorder
				(Graphics graphics, Rectangle bounds, Color leftColor,
			     int leftWidth, ButtonBorderStyle leftStyle, Color topColor,
				 int topWidth, ButtonBorderStyle topStyle, Color rightColor,
				 int rightWidth, ButtonBorderStyle rightStyle,
				 Color bottomColor, int bottomWidth,
				 ButtonBorderStyle bottomStyle)
			{
				// TODO
			}

	// Draw a 3D border within a rectangle.
	public static void DrawBorder3D(Graphics graphics, Rectangle rectangle)
			{
				DrawBorder3D(graphics, rectangle.X, rectangle.Y,
							 rectangle.Width, rectangle.Height,
							 Border3DStyle.Etched,
							 Border3DSide.Left | Border3DSide.Top |
							 Border3DSide.Right | Border3DSide.Bottom);
			}
	public static void DrawBorder3D(Graphics graphics, Rectangle rectangle,
									Border3DStyle style)
			{
				DrawBorder3D(graphics, rectangle.X, rectangle.Y,
							 rectangle.Width, rectangle.Height, style,
							 Border3DSide.Left | Border3DSide.Top |
							 Border3DSide.Right | Border3DSide.Bottom);
			}
	public static void DrawBorder3D(Graphics graphics, Rectangle rectangle,
									Border3DStyle style, Border3DSide sides)
			{
				DrawBorder3D(graphics, rectangle.X, rectangle.Y,
							 rectangle.Width, rectangle.Height, style, sides);
			}
	public static void DrawBorder3D(Graphics graphics, int x, int y,
									int width, int height)
			{
				DrawBorder3D(graphics, x, y, width, height,
							 Border3DStyle.Etched,
							 Border3DSide.Left | Border3DSide.Top |
							 Border3DSide.Right | Border3DSide.Bottom);
			}
	public static void DrawBorder3D(Graphics graphics, int x, int y,
									int width, int height,
									Border3DStyle style)
			{
				DrawBorder3D(graphics, x, y, width, height, style,
							 Border3DSide.Left | Border3DSide.Top |
							 Border3DSide.Right | Border3DSide.Bottom);
			}
	public static void DrawBorder3D(Graphics graphics, int x, int y,
									int width, int height,
									Border3DStyle style,
									Border3DSide sides)
			{
				DrawBorder3D(graphics, x, y, width, height, 
							 SystemColors.InactiveBorder,
							 SystemColors.Control,
							 style, sides);

			}
	internal static void DrawBorder3D(Graphics graphics, int x, int y,
									  int width, int height,
									  Color foreColor,
									  Color backColor,
									  Border3DStyle style,
									  Border3DSide sides)
			{
				Color light, lightlight, dark, darkdark;
				Pen pen;

				// Draw the border around the edges of the button.
				if(style == Border3DStyle.Etched)
				{
					lightlight = DarkDark(backColor);
					darkdark = LightLight(backColor);
					light = Dark(backColor);
					dark = Light(backColor);
				}
				else
				{
					lightlight = LightLight(backColor);
					darkdark = DarkDark(backColor);
					light = Light(backColor);
					dark = Dark(backColor);
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
				if(width >= 4 && height >= 4)
				{
					pen = new Pen(lightlight, 1.0f);
					pen.EndCap = LineCap.Square;
					if((sides & Border3DSide.Left )!=0)
					{
						graphics.DrawLine(pen, x, y + height - 2, x, y);
						pen.Color = light;
						graphics.DrawLine(pen, x + 1, y + height - 3,
									  x + 1, y + 1);
					}
					if((sides & Border3DSide.Right )!=0)
					{
						pen.Color = darkdark;
						graphics.DrawLine(pen, x + width - 1, y,
									  x + width - 1, y + height - 1);
						pen.Color = dark;
						graphics.DrawLine(pen, x + width - 2, y + 1,
									  x + width - 2, y + height - 2);
					}
					if((sides & Border3DSide.Top)!=0)
					{
						pen.Color = lightlight;
						graphics.DrawLine(pen, x + 1, y, x + width - 2, y);
						pen.Color = light;
						graphics.DrawLine(pen, x + 2, y + 1,
									  x + width - 3, y + 1);
					}
					if((sides & Border3DSide.Bottom)!=0)
					{
						pen.Color = darkdark;
						graphics.DrawLine(pen, x + width - 2, y + height - 1,
									  x, y + height - 1);
						pen.Color = dark;
						graphics.DrawLine(pen, x + width - 3, y + height - 2,
									  x + 1, y + height - 2);
					}
					pen.Dispose();
					x += 2;
					y += 2;
					width -= 4;
					height -= 4;
				}
			}

	// Draw a button control.
	public static void DrawButton(Graphics graphics, Rectangle rectangle,
								  ButtonState state)
			{
				DrawButton(graphics, rectangle.X, rectangle.Y,
						   rectangle.Width, rectangle.Height, state,
						   SystemColors.ControlText,
						   SystemColors.Control, false);
			}
	public static void DrawButton(Graphics graphics, int x, int y,
								  int width, int height, ButtonState state)
			{
				DrawButton(graphics, x, y, width, height, state,
						   SystemColors.ControlText,
						   SystemColors.Control, false);
			}
	internal static void DrawButton
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
							lightlight = Dark(backColor);
							darkdark = lightlight;
							light = backColor;
							dark = backColor;
						}
						else
						{
							lightlight = DarkDark(backColor);
							darkdark = LightLight(backColor);
							light = Dark(backColor);
							dark = Light(backColor);
						}
					}
					else
					{
						lightlight = LightLight(backColor);
						darkdark = DarkDark(backColor);
						light = Light(backColor);
						dark = Dark(backColor);
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
	public static void DrawCaptionButton
				(Graphics graphics, Rectangle rectangle,
				 CaptionButton button, ButtonState state)
			{
				DrawCaptionButton(graphics, rectangle.X, rectangle.Y,
						   		  rectangle.Width, rectangle.Height,
								  button, state);
			}
	[TODO]
	public static void DrawCaptionButton
				(Graphics graphics, int x, int y, int width, int height,
				 CaptionButton button, ButtonState state)
			{
				// TODO
			}

	internal static void DrawProgressBar(Graphics graphics, int x, int y,
										int width, int height, 
										int steps, int step,
										int value, bool enabled)
			{
				// TODO : draw disabled mode progressbar
				// TODO : handle large no of blocks ie merge cases
				int blockWidth, blockHeight, xSpacing, ySpacing;
				DrawBorder3D(graphics,x,y,width,height);
				width-=4;
				height-=4;
				x+=2;
				y+=2;

				xSpacing=2;
				ySpacing=2;
				width=width-((steps-1)*xSpacing);
				blockWidth=width/steps;
				blockHeight=height-ySpacing-1;
						
				x+=2*xSpacing;

				for(int i=0;i<steps;i++)
				{
					if((i*step) < value)
					{
						ControlPaint.DrawBlock(graphics, x, y+ySpacing, 
											blockWidth,
											blockHeight,
											SystemColors.Highlight);
					}
					x+=blockWidth+xSpacing;
				}
			}

	internal static void DrawBlock(Graphics graphics, int x, int y,
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
	public static void DrawCheckBox(Graphics graphics, Rectangle rectangle,
								    ButtonState state)
			{
				DrawCheckBox(graphics, rectangle.X, rectangle.Y,
						     rectangle.Width, rectangle.Height, state);
			}
	[TODO]
	public static void DrawCheckBox(Graphics graphics, int x, int y,
								    int width, int height, ButtonState state)
			{
				// TODO
			}

	// Draw a combo box's drop down button control.
	public static void DrawComboButton(Graphics graphics, Rectangle rectangle,
								       ButtonState state)
			{
				DrawComboButton(graphics, rectangle.X, rectangle.Y,
						        rectangle.Width, rectangle.Height, state);
			}
	[TODO]
	public static void DrawComboButton(Graphics graphics, int x, int y,
								       int width, int height,
									   ButtonState state)
			{
				// TODO
			}

	// Draw a container grab handle.
	[TODO]
	public static void DrawContainerGrabHandle
				(Graphics graphics, Rectangle rectangle)
			{
				// TODO
			}

	// Draw a focus rectangle.
	public static void DrawFocusRectangle
				(Graphics graphics, Rectangle rectangle)
			{
				DrawFocusRectangle(graphics, rectangle,
								   SystemColors.ControlText,
								   SystemColors.Control);
			}
	public static void DrawFocusRectangle
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
	[TODO]
	public static void DrawGrabHandle
				(Graphics graphics, Rectangle rectangle,
				 bool primary, bool enabled)
			{
				// TODO
			}

	// Draw a grid of dots.
	[TODO]
	public static void DrawGrid(Graphics graphics, Rectangle area,
								Size pixelsBetweenDots, Color backColor)
			{
				// TODO
			}

	// Draw an image in its disabled state.
	[TODO]
	public static void DrawImageDisabled
				(Graphics graphics, Image image,
				 int x, int y, Color background)
			{
				// TODO
			}

	// Draw a locked selection frame.
	[TODO]
	public static void DrawLockedFrame
				(Graphics graphics, Rectangle rectangle, bool primary)
			{
				// TODO
			}

	// Draw a menu glyph.
	public static void DrawMenuGlyph
				(Graphics graphics, Rectangle rectangle, MenuGlyph glyph)
			{
				DrawMenuGlyph(graphics, rectangle.X, rectangle.Y,
						      rectangle.Width, rectangle.Height, glyph);
			}
	[TODO]
	public static void DrawMenuGlyph
				(Graphics graphics, int x, int y, int width,
				 int height, MenuGlyph glyph)
			{
				// TODO
			}

	// Draw a three-state check box control.
	public static void DrawMixedCheckBox
				(Graphics graphics, Rectangle rectangle, ButtonState state)
			{
				DrawMixedCheckBox(graphics, rectangle.X, rectangle.Y,
						     	  rectangle.Width, rectangle.Height, state);
			}
	[TODO]
	public static void DrawMixedCheckBox
				(Graphics graphics, int x, int y, int width,
				 int height, ButtonState state)
			{
				// TODO
			}

	// Draw a radio button control.
	public static void DrawRadioButton
				(Graphics graphics, Rectangle rectangle, ButtonState state)
			{
				DrawRadioButton(graphics, rectangle.X, rectangle.Y,
						     	rectangle.Width, rectangle.Height, state);
			}
	[TODO]
	public static void DrawRadioButton
				(Graphics graphics, int x, int y, int width,
				 int height, ButtonState state)
			{
				// TODO
			}

	// Draw a reversible frame.
	[TODO]
	public static void DrawReversibleFrame
				(Rectangle rectangle, Color backColor, FrameStyle style)
			{
				// TODO
			}

	// Draw a reversible line.
	[TODO]
	public static void DrawReversibleLine
				(Point start, Point end, Color backColor)
			{
				// TODO
			}

	// Draw a scroll button control.
	public static void DrawScrollButton
				(Graphics graphics, Rectangle rectangle,
				 ScrollButton button, ButtonState state)
			{
				DrawScrollButton(graphics, rectangle.X, rectangle.Y,
						     	 rectangle.Width, rectangle.Height,
								 button, state);
			}
	[TODO]
	public static void DrawScrollButton
				(Graphics graphics, int x, int y, int width, int height,
				 ScrollButton button, ButtonState state)
			{
				// TODO
			}

	// Draw a selection frame.
	[TODO]
	public static void DrawSelectionFrame
				(Graphics graphics, bool active, Rectangle outsideRect,
				 Rectangle insideRect, Color backColor)
			{
				// TODO
			}

	// Draw a size grip.
	public static void DrawSizeGrip
				(Graphics graphics, Color backColor, Rectangle rectangle)
			{
				DrawSizeGrip(graphics, backColor, rectangle.X, rectangle.Y,
						   	 rectangle.Width, rectangle.Height);
			}
	[TODO]
	public static void DrawSizeGrip
				(Graphics graphics, Color backColor,
				 int x, int y, int width, int height)
			{
				// TODO
			}

	// Draw a disabled string.
	public static void DrawStringDisabled
				(Graphics graphics, String s, Font font,
			     Color color, RectangleF layoutRectangle,
				 StringFormat format)
			{
				SolidBrush brush = new SolidBrush(Light(color));
				layoutRectangle.Offset(1.0f, 1.0f);
				graphics.DrawString(s, font, brush, layoutRectangle, format);
				brush.Color = Dark(color);
				layoutRectangle.Offset(-1.0f, -1.0f);
				graphics.DrawString(s, font, brush, layoutRectangle, format);
				brush.Dispose();
			}

	// Draw a filled reversible rectangle.
	[TODO]
	public static void FillReversibleRectangle
				(Rectangle rectangle, Color backColor)
			{
				// TODO
			}

}; // class ControlPaint

}; // namespace System.Windows.Forms
