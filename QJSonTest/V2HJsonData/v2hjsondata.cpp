#include "v2hjsondata.h"

static const QString V2H_LOGTAG("[HMI_V2H]");
static const QString V2H_LOGERROR("[ERROR]");

/* Common Keys */
static const QString KEY_STATUS("status");
static const QString KEY_DATA("data");

/* ServiceFlag  Keys */
static const QString KEY_SERVICE_FLAG("service_flag");
static const QString KEY_APP_LOGO_URL("app_logo_url");
static const QString KEY_APP_DL_URL("app_dl_url");
static const QString KEY_MAKER_APP_DL_URL("maker_app_dl_url");
static const QString KEY_HOME_LINK_TYPE("home_link_type");

/* AppliancesList Keys */
static const QString KEY_ISENABLE("isenable");
static const QString KEY_REDDOT("redDot");
static const QString KEY_APPLIANCES("appliances");
static const QString KEY_TOTALAPPLIANCES("totalAppliances");

/* Appliance Keys */
static const QString KEY_BOTNAME("botName");
static const QString KEY_BOTID("botId");
static const QString KEY_BOTLOGO("botLogo");
static const QString KEY_NUM("num");
static const QString KEY_APPLIANCEID("applianceId");
static const QString KEY_APPLIANCETYPES("applianceTypes");
static const QString KEY_FRIENDLYNAME("friendlyName");
static const QString KEY_FRIENDLYDESCRIPTION("friendlyDescription");
static const QString KEY_GROUPNAME("groupName");
static const QString KEY_GUIDELINE("guideline");
static const QString KEY_SUPPORTACTIONS("supportActions");
static const QString KEY_ATTRIBUTES("attributes");

/* Appliance Keys for cJSON */
static const char *CJSON_KEY_GUIDELINE = "guideline";
static const char *CJSON_KEY_SUPPORTACTIONS= "supportActions";
static const char *CJSON_KEY_ATTRIBUTES = "attributes";
static const char *CJSON_KEY_DETAIL = "detail";
static const char *CJSON_KEY_RANGE = "range";

/* Action Bar Number Keys */
static const QString KEY_ACTION_INCREMENT("increment");
static const QString KEY_ACTION_DECREMENT("decrement");

/* SupportActions Keys */
static const QString KEY_ACTION_TURNON("turnOn");
static const QString KEY_ACTION_TURNOFF("turnOff");
static const QString KEY_ACTION_SETMODE("setMode");

/* Attribute Names */
static const QString KEY_ATTR_MODE("mode");
static const QString KEY_ATTR_TURNONSTATE("turnOnState");

/* Attribute Subkeys */
static const QString KEY_DETAIL("detail");
static const QString KEY_VALUE("value");
static const QString KEY_DETAIL_NAME("name");
static const QString KEY_DETAIL_SCALE("scale");
static const QString KEY_DETAIL_RANGE("range");
static const QString KEY_DETAIL_RANGE_MIN("min");
static const QString KEY_DETAIL_RANGE_MAX("max");

/* Appliance Operation Keys */
static const QString KEY_RESULT("result");
static const QString KEY_TTS_STATUS("tts_status");
static const QString KEY_TTS_STATUS_TTS("tts");
static const QString KEY_ATTRIBUTE("attribute");

/* Common Request Keys for cJSON */
static const char *CJSON_IMEI = "imei";

/* Operation Request Keys for cJSON */
static const char *CJSON_DEVICEID = "deviceId";
static const char *CJSON_DEVICENAME = "deviceName";
static const char *CJSON_GROUPNAME = "groupName";
static const char *CJSON_OPERATION = "operation";
static const char *CJSON_OPERATIONPARAM = "operationParam";
static const char *CJSON_HOME_LINK_TYPE = "home_link_type";
static const char *CJSON_MODE = "mode";
static const char *CJSON_MODE_DEVICETYPE = "deviceType";
static const char *CJSON_MODE_VALUE = "value";

static const char *OPERATION_IMEI("XXXXXXXXXXXXXXXX");

static const int STATUS_OK = 0;
static const int HOMELINKTYPE_BAIDU = 1;
static const int HOMELINKTYPE_OTHER = 2;

QHash<QString, QString> V2HJsonData::m_DeviceTypeMap = QHash<QString, QString>();
QHash<QString, QString> V2HJsonData::m_AttributeMap = QHash<QString, QString>();
QHash<QString, QStringList> V2HJsonData::m_DeviceTypeGuidelineMap = QHash<QString, QStringList>();
QString V2HJsonData::m_V2H_AppliancesListJsonString = QString();
QString V2HJsonData::m_V2H_ServiceFlagJsonString = QString();
QString V2HJsonData::m_V2H_ApplianceOperationJsonString = QString();
ServiceFlag V2HJsonData::m_V2H_ServiceFlag = ServiceFlag();
QJsonObject V2HJsonData::m_V2H_JsonData = QJsonObject();
QJsonArray V2HJsonData::m_V2H_ApplianceArray = QJsonArray();
cJSON V2HJsonData::m_V2H_cJSONAppliancesArray = cJSON();
QList<ApplianceInfo> V2HJsonData::m_V2H_ApplianceInfoList = QList<ApplianceInfo>();
QList<ApplianceInfo> V2HJsonData::m_V2H_FiltedApplianceInfoList = QList<ApplianceInfo>();
QList<OperationResult> V2HJsonData::m_V2H_OperationResults = QList<OperationResult>();
QString V2HJsonData::m_GroupNameFilter = QString();
QStringList V2HJsonData::m_GroupNameList = QStringList();
QString V2HJsonData::m_ApplianceTypeFilter = QString();
QStringList V2HJsonData::m_ApplianceTypeList = QStringList();
QStringList V2HJsonData::m_AllGuidelineStrings = QStringList();
int V2HJsonData::m_SelectedApplianceIndex = -1;
ApplianceInfo V2HJsonData::m_SelectedApplianceInfo = ApplianceInfo();
bool V2HJsonData::m_V2H_AppliancesListIsEnable = false;
bool V2HJsonData::m_V2H_AppliancesListRedDot = false;
bool V2HJsonData::m_V2H_ServiceFlagUpdated = false;
bool V2HJsonData::m_V2H_AppliancesListUpdated = false;
bool V2HJsonData::m_V2H_ApplianceOperationUpdated = false;

QPixmap *V2HJsonData::m_V2H_BaiduLogo = Q_NULLPTR;

V2HJsonData::V2HJsonData(QObject *parent) : QObject(parent)
{

}

V2HJsonData::~V2HJsonData()
{

}

void V2HJsonData::Initialize()
{
    setV2HServiceFlagGeted(false);
    InitDeviceTypeMap();
    InitAttributeMap();

    if (Q_NULLPTR == m_V2H_BaiduLogo){
        m_V2H_BaiduLogo = new QPixmap();
    }
}

