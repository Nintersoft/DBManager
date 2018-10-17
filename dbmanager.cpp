/*-------------------------------------------------
#
# Project developed by Nintersoft team
# Developer: Mauro Mascarenhas de Araújo
# Contact: mauro.mascarenhas@nintersoft.com
# License: Nintersoft Open Source Code Licence
# Date: 16 of October of 2018
#
------------------------------------------------- */

#include "dbmanager.h"

DBManager::DBManager(const DBManager::DBData &data) :
    QSqlDatabase(){
    this->setDBPrefix(data.tablePrefix());
    if (data.connectionName().isEmpty() || data.connectionName() == "default")
        QSqlDatabase::addDatabase(getConnectionType(data.connectionType()));
    else QSqlDatabase::addDatabase(getConnectionType(data.connectionType()), data.connectionName());
    this->setDatabaseData(data);
}

DBManager::~DBManager(){
    if (this->isOpen()) this->close();
    this->removeDatabase(dbData.connectionName());
}

DBManager& DBManager::getInstance(const DBData &data){
    if (!currentInstance){
        if (data.databaseName().isNull() || data.connectionType() == UNDEFINED)
            throw std::invalid_argument;

        currentInstance = new DBManager(dbData);
    }
    return *currentInstance;
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
    if (!database.isOpen() || tableName.isEmpty() || tableName.isNull()
            || columns.isEmpty()) return false;

    QString command = "CREATE TABLE IF NOT EXISTS " + prefix + tableName + "(";
    for (int i = 0; i < columns.length() - 1; ++i)
        command += columns.at(i) + ", ";

    command += (columns.at(columns.length() - 1) + ");");

    QSqlQuery query(QSqlDatabase::database(database.connectionName()));
    query.prepare(command);
    return query.exec();
}

bool DBManager::insertRow(const QString &tableName, const QStringList &columnName, const QList<QVariant> &data){
    if (tableName.isEmpty() || tableName.isNull() || columnName.isEmpty()
            || data.isEmpty() || data.size() != columnName.size()) return false;

    QString command = "INSERT INTO " + prefix + tableName + " (";
    int columns = columnName.length();
    for (int i = 0; i < columns - 1; ++i)
        command += (columnName.at(i) + ", ");

    command += (columnName.at(columns - 1) + ") VALUES (");
    for (int i = 0; i < columns - 1; ++i)
        command += (":" + columnName.at(i) + ", ");

    command += (":" + columnName.at(columns - 1) + ");");

    QSqlQuery queryAdd(QSqlDatabase::database(database.connectionName()));
    queryAdd.prepare(command);

    for (int i = 0; i < columns; ++i)
        queryAdd.bindValue(":" + columnName.at(i), data.at(i));

    return queryAdd.exec();
}

bool DBManager::updateRow(const QString &tableName, const QString &columnNameCond,
                            const QVariant &condition, const QStringList &columnName,
                            const QList<QVariant> &data, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull() || columnNameCond.isEmpty()
            || columnNameCond.isNull() || columnName.isEmpty() || columnName.size() != data.size()) return false;

    QString command;
    int columns = columnName.length();

    if (currentType == SQLITE){
        command = "UPDATE " + prefix + tableName + " SET (";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnName.at(i) + ", ");

        command += (columnName.at(columns - 1) + ") = (");
        for (int i = 0; i < columns - 1; ++i)
            command += (":" + columnName.at(i) + ", ");

        command += (":" + columnName.at(columns - 1) + ") WHERE (" +
                        columnNameCond + ") " + operation + " (:" + columnNameCond + "c);");
    }
    else {
        command = "UPDATE " + prefix + tableName + " SET ";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnName.at(i) + " = :" + columnName.at(i) + ", ");

        command += (columnName.at(columns - 1) + " = :" + columnName.at(columns - 1) + " WHERE " + columnNameCond + " = :" + columnNameCond + "c;");
    }

    QSqlQuery queryAdd(QSqlDatabase::database(database.connectionName()));
    queryAdd.prepare(command);
    queryAdd.bindValue(":" + columnNameCond + "c", condition);
    for (int i = 0; i < columns; ++i)
        queryAdd.bindValue(":" + columnName.at(i), data.at(i));

    return queryAdd.exec();
}

