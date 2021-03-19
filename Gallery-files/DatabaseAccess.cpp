#include "DatabaseAccess.h"


bool DatabaseAccess::open()
{
	const char* sqlStatement;
	char* errMessage = nullptr;
	std::string dbFileName = "gallery.db";
	int doesntExist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &this->db);
	if (res != SQLITE_OK)
	{
		std::cout << "Error code: " << std::to_string(res) << std::endl;
		db = nullptr;
		std::cout << "Failed to open DB" << std::endl;
		return false;
	}
	if (doesntExist)
	{
		sqlStatement = "CREATE TABLE USERS (ID INTEGER PRIMARY KEY AUTOINCREMENT,"
			"NAME TEXT NOT NULL);";
		res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
		check_status(errMessage, res);
		sqlStatement = "CREATE TABLE ALBUMS(ID INTEGER PRIMARY KEY  AUTOINCREMENT,"
			"NAME TEXT NOT NULL ,"
			"CREATION_DATE DATE NOT NULL,"
			"USER_ID INTEGER NOT NULL,"
			"FOREIGN KEY (USER_ID) REFERENCES USERS (ID));";
		res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
		check_status(errMessage, res);
		sqlStatement = "CREATE TABLE PICTURES(ID INTEGER PRIMARY KEY  NOT NULL,"
			"NAME TEXT NOT NULL ,"
			"LOCATION TEXT"
			"CREATION_DATE DATE NOT NULL,"
			"PersonID INTEGER,"
			"ALBUM_ID INTEGER NOT NULL,"
			"FOREIGN KEY (ALBUM_ID) REFERENCES ALBUMS(ID));";
		res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
		check_status(errMessage, res);
		sqlStatement = "CREATE TABLE TAGS(ID INTEGER PRIMARY KEY  NOT NULL,"
			"PICTURE_ID INTEGER NOT NULL,"
			"USER_ID INTEGER NOT NULL,"
			"FOREIGN KEY (PICTURE_ID) REFERENCES PICTURES(ID),"
			"FOREIGN KEY (USER_ID) REFERENCES USERS (ID));";
		res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
		check_status(errMessage, res);
	}
	return true;
}
void DatabaseAccess::close()
{
	sqlite3_close(this->db);
	this->db = nullptr;
}
void DatabaseAccess::check_status(char* errMessage, int res)
{
	if (res != SQLITE_OK)
	{
		std::cout << errMessage << std::endl;
		sqlite3_close(db);
		db = nullptr;
		exit(-1);
	}
}