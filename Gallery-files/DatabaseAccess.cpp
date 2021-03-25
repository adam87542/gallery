#include "DatabaseAccess.h"
sqlite3* DatabaseAccess::db;
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
int callback_for_pictures(void* data, int argc, char** argv, char** azColName)
{
	Picture picture;
	int id_of_album = 0;
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID") {
		     picture.setId(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "NAME") {
			picture.setName(argv[i]);
		}
		else if (std::string(azColName[i]) == "CREATION_DATE") {
			picture.setCreationDate(argv[i]);
		}
		else if (std::string(azColName[i]) == "LOCATION") {
			picture.setPath(argv[i]);
		}
		else if (std::string(azColName[i]) == "Album_ID") {
			id_of_album = (atoi(argv[i]));
		}

		for (auto& album : m_albums) {
			if (id_of_album == album.Id())
			{
				picture.set_AlbumId(album.Id());
				album.addPicture(picture);
				break;
			}
		}
	}
	return 0;
}
int callback_for_users(void* data, int argc, char** argv, char** azColName)
{
	int user_id = 0;
	std::string name;
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID") {
			user_id = atoi(argv[i]);
		}
		else if (std::string(azColName[i]) == "NAME") {
			name = (argv[i]);
		}
	}
	User user(user_id, name);
	m_users.push_back(user);
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
int callback_tags(void* data, int argc, char** argv, char** azColName)
{
	if (argc > 1)
	{
		is_taged = true;
		m_id = std::stoi(argv[1]);
	}
	else
	  is_taged = false;
	   return 0;
}
int callback_for_recevie(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
		{
			m_tag_value = std::stoi(argv[i]);
			return 0;
		}
	}
	return 0;
}
bool DatabaseAccess::open()
{
	const char* sqlStatement;
	char* errMessage = nullptr;
	std::string dbFileName = "gallery.db";
	int doesntExist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &db);
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
	sqlite3_close(db);
	db = nullptr;
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
	const std::string sqlStatement = "INSERT INTO ALBUMS(NAME  , CREATION_DATE, USER_ID) VALUES(" + album.getName() + ',' + album.getCreationDate() +  ',' + std::to_string(album.getOwnerId()) + ");";
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
	int res = sqlite3_exec(db, sqlStatement, callback_for_albums, nullptr, &errMessage);
	check_status(errMessage, res);
	for (const auto& album : m_albums) {
		if ((album.getName() == albumName) && (album.getOwnerId() == userId)) {
			return true;
		}
	}
	m_albums.clear();
	return false;
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
	m_albums.clear();
	throw MyException("No album with name " + albumName + " exists");
}
void DatabaseAccess::printAlbums()
{
	char* errMessage = nullptr;
	const char* sqlStatement = "SELECT * FROM ALBUMS;";
	int res = sqlite3_exec(db, sqlStatement, callback_for_albums, nullptr, &errMessage);
	check_status(errMessage, res);
	if (m_albums.empty()) {
		throw MyException("There are no existing albums.");
	}
	std::cout << "Album list:" << std::endl;
	std::cout << "-----------" << std::endl;
	for (const Album& album : m_albums) {
		std::cout << std::setw(5) << "* " << album;
	}
	m_albums.clear();
}





void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	char* errMessage = nullptr;
	 std::string sqlStatement = "SELECT ID FROM ALBUMS WHERE NAME =" + albumName + ';';
	int res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_recevie, nullptr, &errMessage);
	check_status(errMessage, res);
	sqlStatement = "INSERT INTO PICTURES(ID , NAME , LOCATION  , CREATION_DATE, ALBUM_ID) VALUES(" + picture.getId()  + ',' + picture.getName() + picture.getPath() + ',' + picture.getCreationDate() + ',' + std::to_string(m_id) + ");";
	res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	check_status(errMessage, res);
}

