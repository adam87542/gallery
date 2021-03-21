#include "DatabaseAccess.h"
int callback_for_albums(void* data, int argc, char** argv, char** azColName)
{
	Album album;
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "USER_ID") {
			album.setOwner(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "NAME") {
			album.setName(argv[i]);
		}
		else if (std::string(azColName[i]) == "CREATION_DATE") {
			album.setCreationDate(argv[i]);
		}
	}
	 m_albums.push_back(album);
	return 0;
}
int callback_print(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		cout << azColName[i] << " = " << argv[i] << " , ";
	}
	cout << endl;
	return 0;
}
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
		sqlStatement = "CREATE TABLE TAGS(ID INTEGER PRIMARY KEY  AUTOINCREMENT,"
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


bool DatabaseAccess::check_status(char* errMessage, int res)
{
	if (res != SQLITE_OK)
	{
		std::cout << errMessage << std::endl;
		return false;
	}
	return true;
}

const std::list<Album> DatabaseAccess::getAlbums()
{
	char* errMessage = nullptr;
	const char* sqlStatement = "SELECT * FROM ALBUMS";
	int res = sqlite3_exec(db, sqlStatement, callback_for_albums, nullptr, &errMessage);
	check_status(errMessage, res);
	std::list<Album> copy = m_albums;
	m_albums.clear();
	return copy;
}
const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	char* errMessage = nullptr;
	const char* sqlStatement = "SELECT * FROM ALBUMS WHERE ID = " + user.getId();
	int res = sqlite3_exec(db, sqlStatement, callback_for_albums, nullptr, &errMessage);
	check_status(errMessage, res);
	std::list<Album> copy = m_albums;
	m_albums.clear();
	return copy;
}
void DatabaseAccess::createAlbum(const Album& album)
{
	char* errMessage = nullptr;
	 std::string sqlStatement = "INSERT INTO ALBUMS(NAME  , CREATION_DATE, USER_ID) VALUES(" + album.getName() + ',' + album.getCreationDate() +  ',' + std::to_string(album.getOwnerId()) + "));";
	int res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	check_status(errMessage, res);
	m_albums.clear();
}
void  DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	char* errMessage = nullptr;
	const char* sqlStatement = "DELETE * FROM ALBUMS WHERE USER_ID = " + userId + ';';
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (!check_status(errMessage, res))
		throw MyException("No album with name " + albumName + " exists");
}
bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	char* errMessage = nullptr;
	const char* sqlStatement = "SELECT * FROM ALBUMS WHERE USER_ID =" + userId + ';';
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (!check_status(errMessage, res))
		return false;
	m_albums.clear();
	return true;
}
Album  DatabaseAccess::openAlbum(const std::string& albumName)
{
	char* errMessage = nullptr;
	const char* sqlStatement = "SELECT * FROM ALBUMS;";
	int res = sqlite3_exec(db, sqlStatement, callback_for_albums, nullptr, &errMessage);
	check_status(errMessage, res);
	for (auto& album : m_albums) {
		if (albumName == album.getName()) 
		{
			m_albums.clear();
			return album;
		}
	}
	throw MyException("No album with name " + albumName + " exists");
}
void DatabaseAccess::printAlbums()
{
	char* errMessage = nullptr;
	const char* sqlStatement = "SELECT * FROM ALBUMS;";
	int res = sqlite3_exec(db, sqlStatement, callback_print, nullptr, &errMessage);
	check_status(errMessage, res);
	m_albums.clear();
}