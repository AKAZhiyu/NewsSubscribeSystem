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
    MySQLConnector(const std::string& host, const std::string& user, const std::string& password) {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = std::unique_ptr<sql::Connection>(driver->connect(host, user, password));
        }
        catch (sql::SQLException& e) {
            std::cout << "SQL Exception: Cannot connect to the database.\n" << e.what();
            throw;
        }
    }

    sql::Connection* getConnection() const {
        return con.get();
    }

    ~MySQLConnector() = default; 
};