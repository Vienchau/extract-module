## Extract log file written by C

### Log format:

```bash
[2022-12-05 17:30:29]: [LOG_INFO]: {"topic":"mac_client","data":[{"mac_cpe":"a4:81:7a:7b:c5:b6","ClientId":1,"Hostname":"DESKTOP-3A3JO0P","Ip":"192.168.1.223","Channel":48,"Mac":"74:c6:3b:b6:69:71","TxBytes":151455,"RxBytes":211423,"TxPkts":344,"RxPkts":795,"RSSI":-68,"rx_bw":"80M","tx_bw":"","rx_rate":"40M","tx_rate":"VHTNSS1-MCS288"}]}
```

### Function:

```c
char* FindByTopicsAndTimestamp(int Topic, long long Timestamp, int range, char* log);
```

**Parameters:**

- `Topic`: Topic ID that identifies the topic to filter. If specified `ALL`, the function will return all log file in range of timestamp.
- `Timestamp`: Specified by epoch timestamp.
- `range`: Range of log file. The function will filter in range of (Timestamp - range) ---- (Timestamp + range).
- `log`: The path to log file.
- `Return`: the char pointer holding log after filtering.

```c
time_t ConvertDatetoEpoch(int year, int month, int day, int hour)
```

- `Return`: Return the epoch timestamp (sencods) from date passed to the function.
- Example:

```c
//17h 5/12/2022
ConvertDatetoEpoch(2022, 12, 5, 17);
//return 1670234400
```

### Char pointer after filtering by FindByTopicsAndTimestamp():

```c
   char* resul = FindByTopicsAndTimestamp(GET_INTERFACE,ConvertDatetoEpoch(2022, 12, 5, 17),ConvertHoursToSecond(1), LOG_PATH);
```

```bash
[{"topic":"interface_stat","data":{"wlan0":{"TxPkts":19618,"RxPkts":60338,"TxBytes":18910308,"RxBytes":11293971,"TxErrors":96,"RxErrors":9907,"TxDrops":0,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236229},"wlan1":{"TxPkts":0,"RxPkts":230432,"TxBytes":0,"RxBytes":62650238,"TxErrors":0,"RxErrors":0,"TxDrops":0,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236229},"lan0":{"TxPkts":2934,"RxPkts":0,"TxBytes":772243,"RxBytes":0,"TxErrors":0,"RxErrors":0,"TxDrops":11,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236229},"lan1":{"TxPkts":2999,"RxPkts":116,"TxBytes":778326,"RxBytes":9630,"TxErrors":0,"RxErrors":0,"TxDrops":8,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236229},"lan2":{"TxPkts":4462,"RxPkts":2326,"TxBytes":996550,"RxBytes":222073,"TxErrors":0,"RxErrors":0,"TxDrops":9,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236229},"lan3":{"TxPkts":2934,"RxPkts":0,"TxBytes":772243,"RxBytes":0,"TxErrors":0,"RxErrors":0,"TxDrops":11,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236229},"ppp0":{"TxPkts":1118,"RxPkts":827,"TxBytes":87708,"RxBytes":187492,"TxErrors":0,"RxErrors":0,"TxDrops":0,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236229}},"time":"2022-12-05 17:30:29"},{"topic":"interface_stat","data":{"wlan0":{"TxPkts":21353,"RxPkts":71685,"TxBytes":19793137,"RxBytes":13326707,"TxErrors":119,"RxErrors":12669,"TxDrops":0,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236825},"wlan1":{"TxPkts":0,"RxPkts":285367,"TxBytes":0,"RxBytes":76356720,"TxErrors":0,"RxErrors":0,"TxDrops":0,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236825},"lan0":{"TxPkts":3577,"RxPkts":0,"TxBytes":929041,"RxBytes":0,"TxErrors":0,"RxErrors":0,"TxDrops":13,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236825},"lan1":{"TxPkts":3651,"RxPkts":139,"TxBytes":935638,"RxBytes":11306,"TxErrors":0,"RxErrors":0,"TxDrops":10,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236825},"lan2":{"TxPkts":5121,"RxPkts":2370,"TxBytes":1154419,"RxBytes":225342,"TxErrors":0,"RxErrors":0,"TxDrops":11,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236825},"lan3":{"TxPkts":3577,"RxPkts":0,"TxBytes":929041,"RxBytes":0,"TxErrors":0,"RxErrors":0,"TxDrops":13,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236825},"ppp0":{"TxPkts":1811,"RxPkts":1280,"TxBytes":202754,"RxBytes":325571,"TxErrors":0,"RxErrors":0,"TxDrops":0,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670236825}},"time":"2022-12-05 17:40:25"},{"topic":"interface_stat","data":{"wlan0":{"TxPkts":21353,"RxPkts":83560,"TxBytes":19793137,"RxBytes":15206710,"TxErrors":119,"RxErrors":17637,"TxDrops":0,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670237425},"wlan1":{"TxPkts":0,"RxPkts":329474,"TxBytes":0,"RxBytes":87438049,"TxErrors":0,"RxErrors":0,"TxDrops":0,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670237425},"lan0":{"TxPkts":3644,"RxPkts":0,"TxBytes":944863,"RxBytes":0,"TxErrors":0,"RxErrors":0,"TxDrops":13,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670237425},"lan1":{"TxPkts":3726,"RxPkts":163,"TxBytes":951884,"RxBytes":13012,"TxErrors":0,"RxErrors":0,"TxDrops":10,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670237425},"lan2":{"TxPkts":5190,"RxPkts":2387,"TxBytes":1170382,"RxBytes":226499,"TxErrors":0,"RxErrors":0,"TxDrops":11,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670237425},"lan3":{"TxPkts":3644,"RxPkts":0,"TxBytes":944863,"RxBytes":0,"TxErrors":0,"RxErrors":0,"TxDrops":13,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670237425},"ppp0":{"TxPkts":1918,"RxPkts":1364,"TxBytes":212179,"RxBytes":358395,"TxErrors":0,"RxErrors":0,"TxDrops":0,"RxDrops":0,"TxCompressed":0,"RxCompressed":0,"last_record":1670237425}},"time":"2022-12-05 17:50:25"}]
```

