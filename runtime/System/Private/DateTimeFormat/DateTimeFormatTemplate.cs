/*
 * DateTimeFormatTemplate.cs - Implementation of the 
 *		"System.Private.DateTimeFormat.DateTimeFormatTemplate" class.
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
	using System.Globalization;

	internal abstract class FormatTemplate
	{
		internal abstract String GetString(DateTime d, DateTimeFormatInfo info);
		internal abstract int Parse(String s, int start, DateTimeFormatInfo info);
		internal abstract ParsedDateTime StoreTo(ParsedDateTime d);
	}
	
	internal class DayFormatter: FormatTemplate  
	{
		internal int Count=0; 
		internal int day = 0;
		internal bool digitDay = false;
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
		
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					digitDay = true;
					if( String.Compare(s, start+1, info.DateSeparator, 0, 1, 
						true) == 0)
					{ 
						day = Convert.ToInt32(s.Substring(start,1));
						return start+1;
					}	
					else 
					{
						day = Convert.ToInt32(s.Substring(start,2));
						return start+2;
					}
				}
				case 2:
				{
					digitDay = true;
					day = Convert.ToInt32(s.Substring(start,2));
					return start+2;
				}
				case 3:
				{
					for( DayOfWeek i = DayOfWeek.Sunday ; 
									i <= DayOfWeek.Saturday ; i++ )
					{
						try 
						{
							if( String.Compare(s, start, 
								info.GetAbbreviatedDayName(i), 0, 
								info.GetAbbreviatedDayName(i).Length, true) 
								== 0)
							{
								return (start+
									info.GetAbbreviatedDayName(i).Length);
							}
						}
						catch ( ArgumentOutOfRangeException ){}
					}
					throw new FormatException("Invalid Abbreviated Day Name");
				}
				default:
				{
					for( DayOfWeek i = DayOfWeek.Sunday ; 
									i <= DayOfWeek.Saturday ; i++ )
					{
						try 
						{
							if( String.Compare(s, start, info.GetDayName(i),
								0, info.GetDayName(i).Length, true) == 0)
							{
								return (start+info.GetDayName(i).Length);	
							}
						}
						catch ( ArgumentOutOfRangeException ){}
					}
					throw new FormatException("Invalid Abbreviated Day Name");
				}
			}
		}
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			if( digitDay )
			{
				d.Day = day;
			}
			// no need to store a day name
			return d;
		}
	}
		
	internal class MonthFormatter: FormatTemplate  
	{
		internal int Count=0; 
		internal int month = 0;
		internal String stringMonth=null;
		internal bool abbrevMonth = false;
		internal DateTimeFormatInfo dtfi;
	
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
	
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			dtfi = info;
			switch(Count)
			{
				case 1:
				{	
					if( (String.Compare(s, start+1, info.DateSeparator, 0, 1,
						true) == 0) || (String.Compare(s, start+1, " ", 0, 1, 
						true) == 0) )
					{
						month = Convert.ToInt32(s.Substring(start,1));
						return start+1;
					}	
					else 
					{
						month = Convert.ToInt32(s.Substring(start,2));
						return start+2;
					}
				}
				case 2:
				{
					month = Convert.ToInt32(s.Substring(start,2));
					return start+2;
				}
				case 3:
				{ 
					for( int i = 1; i < 14; i++ )
					{
						if( String.Compare(s, start, info.GetAbbreviatedMonthName(i), 								0, info.GetAbbreviatedMonthName(i).Length, true) == 0)
						{
							stringMonth = s.Substring(start, 
								info.GetAbbreviatedMonthName(i).Length);
							abbrevMonth = true;
							return (start+info.GetAbbreviatedMonthName(i).Length);			
						}
					}
					throw new FormatException("Invalid Abbreviated Month Name");
				}
				default:
				{
					for( int i = 1; i < 14; i++ )
					{
						if( String.Compare(s, start, info.GetMonthName(i), 0,
							 info.GetMonthName(i).Length, true) == 0)
						{
							stringMonth = s.Substring(start, 
								info.GetMonthName(i).Length);
							return (start+info.GetMonthName(i).Length);			
						}
					}
	
					throw new FormatException("Invalid Month Name");
				}
			}
		}
	
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			int monthIndex = 0;
			if(stringMonth == null)
				d.Month = month;
			else
			{
				if( abbrevMonth )
				{
					for(int i = 0; i < 13; i++)
					{
						if(dtfi.AbbreviatedMonthNames[i]==stringMonth)
							monthIndex = i;
					}
				}
				else
				{
					for(int i = 0; i < 13; i++)
					{
						if(dtfi.MonthNames[i]==stringMonth)
							monthIndex = i;
					}
				}
				d.Month = monthIndex+1;
			}
			return d;
		}
	}

	internal class YearFormatter: FormatTemplate  
	{
		internal int Count=0;
		internal int year; 
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					int century = 0;
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
					int century = 0;
					for(int i = 0; 0 < d.Year - i; i=i+1000)
					{
						century=i;
					}
					if((d.Year-century)<10) 
						return "0"+(d.Year-century).ToString();
						
					return (d.Year-century).ToString();
				}
				default:
				{
					if(d.Year<10) return "000"+d.Year.ToString();
						return (d.Year.ToString()).Substring(
							d.Year.ToString().Length-4);
				}
			}
			throw new FormatException("Invalid format string");
		}
	
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{ 
					if( (String.Compare(s, start+1, info.DateSeparator, 
						0, 1, true) == 0 ) || (String.Compare(s, start+1, "0", 
						0, 1, true) == 0))
					{ 
						year = Convert.ToInt32(s.Substring(start,1));
						return start+1;
					}	
					else 
					{
						year = Convert.ToInt32(s.Substring(start,2));
						return start+2;
					}
				}
				case 2:
				{ 
					year = Convert.ToInt32(s.Substring(start,2));
					return start+2;
				}
				default:
				{ 
					year = Convert.ToInt32(s.Substring(start,4));
					return start+4;
				}
			}
		}
	
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			d.Year = year;
			return d;
		}
	}
	
	internal class FractionalSecondFormatter: FormatTemplate  
	{
		internal int Count=0; 
		internal long tk;
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					tk = (long)(((Convert.ToInt32(s.Substring(start,1))) % 
							(TimeSpan.TicksPerSecond)) * 10000);
					return start+1;
				}
				case 2:
				{
					tk = (long)(((Convert.ToInt32(s.Substring(start,2))) % 
						(TimeSpan.TicksPerSecond)) * 10000);
					return start+2;
				}
				case 3:
				{
					tk = (long)(((Convert.ToInt32(s.Substring(start,3))) % 
						(TimeSpan.TicksPerSecond)) * 10000);
					return start+3;
				}
				case 4:
				{
					tk = (long)(((Convert.ToInt32(s.Substring(start,4))) % 
						(TimeSpan.TicksPerSecond)) * 10000);
					return start+4;
				}
				case 5:
				{
					tk = (long)(((Convert.ToInt32(s.Substring(start,5))) % 
						(TimeSpan.TicksPerSecond)) * 10000);
					return start+5;
				}
				case 6:
				{
					tk = (long)(((Convert.ToInt32(s.Substring(start,6))) % 
						(TimeSpan.TicksPerSecond)) * 10000);
					return start+6;
				}	
				case 7:
				{
					tk = (long)(((Convert.ToInt32(s.Substring(start,7))) % 
						(TimeSpan.TicksPerSecond)) * 10000);
					return start+7;
				}
				default:
				{
					throw new FormatException("bad fractional second number");
				} 
			}
		}
	
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			d.FractionalSecond = tk;
			return d;
		}
	}	
	
	internal class SecondFormatter: FormatTemplate  
	{
		internal int Count=0;
		internal int sec; 
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
					if( d.Second<10) return "0"+d.Second.ToString();
					return d.Second.ToString();
				}
			}
			throw new FormatException("Invalid format string");
		}
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{ 
					if( (String.Compare(s, start+1, info.TimeSeparator, 0,
						1, true) == 0) || (String.Compare(s, start+1, " ", 0, 1,
						true) == 0 ) )
					{
						sec = Convert.ToInt32(s.Substring(start,1));
						return start+1;
					}	
					else 
					{
						sec = Convert.ToInt32(s.Substring(start,2));
						return start+2;
					}
					return start;
				}
				default:
				{
					sec = Convert.ToInt32(s.Substring(start,2));
					return start+2;
				}
			}
		}
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			d.Second = sec;
			return d;
		}
	}
	
	internal class MinuteFormatter: FormatTemplate  
	{
		internal int Count=0; 
		internal int min;
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					if( String.Compare(s, start+1, info.TimeSeparator, 0, 1,
						 true) == 0)
					{
						min = Convert.ToInt32(s.Substring(start,1));
						return start+1;
					}	
					else 
					{
						min = Convert.ToInt32(s.Substring(start,2));
						return start+2;
					}
				}	
				default:
				{
					min = Convert.ToInt32(s.Substring(start,2));
					return start+2;
				}
			}
		}
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			d.Minute = min;
			return d;
		}
	}
	
	internal class TwelveHourFormatter: FormatTemplate  
	{
		internal int Count=0; 
		internal int twelvehr;
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					if( (String.Compare(s, start+1, info.TimeSeparator, 0, 1, 
						true) == 0) || ( String.Compare(s, start+1, " ", 0, 1, 
						true) == 0) )
					{
						twelvehr = Convert.ToInt32(s.Substring(start,1));
						return start+1;
					}	
					else 
					{
						twelvehr = Convert.ToInt32(s.Substring(start,2));
						return start+2;
					}
				}	
				default:
				{
					twelvehr = Convert.ToInt32(s.Substring(start,2));
					return start+2;
				}
			}
		}
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			d.TwelveHour = twelvehr;
			return d;
		}
	}
	
	internal class TwentyFourHourFormatter: FormatTemplate  
	{
		internal int Count=0;
		internal int twentyfourhr; 
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{
					if( String.Compare(s, start+1, info.TimeSeparator, 0, 1, 
						true) == 0)
					{
						twentyfourhr = Convert.ToInt32(s.Substring(start,1));
						return start+1;
					}	
					else 
					{
						twentyfourhr = Convert.ToInt32(s.Substring(start,2));
						return start+2;
					}
				}
				default:
				{
					twentyfourhr = Convert.ToInt32(s.Substring(start,2));
					return start+2;
				}
			}
		}

		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			d.TwentyFourHour = twentyfourhr;
			return d;
		}
	}		

	internal class AMPMFormatter: FormatTemplate  
	{
		internal int Count=0;
		internal String ampm; 
		internal bool onechar = false;
		internal DateTimeFormatInfo dtfi;
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
	
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			dtfi = info;
			switch(Count)
			{
				case 1:
				{
					onechar =true;
					if( String.Compare(s, start, info.AMDesignator, 0, 1, false)
						== 0)
					{
						ampm = info.AMDesignator.Substring(0,1);
					}
					else if( String.Compare(s, start, info.PMDesignator, 0, 1,
						false) == 0)
					{
						ampm = info.PMDesignator.Substring(0,1);
					}
					else
					{
						throw new FormatException("Invalid AMPM Designator");
					}
					return start+1;
				}
				default:
				{
					if( String.Compare(s, start, info.AMDesignator,
						 0, info.AMDesignator.Length, true) == 0)
					{
						ampm = info.AMDesignator;
						return start+info.PMDesignator.Length;
					}
					else if( String.Compare(s, start, info.PMDesignator, 
						0, info.PMDesignator.Length, true) == 0)
					{
						ampm = info.PMDesignator;
						return start+info.PMDesignator.Length;
					}
					throw new FormatException("Invalid AMPM Designator");
				}
			}
		}	
	
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			if( onechar ) 
			{
				if( String.Compare(ampm, 0, dtfi.AMDesignator, 0, 1, false) == 0)
				{
					d.AMPM = 0;
				}
				else
				{
					d.AMPM = 1;
				}
			}
			else
			{
				if( String.Compare(ampm, 0, dtfi.AMDesignator, 0, 
					dtfi.AMDesignator.Length, false) == 0)
				{
					d.AMPM = 0;
				}
				else
				{
					d.AMPM = 1;
				}
			}
			return d;
		}
	}

	internal class UTCFormatter: FormatTemplate  
	{
		internal int Count=0; 
		internal int hr=0;
		internal int min=0;
		internal bool negpos = false;
		internal bool hasMin = false;
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			switch(Count)
			{
				case 1:
				{	
					String sp = " ";
					String sign = s.Substring(start,1);
					if( sign == "+" ) negpos=true;
					if( (String.Compare(s, start+2, info.TimeSeparator, 0, 1,
						true) == 0) || (String.Compare(s, start+2, sp, 0, 1, 
						true) == 0)) 
					{
						hr= Convert.ToInt32(s.Substring(start+1,1));
						return start+2;	
					}
					else
					{
						hr = Convert.ToInt32(s.Substring(start+1, 2));
						return start+3;
					}
				}
				case 2:
				{
					String z = "0";
					String sign = s.Substring(start,1);
					if( sign == "+" ) negpos=true;
					if( String.Compare(s, start+1, z, 0, 1, true) == 0 )
					{
						hr = Convert.ToInt32(s.Substring(start+2, 1));
						return start+3;
					}
					else
					{	
						hr = Convert.ToInt32(s.Substring(start+1, 2));
						return start+3;
					}
				}
				default:
				{
					hasMin=true;
					String sign = s.Substring(start,1);
					if( sign == "+" ) negpos=true;
					if( String.Compare(s, start+1, "0", 0, 1, true) == 0 )
					{
						hr =Convert.ToInt32(s.Substring(start+2,1));
					}
					else
					{
						hr = Convert.ToInt32(s.Substring(start+1,2));
					}
					if( String.Compare(s, start+4, "0", 0, 1, true) == 0)
					{
						min = Convert.ToInt32(s.Substring(start+5,1));
					}
					else
					{
						min = Convert.ToInt32(s.Substring(start+4,2));
					}
					return start+6;
				}
			}
		}

		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			d.UTC = true;
			DateTime n = DateTime.Now;
			int new_hr;
			int new_min;
			if( negpos )
			{
				new_hr  = ((n.Hour + hr)%24);
				if( hasMin ) 
				{
					new_min =  n.Minute + min;
					d.Minute = new_min;
				}
			}
			else
			{
				new_hr = ((n.Hour - hr)%24);
				if( hasMin) 
				{
					new_min = n.Minute - min;
					d.Minute = new_min;
				}
			}
			d.TwentyFourHour = new_hr;
			return d;
		}
	}
	
	internal class EraFormatter: FormatTemplate  
	{
		internal int Count=0; 
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
	
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			// assuming the possible Era values
			String a = "A.D.";
			String b = "B.C.";
			if( String.Compare(s, start, a, 0, 4, false) == 0)
			{	
			}
			else if( String.Compare(s, start, b, 0, 4, false) == 0)
			{
			}
			else
			{
				throw new FormatException("Invalid Era Name");
			}
			return start+4;
		}
	
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			return d;
		}
	}
	
	internal class DateSeparatorFormatter: FormatTemplate  
	{
		internal int Count=0; 
		
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			if( String.Compare(s, start, info.DateSeparator, 0, 1, true) == 0 )
			{
				return start+1;
			}
			else
			{
				throw new FormatException("Invalid Date Formatter");
			}
		}
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			return d;
		}
	}
	
	internal class TimeSeparatorFormatter: FormatTemplate  
	{
		internal int Count=0; 
		internal override String GetString(DateTime d, DateTimeFormatInfo info)
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
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			if( String.Compare(s, start, info.TimeSeparator, 0, 1, true) == 0 )
			{
				return start+1;
			}
			else
			{
				throw new FormatException("Invalid Date Formatter");
			}
		}
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			return d;
		}
	}
	
	internal class ExplicitString: FormatTemplate
	{
		internal String Text="";
		internal override String GetString(DateTime d,DateTimeFormatInfo info)
		{
			return Text;
		}
		internal override int Parse(String s, int start, DateTimeFormatInfo info)
		{
			return start+Text.Length;
		}
		internal override ParsedDateTime StoreTo(ParsedDateTime d)
		{
			return d;
		}
	}
}

