/*-------------------------------------------------
#
# Project developed by Nintersoft team
# Developer: Mauro Mascarenhas de Araújo
# Contact: mauro.mascarenhas@nintersoft.com
# License: Nintersoft Open Source Code Licence
# Date: 23 of January of 2019
#
------------------------------------------------- */

#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "dbmanager_global.h"

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>

#include <exception>

#include <QStringList>
#include <QByteArray>
#include <QSqlDriver>
#include <QMetaType>
#include <QDateTime>
#include <QVariant>
#include <QPixmap>
#include <QBuffer>
#include <QList>

class DBMANAGERSHARED_EXPORT DBManager : public QSqlDatabase
{

public:

    //Supported Drivers (According to Qt)
    enum DBConnectionType{
        DB2,
        IBASE,
        MYSQL,
        OCI,
        ODBC,
        PSQL,
        SQLITE,
        SQLITE2,
        TDS,
        UNDEFINED
    };

    //Configuration Assistant Class
    class DBData {
        public:
            DBData(){
                this->databaseConnectionTypeS = UNDEFINED;
                this->databaseNumericalPrecisionPolicy = QSql::LowPrecisionDouble;
                this->databasePort = -1;
                this->databaseHostName =
                this->databaseNameS =
                this->databaseUserame =
                this->databasePassword =
                this->databaseConnectionName =
                this->databaseConnectionOptions =
                this->databaseTablePrefix = QString();
            }
            ~DBData(){}

            inline void setHostName(const QString &host){ this->databaseHostName = host; }
            inline void setDatabaseName(const QString &name){ this->databaseNameS = name; }
            inline void setUserName(const QString &name){ this->databaseUserame = name; }
            inline void setPassword(const QString &password){ this->databasePassword = password; }
            inline void setConnectionName(const QString &cName){ this->databaseConnectionName = cName; }
            inline void setConnectOptions(const QString &options){ this->databaseConnectionOptions = options; }
            inline void setTablePrefix(const QString &prefix){ this->databaseTablePrefix = prefix; }
            inline void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy){ this->databaseNumericalPrecisionPolicy = precisionPolicy; }
            inline void setDatabaseConnectionType(DBConnectionType type) { this->databaseConnectionTypeS = type; }
            inline void setPort(int port){ this->databasePort = port; }

            inline QString hostName() const { return this->databaseHostName; }
            inline QString databaseName() const { return this->databaseNameS; }
            inline QString username() const { return this->databaseUserame; }
            inline QString password() const { return this->databasePassword; }
            inline QString connectionName() const { return this->databaseConnectionName; }
            inline QString connectOptions() const { return this->databaseConnectionOptions; }
            inline QString tablePrefix() const { return this->databaseTablePrefix; }
            inline QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const { return this->databaseNumericalPrecisionPolicy; }
            inline DBConnectionType connectionType() const { return this->databaseConnectionTypeS; }
            inline int port() const { return this->databasePort; }

