#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "extract.h"

// TODO: string to json transform
int main() {
   /* Tue, 06 Dec 2022 00:00:00 GMT+7, 5 hours range */
   ExtractInterfaceData(ConvertDatetoEpoch(2022, 12, 5, 23),
                        ConvertHoursToSecond(10));
   return 0;
}