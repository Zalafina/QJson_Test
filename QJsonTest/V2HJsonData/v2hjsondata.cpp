#include "v2hjsondata.h"

static const QString V2H_LOGTAG("[HMI_V2H]");

static const QString KEY_STATUS("status");
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

QString V2HJsonData::m_V2H_OriginalJsonString = QString();
QJsonObject V2HJsonData::m_V2H_JsonData = QJsonObject();
QJsonArray V2HJsonData::m_V2H_ApplianceArray = QJsonArray();
QList<ApplianceInfo> V2HJsonData::m_V2H_ApplianceInfoList = QList<ApplianceInfo>();
QList<ApplianceInfo> V2HJsonData::m_V2H_GroupApplianceInfoList = QList<ApplianceInfo>();
QString V2HJsonData::m_GroupNameFilter = QString();
QStringList V2HJsonData::m_GroupNameList = QStringList();
QString V2HJsonData::m_SelectedApplianceID = QString();
ApplianceInfo V2HJsonData::m_SelectedApplianceInfo = ApplianceInfo();
bool V2HJsonData::m_V2H_JsonDataIsEnable = false;

V2HJsonData::V2HJsonData(QObject *parent) : QObject(parent)
{

}

V2HJsonData::~V2HJsonData()
{

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
        for(const QJsonValue &json_value : json_array){
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

                                QStringList keys = json_range.keys();

                                for(const QString &key : keys){
                                    if((true == json_range.contains(key))
                                            && true == json_range.value(key).isString()){
                                        MappedInfo range;
                                        range.key = key;
                                        range.value = json_range.value(key).toString();
                                        attributeinfo.detail.range_map.append(range);
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
        }
    }

    return applianceinfolist;
}

QStringList V2HJsonData::makeGroupNameList()
{
    QStringList groupnamelist;

    if (getTotalAppliances() > 0){

        for(const ApplianceInfo &appliance : m_V2H_ApplianceInfoList){
            if (false == appliance.groupName.isEmpty()){
                groupnamelist.append(appliance.groupName);
            }
        }

        V2H_Debug().noquote() << __FUNCTION__ << ":" << "GroupNameList.size =" << groupnamelist.size();
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

bool V2HJsonData::setV2HJsonData(const char *json_buffer)
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
                m_V2H_OriginalJsonString = QString(json_buffer);
                m_V2H_JsonData = json_obj.value(KEY_DATA).toObject();
                m_V2H_JsonDataIsEnable = verifyV2HJsonData(m_V2H_JsonData);
                m_V2H_ApplianceArray = getJsonAppliancesArrayFromJsonData();
                m_V2H_ApplianceInfoList = makeApplianceInfoListFromJsonArray(m_V2H_ApplianceArray);

                if (false == m_GroupNameFilter.isEmpty()){
                    m_V2H_GroupApplianceInfoList = makeAppliancesInfoListByGroup(m_GroupNameFilter);
                }
                else{
                    m_V2H_GroupApplianceInfoList = m_V2H_ApplianceInfoList;
                }
                m_GroupNameList = makeGroupNameList();
                result = true;

                V2H_Debug().noquote() << "getTotalAppliances" << V2HJsonData::getTotalAppliances();
            }
        }
    }

    return result;
}

bool V2HJsonData::setGroupFilter(QString groupname)
{
    bool result = false;
    bool find = false;

    for(const QString &group : m_GroupNameList){
        if (groupname == group){
            find = true;
            V2H_Debug().noquote().nospace() << __FUNCTION__ << ":" << "GroupName(" << groupname << ") found.";
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

bool V2HJsonData::setSelectApplianceID(QString appliance_id)
{
    bool result = false;

    if (false == appliance_id.isEmpty()){
        ApplianceInfo appliance_info;
        int appliance_index = getApplianceInfoFromID(appliance_id, appliance_info);

        if (appliance_index >= 0){
            m_SelectedApplianceID = appliance_id;
            m_SelectedApplianceInfo = appliance_info;
            result = true;
        }
    }

    return result;
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

QJsonArray V2HJsonData::getJsonAppliancesArrayFromJsonData()
{
    QJsonArray json_array;

    if((true == m_V2H_JsonData.contains(KEY_APPLIANCES))
            && (true == m_V2H_JsonData.value(KEY_APPLIANCES).isArray())
            && (true == m_V2H_JsonData.contains(KEY_TOTALAPPLIANCES))
            && (0 < m_V2H_JsonData.value(KEY_TOTALAPPLIANCES).toInt() && m_V2H_JsonData.value(KEY_TOTALAPPLIANCES).toInt() < V2H_APPLIANCE_INDEX_MAX)){
        if (m_V2H_JsonData.value(KEY_APPLIANCES).toArray().size() == m_V2H_JsonData.value(KEY_TOTALAPPLIANCES).toInt()){
            json_array = m_V2H_JsonData.value(KEY_APPLIANCES).toArray();
            V2H_Debug("%s totalAppliances:%d", V2H_LOGTAG.toStdString().c_str(), m_V2H_JsonData.value(KEY_TOTALAPPLIANCES).toInt());
        }
        else{
            V2H_Error("%s totalAppliances:%d, appliancesArray size:%d", V2H_LOGTAG.toStdString().c_str(), m_V2H_JsonData.value(KEY_TOTALAPPLIANCES).toInt(), m_V2H_JsonData.value(KEY_APPLIANCES).toArray().size());
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
    V2H_Debug() << "m_V2H_JsonData.size()" << m_V2H_JsonData.size();

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
    m_SelectedApplianceID.clear();
    m_SelectedApplianceInfo = ApplianceInfo();

    return true;
}
