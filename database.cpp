#include "database.h"

DataBase::DataBase(const QString &dbName) : db(QSqlDatabase::addDatabase("QSQLITE"))
{
    db.setDatabaseName(dbName);
    if (!db.open())
    {
        qCritical() << "Failed to open database:" << db.lastError().text();
        return;												// TODO: constructor'in icinde return etmek dogru mu?
    }
}

DataBase::~DataBase()
{
    db.close();
}

QSqlDatabase DataBase::getDataBase() const
{
    return db;
}

//ReturnSuccess DataBase::createTable(const QString& table)
ReturnSuccess DataBase::createTable(const QString& table_creation_query)
{
//    if (!db.tables().contains(table))
//    {
        QSqlQuery query(db);
//        const QString table_creation_query = "CREATE TABLE " + table + " (question TEXT PRIMARY KEY, answer TEXT)";
        if (!query.exec("CREATE TABLE IF NOT EXISTS " + table_creation_query))
        {
            qCritical() << "Failed to create table:" << query.lastError().text();
            return BadCreateTable;
        }
//    }
//    else
//        qDebug() << "Table is already present";
    return GoodCreateTable;
}

//ReturnSuccess DataBase::insertValue(const QString& table, const QString& insertQuery)
ReturnSuccess DataBase::insertValue(const QString& table, const QStringList& values)
{
    if (!db.tables().contains(table))
    {
        qCritical() << "No such table!";
        return NoSuchTable;
    }
    QSqlQuery query("SELECT * from " + table, db);

    const QString insertQuery = values.join(", ");

    while (query.next())
    {
        QStringList entry;
        for (int i = 0; i < values.size(); ++i)
            entry += query.value(i).toString();
        if (entry.join(", ") == insertQuery)
            return SameEntryExists;
    }

    if (!query.exec("INSERT INTO " + table + " VALUES (" + insertQuery + ")"))
    {
        qCritical() << "Failed to insert data into table:" << query.lastError().text();
        return BadInsertion;
    }
    return GoodInsertion;
}

int DataBase::calculate_current_id_of_table(const QString& table) const
{
    QSqlQuery query(db);
    if (!query.exec("select max(rowid) from " + table))    // 0 yok. 1'den basliyor
    {
        qCritical() << "Failed to query max rowid:" << query.lastError().text();
        return RowIdQueryFailed;
    }
    if (query.next())
    {
        return query.value(0).toInt();
    }
    qDebug() << "No Entry";
    return NoEntry;
}
