#pragma once

#include <string>
#include <vector>

typedef unsigned long long ullint;
typedef unsigned char uchar;

struct UserInfo {

    uchar* password;

	uchar* about;

};

struct cryptData {

    ullint length;

    unsigned char* encrypted_data;

};

class Server
{
	
public:

	Server();

	int startServer();

	void initialSetup();

	void secureEraseString(std::string& str);

	std::string generatePrintableString(size_t length);

	static void addUser(const char* username, const char* password,char* about);

	//static EVP_PKEY* loadPrivateKey();

	static void* handleClient(void* args);

//	static char* receiveEncryptedMessage(int clientSocket, EVP_PKEY* privateKey);

	static char* generateSalt(int salt_size);

	static UserInfo* findInfo(char* username,unsigned char* lokekey);

	static void secureErase(std::string& str);

	static void makeHash(const unsigned char* unhashed_password,int unhashed_password_length, unsigned char* hashed_password, int hashed_password_length);

	static cryptData* encrypt(const char* message);
	
	static unsigned char* decrypt(cryptData* in_data,unsigned long long size);
	
	static char* encryptPassword(char* hashed_password,char* salt);

};
;