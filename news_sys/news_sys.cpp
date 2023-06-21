#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

#include <iostream>

int main()
{
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;

    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "zzx20030628");
    con->setSchema("subscription_db");
    stmt = con->createStatement();
    //stmt->execute("CREATE TABLE IF NOT EXISTS test_table (id INT AUTO_INCREMENT, name VARCHAR(255), PRIMARY KEY(id))");
    //stmt->executeQuery("SELECT * FROM newspaper");
    sql::ResultSet* res;
    res = stmt->executeQuery("SELECT * FROM newspaper");
    while (res->next()) {
        std::cout << "id: " << res->getInt("id") << ", name: " << res->getString("newspaper_name") << std::endl;
        std::cout << "period: " << res->getInt("_period") << std::endl;

    }
    delete res;
    delete stmt;
    delete con;
    return 0;
}

/*
* 
*   sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;

    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "zzx20030628");
    con->setSchema("subscription_db");
    stmt = con->createStatement();
    //stmt->execute("CREATE TABLE IF NOT EXISTS test_table (id INT AUTO_INCREMENT, name VARCHAR(255), PRIMARY KEY(id))");

    sql::ResultSet* res;

    res = stmt->executeQuery("SELECT * FROM newspaper");

    while (res->next()) {
        std::cout << "id: " << res->getInt("id") << ", name: " << res->getString("newspaper_name") << std::endl;
        std::cout << "period: " << res->getInt("_period") << std::endl;

    }
    delete res;
    delete stmt;
    delete con;

    getInt()：用于获取整数值，对应于 MySQL 中的 INT 类型。

    getUInt()：用于获取无符号整数值，对应于 MySQL 中的 UNSIGNED INT 类型。

    getInt64() 和 getUInt64()：用于获取长整数值，对应于 MySQL 中的 BIGINT 类型。

    getDouble()：用于获取浮点数值，对应于 MySQL 中的 FLOAT 和 DOUBLE 类型。

    getString()：用于获取字符串值，对应于 MySQL 中的 CHAR、VARCHAR、TEXT 类型。

    getBoolean()：用于获取布尔值，对应于 MySQL 中的 TINYINT(1) 类型。

    getBlob()：用于获取二进制大对象(BLOB)，对应于 MySQL 中的 BLOB 类型。

    getDate()、getTime() 和 getTimestamp()：用于获取日期、时间和时间戳，对应于 MySQL 中的 DATE、TIME 和 TIMESTAMP 类型。

    sql::Date date = res->getDate("date_column");
    int year = date.getYear();
    int month = date.getMonth();
    int day = date.getDay();

*/