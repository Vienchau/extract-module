#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "extract.h"

#define LOG_PATH "/home/vienchau/Desktop/ws/extract-module/log/mqtt_log"

// TODO: string to json transform
int main() {
   /* Tue, 06 Dec 2022 00:00:00 GMT+7, 5 hours range */
   char* resul = FindByTopicsAndTimestamp(GET_CHANNEL_USAGE,
                                          ConvertDatetoEpoch(2022, 12, 6, 0),
                                          ConvertHoursToSecond(1), LOG_PATH);
   if (resul == NULL) {
      fprintf(stderr, "ERROR: Could not find log file\n");
   } else {
      printf("%s\n", resul);
   }
   free(resul);
   return 0;
}