bool DBManager::updateRow(const QString &tableName, const QStringList &columnNameCond,
                            const QList<QVariant> &condition, const QStringList &columnName,
                            const QList<QVariant> &data, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull() || condition.isEmpty()
            || columnNameCond.isEmpty() || columnName.isEmpty() || data.isEmpty()
            || columnNameCond.size() != condition.size() || columnName.size() != data.size()) return false;

    QString command;
    int columns = columnName.length();
    int columnsS = columnNameCond.length();

    if (currentType == SQLITE){
        command = "UPDATE " + prefix + tableName + " SET (";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnName.at(i) + ", ");

        command += (columnName.at(columns - 1) + ") = (");
        for (int i = 0; i < columns - 1; ++i)
            command += (":" + columnName.at(i) + ", ");

        command += (":" + columnName.at(columns - 1) + ") WHERE (");

        for (int i = 0; i < columnsS - 1; ++i)
            command += (columnNameCond.at(i) + ", ");

        command += (columnNameCond.at(columnsS - 1) + ") " + operation + " (");
        for (int i = 0; i < columnsS - 1; ++i)
            command += (":" + columnNameCond.at(i) + "c, ");

        command += (":" + columnNameCond.at(columnsS - 1) + "c);");
    }
    else {
        command = "UPDATE " + prefix + tableName + " SET ";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnName.at(i) + " = :" + columnName.at(i) + ", ");

        command += (columnName.at(columns - 1) + " = :" + columnName.at(columns - 1) + " WHERE ");
        for (int i = 0; i < columnsS - 1; ++i)
            command += (columnNameCond.at(i) + " " + operation + " :" + columnNameCond.at(i) + "c AND ");

        command += (columnNameCond.at(columnsS - 1) + " " + operation + " :" + columnNameCond.at(columnsS - 1) + "c;");
    }

    QSqlQuery queryUpdate(QSqlDatabase::database(database.connectionName()));
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

    QSqlQuery removeQuery(QSqlDatabase::database(database.connectionName()));
    QString command;

    if (currentType == SQLITE) command = "DELETE FROM " + prefix + tableName + " WHERE (" + columnNameCond + ") "+ operation + " (:" + columnNameCond + ");";
    else command = "DELETE FROM " + prefix + tableName + " WHERE " + columnNameCond + " " + operation + " :" + columnNameCond + ";";

    removeQuery.prepare(command);
    removeQuery.bindValue(":" + columnNameCond, condition);

    return removeQuery.exec();
}

bool DBManager::removeRow(const QString &tableName, const QStringList &columnNameCond,
                          const QList<QVariant> &condition, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull()
            || condition.isEmpty() || columnNameCond.isEmpty()
            || columnNameCond.size() != condition.size()) return false;

    if (!rowExists(tableName, columnNameCond, condition, operation)) return false;

    QSqlQuery removeQuery(QSqlDatabase::database(database.connectionName()));
    QString command;
    int columns = columnNameCond.length();

    if (currentType == SQLITE){
        command = "DELETE FROM " + prefix + tableName + " WHERE (";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnNameCond.at(i) + ", ");

        command += (columnNameCond.at(columns - 1) + ") " + operation + " (");
        for (int i = 0; i < columns - 1; ++i)
            command += (":" + columnNameCond.at(i) + ", ");

        command += (":" + columnNameCond.at(columns - 1) + ");");
    }
    else {
        command = "DELETE FROM " + prefix + tableName + " WHERE ";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnNameCond.at(i) + " " + operation + " :" + columnNameCond.at(i) + " AND ");

        command += (columnNameCond.at(columns - 1) + " " + operation + " :" + columnNameCond.at(columns - 1) + ";");
    }

    removeQuery.prepare(command);
    for (int i = 0; i < columns; ++i)
        removeQuery.bindValue((":" + columnNameCond.at(i)), condition.at(i));

    return removeQuery.exec();

}

bool DBManager::rowExists(const QString &tableName, const QString &columnNameCond,
                          const QVariant &data, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull() || data.isNull()
            || columnNameCond.isEmpty() || columnNameCond.isNull()) return false;

    QSqlQuery checkQuery(QSqlDatabase::database(database.connectionName()));

    QString command;
    if (currentType == SQLITE) command = "SELECT " + columnNameCond + " FROM " + prefix + tableName + " WHERE (" + columnNameCond + ") " + operation + " (:" + columnNameCond + ");";
    else command = "SELECT " + columnNameCond + " FROM " + prefix + tableName + " WHERE " + columnNameCond + " " + operation + " :" + columnNameCond + ";";

    checkQuery.prepare(command);
    checkQuery.bindValue(":" + columnNameCond, data);

    bool exists = false;
    if (checkQuery.exec())
        exists = checkQuery.next();

    return exists;
}

