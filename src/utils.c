#define __USE_XOPEN
#define _GNU_SOURCE

#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "extract.h"

void print_json(json_t *root)
{
   char *temp = json_dumps(root, 0);
   printf(" ROOT : %s", temp);
   free(temp);
}

void RemoveChar(char *des, char remove)
{
   int j, n = strlen(des);
   int i;
   for (i = j = 0; i < n; i++)
   {
      if (des[i] != remove)
         des[j++] = des[i];
   }
   des[j] = '\0';
}

char *ConvertIDTopic(int topic)
{
   switch (topic)
   {
   // case GET_MAC: {
   //    return MAC_CLIENT_TOPIC;
   // }
   case GET_WLAN:
   {
      return WLAN_CLIENT;
   }
   case GET_LAN:
   {
      return LAN_CLIENT;
   }
   case GET_INTERFACE:
   {
      return INTERFACE_STAT;
   }
   case GET_NAP_0:
   {
      return NAP_0;
   }
   case GET_NAP_1:
   {
      return NAP_1;
   }
   case GET_IP:
   {
      return IP_STAT;
   }
   // case GET_REGISTER: {
   //    return REGISTER;
   // }
   case GET_SSIDS:
   {
      return SSIDS;
   }
   case GET_MEM:
   {
      return MEM;
   }
   case GET_CPU:
   {
      return CPU;
   }
   case GET_CHANNEL_USAGE:
   {
      return CHANNEL_USAGE;
   }
   case ALL:
   {
      return LOGFILE;
   }
   default:
   {
      return NULL;
   }
   }
}

int ConvertHoursToSecond(int hour)
{
   int second = hour * 60 * 60;
   return second;
}

time_t ConvertDatetoEpoch(int year, int month, int day, int hour)
{
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

int GetMaxElementObject(json_t *root)
{
   int maxElementCount = 0;
   if (json_is_array(root))
   {
      size_t index;
      json_t *value;
      json_array_foreach(root, index, value)
      {
         maxElementCount = (maxElementCount < (int)json_object_size(value))
                               ? (int)(json_object_size(value))
                               : maxElementCount;
      }
   }
   else if (json_is_object(root))
   {
      const char *key;
      json_t *value;
      json_object_foreach(root, key, value)
      {
         maxElementCount = (maxElementCount < (int)json_object_size(value))
                               ? (int)(json_object_size(value))
                               : maxElementCount;
      }
   }

   return maxElementCount;
}

void DumpToFile(char *path, json_t *root)
{
   FILE *fp = fopen(path, "w");
   if (fp == NULL)
   {
      fprintf(stderr, "Can't open file %s", path);
   }
   int flag = json_dumpf(root, fp, JSON_INDENT(4));
   fclose(fp);
   if (flag == -1)
   {
      printf("Failed to dump to file.\n");
   }
   printf("Dump to file: %s\n", path);
}

/************* INTEFACE IMPLEMENTATION *****************
********************************************************
********************************************************
********************************************************
********************************************************
********************************************************
********************************************************/
void CreateInterfaceStat(Interface_Stat *stat, int numberOfRecords)
{
   for (int counter = 0; counter < numberOfRecords; counter++)
   {
      stat[counter].interface = EMPTY_INTERFACE;
      stat[counter].lastRecords = 0;
      stat[counter].txBytes = json_array();
      stat[counter].rxBytes = json_array();
      stat[counter].txErrors = json_array();
      stat[counter].rxErrors = json_array();
   }
}

void ParserInterfaceStats(Interface_Stat *stat, json_t *root)
{
   if (json_object_size(root) == 0)
   {
      return;
   }
   unsigned int lastRecords =
       json_integer_value(json_object_get(root, "last_record"));
   stat->lastRecords = lastRecords;
   json_array_append_new(stat->txBytes, json_object_get(root, "TxBytes"));
   json_array_append_new(stat->rxBytes, json_object_get(root, "RxBytes"));
   json_array_append_new(stat->txErrors, json_object_get(root, "TxErrors"));
   json_array_append_new(stat->rxErrors, json_object_get(root, "RxErrors"));
}

int SearchInterfaces(Interface_Stat *interface, int ArrSize, const char *key_s)
{
   for (int i = 0; i < ArrSize; i++)
   {
      if (strcmp(interface[i].interface, key_s) == 0)
      {
         return i;
      }
   }
   return -1;
}

/************* WLAN_CLIENT_STAT IMPLEMENTATION *****************
********************************************************
********************************************************
********************************************************
********************************************************
********************************************************
********************************************************/
void CreateWlanClientStat(Wlan_Client_Stat *stat, int numberOfRecords)
{
   for (int counter = 0; counter < numberOfRecords; counter++)
   {
      stat[counter].last_record = 0;
      stat[counter].mac_name = "N/a";
      stat[counter].host_name = NULL;
      stat[counter].txBytes = 0;
      stat[counter].rxBytes = 0;
      stat[counter].rssi = json_array();
   }
}

void ParserWlanClientStats(Wlan_Client_Stat *stat, json_t *root)
{
   if (json_object_size(root) == 0)
   {
      return;
   }
   unsigned int lastRecords = json_integer_value(json_object_get(root, "last_record"));
   stat->last_record = lastRecords;
   printf("Bug: %d\n", 1);
   // stat->host_name = (char *)malloc(strlen(json_string_value(json_object_get(root, "Hostname"))) + 1);
   printf("Bug: %d\n", 2);
   // ? strcpy(stat->host_name, json_string_value(json_object_get(root, "Hostname")));
   printf("Bug: %d\n", 3);
   stat->host_name = json_string_value(json_object_get(root, "Hostname"));
   stat->txBytes = json_integer_value(json_object_get(root, "TxBytes"));
   stat->rxBytes = json_integer_value(json_object_get(root, "RxBytes"));
   json_array_append_new(stat->rssi, json_object_get(root, "RSSI"));
}

// int SearchMacName(Wlan_Client_Stat *stat, int ArrSize, const char *key_s)
// {
//    int i = 0;

//    for (i; i < ArrSize; i++)
//    {
//       if (strcmp(stat[i].mac_name, key_s) == 0)
//       {
//          return i;
//       }
//       printf("i value: %d\n", i);
//    }

//    return -1;
// }

int SearchMacName(Wlan_Client_Stat *stat, const char *key_s)
{
   int i = 0;

   for (i; i < vector_size(stat); i++)
   {
      if (strcmp(stat[i].mac_name, key_s) == 0)
      {
         return i;
      }
   }

   return -1;
}