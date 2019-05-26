#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QString>
#include <QDebug>

#include "v2hjsondata.h"

const char *json_filename_01 = "../json_data/GetServiceFlag.json";
const char *json_filename_02 = "../json_data/AppliancesList.json";
const char *json_filename_03 = "../json_data/AppOperation_OK.json";

static const QString GROUP_KETING("客厅");
static const QString GROUP_WOSHI("卧室");
static const QString GROUP_ERTONGFANG("儿童房");

static const QString TYPE_CHAZUO("插座");
static const QString TYPE_DENG("灯");


int load_json(const char *json_filename);

int main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);

    load_json(json_filename_02);

    return 0;
}

int load_json(const char *json_filename)
{
    QFile file(json_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return -1;
    }

    QByteArray jsondata = file.readAll();
    file.close();

    if (jsondata.isEmpty() != true){

#if 0
        const cJSON *data = NULL;
        const cJSON *appliances = NULL;

        cJSON *monitor_json = cJSON_Parse(jsondata.constData());
        if (monitor_json == NULL)
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                V2H_Debug("Error before: %s", errorsetV2HAppliancesListJsonData_ptr);
            }
            return -1;
        }

        data = cJSON_GetObjectItemCaseSensitive(monitor_json, "data");
        if ((false == cJSON_IsNull(data)) && (true == cJSON_IsObject(data)))
        {
            appliances = cJSON_GetObjectItemCaseSensitive(data, "appliances");

            if ((false == cJSON_IsNull(appliances)) && (true == cJSON_IsArray(appliances)))
            {
                int size = cJSON_GetArraySize(appliances);

            }
        }
#endif

        bool result = V2HJsonData::setV2HAppliancesListJsonData(jsondata.constData());

        if (true == result){
            V2H_Debug("V2HJsonData::setV2HAppliancesListJsonData Success.");

            V2HJsonData::setSelectApplianceID(QString("00000000000000000000780f77fc66d7"));
            //V2HJsonData::setSelectApplianceID(QString("008bb7698fa34a2bbe97ff3766e88850"));

            bool setGroupFilter = false;
            setGroupFilter = V2HJsonData::setGroupNameFilter(GROUP_KETING);
            Q_UNUSED(setGroupFilter);

            ApplianceInfo applianceinfo = V2HJsonData::getSelectedApplianceInfo();
            QList<MappedInfo> modelist = V2HJsonData::getSelectedApplianceModeList();

            QList<ApplianceInfo> groupapplilancelist = V2HJsonData::getGroupedAppliancesInfoList();

            qDebug() << "groupapplilancelist.size():" << groupapplilancelist.size();
            Q_UNUSED(applianceinfo);
            Q_UNUSED(modelist);


            bool setTypeFilter = false;
            setTypeFilter = V2HJsonData::setApplianceTypeFilter(TYPE_CHAZUO);
            Q_UNUSED(setTypeFilter);

            QList<ApplianceInfo> typeapplilancelist = V2HJsonData::getTypedAppliancesInfoList();

            qDebug() << "typeapplilancelist.size():" << typeapplilancelist.size();
        }
    }

    return 0;
}
