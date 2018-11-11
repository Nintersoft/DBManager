#include "dbmanager.h"
#include <QGuiApplication>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QFile>
#include <QPixmap>

#define SQLITET

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    DBManager::DBData myData;
#ifdef SQLITET
    myData.setDatabaseName(QDir::homePath() + "/test.db");
    DBManager myDB(myData, "mtt_", DBManager::SQLITE);
#else
    myData.setHostName("127.0.0.1");
    myData.setPort(3306);
    myData.setDatabaseName("test");
    myData.setUserName("root");
    myData.setPassword("");
    DBManager myDB(myData, "mtt_", DBManager::MYSQL);
#endif

#ifdef FORCEMYSQLTEST
    QSqlDatabase myDatab;
    myDatab = QSqlDatabase::addDatabase("QMYSQL");
    myDatab.setHostName("127.0.0.1");
    myDatab.setPort(3306);
    myDatab.setDatabaseName("test");
    myDatab.setUserName("root");
    myDatab.setPassword("");

    qDebug () << myDatab.open();

    QSqlQuery myQuery;
    myQuery.prepare("CREATE TABLE IF NOT EXISTS mysql_test (ID INTEGER(64) UNSIGNED AUTO_INCREMENT PRIMARY KEY, NAME TEXT NOT NULL);");
    qDebug () << myQuery.exec();
    myDatab.close();
    qDebug() << ".: End of forced MYSQL :.\n\n";
#endif

    qDebug() << "Database Manager test.\nDB Status .:";
    qDebug() << "Has valid settings : " << myDB.hasValidSettings();
    qDebug() << "Prefix : " << myDB.dbPrefix();
    qDebug() << "Database name : " << myDB.databaseData().databaseName();
    qDebug() << "Generate random connection name : " << DBManager::getUniqueConnectionName("test");
    qDebug() << "DB current status : " << myDB.isOpen();
    qDebug() << "DB is valid : " << myDB.isValid();
    qDebug() << "DB current connection type : " << myDB.connectionType();
    qDebug() << "DB current connection name : " << myDB.currentConnectionName();

#ifndef SQLITET
    qDebug() << myDB.databaseData().hostName();
    qDebug() << myDB.databaseData().port();
    qDebug() << myDB.databaseData().databaseName();
    qDebug() << myDB.databaseData().username();
    qDebug() << myDB.databaseData().password();
#endif

    qDebug() << "\nOpen DB .:";
    qDebug() << myDB.openDB();
    qDebug() << myDB.isValid();

    qDebug() << "\nCreate table .:";
#ifdef SQLITET
    qDebug() << myDB.createTable("testTable", QStringList() << "ID INTEGER PRIMARY KEY" << "NAME TEXT NOT NULL"
                                                            << "PARENT TEXT NOT NULL" << "AGE INTEGER NOT NULL");
    qDebug() << myDB.createTable("testTableDrop", QStringList() << "ID INTEGER PRIMARY KEY" << "NAME TEXT NOT NULL"
                                                            << "PARENT TEXT NOT NULL" << "AGE INTEGER NOT NULL");
    qDebug() << myDB.createTable("testTableDrop1", QStringList() << "ID INTEGER PRIMARY KEY" << "NAME TEXT NOT NULL"
                                                            << "PARENT TEXT NOT NULL" << "AGE INTEGER NOT NULL");
    qDebug() << myDB.createTable("testImageTable", QStringList() << "ID INTEGER PRIMARY KEY" << "IMAGE BLOB");

#else
    qDebug() << myDB.createTable("testTable" , QStringList() << "ID INTEGER(64) UNSIGNED AUTO_INCREMENT PRIMARY KEY" << "NAME TEXT NOT NULL"
                                                            << "PARENT TEXT NOT NULL" << "AGE INTEGER NOT NULL");
    qDebug() << myDB.createTable("testTableDrop" , QStringList() << "ID INTEGER(64) UNSIGNED AUTO_INCREMENT PRIMARY KEY" << "NAME TEXT NOT NULL"
                                                            << "PARENT TEXT NOT NULL" << "AGE INTEGER NOT NULL");
    qDebug() << myDB.createTable("testTableDrop1" , QStringList() << "ID INTEGER(64) UNSIGNED AUTO_INCREMENT PRIMARY KEY" << "NAME TEXT NOT NULL"
                                                            << "PARENT TEXT NOT NULL" << "AGE INTEGER NOT NULL");
    qDebug() << myDB.createTable("testImageTable" , QStringList() << "ID INTEGER(64) UNSIGNED AUTO_INCREMENT PRIMARY KEY" << "IMAGE BLOB");
