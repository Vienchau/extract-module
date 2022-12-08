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
   FILE *file_ptr, *file_temp;
   file_ptr = fopen(log, "r");
   file_temp = fopen("temp_2", "a+");
   char line[LINE];
   char* returnLines;
   long lSize;

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

         // char* linePointer = (char*)malloc(strlen(line) + 1);
         // strcpy(linePointer, line);
         // fputs(linePointer, file_temp);
         // free(linePointer);
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
               print_json(root);
               json_array_append_new(arrayApi, json_deep_copy(root));
            }
         }
         json_decref(root);
         free(temp_line_date);
      }
   }

   print_json(arrayApi);
   // printf(sizeof(arrayApi));

   json_decref(arrayApi);

   // fclose(file_ptr);
   // fclose(file_temp);
   return NULL;

   /* Read all line into char pointer*/

   // fseek(file_temp, 0L, SEEK_END);
   // lSize = ftell(file_temp);
   // rewind(file_temp);
   // returnLines = calloc(1, lSize + 1);
   // if (!returnLines)
   //    fclose(file_temp), fputs("memory alloc fails", stderr), exit(1);
   // if (1 != fread(returnLines, lSize, 1, file_temp))
   //    fclose(file_temp), free(returnLines), fputs("entire read fails",
   //    stderr),
   //        exit(1);

   // fclose(file_ptr);
   // fclose(file_temp);

   // /* Remove temp file */

   // if (remove("temp_2") == 0) {
   //    printf("temp file removed\n");
   // } else {
   //    printf("temp file fail to removed\n");
   // }
   // return returnLines;
}

char* FindByTimestamp(long long Timestamp, int range, char* log,
                      int keepTempFileFlag) {
   FILE *file_ptr, *file_temp;
   file_ptr = fopen(log, "r");
   file_temp = fopen("temp", "w");
   char line[LINE];

   long lSize;
   static const struct tm tm_date_empty;
   time_t epoch;
   int minRange = Timestamp - range;
   int maxRange = Timestamp + range;

   while (fgets(line, LINE, file_ptr)) {
      char* temp_line = (char*)malloc(sizeof(line) + 1);
      strcpy(temp_line, line);
      char* token = strtok(temp_line, "[]");
      if (strlen(token) < 21) {
         printf("%s\n", token);
         char* temp_token = (char*)malloc(strlen(token) + 1);
         strcpy(temp_token, token);
         struct tm tm_date;
         if (strptime(temp_token, "%Y-%m-%d %H:%M:%S", &tm_date)) {
            epoch = mktime(&tm_date);
            if (epoch >= minRange && epoch <= maxRange) {
               char* linePointer = (char*)malloc(strlen(line) + 1);
               strcpy(linePointer, line);
               fputs(linePointer, file_temp);
               free(linePointer);
            }
            free(temp_token);
            tm_date = tm_date_empty;
         }
      }
      free(temp_line);
   }
   fclose(file_temp);
   fclose(file_ptr);

   /* Remove temp file */
   if (keepTempFileFlag == RETURN_STRING) {
      char* returnLines;
      file_temp = fopen("temp", "r");
      fseek(file_temp, 0L, SEEK_END);
      lSize = ftell(file_temp);
      rewind(file_temp);

      returnLines = calloc(1, lSize + 1);
      if (!returnLines) {
         fclose(file_temp), fputs("memory alloc fails", stderr), exit(1);
      }

      if (1 != fread(returnLines, lSize, 1, file_temp)) {
         fclose(file_temp), free(returnLines),
             fputs("entire read fails from timestamp", stderr), exit(1);
      }
      fclose(file_temp);
      if (remove("temp") == 0) {
         printf("temp_2 file removed\n");
      } else {
         printf("temp_2 file fail to removed\n");
      }
      return returnLines;
   } else {
      return NULL;
   }
}

char* FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range,
                               char* log) {
   FindByTimestamp(Timestamp, range, log, RETURN_TEMP_FILE);
   char* finalFilter = FindByTopics(Topic, "temp");
   return finalFilter;
}
