/*
 * DateTimeParser.cs - Implementation of the 
 *				"System.Private.DateTimeFormat.DateTimeParser" class.
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
	
	internal class DateTimeParser
	{ 
		public static DateTime Parse(String s, DateTimeFormatInfo info,
					DateTimeStyles style)
		{
			if(info==null) info=DateTimeFormatInfo.InvariantInfo;
			String[] formatSpecifiers =  new String[] {"d", "D", "f", "F", "g", 
				"G", "m", "M", "t", "T", "U", "y", "Y"};
			String[] infoPatterns = new String[] {info.FullDateTimePattern, 
				info.LongDatePattern, info.LongTimePattern, info.MonthDayPattern, 
				info.ShortDatePattern, info.ShortTimePattern, 
				info.YearMonthPattern};

			int fsl = formatSpecifiers.Length;
			int ipl = infoPatterns.Length;
			String[] patterns = new String [fsl + ipl];	

			for( int i = 0; i < fsl; i++ )
				patterns[i] = 
					DateTimeFormatter.StandardFormatSpecifier(formatSpecifiers[i]);

			for( int i = fsl; i < fsl+ipl; i++ )
				patterns[i] = infoPatterns[i-fsl];

			DateTime result = new DateTime(0);

			bool success=false;
			
			for( int i = 0; (!success) && i < patterns.Length; i++ )
			{	
				try
				{
					result=ParseExact(s, patterns[i], info, style);
					success=true; // it worked !
				}
				// catch day and month names which are > than string indexes
				catch(ArgumentOutOfRangeException){}	
				// catch failing to parse with any given pattern
				catch(FormatException){}
			}		

			if(!success)
			{
				// throw FormatException if no default pattern matched s
				throw new FormatException("No DateTime pattern parsed correctly");
			}
			else
				return result;
		}		

		public static DateTime ParseExact(String s, String[] patterns,
					DateTimeFormatInfo info, DateTimeStyles style)
		{
			if(info==null) 
				info=DateTimeFormatInfo.InvariantInfo;	

			DateTime result = new DateTime(0);	
			bool success=false;

			for( int i=0; (!success) && i < patterns.Length; i++ )
			{
				try
				{
					result=ParseExact(s, patterns[i], info, 
										DateTimeStyles.NoCurrentDateDefault);
					success=true;
				}
				// catch matching name, months that are out of range
				catch ( ArgumentOutOfRangeException ){}
				// catch parses that do not succed
				catch ( FormatException ){}
			}
			if(!success)
			{
				throw new FormatException("No DateTime pattern parsed correctly"); 
			}
			else
				return result;
		}

		public static DateTime ParseExact(String s, String format,
				 	DateTimeFormatInfo info, DateTimeStyles style)
		{
			if( (format.Length) == 1 )
				format=DateTimeFormatter.StandardFormatSpecifier(format);
	
			Queue q=new Queue();
			if(info==null) 
				info=DateTimeFormatInfo.InvariantInfo;
	
			FormatTemplate current=null;
			current=new ExplicitString(); //mark the start
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
			ParsedDateTime d= new ParsedDateTime();
			int start = 0;
	
			foreach(FormatTemplate temp in q)
			{	
				start = temp.Parse(s, start, info); // parse or throw error
			}
	
			if(start!=s.Length)
			{ 
				throw new FormatException("Input string not fully consumed");
			}
	
			foreach(FormatTemplate temp in q)
			{
				d=temp.StoreTo(d); 
			}
	
			DateTime stored = d.storeInDateTime(style);
			return stored;
		}
	}
}