void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	char* errMessage = nullptr;
	const std::string sqlStatement = "DELETE FROM PICTURES WHERE NAME = " + pictureName + ';';
	int res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	check_status(errMessage, res);
}

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	char* errMessage = nullptr;
	 std::string sqlStatement = "SELECT ID FROM PICTURES WHERE NAME =" + pictureName + ';';
	int res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_recevie, nullptr, &errMessage);
	check_status(errMessage, res);
	sqlStatement = "INSERT INTO TAGS(PICTURE_ID , USER_ID) VALUES(" + std::to_string(m_id) +  ',' + std::to_string(userId) + ");";
	 res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	check_status(errMessage, res);
}

void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	char* errMessage = nullptr;
	std::string sqlStatement = "SELECT ID FROM PICTURES WHERE NAME =" + pictureName + ';';
	int res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_recevie, nullptr, &errMessage);
	check_status(errMessage, res);
	sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID = " + std::to_string(m_id) + ';';
	 res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	check_status(errMessage, res);
}



void DatabaseAccess::printUsers()
{
	char* errMessage = nullptr;
	std::string sqlStatement = "SELECT * FROM USERS;";
	int res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_users, nullptr, &errMessage);
	check_status(errMessage, res);
	std::cout << "Users list:" << std::endl;
	std::cout << "-----------" << std::endl;
	for (const auto& user : m_users) {
		std::cout << user << std::endl;
	}
	m_users.clear();
}
void DatabaseAccess::createUser(User& user)
{
	char* errMessage = nullptr;
	const std::string sqlStatement = "INSERT INTO ALBUMS(ID , NAME) VALUES(" + std::to_string(user.getId()) + ',' + user.getName() + ");";
	int res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	check_status(errMessage, res);
}
void DatabaseAccess::deleteUser(const User& user)
{
	char* errMessage = nullptr;
	const std::string sqlStatement = "DELETE FROM USERS WHERE ID = "+ std::to_string(user.getId()) +"AND NAME = " + user.getName() + ";";
	int res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	check_status(errMessage, res);
}
bool DatabaseAccess::doesUserExists(int userId)
{
	char* errMessage = nullptr;
	std::string sqlStatement = "SELECT * FROM USERS;";
	int res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_users, nullptr, &errMessage);
	check_status(errMessage, res);
	auto iter = m_users.begin();
	for (const auto& user : m_users) {
		if (user.getId() == userId) {
			m_users.clear();
			return true;
		}
	}
	m_users.clear();
	return false;
}
User DatabaseAccess::getUser(int userId)
{
	char* errMessage = nullptr;
	const char* sqlStatement = "SELECT * FROM USERS;";
	int res = sqlite3_exec(db, sqlStatement, callback_for_users, nullptr, &errMessage);
	check_status(errMessage, res);
	for (const auto& user : m_users) {
		if (user.getId() == userId) {
			return user;
		}
	}
	m_users.clear();
	throw ItemNotFoundException("User", userId);
}
int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	char* errMessage = nullptr;
	const char* sqlStatement = "SELECT * FROM ALBUMS;";
	int res = sqlite3_exec(db, sqlStatement, callback_for_albums, nullptr, &errMessage);
	check_status(errMessage, res);
	int albumsCount = 0;

	for (const auto& album : m_albums) {
		if (album.getOwnerId() == user.getId()) {
			++albumsCount;
		}
	}

	return albumsCount;
}
int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	char* errMessage = nullptr;
	std::string sqlStatement = "SELECT * FROM ALBUMS;";
	int res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_albums, nullptr, &errMessage);
	check_status(errMessage, res);
	 sqlStatement = "SELECT * FROM PICTURES;";
	 res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_pictures, nullptr, &errMessage);
	check_status(errMessage, res);
	int albumsCount = 0;

	for (const auto& album : m_albums)
	{
		const std::list<Picture>& pics = album.getPictures();
		for (const auto& picture : pics)
		{
			 sqlStatement = "SELECT * FROM TAGS WHERE PICTURE_ID =" + std::to_string(picture.getId()) + ";";
			 res = sqlite3_exec(db, sqlStatement.c_str(), callback_tags, nullptr, &errMessage);
			check_status(errMessage, res);
			if (is_taged)
			{
				albumsCount++;
				break;
			}
		}
	}
	return albumsCount;
}

