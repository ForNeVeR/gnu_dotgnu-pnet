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
			/* LAMESPEC: ECMA says dddd* is equivalent to ddddd, dddddd,...
					so 4 d's and greater are considered default case*/
			switch(Count)
			{
				case 1:
				{
					return d.Day.ToString();
				}
				case 2:
				{
					if(d.Day<10) return "0"+d.Day.ToString();
					else return d.Day.ToString();
				}
				case 3:
				{
					return info.GetAbbreviatedDayName((DayOfWeek)d.DayOfWeek);
				}
				default:
				{
					return info.GetDayName((DayOfWeek)(d.DayOfWeek));
				}
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
			/* LAMESPEC: ECMA says MMMM* is equivalent to MMMMM, MMMMMM,...
					so 4 M's and greater are considered default case*/
			switch(Count)
			{
				case 1:
				{
					return d.Month.ToString();
				}
				case 2:
				{
					 if(d.Month<10) return "0"+d.Month.ToString();
					else return d.Month.ToString();
				}
				case 3:
				{
					return info.GetAbbreviatedMonthName(d.Month);
				}
				default:
				{
					return info.GetMonthName(d.Month);
				}
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
				case 4:
				{
					if(d.Year<10) return "00"+d.Year.ToString();
					return (d.Year.ToString()).Substring(d.Year.ToString().Length-4);
				}
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class FractionalSecondFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{
		
			int fraction;
			unchecked
			{
				fraction=(int)(((d.Ticks) % (TimeSpan.TicksPerSecond)) / 10000);
			}
			
			switch(Count)
			{
				case 1:
				{
					return (fraction.ToString("d01")).Substring(0,1);
				}
				case 2:
				{
				
					return (fraction.ToString("d02")).Substring(0,2);
				}
				case 3:
				{
				
					return (fraction.ToString("d03")).Substring(0,3);
				}
				case 4:
				{
				
					return (fraction.ToString("d04")).Substring(0,4);
				}
				case 5:
				{
					
					return (fraction.ToString("d05")).Substring(0,5);
				}
				case 6:
				{
					
					return (fraction.ToString("d06")).Substring(0,6);
				}	
				case 7:
				{
				
					return (fraction.ToString("d07")).Substring(0,7);
				}

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
			/* LAMESPEC: ECMA says ss* is equivalent to sss, ssss,...
					so 2 s's and greater are considered default case*/
			switch(Count)
			{
				case 1:
				{
					return (d.Second).ToString();
				}
				default:
				{
					if( d.Second<10) return "0"+d.Minute.ToString();
					return d.Second.ToString();
				}
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
			/* LAMESPEC: ECMA says mm* is equivalent to mmm, mmmm,...
					so 2 m's and greater are considered default case */
			switch(Count)
			{
				case 1:
				{
					return (d.Minute).ToString();
				}
				default:
				{
					if( d.Minute<10) return "0"+d.Minute.ToString();
					return d.Minute.ToString();
				}
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
			/* LAMESPEC: ECMA says hh* is equivalent to hhh, hhhh,...
					so 2 h's and greater are considered default case*/
			switch(Count)
			{
				case 1:
				{
					return (d.Hour % 12).ToString();
				}
				default:
				{
					int hr =  (d.Hour % 12);
					return hr.ToString("d02");
				}
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
			/* LAMESPEC: ECMA says HH* is equivalent to HHH, HHHH,...
					so 2 H's and greater are considered default case*/
			switch(Count)
			{
				case 1:
				{
					return (d.Hour).ToString();
				}
				default:
				{
					int hr =  (d.Hour);
					if( hr<10) return "0"+d.Hour.ToString();
					return d.Hour.ToString();
				}
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
			/* LAMESPEC: ECMA says tt* is equivalent to ttt, tttt,...
					so 2 t's and greater are considered default case*/
			switch(Count)
			{
				case 1:
				{
					if( d.Hour < 12) return info.AMDesignator.Substring(0,1);
					return info.PMDesignator.Substring(0,1);

				}
				default:
				{
					if( d.Hour < 12) return info.AMDesignator;
					return info.PMDesignator;
				}
			}
			throw new FormatException("Invalid format string");
		}
	}
	private class UTCFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		{

			DateTime newDate=d.ToUniversalTime();
			string retval="";
			if( d > newDate) retval+="+";
			else retval+="-";
			TimeSpan difference=(newDate - d);
			int hours=(difference.Duration()).Hours;
			int minutes=(difference.Duration()).Minutes;
			/* LAMESPEC: ECMA says zzz* is equivalent to zzzz, zzzzz,...
					so 3 z's and greater are considered default case*/
			switch(Count)
			{
				case 1:
				{
					return retval+hours.ToString();
				}
				case 2:
				{
					return retval+hours.ToString("d02");
				}					
				default:
				{
					return retval+hours.ToString("d02")+":"+minutes.ToString("d02");
				}
			}
			throw new FormatException("Invalid format string");
		}
	}
	
	private class EraFormatter: FormatTemplate  //nested class
	{
		internal int Count=0; 
		internal override String GetString(DateTime d,
					DateTimeFormatInfo info)
		
		{
			/* LAMESPEC: ECMA says g* is equivalent to gg, ggg,...
					so 1 g and greater are considered default case*/
			switch(Count)
			{
				default:
				{
					return info.GetEraName(d.Year);
				}
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
			}
			throw new FormatException("Invalid format string");
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
	public static String Format(DateTime date,
							String format, DateTimeFormatInfo info)
	{
		if(format.Length == 1) format=StandardFormatSpecifier(format);
		
		StringBuilder builder=new StringBuilder(format.Length);
		Queue q=new Queue();
		
		if(info==null) info=DateTimeFormatInfo.InvariantInfo;
		FormatTemplate current=null;
		current=new ExplicitString(); // marker for the start
 
		bool literal=false;

		foreach(char c in format)
		{
			if(literal) {
			
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
}

