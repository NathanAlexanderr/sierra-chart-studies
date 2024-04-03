#include "sierrachart.h"

SCDLLName("NEA - Outside Patterns");

/*==========================================================================*/
// "Outside (Or Equals)" Multiple Bar Pattern -
// Highlights price patterns where the High-Low range of a set of consecutive bars 
// extends beyond or equals the High-Low range of the prior set of bars. This pattern 
// can be useful for detecting momentum, potential trend reversals, or continuation 
// signals in price action.
/*==========================================================================*/

SCSFExport scsf_Outside_Multiple_Bar_Pattern(SCStudyInterfaceRef sc)
{

	SCInputRef i_Inside_Range = sc.Input[0];
	SCInputRef i_Outside_Range = sc.Input[1];

	SCSubgraphRef Subgraph_Top = sc.Subgraph[0];
	SCSubgraphRef Subgraph_Bottom = sc.Subgraph[1];
	
	// Set configuration variables
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "NEA - Outside (Or Equals) Multiple Bar Pattern";
		sc.GraphRegion = 0;

		Subgraph_Top.Name ="Top";
		Subgraph_Top.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_BOTTOM;
		Subgraph_Top.PrimaryColor = RGB(0,110,255);
		Subgraph_Top.DrawZeros = false;

		Subgraph_Bottom.Name ="Bottom";
		Subgraph_Bottom.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_TOP;
		Subgraph_Bottom.PrimaryColor = RGB(0,110,255);
		Subgraph_Bottom.DrawZeros = false;
		
		i_Inside_Range.Name = "Inside Bar Number Range";
		i_Inside_Range.SetInt(5);

		i_Outside_Range.Name = "Outside Bar Number Range";
		i_Outside_Range.SetInt(2);

		sc.AutoLoop = 1;
		sc.DrawStudyUnderneathMainPriceGraph = 1;
		
		return;
	}
	
	// Array references
	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;
	
	double &InsideRangeHigh = sc.GetPersistentDouble(0);
	double &InsideRangeLow = sc.GetPersistentDouble(1);
	double &OutsideRangeHigh = sc.GetPersistentDouble(2);
	double &OutsideRangeLow = sc.GetPersistentDouble(3);

	// **************************
	// Do data processing
	// **************************

	int InsideRange = i_Inside_Range.GetInt();
	int OutsideRange = i_Outside_Range.GetInt();

	// Set Inside Range variables (High & Low)
	
	// set InsideRange H & L to first bar H & L
	InsideRangeHigh = High[sc.Index - (InsideRange + OutsideRange - 1)];
	InsideRangeLow = Low[sc.Index - (InsideRange + OutsideRange - 1)];

	for (int i = (2 - InsideRange - OutsideRange); i < (1 - OutsideRange); i++) {

		// if any other bar has High > InsideBarHigh
		if (InsideRangeHigh < High[sc.Index + i]) {
			InsideRangeHigh = High[sc.Index + i];
		}
		// if any other bar has Low < InsideBarLow
		if (InsideRangeLow > Low[sc.Index + i]) {
			InsideRangeLow = Low[sc.Index + i];
		}
	}

	// Set Outside Range variables to first Outside Range Bar High & Low
	OutsideRangeHigh = High[sc.Index + 1 - OutsideRange];
	OutsideRangeLow = Low[sc.Index + 1 - OutsideRange];

	for (int i = (2 - OutsideRange); i < 1; i++) {

		if (OutsideRangeHigh < High[sc.Index + i]) {
				OutsideRangeHigh = High[sc.Index];
		}
		if (OutsideRangeLow > Low[sc.Index + i]) {
				OutsideRangeLow = Low[sc.Index];
		}
	}

	// If Inside Range H <= Outside Range H && Inside Range L >= Outside Range L
	if (InsideRangeHigh <= OutsideRangeHigh && InsideRangeLow >= OutsideRangeLow) {
		// then highlight the Outside Range Bars
		// Iterate through OutsideRange bars...
		for (int i = (1 - OutsideRange); i < 1; i++) {
			Subgraph_Top[sc.Index + i] = OutsideRangeHigh;
			Subgraph_Bottom[sc.Index + i] = OutsideRangeLow;
		}

		return;
	}
	else {
		Subgraph_Top[sc.Index] = 0;
		Subgraph_Bottom[sc.Index] = 0;
	}
} // That's All Folks!