#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

enum ReturnSuccess {GoodCreateTable, BadCreateTable, BadInsertion, GoodInsertion, NoSuchTable, RowIdQueryFailed = -2, NoEntry = -1, SameEntryExists};

struct Person
{
    QString name, surname, group;
};

class DataBase
{
public:
    DataBase(const QString& dbName);
    ~DataBase();
//    class Query : public QSqlQuery
//    {
//    public:
//        Query(QSqlDatabase& db, const QString& query) : QSqlQuery("SELECT * FROM " + query, db), db(db) {}
//        Query() : QSqlQuery(db), db(db) {}
//        void selectAllFromTable(const std::string& table)
//        {
//        }
//    private:
//        QSqlDatabase& db;
//    };
    QSqlDatabase getDataBase() const;
    ReturnSuccess createTable(const QString &table_creation_query);
    ReturnSuccess insertValue(const QString& table, const QStringList& values);
    int calculate_current_id_of_table(const QString& table) const;
private:

    QSqlDatabase db;
};

//static bool createConnection()
//{
//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName(":memory:");
//    if (!db.open()) {
//        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
//            QObject::tr("Unable to establish a database connection.\n"
//                        "This example needs SQLite support. Please read "
//                        "the Qt SQL driver documentation for information how "
//                        "to build it.\n\n"
//                        "Click Cancel to exit."), QMessageBox::Cancel);
//        return false;
//    }

//    QSqlQuery query;

//	query.exec("create table artists (id int primary key, artist varchar(40), albumcount int)");

//    query.exec("insert into artists values(0, '<all>', 0)");
//    query.exec("insert into artists values(1, 'Ane Brun', 2)");
//    query.exec("insert into artists values(2, 'Thomas Dybdahl', 3)");
//    query.exec("insert into artists values(3, 'Kaizers Orchestra', 3)");

//	query.exec("create table albums (albumid int primary key, title varchar(50), artistid int, year int)");

//	query.exec("insert into albums values(1, 'Spending Time With Morgan', 1, 2003)");
//    query.exec("insert into albums values(2, 'A Temporary Dive', 1, 2005)");
//	query.exec("insert into albums values(3, '...The Great October Sound', 2, 2002)");
//    query.exec("insert into albums values(4, 'Stray Dogs', 2, 2003)");
//	query.exec("insert into albums values(5, 'One day you`ll dance for me, New York City', 2, 2004)");
//    query.exec("insert into albums values(6, 'Ompa Til Du D\xc3\xb8r', 3, 2001)");
//    query.exec("insert into albums values(7, 'Evig Pint', 3, 2002)");
//    query.exec("insert into albums values(8, 'Maestro', 3, 2005)");

//    return true;
//}

#endif


