#include "v2hjsondata.h"

static const QString V2H_LOGTAG("[HMI_V2H]");

static const QString KEY_STATUS("status");
static const QString KEY_DATA("data");
static const QString KEY_ISENABLE("isenable");
static const QString KEY_APPLIANCES("appliances");
static const QString KEY_TOTALAPPLIANCES("totalAppliances");
static const int STATUS_OK = 0;

QJsonObject V2HJsonData::m_V2H_JsonData = QJsonObject();
QString V2HJsonData::m_GroupNameFilter = QString();
//quint16 V2HJsonData::m_SelectApplianceIndex = V2H_APPLIANCE_INDEX_INIT;
QString V2HJsonData::m_SelectedApplianceID = QString();
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

bool V2HJsonData::setV2HJsonData(const char *json_buffer)
{
    bool result = false;
    QJsonDocument json_doc = QJsonDocument::fromJson(QByteArray(json_buffer));

    if (true == json_doc.isObject() && false == json_doc.isNull()){
        QJsonObject json_obj = json_doc.object();
        if((true == json_obj.contains(KEY_STATUS))
                && STATUS_OK == json_obj.value(KEY_STATUS).toInt()){
            if((true == json_obj.contains(KEY_DATA))
                    && true == json_obj.value(KEY_DATA).isObject()){
                m_V2H_JsonData = json_obj.value(KEY_DATA).toObject();
                m_V2H_JsonDataIsEnable = verifyV2HJsonData(m_V2H_JsonData);
                m_V2H_ApplianceArray = getJsonAppliancesList();
                result = true;
                V2H_Debug() << "m_V2H_JsonData.size()" << m_V2H_JsonData.size();
            }
        }
    }

    return result;
}

bool V2HJsonData::setGroupFilter(QString groupname)
{
    bool result = false;
    if (false == groupname.isEmpty()){
        m_GroupNameFilter = groupname;
        result = true;
    }

    return result;
}

bool V2HJsonData::setSelectApplianceID(QString appliance_id)
{
    bool result = false;
    if (false == appliance_id.isEmpty()){
        m_SelectedApplianceID = appliance_id;
        result = true;
    }

    return result;
}

#if 0
bool V2HJsonData::setSelectApplianceIndex(quint16 index)
{
    bool result = false;
    if (index < V2H_APPLIANCE_INDEX_MAX){
        m_SelectApplianceIndex = index;
        result = true;
    }

    return result;
}
#endif

bool V2HJsonData::getV2HJsonDataIsEnable()
{
    return m_V2H_JsonDataIsEnable;
}

QString V2HJsonData::getGroupFilter()
{
    return m_GroupNameFilter;
}

QString V2HJsonData::getSelectApplianceID()
{
    return m_SelectedApplianceID;
}

int V2HJsonData::getSelectApplianceIndexFromID(QString &appliance_id)
{

}

#if 0
int V2HJsonData::getSelectApplianceIndex()
{
    return m_SelectApplianceIndex;
}
#endif

QJsonArray V2HJsonData::getJsonAppliancesList()
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

QJsonObject V2HJsonData::getJsonApplianceFromID(QString &appliance_id)
{
    QJsonObject json_obj;

    if (false == appliance_id.isEmpty()){
        QJsonArray json_array = V2HJsonData::getJsonAppliancesList();

    }

    return json_obj;
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

    return true;
}

#if 0
bool V2HJsonData::clearSelectApplianceIndex()
{
    m_SelectApplianceIndex = V2H_APPLIANCE_INDEX_INIT;

    return true;
}
#endif

bool V2HJsonData::clearSelectApplianceID()
{
    m_SelectedApplianceID.clear();
}
