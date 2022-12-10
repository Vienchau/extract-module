#include "extract.h"

#define LOG_PATH "./log/mqtt_log"

char* ExtractInterfaceData(time_t time, int range) {

   int maxElementCount = 0;

   json_t* resul =
       FindByTopicsAndTimestamp(GET_INTERFACE, time, range, LOG_PATH);
   if (resul == NULL) {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   json_t* returnArray = json_object_get(resul, "Retturn");
   if (returnArray == NULL) {
      printf("ERROR: Could not parse return array\n");
      return NULL;
   }

   maxElementCount = GetMaxElementObject(returnArray) - 1;

   Interface_Stat interface_array[maxElementCount];
   CreateInterfaceStat(interface_array, maxElementCount);

   for (int i = 0; i < maxElementCount; i++) {
      printf("%d: \n", i);
      printf(" %s\n", interface_array[i].interface);
      for (int j = 0; j < ARRAY_MAX_SIZE; j++) {
         printf(" %lld", interface_array[i].txBytes[j]);
         printf(" %lld", interface_array[i].rxBytes[j]);
         printf(" %d", interface_array[i].txErrors[j]);
         printf(" %d", interface_array[i].rxErrors[j]);
      }
      printf("\n");
   }

   /* Array for each implement */
   //    size_t index;
   //    json_t* value;
   //    json_array_foreach(returnArray, index, value) {
   //       const char* key;
   //       json_t* value_object;
   //       json_object_foreach(returnArray, key, value_object) {

   //       }
   //    }

   json_decref(returnArray);
   json_decref(resul);
   return NULL;
}
