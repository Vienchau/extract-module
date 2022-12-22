#include <string.h>
#include "extract.h"
#include "vec.h"

#define LOG_PATH "./log/mqtt_log"

// Here is where to test new features function
void ExtractAllData()
{
    json_t *interface_json = ExtractInterfaceData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
    json_t *wlan_json = ExtractWlanData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
    json_t *ssid_json = ExtractSsidData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
    json_t *lan_json = ExtractLanData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
    json_t *ip_json = ExtractIpData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
    json_t *mem_json = ExtractMemData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
    json_t *cpu_json = ExtractCpuData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));
    json_t *channel_json = ExtractChannelUsageData(ConvertDatetoEpoch(2022, 12, 13, 0), ConvertHoursToSecond(24));

    json_t *root = json_object();
    json_object_set_new(root, "interface_stat", interface_json);
    json_object_set_new(root, "wlan_client_stat", wlan_json);
    json_object_set_new(root, "get_ssids_stat", ssid_json);
    json_object_set_new(root, "lan_client_stat", lan_json);
    json_object_set_new(root, "ip_stat", ip_json);
    json_object_set_new(root, "info_mem", mem_json);
    json_object_set_new(root, "info_cpu", cpu_json);
    json_object_set_new(root, "get_channel_usage", channel_json);

    DumpToFile("result.json", root);
    json_decref(root);
}