int DatabaseAccess::countTagsOfUser(const User& user)
{
	char* errMessage = nullptr;
	 std::string sqlStatement = "SELECT * FROM ALBUMS;";
	int res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_albums, nullptr, &errMessage);
	check_status(errMessage, res);
	sqlStatement = "SELECT * FROM PICTURES;";
	res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_pictures, nullptr, &errMessage);
	check_status(errMessage, res);
	int tagsCount = 0;

	for (const auto& album : m_albums) 
	{
		const std::list<Picture>& pics = album.getPictures();

		for (const auto& picture : pics)
		{
			 sqlStatement = "SELECT * FROM TAGS WHERE  USER_ID =" + std::to_string(user.getId()) + ";";
			res = sqlite3_exec(db, sqlStatement.c_str(), callback_tags, nullptr, &errMessage);
			check_status(errMessage, res);
			if (is_taged)
			{
				tagsCount++;
			}
		}
	}

	return tagsCount;
}

float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	int albumsTaggedCount = countAlbumsTaggedOfUser(user);
	if (0 == albumsTaggedCount) {
		return 0;
	}

	return static_cast<float>(countTagsOfUser(user)) / albumsTaggedCount;
}

User DatabaseAccess::getTopTaggedUser()
{
	char* errMessage = nullptr;
	std::string sqlStatement = "SELECT  USER_ID, COUNT(PICTURE__ID) AS 'value_occurrence' FROM  TAGS GROUP BY  USER_ID ORDER BY 'value_occurrence' DESC  LIMIT  1;";
	int res = sqlite3_exec(db, sqlStatement.c_str(), callback_tags, nullptr, &errMessage);
	return getUser(m_id);
}

Picture DatabaseAccess::getTopTaggedPicture()
{
	int currentMax = -1;
	const Picture* mostTaggedPic = nullptr;
	getAlbums();
	char* errMessage = nullptr;
	const char* sqlStatement = "SELECT * FROM PICTURES;";
	int res = sqlite3_exec(db, sqlStatement, callback_for_pictures, nullptr, &errMessage);
	check_status(errMessage, res);
	for (const auto& album : m_albums)
	{
		const std::list<Picture>& pics = album.getPictures();
		for (const auto& picture : pics)
		{
			sqlStatement = "SELECT COUNT(PICTURE_ID) FROM TAGS WHERE PICTURE_ID = " + picture.getId();
			int res = sqlite3_exec(db, sqlStatement,callback_tags, nullptr, &errMessage);
			int tagsCount = m_tag_value;
			if (tagsCount == 0) {
				continue;
			}

			if (tagsCount <= currentMax) {
				continue;
			}

			mostTaggedPic = &picture;
			currentMax = tagsCount;
		}
	}
	if (nullptr == mostTaggedPic) {
		throw MyException("There isn't any tagged picture.");
	}

	return *mostTaggedPic;
}

std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	getAlbums();
	char* errMessage = nullptr;
	 std::string sqlStatement = "SELECT * FROM PICTURES;";
	int res = sqlite3_exec(db, sqlStatement.c_str(), callback_for_pictures, nullptr, &errMessage);
	check_status(errMessage, res);
	std::list<Picture> pictures;

	for (const auto& album : m_albums) {
		for (const auto& picture : album.getPictures()) {
			sqlStatement = "SELECT * FROM TAGS WHERE  USER_ID = " + std::to_string(user.getId()) + "AND PICTURE_ID = " + std::to_string(picture.getId()) + ";";
			int res = sqlite3_exec(db, sqlStatement.c_str(), callback_tags, nullptr, &errMessage);
			if (is_taged)
			{
				pictures.push_back(picture);
			}
		}
	}

	return pictures;
}