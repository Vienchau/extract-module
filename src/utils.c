#define __USE_XOPEN
#define _GNU_SOURCE

#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "extract.h"

void print_json(json_t* root) {
   char* temp = json_dumps(root, JSON_INDENT(4));
   printf(" ROOT : %s", temp);
   free(temp);
}

void RemoveChar(char* des, char remove) {
   int j, n = strlen(des);
   int i;
   for (i = j = 0; i < n; i++) {
      if (des[i] != remove) des[j++] = des[i];
   }
   des[j] = '\0';
}

char* ConvertIDTopic(int topic) {
   switch (topic) {
      // case GET_MAC: {
      //    return MAC_CLIENT_TOPIC;
      // }
      case GET_WLAN: {
         return WLAN_CLIENT;
      }
      case GET_LAN: {
         return LAN_CLIENT;
      }
      case GET_INTERFACE: {
         return INTERFACE_STAT;
      }
      case GET_NAP_0: {
         return NAP_0;
      }
      case GET_NAP_1: {
         return NAP_1;
      }
      case GET_IP: {
         return IP_STAT;
      }
      // case GET_REGISTER: {
      //    return REGISTER;
      // }
      case GET_SSIDS: {
         return SSIDS;
      }
      case GET_MEM: {
         return MEM;
      }
      case GET_CPU: {
         return CPU;
      }
      case GET_CHANNEL_USAGE: {
         return CHANNEL_USAGE;
      }
      case ALL: {
         return LOGFILE;
      }
      default: {
         return NULL;
      }
   }
}

int ConvertHoursToSecond(int hour) {
   int second = hour * 60 * 60;
   return second;
}

time_t ConvertDatetoEpoch(int year, int month, int day, int hour) {
   struct tm tm_date = {
       .tm_year = year - 1900,
       .tm_mon = month - 1,
       .tm_mday = day,
       .tm_hour = hour,
       .tm_min = 0,
       .tm_sec = 0,
   };

   return mktime(&tm_date);
}

int GetMaxElementObject(json_t* root) {
   int maxElementCount = 0;
   if (json_is_array(root)) {
      size_t index;
      json_t* value;
      json_array_foreach(root, index, value) {
         maxElementCount = (maxElementCount < (int)json_object_size(value))
                               ? (int)(json_object_size(value))
                               : maxElementCount;
      }
   } else if (json_is_object(root)) {
      const char* key;
      json_t* value;
      json_object_foreach(root, key, value) {
         maxElementCount = (maxElementCount < (int)json_object_size(value))
                               ? (int)(json_object_size(value))
                               : maxElementCount;
      }
   }

   return maxElementCount;
}

void DumpToFile(char* path, json_t* root) {
   FILE* fp = fopen(path, "w");
   if (fp == NULL) {
      fprintf(stderr, "Can't open file %s", path);
   }
   int flag = json_dumpf(root, fp, JSON_INDENT(4));
   fclose(fp);
   if (flag == -1) {
      printf("Failed to dump to file.\n");
   }
   printf("Dump to file: %s\n", path);
}

void CreateInterfaceStat(Interface_Stat* stat, int numberOfRecords) {
   for (int counter = 0; counter < numberOfRecords; counter++) {
      stat[counter].interface = "N/a";
      memset(stat[counter].txBytes, 0,
             ARRAY_MAX_SIZE * sizeof(unsigned long long));
      memset(stat[counter].rxBytes, 0,
             ARRAY_MAX_SIZE * sizeof(unsigned long long));
      memset(stat[counter].txErrors, 0, ARRAY_MAX_SIZE * sizeof(unsigned int));
      memset(stat[counter].rxErrors, 0, ARRAY_MAX_SIZE * sizeof(unsigned int));
      memset(stat[counter].time, NULL, ARRAY_MAX_SIZE);
      stat[counter].lastRecords = 0;
   }
}