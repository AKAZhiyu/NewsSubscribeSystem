#pragma once
#include "Person.h"

class User :
    public Person
{
protected:
    std::string username;
    std::string address;
    double balance;

    void validateUserLogin() {
        std::unique_ptr<sql::Statement> stmt((*connector).getConnection()->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT password FROM UserAccount WHERE id = " + id));
        if (!isPassword(res->getString("password"))) {
            std::string errMsg = "Wrong Password";
            throw std::invalid_argument(errMsg);
        }
    }

public:
    User(const std::string& id, const std::string& password,
        MySQLConnector* con) : Person(id, password, con) {
        validateUserLogin();
        std::unique_ptr<sql::Statement> stmt((*connector).getConnection()->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM UserAccount WHERE id = " + id));
        username = res->getString("username");
        address = res->getString("address");
        balance = res->getDouble("balance");
    }
    const std::string& getUsername() const {
        return username;
    }

    const std::string& getAddress() const {
        return address;
    }

    double getBalance() const {
        return balance;
    }
};
