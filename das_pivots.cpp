#include "sierrachart.h"
#include "SCStudyFunctions.h"

SCDLLName("DAS Camarilla Pivots");

/*==========================================================================*/
int CalculateDASPivotPoints
( float PriorOpen
, float PriorHigh
, float PriorLow
, float PriorClose
, float CurrentOpen
, float& PivotPoint
, float& PivotPointHigh
, float& PivotPointLow
, float& R_5
, float& R1, float& R1_5
, float& R2, float& R2_5
, float& R3
, float& S_5
, float& S1, float& S1_5
, float& S2, float& S2_5
, float& S3
, float& R3_5
, float& S3_5
, float& R4
, float& R4_5
, float& S4
, float& S4_5
, float& R5
, float& S5
, float& R6
, float& S6
, float& R7
, float& S7
, float& R8
, float& S8
, float& R9
, float& S9
, float& R10
, float& S10
, int FormulaType
)
{
	if (FormulaType == 0) // DAS Camarilla Pivot Points
	{
		float Range = PriorHigh - PriorLow;
		PivotPoint = (PriorHigh + PriorLow + PriorClose) / 3;

		R_5 = PriorClose + Range * 1.1f/18;
		R1 = PriorClose + Range * 1.1f/12;
		R1_5 = PriorClose + Range * 1.1f/9;
		R2 = PriorClose + Range * 1.1f/6;
		R2_5 = PriorClose + Range * 1.1f/5;
		R3 = PriorClose + Range * 1.1f/4;
		R3_5 = PriorClose + Range * 1.1f/3;
		R4 = PriorClose + Range * 1.1f/2;
		R4_5 = PriorClose + Range * 1.1f / 1.33f;

		S_5 = PriorClose - Range * 1.1f/18;
		S1 = PriorClose - Range * 1.1f/12;
		S1_5 = PriorClose - Range * 1.1f/9;
		S2 = PriorClose - Range * 1.1f/6;
		S2_5 = PriorClose - Range * 1.1f/5;
		S3 = PriorClose - Range * 1.1f/4;
		S3_5 = PriorClose - Range * 1.1f/3;
		S4 = PriorClose - Range * 1.1f/2;
		S4_5 = PriorClose - Range * 1.1f / 1.33f;
		R5 = R4 + (1.168f * (R4 - R3));
		S5 = S4 - (1.168f * (S3 - S4));
		R6 = (PriorHigh/PriorLow) *PriorClose;
		S6 = PriorClose -(R6-PriorClose);
	}

	return 1;
}


/*==========================================================================*/
int CalculateDailyOHLC
( SCStudyInterfaceRef sc
, const SCDateTimeMS& CurrentBarTradingDayDate
, int InNumberOfDaysBack
, int InNumberOfDaysToCalculate
, int InUseSaturdayData
, int InUseThisIntradayChart
, int InDailyChartNumber
, SCGraphData& DailyChartData
, SCDateTimeArray& DailyChartDateTimes
, int UseDaySessionOnly
, float& Open
, float& High
, float& Low
, float& Close
, float& Volume
, int InIncludeFridayEveningSessionWithSundayEveningSession
, int InUseSundayData
)
{
	if (InUseThisIntradayChart) // Use this Chart
	{
		int ArraySize = sc.ArraySize;
		if (ArraySize <= 1)
			return 0;
		
		const int LastIndex = ArraySize - 1;
		
		SCDateTimeMS LastTradingDayDateInChart = sc.GetTradingDayDate(sc.BaseDateTimeIn[LastIndex]);
		
		// Return and do not calculate if based upon trading day dates, that the current bar is earlier than the number of days to calculate.
		if (CurrentBarTradingDayDate <= SCDateTimeMS(LastTradingDayDateInChart).SubtractDays(InNumberOfDaysToCalculate))
		{
			return 0;
		}
				
		SCDateTimeMS StartDateTimeForTradingDate = sc.GetStartDateTimeForTradingDate(CurrentBarTradingDayDate);		

		for (int DaysBackCount = 1; DaysBackCount <= InNumberOfDaysBack; DaysBackCount++)
		{
			StartDateTimeForTradingDate.SubtractDays(1);

			SCDateTimeMS TradingDayDate = sc.GetTradingDayDate(StartDateTimeForTradingDate);

			if (!InUseSundayData && TradingDayDate.IsSunday())
			{
				StartDateTimeForTradingDate.SubtractDays(1);
			}

			TradingDayDate = sc.GetTradingDayDate(StartDateTimeForTradingDate);

			if (!InUseSaturdayData && TradingDayDate.IsSaturday())
			{
				StartDateTimeForTradingDate.SubtractDays(1);
			}
		}

		SCDateTimeMS IntendedTradingDayDate = sc.GetTradingDayDate(StartDateTimeForTradingDate);

		SCDateTimeMS ContainingIndexTradingDayDate;

		for(int DayCount = 0; DayCount < DAYS_PER_WEEK; DayCount++)
		{
			
			int ContainingIndex = sc.GetNearestMatchForSCDateTime(sc.ChartNumber, StartDateTimeForTradingDate);

			ContainingIndexTradingDayDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[ContainingIndex]);

			if ((ContainingIndexTradingDayDate - IntendedTradingDayDate).GetDaysSinceBaseDate() <= -1)
			{
				ContainingIndex++;
				ContainingIndexTradingDayDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[ContainingIndex]);
			}

			if ((ContainingIndexTradingDayDate - IntendedTradingDayDate).GetDaysSinceBaseDate() >= 1)
			{
				ContainingIndex--;
				ContainingIndexTradingDayDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[ContainingIndex]);
			}


			if (!InUseSundayData && ContainingIndexTradingDayDate.IsSunday())
			{
				StartDateTimeForTradingDate.SubtractDays(1);
				continue;
			}

			if (!InUseSaturdayData && ContainingIndexTradingDayDate.IsSaturday())
			{
				StartDateTimeForTradingDate.SubtractDays(1);
				continue;
			}


			if (ContainingIndexTradingDayDate > CurrentBarTradingDayDate)
			{
				StartDateTimeForTradingDate.SubtractDays(1);
				continue;
			}

			if (InNumberOfDaysBack > 0
				&& ContainingIndexTradingDayDate == CurrentBarTradingDayDate)
			{
				StartDateTimeForTradingDate.SubtractDays(1);
				IntendedTradingDayDate.SubtractDays(1);
				continue;
			}

			break;
		}

		if(!UseDaySessionOnly)
		{
			if (!sc.GetOpenHighLowCloseVolumeForDate(ContainingIndexTradingDayDate, Open, High, Low, Close, Volume, InIncludeFridayEveningSessionWithSundayEveningSession))
			{
				return 0;
			}
		}
		else
		{
			SCDateTimeMS StartDateTime;
			SCDateTimeMS EndDateTime;

			if(sc.StartTime1 <= sc.EndTime1)
			{
				StartDateTime.SetDate(ContainingIndexTradingDayDate);
				StartDateTime.SetTime(sc.StartTime1);

				EndDateTime.SetDate(ContainingIndexTradingDayDate);
				EndDateTime.SetTime(sc.EndTime1);
			}
			else
			{
				StartDateTime.SetDate(ContainingIndexTradingDayDate - 1);
				StartDateTime.SetTime(sc.StartTime1);

				EndDateTime.SetDate(ContainingIndexTradingDayDate);
				EndDateTime.SetTime(sc.EndTime1);
			}

			// Adjust the time to .999999 us
			EndDateTime.AddSeconds(1);
			EndDateTime.SubtractMicroseconds(1);

			float NextOpen = 0;

			if (!sc.GetOHLCOfTimePeriod(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen))
			{
				return 0;
			}
		}
	}
	else // Use Daily Chart
	{		
		SCDateTimeMS LastDateInDestinationChart = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.ArraySize - 1]);
		
		// Return and do not calculate if based upon trading day dates, that the current bar is earlier than the number of days to calculate.
		if (CurrentBarTradingDayDate <= SCDateTimeMS(LastDateInDestinationChart).SubtractDays(InNumberOfDaysToCalculate))
		{
			return 0;
		}
		
		
		// Look for a matching date that is not a weekend

		int FirstIndexOfReferenceDay = 0;
		SCDateTimeMS ReferenceDay = CurrentBarTradingDayDate;
		ReferenceDay.SubtractDays(InNumberOfDaysBack);

		if (InNumberOfDaysBack == 0)
		{
			FirstIndexOfReferenceDay = sc.GetFirstNearestIndexForTradingDayDate(InDailyChartNumber, ReferenceDay.GetDate());
		}
		else
		{
			while (ReferenceDay.IsWeekend(InUseSaturdayData != 0))
			{
				ReferenceDay.SubtractDays(1);
			}

			FirstIndexOfReferenceDay = sc.GetFirstNearestIndexForTradingDayDate(InDailyChartNumber, ReferenceDay.GetDate());

			if (sc.GetTradingDayDate(DailyChartDateTimes[FirstIndexOfReferenceDay]) == CurrentBarTradingDayDate
				&& FirstIndexOfReferenceDay >= 1)
			{
				--FirstIndexOfReferenceDay;
			}

#ifdef _DEBUG
			SCString DateString1 = sc.DateTimeToString(ReferenceDay, FLAG_DT_COMPLETE_DATETIME);
			SCString DateString2 = sc.DateTimeToString(DailyChartDateTimes[FirstIndexOfReferenceDay].GetAsDouble(), FLAG_DT_COMPLETE_DATETIME);
			SCString LogMessage;
			LogMessage.Format("%s, %s", DateString1.GetChars(), DateString2.GetChars());
			sc.AddMessageToLog(LogMessage, 0);
#endif
		}

		
		Open = DailyChartData[SC_OPEN][FirstIndexOfReferenceDay];
		High = DailyChartData[SC_HIGH][FirstIndexOfReferenceDay];
		Low = DailyChartData[SC_LOW][FirstIndexOfReferenceDay];
		Close = DailyChartData[SC_LAST][FirstIndexOfReferenceDay];
	}
	
	return 1;
}

