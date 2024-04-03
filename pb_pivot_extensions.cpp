#include "sierrachart.h"
#include "SCStudyFunctions.h"

SCDLLName("PivotBoss Pivot Extensions");


/*==========================================================================*/
int CalculatePBPivotExtensions
( float PriorOpen
, float PriorHigh
, float PriorLow
, float PriorClose
, float CurrentOpen
, float& PivotPoint
, float& TopPivot
, float& BottomPivot
, float& R25
, float& R50
, float& R75
, float& R100
, float& S25
, float& S50
, float& S75
, float& S100
, int FormulaType
)
{
	if (FormulaType == 0) // PivotBoss Pivot Extensions
	{
		float Range = PriorHigh - PriorLow;
		PivotPoint = (PriorHigh + PriorLow + PriorClose) / 3;
		BottomPivot = (PriorHigh + PriorLow) / 2;
		TopPivot = PivotPoint + (PivotPoint - BottomPivot);

		R25 = PriorHigh + Range * .25f;
		R50 = PriorHigh + Range * .5f;
		R75 = PriorHigh + Range * .75f;
		R100 = PriorHigh + Range;

		S25 = PriorLow - Range * .25f;
		S50 = PriorLow - Range * .5f;
		S75 = PriorLow - Range * .75f;
		S100 = PriorLow - Range;
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
int CalculatePBDailyPivotExtensions
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
, float& TopPivot
, float& BottomPivot
, float& R25
, float& R50
, float& R75
, float& R100
, float& S25
, float& S50
, float& S75
, float& S100
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

	return CalculatePBPivotExtensions
		(  Open
		,  High
		,  Low
		,  Close
		,  NextOpen
		,  PivotPoint
		,  TopPivot
		,  BottomPivot
		,  R25
		,  R50
		,  R75
		,  R100
		,  S25
		,  S50
		,  S75
		,  S100
		,  FormulaType
		);
}


/*==========================================================================*/
SCSFExport scsf_PBPivotExtensionsDaily(SCStudyInterfaceRef sc)
{

	SCSubgraphRef Subgraph_PP = sc.Subgraph[0];
	SCSubgraphRef Subgraph_TP = sc.Subgraph[1];
	SCSubgraphRef Subgraph_BP = sc.Subgraph[2];
	SCSubgraphRef Subgraph_PrevHigh = sc.Subgraph[3];
	SCSubgraphRef Subgraph_PrevLow = sc.Subgraph[4];	
	SCSubgraphRef Subgraph_R25 = sc.Subgraph[5];
	SCSubgraphRef Subgraph_R50 = sc.Subgraph[6];
	SCSubgraphRef Subgraph_R75 = sc.Subgraph[7];
	SCSubgraphRef Subgraph_R100 = sc.Subgraph[8];
	SCSubgraphRef Subgraph_S25 = sc.Subgraph[9];
	SCSubgraphRef Subgraph_S50 = sc.Subgraph[10];
	SCSubgraphRef Subgraph_S75 = sc.Subgraph[11];
	SCSubgraphRef Subgraph_S100 = sc.Subgraph[12];
	
	const int NUMBER_OF_STUDY_SUBGRAPHS = 13;
	
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

		sc.GraphName = "PivotBoss Pivot Extensions-Daily";
		
		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		
		sc.DrawStudyUnderneathMainPriceGraph = 1;

		Subgraph_PP.Name = "PP";
		Subgraph_TP.Name = "TP";
		Subgraph_BP.Name = "BP";
		//Subgraph_PrevHigh.Name = "pH";
		//Subgraph_PrevLow.Name = "pL";		
		Subgraph_R25.Name = "R25";
		Subgraph_R50.Name = "R50";
		Subgraph_R75.Name = "R75";
		Subgraph_R100.Name = "R100";
		Subgraph_S25.Name = "S25";
		Subgraph_S50.Name = "S50";
		Subgraph_S75.Name = "S75";
		Subgraph_S100.Name = "S100";

		Subgraph_PP.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_TP.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_BP.DrawStyle = DRAWSTYLE_HIDDEN;
		//Subgraph_PrevHigh.DrawStyle = DRAWSTYLE_DASH;
		//Subgraph_PrevLow.DrawStyle = DRAWSTYLE_DASH;		
		Subgraph_R25.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R50.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R75.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R100.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S25.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S50.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S75.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S100.DrawStyle = DRAWSTYLE_DASH;

		Subgraph_PP.LineWidth = 3;
		Subgraph_TP.LineWidth = 3;
		Subgraph_BP.LineWidth = 3;
		//Subgraph_PrevHigh.LineWidth = 3;
		//Subgraph_PrevLow.LineWidth = 3;		
		Subgraph_R25.LineWidth = 3;
		Subgraph_R50.LineWidth = 3;
		Subgraph_R75.LineWidth = 3;
		Subgraph_R100.LineWidth = 3;
		Subgraph_S25.LineWidth = 3;
		Subgraph_S50.LineWidth = 3;
		Subgraph_S75.LineWidth = 3;
		Subgraph_S100.LineWidth = 3;

		Subgraph_PP.UseTransparentLabelBackground = 1;
		Subgraph_TP.UseTransparentLabelBackground = 1;
		Subgraph_BP.UseTransparentLabelBackground = 1;
		//Subgraph_PrevHigh.UseTransparentLabelBackground = 1;
		//Subgraph_PrevLow.UseTransparentLabelBackground = 1;
		Subgraph_R25.UseTransparentLabelBackground = 1;
		Subgraph_R50.UseTransparentLabelBackground = 1;
		Subgraph_R75.UseTransparentLabelBackground = 1;
		Subgraph_R100.UseTransparentLabelBackground = 1;
		Subgraph_S25.UseTransparentLabelBackground = 1;
		Subgraph_S50.UseTransparentLabelBackground = 1;
		Subgraph_S75.UseTransparentLabelBackground = 1;
		Subgraph_S100.UseTransparentLabelBackground = 1;

		Subgraph_PP.DrawZeros = false;
		Subgraph_TP.DrawZeros = false;
		Subgraph_BP.DrawZeros = false;
		//Subgraph_PrevHigh.DrawZeros = false;
		//Subgraph_PrevLow.DrawZeros = false;
		Subgraph_R25.DrawZeros = false;
		Subgraph_R50.DrawZeros = false;
		Subgraph_R75.DrawZeros = false;
		Subgraph_R100.DrawZeros = false;
		Subgraph_S25.DrawZeros = false;
		Subgraph_S50.DrawZeros = false;
		Subgraph_S75.DrawZeros = false;
		Subgraph_S100.DrawZeros = false;

		Subgraph_PP.PrimaryColor = RGB(255, 0, 255);
		Subgraph_TP.PrimaryColor = RGB(255, 0, 255);
		Subgraph_BP.PrimaryColor = RGB(255, 0, 255);

		//Subgraph_PrevHigh.PrimaryColor = RGB(0, 0, 200);
		//Subgraph_PrevLow.PrimaryColor = RGB(0, 0, 200);

		Subgraph_R25.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R50.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R75.PrimaryColor = RGB(255, 0, 0);
		Subgraph_R100.PrimaryColor = RGB(255, 0, 0);
		Subgraph_S25.PrimaryColor = RGB(255, 0, 0);
		Subgraph_S50.PrimaryColor = RGB(128, 0, 0);
		Subgraph_S75.PrimaryColor = RGB(0, 128, 0);
		Subgraph_S100.PrimaryColor = RGB(255, 0, 0);


		for (int SubgraphIndex = 0; SubgraphIndex < NUMBER_OF_STUDY_SUBGRAPHS; SubgraphIndex++)
		{
			//sc.Subgraph[SubgraphIndex].LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
			sc.Subgraph[SubgraphIndex].LineLabel = LL_DISPLAY_NAME | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT;
		}

		sc.Input[3].SetChartNumber(1);

		Input_DailyChartNumber.Name = "Daily Chart Number";
		Input_DailyChartNumber.SetChartNumber(1);

		Input_FormulaType.Name = "Formula Type (Use 0)";
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

	float fPivotPoint = 0, fTopPivot = 0, fBottomPivot = 0;

	float fR25 = 0, fR50 = 0, fR75 = 0, fR100 = 0;
	float fS25 = 0, fS50 = 0, fS75 = 0, fS100 = 0;	

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
				CalculatePBDailyPivotExtensions(
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
					fTopPivot,
					fBottomPivot,
					fR25,
					fR50,
					fR75,
					fR100,
					fS25,
					fS50,
					fS75,
					fS100,
					Input_UseDailyChartForSettlementOnly.GetYesNo()
				);
		}

		if (!ValidPivotPoint)
			continue;
		
		if (Input_RoundToTickSize.GetYesNo() != 0)
		{
			Subgraph_PP[Index] = static_cast<float>(sc.RoundToTickSize(fPivotPoint, sc.TickSize));
			Subgraph_TP[Index] = static_cast<float>(sc.RoundToTickSize(fTopPivot, sc.TickSize));
			Subgraph_BP[Index] = static_cast<float>(sc.RoundToTickSize(fBottomPivot, sc.TickSize));

			Subgraph_R25[Index] = static_cast<float>(sc.RoundToTickSize(fR25, sc.TickSize));
			Subgraph_R50[Index] = static_cast<float>(sc.RoundToTickSize(fR50, sc.TickSize));
			Subgraph_R75[Index] = static_cast<float>(sc.RoundToTickSize(fR75, sc.TickSize));
			Subgraph_R100[Index] = static_cast<float>(sc.RoundToTickSize(fR100, sc.TickSize));
			Subgraph_S25[Index] = static_cast<float>(sc.RoundToTickSize(fS25, sc.TickSize));
			Subgraph_S50[Index] = static_cast<float>(sc.RoundToTickSize(fS50, sc.TickSize));
			Subgraph_S75[Index] = static_cast<float>(sc.RoundToTickSize(fS75, sc.TickSize));
			Subgraph_S100[Index] = static_cast<float>(sc.RoundToTickSize(fS100, sc.TickSize));
			
		}
		else 
		{
			Subgraph_PP[Index] = fPivotPoint;
			Subgraph_TP[Index] = fTopPivot;
			Subgraph_BP[Index] = fBottomPivot;

			Subgraph_R25[Index] = fR25;
			Subgraph_R50[Index] = fR50;
			Subgraph_R75[Index] = fR75;
			Subgraph_R100[Index] = fR100;
			Subgraph_S25[Index] = fS25;
			Subgraph_S50[Index] = fS50;
			Subgraph_S75[Index] = fS75;
			Subgraph_S100[Index] = fS100;
		}
	} 
}


