#include <stdio.h>
#include <stdlib.h>
#include "extract.h"

int main() {
   char* resultString = FindByTopics(
       GET_WLAN, "/home/vienchau/Desktop/ws/extract-module/mqtt_log");
   printf("%s\n", resultString);
   free(resultString);
   return 0;
}