void V2HJsonData::InitDeviceTypeMap()
{
    m_DeviceTypeMap.insert(QString("灯"),              QString("LIGHT"));
    m_DeviceTypeMap.insert(QString("空调"),            QString("AIR_CONDITION"));
    m_DeviceTypeMap.insert(QString("洗衣机"),          QString("WASHING_MACHINE"));
    m_DeviceTypeMap.insert(QString("插座"),            QString("SOCKET"));
    m_DeviceTypeMap.insert(QString("电风扇"),          QString("FAN"));
    m_DeviceTypeMap.insert(QString("扫地机器人"),      QString("SWEEPING_ROBOT"));
    m_DeviceTypeMap.insert(QString("空气净化器"),      QString("AIR_PURIFIER"));
    m_DeviceTypeMap.insert(QString("电饭煲"),          QString("RICE_COOKER"));
    m_DeviceTypeMap.insert(QString("微波炉"),          QString("MICROWAVE_OVEN"));
    m_DeviceTypeMap.insert(QString("烤箱"),            QString("OVEN"));
    m_DeviceTypeMap.insert(QString("取暖器"),          QString("AIR_HEATER"));
    m_DeviceTypeMap.insert(QString("加湿器"),          QString("HUMIDIFIER"));
    m_DeviceTypeMap.insert(QString("热水器"),          QString("WATER_HEATER"));
    m_DeviceTypeMap.insert(QString("窗帘"),            QString("CURTAIN"));
    m_DeviceTypeMap.insert(QString("抽油烟机"),        QString("RANGE_HOOD"));
    m_DeviceTypeMap.insert(QString("电磁炉"),          QString("INDUCTION_COOKER"));
    m_DeviceTypeMap.insert(QString("电冰箱"),          QString("FRIDGE"));
    m_DeviceTypeMap.insert(QString("净水器"),          QString("WATER_PURIFIER"));
    m_DeviceTypeMap.insert(QString("伸缩衣架"),        QString("CLOTHES_RACK"));
    m_DeviceTypeMap.insert(QString("其他家电"),        QString("OTHER"));
}

void V2HJsonData::InitAttributeMap()
{
    m_AttributeMap.insert(QString("turnOn"),                                QString("turnOnState"));
    m_AttributeMap.insert(QString("turnOff"),                               QString("turnOnState"));
    m_AttributeMap.insert(QString("incrementBrightnessPercentage"),         QString("brightness"));
    m_AttributeMap.insert(QString("decrementBrightnessPercentage"),         QString("brightness"));
    m_AttributeMap.insert(QString("setBrightnessPercentage"),               QString("brightness"));
    m_AttributeMap.insert(QString("incrementTemperature"),                  QString("targetTemperature"));
    m_AttributeMap.insert(QString("decrementTemperature"),                  QString("targetTemperature"));
    m_AttributeMap.insert(QString("setTemperature"),                        QString("targetTemperature"));
    m_AttributeMap.insert(QString("incrementFanSpeed"),                     QString("fanSpeed"));
    m_AttributeMap.insert(QString("decrementFanSpeed"),                     QString("fanSpeed"));
    m_AttributeMap.insert(QString("setFanSpeed"),                           QString("fanSpeed"));
    m_AttributeMap.insert(QString("charge"),                                QString("electricityCapacity"));
    m_AttributeMap.insert(QString("discharge"),                             QString("electricityCapacity"));
    m_AttributeMap.insert(QString("setSuction"),                            QString("suction"));
    m_AttributeMap.insert(QString("setWaterLevel"),                         QString("waterLevel"));
    m_AttributeMap.insert(QString("incrementStatus"),                       QString("status"));
    m_AttributeMap.insert(QString("decrementStatus"),                       QString("status"));

    /* Temp Recovery */
    m_AttributeMap.insert(QString("targetTemperature"),                     QString("temperature"));
}

void V2HJsonData::Deinitialize()
{
    if (m_V2H_BaiduLogo != Q_NULLPTR){
        delete m_V2H_BaiduLogo;
    }
}

cJSON V2HJsonData::makecJSONAppliancesListArray(const char *json_buffer)
{
    cJSON applianceslist;
    const cJSON *data = NULL;
    const cJSON *appliances = NULL;

    cJSON *monitor_json = cJSON_Parse(json_buffer);
    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            V2H_ERROR_LOG << "Error before:" << error_ptr;
        }
        return applianceslist;
    }

    data = cJSON_GetObjectItemCaseSensitive(monitor_json, "data");
    if ((false == cJSON_IsNull(data)) && (true == cJSON_IsObject(data)))
    {
        appliances = cJSON_GetObjectItemCaseSensitive(data, "appliances");

        if ((false == cJSON_IsNull(appliances)) && (true == cJSON_IsArray(appliances)))
        {
            int size = cJSON_GetArraySize(appliances);

            if (size == m_V2H_ApplianceArray.size()){
                applianceslist = *appliances;
            }
        }
    }

    return applianceslist;
}

bool V2HJsonData::verifyV2HJsonData(QJsonObject &json_obj)
{
    bool result = false;

    if((true == json_obj.contains(KEY_ISENABLE))
            && true == json_obj.value(KEY_ISENABLE).isBool()
            && true == json_obj.value(KEY_ISENABLE).toBool()){
        result = true;
    }

    return result;
}

