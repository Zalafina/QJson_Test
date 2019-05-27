#include "v2hjsondata.h"

static const QString V2H_LOGTAG("[HMI_V2H]");
static const QString V2H_LOGERROR("[ERROR]");

/* Common Keys */
static const QString KEY_STATUS("status");

/* ServiceFlag  Keys */
static const QString KEY_SERVICE_FLAG("service_flag");
static const QString KEY_APP_LOGO_URL("app_logo_url");
static const QString KEY_APP_DL_URL("app_dl_url");
static const QString KEY_MAKER_APP_DL_URL("maker_app_dl_url");
static const QString KEY_HOME_LINK_TYPE("home_link_type");

/* AppliancesList Keys */
static const QString KEY_DATA("data");
static const QString KEY_ISENABLE("isenable");
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

/* Attribute Names */
static const QString KEY_ATTR_MODE("mode");

/* Attribute Subkeys */
static const QString KEY_DETAIL("detail");
static const QString KEY_VALUE("value");
static const QString KEY_DETAIL_NAME("name");
static const QString KEY_DETAIL_SCALE("scale");
static const QString KEY_DETAIL_RANGE("range");
static const QString KEY_DETAIL_RANGE_MIN("min");
static const QString KEY_DETAIL_RANGE_MAX("max");

static const int STATUS_OK = 0;

QString V2HJsonData::m_V2H_AppliancesListJsonString = QString();
QString V2HJsonData::m_V2H_ServiceFlagJsonString = QString();
QString V2HJsonData::m_V2H_ApplianceOperationJsonString = QString();
ServiceFlag V2HJsonData::m_V2H_ServiceFlag = ServiceFlag();
QJsonObject V2HJsonData::m_V2H_JsonData = QJsonObject();
QJsonArray V2HJsonData::m_V2H_ApplianceArray = QJsonArray();
cJSON V2HJsonData::m_V2H_cJSONAppliancesArray = cJSON();
QList<ApplianceInfo> V2HJsonData::m_V2H_ApplianceInfoList = QList<ApplianceInfo>();
QList<ApplianceInfo> V2HJsonData::m_V2H_GroupApplianceInfoList = QList<ApplianceInfo>();
QList<ApplianceInfo> V2HJsonData::m_V2H_TypeApplianceInfoList = QList<ApplianceInfo>();
QString V2HJsonData::m_GroupNameFilter = QString();
QStringList V2HJsonData::m_GroupNameList = QStringList();
QString V2HJsonData::m_ApplianceTypeFilter = QString();
QStringList V2HJsonData::m_ApplianceTypeList = QStringList();
QString V2HJsonData::m_SelectedApplianceID = QString();
int V2HJsonData::m_SelectedApplianceIndex = -1;
ApplianceInfo V2HJsonData::m_SelectedApplianceInfo = ApplianceInfo();
bool V2HJsonData::m_V2H_JsonDataIsEnable = false;
bool V2HJsonData::m_V2H_ServiceFlagUpdated = false;
bool V2HJsonData::m_V2H_AppliancesListUpdated = false;
bool V2HJsonData::m_V2H_ApplianceOperationUpdated = false;

V2HJsonData::V2HJsonData(QObject *parent) : QObject(parent)
{
    setV2HServiceFlagGeted(false);
}

