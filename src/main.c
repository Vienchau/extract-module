#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "extract.h"

int main()
{
   /* Tue, 06 Dec 2022 00:00:00 GMT+7, 5 hours range */
   // ExtractWlanData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
   // ExtractInterfaceData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
   // ExtractChannelUsageData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
   ExtractAllData();
   return 0;
}