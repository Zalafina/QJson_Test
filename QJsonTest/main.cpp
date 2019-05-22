#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <QString>
#include <QDebug>

#include "v2hjsondata.h"

const char *json_filename_01 = "../json_data/GetServiceFlag.json";
const char *json_filename_02 = "../json_data/AppliancesList.json";
const char *json_filename_03 = "../json_data/AppOperation_OK.json";

int load_json(const char *json_filename);
char* ReadFile(const char *filename);

int main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);
    V2H_Debug("cJSON Test!");

    load_json(json_filename_02);

    return 0;
}

int load_json(const char *json_filename)
{
    char *json_buffer = NULL;

    json_buffer = ReadFile(json_filename);

    if (json_buffer != NULL){
        //cJSON *json = cJSON_Parse(json_buffer);

        //V2H_Debug().noquote() << json_buffer;

        bool result = V2HJsonData::setV2HJsonData(json_buffer);

        if (true == result){
            V2H_Debug("V2HJsonData::setV2HJsonData Success.");

            //V2H_Debug() << V2HJsonData::getJsonAppliancesList();

            //V2HJsonData::setSelectApplianceID(QString("00000000000000000000780f77fc66d7"));
            V2HJsonData::setSelectApplianceID(QString("008bb7698fa34a2bbe97ff3766e88850"));

            QString appliance_id = V2HJsonData::getSelectApplianceID();
            QJsonObject appliance_json;
            int appliance_index = V2HJsonData::getJsonApplianceFromID(appliance_id, appliance_json);

            if (appliance_index >= 0){
                V2H_Debug() << "Seleted Appliance Index:" << appliance_index;
                V2H_Debug() << "Seleted Appliance Json:" << appliance_json;
            }

            //V2H_Debug() << V2HJsonData::getJsonAppliancesList();
        }

        free(json_buffer);
    }

    return 0;
}

char* ReadFile(const char *filename)
{
   char *buffer = NULL;
   int string_size,read_size;
   FILE *handler = fopen(filename,"r");

   if (handler)
   {
       //seek the last byte of the file
       fseek(handler,0,SEEK_END);
       //offset from the first to the last byte, or in other words, filesize
       string_size = ftell (handler);
       //go back to the start of the file
       rewind(handler);

       //allocate a string that can hold it all
       buffer = (char*) malloc (sizeof(char) * (string_size + 1) );
       //read it all in one operation
       read_size = fread(buffer,sizeof(char),string_size,handler);
       //fread doesnt set it so put a \0 in the last position
       //and buffer is now officialy a string
       buffer[string_size+1] = '\0';

       if (string_size != read_size) {
           //something went wrong, throw away the memory and set
           //the buffer to NULL
           free(buffer);
           buffer = NULL;
        }
    }

    return buffer;
}
