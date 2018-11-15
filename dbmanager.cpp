/*-------------------------------------------------
#
# Project developed by Nintersoft team
# Developer: Mauro Mascarenhas de Araújo
# Contact: mauro.mascarenhas@nintersoft.com
# License: Nintersoft Open Source Code Licence
# Date: 15 of November of 2018
#
------------------------------------------------- */

#include "dbmanager.h"

DBManager* DBManager::currentInstance = NULL;

DBManager::DBManager(const DBManager::DBData &data) :
    QSqlDatabase( data.connectionName().isEmpty() ?
                      QSqlDatabase::addDatabase(getConnectionType(data.connectionType())) :
                      QSqlDatabase::addDatabase(getConnectionType(data.connectionType()), data.connectionName())){
    this->setDBPrefix(data.tablePrefix());
    this->setDatabaseData(data);

    this->driverHasQuerySize = this->driver()->hasFeature(QSqlDriver::QuerySize);
}

DBManager::~DBManager(){
    if (this->isOpen()) this->close();
    this->removeDatabase(dbData.connectionName());
}

DBManager* DBManager::getInstance(const DBData &data){
    if (!currentInstance){
        if (data.databaseName().isNull() || data.connectionType() == UNDEFINED)
            throw std::invalid_argument("Database name is empty or database type is undefined.");

        currentInstance = new DBManager(data);
    }
    return currentInstance;
}

bool DBManager::removeInstance(){
    if (currentInstance && !this->isOpen()){
        delete currentInstance;
        currentInstance = NULL;
        return true;
    }
    return false;
}

bool DBManager::createTable(const QString &tableName, const QStringList &columns){
    if (!this->isOpen() || tableName.isEmpty() || tableName.isNull()
            || columns.isEmpty()) return false;

    QString command = "CREATE TABLE IF NOT EXISTS " + this->dbData.tablePrefix() + tableName + "(";
    for (int i = 0; i < columns.length() - 1; ++i)
        command += columns.at(i) + ", ";

    command += (columns.at(columns.length() - 1) + ");");

    QSqlQuery query(QSqlDatabase::database(this->connectionName()));
    query.prepare(command);
    return query.exec();
}

bool DBManager::insertRow(const QString &tableName, const QStringList &columnName, const QVariantList &data){
    if (tableName.isEmpty() || tableName.isNull() || columnName.isEmpty()
            || data.isEmpty() || data.size() != columnName.size()) return false;

    QString command = "INSERT INTO " + this->dbData.tablePrefix() + tableName + " (";
    int columns = columnName.length();
    for (int i = 0; i < columns - 1; ++i)
        command += (columnName.at(i) + ", ");

    command += (columnName.at(columns - 1) + ") VALUES (");
    for (int i = 0; i < columns - 1; ++i)
        command += (":" + columnName.at(i) + ", ");

    command += (":" + columnName.at(columns - 1) + ");");

    QSqlQuery queryAdd(QSqlDatabase::database(this->connectionName()));
    queryAdd.prepare(command);

    for (int i = 0; i < columns; ++i)
        queryAdd.bindValue(":" + columnName.at(i), data.at(i));

    return queryAdd.exec();
}

bool DBManager::updateRow(const QString &tableName, const QString &columnNameCond,
                            const QVariant &condition, const QStringList &columnName,
                            const QVariantList &data, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull() || columnNameCond.isEmpty()
            || columnNameCond.isNull() || columnName.isEmpty() || columnName.size() != data.size()) return false;

    QString command;
    int columns = columnName.length();

    command = "UPDATE " + this->dbData.tablePrefix() + tableName + " SET ";
    for (int i = 0; i < columns - 1; ++i)
        command += (columnName.at(i) + " = :" + columnName.at(i) + ", ");

    command += (columnName.at(columns - 1) + " = :" + columnName.at(columns - 1) + " WHERE " + columnNameCond + " "
                + operation + " :" + columnNameCond + "c;");

    QSqlQuery queryAdd(QSqlDatabase::database(this->connectionName()));
    queryAdd.prepare(command);
    queryAdd.bindValue(":" + columnNameCond + "c", condition);
    for (int i = 0; i < columns; ++i)
        queryAdd.bindValue(":" + columnName.at(i), data.at(i));

    return queryAdd.exec();
}