int GetDailyChartIndexForDate(SCGraphData& DailyChartData, int NumberOfDaysToCalculate, SCStudyInterfaceRef& sc, int DailyChartNumber, SCDateTimeArray& DailyChartDateTimeArray, int TargetDate)
{
	if (DailyChartData[0].GetArraySize() < 2)
		return  -1;

	int DailyChartBarIndex = -1;
	int CountBack = DailyChartData[SC_OPEN].GetArraySize() - NumberOfDaysToCalculate - 1;
	for (int CurrentIndex = DailyChartData[SC_OPEN].GetArraySize() - 1; CurrentIndex >= CountBack && CurrentIndex >= 0; CurrentIndex--)
	{
		if (sc.GetTradingDayDateForChartNumber(DailyChartNumber, DailyChartDateTimeArray[CurrentIndex]) < TargetDate)
		{
			DailyChartBarIndex = CurrentIndex;
			break;
		}
	}

	return DailyChartBarIndex;
}


/*==========================================================================*/
int CalculateDASDailyPivotPoints
( SCStudyInterfaceRef sc
, int IntradayChartDate
, int FormulaType
, int DailyChartNumber
, SCGraphData& DailyChartData
, SCDateTimeArray& DailyChartDateTimeArray
, int NumberOfDaysToCalculate
, int UseSaturdayData
, int UseThisChart
, int UseManualValues
, float UserOpen
, float UserHigh
, float UserLow
, float UserClose
, int  UseDaySessionOnly
, float& PivotPoint
, float& PivotPointHigh
, float& PivotPointLow
, float& R_5
, float& R1, float& R1_5
, float& R2, float& R2_5
, float& R3
, float& S_5
, float& S1, float& S1_5
, float& S2, float& S2_5
, float& S3
, float& R3_5
, float& S3_5
, float& R4
, float& R4_5
, float& S4
, float& S4_5
, float& R5
, float& S5
, float& R6
, float& S6
, float& R7
, float& S7
, float& R8
, float& S8
, float& R9
, float& S9
, float& R10
, float& S10
, int UseDailyChartForSettlementOnly
)
{

	float Open = 0.0, High = 0.0, Low = 0.0, Close = 0.0, NextOpen = 0.0;

	if (UseThisChart != 0) // Use this Chart
	{
		int ArraySize = sc.BaseDateTimeIn.GetArraySize();
		if (ArraySize < 2)
			return 0;

		int LastIndex = ArraySize - 1;

		int CurrentDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[LastIndex]);
		
		// Return and do not calculate pivots if the current bar to be calculated is further back than the number of days to calculate
		if (CurrentDate - NumberOfDaysToCalculate >= IntradayChartDate)
			return 0;


		// Look for the last good date which is not a weekend and has data
		int FirstIndexOfPriorDay;
		int PreviousDay = IntradayChartDate;
		while (true)
		{
			--PreviousDay;

		
			if (IsWeekend(PreviousDay, UseSaturdayData != 0))
				continue;

			// It is not a weekend
			FirstIndexOfPriorDay = sc.GetFirstIndexForDate(sc.ChartNumber, PreviousDay);
			if (sc.GetTradingDayDate(sc.BaseDateTimeIn[FirstIndexOfPriorDay]) == PreviousDay)
				break;

			if (FirstIndexOfPriorDay == 0)
			{
				// At the beginning of the date array, so we can't look back any farther
				break;
			}
		}

		if (sc.GetTradingDayDate(sc.BaseDateTimeIn[FirstIndexOfPriorDay]) != PreviousDay)
		{
			// Previous day not found
			return 0;
		}


		if(!UseDaySessionOnly)
		{

			if (!sc.GetOHLCForDate(PreviousDay, Open, High, Low, Close))
				return 0;

			float NextHigh, NextLow, NextClose;  // The values returned for these are unused
			if (!sc.GetOHLCForDate(IntradayChartDate, NextOpen, NextHigh, NextLow, NextClose))
				NextOpen = Close;  // If there is a failure, use the prior Close
		}
		else
		{
			SCDateTime StartDateTime;
			SCDateTime EndDateTime;

			if(sc.StartTime1 <= sc.EndTime1)
			{
				StartDateTime.SetDate(PreviousDay);
				StartDateTime.SetTime(sc.StartTime1);

				EndDateTime.SetDate(PreviousDay);
				EndDateTime.SetTime(sc.EndTime1);
			}
			else
			{
				StartDateTime.SetDate(PreviousDay - 1);
				StartDateTime.SetTime(sc.StartTime1);

				EndDateTime.SetDate(PreviousDay);
				EndDateTime.SetTime(sc.EndTime1);
			}

			
			if(!sc.GetOHLCOfTimePeriod(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen))
				return 0;
		}

		if(UseDailyChartForSettlementOnly)
		{
			int DailyChartBarIndex = GetDailyChartIndexForDate(DailyChartData, NumberOfDaysToCalculate, sc, DailyChartNumber, DailyChartDateTimeArray, IntradayChartDate);

			if (DailyChartBarIndex == -1)
				return 0;

			Close = DailyChartData[SC_LAST][DailyChartBarIndex];
		}
	}
	else // Use Daily Chart
	{
		int DailyChartBarIndex = GetDailyChartIndexForDate(DailyChartData, NumberOfDaysToCalculate, sc, DailyChartNumber, DailyChartDateTimeArray, IntradayChartDate);


		if(DailyChartBarIndex == -1)
			return 0;

		Open = DailyChartData[SC_OPEN][DailyChartBarIndex];
		High = DailyChartData[SC_HIGH][DailyChartBarIndex];
		Low = DailyChartData[SC_LOW][DailyChartBarIndex];
		Close = DailyChartData[SC_LAST][DailyChartBarIndex];

		if (DailyChartBarIndex == DailyChartData[0].GetArraySize() - 1 )
			NextOpen = DailyChartData[SC_LAST][DailyChartBarIndex];
		else
			NextOpen = DailyChartData[SC_OPEN][DailyChartBarIndex + 1];
	}
	
	// If we are calculating the last day, and the user has manually entered OHLC values, then use those
	if (UseManualValues)
	{
		int ArraySize = sc.BaseDateTimeIn.GetArraySize();

		int LastIndex = ArraySize - 1;

		int CurrentDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[LastIndex]);

		if (CurrentDate == IntradayChartDate)
		{
			Open = (UserHigh + UserLow + UserClose) / 3.0f;  // This is not normally used, but we need to set it to something reasonable
			High = UserHigh;
			Low = UserLow;
			Close = UserClose;
			NextOpen = UserOpen;
		}
	}

	return CalculateDASPivotPoints
		(  Open
		,  High
		,  Low
		,  Close
		,  NextOpen
		,  PivotPoint
		, PivotPointHigh
		, PivotPointLow
		,  R_5
		,  R1,  R1_5
		,  R2,  R2_5
		,  R3
		,  S_5
		,  S1,  S1_5
		,  S2,  S2_5
		,  S3
		,  R3_5
		,  S3_5
		,  R4
		,  R4_5
		,  S4
		,  S4_5
		,  R5
		,  S5
		,  R6
		,  S6
		,  R7
		,  S7
		,  R8
		,  S8
		,  R9
		,  S9
		,  R10
		,  S10
		,  FormulaType
		);
}


