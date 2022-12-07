#include <stdio.h>
#include <stdlib.h>
#include "extract.h"

#define LOG_PATH \
   "/home/vienchau/Desktop/extract_module/extract-module/log/mqtt_log"

// TODO: string to json transform
int main() {
   /* Tue, 06 Dec 2022 02:20:37 GMT+7, 3 hours range */
   char* resul =
       FindByTopicsAndTimestamp(GET_INTERFACE, 1670268037, 10800, LOG_PATH);
   printf("%s\n", resul);
   free(resul);

   return 0;
}