QList<ApplianceInfo> V2HJsonData::makeApplianceInfoListFromJsonArray(QJsonArray &json_array)
{
    QList<ApplianceInfo> applianceinfolist;

    if (json_array.size() > 0){
        int arrayindex = 0;
        for(const QJsonValue &json_value : json_array){
            const cJSON *temp_cjson = NULL;
            const cJSON *appliance_cjson = NULL;
            const cJSON *guideline_cjson = NULL;
            const cJSON *supportactions_cjson = NULL;
            const cJSON *attributes_cjson = NULL;

            if (arrayindex < cJSON_GetArraySize(&m_V2H_cJSONAppliancesArray)){
                appliance_cjson = cJSON_GetArrayItem(&m_V2H_cJSONAppliancesArray, arrayindex);

                if ((false == cJSON_IsNull(appliance_cjson)) && (true == cJSON_IsObject(appliance_cjson)))
                {
                    temp_cjson = cJSON_GetObjectItemCaseSensitive(appliance_cjson, CJSON_KEY_GUIDELINE);

                    if ((false == cJSON_IsNull(temp_cjson)) && (true == cJSON_IsObject(temp_cjson)))
                    {
                        guideline_cjson = temp_cjson;
                    }

                    temp_cjson = cJSON_GetObjectItemCaseSensitive(appliance_cjson, CJSON_KEY_SUPPORTACTIONS);

                    if ((false == cJSON_IsNull(temp_cjson)) && (true == cJSON_IsObject(temp_cjson)))
                    {
                        supportactions_cjson = temp_cjson;
                    }

                    temp_cjson = cJSON_GetObjectItemCaseSensitive(appliance_cjson, CJSON_KEY_ATTRIBUTES);

                    if ((false == cJSON_IsNull(temp_cjson)) && (true == cJSON_IsObject(temp_cjson)))
                    {
                        attributes_cjson = temp_cjson;
                    }
                }
            }

            ApplianceInfo applianceinfo;
            QJsonObject json_obj = json_value.toObject();

            if((true == json_obj.contains(KEY_BOTNAME))
                    && true == json_obj.value(KEY_BOTNAME).isString()){
                applianceinfo.botName = json_obj.value(KEY_BOTNAME).toString();
            }

            if((true == json_obj.contains(KEY_BOTID))
                    && true == json_obj.value(KEY_BOTID).isString()){
                applianceinfo.botId = json_obj.value(KEY_BOTID).toString();
            }

            if((true == json_obj.contains(KEY_BOTLOGO))
                    && true == json_obj.value(KEY_BOTLOGO).isString()){
                applianceinfo.botLogo = json_obj.value(KEY_BOTLOGO).toString();
            }

            if((true == json_obj.contains(KEY_NUM))
                    && true == json_obj.value(KEY_NUM).isString()){
                applianceinfo.num = json_obj.value(KEY_NUM).toString();
            }

            if((true == json_obj.contains(KEY_APPLIANCEID))
                    && true == json_obj.value(KEY_APPLIANCEID).isString()){
                applianceinfo.applianceId = json_obj.value(KEY_APPLIANCEID).toString();
            }

            if((true == json_obj.contains(KEY_APPLIANCETYPES))
                    && true == json_obj.value(KEY_APPLIANCETYPES).isArray()){
                QJsonArray json_array = json_obj.value(KEY_APPLIANCETYPES).toArray();

                for(const QJsonValue &json_value : json_array){
                    if (true == json_value.isString()){
                        applianceinfo.applianceTypes.append(json_value.toString());
                    }
                }
            }

            if((true == json_obj.contains(KEY_FRIENDLYNAME))
                    && true == json_obj.value(KEY_FRIENDLYNAME).isString()){
                applianceinfo.friendlyName = json_obj.value(KEY_FRIENDLYNAME).toString();
            }

            if((true == json_obj.contains(KEY_FRIENDLYDESCRIPTION))
                    && true == json_obj.value(KEY_FRIENDLYDESCRIPTION).isString()){
                applianceinfo.friendlyDescription = json_obj.value(KEY_FRIENDLYDESCRIPTION).toString();
            }

            if((true == json_obj.contains(KEY_GROUPNAME))
                    && true == json_obj.value(KEY_GROUPNAME).isString()){
                applianceinfo.groupName = json_obj.value(KEY_GROUPNAME).toString();
            }

            if((true == json_obj.contains(KEY_GUIDELINE))
                    && true == json_obj.value(KEY_GUIDELINE).isObject()
                    && guideline_cjson != NULL){
                const cJSON *guideline_next_cjson = NULL;
                const cJSON *guideline_child_cjson = NULL;
                if (guideline_cjson->child != NULL){
                    guideline_next_cjson = guideline_cjson->child;

                    if (true == cJSON_IsArray(guideline_next_cjson)){
                        while(guideline_next_cjson != NULL){
                            Guideline guideline;
                            guideline.key = QString(guideline_next_cjson->string);

                            guideline_child_cjson = guideline_next_cjson->child;
                            while(guideline_child_cjson != NULL){
                                if (true == cJSON_IsString(guideline_child_cjson)){
                                    guideline.values.append(QString(guideline_child_cjson->valuestring));
                                    m_AllGuidelineStrings.append(QString(guideline_child_cjson->valuestring));
                                }

                                guideline_child_cjson = guideline_child_cjson->next;
                            }

                            applianceinfo.guideline.append(guideline);
                            guideline_next_cjson = guideline_next_cjson->next;
                        }
                    }
                }
            }

            if((true == json_obj.contains(KEY_SUPPORTACTIONS))
                    && true == json_obj.value(KEY_SUPPORTACTIONS).isObject()){

                quint32 incrementNumber = 0;
                quint32 decrementNumber = 0;
                quint32 actionBarNum = 0;
                QString incrementactionKeyString;
                QString decrementactionKeyString;
                QString incrementactionValueString;
                QString decrementactionValueString;
                QString incrementaction;
                QString decrementaction;
                bool actionturnOn = false;
                bool actionturnOff = false;
                bool actionsetMode = false;

                applianceinfo.actionturnOnOffSupported = false;
                applianceinfo.actionsetModeSupported = false;

                const cJSON *supportactions_child_cjson = NULL;
                if (supportactions_cjson->child != NULL){
                    supportactions_child_cjson = supportactions_cjson->child;

                    while(supportactions_child_cjson != NULL){
                        if (true == cJSON_IsString(supportactions_child_cjson)){
                            MappedInfo action;
                            action.key = QString(supportactions_child_cjson->string);
                            action.value = QString(supportactions_child_cjson->valuestring);
                            applianceinfo.supportActions.append(action);

                            if (KEY_ACTION_TURNON == action.key){
                                actionturnOn = true;
                            }
                            else if (KEY_ACTION_TURNOFF == action.key){
                                actionturnOff = true;
                            }
                            else if (KEY_ACTION_SETMODE == action.key){
                                actionsetMode = true;
                            }
                            else if (true == action.key.startsWith(KEY_ACTION_INCREMENT)){
                                incrementactionKeyString = action.key;
                                incrementactionValueString = action.value;
                                incrementaction = action.key.right(incrementactionKeyString.size() - KEY_ACTION_INCREMENT.size());
                                incrementNumber += 1;
                            }
                            else if (true == action.key.startsWith(KEY_ACTION_DECREMENT)){
                                decrementactionKeyString = action.key;
                                decrementactionValueString = action.value;
                                decrementaction = action.key.right(decrementactionKeyString.size() - KEY_ACTION_DECREMENT.size());
                                decrementNumber += 1;
                            }

                            if ((true == actionturnOn) && (true == actionturnOff)){
                                applianceinfo.actionturnOnOffSupported = true;
                            }

                            if (true == actionsetMode){
                                applianceinfo.actionsetModeSupported = true;
                            }

                            if ((incrementNumber > 0)
                                    && (incrementNumber == decrementNumber)
                                    && (incrementNumber > actionBarNum)){
                                if (incrementaction == decrementaction){
                                    ActionBar actionbar;
                                    actionbar.incrementAction.key = incrementactionKeyString;
                                    actionbar.incrementAction.value = incrementactionValueString;
                                    actionbar.decrementAction.key = decrementactionKeyString;
                                    actionbar.decrementAction.value = decrementactionValueString;
                                    applianceinfo.actionBars.append(actionbar);
                                    actionBarNum = incrementNumber;
                                }
                            }
                        }
                        supportactions_child_cjson = supportactions_child_cjson->next;
                    }
                }
            }

            if((true == json_obj.contains(KEY_ATTRIBUTES))
                    && true == json_obj.value(KEY_ATTRIBUTES).isObject()){
                QJsonObject json_attributes = json_obj.value(KEY_ATTRIBUTES).toObject();

                QStringList keys = json_attributes.keys();

                for(const QString &key : keys){
                    if((true == json_attributes.contains(key))
                            && true == json_attributes.value(key).isObject()){
                        const cJSON *temp_attribute_cjson = NULL;
                        const cJSON *temp_detail_cjson = NULL;
                        const cJSON *temp_range_cjson = NULL;
                        AttributeInfo attributeinfo;
                        attributeinfo.attributename = key;

                        if((true == json_attributes.value(key).toObject().contains(KEY_DETAIL))
                                && true == json_attributes.value(key).toObject().value(KEY_DETAIL).isObject()){
                            QJsonObject json_detail = json_attributes.value(key).toObject().value(KEY_DETAIL).toObject();

                            if((true == json_detail.contains(KEY_DETAIL_NAME))
                                    && true == json_detail.value(KEY_DETAIL_NAME).isString()){
                                attributeinfo.detail.name = json_detail.value(KEY_DETAIL_NAME).toString();
                            }

                            if((true == json_detail.contains(KEY_DETAIL_SCALE))
                                    && true == json_detail.value(KEY_DETAIL_SCALE).isString()){
                                attributeinfo.detail.scale = json_detail.value(KEY_DETAIL_SCALE).toString();
                            }

                            if((true == json_detail.contains(KEY_DETAIL_RANGE))
                                    && true == json_detail.value(KEY_DETAIL_RANGE).isObject()){
                                QJsonObject json_range = json_detail.value(KEY_DETAIL_RANGE).toObject();

                                if((true == json_range.contains(KEY_DETAIL_RANGE_MIN))
                                        && true == json_range.value(KEY_DETAIL_RANGE_MIN).isDouble()){
                                    attributeinfo.detail.range_min = json_range.value(KEY_DETAIL_RANGE_MIN).toDouble();
                                    attributeinfo.detail.minmax_flag = true;
                                }
                                else{
                                    attributeinfo.detail.range_min = 0;
                                    attributeinfo.detail.minmax_flag = false;
                                }

                                if((true == json_range.contains(KEY_DETAIL_RANGE_MAX))
                                        && true == json_range.value(KEY_DETAIL_RANGE_MAX).isDouble()){
                                    attributeinfo.detail.range_max = json_range.value(KEY_DETAIL_RANGE_MAX).toDouble();
                                    attributeinfo.detail.minmax_flag = true;
                                }
                                else{
                                    attributeinfo.detail.range_max = 0;
                                    attributeinfo.detail.minmax_flag = false;
                                }

                                if (true == cJSON_HasObjectItem(attributes_cjson, key.toStdString().c_str())){
                                    temp_attribute_cjson = cJSON_GetObjectItemCaseSensitive(attributes_cjson, key.toStdString().c_str());

                                    if (true == cJSON_HasObjectItem(temp_attribute_cjson, CJSON_KEY_DETAIL)){
                                        temp_detail_cjson = cJSON_GetObjectItemCaseSensitive(temp_attribute_cjson, CJSON_KEY_DETAIL);

                                        if (true == cJSON_HasObjectItem(temp_detail_cjson, CJSON_KEY_RANGE)){
                                            temp_range_cjson = cJSON_GetObjectItemCaseSensitive(temp_detail_cjson, CJSON_KEY_RANGE);

                                            const cJSON *range_child_cjson = NULL;
                                            if (temp_range_cjson->child != NULL){
                                                range_child_cjson = temp_range_cjson->child;

                                                while(range_child_cjson != NULL){
                                                    if (true == cJSON_IsString(range_child_cjson)){
                                                        MappedInfo range;
                                                        range.key = QString(range_child_cjson->string);
                                                        range.value = QString(range_child_cjson->valuestring);
                                                        attributeinfo.detail.range_map.append(range);
                                                    }
                                                    range_child_cjson = range_child_cjson->next;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if((true == json_attributes.value(key).toObject().contains(KEY_VALUE))
                                && true == json_attributes.value(key).toObject().value(KEY_VALUE).isString()){
                            attributeinfo.value = json_attributes.value(key).toObject().value(KEY_VALUE).toString();
                        }

                        applianceinfo.attributes.append(attributeinfo);
                    }
                }
            }

            applianceinfolist.append(applianceinfo);
            arrayindex += 1;
        }
    }

    return applianceinfolist;
}

QStringList V2HJsonData::makeGroupNameList()
{
    QStringList groupnamelist;

    if (getTotalAppliances() > 0){

        for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
            if ((false == appliance.groupName.isEmpty())
                    && (false == groupnamelist.contains(appliance.groupName))){
                groupnamelist.append(appliance.groupName);
            }
        }

        V2H_NORMAL_LOG << "GroupNameList.size = " << groupnamelist.size();
    }

    return groupnamelist;
}

QStringList V2HJsonData::makeApplianceTypeList()
{
    QStringList appliancetypelist;

    if (getTotalAppliances() > 0){

        for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
            if (false == appliance.applianceTypes.isEmpty()){
                for(const QString &type : appliance.applianceTypes){
                    if ((false == type.isEmpty())
                            && (false == appliancetypelist.contains(type))){
                        appliancetypelist.append(type);
                    }
                }
            }
        }

        V2H_NORMAL_LOG << "ApplianceTypeList.size = " << appliancetypelist.size();
    }

    return appliancetypelist;
}

QHash<QString, QStringList> V2HJsonData::makeDeviceTypeGuidelineMap()
{
    QHash<QString, QStringList> guidelinemap;

    for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
        if (false == appliance.guideline.isEmpty()){
            QStringList type_guidelines;

            if (true == guidelinemap.contains(appliance.applianceTypes.at(0))){
                type_guidelines = guidelinemap.take(appliance.applianceTypes.at(0));
            }
            for(const Guideline &guideline : appliance.guideline){
                for(const QString &guidestring : guideline.values){
                    if (false == type_guidelines.contains(guidestring)){
                        type_guidelines.append(guidestring);
                    }
                }
            }

            guidelinemap.insert(appliance.applianceTypes.at(0), type_guidelines);
        }
    }

    return guidelinemap;
}

QList<ApplianceInfo> V2HJsonData::makeFiltedAppliancesInfoList()
{
    QList<ApplianceInfo> applianceinfolist;
    QString groupFilter = getGroupFilter();
    QString typeFilter = getTypeFilter();

    if ((false == groupFilter.isEmpty()) && (false == typeFilter.isEmpty())){
        for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
            if (groupFilter == appliance.groupName){
                if (false == appliance.applianceTypes.isEmpty()){
                    for(const QString &type : appliance.applianceTypes){
                        if ((false == type.isEmpty())
                                && (typeFilter == type)){
                            applianceinfolist.append(appliance);
                            break;
                        }
                    }
                }
            }
        }
    }
    else if (false == groupFilter.isEmpty()){
        for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
            if (groupFilter == appliance.groupName){
                applianceinfolist.append(appliance);
            }
        }
    }
    else if (false == typeFilter.isEmpty()){
        for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
            if (false == appliance.applianceTypes.isEmpty()){
                for(const QString &type : appliance.applianceTypes){
                    if ((false == type.isEmpty())
                            && (typeFilter == type)){
                        applianceinfolist.append(appliance);
                        break;
                    }
                }
            }
        }
    }
    else{
        return m_V2H_ApplianceInfoList;
    }

    return applianceinfolist;
}

QList<ApplianceInfo> V2HJsonData::makeAppliancesInfoListByGroup(QString &groupname)
{
    QList<ApplianceInfo> applianceinfolist;

    if((false == groupname.isEmpty())
            && (getTotalAppliances() > 0)){

        for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
            if (groupname == appliance.groupName){
                applianceinfolist.append(appliance);
            }
        }
    }
    else{
        applianceinfolist = m_V2H_ApplianceInfoList;
    }

    return applianceinfolist;
}

