/*
 * DateTimeFormatter.cs - Implementation of the 
 *			"System.Private.DateTimeFormat.DateTimeFormatter" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * Contributions from Michael Moore <mtmoore@uga.edu>
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

namespace System.Private.DateTimeFormat
{

	using System;
	using System.Collections;
	using System.Globalization;
	using System.Text;
	internal class DateTimeFormatter
	{ 
		public static String Format(DateTime date,
							String format, DateTimeFormatInfo info)
		{
			if(format.Length==1) 
				format=StandardFormatSpecifier(format);
			
			StringBuilder builder=new StringBuilder(format.Length);
			Queue q=new Queue();
			
			if(info==null) 
				info=DateTimeFormatInfo.InvariantInfo;

			FormatTemplate current=null;
			current=new ExplicitString(); // marker for the start
			bool literal=false;
	
			foreach(char c in format)
			{
				if(literal) 
				{
				
					if(!(current is ExplicitString))
					{
						q.Enqueue(current);
						current=new ExplicitString();
					}
					(current as ExplicitString).Text+=c;
					literal=false;
					continue;
				}
			
				switch(c)
				{
					case 'd':
					{
						if(!(current is DayFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new DayFormatter();
						}
						(current as DayFormatter).Count++;
						break;
					}
					case 'f':
					{
						if(!(current is FractionalSecondFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new FractionalSecondFormatter();
						}
						(current as FractionalSecondFormatter).Count++;
						break;
					}
					case 'g':
					{
						if(!(current is EraFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new EraFormatter();
						}
						(current as EraFormatter).Count++;
						break;
					}
					case 'h':
					{
						if(!(current is TwelveHourFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new TwelveHourFormatter();
						}
						(current as TwelveHourFormatter).Count++;
						break;
					}
					case 'H':
						{
						if(!(current is TwentyFourHourFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new TwentyFourHourFormatter();
						}
						(current as TwentyFourHourFormatter).Count++;
						break;
					}
					case 'm':
					{
						if(!(current is MinuteFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new MinuteFormatter();
						}
						(current as MinuteFormatter).Count++;
						break;
					}
					case 'M':
					{
						if(!(current is MonthFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new MonthFormatter();
						}
						(current as MonthFormatter).Count++;
						break;
					}
					case 's':
					{
						if(!(current is SecondFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new SecondFormatter();
						}
						(current as SecondFormatter).Count++;
						break;
					}
					case 't':
					{
						if(!(current is AMPMFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new AMPMFormatter();
						}
						(current as AMPMFormatter).Count++;
						break;
					}
					case 'y':
					{
						if(!(current is YearFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new YearFormatter();
						}
						(current as YearFormatter).Count++;
						break;
					}
					case 'z':
					{
						if(!(current is UTCFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new UTCFormatter();
						}
						(current as UTCFormatter).Count++;
						break;
					}
					case '/':
					{
						if(!(current is DateSeparatorFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new DateSeparatorFormatter();
						}
						(current as DateSeparatorFormatter).Count++;
						break;
					}
					case ':':
					{
						if(!(current is TimeSeparatorFormatter))
						{
							q.Enqueue(current); //save the last node
							current=new TimeSeparatorFormatter();
						}
						(current as TimeSeparatorFormatter).Count++;
						break;
					}
					case '%':
					{
						//% is just a place holder so a single char
						// can be a custom formatter
						break;
					}
					case '\\':
					{
						literal=true;
						break;
					}
					default:
					{
						if(!(current is ExplicitString))
						{
							q.Enqueue(current);
							current=new ExplicitString();
						}
						(current as ExplicitString).Text+=c;
						break;
					}
				}
			}
			
			q.Enqueue(current);
			
			foreach(Object x in q)
			{
				builder.Append((x as FormatTemplate).GetString(date,info));
				/* traverse the list and add the stuff */
			}
			return builder.ToString();
		}
	
		internal static String StandardFormatSpecifier(String format)
		{
			switch(format)
			{		
				case "d":
				{
					return "MM/dd/yyyy";
				}
				case "D":
				{
					return "dddd, MMMM dd, yyyy";
				}
				case "f":
				{
					return "dddd, MMMM dd, yyyy HH:mm";
				}
				case "F":
				{
					return "dddd, MMMM dd, yyyy HH:mm:ss";
				}
				case "g":
				{
					return "MM/dd/yyyy HH:mm";
				}
				case "G":
				{
					return "MM/dd/yyyy HH:mm:ss";
				}
				case "m":
				{
					return "MMMM dd";
				}
				case "M":
				{
					return "MMMM dd";
				}
				case "t":
				{
					return "HH:mm";
				}
				case "T":
				{
					return "HH:mm:ss";
				}
				case "U":
				{
					return "dddd, MMMM dd, yyyy HH:mm:ss";
				}
				case "y":
				{
					return "yyyy MMMM";
				}
				case "Y":
				{
					return "yyyy MMMM";
				}
				default:
				{
					return format;
				}
			}
			return format;
		}
	}
}
	
	
