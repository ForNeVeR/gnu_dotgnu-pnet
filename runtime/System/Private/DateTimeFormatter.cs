/*
 * DateTimeFormatter.cs - Implementation of the "System.Private.DateTimeFormatter" class.
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
using System;
using System.Text;
using System.Collections;
using System.Globalization;
internal class DateTimeFormatter
{
	private abstract class FormatTemplate
	{
		internal abstract String GetString(DateTime d,
					DateTimeFormatInfo info);
	}
	private class DayFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					return d.Day.ToString();
				}
				break;
				case 2:
				{
					if(d.Day<10) return "0"+d.Day.ToString();
					else return d.Day.ToString();
				}
				break;
				case 3:
				{
					return info.GetAbbreviatedDayName((DayOfWeek)d.DayOfWeek);
				}
				break;
				case 4:
				{
					return info.GetDayName((DayOfWeek)(d.DayOfWeek));
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class MonthFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					return d.Month.ToString();
				}
				break;
				case 2:
				{
					 if(d.Month<10) return "0"+d.Month.ToString();
					else return d.Month.ToString();
				}
				break;
				case 3:
				{
					return info.GetAbbreviatedMonthName(d.Month);
				}
				break;
				case 4:
				{
					return info.GetMonthName(d.Month);
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class YearFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		
		{
			switch(Count)
			{
				case 1:
				{
					int century;
					for(int i = 0; 0 < d.Year - i; i=i+1000)
					{
						century=i;
					}
					// find the current century
					// return current year - century
					return (d.Year-century).ToString();
				}
				break;
				case 2:
				{
					int century;
					for(int i = 0; 0 < d.Year - i; i=i+1000)
					{
						century=i;
					}
					if((d.Year-century)<10) return "0"+(d.Year-century).ToString();
					return (d.Year-century).ToString();
				}
				break;
				case 4:
				{
					if(d.Year<10) return "00"+d.Year.ToString();
					return (d.Year.ToString()).Substring(d.Year.ToString().Length-4);
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class SecondFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					return (d.Second).ToString();
				}
				break;
				case 2:
				{
					if( d.Second<10) return "0"+d.Minute.ToString();
					return d.Second.ToString();
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class MinuteFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					return (d.Minute).ToString();
				}
				break;
				case 2:
				{
					if( d.Minute<10) return "0"+d.Minute.ToString();
					return d.Minute.ToString();
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class TwelveHourFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					return (d.Hour % 12).ToString();
				}
				break;
				case 2:
				{
					int hr =  (d.Hour % 12);
					if( hr<10) return "0"+hr.ToString();
					return hr.ToString();
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class TwentyFourHourFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					return (d.Hour).ToString();
				}
				break;
				case 2:
				{
					int hr =  (d.Hour);
					if( hr<10) return "0"+d.Hour.ToString();
					return d.Hour.ToString();
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}	
	private class AMPMFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					if( d.Hour < 12) return info.AMDesignator.Substring(0,1);
					return info.PMDesignator.Substring(0,1);

				}
				break;
				case 2:
				{
					if( d.Hour < 12) return info.AMDesignator;
					return info.PMDesignator;
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	[TODO]
	private class UTCFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
				}
				break;
				case 2:
				{
				}					
				break;
				case 3:
				{
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	[TODO]
	private class EraFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		
		{
			switch(Count)
			{
				case 1:
				{
					return info.GetEraName(d.Year);
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class DateSeparatorFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					return info.DateSeparator;
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class TimeSeparatorFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					return info.TimeSeparator;
				}
				break;
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class LiteralChar: FormatTemplate
	{
		internal String Text="";
		internal override String GetString(DateTime d,DateTimeFormatInfo info)
		{
			return Text;
		}
	}
	private class ExplicitString: FormatTemplate
	{
		internal String Text="";
		internal override String GetString(DateTime d,DateTimeFormatInfo info)
		{
			return Text;
		}
	}
	private static String StandardFormatSpecifier(String format)
	{
		Char c = format[0];
		switch(c)
		{		
			case 'd':
			{
				return "MM/dd/yyyy\n";
			}
			break;
			case 'D':
			{
				return "dddd, MMMM dd, yyyy\n";
			}
			break;
			case 'f':
			{
				return "dddd, MMMM dd, yyyy HH:mm\n";
			}
			break;
			case 'F':
			{
				return "dddd, MMMM dd, yyyy HH:mm:ss\n";
			}
			break;
			case 'g':
			{
				return "MM/dd/yyyy HH:mm\n";
			}
			break;
			case 'G':
			{
				return "MM/dd/yyyy HH:mm:ss\n";
			}
			break;
			case 'm':
			{
				return "MMMM dd\n";
			}
			break;
			case 'M':
			{
				return "MMMM dd\n";
			}
			break;
			case 't':
			{
				return "HH:mm\n";
			}
			break;
			case 'T':
			{
				return "HH:mm:ss\n";
			}
			break;
			case 'U':
			{
				return "dddd, MMMM dd, yyyy HH:mm:ss\n";
			}
			break;
			case 'y':
			{
				return "yyyy MMMM\n";
			}
			break;		
			case 'Y':
			{
				return "yyyy MMMM\n";
			}
			break;		
			default:
			{
				return "\n";
			}
			break;
		}
		return format;
	}
	public static String Format(DateTime date,
							String format, DateTimeFormatInfo info)
	{
		if(format.Length == 1) format=StandardFormatSpecifier(format);
		else 
		{	//manually append a newline string if not of length 1			
			StringBuilder tempformat = new StringBuilder(format);
			tempformat.Append("\n");
			format=tempformat.ToString();
		}
		StringBuilder builder=new StringBuilder(format.Length);
		Queue q=new Queue();
		
		if(info==null) info=DateTimeFormatInfo.InvariantInfo;
		FormatTemplate current=null;
		current=new ExplicitString(); // marker for the start
 
		bool literal=false;

		foreach(char c in format)
		{
			if(literal) 
			//if recieved / as last char just push char on queue
			{
				if(!(current is LiteralChar))
				{
					q.Enqueue(current);
					current=new LiteralChar();
				}
				(current as LiteralChar).Text+=c;
				literal=false;
			}
			else
			{
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
		}
	
		foreach(Object x in q)
		{
			builder.Append((x as FormatTemplate).GetString(date,info));
			/* traverse the list and add the stuff */
		
		}
		return builder.ToString();
	}
}