QList<ApplianceInfo> V2HJsonData::makeAppliancesInfoListByType(QString &appliancetype)
{
    QList<ApplianceInfo> applianceinfolist;

    if((false == appliancetype.isEmpty())
            && (getTotalAppliances() > 0)){
        for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
            if (false == appliance.applianceTypes.isEmpty()){
                for(const QString &type : appliance.applianceTypes){
                    if ((false == type.isEmpty())
                            && (appliancetype == type)){
                        applianceinfolist.append(appliance);
                        break;
                    }
                }
            }
        }

        V2H_NORMAL_LOG << "ApplianceTypeList(" << appliancetype << ").size()" << applianceinfolist.size();
    }
    else{
        applianceinfolist = m_V2H_ApplianceInfoList;
    }

    return applianceinfolist;
}

ServiceFlag V2HJsonData::makeServiceFlagFromJsonObj(QJsonObject &json_obj)
{
    ServiceFlag serviceflag;

    if((true == json_obj.contains(KEY_SERVICE_FLAG))
            && true == json_obj.value(KEY_SERVICE_FLAG).isString()){
        serviceflag.service_flag = json_obj.value(KEY_SERVICE_FLAG).toString();
    }

    if((true == json_obj.contains(KEY_APP_LOGO_URL))
            && true == json_obj.value(KEY_APP_LOGO_URL).isString()){
        serviceflag.app_logo_url = json_obj.value(KEY_APP_LOGO_URL).toString();
    }

    if((true == json_obj.contains(KEY_APP_DL_URL))
            && true == json_obj.value(KEY_APP_DL_URL).isString()){
        serviceflag.app_dl_url = json_obj.value(KEY_APP_DL_URL).toString();
    }

    if((true == json_obj.contains(KEY_MAKER_APP_DL_URL))
            && true == json_obj.value(KEY_MAKER_APP_DL_URL).isString()){
        serviceflag.maker_app_dl_url = json_obj.value(KEY_MAKER_APP_DL_URL).toString();
    }

    if((true == json_obj.contains(KEY_HOME_LINK_TYPE))
            && true == json_obj.value(KEY_HOME_LINK_TYPE).isDouble()){
        serviceflag.home_link_type = json_obj.value(KEY_HOME_LINK_TYPE).toInt();
    }

    serviceflag.geted = true;

    return serviceflag;
}

