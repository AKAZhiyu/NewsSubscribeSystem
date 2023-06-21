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
    sql::Connection* con;

public:
    MySQLConnector(const std::string& tcp, const std::string& user, const std::string& password, const std::string& dbs) {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        try {
            con = driver->connect(tcp, user, password);
            con->setSchema(dbs);
        }
        catch (sql::SQLException& e) {
            std::string errMsg = "Connection to the database is Failed";
            errMsg += e.what();
            throw sql::SQLException(errMsg);
        }
    }
    
    const auto& getConnector() {
        return con;
    }

    ~MySQLConnector() {
        if (con != NULL) {
            delete con;
        }
    }


    
};
