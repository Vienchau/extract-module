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