        private:
            QString databaseHostName;
            QString databaseNameS;
            QString databaseUserame;
            QString databasePassword;
            QString databaseConnectionName;
            QString databaseConnectionOptions;
            QString databaseTablePrefix;
            QSql::NumericalPrecisionPolicy databaseNumericalPrecisionPolicy;
            DBConnectionType databaseConnectionTypeS;
            int databasePort;
    };

    //Singleton Method to get current database instance (Unique Object)
    static DBManager* getInstance(const DBData &data = DBData());
    //Destroy the current object instance (only if necessary)
    bool removeInstance();

    bool createTable(const QString &tableName, const QStringList &columns);
    bool insertRow(const QString &tableName, const QStringList &columnName, const QVariantList &data);

    bool updateRow(const QString &tableName, const QString &columnNameCond, const QVariant &condition,
                       const QStringList &columnName, const QVariantList &data,
                       const QString &operation = "=");
    bool updateRow(const QString &tableName, const QStringList &columnNameCond, const QVariantList &condition,
                       const QStringList &columnName, const QVariantList &data,
                       const QString &operation = "=");

    bool removeRow(const QString &tableName, const QString &columnNameCond, const QVariant &condition,
                       const QString &operation = "=");
    bool removeRow(const QString &tableName, const QStringList &columnNameCond, const QVariantList &condition,
                       const QString &operation = "=");

    bool rowExists(const QString &tableName, const QString &columnNameCond, const QVariant &data,
                       const QString &operation = "=");
    bool rowExists(const QString &tableName, const QStringList &columnNameCond, const QVariantList &data,
                       const QString &operation = "=");

    QVariantList retrieveRow(const QString &tableName, const QString &columnNameCond,
                                const QVariant &condition, const QString &operation = "=");
    QVariantList retrieveRow(const QString &tableName, const QStringList &columnNameCond,
                                const QVariantList &condition, const QString &operation = "=");
    QVariantList retrieveRow(const QString &tableName, const QStringList &columnNameCond,
                                const QVariantList &condition, QStringList columnName,
                                const QString &operation = "=");

    QList< QVariantList > retrieveAll(const QString &tableName, const QStringList &columns = QStringList(),
                                          const QStringList &groupby = QStringList(),
                                          const QStringList &orderby = QStringList());

    QList< QVariantList > retrieveAllCond(const QString &tableName,
                                              const QString &columnCondition, const QVariant &condition,
                                              const QString &operation = "=",
                                              const QStringList &orderby = QStringList());
    QList< QVariantList > retrieveAllCond(const QString &tableName,
                                          const QStringList &columnCondition, const QVariantList &condition,
                                          const QString &operation = "=",
                                          const QStringList &orderby = QStringList());
    QList< QVariantList > retrieveAllCond(const QString &tableName, const QStringList &columnName,
                                          const QString &columnCondition, const QVariant &condition,
                                          const QString &operation = "=",
                                          const QStringList &groupby = QStringList(),
                                          const QStringList &orderby = QStringList());
    QList< QVariantList > retrieveAllCond(const QString &tableName, const QStringList &columnName,
                                              const QStringList &columnCondition, const QVariantList &condition,
                                              const QString &operation = "=",
                                              const QStringList &groupby = QStringList(),
                                              const QStringList &orderby = QStringList());

    int rowsCount(const QString &tableName);
    int rowsCountCond(const QString &tableName,
                        const QString &columnCondition, const QVariant &condition,
                        const QString &operation = "=");
    int rowsCountCond(const QString &tableName,
                          const QStringList &columnCondition, const QVariantList &condition,
                          const QString &operation = "=");

    bool clearTable(const QString &tableName);
    bool dropTable(const QString &tableName);

    QSqlQuery createCustomQuery(const QString &query = "");

    static QVariant pixmapToVariant(const QPixmap &pixmap);
    static QPixmap variantToPixmap(const QVariant &variant);

    static QString getUniqueConnectionName(const QString &partname = "");

    bool setConnectionType(DBConnectionType cType);
    DBConnectionType connectionType();

    bool setConnectionName(const QString &cName);
    QString currentConnectionName();

    bool setDBPrefix(const QString &prefix);
    inline QString dbPrefix() { return this->dbData.tablePrefix(); }

    bool setDatabaseData(const DBManager::DBData &dbData);
    DBManager::DBData databaseData();

private:
    explicit DBManager(const DBManager::DBData &data = DBData());
    ~DBManager();

    QString buildQuery(const QString &tableName, const QStringList &columnName = QStringList(),
                           const QStringList &columnCondition = QStringList(),
                           const QVariantList &condition = QVariantList(),
                           const QString &operation = "=",
                           const QStringList &groupby = QStringList(),
                           const QStringList &orderby = QStringList(),
                           const QStringList &bindFields = QStringList());

    QSqlQuery buildBindedQuery(const QString &tableName, const QStringList &columnName = QStringList(),
                                   const QStringList &columnCondition = QStringList(),
                                   const QVariantList &condition = QVariantList(),
                                   const QString &operation = "=",
                                   const QStringList &groupby = QStringList(),
                                   const QStringList &orderby = QStringList());

    QList< QVariantList > executeSelectQuery(QSqlQuery &query);

    static DBManager *currentInstance;

protected:
    inline bool hasQuerySize() { return this->driverHasQuerySize; }

    using QSqlDatabase::addDatabase;
    using QSqlDatabase::cloneDatabase;
    using QSqlDatabase::connectionNames;
    using QSqlDatabase::contains;
    using QSqlDatabase::database;
    using QSqlDatabase::removeDatabase;
    using QSqlDatabase::operator=;

    QString getConnectionType(DBConnectionType cType);

    DBManager::DBData dbData;
    bool driverHasQuerySize;
};

#endif // DBMANAGER_H