QStringList V2HJsonData::makeRandomGuidelineStringlist(int maxstings)
{
    QStringList guidelinestrlist;

    if (maxstings > 0){
        QStringList wholeguidelinestrings = getAllGuidelineStrings();

        if (maxstings < wholeguidelinestrings.size()){
            uint loopcount = 1;
            while (guidelinestrlist.size() < maxstings){
                uint random_seed = QTime::currentTime().msec() + loopcount;
                qsrand(random_seed);
                int rand_int = qrand() % wholeguidelinestrings.size();
                qDebug() << "rand_int:" << rand_int;

                QString rand_guidelinestr = wholeguidelinestrings.at(rand_int);
                if (false == guidelinestrlist.contains(rand_guidelinestr)){
                    guidelinestrlist.append(rand_guidelinestr);
                }
                loopcount += 1;
            }

        }
        else{
            guidelinestrlist = wholeguidelinestrings;
        }
    }

    return guidelinestrlist;
}

QStringList V2HJsonData::makeDeviceTypeRandomGuidelineStringlist(QString &appliancetype, int maxstings)
{
    QStringList guidelinestrlist;

    if ((false == appliancetype.isEmpty()) && (maxstings > 0)){
        if (true == m_DeviceTypeGuidelineMap.contains(appliancetype)){
            QStringList typeguidelinestrings = m_DeviceTypeGuidelineMap.value(appliancetype);

            if (maxstings < typeguidelinestrings.size()){
                uint loopcount = 1;
                while (guidelinestrlist.size() < maxstings){
                    uint random_seed = QTime::currentTime().msec() + loopcount;
                    qsrand(random_seed);
                    int rand_int = qrand() % typeguidelinestrings.size();
                    qDebug() << "rand_int:" << rand_int;

                    QString rand_guidelinestr = typeguidelinestrings.at(rand_int);
                    if (false == guidelinestrlist.contains(rand_guidelinestr)){
                        guidelinestrlist.append(rand_guidelinestr);
                    }
                    loopcount += 1;
                }

            }
            else{
                guidelinestrlist = typeguidelinestrings;
            }
        }
        else{
            V2H_ERROR_LOG << "m_DeviceTypeGuidelineMap do not contains type(" << appliancetype << ")";
        }
    }
    else{
        V2H_ERROR_LOG << "appliancetype.isEmpty()=" << appliancetype.isEmpty() << ", " << "maxstings=" << maxstings;
    }

    return guidelinestrlist;
}

QString V2HJsonData::convertApplianceType2DeviceType(QString &appliancetype)
{
    QString DeviceType;

    if (true == m_DeviceTypeMap.contains(appliancetype)){
        DeviceType = m_DeviceTypeMap.value(appliancetype);
    }

    return DeviceType;
}

QString V2HJsonData::convertAction2AttributeName(QString &action)
{
    QString AttributeName;

    if (true == m_AttributeMap.contains(action)){
        AttributeName = m_AttributeMap.value(action);
    }

    return AttributeName;
}

QByteArray V2HJsonData::generateGetServiceFlagJson()
{
    QByteArray getserviceflag_json;
    char *string = NULL;
    cJSON *imei = NULL;

    cJSON *getserviceflag_cjson = cJSON_CreateObject();
    if (getserviceflag_cjson == NULL)
    {
        return getserviceflag_json;
    }

    imei = cJSON_CreateString(OPERATION_IMEI);
    if (imei == NULL)
    {
        return getserviceflag_json;
    }
    cJSON_AddItemToObject(getserviceflag_cjson, CJSON_IMEI, imei);

    string = cJSON_Print(getserviceflag_cjson);

    getserviceflag_json = QByteArray(string);

    return getserviceflag_json;
}

QByteArray V2HJsonData::generateGetAppliancesListJson()
{
    QByteArray getapplianceslist_json;
    char *string = NULL;
    cJSON *imei = NULL;
    cJSON *home_link_type = NULL;

    cJSON *getapplianceslist_cjson = cJSON_CreateObject();
    if (getapplianceslist_cjson == NULL)
    {
        return getapplianceslist_json;
    }

    imei = cJSON_CreateString(OPERATION_IMEI);
    if (imei == NULL)
    {
        return getapplianceslist_json;
    }
    cJSON_AddItemToObject(getapplianceslist_cjson, CJSON_IMEI, imei);

    home_link_type = cJSON_CreateNumber(HOMELINKTYPE_BAIDU);
    if (home_link_type == NULL)
    {
        return getapplianceslist_json;
    }
    cJSON_AddItemToObject(getapplianceslist_cjson, CJSON_HOME_LINK_TYPE, home_link_type);

    string = cJSON_Print(getapplianceslist_cjson);

    getapplianceslist_json = QByteArray(string);

    return getapplianceslist_json;
}