bool DBManager::rowExists(const QString &tableName, const QStringList &columnNameCond,
                          const QList<QVariant> &data, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull() || data.isEmpty()
            || columnNameCond.isEmpty() || columnNameCond.size() != data.size()) return false;

    QString command;
    int columns = columnNameCond.length();

    if (currentType == SQLITE){
        command = "SELECT * FROM " + prefix + tableName + " WHERE (";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnNameCond.at(i) + ", ");

        command += (columnNameCond.at(columns - 1) + ") " + operation + " (");
        for (int i = 0; i < columns - 1; ++i)
            command += (":" + columnNameCond.at(i) + ", ");

        command += (":" + columnNameCond.at(columns - 1) + ");");
    }
    else {
        command = "SELECT * FROM " + prefix + tableName + " WHERE ";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnNameCond.at(i) + " " + operation + " :" + columnNameCond.at(i) + " AND ");

        command += (columnNameCond.at(columns - 1) + " " + operation + " :" + columnNameCond.at(columns - 1) + ";");
    }

    QSqlQuery checkQuery(QSqlDatabase::database(database.connectionName()));
    checkQuery.prepare(command);
    for (int i = 0; i < columns; ++i)
        checkQuery.bindValue(":" + columnNameCond.at(i), data.at(i));

    bool exists = false;
    if (checkQuery.exec())
        exists = checkQuery.next();
    return exists;
}

QList<QVariant> DBManager::retrieveRow(const QString &tableName, const QString &columnNameCond,
                                        const QVariant &condition, const QString &operation){
    QList<QVariant> retrievedData;

    QSqlQuery retrieveQuery(QSqlDatabase::database(database.connectionName()));

    QString command;
    if (currentType == SQLITE) command = "SELECT * FROM " + prefix + tableName + " WHERE (" + columnNameCond + ") " + operation + " (:" + columnNameCond + ");";
    else command = "SELECT * FROM " + prefix + tableName + " WHERE " + columnNameCond + " " + operation + " :" + columnNameCond + ";";

    retrieveQuery.prepare(command);
    retrieveQuery.bindValue(":" + columnNameCond, condition);

    if (retrieveQuery.exec()){
        if (retrieveQuery.next()){
            int count = 0;
            while (retrieveQuery.value(count).isValid()){
                retrievedData << retrieveQuery.value(count);
                ++count;
            }
        }
    }

    return retrievedData;
}

QList<QVariant> DBManager::retrieveRow(const QString &tableName, const QStringList &columnNameCond,
                                        const QList<QVariant> &condition, const QString &operation){

    QString command;
    int columns = columnNameCond.length();

    if (currentType == SQLITE){
        command = "SELECT * FROM " + prefix + tableName + " WHERE (";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnNameCond.at(i) + ", ");

        command += (columnNameCond.at(columns - 1) + ") "+ operation + " (");
        for (int i = 0; i < columns - 1; ++i)
            command += (":" + columnNameCond.at(i) + ", ");

        command += (":" + columnNameCond.at(columns - 1) + ");");
    }
    else {
        command = "SELECT * FROM " + prefix + tableName + " WHERE ";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnNameCond.at(i) + " " + operation + " :" + columnNameCond.at(i) + " AND ");

        command += (columnNameCond.at(columns - 1) + " " + operation + " :" + columnNameCond.at(columns - 1) + ";");
    }

    QSqlQuery retrieveQuery(QSqlDatabase::database(database.connectionName()));
    retrieveQuery.prepare(command);
    for (int i = 0; i < columns; ++i)
        retrieveQuery.bindValue(":" + columnNameCond.at(i), condition.at(i));

    QList<QVariant> retrievedData;

    if (retrieveQuery.exec()){
        if (retrieveQuery.next()){
            int count = 0;
            while (retrieveQuery.value(count).isValid()){
                retrievedData << retrieveQuery.value(count);
                ++count;
            }
        }
    }

    return retrievedData;
}

