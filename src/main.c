#include <stdio.h>
#include <stdlib.h>
#include "extract.h"

int main() {
   /* Tue, 06 Dec 2022 02:20:37 GMT+7, 3 hours range */
   char* resul = FindByTopicsAndTimestamp(
       GET_INTERFACE, 1670268037, 10800,
       "/home/vienchau/Desktop/ws/extract-module/mqtt_log");
   printf("%s\n", resul);
   free(resul);

   return 0;
}