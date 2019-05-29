#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QString>
#include <QDebug>

#include "v2hjsondata.h"

const char *json_filename_01 = "../json_data/GetServiceFlag.json";
//const char *json_filename_02 = "../json_data/AppliancesList.json";
const char *json_filename_02 = "../json_data/AppliancesList_New.json";
const char *json_filename_03 = "../json_data/AppOperation_OK.json";

static const QString GROUP_KETING("客厅");
static const QString GROUP_WOSHI("卧室");
static const QString GROUP_ERTONGFANG("儿童房");
static const QString GROUP_WEIFENZU("未分组");

static const QString TYPE_CHAZUO("插座");
static const QString TYPE_DENG("灯");
static const QString TYPE_KONGTIAO("空调");


int load_json_01(const char *json_filename);
int load_json_02(const char *json_filename);
int load_json_03(const char *json_filename);

int main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);

    V2HJsonData::InitDeviceTypeMap();
    load_json_01(json_filename_01);
    load_json_02(json_filename_02);
    load_json_03(json_filename_03);

    return 0;
}

int load_json_01(const char *json_filename)
{
    QFile file(json_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return -1;
    }

    QByteArray jsondata = file.readAll();
    file.close();

    if (jsondata.isEmpty() != true){
        bool result = V2HJsonData::setV2HServiceFlagJsonData(jsondata.constData());

        if (true == result){
            V2H_Debug("V2HJsonData::setV2HServiceFlagJsonData Success.");
        }
    }

    return 0;
}

int load_json_02(const char *json_filename)
{
    QFile file(json_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return -1;
    }

    QByteArray jsondata = file.readAll();
    file.close();

    if (jsondata.isEmpty() != true){
#if 0
        cJSON *v2h_json = cJSON_Parse(jsondata.constData());
        if (v2h_json == NULL)
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                qDebug() << "Error before:" << error_ptr;
            }
            return -1;
        }
#endif

        bool result = V2HJsonData::setV2HAppliancesListJsonData(jsondata.constData());

        if (true == result){
            V2H_Debug("V2HJsonData::setV2HAppliancesListJsonData Success.");

            //V2HJsonData::setSelectApplianceID(QString("008bb7698fa34a2bbe97ff3766e88850"));
            //V2HJsonData::setSelectApplianceID(QString("00000000000000000000780f77fc66d7"));
            V2HJsonData::setSelectApplianceID(QString("140737488379182"));

            bool setGroupFilter = false;
            setGroupFilter = V2HJsonData::setGroupNameFilter(GROUP_WEIFENZU);
            Q_UNUSED(setGroupFilter);

            ApplianceInfo applianceinfo = V2HJsonData::getSelectedApplianceInfo();
            QList<MappedInfo> modelist = V2HJsonData::getSelectedApplianceModeList();

            QList<ApplianceInfo> groupapplilancelist = V2HJsonData::getGroupedAppliancesInfoList();

            qDebug() << "groupapplilancelist.size():" << groupapplilancelist.size();
            Q_UNUSED(applianceinfo);
            Q_UNUSED(modelist);

            MappedInfo currentmode = V2HJsonData::getSelectedApplianceCurrentMode();

            qDebug() << "currentmode.key:" << currentmode.key << "; value:" << currentmode.value;

            QList<ActionBar> actionbars = V2HJsonData::getSelectedApplianceActionBars();

            qDebug() << "actionbars number:" << actionbars.size();

            qDebug() << "getSelectedApplianceTurnOnOffSupported:" << V2HJsonData::getSelectedApplianceTurnOnOffSupported();
            qDebug() << "getSelectedApplianceSetModeSupported:" << V2HJsonData::getSelectedApplianceSetModeSupported();

            QList<Guideline> guideline = V2HJsonData::getSelectedApplianceGuideline();
            qDebug() << "SelectedApplianceGuideline size:" << guideline.size();

            bool setTypeFilter = false;
            setTypeFilter = V2HJsonData::setApplianceTypeFilter(TYPE_KONGTIAO);
            Q_UNUSED(setTypeFilter);

            QList<ApplianceInfo> typeapplilancelist = V2HJsonData::getTypedAppliancesInfoList();

            OperationRequest operationrequest;
            operationrequest.deviceId = applianceinfo.applianceId;
            operationrequest.deviceName = applianceinfo.friendlyName;
            operationrequest.groupName = applianceinfo.groupName;
            operationrequest.operation = QString("setMode");
            operationrequest.mode.deviceType = QString("AIR_CONDITION");
            operationrequest.mode.value = QString("HEAT");
            QByteArray operation_req_json = V2HJsonData::generateOperationRequestJson(operationrequest);
            qDebug() << "Request Json ByteArray:";
            qDebug() << operation_req_json.constData();


            OperationRequest operationrequest_1;
            operationrequest_1.deviceId = applianceinfo.applianceId;
            operationrequest_1.deviceName = applianceinfo.friendlyName;
            operationrequest_1.groupName = applianceinfo.groupName;
            operationrequest_1.operation = QString("incrementTemperature");
            operation_req_json = V2HJsonData::generateOperationRequestJson(operationrequest_1);
            qDebug() << "Request Json ByteArray1:";
            qDebug() << operation_req_json.constData();

            qDebug() << "GetServiceFlag Json ByteArray:";
            qDebug() << V2HJsonData::generateGetServiceFlagJson().constData();

            qDebug() << "GetAppliancesList Json ByteArray:";
            qDebug() << V2HJsonData::generateGetAppliancesListJson().constData();

            QString appliancetype = QString("微波炉");
            qDebug() << V2HJsonData::convertDeviceTypefromApplianceType(appliancetype);
        }
    }

    return 0;
}

int load_json_03(const char *json_filename)
{
    QFile file(json_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return -1;
    }

    QByteArray jsondata = file.readAll();
    file.close();

    if (jsondata.isEmpty() != true){
        bool result = V2HJsonData::setV2HApplianceOperationJsonData(jsondata.constData());

        if (true == result){
            V2H_Debug("V2HJsonData::setV2HApplianceOperationJsonData Success.");
        }
    }

    return 0;
}
