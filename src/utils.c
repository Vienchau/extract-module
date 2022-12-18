#define __USE_XOPEN
#define _GNU_SOURCE

#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "extract.h"
#include "vec.h"

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
      return;
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

void ParserInterfaceNew(json_t *value, const char *key, Interface_Stat *interface_temp)
{
   interface_temp->last_record = json_array();
   json_array_append_new(interface_temp->last_record, json_object_get(value, "last_record"));

   interface_temp->txBytes = json_array();
   json_array_append_new(interface_temp->txBytes, json_object_get(value, "TxBytes"));

   interface_temp->rxBytes = json_array();
   json_array_append_new(interface_temp->rxBytes, json_object_get(value, "RxBytes"));

   interface_temp->txErrors = json_array();
   json_array_append_new(interface_temp->txErrors, json_object_get(value, "TxErrors"));

   interface_temp->rxErrors = json_array();
   json_array_append_new(interface_temp->rxErrors, json_object_get(value, "RxErrors"));

   interface_temp->interface = key;
}
void ParserInterfaceStats(Interface_Stat *interface, json_t *value, int index_interface)
{
   json_array_append_new(interface[index_interface].last_record, json_object_get(value, "last_record"));
   json_array_append_new(interface[index_interface].txBytes, json_object_get(value, "TxBytes"));
   json_array_append_new(interface[index_interface].rxBytes, json_object_get(value, "RxBytes"));
   json_array_append_new(interface[index_interface].txErrors, json_object_get(value, "TxErrors"));
   json_array_append_new(interface[index_interface].rxErrors, json_object_get(value, "RxErrors"));
}
int SearchInterfaces(Interface_Stat *stat, const char *key_s)
{

   for (int i = 0; i < vector_size(stat); i++)
   {
      if (strcmp(stat[i].interface, key_s) == 0)
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

void ParserWlanClientStatsNew(json_t *value, const char *key, Wlan_Client_Stat *wlan_clients_temp)
{
   // Wlan_Client_Stat *wlan_clients_temp = vector_add_asg(&(*wlan_clients));
   wlan_clients_temp->last_record = json_array();
   json_array_append_new(wlan_clients_temp->last_record, json_object_get(value, "last_record"));

   wlan_clients_temp->rssi = json_array();
   json_array_append_new(wlan_clients_temp->rssi, json_object_get(value, "RSSI"));

   wlan_clients_temp->mac_name = key;
   wlan_clients_temp->host_name = json_string_value(json_object_get(value, "Hostname"));
   wlan_clients_temp->txBytes = json_integer_value(json_object_get(value, "TxBytes"));
   wlan_clients_temp->rxBytes = json_integer_value(json_object_get(value, "RxBytes"));
}
void ParserWlanClientStats(Wlan_Client_Stat *wlan_clients, json_t *value, int index_interface)
{
   wlan_clients[index_interface].txBytes = json_integer_value(json_object_get(value, "TxBytes"));
   wlan_clients[index_interface].rxBytes = json_integer_value(json_object_get(value, "RxBytes"));
   json_array_append_new(wlan_clients[index_interface].rssi, json_object_get(value, "RSSI"));
   json_array_append_new(wlan_clients[index_interface].last_record, json_object_get(value, "last_record"));
}

int SearchMacName(Wlan_Client_Stat *stat, const char *key_s)
{

   for (int i = 0; i < vector_size(stat); i++)
   {
      if (strcmp(stat[i].mac_name, key_s) == 0)
      {
         return i;
      }
   }
   return -1;
}
