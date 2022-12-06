#ifndef __EXTRACT_H__
#define __EXTRACT_H__

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
/* Function call define */
char* FindByTopics(int Topic, char* log);
char* FindByTimestamp(char* Timestamp, char* log);
char* FindByTopicsAndTimestamp(char* Topic, char* Timestamp, char* log);

#endif