bool DBManager::updateRow(const QString &tableName, const QStringList &columnNameCond,
                            const QVariantList &condition, const QStringList &columnName,
                            const QVariantList &data, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull() || condition.isEmpty()
            || columnNameCond.isEmpty() || columnName.isEmpty() || data.isEmpty()
            || columnNameCond.size() != condition.size() || columnName.size() != data.size()) return false;

    QString command;
    int columns = columnName.length();
    int columnsS = columnNameCond.length();

    command = "UPDATE " + this->dbData.tablePrefix() + tableName + " SET ";
    for (int i = 0; i < columns - 1; ++i)
        command += (columnName.at(i) + " = :" + columnName.at(i) + ", ");

    command += (columnName.at(columns - 1) + " = :" + columnName.at(columns - 1) + " WHERE ");
    for (int i = 0; i < columnsS - 1; ++i)
        command += (columnNameCond.at(i) + " " + operation + " :" + columnNameCond.at(i) + "c AND ");

    command += (columnNameCond.at(columnsS - 1) + " " + operation + " :" + columnNameCond.at(columnsS - 1) + "c;");

    QSqlQuery queryUpdate(QSqlDatabase::database(this->connectionName()));
    queryUpdate.prepare(command);
    for (int i = 0; i < columnsS; ++i)
        queryUpdate.bindValue(":" + columnNameCond.at(i) + "c", condition.at(i));

    for (int i = 0; i < columns; ++i)
        queryUpdate.bindValue(":" + columnName.at(i), data.at(i));

    return queryUpdate.exec();
}

bool DBManager::removeRow(const QString &tableName, const QString &columnNameCond,
                            const QVariant &condition, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull() || condition.isNull()
            || columnNameCond.isEmpty() || columnNameCond.isNull()) return false;

    if (!rowExists(tableName, columnNameCond, condition, operation)) return false;

    QSqlQuery removeQuery(QSqlDatabase::database(this->connectionName()));
    QString command;

    command = "DELETE FROM " + this->dbData.tablePrefix() + tableName + " WHERE "
                + columnNameCond + " " + operation + " :" + columnNameCond + ";";

    removeQuery.prepare(command);
    removeQuery.bindValue(":" + columnNameCond, condition);

    return removeQuery.exec();
}

bool DBManager::removeRow(const QString &tableName, const QStringList &columnNameCond,
                            const QVariantList &condition, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull()
            || condition.isEmpty() || columnNameCond.isEmpty()
            || columnNameCond.size() != condition.size()) return false;

    if (!rowExists(tableName, columnNameCond, condition, operation)) return false;

    QSqlQuery removeQuery(QSqlDatabase::database(this->connectionName()));
    QString command;
    int columns = columnNameCond.length();

    command = "DELETE FROM " + this->dbData.tablePrefix() + tableName + " WHERE ";
    for (int i = 0; i < columns - 1; ++i)
        command += (columnNameCond.at(i) + " " + operation + " :" + columnNameCond.at(i) + " AND ");

    command += (columnNameCond.at(columns - 1) + " " + operation + " :" + columnNameCond.at(columns - 1) + ";");

    removeQuery.prepare(command);
    for (int i = 0; i < columns; ++i)
        removeQuery.bindValue((":" + columnNameCond.at(i)), condition.at(i));

    return removeQuery.exec();

}

bool DBManager::rowExists(const QString &tableName, const QString &columnNameCond,
                            const QVariant &data, const QString &operation){
    QSqlQuery checkQuery = buildBindedQuery(tableName, QStringList(),
                                            QStringList() << columnNameCond,
                                            QVariantList() << data,
                                            operation);
    bool exists = false;
    if (checkQuery.exec())
        exists = checkQuery.next();

    return exists;
}

bool DBManager::rowExists(const QString &tableName, const QStringList &columnNameCond,
                            const QVariantList &data, const QString &operation){
    QSqlQuery checkQuery = buildBindedQuery(tableName, QStringList(),
                                            columnNameCond, data,
                                            operation);
    bool exists = false;
    if (checkQuery.exec())
        exists = checkQuery.next();
    return exists;
}

