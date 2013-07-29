#include "sqladapter.h"

SqlAdapter::SqlAdapter()
{
}

bool SqlAdapter::createDatabase(const QString &name)
{
    m_lastQuery = QString("CREATE DATABASE %1;")
            .arg(name);
    return m_query.exec(m_lastQuery);
}

bool SqlAdapter::createTable(const QString &tableName, const QHash<QString, QString> &info)
{
    QString name;
    m_lastQuery = QString("CREATE TABLE %1(id BIGINT, ")
            .arg(tableName);
    foreach(name, info.keys())
        m_lastQuery += QString("%1 %2, ")
                .arg(name)
                .arg(m_tableTypes.value(info.value(name)));
    m_lastQuery += "PRIMARY KEY (id));";
    return m_query.exec(m_lastQuery);
}

bool SqlAdapter::createTableRelations(const QString &parent, Relation rel, const QString &child)
{
    if(rel == HasOne || rel == HasMany)
        m_lastQuery = QString("ALTER TABLE %1 ADD %2_id INTEGER;")
                .arg(child)
                .arg(parent);
    return m_query.exec(m_lastQuery);
}

bool SqlAdapter::dropTable(const QString &tableName)
{
    m_lastQuery = QString("DROP TABLE %1;")
            .arg(tableName);
    return m_query.exec(m_lastQuery);
}

bool SqlAdapter::dropDatabase(const QString &name)
{
    m_lastQuery = QString("DROP DATABASE %1;")
            .arg(name);
    return m_query.exec(m_lastQuery);
}

int SqlAdapter::addRecord(const QString &tableName, const QHash<QString, QVariant> &info)
{
    QString key;
    m_lastQuery = QString("INSERT INTO %1(")
            .arg(tableName);
    foreach(key, info.keys())
        m_lastQuery += key + ", ";
    if(!info.isEmpty())
        m_lastQuery.resize(m_lastQuery.size() - 2);
    m_lastQuery += ") VALUES(";
    foreach(key, info.keys())
        m_lastQuery += "'" + info.value(key).toString() + "', ";
    if(!info.isEmpty())
        m_lastQuery.resize(m_lastQuery.size() - 2);
    m_lastQuery += ");";
    if(m_query.exec(m_lastQuery))
        return m_query.lastInsertId().toInt();
    else
        return -1;
}

bool SqlAdapter::updateRecord(const QString &tableName, const qlonglong id, const QHash<QString, QVariant> &info)
{
    m_lastQuery = QString("UPDATE %1 SET ")
            .arg(tableName);
    QString fieldName;
    foreach(fieldName, info.keys())
        m_lastQuery += QString("%1 = '%2', ")
                .arg(fieldName)
                .arg(info.value(fieldName).toString());
    m_lastQuery.resize(m_lastQuery.size() - 2);
    m_lastQuery += QString(" WHERE id = %1;")
            .arg(id);
    return m_query.exec(m_lastQuery);
}

QList<QSqlRecord> SqlAdapter::find(const QString &tableName, const QString &fieldName, const QString &params)
{
    QList<QSqlRecord> result;
    m_lastQuery = QString("SELECT %1 FROM %2 %3;")
            .arg(fieldName)
            .arg(tableName)
            .arg(params);
    if(m_query.exec(m_lastQuery))
        while(m_query.next())
            result.append(m_query.record());
    return result;
}

QSqlRecord SqlAdapter::first(const QString &tableName)
{
    m_lastQuery = QString("SELECT * FROM %1 ORDER BY id ASC LIMIT 1;")
            .arg(tableName);
    m_query.exec(m_lastQuery);
    m_query.next();
    return m_query.record();
}

QSqlRecord SqlAdapter::last(const QString &tableName)
{
    m_lastQuery = QString("SELECT * FROM %1 ORDER BY id DESC LIMIT 1;")
            .arg(tableName);
    m_query.exec(m_lastQuery);
    m_query.next();
    return m_query.record();
}

bool SqlAdapter::remove(const QString &tableName, const QString &params)
{
    m_lastQuery = QString("DELETE FROM %1 %2;")
            .arg(tableName)
            .arg(params);
    bool result = m_query.exec(m_lastQuery);
    return result;
}

int SqlAdapter::count(const QString &tableName, const QString &arg)
{
    m_lastQuery = QString("SELECT COUNT(%1) FROM %2;")
            .arg(arg)
            .arg(tableName);
    if(m_query.exec(m_lastQuery))
    {
        m_query.next();
        return m_query.value(0).toInt();
    }
    else
        return -1;
}

int SqlAdapter::countBy(const QString &tableName, const QString &params)
{
    m_lastQuery = QString("SELECT COUNT(*) FROM %1 %2;")
            .arg(tableName)
            .arg(params);
    if(m_query.exec(m_lastQuery))
    {
        m_query.next();
        return m_query.value(0).toInt();
    }
    else
        return -1;
}

double SqlAdapter::calculation(ORMAbstractAdapter::Calculation func, const QString &tableName,
                                 const QString &fieldName, const QString &params)
{
    QString funcName;
    switch(func)
    {
    case Average:
        funcName = "AVG";
        break;
    case Maximum:
        funcName = "MAX";
        break;
    case Minimum:
        funcName = "MIN";
        break;
    case Sum:
        funcName = "SUM";
        break;
    default:
        return 0;
    }
    m_lastQuery = QString("SELECT %1(%2) FROM %3 %4;")
            .arg(funcName)
            .arg(fieldName)
            .arg(tableName)
            .arg(params);
    m_query.exec(m_lastQuery);
    m_query.next();
    return m_query.value(0).toDouble();
}

QHash<QString, QList<QSqlRecord> > SqlAdapter::includes(const QString &parentModel, const QStringList &childModels, const QString &params)
{
    QList<QSqlRecord> generalList = find(parentModel, "*", params);
    QHash<QString, QList<QSqlRecord> > result;
    result.insert(parentModel, generalList);
    QString whereForChildren = QString("WHERE %1_id IN (")
            .arg(parentModel);
    for(int i = 0; i < generalList.size(); i++)
        whereForChildren += generalList.value(i).field("id").value().toString() + ", ";
    whereForChildren.resize(whereForChildren.size() - 2);
    whereForChildren += ')';
    for(int i = 0; i < childModels.size(); i++)
        result.insert(childModels.value(i), find(childModels.value(i), "*", whereForChildren));
    return result;
}

void SqlAdapter::fillTableTypes()
{
    m_tableTypes.insert("bool", "BOOL");
    m_tableTypes.insert("int", "INT");
    m_tableTypes.insert("uint", "INT UNSIGNED");
    m_tableTypes.insert("qlonglong", "BIGINT");
    m_tableTypes.insert("qulonglong", "BIGINT UNSIGNED");
    m_tableTypes.insert("double", "DOUBLE");
    m_tableTypes.insert("QByteArray", "BLOB");
    m_tableTypes.insert("QChar", "CHAR(1)");
    m_tableTypes.insert("QDate", "DATE");
    m_tableTypes.insert("QTime", "TIME");
    m_tableTypes.insert("QDateTime", "DATETIME");
    m_tableTypes.insert("QString", "TEXT");
}