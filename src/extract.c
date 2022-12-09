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

   /* Convert TopicID into Topic's Name */
   char* topicString = ConvertIDTopic(TopicId);
   if (topicString == NULL) {
      return NULL;
   }

   /* Get all line have the keyword  "Topic's name" */
   /* Write to temporary file */
   while (fgets(line, LINE, file_ptr)) {

      if (strstr(line, topicString)) {
         char* temp_line_date = (char*)malloc(strlen(line) + 1);
         strcpy(temp_line_date, line);
         char date[21], data[strlen(temp_line_date)];
         sscanf(temp_line_date, "%*[^2] %[^]]]: %*[^]]]: %[^\n]", date, data);
         json_t* root = json_loads(data, 0, &error);

         if (!root) {
            printf("Couldn't parse json from string: %s\n", error.text);
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
   returnLine = json_dumps(arrayApi, JSON_COMPACT);
   if (returnLine == NULL) {
      printf("Error: Couldn't dump json to string\n");
      return NULL;
   }
   json_decref(arrayApi);

   fclose(file_ptr);
   if (remove("temp") == 0) {
      printf("temp file removed\n");
   } else {
      printf("temp file fail to removed\n");
   }
   return returnLine;
}

void FindByTimestamp(long long Timestamp, int range, char* log) {
   FILE *file_ptr, *file_temp;
   file_ptr = fopen(log, "r");
   file_temp = fopen("temp", "a+");

   char line[LINE];

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
            char* linePointer = (char*)malloc(strlen(line) + 1);
            strcpy(linePointer, line);
            fputs(linePointer, file_temp);
            free(linePointer);
         }

         tm_date = tm_date_empty;
         free(temp_line_date);
      }
   }
   fclose(file_temp);
   fclose(file_ptr);
}

char* FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range,
                               char* log) {
   FindByTimestamp(Timestamp, range, log);
   char* finalFilter = FindByTopics(Topic, "temp");

   return finalFilter;
}
