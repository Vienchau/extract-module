## Extract log file written by C

### Log format:

```bash
[2022-12-13 07:58:38]: [LOG_INFO]: {"topic":"get_ssids_stat","data":[{"mac_cpe":"a8:25:eb:14:c3:2e","Wlan":"wlan1","TxPkts":0,"RxPkts":645737,"TxBytes":0,"RxBytes":79319643,"TxUnicastPkts":78643,"TxMulticastPkts":6,"TxBroadcastPkts":390,"TxRetries":0,"TxFailures":0,"TxDropped":8606,"TxMgmtFrames":79039,"Rx_ucast_pkts":632,"RxBroadcastPkts":5922955,"rx_retrys":181,"RxPktsErrored":0,"RxErrors":0,"RxPktsDropped":0,"RxMgmtFrames":5505531}]}
[2022-12-13 07:58:41]: [LOG_INFO]: {"topic":"get_ssids_stat","data":[{"mac_cpe":"a8:25:eb:14:c3:2e","Wlan":"wlan0","TxPkts":1038,"RxPkts":500481,"TxBytes":267465,"RxBytes":51968622,"TxUnicastPkts":35977,"TxMulticastPkts":122,"TxBroadcastPkts":376,"TxRetries":0,"TxFailures":36,"TxDropped":8396,"TxMgmtFrames":35438,"Rx_ucast_pkts":1957,"RxBroadcastPkts":1744578,"rx_retrys":551,"RxPktsErrored":112547,"RxErrors":112547,"RxPktsDropped":6,"RxMgmtFrames":1331616}]}
[2022-12-13 07:58:44]: [LOG_INFO]: {"topic":"info_mem","data":[{"mac_cpe":"a8:25:eb:14:c3:2e","total":174568,"used":71748,"free":57988,"shared":9112,"cache":44832,"available":98704}]}
[2022-12-13 07:58:47]: [LOG_INFO]: {"topic":"info_cpu","data":[{"mac_cpe":"a8:25:eb:14:c3:2e","current":6,"5minutes":6,"10minutes":6,"process":"1/109","lastprocess":"21142"}]}
[2022-12-13 07:59:09]: [LOG_INFO]: {"topic":"ping_lan","data":[{"mac_cpe":"a8:25:eb:14:c3:2e","client":"b0:7b:25:0d:8a:5b","lossrate":"100%","min":0.0,"avg":0.0,"max":0.0,"jitter":0.0},{"mac_cpe":"a8:25:eb:14:c3:2e","client":"da:c4:4e:73:bf:ff","lossrate":"0%","min":0.738,"avg":0.854,"max":1.138,"jitter":0.0},{"mac_cpe":"a8:25:eb:14:c3:2e","client":"54:46:17:fc:7b:75","lossrate":"0%","min":0.461,"avg":0.542,"max":1.032,"jitter":0.0}]}
[2022-12-13 07:59:09]: [LOG_INFO]: {"topic":"get_channel_usage","data":[{"mac_cpe":"a8:25:eb:14:c3:2e","Currentchannel":149,"interence_time":71,"DUT_Time":1,"other_APs_Time":10,"free_Time":18,"ch_load":11,"ch_load(includeinterence)":82}]}
```

### Function:

```c
json_t *ExtractData(time_t time, int range, int topic);
```

**Parameters:**

- `time`: Specified by epoch timestamp.
- `range`: Range of log file. The function will filter in range of (Timestamp - range) - (Timestamp + range).
- `topic`: Topic ID that identifies the topic to filter. If specified `ALL`, the function will return all topic in range of timestamp.
- `Return`: **json_t\*** pointer hold data after process.

**Possible topic types:**

```c
enum
{
   GET_WLAN,              //"wlan_client_stat"
   GET_LAN,               //"lan_client_stat"
   GET_INTERFACE,         //"interface_stat"
   GET_IP,                //"ip_stat"
   GET_SSIDS,             //"get_ssids_stat"
   GET_MEM,               //"info_mem"
   GET_CPU,               //"info_cpu"
   GET_CHANNEL_USAGE,     //"get_channel_usage"
   ALL,                   //All topic
};
```

### Example usage:

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "extract.h"

int main()
{
   /* Tue, 13 Dec 2022 00:00:00 GMT+7, 24 hours range */
   time_t time = ConvertDatetoEpoch(2022, 12, 13, 0);
   int range = ConvertHoursToSecond(24);
   json_t *filtering = ExtractData(time, range, GET_INTERFACE);
   DumpToFile("result.json", filtering);
   json_decref(filtering);
   return 0;
}
```

### Result:

```bash
{
    "interface_stat": [
        {
            "Interface": "wlan0",
            "TxBytes": [
                267465,
                1135031,
                73502212,
                81386745,
                ....
            ],
            "RxBytes": [
                51808273,
                65645571,
                112728630,
                142675425,
                ....
            ],
            "TxErrors": [
                36,
                41,
                226,
                227,
                ....
            ],
            "RxErrors": [
                112524,
                115504,
                118026,
                119795,
                ....
            ],
            "Timestamp": [
                1670893114,
                1670893714,
                1670894314,
                1670894914,
                ....
            ]
        },

}

```

### Convert Date to Epoch TimeStamp

```c
time_t ConvertDatetoEpoch(int year, int month, int day, int hour)
```

- `Return`: Return the epoch timestamp (sencods) from date passed to the function.

**Example:**

```c
//17h 5/12/2022
ConvertDatetoEpoch(2022, 12, 5, 17);
//return 1670234400
```
