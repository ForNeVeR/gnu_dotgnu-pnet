/*
 * Financial.cs - Implementation of the
 *			"Microsoft.VisualBasic.Financial" class.
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

namespace Microsoft.VisualBasic
{

using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using Microsoft.VisualBasic.CompilerServices;

[StandardModule]
public sealed class Financial
{
	// This class cannot be instantiated.
	private Financial() {}

	// Calculate depreciation.
	[TODO]
	public static double DDB(double Cost, double Salvage,
							 double Life, double Period,
							 [Optional] [DefaultValue(2.0)] double Factor)
			{
				// TODO
				return 0.0;
			}

	// Calculate future value.
	[TODO]
	public static double FV(double Rate, double Nper, double Pmt,
							 [Optional] [DefaultValue(0.0)] double PV,
							 [Optional] [DefaultValue(DueDate.EndOfPeriod)]
							 		DueDate Due)
			{
				// TODO
				return 0.0;
			}

	// Calculate interest payment.
	[TODO]
	public static double IPmt(double Rate, double Per, double NPer, double PV,
							  [Optional] [DefaultValue(0.0)] double FV,
							  [Optional] [DefaultValue(DueDate.EndOfPeriod)]
							 		DueDate Due)
			{
				// TODO
				return 0.0;
			}

	// Calculate internal rate of return.
	[TODO]
	public static double IRR(ref double[] ValueArray,
							 [Optional] [DefaultValue(0.1)] double Guess)
			{
				// TODO
				return 0.0;
			}

	// Calculate modified internal rate of return.
	[TODO]
	public static double MIRR(ref double[] ValueArray,
							  double FinanceRate, double ReinvestRate)
			{
				// TODO
				return 0.0;
			}

	// Calculate number of periods.
	[TODO]
	public static double NPer(double Rate, double Pmt, double PV,
							  [Optional] [DefaultValue(0.0)] double FV,
							  [Optional] [DefaultValue(DueDate.EndOfPeriod)]
							 		DueDate Due)
			{
				// TODO
				return 0.0;
			}

	// Calculate net present value.
	[TODO]
	public static double NPV(double Rate, ref double[] ValueArray)
			{
				// TODO
				return 0.0;
			}

	// Calculate payment.
	[TODO]
	public static double Pmt(double Rate, double NPer, double PV,
							 [Optional] [DefaultValue(0.0)] double FV,
							 [Optional] [DefaultValue(DueDate.EndOfPeriod)]
							 		DueDate Due)
			{
				// TODO
				return 0.0;
			}

	// Calculate principal payment.
	[TODO]
	public static double PPmt(double Rate, double Per, double NPer, double PV,
							  [Optional] [DefaultValue(0.0)] double FV,
							  [Optional] [DefaultValue(DueDate.EndOfPeriod)]
							 		DueDate Due)
			{
				// TODO
				return 0.0;
			}

	// Calculate present value.
	[TODO]
	public static double PV(double Rate, double NPer, double PV,
							[Optional] [DefaultValue(0.0)] double FV,
							[Optional] [DefaultValue(DueDate.EndOfPeriod)]
							 		DueDate Due)
			{
				// TODO
				return 0.0;
			}

	// Calculate rate.
	[TODO]
	public static double Rate(double NPer, double Pmt, double PV,
							  [Optional] [DefaultValue(0.0)] double FV,
							  [Optional] [DefaultValue(DueDate.EndOfPeriod)]
							 		DueDate Due,
							  [Optional] [DefaultValue(0.1)] double Guess)
			{
				// TODO
				return 0.0;
			}

	// Calculate straight-line depreciation.
	[TODO]
	public static double SLN(double Cost, double Salvage, double Life)
			{
				// TODO
				return 0.0;
			}

	// Calculate sum of years for depreciation.
	[TODO]
	public static double SYD(double Cost, double Salvage,
							 double Life, double Period)
			{
				// TODO
				return 0.0;
			}

}; // class Financial

}; // namespace Microsoft.VisualBasic
