#define __USE_XOPEN
#define _GNU_SOURCE

#include "extract.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOG_PATH "./log/mqtt_log.1"

/* ERRORR: RETURN -1 IN LINE 79 */
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

// char *ExtractInterfaceData(time_t time, int range)
// {
//    /* GET OBJECT RAW DATA FROM  LOG FILE */
//    json_t *resul = FindByTopicsAndTimestamp(GET_INTERFACE, time, range, LOG_PATH);
//    if (resul == NULL)
//    {
//       fprintf(stderr, "ERROR: Could not find log file\n");
//       return NULL;
//    }

//    json_t *returnArray = json_object_get(resul, "Retturn");
//    if (returnArray == NULL)
//    {
//       printf("ERROR: Could not parse return array\n");
//       return NULL;
//    }

//    /* GET MAX ELEMENT OF ALL OBJECTS TO CREATE ARRAY  STRUCT */

//    int maxElementCount = 0;
//    maxElementCount = GetMaxElementObject(returnArray) - 1;

//    /* CREATE ARRAY OF STRUCT TO HOLD  DATA */
//    Interface_Stat interface_array[maxElementCount];
//    json_t *time_array = json_array();

//    /* INIT ARRAY OF STRUCT */
//    CreateInterfaceStat(interface_array, maxElementCount);

//    /* LOOP OVER ALL OBJECTS IN ARRAY */
//    size_t index;
//    json_t *value;
//    int counter_object;
//    json_array_foreach(returnArray, index, value)
//    {
//       const char *key;
//       json_t *value_object;
//       /* LOOP OVER ALL ELEMENT OF OBJECT */
//       /* IF FIRST ELEMENT OF ARRAY = "N/a", THIS MEAN ARRAY IS ALL EMPTY
//        * MALLOC AND CREATE NEW INTERFACE NAME IN STRUCT*/
//       if (strstr(interface_array[0].interface, EMPTY_INTERFACE))
//       {
//          counter_object = 0;
//          json_object_foreach(value, key, value_object)
//          {
//             if (strstr(key, "time"))
//             {
//                json_array_append_new(time_array, json_deep_copy(value_object));
//                // print_json(time_array);
//             }
//             else
//             {
//                interface_array[counter_object].interface = (char *)malloc(strlen(key) + 1);
//                strcpy(interface_array[counter_object].interface, (key));
//                ParserInterfaceStats(&interface_array[counter_object], value_object);
//             }
//             counter_object++;
//          }
//       }
//       else
//       {
//          counter_object = 0;
//          json_object_foreach(value, key, value_object)
//          {
//             if (strstr(key, "time"))
//             {
//                json_array_append_new(time_array, json_deep_copy(value_object));
//                // print_json(time_array);
//             }
//             else
//             {
//                /* SEARCH INDEX THAT HAS THE INTERFACE NAME SAME AS
//                 * VALUE_OBJECT.INTERFACE, PARSE DATA INTO THIS */
//                int index_interface = SearchInterfaces(interface_array, maxElementCount, key);
//                /* IF INDEX <0, THIS MEAN ARRAY HAVEN'T INIT THIS
//                 * INTERFACE, CHECK THE NEAREST ELEMENT OF ARRAY THAT
//                 * EMPTY */
//                if (index_interface < 0)
//                {
//                   index_interface = SearchInterfaces(interface_array, maxElementCount, EMPTY_INTERFACE);
//                   ParserInterfaceStats(&interface_array[index_interface], value_object);
//                }
//                ParserInterfaceStats(&interface_array[index_interface], value_object);
//             }
//             counter_object++;
//          }
//       }
//    }

//    json_t *arrayROOT = json_array();
//    // print_json(time_array);
//    for (int c = 0; c < maxElementCount; c++)
//    {
//       json_t *returnROOT = json_object();
//       json_object_set_new(returnROOT, "Interface", json_string(interface_array[c].interface));
//       json_object_set_new(returnROOT, "TxBytes", interface_array[c].txBytes);
//       json_object_set_new(returnROOT, "RxBytes", interface_array[c].rxBytes);
//       json_object_set_new(returnROOT, "TxErrors", interface_array[c].txErrors);
//       json_object_set_new(returnROOT, "RxErrors", interface_array[c].rxErrors);
//       json_object_set_new(returnROOT, "Timestamp", json_integer(interface_array[c].lastRecords));
//       json_object_set_new(returnROOT, "Timeline", time_array);
//       json_array_append_new(arrayROOT, returnROOT);
//    }

//    print_json(arrayROOT);
//    DumpToFile("FinalFilter.json", arrayROOT);

//    for (int c = 0; c < maxElementCount; c++)
//    {
//       free(interface_array[c].interface);
//    }

//    json_decref(arrayROOT);
//    json_decref(returnArray);
//    json_decref(resul);
//    return NULL;
// }