QList<QVariant> DBManager::retrieveRow(const QString &tableName, const QStringList &columnNameCond,
                                        const QList<QVariant> &condition, QStringList columnName, const QString &operation){
    int columnsc = columnName.length();
    int columns = columnNameCond.length();
    QString command = "SELECT ";

    for(int i = 0; i < columnsc - 1; ++i)
        command += (columnName.at(i) + ", ");

    command += (columnName.at(columnsc - 1) + " FROM " + prefix + tableName + " WHERE ");

    if (currentType == SQLITE){
        command += "(";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnNameCond.at(i) + ", ");

        command += (columnNameCond.at(columns - 1) + ") "+ operation + " (");
        for (int i = 0; i < columns - 1; ++i)
            command += (":" + columnNameCond.at(i) + ", ");

        command += (":" + columnNameCond.at(columns - 1) + ");");
    }
    else {
        for (int i = 0; i < columns - 1; ++i)
            command += (columnNameCond.at(i) + " " + operation + " :" + columnNameCond.at(i) + " AND ");

        command += (columnNameCond.at(columns - 1) + " " + operation + " :" + columnNameCond.at(columns - 1) + ";");
    }

    QSqlQuery retrieveQuery(QSqlDatabase::database(database.connectionName()));
    retrieveQuery.prepare(command);
    for (int i = 0; i < columns; ++i)
        retrieveQuery.bindValue(":" + columnNameCond.at(i), condition.at(i));

    QList<QVariant> retrievedData;

    if (retrieveQuery.exec()){
        if (retrieveQuery.next()){
            int count = 0;
            while (retrieveQuery.value(count).isValid()){
                retrievedData << retrieveQuery.value(count);
                ++count;
            }
        }
    }

    return retrievedData;
}

QList<QList<QVariant> > DBManager::retrieveAll(const QString &tableName){
    QSqlQuery retrieveQuery(QSqlDatabase::database(database.connectionName()));
    QString command = "SELECT * FROM " + prefix + tableName + ";";
    retrieveQuery.prepare(command);

    QList<QList<QVariant> > results;

    if (retrieveQuery.exec()){
        int currentRow = 0;
        while(retrieveQuery.next()){
            results << QList<QVariant>();
            int count = 0;
            while (retrieveQuery.value(count).isValid()){
                results[currentRow] << retrieveQuery.value(count);
                count++;
            }
            currentRow++;
        }
    }

    return results;
}

QList<QList<QVariant> > DBManager::retrieveAll(const QString &tableName, const QStringList &columns){
    QSqlQuery retrieveQuery(QSqlDatabase::database(database.connectionName()));

    QString command = "SELECT ";
    for (int i = 0; i < columns.length() - 1; ++ i)
        command += (columns.at(i) + ", ");

    command += (columns.at(columns.length() - 1) + " FROM " + prefix + tableName + ";");
    retrieveQuery.prepare(command);

    QList<QList<QVariant> > results;

    if (retrieveQuery.exec()){
        int currentRow = 0;
        while(retrieveQuery.next()){
            results << QList<QVariant>();
            for (int i = 0; i < columns.length(); ++i)
                results[currentRow] << retrieveQuery.value(i);
            currentRow++;
        }
    }

    return results;
}

QList<QList<QVariant> > DBManager::retrieveAllCond(const QString &tableName, const QString &columnCondition,
                                                    const QVariant &condition, const QString &operation){
    QSqlQuery retrieveQuery(QSqlDatabase::database(database.connectionName()));

    QString command;
    if (currentType == SQLITE) command = "SELECT * FROM " + prefix + tableName + " WHERE (" + columnCondition + ") " + operation + " (:" + columnCondition + ");";
    else command = "SELECT * FROM " + prefix + tableName + " WHERE " + columnCondition + " " + operation + " :" + columnCondition + ";";

    retrieveQuery.prepare(command);
    retrieveQuery.bindValue(":" + columnCondition, condition);

    QList<QList<QVariant> > results;

    if (retrieveQuery.exec()){
        int currentRow = 0;
        while(retrieveQuery.next()){
            results << QList<QVariant>();
            int count = 0;
            while (retrieveQuery.value(count).isValid()){
                results[currentRow] << retrieveQuery.value(count);
                count++;
            }
            currentRow++;
        }
    }

    return results;
}

