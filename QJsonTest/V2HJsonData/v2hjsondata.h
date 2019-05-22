#ifndef V2HJSONDATA_H
#define V2HJSONDATA_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#define V2H_JSON_DATA_DEBUG

#ifdef V2H_JSON_DATA_DEBUG
#define V2H_Debug  qDebug
#else
#define V2H_Debug  QT_NO_QDEBUG_MACRO
#endif
#define V2H_Error  qWarning

//#define V2H_APPLIANCE_INDEX_INIT   (0xFFFFU)
#define V2H_APPLIANCE_INDEX_MAX    (500)

class V2HJsonData : public QObject
{
    Q_OBJECT
public:
    explicit V2HJsonData(QObject *parent = 0);
    ~V2HJsonData();

    /*  */
    static bool verifyV2HJsonData(QJsonObject &json_obj);

    /* Set Function for V2H Data */
    static bool setV2HJsonData(const char *json_buffer);
    static bool setGroupFilter(QString groupname);
    //static bool setSelectApplianceIndex(quint16 index);
    static bool setSelectApplianceID(QString appliance_id);

    /* Get Function for V2H Data */
    static bool getV2HJsonDataIsEnable(void);
    static QString getGroupFilter(void);
    static QString getSelectApplianceID(void);

    /* Get Function for V2H JsonData */
    static QJsonArray getJsonAppliancesList(void);
    static int getJsonApplianceFromID(QString &appliance_id, QJsonObject &appliance_json);

    /* Clear Function for V2H Data */
    static bool clearV2HJsonData(void);
    static bool clearGroupFilter(void);
    //static bool clearSelectApplianceIndex(void);
    static bool clearSelectApplianceID(void);

signals:

public slots:

public:
    static QJsonObject m_V2H_JsonData;
    static QJsonArray  m_V2H_ApplianceArray;
    static QString m_GroupNameFilter;
    //static quint16 m_SelectApplianceIndex;
    static QString m_SelectedApplianceID;
    static bool m_V2H_JsonDataIsEnable;

private:
};

#endif // V2HJSONDATA_H
