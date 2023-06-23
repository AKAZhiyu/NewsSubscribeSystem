#pragma once
#include "Person.h"
class Admin :
    public Person
{
public:
    virtual bool renewPassword(const std::string& old_password, const std::string& new_password) {
        if (!isPassword(old_password)) {
            throw std::invalid_argument("Old password incorrect");
        }
        else {
            try {
                std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                    "UPDATE _Admin SET password = ? WHERE id = ?"
                ));

                pstmt->setString(1, new_password);
                pstmt->setString(2, id);
                pstmt->executeUpdate();

                // 成功更新数据库后，更新对象中的密码
                password = new_password;
                return true;
            }
            catch (sql::SQLException& e) {
                std::cerr << "# ERR: SQLException in " << __FILE__;
                std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
                std::cerr << "# ERR: " << e.what();
                std::cerr << " (MySQL error code: " << e.getErrorCode();
                std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
                return false;
            }
        }
    }


private:

    void changeBalance(const std::string& id, double amount) {
        double balance = getBalanceById(id);
        balance += amount;
        if (balance < 0) {
            throw std::invalid_argument("Balance cannot be negative");
        }
        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                R"(UPDATE UserAccount
                SET balance = ?
                WHERE id = ?)"));
            pstmt->setDouble(1, balance);
            pstmt->setString(2, id);
            pstmt->execute();
        }
        catch (sql::SQLException& e) {
            std::cerr << "# ERR: Change Balance failed!" << std::endl;
            std::cerr << "# ERR: SQLException in " << __FILE__;
            std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
            std::cerr << "# ERR: " << e.what();
            std::cerr << " (MySQL error code: " << e.getErrorCode();
            std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
            throw;
        }
    
    }

    double getBalanceById(const std::string& id) {
        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                R"(SELECT balance FROM useraccount WHERE id = ?)"));
            pstmt->setString(1, id);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            if (res->next()) { // 确保结果集不为空
                return res->getDouble("balance");
            }
            else {
                throw std::runtime_error("User ID not found: " + id);
            }
        }
        catch (sql::SQLException& e) {
            std::cerr << "# ERR: Get Balance failed!" << std::endl;
            std::cerr << "# ERR: SQLException in " << __FILE__;
            std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
            std::cerr << "# ERR: " << e.what();
            std::cerr << " (MySQL error code: " << e.getErrorCode();
            std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;

            throw std::runtime_error("Database error while getting balance");
        }
    }

    //todo
    void changePassword(const std::string& id, const std::string& new_password) {
        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                R"(UPDATE useraccount SET password = ? WHERE id = ?)"
            ));
            pstmt->setString(1, new_password);
            pstmt->setString(2, id);

            pstmt->execute();
        }
        catch (sql::SQLException& e) {
        std::cerr << "SQLException caught: " << e.what() << '\n';
        std::cerr << "Error code: " << e.getErrorCode() << '\n';
        std::cerr << "SQLState: " << e.getSQLState() << '\n';
        throw;
        }
    }

    std::vector<std::tuple<int, std::string, std::string, std::string, 
                std::string, int, std::string, double, std::string>> 
        getUserSubscription(const std::string& id) {

        std::vector<std::tuple<int, std::string, std::string, std::string, std::string, int, std::string, double, std::string>> subscriptions;

        try {
            // Prepare the SQL statement
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                R"(SELECT S.subscription_id, U.username, U.address, N.newspaper_name, N.cover, N._period, N.introduction, N.price, N._type 
            FROM Subscription S JOIN UserAccount U ON S._user_id = U.id JOIN Newspaper N ON S.newspaper_id = N.id 
            WHERE U.id = ?)"
            ));
            pstmt->setString(1, id);

            // Execute the query
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            // Loop through the result set and add the tuples to the vector
            while (res->next()) {
                subscriptions.push_back(std::make_tuple(
                    res->getInt("subscription_id"),
                    res->getString("username"),
                    res->getString("address"),
                    res->getString("newspaper_name"),
                    res->getString("cover"),
                    res->getInt("_period"),
                    res->getString("introduction"),
                    res->getDouble("price"),
                    res->getString("_type")
                ));
            }
        }
        catch (sql::SQLException& e) {
            std::cerr << "# ERR: SQLException in " << __FILE__;
            std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
            std::cerr << "# ERR: " << e.what();
            std::cerr << " (MySQL error code: " << e.getErrorCode();
            std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        }

        // Return the vector of tuples
        return subscriptions;
    }


};

