#include <stdlib.h>
#include <string.h>
#include "extract.h"

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
      case GET_MAC: {
         return MAC_CLIENT_TOPIC;
      }
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
      case GET_REGISTER: {
         return REGISTER;
      }
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
      default: {
         return NULL;
      }
   }
}

long long ConvertTimeToEpoch() {}