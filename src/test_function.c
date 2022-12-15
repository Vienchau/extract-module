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
    /* GET MAX ELEMENT OF ALL OBJECTS TO CREATE ARRAY  STRUCT */
    Wlan_Client_Stat *wlan_clients = vector_create();

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
                Wlan_Client_Stat *wlan_clients_temp = vector_add_asg(&wlan_clients);
                ParserWlanClientStatsNew(&wlan_clients, value_object, key, wlan_clients_temp);
                wlan_clients_temp = NULL;
            }
        }
        else
        {
            json_object_foreach(value, key, value_object)
            {
                int index_interface = SearchMacName(wlan_clients, key);
                if (index_interface < 0)
                {
                    Wlan_Client_Stat *wlan_clients_temp_2 = vector_add_asg(&wlan_clients);
                    ParserWlanClientStatsNew(&wlan_clients, value_object, key, wlan_clients_temp_2);
                    wlan_clients_temp_2 = NULL;
                }
                else
                {
                    ParserWlanClientStats(wlan_clients, value_object, index_interface);
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
        json_object_set_new(returnROOT, "Timestamp", wlan_clients[c].last_record);
        json_object_set_new(returnROOT, "RSSI", wlan_clients[c].rssi);
        json_array_append_new(arrayROOT, returnROOT);
    }

    DumpToFile("FinalFilter_WLAN.json", arrayROOT);

    json_decref(arrayROOT);
    json_decref(returnArray);
    json_decref(resul);
    return NULL;
}

char *ExtractInterfaceData(time_t time, int range)
{
    /* GET OBJECT RAW DATA FROM  LOG FILE */
    json_t *resul = FindByTopicsAndTimestamp(GET_INTERFACE, time, range, LOG_PATH);
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
    /* GET MAX ELEMENT OF ALL OBJECTS TO CREATE ARRAY  STRUCT */

    /* CREATE ARRAY OF STRUCT TO HOLD  DATA */
    Interface_Stat *interface = vector_create();

    /* LOOP OVER ALL OBJECTS IN ARRAY */
    size_t index;
    json_t *value;
    int counter_object;
    json_array_foreach(returnArray, index, value)
    {
        const char *key;
        json_t *value_object;
        if (vector_size(interface) == 0)
        {
            json_object_foreach(value, key, value_object)
            {
                Interface_Stat *interface_temp = vector_add_asg(&interface);
                ParserInterfaceNew(&interface, value_object, key, interface_temp);
                        }
        }
        else
        {
            json_object_foreach(value, key, value_object)
            {
                int index_interface = SearchInterfaces(interface, key);
                if (index_interface < 0)
                {
                    Interface_Stat *interface_temp_2 = vector_add_asg(&interface);
                    ParserInterfaceNew(&interface, value_object, key, interface_temp_2);
                }
                else
                {
                    ParserInterfaceStats(interface, value_object, index_interface);
                }
            }
        }
    }

    json_t *arrayROOT = json_array();

    for (int c = 0; c < vector_size(interface); c++)
    {
        json_t *returnROOT = json_object();
        json_object_set_new(returnROOT, "Interface", json_string(interface[c].interface));
        json_object_set_new(returnROOT, "TxBytes", interface[c].txBytes);
        json_object_set_new(returnROOT, "RxBytes", interface[c].rxBytes);
        json_object_set_new(returnROOT, "TxErrors", interface[c].txErrors);
        json_object_set_new(returnROOT, "RxErrors", interface[c].rxErrors);
        json_object_set_new(returnROOT, "Timestamp", interface[c].last_record);
        json_array_append_new(arrayROOT, returnROOT);
    }

    DumpToFile("FinalFilter_INTERFACE.json", arrayROOT);

    json_decref(arrayROOT);
    json_decref(returnArray);
    json_decref(resul);
    return NULL;
}
