#define __USE_XOPEN
#define _GNU_SOURCE

#include "extract.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOG_PATH "./log/mqtt_log.1"

// TODO: CHANGE RETURN CHAR* IN THE EXTRACT DATA TOPIC

json_t *FindByTopics(int TopicId, char *log)
{
   /* Variable  Declaration */
   FILE *file_ptr;
   file_ptr = fopen(log, "r");
   char line[LINE];
   json_error_t error;
   json_t *arrayApi = json_array();
   int foundTopics = 0;

   /* Convert TopicID into Topic's Name */
   char *topicString = ConvertIDTopic(TopicId);
   if (topicString == NULL)
   {
      fclose(file_ptr);
      json_decref(arrayApi);
      return NULL;
   }
   printf("found: %s\n", topicString);

   /* Get all line have the keyword  "Topic's name" */
   while (fgets(line, LINE, file_ptr))
   {
      if (strstr(line, topicString) && strstr(line, LOGFILE))
      {
         foundTopics = 1;
         char *temp_line_date = (char *)malloc(strlen(line) + 1);
         strcpy(temp_line_date, line);
         char date[21], data[strlen(temp_line_date)];
         sscanf(temp_line_date, "%*[^2] %[^]]]: %*[^]]]: %[^\n]", date, data);
         json_t *root = json_loads(data, 0, &error);

         if (!root)
         {
            printf("Couldn't parse json from string: %s\n", error.text);
            json_decref(root);
            free(temp_line_date);
            json_decref(arrayApi);
            fclose(file_ptr);
            return NULL;
         }
         else
         {
            if (json_object_size(json_object_get(root, "data")) != 0 ||
                json_is_array(json_object_get(root, "data")))
            {
               json_t *root_data = json_object_get(root, "data");
               json_array_append_new(arrayApi, json_deep_copy(root_data));
               json_decref(root_data);
            }
         }

         json_decref(root);

         free(temp_line_date);
      }
   }

   if (foundTopics == 0)
   {
      printf("Error: Couldn't found any data in this topic\n");
      json_decref(arrayApi);
      fclose(file_ptr);
      return NULL;
   }

   fclose(file_ptr);

   // Remove temp files
   if (remove("temp") == 0)
   {
      printf("temp file removed\n");
   }
   else
   {
      printf("temp file fail to removed\n");
   }
   return arrayApi;
}

int FindByTimestamp(long long Timestamp, int range, char *log)
{
   FILE *file_ptr, *file_temp;
   file_ptr = fopen(log, "r");
   file_temp = fopen("temp", "a+");

   char line[LINE];
   int emptyFlag = 0;

   static const struct tm tm_date_empty;
   time_t epoch;
   int minRange = Timestamp - range;
   int maxRange = Timestamp + range;

   while (fgets(line, LINE, file_ptr))
   {

      char *temp_line_date = (char *)malloc(strlen(line) + 1);
      strcpy(temp_line_date, line);
      char date[21];
      sscanf(temp_line_date, "%*[^2] %[^]]] %*[^\n]", date);
      struct tm tm_date;
      if (strptime(date, "%Y-%m-%d %H:%M:%S", &tm_date))
      {
         epoch = mktime(&tm_date);
         if (epoch >= minRange && epoch <= maxRange)
         {
            emptyFlag = 1;
            char *linePointer = (char *)malloc(strlen(line) + 1);
            strcpy(linePointer, line);
            fputs(linePointer, file_temp);
            free(linePointer);
         }

         tm_date = tm_date_empty;
         free(temp_line_date);
      }
   }
   if (emptyFlag == 0)
   {
      printf("No data in this time range.\n");
      fclose(file_temp);
      fclose(file_ptr);
      return FILE_EMPTY;
   }

   fclose(file_temp);
   fclose(file_ptr);
   return FILE_OK;
}

char *FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range, char *log)
{
   int result = FindByTimestamp(Timestamp, range, log);
   if (result == FILE_EMPTY)
   {
      return NULL;
   }
   else
   {
      json_t *finalFilter = json_object();
      json_object_set_new(finalFilter, "Retturn", FindByTopics(Topic, "temp"));
      if (finalFilter == NULL)
      {

         return NULL;
      }

      return finalFilter;
   }
}