V2HJsonData::~V2HJsonData()
{

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
            V2H_Debug("Error before: %s", error_ptr);
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
                    && true == json_obj.value(KEY_GUIDELINE).isObject()){
                applianceinfo.guideline = json_obj.value(KEY_GUIDELINE).toObject();
            }

            if((true == json_obj.contains(KEY_SUPPORTACTIONS))
                    && true == json_obj.value(KEY_SUPPORTACTIONS).isObject()){
                quint32 incrementNumber = 0;
                quint32 decrementNumber = 0;
                QJsonObject actions_obj = json_obj.value(KEY_SUPPORTACTIONS).toObject();
                QStringList keys = actions_obj.keys();

                for(const QString &key : keys){
                    if((true == actions_obj.contains(key))
                            && true == actions_obj.value(key).isString()){
                        MappedInfo action;
                        action.key = key;
                        action.value = actions_obj.value(key).toString();
                        applianceinfo.supportActions.append(action);
                        if (true == key.startsWith(KEY_ACTION_INCREMENT)){
                            incrementNumber += 1;
                        }
                        else if (true == key.startsWith(KEY_ACTION_DECREMENT)){
                            decrementNumber += 1;
                        }
                    }
                }

                if (incrementNumber == decrementNumber){
                    applianceinfo.actionBarNum = incrementNumber;
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

                        if((true == json_obj.contains(KEY_VALUE))
                                && true == json_obj.value(KEY_VALUE).isString()){
                            attributeinfo.value = json_obj.value(KEY_VALUE).toString();
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

        V2H_NORMAL_LOG << "GroupNameList.size =" << groupnamelist.size();
    }

    return groupnamelist;
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

        V2H_NORMAL_LOG << "ApplianceTypeList.size =" << appliancetypelist.size();
    }

    return appliancetypelist;
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
                m_V2H_JsonDataIsEnable = verifyV2HJsonData(m_V2H_JsonData);
                if (m_V2H_JsonDataIsEnable != true){
                    return true;
                }

                m_V2H_AppliancesListJsonString = QString(json_buffer);
                m_V2H_ApplianceArray = getJsonAppliancesArrayFromJsonData();

                cJSON cjson_array;
                if (m_V2H_ApplianceArray.size() > 0){
                    cjson_array = makecJSONAppliancesListArray(json_buffer);
                }

                if (false == cJSON_IsNull(&cjson_array)){
                    m_V2H_cJSONAppliancesArray = cjson_array;
                    m_V2H_ApplianceInfoList = makeApplianceInfoListFromJsonArray(m_V2H_ApplianceArray);
                }

                m_GroupNameList = makeGroupNameList();
                if (false == m_GroupNameFilter.isEmpty()){
                    bool setGroupFilter = false;
                    setGroupFilter = setGroupNameFilter(m_GroupNameFilter);

                    if (true == setGroupFilter){
                        V2H_NORMAL_LOG << "Reset GroupNameFilter(" << m_GroupNameFilter << ")" << "Success.";
                    }
                    else{
                        m_V2H_GroupApplianceInfoList = m_V2H_ApplianceInfoList;
                        V2H_ERROR_LOG << "Reset GroupNameFilter(" << m_GroupNameFilter << ")" << "Failure.";
                    }
                }
                else{
                    m_V2H_GroupApplianceInfoList = m_V2H_ApplianceInfoList;
                }

                m_ApplianceTypeList = makeApplianceTypeList();
                if (false == m_ApplianceTypeFilter.isEmpty()){
                    bool setTypeFilter = false;
                    setTypeFilter = setApplianceTypeFilter(m_ApplianceTypeFilter);

                    if (true == setTypeFilter){
                        V2H_NORMAL_LOG << "Reset ApplianceTypeFilter(" << m_ApplianceTypeFilter << ")" << "Success.";
                    }
                    else{
                        m_V2H_TypeApplianceInfoList = m_V2H_ApplianceInfoList;
                        V2H_ERROR_LOG << "Reset ApplianceTypeFilter(" << m_ApplianceTypeFilter << ")" << "Failure.";
                    }
                }
                else{
                    m_V2H_TypeApplianceInfoList = m_V2H_ApplianceInfoList;
                }
                result = true;

                V2H_NORMAL_LOG << "TotalAppliances" << V2HJsonData::getTotalAppliances();
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
        QList<ApplianceInfo> applianceinfolist = makeAppliancesInfoListByGroup(groupname);

        if (applianceinfolist.size() > 0){
            m_V2H_GroupApplianceInfoList = applianceinfolist;
        }

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
        QList<ApplianceInfo> applianceinfolist = makeAppliancesInfoListByType(appliancetype);

        if (applianceinfolist.size() > 0){
            m_V2H_TypeApplianceInfoList = applianceinfolist;
        }

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
            m_SelectedApplianceID = appliance_id;
            m_SelectedApplianceInfo = appliance_info;
            result = true;
        }
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
    return m_V2H_JsonDataIsEnable;
}

QString V2HJsonData::getGroupFilter()
{
    return m_GroupNameFilter;
}

QString V2HJsonData::getSelectedApplianceID()
{
    return m_SelectedApplianceID;
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

ApplianceInfo V2HJsonData::getSelectedApplianceInfo()
{
    return m_SelectedApplianceInfo;
}

QList<MappedInfo> V2HJsonData::getSelectedApplianceModeList()
{
    QList<MappedInfo> modelist;

    if (false == m_SelectedApplianceID.isEmpty()){
        for(const AttributeInfo &attribute : m_SelectedApplianceInfo.attributes){
            if (KEY_ATTR_MODE == attribute.attributename){
                modelist = attribute.detail.range_map;
            }
        }
    }

    return modelist;
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

QList<ApplianceInfo> V2HJsonData::getGroupedAppliancesInfoList()
{
    return m_V2H_GroupApplianceInfoList;
}

QList<ApplianceInfo> V2HJsonData::getTypedAppliancesInfoList()
{
    return m_V2H_TypeApplianceInfoList;
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
    m_V2H_GroupApplianceInfoList = m_V2H_ApplianceInfoList;

    return true;
}

bool V2HJsonData::clearSelectApplianceID()
{
    m_SelectedApplianceIndex = -1;
    m_SelectedApplianceID.clear();
    m_SelectedApplianceInfo = ApplianceInfo();

    return true;
}
