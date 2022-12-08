#ifndef __EXTRACT_H__
#define __EXTRACT_H__

#include <time.h>

/* Topic defines */
#define MAC_CLIENT_TOPIC "mac_client"
#define WLAN_CLIENT "wlan_client_stat"
#define LAN_CLIENT "lan_client_stat"
#define INTERFACE_STAT "interface_stat"
#define NAP_0 "nap_wlan0"
#define NAP_1 "nap_wlan1"
#define IP_STAT "ip_stat"
#define REGISTER "register"
#define SSIDS "get_ssids_stat"
#define MEM "info_mem"
#define CPU "info_cpu"
#define CHANNEL_USAGE "get_channel_usage"

/* Keep log file define */
#define RETURN_TEMP_FILE 0
#define RETURN_STRING 1

enum {
   GET_MAC,
   GET_WLAN,
   GET_LAN,
   GET_INTERFACE,
   GET_NAP_0,
   GET_NAP_1,
   GET_IP,
   GET_REGISTER,
   GET_SSIDS,
   GET_MEM,
   GET_CPU,
   GET_CHANNEL_USAGE,
   // DEFAULT,
};

#define LINE 1024

/* Utils define */
void RemoveChar(char* des, char remove);
char* ConvertIDTopic(int topic);
int ConvertHoursToSecond(int hour);
time_t ConvertDatetoEpoch(int year, int month, int day, int hour);

/* Function call define */
/*
 *FindByTopics:
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
   ***Passing the log path for sencond argument
*/
char* FindByTopics(int TopicId, char* log);
/*
 *Find all log line in range of (Timestamp - range) -> (Timestamp + range)
 *Timestamp and range was formed by Epoch Timestamp
 *Passing the log path for the third argument
 *Passing 1 for remove the temp log file and return the string holding data
 *Passing 0 for keep the temp log and return NULL
 */
char* FindByTimestamp(long long Timestamp, int range, char* log,
                      int keepTempFileFlag);

/*
*Find by Topic and Timestamp is finding all line of log
that match the topicID and the range of (Timestamp- range) -> (Timestamp +
range)
 */
char* FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range,
                               char* log);

#endif