QVariantList DBManager::retrieveRow(const QString &tableName, const QString &columnNameCond,
                                        const QVariant &condition, const QString &operation){
    QSqlQuery retrieveQuery = buildBindedQuery(tableName, QStringList(),
                                               QStringList() << columnNameCond,
                                               QVariantList() << condition,
                                               operation);
    QVariantList retrievedData;

    if (retrieveQuery.exec()){
        if (retrieveQuery.next()){
            int max = retrieveQuery.record().count();
            for (int i = 0; i < max; ++i)
                retrievedData << retrieveQuery.value(i);
        }
    }
    return retrievedData;
}

QVariantList DBManager::retrieveRow(const QString &tableName, const QStringList &columnNameCond,
                                        const QVariantList &condition, const QString &operation){
    QSqlQuery retrieveQuery = buildBindedQuery(tableName, QStringList(),
                                               columnNameCond, condition,
                                               operation);
    QVariantList retrievedData;

    if (retrieveQuery.exec()){
        if (retrieveQuery.next()){
            int max = retrieveQuery.record().count();
            for (int i = 0; i < max; ++i)
                retrievedData << retrieveQuery.value(i);
        }
    }
    return retrievedData;
}

QVariantList DBManager::retrieveRow(const QString &tableName, const QStringList &columnNameCond,
                                       const QVariantList &condition, QStringList columnName,
                                       const QString &operation){
    QSqlQuery retrieveQuery = buildBindedQuery(tableName, columnName,
                                               columnNameCond, condition,
                                               operation);
    QVariantList retrievedData;

    if (retrieveQuery.exec()){
        if (retrieveQuery.next()){
            int max = retrieveQuery.record().count();
            for (int i = 0; i < max; ++i)
                retrievedData << retrieveQuery.value(i);
        }
    }
    return retrievedData;
}

QList<QVariantList> DBManager::retrieveAll(const QString &tableName, const QStringList &columns,
                                            const QStringList &groupby, const QStringList &orderby){
    QSqlQuery retrieveQuery = buildBindedQuery(tableName, columns,
                                               QStringList(), QVariantList(),
                                               "=", groupby, orderby);
    return executeSelectQuery(retrieveQuery);
}

QList<QVariantList> DBManager::retrieveAllCond(const QString &tableName, const QString &columnCondition,
                                                   const QVariant &condition, const QString &operation,
                                                   const QStringList &orderby){
    QSqlQuery retrieveQuery = buildBindedQuery(tableName, QStringList(),
                                               QStringList() << columnCondition,
                                               QList< QVariant >() << condition,
                                               operation, QStringList(),
                                               orderby);
    return executeSelectQuery(retrieveQuery);
}

QList<QVariantList> DBManager::retrieveAllCond(const QString &tableName, const QStringList &columnCondition,
                                                   const QVariantList &condition, const QString &operation,
                                                   const QStringList &orderby){
    QSqlQuery retrieveQuery = buildBindedQuery(tableName, QStringList(),
                                               columnCondition, condition,
                                               operation, QStringList(),
                                               orderby);
    return executeSelectQuery(retrieveQuery);
}

QList<QVariantList> DBManager::retrieveAllCond(const QString &tableName, const QStringList &columnName,
                                                   const QString &columnCondition, const QVariant &condition,
                                                   const QString &operation, const QStringList &groupby,
                                                   const QStringList &orderby){
    QSqlQuery retrieveQuery = buildBindedQuery(tableName, columnName,
                                               QStringList() << columnCondition,
                                               QList< QVariant >() << condition,
                                               operation, groupby,
                                               orderby);
    return executeSelectQuery(retrieveQuery);
}

QList<QVariantList> DBManager::retrieveAllCond(const QString &tableName, const QStringList &columnName,
                                                   const QStringList &columnCondition, const QVariantList &condition,
                                                   const QString &operation, const QStringList &groupby,
                                                   const QStringList &orderby){
    QSqlQuery retrieveQuery = buildBindedQuery(tableName, columnName,
                                               columnCondition, condition,
                                               operation, groupby,
                                               orderby);
    return executeSelectQuery(retrieveQuery);
}

int DBManager::rowsCount(const QString &tableName){

    QSqlQuery countQuery(QSqlDatabase::database(this->connectionName()));
    QString command = buildQuery(tableName);
    countQuery.prepare(command);

    if (countQuery.exec()){
        if (this->hasQuerySize()) return countQuery.size();

        int counter = 0;
        while (countQuery.next()) counter++;
        return counter;
    }
    return -1;
}

