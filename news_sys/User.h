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
        if (res->next()) {  // 如果结果集不为空
            if (!isPassword(res->getString("password"))) {
                std::string errMsg = "Wrong Password";
                throw std::invalid_argument(errMsg);
            }
        }
        else {
            // 找不到指定id的用户，抛出异常或其他处理逻辑
            std::string errMsg = "User not found";
            throw std::invalid_argument(errMsg);
        }

    }

    void subtractBalance(double sub) {
        if (sub < 0) {
            throw std::invalid_argument("Cannot add balance");
        }
        if (balance - sub < 0) {
            throw std::invalid_argument("Insufficient balance");
        }
        double new_balance = balance - sub;
        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                R"(UPDATE UserAccount
                SET balance = ?
                WHERE id = ?)"));
            pstmt->setDouble(1, new_balance);
            pstmt->setString(2, id);
            pstmt->execute();
        }
        catch (sql::SQLException& e) {
            std::cerr << "# ERR: Subtract Balance failed!" << std::endl;
            std::cerr << "# ERR: SQLException in " << __FILE__;
            std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
            std::cerr << "# ERR: " << e.what();
            std::cerr << " (MySQL error code: " << e.getErrorCode();
            std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        }
        balance = new_balance;
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

    virtual bool renewPassword(const std::string& old_password, const std::string& new_password)  {
        if (!isPassword(old_password)) {
            throw std::invalid_argument("Old password incorrect");
        }
        else {
            try {
                std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                    "UPDATE UserAccount SET password = ? WHERE id = ?"
                ));

                pstmt->setString(1, new_password);
                pstmt->setString(2, id);
                pstmt->executeUpdate();

                // 成功更新数据库后，更新对象中的密码
                password = new_password;
                return true;
            }
            catch (sql::SQLException& e) {
                std::cerr << "# ERR: Change password failed!" << std::endl;
                std::cerr << "# ERR: SQLException in " << __FILE__;
                std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
                std::cerr << "# ERR: " << e.what();
                std::cerr << " (MySQL error code: " << e.getErrorCode();
                std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
                return false;
            }
        }
    }

    const std::string& getAddress() const {
        return address;
    }

    double getBalance() const {
        return balance;
    }

    void setUsername(const std::string& new_username) {
        try {
            // Prepare SQL statement to update the username of the user
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                "UPDATE UserAccount SET username = ? WHERE id = ?"
            ));

            pstmt->setString(1, new_username);
            pstmt->setString(2, id);
            pstmt->executeUpdate();

            // If successful, update the username in the object
            username = new_username;
        }
        catch (sql::SQLException& e) {
            std::cerr << "# ERR: setUsername failed!" << std::endl;
            std::cerr << "# ERR: SQLException in " << __FILE__;
            std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
            std::cerr << "# ERR: " << e.what();
            std::cerr << " (MySQL error code: " << e.getErrorCode();
            std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        }
    }

    void setAddress(const std::string& new_address) {
        try {
            // Prepare SQL statement to update the address of the user
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                "UPDATE UserAccount SET address = ? WHERE id = ?"
            ));

            pstmt->setString(1, new_address);
            pstmt->setString(2, id);
            pstmt->executeUpdate();

            // If successful, update the address in the object
            address = new_address;
        }
        catch (sql::SQLException& e) {
            std::cerr << "# ERR: setAddress failed!" << std::endl;
            std::cerr << "# ERR: SQLException in " << __FILE__;
            std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
            std::cerr << "# ERR: " << e.what();
            std::cerr << " (MySQL error code: " << e.getErrorCode();
            std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        }
    }

    std::vector<std::tuple<int, std::string, std::string, 
                           std::string, std::string, int, std::string,
                           double, std::string>> showSubscriptions() {
        std::vector<std::tuple<int, std::string, std::string, std::string, std::string, int, std::string, double, std::string>> subscriptions;

        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                "SELECT UserSubscription.newspaper_id, UserSubscription.newspaper_name, UserSubscription.start_date, UserSubscription.end_date, "
                "Newspaper.cover, Newspaper._period, Newspaper.introduction, Newspaper.price, Newspaper._type "
                "FROM UserSubscription "
                "JOIN Newspaper ON UserSubscription.newspaper_id = Newspaper.id "
                "WHERE UserSubscription.user_id = ?"
            ));

            pstmt->setString(1, id);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            while (res->next()) {
                subscriptions.push_back(std::make_tuple(res->getInt("newspaper_id"), res->getString("newspaper_name"),
                    res->getString("start_date"), res->getString("end_date"), res->getString("cover"),
                    res->getInt("_period"), res->getString("introduction"), res->getDouble("price"),
                    res->getString("_type")));
            }
        }
        catch (sql::SQLException& e) {
            std::cerr << "# ERR: SQLException in " << __FILE__;
            std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
            std::cerr << "# ERR: " << e.what();
            std::cerr << " (MySQL error code: " << e.getErrorCode();
            std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        }

        return subscriptions;
    }
    
    void subscribeNewspaper(const std::string& newspaperId, int times) {
        // 获取报纸的价格
        double newspaperPrice;
        {
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                "SELECT price FROM Newspaper WHERE id = ?"
            ));
            pstmt->setString(1, newspaperId);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            if (res->next()) {
                newspaperPrice = res->getDouble("price");
            }
            else {
                throw std::runtime_error("Newspaper ID not found: " + newspaperId);
            }
        }

        // 计算总价格
        double totalCost = newspaperPrice * times;

        // 检查用户的余额是否足够
        if (balance < totalCost) {
            throw std::runtime_error("Not enough balance for the subscription");
        }

        // 扣除余额
        balance -= totalCost;
        {
            std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
                "UPDATE UserAccount SET balance = ? WHERE id = ?"
            ));
            pstmt->setDouble(1, balance);
            pstmt->setString(2, id);
            pstmt->executeUpdate();
        }

        //  Add or update a record in the Subscription table
        
        // First, check if a subscription already exists
        std::unique_ptr<sql::PreparedStatement> pstmt_check(connector->getConnection()->prepareStatement(
            "SELECT * FROM Subscription WHERE _user_id = ? AND newspaper_id = ?"
        ));
        pstmt_check->setString(1, this->id);
        pstmt_check->setString(2, newspaperId);
        std::unique_ptr<sql::ResultSet> res_check(pstmt_check->executeQuery());

        if (res_check->next()) {
            // If a subscription already exists, update it
            std::unique_ptr<sql::PreparedStatement> pstmt_update(connector->getConnection()->prepareStatement(
                "UPDATE Subscription SET duration = duration + ?, end_date = DATE_ADD(end_date, INTERVAL ? DAY) WHERE _user_id = ? AND newspaper_id = ?"
            ));
            pstmt_update->setInt(1, times);
            pstmt_update->setInt(2, times);
            pstmt_update->setString(3, this->id);
            pstmt_update->setString(4, newspaperId);
            pstmt_update->executeUpdate();
        }
        else {
            // If no subscription exists, create a new one
            std::unique_ptr<sql::PreparedStatement> pstmt_insert(connector->getConnection()->prepareStatement(
                "INSERT INTO Subscription(_user_id, newspaper_id, duration, start_date, end_date) VALUES (?, ?, ?, CURDATE(), DATE_ADD(CURDATE(), INTERVAL ? DAY))"
            ));
            pstmt_insert->setString(1, this->id);
            pstmt_insert->setString(2, newspaperId);
            pstmt_insert->setInt(3, times);
            pstmt_insert->setInt(4, times);
            pstmt_insert->executeUpdate();
        }
        


;
    }


};
