#pragma once
#include<string>
#include"MySQLConnector.h"
#include<utility> // for std::pair
#include<vector>

class Person
{
protected:
	std::string id;
	std::string password;
	const MySQLConnector* connector;


public:
	virtual ~Person() {}
	Person(const std::string& id, const std::string& password,
		MySQLConnector* con) :
		id(id), password(password), connector(con) {
    }

	const std::string& getId() const { return id; }

	bool isPassword(const std::string& passed) const { return passed == password; }

    //std::vector<std::pair<std::string, int>> getTopPublications(int start, int end) {
    //    std::vector<std::pair<std::string, int>> topPublications;
    //    int n = end - start + 1; // 计算要返回的行数
    //    if (n < 1) {
    //        std::cerr << "Invalid range.\n";
    //        return topPublications;
    //    }
    //    sql::Statement* stmt = connector->getConnection()->createStatement();
    //    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
    //        "SELECT newspaper_name, COUNT(*) as count "
    //        "FROM Subscription "
    //        "JOIN Newspaper ON Subscription.newspaper_id = Newspaper.id "
    //        "GROUP BY newspaper_id "
    //        "ORDER BY count DESC "
    //        "LIMIT " + std::to_string(start - 1) + ", " + std::to_string(n)));
    //    while (res->next()) {
    //        topPublications.push_back({ res->getString("newspaper_name"), res->getInt("count") });
    //    }
    //    delete stmt;
    //    return topPublications;
    //}

