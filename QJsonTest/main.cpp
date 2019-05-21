#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <QString>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

const char *json_filename_01 = "../json_data/GetServiceFlag.json";
const char *json_filename_02 = "../json_data/AppliancesList.json";
const char *json_filename_03 = "../json_data/AppOperation_OK.json";

int load_json(const char *json_filename);
char* ReadFile(const char *filename);

int main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);
    qDebug("cJSON Test!");

    load_json(json_filename_02);

    return 0;
}

int load_json(const char *json_filename)
{
    char *json_buffer = NULL;

    json_buffer = ReadFile(json_filename);

    if (json_buffer != NULL){
        cJSON *json = cJSON_Parse(json_buffer);

        qDebug().noquote() << json_buffer;

        QJsonDocument json_doc = QJsonDocument::fromJson(QByteArray(json_buffer));
        QJsonObject json_obj;

        if (true == json_doc.isObject() && false == json_doc.isNull()){
            QJsonObject json_obj = QJsonDocument::fromJson(QByteArray(json_buffer)).object();
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
