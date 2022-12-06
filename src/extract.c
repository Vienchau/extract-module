#include "extract.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* FindByTopics(int Topic, char* log) {
   FILE *file_ptr, *file_temp;
   file_ptr = fopen(log, "r");
   file_temp = fopen("temp", "a+");
   char line[LINE];
   char* returnLines;
   long lSize;

   char* topicString = ConvertIDTopic(Topic);
   if (topicString == NULL) {
      return NULL;
   }

   while (fgets(line, LINE, file_ptr)) {
      if (strstr(line, topicString)) {
         char* linePointer = (char*)malloc(strlen(line) + 1);
         strcpy(linePointer, line);
         fputs(linePointer, file_temp);
         free(linePointer);
      }
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

   return returnLines;
}