QList<QList<QVariant> > DBManager::retrieveAllCond(const QString &tableName, const QStringList &columnCondition,
                                                    const QList<QVariant> &condition, const QString &operation){
    QSqlQuery retrieveQuery(QSqlDatabase::database(database.connectionName()));

    QString command;
    int columns = columnCondition.length();

    if (currentType == SQLITE){
        command = "SELECT * FROM " + prefix + tableName + " WHERE (";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnCondition.at(i) + ", ");

        command += (columnCondition.at(columns - 1) + ") "+ operation + " (");
        for (int i = 0; i < columns - 1; ++i)
            command += (":" + columnCondition.at(i) + ", ");

        command += (":" + columnCondition.at(columns - 1) + ");");
    }
    else {
        command = "SELECT * FROM " + prefix + tableName + " WHERE ";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnCondition.at(i) + " " + operation + " :" + columnCondition.at(i) + " AND ");

        command += (columnCondition.at(columns - 1) + " " + operation + " :" + columnCondition.at(columns - 1) + ";");
    }

    retrieveQuery.prepare(command);
    for (int i = 0; i < columnCondition.length(); ++i)
        retrieveQuery.bindValue(":" + columnCondition.at(i), QVariant(condition.at(i)));

    QList<QList<QVariant> > results;

    if (retrieveQuery.exec()){
        int currentRow = 0;
        while(retrieveQuery.next()){
            results << QList<QVariant>();
            int count = 0;
            while (retrieveQuery.value(count).isValid()){
                results[currentRow] << retrieveQuery.value(count);
                count++;
            }
            currentRow++;
        }
    }

    return results;
}

QList<QList<QVariant> > DBManager::retrieveAllCond(const QString &tableName, const QStringList &columnName,
                                                    const QString &columnCondition, const QVariant &condition,
                                                    const QString &operation){
    QSqlQuery retrieveQuery(QSqlDatabase::database(database.connectionName()));
    int columns = columnName.length();
    QString command = "SELECT ";
    for (int i = 0; i < columns - 1; ++i)
        command += (columnName.at(i) + ", ");

    command += (columnName.at(columns - 1) + " FROM " + prefix + tableName + " WHERE ");
    if (currentType == SQLITE) command += ("(" + columnCondition + ") " + operation + " (:" + columnCondition + ");");
    else command += (columnCondition + " " + operation + " :" + columnCondition + ";");

    retrieveQuery.prepare(command);
    retrieveQuery.bindValue(":" + columnCondition, condition);

    QList<QList<QVariant> > results;

    if (retrieveQuery.exec()){
        int currentRow = 0;
        while(retrieveQuery.next()){
            results << QList<QVariant>();
            int count = 0;
            while (retrieveQuery.value(count).isValid()){
                results[currentRow] << retrieveQuery.value(count);
                count++;
            }
            currentRow++;
        }
    }

    return results;
}

QList<QList<QVariant> > DBManager::retrieveAllCond(const QString &tableName, const QStringList &columnName,
                                                    const QStringList &columnCondition, const QList<QVariant> &condition,
                                                    const QString &operation){
    QSqlQuery retrieveQuery(QSqlDatabase::database(database.connectionName()));
    int columnsN = columnName.length();
    int columns = columnCondition.length();

    QString command = "SELECT ";
    for (int i = 0; i < columnsN - 1; ++i)
        command += (columnName.at(i) + ", ");

    command += (columnName.at(columnsN - 1) + " FROM " + prefix + tableName + " WHERE ");

    if (currentType == SQLITE){
        command += "(";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnCondition.at(i) + ", ");

        command += (columnCondition.at(columns - 1) + ") "+ operation + " (");
        for (int i = 0; i < columns - 1; ++i)
            command += (":" + columnCondition.at(i) + ", ");

        command += (columnCondition.at(columns - 1) + ");");
    }
    else{
        for (int i = 0; i < columns - 1; ++i)
            command += (columnCondition.at(i) + " " + operation + " :" + columnCondition.at(i) + " AND ");

        command += (columnCondition.at(columns - 1) + " " + operation + " :" + columnCondition.at(columns - 1) + ";");
    }

    retrieveQuery.prepare(command);
    for (int i = 0; i < columns; ++i)
        retrieveQuery.bindValue(":" + columnCondition.at(i), condition.at(i));


    QList<QList<QVariant> > results;

    if (retrieveQuery.exec()){
        int currentRow = 0;
        while(retrieveQuery.next()){
            results << QList<QVariant>();
            int count = 0;
            while (retrieveQuery.value(count).isValid()){
                results[currentRow] << retrieveQuery.value(count);
                count++;
            }
            currentRow++;
        }
    }

    return results;
}

int DBManager::rowsCount(const QString &tableName){
    if (tableName.isEmpty() || tableName.isNull()) return -1;

    QSqlQuery countQuery(QSqlDatabase::database(database.connectionName()));
    QString command = "SELECT * FROM " + prefix + tableName;
    countQuery.prepare(command);

    if (database.driver()->hasFeature(QSqlDriver::QuerySize)){
        if (countQuery.exec()) return countQuery.size();
        return -1;
    }

    int counter = 0;
    if (countQuery.exec()){
        while (countQuery.next()) counter++;
        return counter;
    }
    return -1;
}

