#ifndef __EXTRACT_H__
#define __EXTRACT_H__

#include <jansson.h>
#include <time.h>

/* Topic defines */
// #define MAC_CLIENT_TOPIC "\"mac_client\""
#define WLAN_CLIENT "\"wlan_client_stat\""
#define LAN_CLIENT "\"lan_client_stat\""
#define INTERFACE_STAT "\"interface_stat\""
// #define NAP_0 "\"nap_wlan0\""
// #define NAP_1 "\"nap_wlan1\""
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

#define CHANNEL5G "5G"
#define CHANNEL2G "2.4G"
#define LINE 4096

enum
{
   // GET_MAC,
   GET_WLAN,
   GET_LAN,
   GET_INTERFACE,
   // GET_NAP_0,
   // GET_NAP_1,
   GET_IP,
   // GET_REGISTER,
   GET_SSIDS,
   GET_MEM,
   GET_CPU,
   GET_CHANNEL_USAGE,
   ALL,
   // DEFAULT,
};

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

/* Parsers Information in Json form store in char pointer to struct */
void ParserInterfaceStats(interface_stat_t *interface, json_t *value, int index_interface);
void ParserInterfaceNew(json_t *value, const char *key, interface_stat_t *interface_temp);
/* Search the index Interface_arr that has the key */
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
/* Get the data of Wlan client Topic after process */
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

/*General Function Call defines */
json_t *FindByTopics(int TopicId, char *log);
int FindByTimestamp(long long Timestamp, int range, char *log);
char *FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range,
                               char *log);

/* Extract all data from all Topic spec by time and time range*/
json_t *ExtractData(time_t time, int range, int topic);
#endif