#pragma once
#include "IDataAccess.h"

class DatabaseAccess
{
public:
	//open and close DB
	bool open();
	void close();
	
	//check message from DB
	void check_status(char* errMessage, int res);

private:
	sqlite3* db;
};