int DBManager::rowsCountCond(const QString &tableName, const QString &columnCondition,
                                const QVariant &condition, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull() || columnCondition.isEmpty() || columnCondition.isNull()
            || condition.isNull()) return -1;

    QSqlQuery countQuery(QSqlDatabase::database(database.connectionName()));

    QString command;
    if (currentType == SQLITE) command = "SELECT * FROM " + prefix + tableName + " WHERE (" + columnCondition + ") " + operation + " (:" + columnCondition + ");";
    else command = "SELECT * FROM " + prefix + tableName + " WHERE " + columnCondition + " " + operation + " :" + columnCondition + ";";

    countQuery.prepare(command);
    countQuery.bindValue(":" + columnCondition, condition);

    if (database.driver()->hasFeature(QSqlDriver::QuerySize)){
        if (countQuery.exec()) return countQuery.size();
        return -1;
    }

    int counter = 0;
    if (countQuery.exec()){
        while (countQuery.next()) counter++;
        return counter;
    }
    return -1;
}

int DBManager::rowsCountCond(const QString &tableName, const QStringList &columnCondition,
                                const QList<QVariant> &condition, const QString &operation){
    if (tableName.isEmpty() || tableName.isNull() || columnCondition.isEmpty()
            || condition.isEmpty() || columnCondition.size() != condition.size()) return -1;

    QSqlQuery countQuery(QSqlDatabase::database(database.connectionName()));

    QString command;
    int columns = columnCondition.length();

    if (currentType == SQLITE){
        command = "SELECT * FROM " + prefix + tableName  + " WHERE (";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnCondition.at(i) + ", ");

        command += (columnCondition.at(columns - 1) + ") "+ operation + " (");
        for (int i = 0; i < columns - 1; ++i)
            command += (":" + columnCondition.at(i) + ", ");

        command += (":" + columnCondition.at(columns - 1) + ");");
    }
    else {
        command = "SELECT * FROM " + prefix + tableName + " WHERE ";
        for (int i = 0; i < columns - 1; ++i)
            command += (columnCondition.at(i) + " " + operation + " :" + columnCondition.at(i) + " AND ");

        command += (columnCondition.at(columns - 1) + " " + operation + " :" + columnCondition.at(columns - 1) + ";");
    }

    countQuery.prepare(command);
    for (int i = 0; i < columns; ++i)
        countQuery.bindValue(":" + columnCondition.at(i), condition.at(i));

    if (database.driver()->hasFeature(QSqlDriver::QuerySize)){
        if (countQuery.exec()) return countQuery.size();
        return -1;
    }

    int counter = 0;
    if (countQuery.exec()){
        while (countQuery.next()) counter++;
        return counter;
    }
    return -1;
}

bool DBManager::clearTable(const QString &tableName){
    if (tableName.isEmpty() || tableName.isNull()) return false;

    QSqlQuery dropQuery(QSqlDatabase::database(database.connectionName()));
    QString command = "DELETE FROM " + prefix + tableName + ";";
    dropQuery.prepare(command);

    return dropQuery.exec();
}

bool DBManager::dropTable(const QString &tableName){
    if (tableName.isEmpty() || tableName.isNull()) return false;

    QSqlQuery dropQuery(QSqlDatabase::database(database.connectionName()));
    QString command = "DROP TABLE " + prefix + tableName + ";";
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

    QSqlDatabase::removeDatabase(this->dbData.databaseName());
    if (cName.isEmpty() || cName == "default")
        QSqlDatabase::addDatabase(getConnectionType(this->dbData.connectionType()));
    else QSqlDatabase::addDatabase(getConnectionType(this->dbData.connectionType()), cName);
    this->dbData.setConnectionName(cName);

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
    if (!dbData.connectionOptions().isNull() && !dbData.connectionOptions().isEmpty())
        this->setDatabaseName(this->dbData.connectionOptions());
    if (dbData.port() > 0)
        this->setPort(this->dbData.port());

    this->setNumericalPrecisionPolicy(this->dbData.numericalPrecisionPolicy());

    return true;
}

DBManager::DBData DBManager::databaseData(){
    //Now it has to retrieve the data from QSqlDatabase, build a DBData and return it
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
