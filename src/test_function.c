#include <string.h>
#include "extract.h"
#include "vec.h"

#define LOG_PATH "./log/mqtt_log"

char *ExtractWlanData(time_t time, int range)
{
    /* GET OBJECT RAW DATA FROM  LOG FILE */
    json_t *resul = FindByTopicsAndTimestamp(GET_WLAN, time, range, LOG_PATH);
    if (resul == NULL)
    {
        fprintf(stderr, "ERROR: Could not find log file\n");
        return NULL;
    }

    json_t *returnArray = json_object_get(resul, "Retturn");
    if (returnArray == NULL)
    {
        printf("ERROR: Could not parse return array\n");
        return NULL;
    }
    // DumpToFile("wlan.json", returnArray);

    /* GET MAX ELEMENT OF ALL OBJECTS TO CREATE ARRAY  STRUCT */

    Wlan_Client_Stat *wlan_clients = vector_create();

    // wlan_clients_temp = vector_add_asg(&wlan_clients);
    json_t *time_array = json_array();

    size_t index;
    json_t *value;

    json_array_foreach(returnArray, index, value)
    {

        const char *key;
        json_t *value_object;
        if (vector_size(wlan_clients) == 0)
        {

            json_object_foreach(value, key, value_object)
            {

                if (strstr(key, "time"))
                {
                    json_array_append_new(time_array, json_deep_copy(value_object));
                }
                else
                {
                    Wlan_Client_Stat *wlan_clients_temp = vector_add_asg(&wlan_clients);
                    unsigned int lastRecords = json_integer_value(json_object_get(value_object, "last_record"));
                    wlan_clients_temp->last_record = lastRecords;
                    wlan_clients_temp->rssi = json_array();
                    wlan_clients_temp->mac_name = key;
                    wlan_clients_temp->host_name = json_string_value(json_object_get(value_object, "Hostname"));
                    wlan_clients_temp->txBytes = json_integer_value(json_object_get(value_object, "TxBytes"));
                    wlan_clients_temp->rxBytes = json_integer_value(json_object_get(value_object, "RxBytes"));
                    json_array_append_new(wlan_clients_temp->rssi, json_object_get(value_object, "RSSI"));
                    wlan_clients_temp = NULL;
                }
            }
        }
        else
        {

            json_object_foreach(value, key, value_object)
            {
                if (strstr(key, "time"))
                {
                    json_array_append_new(time_array, json_deep_copy(value_object));
                }
                else
                {
                    printf("key: %s\n", key);
                    int index_interface = SearchMacName(wlan_clients, key);
                    if (index_interface < 0)
                    {

                        Wlan_Client_Stat *wlan_clients_temp_2 = vector_add_asg(&wlan_clients);
                        unsigned int lastRecords = json_integer_value(json_object_get(value_object, "last_record"));
                        wlan_clients_temp_2->rssi = json_array();
                        wlan_clients_temp_2->last_record = lastRecords;
                        wlan_clients_temp_2->mac_name = key;
                        wlan_clients_temp_2->host_name = json_string_value(json_object_get(value_object, "Hostname"));
                        wlan_clients_temp_2->txBytes = json_integer_value(json_object_get(value_object, "TxBytes"));
                        wlan_clients_temp_2->rxBytes = json_integer_value(json_object_get(value_object, "RxBytes"));
                        json_array_append_new(wlan_clients_temp_2->rssi, json_object_get(value_object, "RSSI"));
                        // json_decref(wlan_clients_temp_2->rssi);
                        // wlan_clients_temp_2 = NULL;
                        // wlan_clients_temp_2 = NULL;
                    }
                    else
                    {
                        printf("\nindex interface value: %d\n", index_interface);
                        unsigned int lastRecords = json_integer_value(json_object_get(value_object, "last_record"));
                        wlan_clients[index_interface].last_record = lastRecords;
                        wlan_clients[index_interface].txBytes = json_integer_value(json_object_get(value_object, "TxBytes"));
                        wlan_clients[index_interface].rxBytes = json_integer_value(json_object_get(value_object, "RxBytes"));
                        json_array_append_new(wlan_clients[index_interface].rssi, json_object_get(value_object, "RSSI"));
                    }
                }
            }
        }
        //
    }

    json_t *arrayROOT = json_array();

    for (int c = 0; c < vector_size(wlan_clients); c++)
    {
        json_t *returnROOT = json_object();
        json_object_set_new(returnROOT, "Macname", json_string(wlan_clients[c].mac_name));
        json_object_set_new(returnROOT, "Hostname", json_string(wlan_clients[c].host_name));
        json_object_set_new(returnROOT, "TxBytes", json_integer(wlan_clients[c].txBytes));
        json_object_set_new(returnROOT, "RxBytes", json_integer(wlan_clients[c].rxBytes));
        json_object_set_new(returnROOT, "Timestamp", json_integer(wlan_clients[c].last_record));
        json_object_set_new(returnROOT, "RSSI", wlan_clients[c].rssi);
        json_object_set_new(returnROOT, "Timeline", time_array);
        json_array_append_new(arrayROOT, returnROOT);
    }

    DumpToFile("FinalFilter_WLAN.json", arrayROOT);

    json_decref(arrayROOT);
    json_decref(returnArray);
    json_decref(resul);
    return NULL;
}