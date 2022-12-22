#ifndef __EXTRACT_H__
#define __EXTRACT_H__

#include <jansson.h>
#include <time.h>

/* Topic defines */
// #define MAC_CLIENT_TOPIC "\"mac_client\""
#define WLAN_CLIENT "\"wlan_client_stat\""  // ok
#define LAN_CLIENT "\"lan_client_stat\""    // ok
#define INTERFACE_STAT "\"interface_stat\"" // ok
#define NAP_0 "\"nap_wlan0\""
#define NAP_1 "\"nap_wlan1\""
#define IP_STAT "\"ip_stat\"" // ok
// #define REGISTER "\"register\""
#define SSIDS "\"get_ssids_stat\"" // ok
#define MEM "\"info_mem\""         // ok
#define CPU "\"info_cpu\""         // ok
#define CHANNEL_USAGE "\"get_channel_usage\""
#define LOGFILE "[LOG_INFO]"

/* Keep log file define */
#define FILE_EMPTY 1
#define FILE_OK 0
#define DATE_FORMAT "yyyy-MM-dd HH:mm:ss"

#define CHANNEL5G "5G"
#define CHANNEL2G "2.4G"

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
} interface_stat_t;

/* Print Interface struct */
/* Parsers Information in Json form store in char pointer to struct */
void ParserInterfaceStats(interface_stat_t *interface, json_t *value, int index_interface);
void ParserInterfaceNew(json_t *value, const char *key, interface_stat_t *interface_temp);
/* Search the index Interface_arr that has the key
 *(interface[index].interface == key) -> return index */
int SearchInterfaces(interface_stat_t *stat, const char *key_s);
/* Get the data of InterfaceTopic after process */
json_t *ExtractInterfaceData(time_t time, int range);

/******** GET WLAN TOPIC DEFINES ********/
typedef struct
{
   const char *mac_name;
   json_t *last_record;
   const char *host_name;
   unsigned long long txBytes;
   unsigned long long rxBytes;
   json_t *rssi;
} wlan_client_stat_t;

json_t *ExtractWlanData(time_t time, int range);
void ParserWlanClientStats(wlan_client_stat_t *wlan_clients, json_t *value, int index_interface);
void ParserWlanClientStatsNew(json_t *value, const char *key, wlan_client_stat_t *wlan_clients_temp);

// int SearchMacName(Wlan_Client_Stat *stat, int ArrSize, const char *key_s);
int SearchMacName(wlan_client_stat_t *stat, const char *key_s);

/******** GET SSIDS STAT TOPIC DEFINES ********/
json_t *ExtractSsidData(time_t time, int range);

/******** GET LAN STAT TOPIC DEFINES ********/
typedef struct
{
   unsigned long long last_record;
   const char *interface;
   const char *mac_name;
   const char *ip;
} lan_client_stat_t;

json_t *ExtractLanData(time_t time, int range);
void ParserLanClientStatsNew(json_t *value, const char *key, lan_client_stat_t *lan_clients_temp);
int SearchMacName_Lan(lan_client_stat_t *stat, const char *key_s);

/******** GET IP STAT TOPIC DEFINES ********/
json_t *ExtractIpData(time_t time, int range);

/******** GET MEM STAT TOPIC DEFINES ********/
json_t *ExtractMemData(time_t time, int range);

/******** GET CPU STAT TOPIC DEFINES ********/
json_t *ExtractCpuData(time_t time, int range);

/******** GET CPU STAT TOPIC DEFINES ********/
typedef struct
{
   const char *channel;
   json_t *free_time;
   json_t *interence_time;
   json_t *dut_time;
   json_t *other_ap_time;
} channel_usage_t;

json_t *ExtractChannelUsageData(time_t time, int range);
void ChannelUsageInit(channel_usage_t *channel, const char *channeName);
void ChannelUsageClear(channel_usage_t *channel);
/* Function Call defines */
json_t *FindByTopics(int TopicId, char *log);
int FindByTimestamp(long long Timestamp, int range, char *log);

/*
   ***Passing the TopicId which is defined as enum type.
*Find by Topic and Timestamp is finding all line of log
that match the topicID and the range of (Timestamp- range) -> (Timestamp +
range)
 */
char *FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range,
                               char *log);

void ExtractAllData();
#endif