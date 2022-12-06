#include "extract.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* FindByTopics(int TopicId, char* log) {
   /* Variable  Declaration */
   FILE *file_ptr, *file_temp;
   file_ptr = fopen(log, "r");
   file_temp = fopen("temp", "a+");
   char line[LINE];
   char* returnLines;
   long lSize;

   /* Convert TopicID into Topic's Name */
   char* topicString = ConvertIDTopic(TopicId);
   if (topicString == NULL) {
      return NULL;
   }

   /* Get all line have the keyword  "Topic's name" */
   /* Write to temporary file */
   while (fgets(line, LINE, file_ptr)) {
      if (strstr(line, topicString)) {
         char* linePointer = (char*)malloc(strlen(line) + 1);
         strcpy(linePointer, line);
         fputs(linePointer, file_temp);
         free(linePointer);
      }
   }

   /* Read all line into char pointer*/

   fseek(file_temp, 0L, SEEK_END);
   lSize = ftell(file_temp);
   rewind(file_temp);
   returnLines = calloc(1, lSize + 1);
   if (!returnLines)
      fclose(file_temp), fputs("memory alloc fails", stderr), exit(1);
   if (1 != fread(returnLines, lSize, 1, file_temp))
      fclose(file_temp), free(returnLines), fputs("entire read fails", stderr),
          exit(1);

   fclose(file_ptr);
   fclose(file_temp);

   /* Remove temp file */

   if (remove("temp") == 0) {
      printf("temp file removed\n");
   } else {
      printf("temp file fail to removed\n");
   }
   return returnLines;
}

char* FindByTimestamp(long long Timestamp, int range, char* log,
                      int keepTempFileFlag) {
   FILE *file_ptr, *file_temp;
   file_ptr = fopen(log, "r");
   file_temp = fopen("temp", "a+");
   char line[LINE];
   char* returnLines;
   long lSize;

   struct tm tm;
   time_t epoch;
   int minRange = Timestamp - range;
   int maxRange = Timestamp + range;

   while (fgets(line, LINE, file_ptr)) {
      char* temp_line = (char*)malloc(sizeof(line) + 1);
      strcpy(temp_line, line);
      char* token = strtok(temp_line, "[]");
      if (strlen(token) < 21) {
         if (strptime(token, "%Y-%m-%d %H:%M:%S", &tm) != NULL) {
            epoch = mktime(&tm);
            if (epoch >= minRange && epoch <= maxRange) {
               char* linePointer = (char*)malloc(strlen(line) + 1);
               strcpy(linePointer, line);
               fputs(linePointer, file_temp);
               free(linePointer);
            }
         }
      }
      free(temp_line);
   }

   fseek(file_temp, 0L, SEEK_END);
   lSize = ftell(file_temp);
   rewind(file_temp);
   returnLines = calloc(1, lSize + 1);
   if (!returnLines)
      fclose(file_temp), fputs("memory alloc fails", stderr), exit(1);
   if (1 != fread(returnLines, lSize, 1, file_temp))
      fclose(file_temp), free(returnLines), fputs("entire read fails", stderr),
          exit(1);

   fclose(file_ptr);
   fclose(file_temp);

   /* Remove temp file */
   if (keepTempFileFlag == 1) {
      if (remove("temp") == 0) {
         printf("temp file removed\n");
      } else {
         printf("temp file fail to removed\n");
      }
      return returnLines;
   } else {
      free(returnLines);
      return NULL;
   }
}

char* FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range,
                               char* log) {
   FindByTimestamp(Timestamp, range, log, 0);
   char* finalFilter = FindByTopics(Topic, log);
   return finalFilter;
}