    /*
     * Get top publications from start to end, 0-indexed.
     */
    std::vector<std::pair<std::string, int>> getTopPublications(int start, int end) {
        std::vector<std::pair<std::string, int>> topPublications;
        int n = end - start + 1;
        if (n < 1) {
            std::cerr << "Invalid range.\n";
            return topPublications;
        }
        std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
            R"(SELECT newspaper_name, count(*) AS count
            FROM UserSubscription
            GROUP BY newspaper_name
            ORDER BY count DESC
            LIMIT ?, ?)"
        ));
        pstmt->setInt(1, start);
        pstmt->setInt(2, n);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            topPublications.push_back({ res->getString("newspaper_name"), res->getInt("count") });
        }
        return topPublications;
    }


    std::vector<std::tuple<int, std::string, int>> getTopPublishers(int start, int end) {
        std::vector<std::tuple<int, std::string, int>> topPublishers;
        int n = end - start + 1;
        if (n < 1) {
            std::cerr << "Invalid range.\n";
            return topPublishers;
        }
        // 创建预备 SQL 查询
        std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
            R"(SELECT  p.publisher_id,
            p.publisher_name,
            count(*) AS subscription_amount
            FROM
                PublisherNewspaper p
            JOIN
                Subscription s ON s.newspaper_id = p.newspaper_id
            GROUP BY
                p.publisher_id
            ORDER BY
                subscription_amount DESC
            LIMIT ?, ?)"
        ));

        // 设置预备 SQL 查询参数
        pstmt->setInt(1, start);
        pstmt->setInt(2, n);

        // 执行预备 SQL 查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 遍历结果集
        while (res->next()) {
            topPublishers.push_back(std::make_tuple(res->getInt("publisher_id"), res->getString("publisher_name"), 
                                    res->getInt("subscription_amount")));
        }
        return topPublishers;
    }

    std::vector<std::tuple<int, std::string, std::string, int, std::string, double, std::string>>
        showNewspapers(int start, int end) {
        std::vector<std::tuple<int, std::string, std::string, int, std::string, double, std::string>> Newspapers;
        int n = end - start + 1;
        if (n < 1) {
            std::cerr << "Invalid range.\n";
            return Newspapers;
        }
        // 创建预备 SQL 查询
        std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
            R"(SELECT id, newspaper_name, cover, _period, introduction, price, _type
            FROM Newspaper
            ORDER BY id
            LIMIT ?, ?)"
        ));

        // 设置预备 SQL 查询参数
        pstmt->setInt(1, start);
        pstmt->setInt(2, n);

        // 执行预备 SQL 查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 遍历结果集
        while (res->next()) {
            Newspapers.push_back(std::make_tuple(res->getInt("id"), res->getString("newspaper_name"),
                res->getString("cover"), res->getInt("_period"),
                res->getString("introduction"), res->getDouble("price"),
                res->getString("_type")));
        }
        return Newspapers;
    }

    std::vector<std::tuple<int, std::string, std::string, std::string>>
        showPublishers(int start, int end) {
        std::vector<std::tuple<int, std::string, std::string, std::string>> publishers;
        int n = end - start + 1;
        if (n < 1) {
            std::cerr << "Invalid range.\n";
            return publishers;
        }
        // 创建预备 SQL 查询
        std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
            R"(SELECT id, username, address, introduction
            FROM Publisher
            ORDER BY id
            LIMIT ?, ?)"
        ));

        // 设置预备 SQL 查询参数
        pstmt->setInt(1, start);
        pstmt->setInt(2, n);

        // 执行预备 SQL 查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 遍历结果集
        while (res->next()) {
            publishers.push_back(std::make_tuple(res->getInt("id"), res->getString("username"),
                res->getString("address"), res->getString("introduction")));
        }
        return publishers;
    }

    std::vector<std::tuple<int, std::string, std::string, int, std::string, double, std::string>> 
        findNewspaperByName(const std::string& name) {
        // 创建存储返回值的向量
        std::vector<std::tuple<int, std::string, std::string, int, std::string, double, std::string>> foundNewspapers;

        // 创建预备 SQL 查询
        std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
            R"(SELECT *
        FROM Newspaper
        WHERE newspaper_name LIKE ?)"
        ));

        // 设置预备 SQL 查询参数
        pstmt->setString(1, "%" + name + "%");

        // 执行预备 SQL 查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 遍历结果集
        while (res->next()) {
            foundNewspapers.push_back(std::make_tuple(
                res->getInt("id"),
                res->getString("newspaper_name"),
                res->getString("cover"),
                res->getInt("_period"),
                res->getString("introduction"),
                res->getDouble("price"),
                res->getString("_type")));
        }

        return foundNewspapers;
    }

    //If donnot find, return std::make_tuple(-1, "", "", -1, "", -1.0, "");
    std::tuple<int, std::string, std::string, int, std::string, double, std::string> 
        findNewspaperById(int id) {
        // 创建预备 SQL 查询
        std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
            R"(SELECT *
        FROM Newspaper
        WHERE id = ?)"
        ));

        // 设置预备 SQL 查询参数
        pstmt->setInt(1, id);

        // 执行预备 SQL 查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 如果结果集不为空，返回结果
        if (res->next()) {
            return std::make_tuple(
                res->getInt("id"),
                res->getString("newspaper_name"),
                res->getString("cover"),
                res->getInt("_period"),
                res->getString("introduction"),
                res->getDouble("price"),
                res->getString("_type"));
        }

        // 如果结果集为空，返回一个特殊的tuple，其id为-1，表示找不到结果
        return std::make_tuple(-1, "", "", -1, "", -1.0, "");
    }

    std::vector<std::tuple<int, std::string, std::string, int, std::string,
        double, std::string, std::string>> 
        findNewspapersByPublisherName(const std::string& publisherName) {
        std::vector<std::tuple<int, std::string, std::string, int, std::string, double, std::string, std::string>> newspapers;
        std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
            R"(SELECT Newspaper.*, Publisher.username AS publisher_username
        FROM Newspaper
        JOIN Publication ON Newspaper.id = Publication.newspaper_id
        JOIN Publisher ON Publication.publisher_id = Publisher.id
        WHERE Publisher.username LIKE ?)"
        ));

        pstmt->setString(1, "%" + publisherName + "%");
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            newspapers.push_back(
                std::make_tuple(
                    res->getInt("id"),
                    res->getString("newspaper_name"),
                    res->getString("cover"),
                    res->getInt("_period"),
                    res->getString("introduction"),
                    res->getDouble("price"),
                    res->getString("_type"),
                    res->getString("publisher_username")
                )
            );
        }
        return newspapers;
    }

    std::vector<std::tuple<int, std::string, std::string, int, std::string,
        double, std::string, std::string>>
        findNewspapersByPublisherId(int publisherId) {
        std::vector<std::tuple<int, std::string, std::string, int, std::string,
            double, std::string, std::string>> newspapers;

        std::unique_ptr<sql::PreparedStatement> pstmt(connector->getConnection()->prepareStatement(
            R"(SELECT Newspaper.*, Publisher.username AS publisher_username
        FROM Newspaper
        JOIN Publication ON Newspaper.id = Publication.newspaper_id
        JOIN Publisher ON Publication.publisher_id = Publisher.id
        WHERE Publisher.id = ?)"
        ));

        pstmt->setInt(1, publisherId);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            newspapers.push_back(
                std::make_tuple(
                    res->getInt("id"),
                    res->getString("newspaper_name"),
                    res->getString("cover"),
                    res->getInt("_period"),
                    res->getString("introduction"),
                    res->getDouble("price"),
                    res->getString("_type"),
                    res->getString("publisher_username")
                )
            );
        }

        return newspapers;
    }

};

