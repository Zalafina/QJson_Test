#ifndef V2HJSONDATA_H
#define V2HJSONDATA_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "cJSON.h"

#define V2H_JSON_DATA_DEBUG

#ifdef V2H_JSON_DATA_DEBUG
#define V2H_Debug  qDebug
#else
#define V2H_Debug  QT_NO_QDEBUG_MACRO
#endif
#define V2H_Error  qWarning

#define V2H_NORMAL_LOG V2H_Debug().noquote().nospace()<<V2H_LOGTAG<<__FUNCTION__<<":"
#define V2H_ERROR_LOG V2H_Debug().noquote().nospace()<<V2H_LOGTAG<<V2H_LOGERROR<<__FUNCTION__<<":"

#define V2H_APPLIANCE_INDEX_MAX    (500)

typedef struct
{
    QString key;
    QString value;
}MappedInfo;

typedef struct
{
    QString name;
    QString scale;
    QList<MappedInfo> range_map;
    double range_min;
    double range_max;
    bool minmax_flag;
}AttributeDetailInfo;

typedef struct
{
    QString attributename;
    AttributeDetailInfo detail;
    QString value;
}AttributeInfo;

typedef struct
{
    QString botName;
    QString botId;
    QString botLogo;
    QString num;
    QString applianceId;
    QStringList applianceTypes;
    QString friendlyName;
    QString friendlyDescription;
    QString groupName;
    QJsonObject guideline;
    QList<MappedInfo> supportActions;
    quint32 actionBarNum;
    QList<AttributeInfo> attributes;
}ApplianceInfo;

class V2HJsonData : public QObject
{
    Q_OBJECT
public:
    explicit V2HJsonData(QObject *parent = 0);
    ~V2HJsonData();

    /* Common Function */
    static cJSON makecJSONAppliancesListArray(const char *json_buffer);
    static bool verifyV2HJsonData(QJsonObject &json_obj);
    static QList<ApplianceInfo> makeApplianceInfoListFromJsonArray(QJsonArray &json_array);
    static QStringList makeGroupNameList(void);
    static QList<ApplianceInfo> makeAppliancesInfoListByGroup(QString &groupname);
    static QStringList makeApplianceTypeList(void);
    static QList<ApplianceInfo> makeAppliancesInfoListByType(QString &appliancetype);

    /* Set Function for V2H Data */
    static bool setV2HAppliancesListJsonData(const char *json_buffer);
    static bool setGroupNameFilter(QString groupname);
    static bool setApplianceTypeFilter(QString appliancetype);
    static bool setSelectApplianceID(QString appliance_id);

    /* Get Function for V2H Data */
    static bool getV2HJsonDataIsEnable(void);
    static QString getGroupFilter(void);
    static QString getSelectedApplianceID(void);
    static int getSelectedApplianceIndex(void);
    static int getTotalAppliances(void);
    static QStringList getGroupNameList(void);
    static ApplianceInfo getSelectedApplianceInfo(void);
    static QList<MappedInfo> getSelectedApplianceModeList(void);
    static int getApplianceInfoFromID(QString &appliance_id, ApplianceInfo &applianceinfo);
    static QList<ApplianceInfo> getAppliancesInfoList(void);
    static QList<ApplianceInfo> getGroupedAppliancesInfoList(void);
    static QList<ApplianceInfo> getTypedAppliancesInfoList(void);

    /* Get Function for V2H JsonData */
    static QJsonArray getJsonAppliancesArrayFromJsonData(void);
    static QJsonArray getJsonAppliancesArray(void);

    /* Clear Function for V2H Data */
    static bool clearV2HJsonData(void);
    static bool clearGroupFilter(void);
    static bool clearSelectApplianceID(void);

signals:

public slots:

public:
    static QString m_V2H_OriginalJsonString;
    static QJsonObject m_V2H_JsonData;
    static QJsonArray  m_V2H_ApplianceArray;
    static cJSON m_V2H_cJSONAppliancesArray;
    static QList<ApplianceInfo> m_V2H_ApplianceInfoList;
    static QList<ApplianceInfo> m_V2H_GroupApplianceInfoList;
    static QList<ApplianceInfo> m_V2H_TypeApplianceInfoList;
    static QString m_GroupNameFilter;
    static QStringList m_GroupNameList;
    static QString m_ApplianceTypeFilter;
    static QStringList m_ApplianceTypeList;
    static QString m_SelectedApplianceID;
    static int m_SelectedApplianceIndex;
    static ApplianceInfo m_SelectedApplianceInfo;
    static bool m_V2H_JsonDataIsEnable;

private:
};

#endif // V2HJSONDATA_H