#endif

    qDebug() << "\nClear table .:";
    qDebug() << myDB.clearTable("testTable");
    qDebug() << myDB.clearTable("testImageTable");

    qDebug() << "\nInsert row .:";
    qDebug() << myDB.insertRow("testTable", QStringList() << "NAME" << "PARENT" << "AGE",
                                            QList<QVariant>() << "Mauro Mascarenhas" << "Paulo Francisco" << 20);
    qDebug() << myDB.insertRow("testTable", QStringList() << "NAME" << "PARENT" << "AGE",
                                            QList<QVariant>() << "Fábio Francisco" << "Paulo Francisco" << 18);
    qDebug() << "\nUpdate row .:";
    qDebug() << myDB.updateRow("testTable", "NAME", "Mauro Mascarenhas",
                                            QStringList() << "NAME" << "PARENT",
                                            QList<QVariant>() << "Mauro Mascarenhas de Araújo" << "Ana Cláudia Mascarenhas");
    qDebug() << myDB.updateRow("testTable", QStringList() << "NAME" << "PARENT",
                                            QList<QVariant>() << "Fábio Francisco" << "Paulo Francisco",
                                            QStringList() << "NAME" << "AGE",
                                            QList<QVariant>() << "Fábio Francisco de Araújo" << 19);

    qDebug() << "\nRow exists .:";
    qDebug() << myDB.rowExists("testTable", "NAME", "Mauro Mascarenhas de Araújo");
    qDebug() << myDB.rowExists("testTable", "NAME", "Mauro Mascarenhas");

    qDebug() << "\nRow exists[LIST] .:)";
    qDebug() << myDB.rowExists("testTable", QStringList() << "NAME" << "AGE",
                                            QList<QVariant>() << "Fábio Francisco de Araújo" << 19);
    qDebug() << myDB.rowExists("testTable", QStringList() << "NAME" << "AGE",
                                            QList<QVariant>() << "Fábio Francisco de Araújo" << 17);

    qDebug() << "\nRows count .:)";
    qDebug() << myDB.rowsCount("testTable");
    qDebug() << myDB.rowsCountCond("testTable", "NAME", "Mauro Mascarenhas de Araújo");
    qDebug() << myDB.rowsCountCond("testTable", "NAME", "Test");
    qDebug() << myDB.rowsCountCond("testTable", QStringList() << "NAME" << "AGE",
                                                QList<QVariant>() << "Fábio Francisco de Araújo" << 19);

    qDebug() << "\nRetrieve row .:)";
    qDebug() << myDB.retrieveRow("testTable", "NAME", "Mauro Mascarenhas de Araújo");
    qDebug() << myDB.retrieveRow("testTable", QStringList() << "NAME" << "AGE",
                                              QList<QVariant>() << "Fábio Francisco de Araújo" << 19);
    qDebug() << myDB.retrieveRow("testTable", QStringList() << "NAME" << "AGE",
                                              QList<QVariant>() << "Fábio Francisco de Araújo" << 19,
                                              QStringList() << "AGE");

    qDebug() << "\nRetrieve all rows .:)";
    qDebug() << myDB.retrieveAll("testTable");
    qDebug() << myDB.retrieveAll("testTable", QStringList() << "NAME" << "PARENT");

    qDebug() << "\nRetrieve all rows [COND] .:";
    qDebug() << myDB.retrieveAllCond("testTable", "NAME", "%Fábio%", "LIKE");
    qDebug() << myDB.retrieveAllCond("testTable", QStringList() << "NAME" << "AGE",
                                                  QList<QVariant>() << "Fábio Francisco de Araújo" << 19);
    qDebug() << myDB.retrieveAllCond("testTable", QStringList() << "NAME" << "AGE",
                                                  QStringList() << "NAME" << "AGE",
                                                  QList<QVariant>() << "Fábio Francisco de Araújo" << 19);
    qDebug() << myDB.retrieveAllCond("testTable", QStringList() << "NAME" << "AGE",
                                                  "AGE", 10, ">");

    qDebug() << "\nRemove rows .:";
    qDebug() << myDB.removeRow("testTable", "NAME", "Mauro Mascarenhas");
    qDebug() << myDB.removeRow("testTable", QStringList() << "NAME" << "AGE",
                                            QList<QVariant>() << "Fábio Francisco de Araújo" << 19);

    qDebug() << "\nDrop table .:";
    qDebug() << myDB.dropTable("testTableDrop");

    qDebug() << "\nLast error .:";
    qDebug() << myDB.lastError();

    qDebug() << "\nCustom query .:";
    QSqlQuery testC = myDB.runCustomQuery();
    testC.prepare("DROP TABLE mtt_testTableDrop1");
    qDebug() << testC.exec();
    qDebug() << testC.exec();

    qDebug() << "\nQPixmap to QVariant .:";
    QPixmap randomImage;
    qDebug() << randomImage.load(QDir::homePath() + "/Pictures/Nintersoft/ns-logo.png");
    QVariant imageVariant = myDB.pixmapToVariant(randomImage);
    qDebug() << myDB.insertRow("testImageTable", QStringList() << "IMAGE", QList<QVariant>() << imageVariant);

    QPixmap retrievedImage = myDB.variantToPixmap(myDB.retrieveRow("testImageTable", QStringList() << "ID", QList<QVariant>() << 1, QStringList() << "IMAGE").at(0));
    qDebug() << (retrievedImage.toImage() == randomImage.toImage());

    qDebug() << "\nSetters .:";
    qDebug() << myDB.setDatabaseData(myData);
    qDebug() << myDB.setDBPrefix("lol_");
    qDebug() << myDB.setConnectionName(DBManager::getUniqueConnectionName("test_"));
    qDebug() << myDB.setConnectionType(DBManager::MYSQL);

    qDebug() << "\n ---- END OF DBMANAGER TEST ----\n";

    return EXIT_SUCCESS;
}
