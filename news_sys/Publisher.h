#pragma once
#include"Person.h"
class Publisher :
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
                    "UPDATE Publisher SET password = ? WHERE id = ?"
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



};