int DBManager::rowsCountCond(const QString &tableName, const QString &columnCondition,
                                const QVariant &condition, const QString &operation){
    QSqlQuery countQuery = buildBindedQuery(tableName, QStringList(),
                                            QStringList() << columnCondition,
                                            QVariantList() << condition,
                                            operation);
    if (countQuery.exec()){
        if (this->hasQuerySize()) return countQuery.size();

        int counter = 0;
        while (countQuery.next()) counter++;
        return counter;
    }
    return -1;
}

int DBManager::rowsCountCond(const QString &tableName, const QStringList &columnCondition,
                                const QVariantList &condition, const QString &operation){
    QSqlQuery countQuery = buildBindedQuery(tableName, QStringList(),
                                            columnCondition, condition,
                                            operation);
    if (countQuery.exec()){
        if (this->hasQuerySize()) return countQuery.size();

        int counter = 0;
        while (countQuery.next()) counter++;
        return counter;
    }
    return -1;
}

bool DBManager::clearTable(const QString &tableName){
    if (tableName.isEmpty() || tableName.isNull()) return false;

    QSqlQuery dropQuery(QSqlDatabase::database(this->connectionName()));
    QString command = "DELETE FROM " + this->dbData.tablePrefix() + tableName + ";";
    dropQuery.prepare(command);

    return dropQuery.exec();
}

bool DBManager::dropTable(const QString &tableName){
    if (tableName.isEmpty() || tableName.isNull()) return false;

    QSqlQuery dropQuery(QSqlDatabase::database(this->connectionName()));
    QString command = "DROP TABLE " + this->dbData.tablePrefix() + tableName + ";";
    dropQuery.prepare(command);

    return dropQuery.exec();
}

QSqlQuery DBManager::createCustomQuery(const QString &query){
    if (query.isEmpty() || query.isNull())
        return QSqlQuery(QSqlDatabase::database(this->connectionName()));
    else{
        QSqlQuery newCommand(QSqlDatabase::database(this->connectionName()));
        newCommand.prepare(query);
        return newCommand;
    }
}

QVariant DBManager::pixmapToVariant(const QPixmap &pixmap){
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");

    return QVariant(imageData);
}

QPixmap DBManager::variantToPixmap(const QVariant &variant){
    if (variant.isNull()) return QPixmap();

    QPixmap pixmap;
    if (pixmap.loadFromData(variant.toByteArray())) return pixmap;

    return QPixmap();
}

QString DBManager::getUniqueConnectionName(const QString &partname){
    return partname + QDateTime::currentDateTime().toString("dd.MM.yyyy-hh:mm:ss:zzz");
}

bool DBManager::setConnectionType(DBConnectionType cType){
    if (this->isOpen()) return false;

    DBConnectionType oldType = this->dbData.connectionType();
    this->dbData.setDatabaseConnectionType(cType);
    if (setConnectionName(this->dbData.connectionName())) return true;

    this->dbData.setDatabaseConnectionType(oldType);
    return false;
}

DBManager::DBConnectionType DBManager::connectionType(){
    return this->dbData.connectionType();
}

bool DBManager::setConnectionName(const QString &cName){
    if (this->isOpen()) return false;

    this->dbData.setConnectionName(cName);
    removeInstance();
    getInstance(this->dbData);

    return setDatabaseData(this->dbData);
}

QString DBManager::currentConnectionName(){
    return this->connectionName();
}

bool DBManager::setDatabaseData(const DBManager::DBData &dbData){
    if (this->isOpen()) return false;
    this->dbData = dbData;

    if (!dbData.hostName().isNull() && !dbData.hostName().isEmpty())
        this->setHostName(dbData.hostName());
    if (!dbData.databaseName().isNull() && !dbData.databaseName().isEmpty())
        this->setDatabaseName(this->dbData.databaseName());
    if (!dbData.username().isNull() && !dbData.username().isEmpty())
        this->setUserName(this->dbData.username());
    if (!dbData.password().isNull() && !dbData.password().isEmpty())
        this->setPassword(this->dbData.password());
    if (!dbData.connectOptions().isNull() && !dbData.connectOptions().isEmpty())
        this->setConnectOptions(this->dbData.connectOptions());
    if (dbData.port() > 0)
        this->setPort(this->dbData.port());

    this->setNumericalPrecisionPolicy(this->dbData.numericalPrecisionPolicy());

    return true;
}

