#define __USE_XOPEN
#define _GNU_SOURCE

#include "extract.h"
#include "vec.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOG_PATH "./log/mqtt_log"

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

json_t *ExtractWlanData(time_t time, int range)
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
   wlan_client_stat_t *wlan_clients = vector_create();

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
            wlan_client_stat_t *wlan_clients_temp = vector_add_asg(&wlan_clients);
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
               wlan_client_stat_t *wlan_clients_temp = vector_add_asg(&wlan_clients);
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

   // DumpToFile("FinalFilter_WLAN.json", arrayROOT);
   json_t *finalJson = json_deep_copy(arrayROOT);
   json_decref(arrayROOT);
   json_decref(returnArray);
   json_decref(resul);
   vector_free(wlan_clients);

   return finalJson;
}

json_t *ExtractInterfaceData(time_t time, int range)
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
   interface_stat_t *interface = vector_create();

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
            interface_stat_t *interface_temp = vector_add_asg(&interface);
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
               interface_stat_t *interface_temp_2 = vector_add_asg(&interface);
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

   // DumpToFile("FinalFilter_INTERFACE.json", arrayROOT);

   json_t *finalJson = json_deep_copy(arrayROOT);
   json_decref(arrayROOT);
   json_decref(returnArray);
   json_decref(resul);
   vector_free(interface);

   return finalJson;
}