/*==========================================================================*/

SCSFExport scsf_PBPivotExtensionsVariablePeriod(SCStudyInterfaceRef sc)
{
	
	SCSubgraphRef Subgraph_PP = sc.Subgraph[0];
	SCSubgraphRef Subgraph_TP = sc.Subgraph[1];
	SCSubgraphRef Subgraph_BP = sc.Subgraph[2];
	SCSubgraphRef Subgraph_PrevHigh = sc.Subgraph[3];
	SCSubgraphRef Subgraph_PrevLow = sc.Subgraph[4];	
	SCSubgraphRef Subgraph_PrevClose = sc.Subgraph[5];	
	SCSubgraphRef Subgraph_R25 = sc.Subgraph[6];
	SCSubgraphRef Subgraph_R50 = sc.Subgraph[7];
	SCSubgraphRef Subgraph_R75 = sc.Subgraph[8];
	SCSubgraphRef Subgraph_R100 = sc.Subgraph[9];
	SCSubgraphRef Subgraph_S25 = sc.Subgraph[10];
	SCSubgraphRef Subgraph_S50 = sc.Subgraph[11];
	SCSubgraphRef Subgraph_S75 = sc.Subgraph[12];
	SCSubgraphRef Subgraph_S100 = sc.Subgraph[13];
	
	const int NUMBER_OF_STUDY_SUBGRAPHS = 14;

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
		sc.GraphName = "PivotBoss Pivot Extensions-Variable Period";

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 0;

		sc.DrawStudyUnderneathMainPriceGraph = 1;

		Subgraph_PP.Name = "PP";
		Subgraph_TP.Name = "TP";
		Subgraph_BP.Name = "BP";
		Subgraph_PrevHigh.Name = "pH";
		Subgraph_PrevLow.Name = "pL";
		Subgraph_PrevClose.Name = "pC";
		Subgraph_R25.Name = "R25";
		Subgraph_R50.Name = "R50";
		Subgraph_R75.Name = "R75";
		Subgraph_R100.Name = "R100";
		Subgraph_S25.Name = "S25";
		Subgraph_S50.Name = "S50";
		Subgraph_S75.Name = "S75";
		Subgraph_S100.Name = "S100";

		Subgraph_PP.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_TP.DrawStyle = DRAWSTYLE_HIDDEN;
		Subgraph_BP.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_PrevHigh.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_PrevLow.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_PrevClose.DrawStyle = DRAWSTYLE_DASH;		
		Subgraph_R25.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R50.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R75.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_R100.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S25.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S50.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S75.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_S100.DrawStyle = DRAWSTYLE_DASH;

		Subgraph_PP.LineWidth = 3;
		Subgraph_TP.LineWidth = 3;
		Subgraph_BP.LineWidth = 3;
		Subgraph_PrevHigh.LineWidth = 3;
		Subgraph_PrevLow.LineWidth = 3;
		Subgraph_PrevClose.LineWidth = 3;				
		Subgraph_R25.LineWidth = 3;
		Subgraph_R50.LineWidth = 3;
		Subgraph_R75.LineWidth = 3;
		Subgraph_R100.LineWidth = 3;
		Subgraph_S25.LineWidth = 3;
		Subgraph_S50.LineWidth = 3;
		Subgraph_S75.LineWidth = 3;
		Subgraph_S100.LineWidth = 3;

		Subgraph_PP.UseTransparentLabelBackground = 1;
		Subgraph_TP.UseTransparentLabelBackground = 1;
		Subgraph_BP.UseTransparentLabelBackground = 1;
		Subgraph_PrevHigh.UseTransparentLabelBackground = 1;
		Subgraph_PrevLow.UseTransparentLabelBackground = 1;
		Subgraph_PrevClose.UseTransparentLabelBackground = 1;
		Subgraph_R25.UseTransparentLabelBackground = 1;
		Subgraph_R50.UseTransparentLabelBackground = 1;
		Subgraph_R75.UseTransparentLabelBackground = 1;
		Subgraph_R100.UseTransparentLabelBackground = 1;
		Subgraph_S25.UseTransparentLabelBackground = 1;
		Subgraph_S50.UseTransparentLabelBackground = 1;
		Subgraph_S75.UseTransparentLabelBackground = 1;
		Subgraph_S100.UseTransparentLabelBackground = 1;

		Subgraph_PP.DrawZeros = false;
		Subgraph_TP.DrawZeros = false;
		Subgraph_BP.DrawZeros = false;
		Subgraph_PrevHigh.DrawZeros = false;
		Subgraph_PrevLow.DrawZeros = false;
		Subgraph_PrevClose.DrawZeros = false;
		Subgraph_R25.DrawZeros = false;
		Subgraph_R50.DrawZeros = false;
		Subgraph_R75.DrawZeros = false;
		Subgraph_R100.DrawZeros = false;
		Subgraph_S25.DrawZeros = false;
		Subgraph_S50.DrawZeros = false;
		Subgraph_S75.DrawZeros = false;
		Subgraph_S100.DrawZeros = false;

		Subgraph_PP.PrimaryColor = RGB(255, 0, 255);
		Subgraph_TP.PrimaryColor = RGB(255, 0, 255);
		Subgraph_BP.PrimaryColor = RGB(170, 0, 170);

		Subgraph_PrevHigh.PrimaryColor = RGB(0, 0, 200);
		Subgraph_PrevLow.PrimaryColor = RGB(0, 0, 200);
		Subgraph_PrevClose.PrimaryColor = RGB(0, 0, 200);		

		Subgraph_R25.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R50.PrimaryColor = RGB(128, 128, 128);
		Subgraph_R75.PrimaryColor = RGB(150, 0, 0);
		Subgraph_R100.PrimaryColor = RGB(150, 0, 0);
		Subgraph_S25.PrimaryColor = RGB(128, 128, 128);
		Subgraph_S50.PrimaryColor = RGB(128, 128, 128);
		Subgraph_S75.PrimaryColor = RGB(128, 0, 0);
		Subgraph_S100.PrimaryColor = RGB(128, 0, 0);


		for (int SubgraphIndex = 0; SubgraphIndex < NUMBER_OF_STUDY_SUBGRAPHS; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex].LineLabel = 
				// LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				LL_DISPLAY_NAME | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT;
		}

		Input_TimePeriodType.Name = "Time Period Type";
		Input_TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_MINUTES);

		Input_TimePeriodLength.Name = "Time Period Length";
		Input_TimePeriodLength.SetInt(60);
		Input_TimePeriodLength.SetIntLimits(1, 7*MINUTES_PER_DAY);

		Input_FormulaType.Name = "Formula Type (Use 0)";
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

	float fPivotPoint = 0, fTopPivot = 0, fBottomPivot = 0;

	float fR25 = 0, fR50 = 0, fR75 = 0, fR100 = 0;
	float fS25 = 0, fS50 = 0, fS75 = 0, fS100 = 0;	

	int ValidPivotPoint = 1;

	int PeriodLength = Input_TimePeriodLength.GetInt();

	int NumberOfForwardBars = 0;

	if (Input_ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = Input_NumberForwardProjectionBars.GetInt();

		if(sc.UpdateStartIndex == 0)
		{
			Subgraph_PP.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_TP.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_BP.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_PrevHigh.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_PrevLow.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_PrevClose.ExtendedArrayElementsToGraph = NumberOfForwardBars;			
			Subgraph_R25.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R50.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R75.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_R100.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S25.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S50.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S75.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Subgraph_S100.ExtendedArrayElementsToGraph = NumberOfForwardBars;
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

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize+NumberOfForwardBars; Index++ )
	{

		SCDateTime CurrentBarDT = sc.BaseDateTimeIn[Index];
		

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
			CalculatePBPivotExtensions(Open,High,Low,Close,NextOpen,
			fPivotPoint, fTopPivot, fBottomPivot, fR25, fR50, fR75, fR100, 
			fS25, fS50, fS75, fS100, Input_FormulaType.GetInt());


		if (!ValidPivotPoint)
			continue;

		if (Input_RoundToTickSize.GetYesNo() != 0)
		{
			Subgraph_PP[Index] = static_cast<float>(sc.RoundToTickSize(fPivotPoint, sc.TickSize));
			Subgraph_TP[Index] = static_cast<float>(sc.RoundToTickSize(fTopPivot, sc.TickSize));
			Subgraph_BP[Index] = static_cast<float>(sc.RoundToTickSize(fBottomPivot, sc.TickSize));

			Subgraph_PrevHigh[Index] = static_cast<float>(sc.RoundToTickSize(High, sc.TickSize));
			Subgraph_PrevLow[Index] = static_cast<float>(sc.RoundToTickSize(Low, sc.TickSize));			
			Subgraph_PrevClose[Index] = static_cast<float>(sc.RoundToTickSize(Close, sc.TickSize));			

			Subgraph_R25[Index] = static_cast<float>(sc.RoundToTickSize(fR25, sc.TickSize));
			Subgraph_R50[Index] = static_cast<float>(sc.RoundToTickSize(fR50, sc.TickSize));
			Subgraph_R75[Index] = static_cast<float>(sc.RoundToTickSize(fR75, sc.TickSize));
			Subgraph_R100[Index] = static_cast<float>(sc.RoundToTickSize(fR100, sc.TickSize));
			Subgraph_S25[Index] = static_cast<float>(sc.RoundToTickSize(fS25, sc.TickSize));
			Subgraph_S50[Index] = static_cast<float>(sc.RoundToTickSize(fS50, sc.TickSize));
			Subgraph_S75[Index] = static_cast<float>(sc.RoundToTickSize(fS75, sc.TickSize));
			Subgraph_S100[Index] = static_cast<float>(sc.RoundToTickSize(fS100, sc.TickSize));
		}
		else 
		{
			Subgraph_PP[Index] = fPivotPoint;
			Subgraph_TP[Index] = fTopPivot;
			Subgraph_BP[Index] = fBottomPivot;

			Subgraph_PrevHigh[Index] = High;
			Subgraph_PrevLow[Index] = Low;
			Subgraph_PrevClose[Index] = Close;

			Subgraph_R25[Index] = fR25;
			Subgraph_R50[Index] = fR50;
			Subgraph_R75[Index] = fR75;
			Subgraph_R100[Index] = fR100;
			Subgraph_S25[Index] = fS25;
			Subgraph_S50[Index] = fS50;
			Subgraph_S75[Index] = fS75;
			Subgraph_S100[Index] = fS100;
		}
	}
}