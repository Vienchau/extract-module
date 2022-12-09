#define __USE_XOPEN
#define _GNU_SOURCE

#include "extract.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ERRORR: RETURN -1 IN LINE 79 */
char* FindByTopics(int TopicId, char* log) {
   /* Variable  Declaration */
   FILE* file_ptr;
   file_ptr = fopen(log, "r");
   char* returnLine;
   char line[LINE];
   json_error_t error;
   json_t* arrayApi = json_array();
   int foundTopics = 0;

   /* Convert TopicID into Topic's Name */
   char* topicString = ConvertIDTopic(TopicId);
   if (topicString == NULL) {
      fclose(file_ptr);
      json_decref(arrayApi);
      return NULL;
   }
   printf("found: %s\n", topicString);

   /* Get all line have the keyword  "Topic's name" */
   while (fgets(line, LINE, file_ptr)) {
      if (strstr(line, topicString)) {
         foundTopics = 1;
         char* temp_line_date = (char*)malloc(strlen(line) + 1);
         strcpy(temp_line_date, line);
         char date[21], data[strlen(temp_line_date)];
         sscanf(temp_line_date, "%*[^2] %[^]]]: %*[^]]]: %[^\n]", date, data);
         json_t* root = json_loads(data, 0, &error);

         if (!root) {
            printf("Couldn't parse json from string: %s\n", error.text);
            json_decref(root);
            free(temp_line_date);
            json_decref(arrayApi);
            fclose(file_ptr);
            return NULL;
         } else {
            if (json_object_size(json_object_get(root, "data")) != 0 ||
                json_is_array(json_object_get(root, "data"))) {
               json_object_set_new(root, "time", json_string(date));

               json_array_append_new(arrayApi, json_deep_copy(root));
            }
         }
         json_decref(root);
         free(temp_line_date);
      }
   }

   if (foundTopics == 0) {
      printf("Error: Couldn't found any data in this topic\n");
      json_decref(arrayApi);
      fclose(file_ptr);
      return NULL;
   }
   returnLine = json_dumps(arrayApi, JSON_COMPACT);
   if (returnLine == NULL) {
      printf("Error: Couldn't dump json to string\n");
      json_decref(arrayApi);
      fclose(file_ptr);
      return NULL;
   }

   json_decref(arrayApi);
   fclose(file_ptr);

   // Remove temp files
   if (remove("temp") == 0) {
      printf("temp file removed\n");
   } else {
      printf("temp file fail to removed\n");
   }
   return returnLine;
}

int FindByTimestamp(long long Timestamp, int range, char* log) {
   FILE *file_ptr, *file_temp;
   file_ptr = fopen(log, "r");
   file_temp = fopen("temp", "a+");

   char line[LINE];
   int emptyFlag = 0;

   static const struct tm tm_date_empty;
   time_t epoch;
   int minRange = Timestamp - range;
   int maxRange = Timestamp + range;

   while (fgets(line, LINE, file_ptr)) {

      char* temp_line_date = (char*)malloc(strlen(line) + 1);
      strcpy(temp_line_date, line);
      char date[21];
      sscanf(temp_line_date, "%*[^2] %[^]]] %*[^\n]", date);
      struct tm tm_date;
      if (strptime(date, "%Y-%m-%d %H:%M:%S", &tm_date)) {
         epoch = mktime(&tm_date);
         if (epoch >= minRange && epoch <= maxRange) {
            emptyFlag = 1;
            char* linePointer = (char*)malloc(strlen(line) + 1);
            strcpy(linePointer, line);
            fputs(linePointer, file_temp);
            free(linePointer);
         }

         tm_date = tm_date_empty;
         free(temp_line_date);
      }
   }
   if (emptyFlag == 0) {
      printf("No data in this time range.\n");
      fclose(file_temp);
      fclose(file_ptr);
      return FILE_EMPTY;
   }

   fclose(file_temp);
   fclose(file_ptr);
   return FILE_OK;
}

char* FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range,
                               char* log) {
   int result = FindByTimestamp(Timestamp, range, log);
   if (result == FILE_EMPTY) {
      return NULL;
   } else {
      char* finalFilter = FindByTopics(Topic, "temp");
      if (finalFilter == NULL) {
         free(finalFilter);
         return NULL;
      }
      return finalFilter;
   }
}

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