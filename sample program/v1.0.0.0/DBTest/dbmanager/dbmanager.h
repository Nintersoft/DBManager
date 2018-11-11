/*-------------------------------------------------
#
# Project developed by Nintersoft team
# Developer: Mauro Mascarenhas de Araújo
# Contact: mauro.mascarenhas@nintersoft.com
# License: Nintersoft Open Source Code Licence
# Date: 06 of June of 2018
#
------------------------------------------------- */

#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "dbmanager_global.h"

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>

#include <QStringList>
#include <QByteArray>
#include <QSqlDriver>
#include <QDateTime>
#include <QVariant>
#include <QPixmap>
#include <QBuffer>
#include <QList>

class DBMANAGERSHARED_EXPORT DBManager
{

public:

    enum DBConnectionType{
        SQLITE,
        MYSQL
    };

    class DBData {
        public:
            DBData(){}
            ~DBData(){}

            inline void setHostName(const QString &host){ this->databaseHostName = host; }
            inline void setDatabaseName(const QString &name){ this->databaseNameS = name; }
            inline void setUserName(const QString &name){ this->databaseUserame = name; }
            inline void setPassword(const QString &password){ this->databasePassword = password; }
            inline void setConnectOptions(const QString &options = QString()){ this->databaseConnectionOptions = options; }
            inline void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy){ this->databaseNumericalPrecisionPolicy = precisionPolicy; }
            inline void setPort(int port){ this->databasePort = port; }

            inline QString hostName() const { return this->databaseHostName; }
            inline QString databaseName() const { return this->databaseNameS; }
            inline QString username() const { return this->databaseUserame; }
            inline QString password() const { return this->databasePassword; }
            inline QString connectionOptions() const { return this->databaseConnectionOptions; }
            inline QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const { return this->databaseNumericalPrecisionPolicy; }
            inline int port() const { return this->databasePort; }

        private:
            QString databaseHostName;
            QString databaseNameS;
            QString databaseUserame;
            QString databasePassword;
            QString databaseConnectionOptions;
            QSql::NumericalPrecisionPolicy databaseNumericalPrecisionPolicy;
            int databasePort;
    };

    explicit DBManager(const DBManager::DBData &data = DBData(), const QString &tablePrefix = "",
                       DBConnectionType connectionType = SQLITE, const QString &connectionName = QString("default"));
    explicit DBManager(const QStringList &data, DBConnectionType connectionType = SQLITE,
                          const QString &connectionName = QString("default"));
    ~DBManager();

    bool createTable(const QString &tableName, const QStringList &columns);
    bool insertRow(const QString &tableName, const QStringList &columnName, const QList<QVariant> &data);

    bool updateRow(const QString &tableName, const QString &columnNameCond, const QVariant &condition,
                   const QStringList &columnName, const QList<QVariant> &data,
                   const QString &operation = "=");
    bool updateRow(const QString &tableName, const QStringList &columnNameCond, const QList<QVariant> &condition,
                   const QStringList &columnName, const QList<QVariant> &data,
                   const QString &operation = "=");

    bool removeRow(const QString &tableName, const QString &columnNameCond, const QVariant &condition,
                   const QString &operation = "=");
    bool removeRow(const QString &tableName, const QStringList &columnNameCond, const QList<QVariant> &condition,
                   const QString &operation = "=");

    bool rowExists(const QString &tableName, const QString &columnNameCond, const QVariant &data,
                   const QString &operation = "=");
    bool rowExists(const QString &tableName, const QStringList &columnNameCond, const QList<QVariant> &data,
                   const QString &operation = "=");

    QList<QVariant> retrieveRow(const QString &tableName, const QString &columnNameCond,
                                    const QVariant &condition, const QString &operation = "=");
    QList<QVariant> retrieveRow(const QString &tableName, const QStringList &columnNameCond,
                                    const QList<QVariant> &condition, const QString &operation = "=");
    QList<QVariant> retrieveRow(const QString &tableName, const QStringList &columnNameCond,
                                    const QList<QVariant> &condition, QStringList columnName, const QString &operation = "=");

    QList< QList<QVariant> > retrieveAll(const QString &tableName);
    QList< QList<QVariant> > retrieveAll(const QString &tableName, const QStringList &columns);

    QList< QList<QVariant> > retrieveAllCond(const QString &tableName,
                                             const QString &columnCondition, const QVariant &condition, const QString &operation = "=");
    QList< QList<QVariant> > retrieveAllCond(const QString &tableName,
                                             const QStringList &columnCondition, const QList<QVariant> &condition, const QString &operation = "=");
    QList< QList<QVariant> > retrieveAllCond(const QString &tableName, const QStringList &columnName,
                                             const QString &columnCondition, const QVariant &condition, const QString &operation = "=");
    QList< QList<QVariant> > retrieveAllCond(const QString &tableName, const QStringList &columnName,
                                             const QStringList &columnCondition, const QList<QVariant> &condition, const QString &operation = "=");

    int rowsCount(const QString &tableName);
    int rowsCountCond(const QString &tableName,
                        const QString &columnCondition, const QVariant &condition, const QString &operation = "=");
    int rowsCountCond(const QString &tableName,
                        const QStringList &columnCondition, const QList<QVariant> &condition, const QString &operation = "=");

    bool clearTable(const QString &tableName);
    bool dropTable(const QString &tableName);

    QSqlQuery runCustomQuery();
    QVariant pixmapToVariant(const QPixmap &pixmap);
    QPixmap variantToPixmap(const QVariant &variant);

    static QString getUniqueConnectionName(const QString &partname = "");

    bool setConnectionType(DBConnectionType cType);
    DBConnectionType connectionType();

    bool setConnectionName(const QString &cName);
    QString currentConnectionName();

    bool setDatabaseData(const DBManager::DBData &dbData);
    const DBManager::DBData databaseData();

    bool setDBPrefix(const QString &prefix);
    inline QString dbPrefix() { return this->prefix; }

    bool openDB();
    bool isOpen();
    bool isValid();
    void closeDB();

    QSqlError lastError();

    bool hasValidSettings();

private:
    void setConnectionTypeB(DBConnectionType cType);
    void setConnectionNameB(const QString &cName);

    QSqlDatabase database;
    QString prefix;

    DBManager::DBData dbData;
    bool validSettings;

    DBConnectionType currentType;
};

#endif // DBMANAGER_H
