#include "extract.h"

#define LOG_CYCLE_MINUTES 10
#define LOG_LAST 2

const int ARRAY_MAX_SIZE = (LOG_LAST * 24) * 60 / LOG_CYCLE_MINUTES;

typedef struct {
   char* interface;
   long long TxBytes[ARRAY_MAX_SIZE];
   long long RxBytes[ARRAY_MAX_SIZE];
   long long TxErrors[ARRAY_MAX_SIZE];
   long long RxErrors[ARRAY_MAX_SIZE];
   long long LastRecords;
} Interface_Stat;

char* ExtractInterfaceData() {}