DBManager::DBData DBManager::databaseData(){
    this->dbData.setHostName(this->hostName());
    this->dbData.setDatabaseName(this->databaseName());
    this->dbData.setUserName(this->userName());
    this->dbData.setPassword(this->password());
    this->dbData.setConnectOptions(this->connectOptions());
    this->dbData.setPort(this->port());
    this->dbData.setNumericalPrecisionPolicy(this->numericalPrecisionPolicy());
    return this->dbData;
}

bool DBManager::setDBPrefix(const QString &prefix) {
    if (this->isOpen()) return false;
    this->dbData.setTablePrefix(prefix);
    return true;
}

QString DBManager::getConnectionType(DBConnectionType cType){
    switch (cType) {
    case DB2:
        return "QDB2";
    case IBASE:
        return "QIBASE";
    case MYSQL:
        return "QMYSQL";
    case OCI:
        return "QOCI";
    case ODBC:
        return "QODBC";
    case PSQL:
        return "PSQL";
    case SQLITE:
        return "QSQLITE";
    case SQLITE2:
        return "QSQLITE2";
    case TDS:
        return "QTDS";
    default:
        return QString();
    }
}

QString DBManager::buildQuery(const QString &tableName, const QStringList &columnName,
                                  const QStringList &columnCondition, const QVariantList &condition,
                                  const QString &operation, const QStringList &groupby,
                                  const QStringList &orderby){
    if (tableName.isEmpty() || (columnCondition.size() != condition.size()))
        return QString();

    int columnsN = columnName.length() - 1,
        columns = columnCondition.length() - 1,
        groupbySize = groupby.size() - 1,
        orderbySize = orderby.length() - 1;

    QString command = "SELECT ";
    if (columnName.isEmpty())
         command += "*";
    else {
        for (int i = 0; i < columnsN; ++i)
            command += (columnName.at(i) + ", ");
        command += (columnName.at(columnsN));
    }

    command += (" FROM " + this->dbData.tablePrefix() + tableName);

    if (!columnCondition.isEmpty()){
        command += " WHERE ";
        for (int i = 0; i < columns; ++i)
            command += (columnCondition.at(i) + " " + operation + " :" + columnCondition.at(i) + " AND ");
        command += (columnCondition.at(columns) + " " + operation + " :" + columnCondition.at(columns));
    }

    if (!groupby.isEmpty()){
        command += " GROUP BY ";
        for (int i = 0; i < groupbySize; ++i)
            command += (groupby.at(i) + ", ");
        command += (groupby.at(groupbySize));
    }

    if (!orderby.isEmpty()){
        command += " ORDER BY ";
        for (int i = 0; i < orderbySize; ++i)
            command += (orderby.at(i) + ", ");
        command += orderby.at(orderbySize);
    }

    command += ";";

    return command;
}

QSqlQuery DBManager::buildBindedQuery(const QString &tableName, const QStringList &columnName,
                                          const QStringList &columnCondition, const QVariantList &condition,
                                          const QString &operation, const QStringList &groupby,
                                          const QStringList &orderby){
    QString command = buildQuery(tableName, columnName,
                                 columnCondition, condition,
                                 operation, groupby,
                                 orderby);

    if (command.isEmpty()) return QSqlQuery();
    int columns = columnCondition.length();

    QSqlQuery retrieveQuery(QSqlDatabase::database(this->connectionName()));
    retrieveQuery.prepare(command);
    for (int i = 0; i < columns; ++i)
        retrieveQuery.bindValue(":" + columnCondition.at(i), condition.at(i));

    return retrieveQuery;
}

QList<QVariantList> DBManager::executeSelectQuery(QSqlQuery &query){
    QList < QVariantList > results;

    if (query.exec()){
        int records = query.record().count();

        if (this->hasQuerySize()){
            int rows = query.size();
            for (int i = 0; i < rows && query.next(); ++i){
                results << QVariantList();
                for (int j = 0; j < records; ++j)
                    results[i] << query.value(j);
            }
        }
        else {
            for (int i = 0; query.next(); ++i){
                results << QVariantList();
                for (int j = 0; j < records; ++j)
                    results[i] << query.value(j);
            }
        }
    }

    return results;
}