QByteArray V2HJsonData::generateOperationRequestJson(OperationRequest &operation_req)
{
    QByteArray operation_req_json;
    char *string = NULL;
    cJSON *imei = NULL;
    cJSON *deviceId = NULL;
    cJSON *deviceName = NULL;
    cJSON *groupName = NULL;
    cJSON *operation = NULL;
    cJSON *home_link_type = NULL;

    cJSON *operation_cjson = cJSON_CreateObject();
    if (operation_cjson == NULL)
    {
        return operation_req_json;
    }

    imei = cJSON_CreateString(OPERATION_IMEI);
    if (imei == NULL)
    {
        return operation_req_json;
    }
    cJSON_AddItemToObject(operation_cjson, CJSON_IMEI, imei);

    deviceId = cJSON_CreateString(operation_req.deviceId.toUtf8().constData());
    if (deviceId == NULL)
    {
        return operation_req_json;
    }
    cJSON_AddItemToObject(operation_cjson, CJSON_DEVICEID, deviceId);

    deviceName = cJSON_CreateString(operation_req.deviceName.toUtf8().constData());
    if (deviceName == NULL)
    {
        return operation_req_json;
    }
    cJSON_AddItemToObject(operation_cjson, CJSON_DEVICENAME, deviceName);

    groupName = cJSON_CreateString(operation_req.groupName.toUtf8().constData());
    if (groupName == NULL)
    {
        return operation_req_json;
    }
    cJSON_AddItemToObject(operation_cjson, CJSON_GROUPNAME, groupName);

    operation = cJSON_CreateString(operation_req.operation.toUtf8().constData());
    if (operation == NULL)
    {
        return operation_req_json;
    }
    cJSON_AddItemToObject(operation_cjson, CJSON_OPERATION, operation);

    if ((false == operation_req.mode.value.isEmpty())
            && (false == operation_req.mode.deviceType.isEmpty())){
        cJSON *operationparam = cJSON_CreateObject();
        if (operationparam == NULL)
        {
            return operation_req_json;
        }
        cJSON_AddItemToObject(operation_cjson, CJSON_OPERATIONPARAM, operationparam);

        cJSON *mode = cJSON_CreateObject();
        if (mode == NULL)
        {
            return operation_req_json;
        }
        cJSON_AddItemToObject(operationparam, CJSON_MODE, mode);

        cJSON *mode_devicetype = NULL;
        cJSON *mode_value = NULL;
        mode_devicetype = cJSON_CreateString(operation_req.mode.deviceType.toUtf8().constData());
        if (mode_devicetype == NULL)
        {
            return operation_req_json;
        }
        cJSON_AddItemToObject(mode, CJSON_MODE_DEVICETYPE, mode_devicetype);

        mode_value = cJSON_CreateString(operation_req.mode.value.toUtf8().constData());
        if (mode_devicetype == NULL)
        {
            return operation_req_json;
        }
        cJSON_AddItemToObject(mode, CJSON_MODE_VALUE, mode_value);
    }

    home_link_type = cJSON_CreateNumber(HOMELINKTYPE_BAIDU);
    if (home_link_type == NULL)
    {
        return operation_req_json;
    }
    cJSON_AddItemToObject(operation_cjson, CJSON_HOME_LINK_TYPE, home_link_type);

    string = cJSON_Print(operation_cjson);

    operation_req_json = QByteArray(string);

    return operation_req_json;
}

void V2HJsonData::setV2HServiceFlagUpdatedFlag(bool flag)
{
    m_V2H_ServiceFlagUpdated = flag;
}

void V2HJsonData::setV2HAppliancesListUpdatedFlag(bool flag)
{
    m_V2H_AppliancesListUpdated = flag;
}

void V2HJsonData::setV2HApplianceOperationUpdatedFlag(bool flag)
{
    m_V2H_ApplianceOperationUpdated = flag;
}

void V2HJsonData::setV2HServiceFlagGeted(bool geted)
{
    m_V2H_ServiceFlag.geted = geted;
}

bool V2HJsonData::setV2HServiceFlagJsonData(const char *json_buffer)
{
    bool result = false;
    QJsonParseError error;
    QJsonDocument json_doc = QJsonDocument::fromJson(QByteArray(json_buffer), &error);

    if ((QJsonParseError::NoError == error.error)
            && (true == json_doc.isObject())
            && (false == json_doc.isNull())){
        QJsonObject json_obj = json_doc.object();
        if((true == json_obj.contains(KEY_STATUS))
                && STATUS_OK == json_obj.value(KEY_STATUS).toInt()){
            m_V2H_ServiceFlagJsonString = QString(json_buffer);
            m_V2H_ServiceFlag = makeServiceFlagFromJsonObj(json_obj);
            setV2HServiceFlagGeted(true);
            result = true;
        }
    }

    return result;
}

bool V2HJsonData::setV2HAppliancesListJsonData(const char *json_buffer)
{
    bool result = false;
    QJsonParseError error;
    QJsonDocument json_doc = QJsonDocument::fromJson(QByteArray(json_buffer), &error);

    if ((QJsonParseError::NoError == error.error)
            && (true == json_doc.isObject())
            && (false == json_doc.isNull())){
        QJsonObject json_obj = json_doc.object();
        if((true == json_obj.contains(KEY_STATUS))
                && STATUS_OK == json_obj.value(KEY_STATUS).toInt()){
            if((true == json_obj.contains(KEY_DATA))
                    && true == json_obj.value(KEY_DATA).isObject()){
                m_V2H_JsonData = json_obj.value(KEY_DATA).toObject();
                m_V2H_AppliancesListIsEnable = verifyV2HJsonData(m_V2H_JsonData);

                if (m_V2H_AppliancesListIsEnable != true){
                    return true;
                }

                if((true == m_V2H_JsonData.contains(KEY_REDDOT))
                        && true == m_V2H_JsonData.value(KEY_REDDOT).isBool()){
                    m_V2H_AppliancesListRedDot = m_V2H_JsonData.value(KEY_REDDOT).toBool();
                }

                m_V2H_AppliancesListJsonString = QString(json_buffer);
                m_V2H_ApplianceArray = getJsonAppliancesArrayFromJsonData();

                cJSON cjson_array;
                if (m_V2H_ApplianceArray.size() > 0){
                    cjson_array = makecJSONAppliancesListArray(json_buffer);
                }

                if (false == cJSON_IsNull(&cjson_array)){
                    m_V2H_cJSONAppliancesArray = cjson_array;
                    m_AllGuidelineStrings.clear();
                    m_V2H_ApplianceInfoList = makeApplianceInfoListFromJsonArray(m_V2H_ApplianceArray);
                }

                m_GroupNameList = makeGroupNameList();
                m_ApplianceTypeList = makeApplianceTypeList();
                m_V2H_FiltedApplianceInfoList = makeFiltedAppliancesInfoList();
                m_DeviceTypeGuidelineMap = makeDeviceTypeGuidelineMap();

                result = true;
                V2H_NORMAL_LOG << "TotalAppliances = " << V2HJsonData::getTotalAppliances();
            }
        }
    }

    return result;
}

