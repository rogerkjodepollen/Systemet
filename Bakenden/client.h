#pragma once

#include "defines.h"

#include <string>
#include <sodium.h>

#include <sys/prctl.h>
;
class Client
{

public:

	Client();

	~Client();

	int connectToServer(const char* username, const char* password);

	int loadCredentialFile();
	void loadPublicServerKey();


	void makeHash(const unsigned char* unhashed_password, int unhashed_password_length,unsigned char* hashed_password, int hashed_password_length);

	unsigned char* encryptPassword(const char* password,const char* salt);
	

	int sock;


};