/*==========================================================================*/
SCSFExport scsf_DASPivotPointsDaily(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_R1 = sc.Subgraph[0];
	SCSubgraphRef Subgraph_R2 = sc.Subgraph[1];
	SCSubgraphRef Subgraph_S1 = sc.Subgraph[2];
	SCSubgraphRef Subgraph_S2 = sc.Subgraph[3];
	SCSubgraphRef Subgraph_R0_5 = sc.Subgraph[4];
	SCSubgraphRef Subgraph_R1_5 = sc.Subgraph[5];
	SCSubgraphRef Subgraph_R2_5 = sc.Subgraph[6];
	SCSubgraphRef Subgraph_R3 = sc.Subgraph[7];
	SCSubgraphRef Subgraph_S0_5 = sc.Subgraph[8];
	SCSubgraphRef Subgraph_S1_5 = sc.Subgraph[9];
	SCSubgraphRef Subgraph_S2_5 = sc.Subgraph[10];
	SCSubgraphRef Subgraph_S3 = sc.Subgraph[11];
	SCSubgraphRef Subgraph_PP = sc.Subgraph[12];
	SCSubgraphRef Subgraph_PPHigh = sc.Subgraph[13];
	SCSubgraphRef Subgraph_PPLow = sc.Subgraph[14];
	SCSubgraphRef Subgraph_R4 = sc.Subgraph[15];
	SCSubgraphRef Subgraph_S4 = sc.Subgraph[16];
	SCSubgraphRef Subgraph_R3_5 = sc.Subgraph[17];
	SCSubgraphRef Subgraph_S3_5 = sc.Subgraph[18];
	SCSubgraphRef Subgraph_R5 = sc.Subgraph[19];
	SCSubgraphRef Subgraph_S5 = sc.Subgraph[20];
	SCSubgraphRef Subgraph_R6 = sc.Subgraph[21];
	SCSubgraphRef Subgraph_S6 = sc.Subgraph[22];
	SCSubgraphRef Subgraph_R7 = sc.Subgraph[23];
	SCSubgraphRef Subgraph_S7 = sc.Subgraph[24];
	SCSubgraphRef Subgraph_R8 = sc.Subgraph[25];
	SCSubgraphRef Subgraph_S8 = sc.Subgraph[26];
	SCSubgraphRef Subgraph_R9 = sc.Subgraph[27];
	SCSubgraphRef Subgraph_S9 = sc.Subgraph[28];
	SCSubgraphRef Subgraph_R10 = sc.Subgraph[29];
	SCSubgraphRef Subgraph_S10 = sc.Subgraph[30];
	SCSubgraphRef Subgraph_R4_5 = sc.Subgraph[31];
	SCSubgraphRef Subgraph_S4_5 = sc.Subgraph[32];
	const int NUMBER_OF_STUDY_SUBGRAPHS = 33;
	
	SCInputRef Input_FormulaType = sc.Input[4];
	SCInputRef Input_NumberOfDays = sc.Input[5];
	SCInputRef Input_RoundToTickSize = sc.Input[6];
	SCInputRef Input_UseSaturdayData = sc.Input[7];
	SCInputRef Input_DailyChartNumber = sc.Input[9];//previously input 3
	SCInputRef Input_ReferenceDailyChartForData = sc.Input[10];
	SCInputRef Input_ForwardProjectLines = sc.Input[11];
	SCInputRef Input_UseManualValues = sc.Input[12];
	SCInputRef Input_UserOpen = sc.Input[13];
	SCInputRef Input_UserHigh = sc.Input[14];
	SCInputRef Input_UserLow = sc.Input[15];
	SCInputRef Input_UserClose = sc.Input[16];
	SCInputRef Input_UseDaySessionOnly = sc.Input[17];
	SCInputRef Input_Version = sc.Input[18];
	SCInputRef Input_UseDailyChartForSettlementOnly = sc.Input[19];

	if (sc.SetDefaults)
	{

		sc.GraphName = "DAS Pivot Points-Daily";
		
		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		
		sc.DrawStudyUnderneathMainPriceGraph = 1;

		Subgraph_R1.Name = "R1";
		Subgraph_R2.Name = "R2";
		Subgraph_S1.Name = "S1";
		Subgraph_S2.Name = "S2";
		Subgraph_R0_5.Name = "R.5";
		Subgraph_R1_5.Name = "R1.5";
		Subgraph_R2_5.Name = "R2.5";
		Subgraph_R3.Name = "R3";
		Subgraph_S0_5.Name = "S.5";
		Subgraph_S1_5.Name = "S1.5";
		Subgraph_S2_5.Name = "S2.5";
		Subgraph_S3.Name = "S3";
		Subgraph_PP.Name = "PP";
		Subgraph_PPHigh.Name = "PP High";
		Subgraph_PPLow.Name = "PP Low";
		Subgraph_R4.Name = "R4";
		Subgraph_S4.Name = "S4";
		Subgraph_R3_5.Name = "R3.5";
		Subgraph_S3_5.Name = "S3.5";
		Subgraph_R5.Name = "R5";
		Subgraph_S5.Name = "S5";
		Subgraph_R6.Name = "R6";
		Subgraph_S6.Name = "S6";
		Subgraph_R7.Name = "R7";
		Subgraph_S7.Name = "S7";
		Subgraph_R8.Name = "R8";
		Subgraph_S8.Name = "S8";
		Subgraph_R9.Name = "R9";
		Subgraph_S9.Name = "S9";
		Subgraph_R10.Name = "R10";
		Subgraph_S10.Name = "S10";
		Subgraph_R4_5.Name = "R4_5";
		Subgraph_S4_5.Name = "S4_5";

		Subgraph_R1.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R2.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S1.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S2.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R3.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S3.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_PP.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_PPHigh.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_PPLow.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R4.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S4.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R5.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S5.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R6.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S6.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R7.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S7.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R8.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S8.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R9.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S9.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R10.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S10.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R4_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S4_5.DrawStyle = DRAWSTYLE_HIDDEN;

		Subgraph_R1.LineWidth = 3;
		Subgraph_R2.LineWidth = 3;
		Subgraph_S1.LineWidth = 3;
		Subgraph_S2.LineWidth = 3;
		Subgraph_R3.LineWidth = 3;
		Subgraph_R4.LineWidth = 3;
		Subgraph_R5.LineWidth = 3;
		Subgraph_R6.LineWidth = 3;
		Subgraph_S3.LineWidth = 3;
		Subgraph_S4.LineWidth = 3;
		Subgraph_S5.LineWidth = 3;
		Subgraph_S6.LineWidth = 3;

		Subgraph_R1.UseTransparentLabelBackground = 1;
		Subgraph_R2.UseTransparentLabelBackground = 1;
		Subgraph_S1.UseTransparentLabelBackground = 1;
		Subgraph_S2.UseTransparentLabelBackground = 1;
		Subgraph_R3.UseTransparentLabelBackground = 1;
		Subgraph_R4.UseTransparentLabelBackground = 1;
		Subgraph_R5.UseTransparentLabelBackground = 1;
		Subgraph_R6.UseTransparentLabelBackground = 1;
		Subgraph_S3.UseTransparentLabelBackground = 1;
		Subgraph_S4.UseTransparentLabelBackground = 1;
		Subgraph_S5.UseTransparentLabelBackground = 1;
		Subgraph_S6.UseTransparentLabelBackground = 1;

		Subgraph_R1.DrawZeros = false;
		Subgraph_R2.DrawZeros = false;
		Subgraph_S1.DrawZeros = false;
		Subgraph_S2.DrawZeros = false;
		Subgraph_R0_5.DrawZeros = false;
		Subgraph_R1_5.DrawZeros = false;
		Subgraph_R2_5.DrawZeros = false;
		Subgraph_R3.DrawZeros = false;
		Subgraph_S0_5.DrawZeros = false;
		Subgraph_S1_5.DrawZeros = false;
		Subgraph_S2_5.DrawZeros = false;
		Subgraph_S3.DrawZeros = false;
		Subgraph_PP.DrawZeros = false;
		Subgraph_PPHigh.DrawZeros = false;
		Subgraph_PPLow.DrawZeros = false;
		Subgraph_R4.DrawZeros = false;
		Subgraph_S4.DrawZeros = false;
		Subgraph_R3_5.DrawZeros = false;
		Subgraph_S3_5.DrawZeros = false;
		Subgraph_R5.DrawZeros = false;
		Subgraph_S5.DrawZeros = false;
		Subgraph_R6.DrawZeros = false;
		Subgraph_S6.DrawZeros = false;
		Subgraph_R7.DrawZeros = false;
		Subgraph_S7.DrawZeros = false;
		Subgraph_R8.DrawZeros = false;
		Subgraph_S8.DrawZeros = false;
		Subgraph_R9.DrawZeros = false;
		Subgraph_S9.DrawZeros = false;
		Subgraph_R10.DrawZeros = false;
		Subgraph_S10.DrawZeros = false;
		Subgraph_R4_5.DrawZeros = false;
		Subgraph_S4_5.DrawZeros = false;
		

		Subgraph_PP.PrimaryColor = RGB(255, 0, 255);
		Subgraph_PPHigh.PrimaryColor = RGB(255, 0, 255);
		Subgraph_PPLow.PrimaryColor = RGB(255, 0, 255);

		Subgraph_R1.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R2.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R0_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R1_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R2_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R3.PrimaryColor = RGB(128, 0, 0);
		Subgraph_R4.PrimaryColor = RGB(0, 128, 0);
		Subgraph_R4_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R3_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R5.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R6.PrimaryColor = RGB(128, 0, 0);
		Subgraph_R7.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R8.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R9.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R10.PrimaryColor = RGB(255, 0, 0);

		Subgraph_S1.PrimaryColor = RGB(128, 128, 128);
		Subgraph_S2.PrimaryColor = RGB(128, 128, 128);
		Subgraph_S0_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S1_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S2_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S3.PrimaryColor = RGB(0, 128, 0);
		Subgraph_S4.PrimaryColor = RGB(128, 0, 0);
		Subgraph_S4_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S3_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S5.PrimaryColor = RGB(128, 128, 128);
		Subgraph_S6.PrimaryColor = RGB(0, 128, 0);
		Subgraph_S7.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S8.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S9.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S10.PrimaryColor = RGB(0, 255, 0);

		for (int SubgraphIndex = 0; SubgraphIndex < NUMBER_OF_STUDY_SUBGRAPHS; SubgraphIndex++)
		{
			//sc.Subgraph[SubgraphIndex].LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
			sc.Subgraph[SubgraphIndex].LineLabel = LL_DISPLAY_NAME | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT;
		}

		sc.Input[3].SetChartNumber(1);

		Input_DailyChartNumber.Name = "Daily Chart Number";
		Input_DailyChartNumber.SetChartNumber(1);

		Input_FormulaType.Name = "Formula Type";
		Input_FormulaType.SetInt(0);

		Input_NumberOfDays.Name = "Number of Days To Calculate";
		Input_NumberOfDays.SetInt(50);
		Input_NumberOfDays.SetIntLimits(1,MAX_STUDY_LENGTH);

		Input_RoundToTickSize.Name = "Round to Tick Size";
		Input_RoundToTickSize.SetYesNo(0);

		Input_UseSaturdayData.Name = "Use Saturday Data";
		Input_UseSaturdayData.SetYesNo(0);

		Input_ReferenceDailyChartForData.Name = "Reference Daily Chart For Data";
		Input_ReferenceDailyChartForData.SetYesNo(false);

		Input_ForwardProjectLines.Name ="Forward Project Pivot Point Lines";
		Input_ForwardProjectLines.SetYesNo(0);
		
		Input_UseManualValues.Name = "Use User Entered OHLC Values";
		Input_UseManualValues.SetYesNo(0);
		
		Input_UserOpen.Name = "User Entered Open Value";
		Input_UserOpen.SetFloat(0.0f);
		
		Input_UserHigh.Name = "User Entered High Value";
		Input_UserHigh.SetFloat(0.0f);
		
		Input_UserLow.Name = "User Entered Low Value";
		Input_UserLow.SetFloat(0.0f);
		
		Input_UserClose.Name = "User Entered Close Value";
		Input_UserClose.SetFloat(0.0f);

		Input_UseDaySessionOnly.Name = "Use Day Session Only";
		Input_UseDaySessionOnly.SetYesNo(false);

		Input_UseDailyChartForSettlementOnly.Name = "Use Daily Chart For Settlement Only";
		Input_UseDailyChartForSettlementOnly.SetYesNo(false);

		Input_Version.SetInt(2);
		
		return;
	}
	//Upgrade code
	if (Input_Version.GetInt()<2)
	{
		Input_Version.SetInt(2);
		Input_DailyChartNumber.SetInt(sc.Input[3].GetInt() );
		Input_ReferenceDailyChartForData .SetYesNo(!Input_ReferenceDailyChartForData .GetYesNo());
	}

	float fPivotPoint= 0, fPivotPointHigh = 0, fPivotPointLow = 0;

	float fR0_5 = 0, fR1 = 0, fR1_5 = 0, fR2 = 0, fR2_5 = 0, fR3 = 0, fR3_5 = 0, fR4 = 0, fR4_5 = 0, fR5 = 0, fR6 = 0, fR7 = 0, fR8 = 0, fR9 = 0, fR10 = 0;
	float fS0_5 = 0, fS1 = 0, fS1_5 = 0, fS2 = 0, fS2_5 = 0, fS3 = 0, fS3_5 = 0, fS4 = 0, fS4_5 = 0, fS5 = 0, fS6 = 0, fS7 = 0, fS8 = 0, fS9 = 0, fS10 = 0;
	
	int IntradayChartDate = 0;
	int ValidPivotPoint = 1;

	// we get chart data only once for speed
	SCGraphData DailyChartData;
	SCDateTimeArray DailyChartDateTime;
	if (Input_ReferenceDailyChartForData.GetYesNo() || Input_UseDailyChartForSettlementOnly.GetYesNo())
	{
		sc.GetChartBaseData(Input_DailyChartNumber.GetChartNumber(), DailyChartData);
		sc.GetChartDateTimeArray(Input_DailyChartNumber.GetChartNumber(), DailyChartDateTime);

		if (Input_UseDailyChartForSettlementOnly.GetYesNo())
			Input_ReferenceDailyChartForData.SetYesNo(false);
	}

	int NumberOfForwardBars = 0;

	if(Input_ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = 20;

		if(sc.UpdateStartIndex == 0)
		{
			for (int SubgraphIndex = 0; SubgraphIndex < NUMBER_OF_STUDY_SUBGRAPHS; SubgraphIndex++)
				sc.Subgraph[SubgraphIndex].ExtendedArrayElementsToGraph = NumberOfForwardBars;
		}
	}

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize + NumberOfForwardBars; Index++)
	{
		if (IntradayChartDate != sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]))
		{
			IntradayChartDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);

			ValidPivotPoint =
				CalculateDASDailyPivotPoints(
					sc,
					IntradayChartDate,
					Input_FormulaType.GetInt(),
					Input_DailyChartNumber.GetChartNumber(),
					DailyChartData,
					DailyChartDateTime,
					Input_NumberOfDays.GetInt(),
					Input_UseSaturdayData.GetYesNo(),
					Input_ReferenceDailyChartForData.GetYesNo() ? 0 : 1,
					Input_UseManualValues.GetYesNo(),
					Input_UserOpen.GetFloat(),
					Input_UserHigh.GetFloat(),
					Input_UserLow.GetFloat(),
					Input_UserClose.GetFloat(),
					Input_UseDaySessionOnly.GetYesNo(),
					fPivotPoint,
					fPivotPointHigh,
					fPivotPointLow,
					fR0_5,
					fR1, fR1_5,
					fR2, fR2_5,
					fR3,
					fS0_5,
					fS1, fS1_5,
					fS2, fS2_5,
					fS3,
					fR3_5,
					fS3_5,
					fR4,
					fR4_5,
					fS4,
					fS4_5,
					fR5,
					fS5,
					fR6,
					fS6,
					fR7,
					fS7,
					fR8,
					fS8,
					fR9,
					fS9,
					fR10,
					fS10,
					Input_UseDailyChartForSettlementOnly.GetYesNo()
				);
		}

		if (!ValidPivotPoint)
			continue;
		
		if (Input_RoundToTickSize.GetYesNo() != 0)
		{
			Subgraph_R1[Index] = static_cast<float>(sc.RoundToTickSize(fR1, sc.TickSize));
			Subgraph_R2[Index] = static_cast<float>(sc.RoundToTickSize(fR2, sc.TickSize));
			Subgraph_S1[Index] = static_cast<float>(sc.RoundToTickSize(fS1, sc.TickSize));
			Subgraph_S2[Index] = static_cast<float>(sc.RoundToTickSize(fS2, sc.TickSize));

			Subgraph_R0_5[Index] = static_cast<float>(sc.RoundToTickSize(fR0_5, sc.TickSize));
			Subgraph_R1_5[Index] = static_cast<float>(sc.RoundToTickSize(fR1_5, sc.TickSize));
			Subgraph_R2_5[Index] = static_cast<float>(sc.RoundToTickSize(fR2_5, sc.TickSize));
			Subgraph_R3[Index] = static_cast<float>(sc.RoundToTickSize(fR3, sc.TickSize));
			Subgraph_S0_5[Index] = static_cast<float>(sc.RoundToTickSize(fS0_5, sc.TickSize));
			Subgraph_S1_5[Index] = static_cast<float>(sc.RoundToTickSize(fS1_5, sc.TickSize));
			Subgraph_S2_5[Index] = static_cast<float>(sc.RoundToTickSize(fS2_5, sc.TickSize));
			Subgraph_S3[Index] = static_cast<float>(sc.RoundToTickSize(fS3, sc.TickSize));
			Subgraph_PP[Index] = static_cast<float>(sc.RoundToTickSize(fPivotPoint, sc.TickSize));
			Subgraph_PPHigh[Index] = static_cast<float>(sc.RoundToTickSize(fPivotPointHigh, sc.TickSize));
			Subgraph_PPLow[Index] = static_cast<float>(sc.RoundToTickSize(fPivotPointLow, sc.TickSize));
			Subgraph_R4[Index] = static_cast<float>(sc.RoundToTickSize(fR4, sc.TickSize));
			Subgraph_R4_5[Index] = static_cast<float>(sc.RoundToTickSize(fR4_5, sc.TickSize));
			Subgraph_S4[Index] = static_cast<float>(sc.RoundToTickSize(fS4, sc.TickSize));
			Subgraph_S4_5[Index] = static_cast<float>(sc.RoundToTickSize(fS4_5, sc.TickSize));
			Subgraph_R3_5[Index] = static_cast<float>(sc.RoundToTickSize(fR3_5, sc.TickSize));
			Subgraph_S3_5[Index] = static_cast<float>(sc.RoundToTickSize(fS3_5, sc.TickSize));
			Subgraph_R5[Index] = static_cast<float>(sc.RoundToTickSize(fR5, sc.TickSize));
			Subgraph_S5[Index] = static_cast<float>(sc.RoundToTickSize(fS5, sc.TickSize));
			Subgraph_R6[Index] = static_cast<float>(sc.RoundToTickSize(fR6, sc.TickSize));
			Subgraph_S6[Index] = static_cast<float>(sc.RoundToTickSize(fS6, sc.TickSize));
			Subgraph_R7[Index] = static_cast<float>(sc.RoundToTickSize(fR7, sc.TickSize));
			Subgraph_S7[Index] = static_cast<float>(sc.RoundToTickSize(fS7, sc.TickSize));
			Subgraph_R8[Index] = static_cast<float>(sc.RoundToTickSize(fR8, sc.TickSize));
			Subgraph_S8[Index] = static_cast<float>(sc.RoundToTickSize(fS8, sc.TickSize));
			Subgraph_R9[Index] = static_cast<float>(sc.RoundToTickSize(fR9, sc.TickSize));
			Subgraph_S9[Index] = static_cast<float>(sc.RoundToTickSize(fS9, sc.TickSize));
			Subgraph_R10[Index] = static_cast<float>(sc.RoundToTickSize(fR10, sc.TickSize));
			Subgraph_S10[Index] = static_cast<float>(sc.RoundToTickSize(fS10, sc.TickSize));
		}
		else 
		{
			Subgraph_R1[Index] = fR1;
			Subgraph_R2[Index] = fR2;
			Subgraph_S1[Index] = fS1;
			Subgraph_S2[Index] = fS2;

			Subgraph_R0_5[Index] = fR0_5;
			Subgraph_R1_5[Index] = fR1_5;
			Subgraph_R2_5[Index] = fR2_5;
			Subgraph_R3[Index] = fR3;
			Subgraph_S0_5[Index] = fS0_5;
			Subgraph_S1_5[Index] = fS1_5;
			Subgraph_S2_5[Index] = fS2_5;
			Subgraph_S3[Index] = fS3;
			Subgraph_PP[Index] = fPivotPoint;
			Subgraph_PPHigh[Index] = fPivotPointHigh;
			Subgraph_PPLow[Index] = fPivotPointLow;
			Subgraph_R4[Index] = fR4;
			Subgraph_R4_5[Index] = fR4_5;
			Subgraph_S4[Index] = fS4;
			Subgraph_S4_5[Index] = fS4_5;
			Subgraph_R3_5[Index] = fR3_5;
			Subgraph_S3_5[Index] = fS3_5;
			Subgraph_R5[Index] = fR5;
			Subgraph_S5[Index] = fS5;
			Subgraph_R6[Index] = fR6;
			Subgraph_S6[Index] = fS6;
			Subgraph_R7[Index] = fR7;
			Subgraph_S7[Index] = fS7;
			Subgraph_R8[Index] = fR8;
			Subgraph_S8[Index] = fS8;
			Subgraph_R9[Index] = fR9;
			Subgraph_S9[Index] = fS9;
			Subgraph_R10[Index] = fR10;
			Subgraph_S10[Index] = fS10;
		}
	} 
}


