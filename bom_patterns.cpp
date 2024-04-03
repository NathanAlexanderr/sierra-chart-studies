#include "sierrachart.h"

SCDLLName("NEA - BOM Patterns");

/*==========================================================================*/
// Highlights 2 Bar BOM Patterns - ii, OO, oi, io
/*==========================================================================*/
SCSFExport scsf_NEA_BOM_Patterns(SCStudyInterfaceRef sc)
{
	//SCSubgraphRef Subgraph_IB = sc.Subgraph[0];
	SCSubgraphRef Subgraph_Top = sc.Subgraph[0];
	SCSubgraphRef Subgraph_Bottom = sc.Subgraph[1];
	
	// Set configuration variables
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "NEA - BOM Patterns";
		sc.GraphRegion = 0;

		Subgraph_Top.Name ="Top";
		Subgraph_Top.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_BOTTOM;
		Subgraph_Top.PrimaryColor = RGB(255,105,255);
		Subgraph_Top.DrawZeros = false;

		Subgraph_Bottom.Name ="Bottom";
		Subgraph_Bottom.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_TOP;
		Subgraph_Bottom.PrimaryColor = RGB(255,105,255);
		Subgraph_Bottom.DrawZeros = false;
		
		sc.AutoLoop = 1;
		sc.DrawStudyUnderneathMainPriceGraph = 1;
		
		return;
	}
	
	// Array references
	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;
	
	double &PatternHigh = sc.GetPersistentDouble(0);
	double &PatternLow = sc.GetPersistentDouble(1);

	// *******************************
	// Inside-Inside (ii) Pattern
	// *******************************

	// If First bar H is >= Second bar H && First bar L is <= Second bar L
	if (High[sc.Index - 1] <= High[sc.Index - 2] && Low[sc.Index - 1] >= Low[sc.Index - 2]) {

		// If Second bar H is >= Third bar H && Second bar L is <= Third bar L
		if (High[sc.Index] <= High[sc.Index - 1] && Low[sc.Index] >= Low[sc.Index - 1]) {

			// set High and Low of Pattern
			PatternHigh = High[sc.Index - 1];
			PatternLow = Low[sc.Index - 1];

			Subgraph_Top[sc.Index - 1] = PatternHigh;
			Subgraph_Bottom[sc.Index - 1] = PatternLow;			
			Subgraph_Top[sc.Index] = PatternHigh;
			Subgraph_Bottom[sc.Index] = PatternLow;

			return;
		}
		else {
			Subgraph_Top[sc.Index] = 0;
			Subgraph_Bottom[sc.Index] = 0;
		}
	}
	else {
		Subgraph_Top[sc.Index] = 0;
		Subgraph_Bottom[sc.Index] = 0;
	}

	// *******************************
	// Outside-Outside (OO) Pattern
	// *******************************

	// If First bar H is <= Second bar H && First bar L is >= Second bar L
	if (High[sc.Index - 1] >= High[sc.Index - 2] && Low[sc.Index - 1] <= Low[sc.Index - 2]) {

		// If Second bar H is <= Third bar H && Second bar L is >= Third bar L
		if (High[sc.Index] >= High[sc.Index - 1] && Low[sc.Index] <= Low[sc.Index - 1]) {

			// set High and Low of Pattern
			PatternHigh = High[sc.Index];
			PatternLow = Low[sc.Index];

			Subgraph_Top[sc.Index - 1] = PatternHigh;
			Subgraph_Bottom[sc.Index - 1] = PatternLow;			
			Subgraph_Top[sc.Index] = PatternHigh;
			Subgraph_Bottom[sc.Index] = PatternLow;

			return;
		}
		else {
			Subgraph_Top[sc.Index] = 0;
			Subgraph_Bottom[sc.Index] = 0;
		}
	}
	else {
		Subgraph_Top[sc.Index] = 0;
		Subgraph_Bottom[sc.Index] = 0;
	}

	// *******************************
	// Outside-Inside (oi) Pattern
	// *******************************

	// If First bar H is <= Second bar H && First bar L is >= Second bar L
	if (High[sc.Index - 1] >= High[sc.Index - 2] && Low[sc.Index - 1] <= Low[sc.Index - 2]) {

		// If Second bar H is >= Third bar H && Second bar L is <= Third bar L
		if (High[sc.Index] <= High[sc.Index - 1] && Low[sc.Index] >= Low[sc.Index - 1]) {

			// set High and Low of Pattern to Second bar (Outside bar)
			PatternHigh = High[sc.Index - 1];
			PatternLow = Low[sc.Index - 1];

			Subgraph_Top[sc.Index - 1] = PatternHigh;
			Subgraph_Bottom[sc.Index - 1] = PatternLow;			
			Subgraph_Top[sc.Index] = PatternHigh;
			Subgraph_Bottom[sc.Index] = PatternLow;

			return;
		}
		else {
			Subgraph_Top[sc.Index] = 0;
			Subgraph_Bottom[sc.Index] = 0;
		}
	}
	else {
		Subgraph_Top[sc.Index] = 0;
		Subgraph_Bottom[sc.Index] = 0;
	}

	// *******************************
	// Inside-Outside (io) Pattern
	// *******************************

	// If First bar H is >= Second bar H && First bar L is <= Second bar L
	if (High[sc.Index - 1] <= High[sc.Index - 2] && Low[sc.Index - 1] >= Low[sc.Index - 2]) {

		// If Second bar H is <= Third bar H && Second bar L is >= Third bar L
		if (High[sc.Index] >= High[sc.Index - 1] && Low[sc.Index] <= Low[sc.Index - 1]) {

			// set High and Low of Pattern
			PatternHigh = High[sc.Index];
			PatternLow = Low[sc.Index];

			Subgraph_Top[sc.Index - 1] = PatternHigh;
			Subgraph_Bottom[sc.Index - 1] = PatternLow;			
			Subgraph_Top[sc.Index] = PatternHigh;
			Subgraph_Bottom[sc.Index] = PatternLow;

			return;
		}
		else {
			Subgraph_Top[sc.Index] = 0;
			Subgraph_Bottom[sc.Index] = 0;
		}
	}
	else {
		Subgraph_Top[sc.Index] = 0;
		Subgraph_Bottom[sc.Index] = 0;
	}
}
