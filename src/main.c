#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "extract.h"

int main()
{
   /* Tue, 06 Dec 2022 00:00:00 GMT+7, 5 hours range */
   time_t time = ConvertDatetoEpoch(2022, 12, 13, 0);
   int range = ConvertHoursToSecond(24);
   json_t *filtering = ExtractData(time, range, GET_INTERFACE);
   DumpToFile("result.json", filtering);
   json_decref(filtering);
   return 0;
}