char *ExtractWlanData(time_t time, int range)
{
   /* GET OBJECT RAW DATA FROM  LOG FILE */
   json_t *resul = FindByTopicsAndTimestamp(GET_WLAN, time, range, LOG_PATH);
   if (resul == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }
   json_t *returnArray = json_object_get(resul, "Retturn");
   if (returnArray == NULL)
   {
      printf("ERROR: Could not parse return array\n");
      return NULL;
   }
   /* GET MAX ELEMENT OF ALL OBJECTS TO CREATE ARRAY  STRUCT */
   Wlan_Client_Stat *wlan_clients = vector_create();

   size_t index;
   json_t *value;
   json_array_foreach(returnArray, index, value)
   {
      const char *key;
      json_t *value_object;
      if (vector_size(wlan_clients) == 0)
      {
         json_object_foreach(value, key, value_object)
         {
            // Wlan_Client_Stat *wlan_clients_temp = vector_add_asg(&wlan_clients);
            Wlan_Client_Stat *wlan_clients_temp = vector_add_asg(&wlan_clients);
            ParserWlanClientStatsNew(value_object, key, wlan_clients_temp);
         }
      }
      else
      {
         json_object_foreach(value, key, value_object)
         {
            int index_interface = SearchMacName(wlan_clients, key);
            if (index_interface < 0)
            {
               // Wlan_Client_Stat *wlan_clients_temp_2 = vector_add_asg(&wlan_clients);
               Wlan_Client_Stat *wlan_clients_temp = vector_add_asg(&wlan_clients);
               ParserWlanClientStatsNew(value_object, key, wlan_clients_temp);
            }
            else
            {
               ParserWlanClientStats(wlan_clients, value_object, index_interface);
            }
         }
      }
      //
   }

   json_t *arrayROOT = json_array();
   for (int c = 0; c < vector_size(wlan_clients); c++)
   {
      json_t *returnROOT = json_object();
      json_object_set_new(returnROOT, "Macname", json_string(wlan_clients[c].mac_name));
      json_object_set_new(returnROOT, "Hostname", json_string(wlan_clients[c].host_name));
      json_object_set_new(returnROOT, "TxBytes", json_integer(wlan_clients[c].txBytes));
      json_object_set_new(returnROOT, "RxBytes", json_integer(wlan_clients[c].rxBytes));
      json_object_set_new(returnROOT, "Timestamp", wlan_clients[c].last_record);
      json_object_set_new(returnROOT, "RSSI", wlan_clients[c].rssi);
      json_array_append_new(arrayROOT, returnROOT);
   }

   DumpToFile("FinalFilter_WLAN.json", arrayROOT);

   json_decref(arrayROOT);
   json_decref(returnArray);
   json_decref(resul);
   vector_free(wlan_clients);

   return NULL;
}

char *ExtractInterfaceData(time_t time, int range)
{
   /* GET OBJECT RAW DATA FROM  LOG FILE */
   json_t *resul = FindByTopicsAndTimestamp(GET_INTERFACE, time, range, LOG_PATH);
   if (resul == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   json_t *returnArray = json_object_get(resul, "Retturn");
   if (returnArray == NULL)
   {
      printf("ERROR: Could not parse return array\n");
      return NULL;
   }
   /* GET MAX ELEMENT OF ALL OBJECTS TO CREATE ARRAY  STRUCT */

   /* CREATE ARRAY OF STRUCT TO HOLD  DATA */
   Interface_Stat *interface = vector_create();

   /* LOOP OVER ALL OBJECTS IN ARRAY */
   size_t index;
   json_t *value;

   json_array_foreach(returnArray, index, value)
   {
      const char *key;
      json_t *value_object;
      if (vector_size(interface) == 0)
      {
         json_object_foreach(value, key, value_object)
         {
            Interface_Stat *interface_temp = vector_add_asg(&interface);
            ParserInterfaceNew(value_object, key, interface_temp);
         }
      }
      else
      {
         json_object_foreach(value, key, value_object)
         {
            int index_interface = SearchInterfaces(interface, key);
            if (index_interface < 0)
            {
               Interface_Stat *interface_temp_2 = vector_add_asg(&interface);
               ParserInterfaceNew(value_object, key, interface_temp_2);
            }
            else
            {
               ParserInterfaceStats(interface, value_object, index_interface);
            }
         }
      }
   }

   json_t *arrayROOT = json_array();

   for (int c = 0; c < vector_size(interface); c++)
   {
      json_t *returnROOT = json_object();
      json_object_set_new(returnROOT, "Interface", json_string(interface[c].interface));
      json_object_set_new(returnROOT, "TxBytes", interface[c].txBytes);
      json_object_set_new(returnROOT, "RxBytes", interface[c].rxBytes);
      json_object_set_new(returnROOT, "TxErrors", interface[c].txErrors);
      json_object_set_new(returnROOT, "RxErrors", interface[c].rxErrors);
      json_object_set_new(returnROOT, "Timestamp", interface[c].last_record);
      json_array_append_new(arrayROOT, returnROOT);
   }

   DumpToFile("FinalFilter_INTERFACE.json", arrayROOT);

   json_decref(arrayROOT);
   json_decref(returnArray);
   json_decref(resul);
   vector_free(interface);

   return NULL;
}
