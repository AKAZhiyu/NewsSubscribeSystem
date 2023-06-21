#pragma once
#include<string>
#include"MySQLConnector.h"

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
		id(id), password(password), connector(con) {}

	const std::string& getId() const { return id; }

	bool isPassword(const std::string& passed) const { return passed == password; }

};

