/*
 * ParsedDateTime.cs - Implementation of the 
 *				"System.Private.DateTimeFormat.ParsedDateTime" class.
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

	internal class ParsedDateTime
	{
		bool hasDay = false;
		bool hasMonth = false;
		bool hasYear = false;
		bool hasTwentyfourHour = false;
		bool hasTwelveHour = false;
		bool hasMinute = false;
		bool hasSecond = false;
		bool hasAMPM = false;
		bool hasFractionalSecond = false;
		bool hasTicks = false;
		bool hasUTC = false;
		int day;
		int month;
		int yr;
		int hr;
		int min;
		int sec;
		int ap;
		long fs;
	
		internal DateTime storeInDateTime(DateTimeStyles style)
		{	
			bool hasDate = hasDay && hasMonth && hasYear;
			bool hasTime = (hasTwentyfourHour || hasTwelveHour) && hasMinute;
			DateTime newDate;

			DateTime baseDate;
			
			if((style == DateTimeStyles.NoCurrentDateDefault) && !hasDate)
			{
				baseDate = new DateTime(0);
			}
			else
			{
				baseDate = DateTime.Now;
			}
	
			/* TODO : Figure out if the latter case is right */
			/*	if( hasUTC && (hasDate || hasTime ) )
				throw new FormatException("Cannot set UTC and other time value");			*/
			
			if( hasAMPM && hasTwelveHour)
			{
				// time is in 12 hour format and set to PM, add 12, mod 24
				if( (this.TwelveHour<13) && (this.AMPM==1) ) 
					hr=((this.TwelveHour+12)%24);
			}

			// begin picking which items to construct datetime object with
			if( hasDate && !hasTime )
			{
				// set date of object, time is 00:00:00
				newDate = new DateTime(this.Year, this.Month, this.Day);
			}
			else if( hasTime && !hasDate )
			{
				newDate = new DateTime((baseDate).Year, (baseDate).Month,
						(baseDate).Day, hr, this.Minute, this.Second);
			}
			else if( hasDate && hasTime )
			{
				newDate = new DateTime(this.Year, this.Month, this.Day, hr, 
					this.Minute, this.Second);
			}
			else if( hasDay && hasMonth )
			{
				newDate = new DateTime((baseDate).Year, this.Month, this.Day);
			}
			else if( hasTwentyfourHour && hasMinute )
			{
				newDate = new DateTime((baseDate).Year, (baseDate).Month,
					(baseDate).Day, hr, this.Minute, (baseDate).Second);
			}		
			else if( hasTwentyfourHour && hasMinute )
			{
				newDate = new DateTime((baseDate).Year, (baseDate).Month,
					(baseDate).Day, hr, this.Minute, (baseDate).Second);
			}
			else if( hasYear && hasMonth )
			{
				newDate = new DateTime(this.Year, this.Month, (baseDate).Day);
			}
			else
			{
				throw new FormatException("Could not store parsed date");
			}
			if( hasFractionalSecond )
					newDate = newDate.Add(new TimeSpan(fs) );

			return newDate;
		}

		internal int Day
		{
			set
			{
				if(value<=0)
					throw new FormatException("Invalid Day value");
				
				hasDay = true;
				day=value;
			}
			get
			{
				return day;
			}
		}
		internal int Month
		{
			set
			{
				if(value<=0)
					throw new FormatException("Invalid Month value");
				
				hasMonth=true;
				month=value;
			}
			get
			{
				return month;
			}
		}
		internal int Year
		{
			set
			{
				hasYear=true;
				yr=value;
			}
			get
			{
				return yr;
			}
		}
		internal int TwentyFourHour
		{
			set
			{
				if( (value<0) || (value>24) )
					throw new FormatException("TwentyFourHour value out of range");

				hasTwentyfourHour=true;
				hr=value;
			}
			get
			{
				return hr;
			}
		}
		internal int TwelveHour
		{
			set
			{
				if( (value<0) || (value>12) )
					throw new FormatException("TwelveHour value out of range");
				
				hasTwelveHour=true;
				hr=value;
			}
			get
			{
				return hr;
			}
		}
		internal int AMPM
		{	
			// 0 == AM, 1==PM
			set
			{
				hasAMPM=true;
				ap = value;
			}
			get
			{
				return ap;
			}
		}
		internal int Minute
		{
			set
			{
				if( (value<0) || (value>60) )
					throw new FormatException("Minute value out of range");
				
				hasMinute=true;
				min=value;
			}
			get
			{
				return min;
			}
		}
		internal int Second
		{
			set
			{
				if( (value<0) || (value>60) )
					throw new FormatException("Second value out of range");
				
				hasSecond=true;
				sec=value;
			}
			get
			{
				return sec;
			}
		}
		internal long FractionalSecond
		{
			set
			{
				hasFractionalSecond=true;
				fs=value;
			}
			get
			{
				return fs;
			}
		}
		internal bool UTC
		{
			set
			{
				hasUTC=value;
			}
			get
			{
				return hasUTC;
			}
		}
	}
}

	
