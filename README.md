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