bool V2HJsonData::setV2HApplianceOperationJsonData(const char *json_buffer)
{
    bool result = false;
    QJsonParseError error;
    QJsonDocument json_doc = QJsonDocument::fromJson(QByteArray(json_buffer), &error);

    if ((QJsonParseError::NoError == error.error)
            && (true == json_doc.isObject())
            && (false == json_doc.isNull())){
        QJsonObject json_obj = json_doc.object();
        if((true == json_obj.contains(KEY_STATUS))
                && STATUS_OK == json_obj.value(KEY_STATUS).toInt()){
            m_V2H_ApplianceOperationJsonString = QString(json_buffer);

            if((true == json_obj.contains(KEY_RESULT))
                    && true == json_obj.value(KEY_RESULT).isArray()){
                bool updated = false;
                QJsonArray result_array = json_obj.value(KEY_RESULT).toArray();
                ApplianceInfo applianceinfo = m_SelectedApplianceInfo;

                for(const QJsonValue &json_value : result_array){
                    OperationResult operationresult;
                    if (true == json_value.isObject()){
                        if((true == json_value.toObject().contains(KEY_TTS_STATUS))
                                && true == json_value.toObject().value(KEY_TTS_STATUS).isObject()){
                            if((true == json_value.toObject().value(KEY_TTS_STATUS).toObject().contains(KEY_TTS_STATUS_TTS))
                                    && true == json_value.toObject().value(KEY_TTS_STATUS).toObject().value(KEY_TTS_STATUS_TTS).isString()){
                                operationresult.tts = json_value.toObject().value(KEY_TTS_STATUS).toObject().value(KEY_TTS_STATUS_TTS).toString();
                            }
                        }

                        if((true == json_value.toObject().contains(KEY_DATA))
                                && true == json_value.toObject().value(KEY_DATA).isObject()){
                            QJsonObject json_data = json_value.toObject().value(KEY_DATA).toObject();

                            if((true == json_data.contains(KEY_ATTRIBUTE))
                                    && true == json_data.value(KEY_ATTRIBUTE).isObject()){
                                QJsonObject json_attribute = json_data.value(KEY_ATTRIBUTE).toObject();

                                QStringList keys = json_attribute.keys();

                                for(const QString &key : keys){
                                    if((true == json_attribute.contains(key))
                                            && true == json_attribute.value(key).isObject()){
                                        QString attributename = key;
                                        QString attributevalue;
                                        int attributindex = 0;

                                        if((true == json_attribute.value(key).toObject().contains(KEY_VALUE))
                                                && true == json_attribute.value(key).toObject().value(KEY_VALUE).isString()){
                                            attributevalue = json_attribute.value(key).toObject().value(KEY_VALUE).toString();
                                        }

                                        for(const AttributeInfo &attribute : m_SelectedApplianceInfo.attributes){
                                            if (attributename == attribute.attributename){
                                                if (false == attributevalue.isEmpty()){
                                                    applianceinfo.attributes[attributindex].value = attributevalue;
                                                    updated = true;
                                                    V2H_NORMAL_LOG << "Appliance(" << m_SelectedApplianceInfo.friendlyName << ")[" << m_SelectedApplianceInfo.applianceId << "] " << m_SelectedApplianceInfo.attributes[attributindex].attributename << " set to ("<< attributevalue <<")";
                                                }
                                            }
                                            attributindex += 1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    m_V2H_OperationResults.append(operationresult);
                }

                if (true == updated){
                    if (0 <= m_SelectedApplianceIndex && m_SelectedApplianceIndex < getTotalAppliances()){
                        m_V2H_ApplianceInfoList[m_SelectedApplianceIndex] = applianceinfo;
                        V2H_NORMAL_LOG << "ApplianceInfoList Index(" << m_SelectedApplianceIndex << ") Updated";
                    }
                    else{
                        V2H_ERROR_LOG << "SelectedApplianceIndex = " << m_SelectedApplianceIndex << "; TotalAppliances = " << getTotalAppliances();
                    }

                    int filtedindex = -1;
                    int searchfiltedindex = 0;
                    for(const ApplianceInfo &appliance : m_V2H_FiltedApplianceInfoList){
                        if (applianceinfo.applianceId == appliance.applianceId){
                            filtedindex = searchfiltedindex;
                            break;
                        }
                        searchfiltedindex += 1;
                    }

                    if (0 <= filtedindex && filtedindex < m_V2H_FiltedApplianceInfoList.size()){
                        m_V2H_FiltedApplianceInfoList[filtedindex] = applianceinfo;
                        V2H_NORMAL_LOG << "FiltedApplianceInfoList Index(" << filtedindex << ") Updated";
                    }

                    m_SelectedApplianceInfo = applianceinfo;
                }
                else{
                    V2H_NORMAL_LOG << "OperationResult has no valid data to update.";
                }
            }
            result = true;
        }
    }

    return result;
}

bool V2HJsonData::setGroupNameFilter(QString groupname)
{
    bool result = false;
    bool find = false;

    if (true == groupname.isEmpty()){
        return false;
    }

    for(const QString &group : m_GroupNameList){
        if (groupname == group){
            find = true;
            V2H_NORMAL_LOG << "GroupName(" << groupname << ") found.";
            break;
        }
    }

    if (true == find){
        m_GroupNameFilter = groupname;
        m_V2H_FiltedApplianceInfoList = makeFiltedAppliancesInfoList();
        result = true;
    }

    return result;
}

bool V2HJsonData::setApplianceTypeFilter(QString appliancetype)
{
    bool result = false;
    bool find = false;

    if (true == appliancetype.isEmpty()){
        return false;
    }

    for(const QString &type : m_ApplianceTypeList){
        if (appliancetype == type){
            find = true;
            V2H_NORMAL_LOG << "ApplianceType(" << appliancetype << ") found.";
            break;
        }
    }

    if (true == find){
        m_ApplianceTypeFilter = appliancetype;
        m_V2H_FiltedApplianceInfoList = makeFiltedAppliancesInfoList();
        result = true;
    }

    return result;
}

bool V2HJsonData::setSelectApplianceID(QString appliance_id)
{
    bool result = false;

    if (false == appliance_id.isEmpty()){
        ApplianceInfo appliance_info;
        int appliance_index = getApplianceInfoFromID(appliance_id, appliance_info);

        if (appliance_index >= 0){
            m_SelectedApplianceIndex = appliance_index;
            m_SelectedApplianceInfo = appliance_info;
            result = true;
        }
    }

    return result;
}

bool V2HJsonData::setQRCodeBaiduLogo(QPixmap &logo)
{
    bool result = false;
    if ((false == logo.isNull())
            && (logo.width() > 0)
            && (logo.height() > 0)){
        *m_V2H_BaiduLogo = logo;
        result = true;
        V2H_NORMAL_LOG << "Set BaiduLogo width(" << logo.width() << "), height(" << logo.height() << ")";
    }

    return result;
}

bool V2HJsonData::getV2HServiceFlagUpdatedFlag()
{
    return m_V2H_ServiceFlagUpdated;
}

bool V2HJsonData::getV2HAppliancesListUpdatedFlag()
{
    return m_V2H_AppliancesListUpdated;
}

bool V2HJsonData::getV2HApplianceOperationUpdatedFlag()
{
    return m_V2H_ApplianceOperationUpdated;
}

bool V2HJsonData::getV2HServiceFlagGeted()
{
    return m_V2H_ServiceFlag.geted;
}

ServiceFlag V2HJsonData::getV2HServiceFlag()
{
    return m_V2H_ServiceFlag;
}

bool V2HJsonData::getV2HJsonDataIsEnable()
{
    return m_V2H_AppliancesListIsEnable;
}

bool V2HJsonData::getV2HJsonDataRedDot()
{
    return m_V2H_AppliancesListRedDot;
}

QString V2HJsonData::getGroupFilter()
{
    return m_GroupNameFilter;
}

QString V2HJsonData::getTypeFilter()
{
    return m_ApplianceTypeFilter;
}

QString V2HJsonData::getSelectedApplianceID()
{
    return m_SelectedApplianceInfo.applianceId;
}

int V2HJsonData::getSelectedApplianceIndex()
{
    if (0 <= m_SelectedApplianceIndex && m_SelectedApplianceIndex < getTotalAppliances()){
        return m_SelectedApplianceIndex;
    }
    else{
        return -1;
    }
}

int V2HJsonData::getTotalAppliances()
{
    return m_V2H_ApplianceInfoList.size();
}

QStringList V2HJsonData::getGroupNameList()
{
    return m_GroupNameList;
}

QStringList V2HJsonData::getApplianceTypeList()
{
    return m_ApplianceTypeList;
}

ApplianceInfo V2HJsonData::getSelectedApplianceInfo()
{
    return m_SelectedApplianceInfo;
}

QStringList V2HJsonData::getSelectedApplianceTypes()
{
    return m_SelectedApplianceInfo.applianceTypes;
}

QList<MappedInfo> V2HJsonData::getSelectedApplianceModeList()
{
    QList<MappedInfo> modelist;

    if (false == getSelectedApplianceID().isEmpty()){
        for(const AttributeInfo &attribute : m_SelectedApplianceInfo.attributes){
            if (KEY_ATTR_MODE == attribute.attributename){
                modelist = attribute.detail.range_map;
            }
        }
    }

    return modelist;
}

QString V2HJsonData::getSelectedApplianceCurrentModeValue()
{
    QString currentmodevaluestring;

    if (false == getSelectedApplianceID().isEmpty()){
        for(const AttributeInfo &attribute : m_SelectedApplianceInfo.attributes){
            if (KEY_ATTR_MODE == attribute.attributename){
                currentmodevaluestring = attribute.value;
            }
        }
    }

    return currentmodevaluestring;
}

MappedInfo V2HJsonData::getSelectedApplianceCurrentMode()
{
    MappedInfo currentmode;
    QList<MappedInfo> modelist = getSelectedApplianceModeList();
    QString modevaluestring = getSelectedApplianceCurrentModeValue();

    for(const MappedInfo &mode : modelist){
        if (modevaluestring == mode.key){
            currentmode = mode;
            break;
        }
    }

    return currentmode;
}

AttributeInfo V2HJsonData::getSelectedApplianceAttributeInfo(QString attributename)
{
    AttributeInfo attributeinfo;

    if (false == getSelectedApplianceID().isEmpty()){
        for(const AttributeInfo &attribute : m_SelectedApplianceInfo.attributes){
            if (attributename == attribute.attributename){
                attributeinfo = attribute;
            }
        }
    }

    return attributeinfo;
}

QList<ActionBar> V2HJsonData::getSelectedApplianceActionBars()
{
    return m_SelectedApplianceInfo.actionBars;
}

QList<Guideline> V2HJsonData::getSelectedApplianceGuideline()
{
    return m_SelectedApplianceInfo.guideline;
}

bool V2HJsonData::getSelectedApplianceTurnOnOffSupported()
{
    return m_SelectedApplianceInfo.actionturnOnOffSupported;
}

bool V2HJsonData::getSelectedApplianceSetModeSupported()
{
    return m_SelectedApplianceInfo.actionsetModeSupported;
}

int V2HJsonData::getApplianceInfoFromID(QString &appliance_id, ApplianceInfo &applianceinfo)
{
    int result_index = -1;

    if ((false == appliance_id.isEmpty())
            && (getTotalAppliances() > 0) ){
        int appliance_index = 0;

        for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
            if (appliance_id == appliance.applianceId){
                applianceinfo = appliance;
                result_index = appliance_index;
                break;
            }

            appliance_index++;
        }
    }

    return result_index;
}

QList<ApplianceInfo> V2HJsonData::getAppliancesInfoList()
{
    return m_V2H_ApplianceInfoList;
}

QList<ApplianceInfo> V2HJsonData::getFiltedAppliancesInfoList()
{
    return m_V2H_FiltedApplianceInfoList;
}

QStringList V2HJsonData::getAllGuidelineStrings()
{
    return m_AllGuidelineStrings;
}

QPixmap V2HJsonData::getQRCodeBaiduLogo()
{
    return *m_V2H_BaiduLogo;
}

QJsonArray V2HJsonData::getJsonAppliancesArrayFromJsonData()
{
    QJsonArray json_array;

    if((true == m_V2H_JsonData.contains(KEY_APPLIANCES))
            && (true == m_V2H_JsonData.value(KEY_APPLIANCES).isArray())
            && (true == m_V2H_JsonData.contains(KEY_TOTALAPPLIANCES))
            && (0 < m_V2H_JsonData.value(KEY_TOTALAPPLIANCES).toInt() && m_V2H_JsonData.value(KEY_TOTALAPPLIANCES).toInt() < V2H_APPLIANCE_INDEX_MAX)){
        if (m_V2H_JsonData.value(KEY_APPLIANCES).toArray().size() == m_V2H_JsonData.value(KEY_TOTALAPPLIANCES).toInt()){
            json_array = m_V2H_JsonData.value(KEY_APPLIANCES).toArray();
        }
        else{
            V2H_ERROR_LOG<<"totalAppliances:"<<m_V2H_JsonData.value(KEY_TOTALAPPLIANCES).toInt()<<", "<<"appliancesArraySize:"<<m_V2H_JsonData.value(KEY_APPLIANCES).toArray().size();
        }
    }

    return json_array;
}

QJsonArray V2HJsonData::getJsonAppliancesArray()
{
    return m_V2H_ApplianceArray;
}

bool V2HJsonData::clearV2HJsonData()
{
    m_V2H_JsonData = QJsonObject();
    return true;
}

bool V2HJsonData::clearGroupFilter()
{
    m_GroupNameFilter.clear();
    m_V2H_FiltedApplianceInfoList = makeFiltedAppliancesInfoList();

    return true;
}

bool V2HJsonData::clearTypeFilter()
{
    m_ApplianceTypeFilter.clear();
    m_V2H_FiltedApplianceInfoList = makeFiltedAppliancesInfoList();

    return true;
}

bool V2HJsonData::clearAllFilters()
{
    m_GroupNameFilter.clear();
    m_ApplianceTypeFilter.clear();

    m_V2H_FiltedApplianceInfoList = makeFiltedAppliancesInfoList();
    return true;
}

bool V2HJsonData::clearSelectApplianceID()
{
    m_SelectedApplianceIndex = -1;
    m_SelectedApplianceInfo = ApplianceInfo();

    return true;
}
