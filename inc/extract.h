#ifndef __EXTRACT_H__
#define __EXTRACT_H__

#include <jansson.h>
#include <time.h>

/* Topic defines */
// #define MAC_CLIENT_TOPIC "\"mac_client\""
#define WLAN_CLIENT "\"wlan_client_stat\""
#define LAN_CLIENT "\"lan_client_stat\""
#define INTERFACE_STAT "\"interface_stat\""
#define NAP_0 "\"nap_wlan0\""
#define NAP_1 "\"nap_wlan1\""
#define IP_STAT "\"ip_stat\""
// #define REGISTER "\"register\""
#define SSIDS "\"get_ssids_stat\""
#define MEM "\"info_mem\""
#define CPU "\"info_cpu\""
#define CHANNEL_USAGE "\"get_channel_usage\""
#define LOGFILE "[LOG_INFO]"

/* Keep log file define */
#define FILE_EMPTY 1
#define FILE_OK 0
#define DATE_FORMAT "yyyy-MM-dd HH:mm:ss"

#define LOG_CYCLE_MINUTES 10 // 10 MINUTES
#define LOG_LAST 1           // 2 DAYS
#define EMPTY_INTERFACE "N/a"
#define ARRAY_MAX_SIZE (LOG_LAST * 24) * 60 / LOG_CYCLE_MINUTES

enum
{
   // GET_MAC,
   GET_WLAN,
   GET_LAN,
   GET_INTERFACE,
   GET_NAP_0,
   GET_NAP_1, // false
   GET_IP,
   // GET_REGISTER,       // false
   GET_SSIDS,         // false
   GET_MEM,           // false
   GET_CPU,           // false
   GET_CHANNEL_USAGE, // false
   ALL,
   // DEFAULT,
};

#define LINE 4096

/* Utils define */
/* Remove Char from String */
void RemoveChar(char *des, char remove);
/* Convert ID to Topic string */
char *ConvertIDTopic(int topic);
/* Convert Hours to Senconds */
int ConvertHoursToSecond(int hour);
/* Convert Date to TimeStamp Epoch (seconds) */
time_t ConvertDatetoEpoch(int year, int month, int day, int hour);
/* Print Json object in Jansson */
void print_json(json_t *root);
/* Dump Json Object in Jansson to File */
void DumpToFile(char *path, json_t *root);

/******** GET INTERFACE TOPIC DEFINES ********/
typedef struct
{
   const char *interface;
   json_t *last_record;
   json_t *txBytes;
   json_t *rxBytes;
   json_t *txErrors;
   json_t *rxErrors;

} Interface_Stat;

/* Print Interface struct */
/* Parsers Information in Json form store in char pointer to struct */
/* Index is the index of array data Ex: TxBytes = [1,2,3 ...index] */
void ParserInterfaceStats(Interface_Stat *interface, json_t *value, int index_interface);
void ParserInterfaceNew(Interface_Stat **interface, json_t *value, const char *key, Interface_Stat *interface_temp);
/* Search the index Interface_arr that has the key
 *(interface[index].interface == key) -> return index */
int SearchInterfaces(Interface_Stat *stat, const char *key_s);
/* Get the max element in one Object
 *[{
   "wlan0": {},
   "wlan1": {},
   "lan0": {},
   "lan1": {},
   "lan2": {},
   "lan3": {},
   "ppp0": {},
   "time": "2022-12-05 17:30:29"
    },{
   "wlan0": {},
   "wlan1": {},
   "lan0": {},
   "lan1": {},
   "time": "2022-12-05 17:30:29"
    },]
*MaxElements = 8
 * */
int GetMaxElementObject(json_t *root);
/* Get the data of InterfaceTopic after process */
char *ExtractInterfaceData(time_t time, int range);

/******** GET WLAN TOPIC DEFINES ********/
typedef struct
{
   const char *mac_name;
   json_t *last_record;
   const char *host_name;
   unsigned long long txBytes;
   unsigned long long rxBytes;
   json_t *rssi;
} Wlan_Client_Stat;

char *ExtractWlanData(time_t time, int range);
void ParserWlanClientStats(Wlan_Client_Stat *wlan_clients, json_t *value, int index_interface);
void ParserWlanClientStatsNew(Wlan_Client_Stat **wlan_clients, json_t *value, const char *key, Wlan_Client_Stat *wlan_clients_temp);

// int SearchMacName(Wlan_Client_Stat *stat, int ArrSize, const char *key_s);
int SearchMacName(Wlan_Client_Stat *stat, const char *key_s);

/* Function Call defines */
json_t *FindByTopics(int TopicId, char *log);
int FindByTimestamp(long long Timestamp, int range, char *log);
/*
   ***Passing the TopicId which is defined as enum type:
   GET_MAC for "mac_client"
   GET_WLAN for "wlan_client_stat"
   GET_INTERFACE for "lan_client_stat"
   GET_NAP_0 for "interface_stat"
   GET_NAP_1 for "nap_wlan0"
   GET_LAN for "nap_wlan1"
   GET_IP for "ip_stat"
   GET_REGISTER for "register"
   GET_SSIDS for "get_ssids_stat"
   GET_MEM for "info_mem"
   GET_CPU for "info_cpu"
   GET_CHANNEL_USAGE for "get_channel_usage"
*Find by Topic and Timestamp is finding all line of log
that match the topicID and the range of (Timestamp- range) -> (Timestamp +
range)
 */
char *FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range,
                               char *log);

#endif