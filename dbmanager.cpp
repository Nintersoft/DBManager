/*-------------------------------------------------
#
# Project developed by Nintersoft team
# Developer: Mauro Mascarenhas de Araújo
# Contact: mauro.mascarenhas@nintersoft.com
# License: Nintersoft Open Source Code Licence
# Date: 06 of June of 2018
#
------------------------------------------------- */

#include "dbmanager.h"

DBManager::DBManager(const DBManager::DBData &data, const QString &tablePrefix, DBConnectionType connectionType,
                     const QString &connectionName){
    this->setDBPrefix(tablePrefix);
    this->setConnectionTypeB(connectionType);
    this->setConnectionNameB(connectionName);
    this->setDatabaseData(data);
}

DBManager::DBManager(const QStringList &data,  DBConnectionType connectionType,
                     const QString &connectionName){
    validSettings = false;

    DBManager::DBData dbData;
    dbData.setHostName(data.at(0));
    dbData.setDatabaseName(data.at(1));
    dbData.setUserName(data.at(2));
    dbData.setPassword(data.at(3));
    dbData.setPort(data.at(4).toInt());
    dbData.setNumericalPrecisionPolicy((QSql::NumericalPrecisionPolicy)data.at(5).toInt());
    dbData.setConnectOptions(data.at(6));

    this->setDBPrefix(data.at(7));
    this->setConnectionTypeB(connectionType);
    this->setConnectionNameB(connectionName);
    this->setDatabaseData(dbData);
}

DBManager::~DBManager(){
    QString connectionName =  database.connectionName();
    if (database.isOpen()) database.close();
    database = QSqlDatabase();
    database.removeDatabase(connectionName);
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

QSqlQuery DBManager::runCustomQuery(){
    return QSqlQuery(QSqlDatabase::database(database.connectionName()));
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
    if (database.isOpen()) return false;
    DBConnectionType oldType = this->currentType;
    this->currentType = cType;
    if (setConnectionName(database.connectionName())) return true;

    this->currentType = oldType;
    return false;
}

DBManager::DBConnectionType DBManager::connectionType(){
    return this->currentType;
}

void DBManager::setConnectionTypeB(DBConnectionType cType){
    this->currentType = cType;
}

bool DBManager::setConnectionName(const QString &cName){
    if (database.isOpen()) return false;

    QSqlDatabase::removeDatabase(database.connectionName());
    if (currentType == DBManager::MYSQL){
        if (cName == "default") database = QSqlDatabase::addDatabase("QMYSQL");
        else database = QSqlDatabase::addDatabase("QMYSQL", cName);
    }
    else {
        if (cName == "default") database = QSqlDatabase::addDatabase("QSQLITE");
        else database = QSqlDatabase::addDatabase("QSQLITE", cName);
    }
    return setDatabaseData(this->dbData);
}

QString DBManager::currentConnectionName(){
    return database.connectionName();
}

void DBManager::setConnectionNameB(const QString &cName){
    if (currentType == DBManager::MYSQL){
        if (cName == "default") database = QSqlDatabase::addDatabase("QMYSQL");
        else database = QSqlDatabase::addDatabase("QMYSQL", cName);
    }
    else {
        if (cName == "default") database = QSqlDatabase::addDatabase("QSQLITE");
        else database = QSqlDatabase::addDatabase("QSQLITE", cName);
    }
}

bool DBManager::setDatabaseData(const DBManager::DBData &dbData){
    if (database.isOpen()) return false;
    this->dbData = dbData;

    if (currentType == DBManager::MYSQL){
        database.setHostName(dbData.hostName());
        database.setDatabaseName(dbData.databaseName());
        database.setUserName(dbData.username());
        database.setPassword(dbData.password());
        database.setPort(dbData.port());
        database.setNumericalPrecisionPolicy(dbData.numericalPrecisionPolicy());
        database.setConnectOptions(dbData.connectionOptions());

        validSettings = !(dbData.hostName().isNull() || dbData.hostName().isEmpty()
                            || dbData.databaseName().isNull() || dbData.databaseName().isEmpty()
                            || dbData.username().isNull() || dbData.username().isEmpty()
                            || dbData.password().isNull() || dbData.password().isEmpty());
    }
    else {
        database.setDatabaseName(dbData.databaseName());
        database.setNumericalPrecisionPolicy(dbData.numericalPrecisionPolicy());
        database.setConnectOptions(dbData.connectionOptions());
        validSettings = !(dbData.databaseName().isNull() || dbData.databaseName().isEmpty());
    }

    return true;
}

const DBManager::DBData DBManager::databaseData(){
    return this->dbData;
}

bool DBManager::setDBPrefix(const QString &prefix) {
    if (database.isOpen()) return false;
    this->prefix = prefix;
    return true;
}

bool DBManager::openDB(){
    return database.open();
}

bool DBManager::isOpen(){
    return database.isOpen();
}

bool DBManager::isValid(){
    return database.isValid();
}

void DBManager::closeDB(){
    database.close();
}

QSqlError DBManager::lastError(){
    return database.lastError();
}

bool DBManager::hasValidSettings(){
    return this->validSettings;
}

/*
 *  DBData Methods
 *  Every DBData Method became inline methods
 *  Hence, no implementation here is necessary
 */
