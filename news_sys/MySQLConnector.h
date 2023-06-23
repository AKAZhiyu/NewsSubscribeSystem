#pragma once
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

#include <memory>
#include <string>
#include <iostream>
#include <stdexcept>


class MySQLConnector {
private:
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> con;

public:
    MySQLConnector(const std::string& host, const std::string& user, const std::string& password, const std::string& shcema) {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = std::unique_ptr<sql::Connection>(driver->connect(host, user, password));
            con->setSchema("subscription_db");
        }
        catch (sql::SQLException& e) {
            std::cerr << "SQL Exception: Cannot connect to the database.\n" << e.what();
            throw;
        }
    }

    sql::Connection* getConnection() const {
        return con.get();
    }

    ~MySQLConnector() = default; 
};