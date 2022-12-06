#include "extract.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* FindByTopics(int Topic, char* log) {
   FILE* file_ptr;
   file_ptr = fopen(log, "r");
   char line[LINE];

   char* topicString = ConvertIDTopic(Topic);
   if (topicString == NULL) {
      return NULL;
   }

   while (fgets(line, LINE, file_ptr)) {
      if (strstr(line, topicString)) {
         printf("Content of line: %s\n", line);
      }
   }

   fclose(file_ptr);
   return NULL;
}
