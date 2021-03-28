#pragma once
#include "IDataAccess.h"
#include "MyException.h"
#include "ItemNotFoundException.h"


using std::cout; 
using std::endl;

class DatabaseAccess : public IDataAccess
{

public:
	DatabaseAccess() = default;
	 virtual ~DatabaseAccess() = default;

	// album related
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;
	void createAlbum(const Album& album) override;
	void deleteAlbum(const std::string& albumName, int userId) override;
	bool doesAlbumExists(const std::string& albumName, int userId) override;
	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& pAlbum) {};
	void printAlbums() override;

	// picture related
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override;
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;

	// user related
	void printUsers() override;
	void createUser(User& user) override;
	void deleteUser(const User& user) override;
	bool doesUserExists(int userId) override;
	User getUser(int userId) override;

	// user statistics
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	// queries
	User getTopTaggedUser() override;
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	 int get_next_id(const std::string table) override;
	 static void get_users_tag_in_picture(int picture_id);

	bool open() override;
	void close() override;
	void clear() {};

private:
	static sqlite3* db;

	//helpers
	auto getAlbumIfExists(const std::string& albumName);
	static bool check_status(char* errMessage, int res);
	void get_Albums_and_pictures();
	void  getUsers();
};