json_t *ExtractSsidData(time_t time, int range)
{
   json_t *result = FindByTopicsAndTimestamp(GET_SSIDS, time, range, LOG_PATH);
   if (result == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   json_t *returnArray = json_object_get(result, "Retturn");
   if (returnArray == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   int index = json_array_size(returnArray);
   json_t *arrayROOT = json_array();

   json_array_append_new(arrayROOT, json_array_get(json_array_get(returnArray, index - 1), 0));
   json_array_append_new(arrayROOT, json_array_get(json_array_get(returnArray, index - 2), 0));

   // DumpToFile("FINAL.json", arrayROOT);
   json_t *finalJson = json_deep_copy(arrayROOT);
   json_decref(arrayROOT);
   json_decref(returnArray);
   json_decref(result);

   return finalJson;
}

json_t *ExtractLanData(time_t time, int range)
{
   json_t *resul = FindByTopicsAndTimestamp(GET_LAN, time, range, LOG_PATH);
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
   lan_client_stat_t *lan_clients = vector_create();
   size_t index;
   json_t *value;
   json_array_foreach(returnArray, index, value)
   {
      const char *key;
      json_t *value_object;
      if (vector_size(lan_clients) == 0)
      {
         json_object_foreach(value, key, value_object)
         {
            // Wlan_Client_Stat *wlan_clients_temp = vector_add_asg(&wlan_clients);
            lan_client_stat_t *lan_clients_temp = vector_add_asg(&lan_clients);
            ParserLanClientStatsNew(value_object, key, lan_clients_temp);
         }
      }
      else
      {
         json_object_foreach(value, key, value_object)
         {
            int index_interface = SearchMacName_Lan(lan_clients, key);
            if (index_interface < 0)
            {
               // Wlan_Client_Stat *wlan_clients_temp_2 = vector_add_asg(&wlan_clients);
               lan_client_stat_t *lan_clients_temp = vector_add_asg(&lan_clients);
               ParserLanClientStatsNew(value_object, key, lan_clients_temp);
            }
            else
            {
               lan_clients[index_interface].last_record = json_integer_value(json_object_get(value_object, "last_record"));
            }
         }
      }
      //
   }

   json_t *arrayROOT = json_array();
   for (int c = 0; c < vector_size(lan_clients); c++)
   {
      json_t *returnROOT = json_object();
      json_object_set_new(returnROOT, "macname", json_string(lan_clients[c].mac_name));
      json_object_set_new(returnROOT, "interface", json_string(lan_clients[c].interface));
      json_object_set_new(returnROOT, "ip", json_string(lan_clients[c].ip));
      json_object_set_new(returnROOT, "lastRecord", json_integer(lan_clients[c].last_record));
      json_array_append_new(arrayROOT, returnROOT);
   }

   // DumpToFile("FinalFilter_WLAN.json", arrayROOT);

   json_t *finalJson = json_deep_copy(arrayROOT);
   json_decref(arrayROOT);
   json_decref(returnArray);
   json_decref(resul);
   vector_free(lan_clients);

   return finalJson;
}

json_t *ExtractIpData(time_t time, int range)
{
   json_t *result = FindByTopicsAndTimestamp(GET_IP, time, range, LOG_PATH);
   if (result == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   json_t *returnArray = json_object_get(result, "Retturn");
   if (returnArray == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   int index = json_array_size(returnArray);
   json_t *ROOT = json_array_get(returnArray, index - 1);
   json_t *arrayROOT = json_array();

   const char *key;
   json_t *value;
   json_object_foreach(ROOT, key, value)
   {
      json_object_set_new(value, "interface", json_string(key));
      json_array_append_new(arrayROOT, value);
   }

   // DumpToFile("test.json", arrayROOT);
   json_t *finalJson = json_deep_copy(arrayROOT);
   json_decref(arrayROOT);
   json_decref(returnArray);
   json_decref(result);

   return finalJson;
}

json_t *ExtractMemData(time_t time, int range)
{
   json_t *result = FindByTopicsAndTimestamp(GET_MEM, time, range, LOG_PATH);
   if (result == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   json_t *returnArray = json_object_get(result, "Retturn");
   if (returnArray == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   json_t *mem_array = json_array();
   json_t *root = json_object();
   json_t *tempRoot;

   size_t index;
   json_t *value;
   json_array_foreach(returnArray, index, value)
   {
      tempRoot = json_array_get(value, 0);
      json_array_append_new(mem_array, json_object_get(tempRoot, "used"));
   }

   json_object_set_new(root, "mem_used", mem_array);

   // DumpToFile("test.json", root);
   json_t *finalJson = json_deep_copy(root);

   json_decref(tempRoot);
   json_decref(mem_array);
   json_decref(root);
   json_decref(returnArray);
   json_decref(result);

   return finalJson;
}

json_t *ExtractCpuData(time_t time, int range)
{
   json_t *result = FindByTopicsAndTimestamp(GET_CPU, time, range, LOG_PATH);
   if (result == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   json_t *returnArray = json_object_get(result, "Retturn");
   if (returnArray == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   json_t *cpu_array = json_array();
   json_t *root = json_object();
   json_t *tempRoot;

   size_t index;
   json_t *value;
   json_array_foreach(returnArray, index, value)
   {
      tempRoot = json_array_get(value, 0);
      json_array_append_new(cpu_array, json_object_get(tempRoot, "10minutes"));
   }

   json_object_set_new(root, "cpu", cpu_array);

   // DumpToFile("test.json", root);
   json_t *finalJson = json_deep_copy(root);

   json_decref(tempRoot);
   json_decref(cpu_array);
   json_decref(root);

   json_decref(returnArray);
   json_decref(result);

   return finalJson;
}

json_t *ExtractChannelUsageData(time_t time, int range)
{
   json_t *result = FindByTopicsAndTimestamp(GET_CHANNEL_USAGE, time, range, LOG_PATH);
   if (result == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   json_t *returnArray = json_object_get(result, "Retturn");
   if (returnArray == NULL)
   {
      fprintf(stderr, "ERROR: Could not find log file\n");
      return NULL;
   }

   channel_usage_t channel_usage[2];
   ChannelUsageInit(&channel_usage[0], CHANNEL2G);
   ChannelUsageInit(&channel_usage[1], CHANNEL5G);

   json_t *value;
   size_t index;
   json_t *tempRoot;
   json_array_foreach(returnArray, index, value)
   {
      tempRoot = json_array_get(value, 0);

      int currentChannel = json_integer_value(json_object_get(tempRoot, "Currentchannel"));
      if (currentChannel <= 14)
      {
         json_array_append_new(channel_usage[0].dut_time, json_object_get(tempRoot, "DUT_Time"));
         json_array_append_new(channel_usage[0].free_time, json_object_get(tempRoot, "free_Time"));
         json_array_append_new(channel_usage[0].interence_time, json_object_get(tempRoot, "interence_time"));
         json_array_append_new(channel_usage[0].other_ap_time, json_object_get(tempRoot, "other_APs_Time"));
      }
      else
      {
         json_array_append_new(channel_usage[1].dut_time, json_object_get(tempRoot, "DUT_Time"));
         json_array_append_new(channel_usage[1].free_time, json_object_get(tempRoot, "free_Time"));
         json_array_append_new(channel_usage[1].interence_time, json_object_get(tempRoot, "interence_time"));
         json_array_append_new(channel_usage[1].other_ap_time, json_object_get(tempRoot, "other_APs_Time"));
      }
   }

   json_t *arrayROOT = json_array();

   for (int i = 0; i < 2; i++)
   {
      json_t *tempRoot_2 = json_object();
      json_object_set_new(tempRoot_2, "channel", json_string(channel_usage[i].channel));
      json_object_set_new(tempRoot_2, "free_time", channel_usage[i].free_time);
      json_object_set_new(tempRoot_2, "interence_time", channel_usage[i].interence_time);
      json_object_set_new(tempRoot_2, "other_APs_Time", channel_usage[i].other_ap_time);
      json_object_set_new(tempRoot_2, "DUT_Time", channel_usage[i].dut_time);
      json_array_append_new(arrayROOT, tempRoot_2);
   }

   // DumpToFile("channelUsage.json", arrayROOT);
   json_t *finalJson = json_deep_copy(arrayROOT);
   json_decref(arrayROOT);
   json_decref(result);

   return finalJson;
}