### Extract interface topic data into JSON array:

```c
char *ExtractInterfaceData(time_t time, int range)
```

**Parameters:**

- `time`: Specified by epoch timestamp.
- `range`: Range of log file. The function will filter in range of (Timestamp - range) ---- (Timestamp + range).
- `return`: Return char pointer hold JSON formed like:

```bash
[
    {
        "Interface": "wlan0",
        "TxBytes": [
            18910308,
            19793137,
            19793137,
            19793137,
            19793137,
            ...],
        "RxBytes": [
            11293971,
            13326707,
            15206710,
            16832929,
            18337700,
            ...],
        "TxErrors": [
            96,
            119,
            119,
            119,
            119,
            119,
            ...],
         "RxErrors": [
            9907,
            12669,
            17637,
            20523,
            22668,
            24461,
            26324,
            ...],
         "Timestamp": 1670291424,
         "Timeline": [
            "2022-12-05 17:30:29",
            "2022-12-05 17:40:25",
            "2022-12-05 17:50:25",
            ...],
    },
    {
        "Interface": "wlan1",
        "TxBytes": [
            0,
            0,
            0,
            ...],
            ...},
]
```

### Extract Wlan client topic data into JSON array:

```c
char *ExtractWlanData(time_t time, int range)
```

**Parameters:**

- `time`: Specified by epoch timestamp.
- `range`: Range of log file. The function will filter in range of (Timestamp - range) ---- (Timestamp + range).
- `return`: Return char pointer hold JSON formed like:

```bash
[
    {
        "Macname": "32:da:e5:7e:63:a2",
        "Hostname": "unknown_32:da:e5:7e:63:a2",
        "TxBytes": 32986558,
        "RxBytes": 29915893,
        "Timestamp": [
            1670894195,
            1670894795,
            1670895395,
            1670895995,
            1670896595,
            1670897195,
            1670897795,
            1670898395,
            1670898995,
            1670899595,
            1670900195
        ],
        "RSSI": [
            -62,
            -73,
            -62,
            -63,
            -60,
            -63,
            -62,
            -65,
            -63,
            -74,
            -74
        ]
    },
    {
            ...},
]
```