/*==========================================================================*/
SCSFExport scsf_DASPivotPointsVariablePeriod(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_R1 = sc.Subgraph[0];
	SCSubgraphRef Subgraph_R2 = sc.Subgraph[1];
	SCSubgraphRef Subgraph_S1 = sc.Subgraph[2];
	SCSubgraphRef Subgraph_S2 = sc.Subgraph[3];
	SCSubgraphRef Subgraph_R0_5 = sc.Subgraph[4];
	SCSubgraphRef Subgraph_R1_5 = sc.Subgraph[5];
	SCSubgraphRef Subgraph_R2_5 = sc.Subgraph[6];
	SCSubgraphRef Subgraph_R3 = sc.Subgraph[7];
	SCSubgraphRef Subgraph_S0_5 = sc.Subgraph[8];
	SCSubgraphRef Subgraph_S1_5 = sc.Subgraph[9];
	SCSubgraphRef Subgraph_S2_5 = sc.Subgraph[10];
	SCSubgraphRef Subgraph_S3 = sc.Subgraph[11];
	SCSubgraphRef Subgraph_PP = sc.Subgraph[12];
	SCSubgraphRef Subgraph_PPHigh = sc.Subgraph[13];
	SCSubgraphRef Subgraph_PPLow = sc.Subgraph[14];
	SCSubgraphRef Subgraph_R4 = sc.Subgraph[15];
	SCSubgraphRef Subgraph_S4 = sc.Subgraph[16];
	SCSubgraphRef Subgraph_R3_5 = sc.Subgraph[17];
	SCSubgraphRef Subgraph_S3_5 = sc.Subgraph[18];
	SCSubgraphRef Subgraph_R5 = sc.Subgraph[19];
	SCSubgraphRef Subgraph_S5 = sc.Subgraph[20];
	SCSubgraphRef Subgraph_R6 = sc.Subgraph[21];
	SCSubgraphRef Subgraph_S6 = sc.Subgraph[22];
	SCSubgraphRef Subgraph_R7 = sc.Subgraph[23];
	SCSubgraphRef Subgraph_S7 = sc.Subgraph[24];
	SCSubgraphRef Subgraph_R8 = sc.Subgraph[25];
	SCSubgraphRef Subgraph_S8 = sc.Subgraph[26];
	SCSubgraphRef Subgraph_R9 = sc.Subgraph[27];
	SCSubgraphRef Subgraph_S9 = sc.Subgraph[28];
	SCSubgraphRef Subgraph_R10 = sc.Subgraph[29];
	SCSubgraphRef Subgraph_S10 = sc.Subgraph[30];
	SCSubgraphRef Subgraph_R4_5 = sc.Subgraph[31];
	SCSubgraphRef Subgraph_S4_5 = sc.Subgraph[32];
	const int NUMBER_OF_STUDY_SUBGRAPHS = 33;

	SCInputRef Input_TimePeriodType = sc.Input[0];
	SCInputRef Input_TimePeriodLength = sc.Input[1];
	SCInputRef Input_FormulaType = sc.Input[2];
	SCInputRef Input_MinimumRequiredTimePeriodAsPercent = sc.Input[3];
	SCInputRef Input_RoundToTickSize = sc.Input[4];
	SCInputRef Input_DisplayDebuggingOutput = sc.Input[5];
	SCInputRef Input_ForwardProjectLines = sc.Input[6];
	SCInputRef Input_NumberOfDaysToCalculate = sc.Input[7];
	SCInputRef Input_AutoSkipPeriodOfNoTrading = sc.Input[8];
	SCInputRef Input_NumberForwardProjectionBars = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "DAS Pivot Points-Variable Period";

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 0;

		sc.DrawStudyUnderneathMainPriceGraph = 1;

		Subgraph_R1.Name = "R1";
		Subgraph_R2.Name = "R2";
		Subgraph_S1.Name = "S1";
		Subgraph_S2.Name = "S2";
		Subgraph_R0_5.Name = "R.5";
		Subgraph_R1_5.Name = "R1.5";
		Subgraph_R2_5.Name = "R2.5";
		Subgraph_R3.Name = "R3";
		Subgraph_S0_5.Name = "S.5";
		Subgraph_S1_5.Name = "S1.5";
		Subgraph_S2_5.Name = "S2.5";
		Subgraph_S3.Name = "S3";
		Subgraph_PP.Name = "PP";
		Subgraph_PPHigh.Name = "PP High";
		Subgraph_PPLow.Name = "PP Low";
		Subgraph_R4.Name = "R4";
		Subgraph_S4.Name = "S4";
		Subgraph_R3_5.Name = "R3.5";
		Subgraph_S3_5.Name = "S3.5";
		Subgraph_R5.Name = "R5";
		Subgraph_S5.Name = "S5";
		Subgraph_R6.Name = "R6";
		Subgraph_S6.Name = "S6";
		Subgraph_R7.Name = "R7";
		Subgraph_S7.Name = "S7";
		Subgraph_R8.Name = "R8";
		Subgraph_S8.Name = "S8";
		Subgraph_R9.Name = "R9";
		Subgraph_S9.Name = "S9";
		Subgraph_R10.Name = "R10";
		Subgraph_S10.Name = "S10";
		Subgraph_R4_5.Name = "R4.5";
		Subgraph_S4_5.Name = "S4.5";

		Subgraph_R1.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R2.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S1.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S2.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R3.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S3.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_PP.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_PPHigh.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_PPLow.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R4.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S4.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R5.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S5.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R6.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S6.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R7.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S7.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R8.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S8.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R9.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S9.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R10.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S10.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_R4_5.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_S4_5.DrawStyle = DRAWSTYLE_HIDDEN;

		Subgraph_R1.LineWidth = 3;
		Subgraph_R2.LineWidth = 3;
		Subgraph_S1.LineWidth = 3;
		Subgraph_S2.LineWidth = 3;
		Subgraph_R3.LineWidth = 3;
		Subgraph_R4.LineWidth = 3;
		Subgraph_R5.LineWidth = 3;
		Subgraph_R6.LineWidth = 3;
		Subgraph_S3.LineWidth = 3;
		Subgraph_S4.LineWidth = 3;
		Subgraph_S5.LineWidth = 3;
		Subgraph_S6.LineWidth = 3;

		Subgraph_R1.UseTransparentLabelBackground = 1;
		Subgraph_R2.UseTransparentLabelBackground = 1;
		Subgraph_S1.UseTransparentLabelBackground = 1;
		Subgraph_S2.UseTransparentLabelBackground = 1;
		Subgraph_R3.UseTransparentLabelBackground = 1;
		Subgraph_R4.UseTransparentLabelBackground = 1;
		Subgraph_R5.UseTransparentLabelBackground = 1;
		Subgraph_R6.UseTransparentLabelBackground = 1;
		Subgraph_S3.UseTransparentLabelBackground = 1;
		Subgraph_S4.UseTransparentLabelBackground = 1;
		Subgraph_S5.UseTransparentLabelBackground = 1;
		Subgraph_S6.UseTransparentLabelBackground = 1;

		Subgraph_R1.DrawZeros = false;
		Subgraph_R2.DrawZeros = false;
		Subgraph_S1.DrawZeros = false;
		Subgraph_S2.DrawZeros = false;
		Subgraph_R0_5.DrawZeros = false;
		Subgraph_R1_5.DrawZeros = false;
		Subgraph_R2_5.DrawZeros = false;
		Subgraph_R3.DrawZeros = false;
		Subgraph_S0_5.DrawZeros = false;
		Subgraph_S1_5.DrawZeros = false;
		Subgraph_S2_5.DrawZeros = false;
		Subgraph_S3.DrawZeros = false;
		Subgraph_PP.DrawZeros = false;
		Subgraph_PPHigh.DrawZeros = false;
		Subgraph_PPLow.DrawZeros = false;
		Subgraph_R4.DrawZeros = false;
		Subgraph_S4.DrawZeros = false;
		Subgraph_R3_5.DrawZeros = false;
		Subgraph_S3_5.DrawZeros = false;
		Subgraph_R5.DrawZeros = false;
		Subgraph_S5.DrawZeros = false;
		Subgraph_R6.DrawZeros = false;
		Subgraph_S6.DrawZeros = false;
		Subgraph_R7.DrawZeros = false;
		Subgraph_S7.DrawZeros = false;
		Subgraph_R8.DrawZeros = false;
		Subgraph_S8.DrawZeros = false;
		Subgraph_R9.DrawZeros = false;
		Subgraph_S9.DrawZeros = false;
		Subgraph_R10.DrawZeros = false;
		Subgraph_S10.DrawZeros = false;
		Subgraph_R4_5.DrawZeros = false;
		Subgraph_S4_5.DrawZeros = false;
		

		Subgraph_PP.PrimaryColor = RGB(255, 0, 255);
		Subgraph_PPHigh.PrimaryColor = RGB(255, 0, 255);
		Subgraph_PPLow.PrimaryColor = RGB(255, 0, 255);

		Subgraph_R1.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R2.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R0_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R1_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R2_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R3.PrimaryColor = RGB(128, 0, 0);
		Subgraph_R4.PrimaryColor = RGB(0, 128, 0);
		Subgraph_R4_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R3_5.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R5.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R6.PrimaryColor = RGB(128, 0, 0);
		Subgraph_R7.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R8.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R9.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R10.PrimaryColor = RGB(255, 0, 0);

		Subgraph_S1.PrimaryColor = RGB(128, 128, 128);
		Subgraph_S2.PrimaryColor = RGB(128, 128, 128);
		Subgraph_S0_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S1_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S2_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S3.PrimaryColor = RGB(0, 128, 0);
		Subgraph_S4.PrimaryColor = RGB(128, 0, 0);
		Subgraph_S4_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S3_5.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S5.PrimaryColor = RGB(128, 128, 128);
		Subgraph_S6.PrimaryColor = RGB(0, 128, 0);
		Subgraph_S7.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S8.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S9.PrimaryColor = RGB(0, 255, 0);
		Subgraph_S10.PrimaryColor = RGB(0, 255, 0);


		for (int Index = 0; Index < NUMBER_OF_STUDY_SUBGRAPHS; Index++)
		{
			sc.Subgraph[Index].LineLabel = 
				// LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				LL_DISPLAY_NAME | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT;
		}

		Input_TimePeriodType.Name = "Time Period Type";
		Input_TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_MINUTES);

		Input_TimePeriodLength.Name = "Time Period Length";
		Input_TimePeriodLength.SetInt(60);
		Input_TimePeriodLength.SetIntLimits(1, 7*MINUTES_PER_DAY);

		Input_FormulaType.Name = "Formula Type";
		Input_FormulaType.SetInt(0);

		Input_MinimumRequiredTimePeriodAsPercent.Name = "Minimum Required Time Period as %";
		Input_MinimumRequiredTimePeriodAsPercent.SetFloat(25.0f);
		Input_MinimumRequiredTimePeriodAsPercent.SetFloatLimits(1.0f, 100.0f);

		Input_RoundToTickSize.Name = "Round to Tick Size";
		Input_RoundToTickSize.SetYesNo(0);

		Input_DisplayDebuggingOutput.Name = "Display Debugging Output (slows study calculations)";
		Input_DisplayDebuggingOutput.SetYesNo(0);

		Input_ForwardProjectLines.Name ="Forward Project Pivot Point Lines";
		Input_ForwardProjectLines.SetYesNo(0);

		Input_NumberOfDaysToCalculate.Name = "Number Of Days To Calculate";
		Input_NumberOfDaysToCalculate.SetInt(1000);

		Input_AutoSkipPeriodOfNoTrading.Name = "Auto Skip Period Of No Trading";
		Input_AutoSkipPeriodOfNoTrading.SetYesNo(false);

		Input_NumberForwardProjectionBars.Name = "Number of Forward Project Bars";
		Input_NumberForwardProjectionBars.SetInt(20);
		Input_NumberForwardProjectionBars.SetIntLimits(1, 200);
		return;
	}

	float f_PivotPoint= 0, f_PivotPointHigh = 0, f_PivotPointLow = 0;

	float fR0_5 = 0, fR1 = 0, fR1_5 = 0, fR2 = 0, fR2_5 = 0, fR3 = 0, fR3_5 = 0, fR4 = 0, fR4_5 = 0, fR5 = 0, fR6 = 0, fR7 = 0, fR8 = 0, fR9 = 0, fR10 = 0;
	
	float fS0_5 = 0, fS1 = 0, fS1_5 = 0, fS2 = 0, fS2_5 = 0, fS3 = 0, fS3_5 = 0, fS4 = 0, fS4_5 = 0, fS5 = 0, fS6 = 0, fS7 = 0, fS8 = 0, fS9 = 0, fS10 = 0;

	int ValidPivotPoint = 1;

	int PeriodLength = Input_TimePeriodLength.GetInt();

	int NumberOfForwardBars = 0;

	if (Input_ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = Input_NumberForwardProjectionBars.GetInt();

		if(sc.UpdateStartIndex == 0)
		{
			Subgraph_PP.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_PPHigh.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_PPLow.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R1.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R2.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S1.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S2.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R0_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R1_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R2_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R3.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S0_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S1_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S2_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S3.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R4.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S4.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R3_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S3_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R6.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S6.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R7.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S7.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R8.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S8.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R9.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S9.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R10.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S10.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R4_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S4_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
		}
	}

	if (Input_NumberOfDaysToCalculate.GetInt() < 1)
		Input_NumberOfDaysToCalculate.SetInt(1000);

	int LastDateInChart = sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate();
	int FirstDateToCalculate = LastDateInChart - Input_NumberOfDaysToCalculate.GetInt() + 1;

	float Open = 0, High = 0, Low = 0, Close = 0, NextOpen = 0;

	SCDateTime BeginOfRefDateTime, EndOfRefDateTime;
	SCDateTime CurrentPeriodBeginDateTime;
	SCDateTime CurrentPeriodEndDateTime;

	SCDateTime PriorCurrentPeriodStartDateTime;

	for (int index = sc.UpdateStartIndex; index < sc.ArraySize+NumberOfForwardBars; index++ )
	{

		SCDateTime CurrentBarDT = sc.BaseDateTimeIn[index];
		

		if (CurrentBarDT < FirstDateToCalculate)
			continue;

		bool GetReferenceData = true;

		CurrentPeriodBeginDateTime = sc.GetStartOfPeriodForDateTime(CurrentBarDT, Input_TimePeriodType.GetTimePeriodType(),  PeriodLength, 0);

		if(	CurrentPeriodBeginDateTime == PriorCurrentPeriodStartDateTime)
			GetReferenceData  = false;


		PriorCurrentPeriodStartDateTime =	CurrentPeriodBeginDateTime;

		SCDateTime TimeIncrement = sc.TimePeriodSpan(Input_TimePeriodType.GetTimePeriodType(),  PeriodLength);

		if(GetReferenceData)
		{

			BeginOfRefDateTime = sc.GetStartOfPeriodForDateTime(CurrentPeriodBeginDateTime, Input_TimePeriodType.GetTimePeriodType(), PeriodLength, -1);

			EndOfRefDateTime = CurrentPeriodBeginDateTime - SCDateTime::MICROSECONDS(1);
			
			CurrentPeriodEndDateTime = sc.GetStartOfPeriodForDateTime(CurrentPeriodBeginDateTime, Input_TimePeriodType.GetTimePeriodType(), PeriodLength, 1) - SCDateTime::MICROSECONDS(1);

			if (Input_DisplayDebuggingOutput.GetYesNo() != 0)
			{
				SCString Message;

				Message.Format("Current Bar: %s, BeginOfRefDateTime: %s, EndOfRefDateTime: %s, CPBeginDateTime: %s, CPEndDateTime: %s",
					sc.FormatDateTimeMS(CurrentBarDT).GetChars()
					, sc.FormatDateTimeMS(BeginOfRefDateTime).GetChars()
					, sc.FormatDateTimeMS(EndOfRefDateTime).GetChars()
					, sc.FormatDateTimeMS(CurrentPeriodBeginDateTime).GetChars()
					, sc.FormatDateTimeMS(CurrentPeriodEndDateTime).GetChars());

				sc.AddMessageToLog(Message,0);

			}

			int MaxPeriodsToGoBack = 1;

			if(Input_AutoSkipPeriodOfNoTrading.GetYesNo())
				MaxPeriodsToGoBack = 32;

			for (int WalkBack = 0; WalkBack < MaxPeriodsToGoBack; WalkBack++)
			{
				if (WalkBack >= 1) //Walk back 1 period.
				{
					SCDateTime PriorBeginOfRefDateTime = BeginOfRefDateTime;

					BeginOfRefDateTime = sc.GetStartOfPeriodForDateTime(BeginOfRefDateTime, Input_TimePeriodType.GetTimePeriodType(), PeriodLength, -1);

					EndOfRefDateTime = PriorBeginOfRefDateTime - SCDateTime::MICROSECONDS(1);

					if (Input_DisplayDebuggingOutput.GetYesNo() != 0)
					{
						SCString Message;

						Message.Format
							( "Moving back 1 period. BeginOfRefDateTime: %s, EndOfRefDateTime: %s."
							, sc.FormatDateTimeMS(BeginOfRefDateTime).GetChars()
							, sc.FormatDateTimeMS(EndOfRefDateTime).GetChars()
							);

						sc.AddMessageToLog(Message,0);
					}
				}

				int NumberOfBars = 0;
				SCDateTime TotalTimeSpan;
				int Result = sc.GetOHLCOfTimePeriod(BeginOfRefDateTime, EndOfRefDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);

				if (!Result)
					continue;

				if (Input_DisplayDebuggingOutput.GetYesNo() != 0)
				{
					SCString Message;
					Message.Format("Number of Bars: %d, Total Time Span In Minutes: %d", NumberOfBars, static_cast<int>(TotalTimeSpan.GetSecondsSinceBaseDate()/SECONDS_PER_MINUTE));
					sc.AddMessageToLog(Message,0);

					Message.Format("RefOpen %f,RefHigh %f,RefLow %f,RefClose %f,RefNextOpen %f.",Open, High, Low, Close, NextOpen);
					sc.AddMessageToLog(Message,0);
				}

				SCDateTime MinimumTimeSpan = (TimeIncrement.GetAsDouble() * Input_MinimumRequiredTimePeriodAsPercent.GetFloat() / 100.0);

				if (TotalTimeSpan >= MinimumTimeSpan)
					break;
			}
		}


		ValidPivotPoint = 
			CalculateDASPivotPoints(Open,High,Low,Close,NextOpen,
			f_PivotPoint, f_PivotPointHigh, f_PivotPointLow, fR0_5, fR1, fR1_5, fR2, fR2_5, fR3, fS0_5,
			fS1, fS1_5, fS2, fS2_5, fS3, fR3_5, fS3_5, fR4, fR4_5, fS4, fS4_5, fR5, fS5,
			fR6, fS6, fR7, fS7, fR8, fS8, fR9, fS9, fR10, fS10, Input_FormulaType.GetInt());


		if (!ValidPivotPoint)
			continue;

		if (Input_RoundToTickSize.GetYesNo() != 0)
		{
			Subgraph_R1[index] = static_cast<float>(sc.RoundToTickSize(fR1, sc.TickSize));
			Subgraph_R2[index] = static_cast<float>(sc.RoundToTickSize(fR2, sc.TickSize));
			Subgraph_S1[index] = static_cast<float>(sc.RoundToTickSize(fS1, sc.TickSize));
			Subgraph_S2[index] = static_cast<float>(sc.RoundToTickSize(fS2, sc.TickSize));

			Subgraph_R0_5[index] = static_cast<float>(sc.RoundToTickSize(fR0_5, sc.TickSize));
			Subgraph_R1_5[index] = static_cast<float>(sc.RoundToTickSize(fR1_5, sc.TickSize));
			Subgraph_R2_5[index] = static_cast<float>(sc.RoundToTickSize(fR2_5, sc.TickSize));
			Subgraph_R3[index] = static_cast<float>(sc.RoundToTickSize(fR3, sc.TickSize));
			Subgraph_S0_5[index] = static_cast<float>(sc.RoundToTickSize(fS0_5, sc.TickSize));
			Subgraph_S1_5[index] = static_cast<float>(sc.RoundToTickSize(fS1_5, sc.TickSize));
			Subgraph_S2_5[index] = static_cast<float>(sc.RoundToTickSize(fS2_5, sc.TickSize));
			Subgraph_S3[index] = static_cast<float>(sc.RoundToTickSize(fS3, sc.TickSize));
			Subgraph_PP[index] = static_cast<float>(sc.RoundToTickSize(f_PivotPoint, sc.TickSize));
			Subgraph_PPHigh[index] = static_cast<float>(sc.RoundToTickSize(f_PivotPointHigh, sc.TickSize));
			Subgraph_PPLow[index] = static_cast<float>(sc.RoundToTickSize(f_PivotPointLow, sc.TickSize));
			Subgraph_R4[index] = static_cast<float>(sc.RoundToTickSize(fR4, sc.TickSize));
			Subgraph_R4_5[index] = static_cast<float>(sc.RoundToTickSize(fR4_5, sc.TickSize));
			Subgraph_S4[index] = static_cast<float>(sc.RoundToTickSize(fS4, sc.TickSize));
			Subgraph_S4_5[index] = static_cast<float>(sc.RoundToTickSize(fS4_5, sc.TickSize));
			Subgraph_R3_5[index] = static_cast<float>(sc.RoundToTickSize(fR3_5, sc.TickSize));
			Subgraph_S3_5[index] = static_cast<float>(sc.RoundToTickSize(fS3_5, sc.TickSize));
			Subgraph_R5[index] = static_cast<float>(sc.RoundToTickSize(fR5, sc.TickSize));
			Subgraph_S5[index] = static_cast<float>(sc.RoundToTickSize(fS5, sc.TickSize));
			Subgraph_R6[index] = static_cast<float>(sc.RoundToTickSize(fR6, sc.TickSize));
			Subgraph_S6[index] = static_cast<float>(sc.RoundToTickSize(fS6, sc.TickSize));
			Subgraph_R7[index] = static_cast<float>(sc.RoundToTickSize(fR7, sc.TickSize));
			Subgraph_S7[index] = static_cast<float>(sc.RoundToTickSize(fS7, sc.TickSize));
			Subgraph_R8[index] = static_cast<float>(sc.RoundToTickSize(fR8, sc.TickSize));
			Subgraph_S8[index] = static_cast<float>(sc.RoundToTickSize(fS8, sc.TickSize));
			Subgraph_R9[index] = static_cast<float>(sc.RoundToTickSize(fR9, sc.TickSize));
			Subgraph_S9[index] = static_cast<float>(sc.RoundToTickSize(fS9, sc.TickSize));
			Subgraph_R10[index] = static_cast<float>(sc.RoundToTickSize(fR10, sc.TickSize));
			Subgraph_S10[index] = static_cast<float>(sc.RoundToTickSize(fS10, sc.TickSize));
		}
		else 
		{
			Subgraph_R1[index] = fR1;
			Subgraph_R2[index] = fR2;
			Subgraph_S1[index] = fS1;
			Subgraph_S2[index] = fS2;

			Subgraph_R0_5[index] = fR0_5;
			Subgraph_R1_5[index] = fR1_5;
			Subgraph_R2_5[index] = fR2_5;
			Subgraph_R3[index] = fR3;
			Subgraph_S0_5[index] = fS0_5;
			Subgraph_S1_5[index] = fS1_5;
			Subgraph_S2_5[index] = fS2_5;
			Subgraph_S3[index] = fS3;
			Subgraph_PP[index] = f_PivotPoint;
			Subgraph_PPHigh[index] = f_PivotPointHigh;
			Subgraph_PPLow[index] = f_PivotPointLow;
			Subgraph_R4[index] = fR4;
			Subgraph_R4_5[index] = fR4_5;
			Subgraph_S4[index] = fS4;
			Subgraph_S4_5[index] = fS4_5;
			Subgraph_R3_5[index] = fR3_5;
			Subgraph_S3_5[index] = fS3_5;
			Subgraph_R5[index] = fR5;
			Subgraph_S5[index] = fS5;
			Subgraph_R6[index] = fR6;
			Subgraph_S6[index] = fS6;
			Subgraph_R7[index] = fR7;
			Subgraph_S7[index] = fS7;
			Subgraph_R8[index] = fR8;
			Subgraph_S8[index] = fS8;
			Subgraph_R9[index] = fR9;
			Subgraph_S9[index] = fS9;
			Subgraph_R10[index] = fR10;
			Subgraph_S10[index] = fS10;
		}
	} 
}
