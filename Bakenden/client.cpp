#include "client.h"


#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>

#include "loke.h"

#include "hemmelig.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

const char* VALID_USERNAME = "admin";
const char* VALID_PASSWORD = "jalla";

const int LOKEKEY = 11;
const int LOKEPHRASE = 17;

#define hash_BYTES 64

#define salt_BYTES 64


Client::Client() {

    mlock(key,256);
    mlock(iv,128);
    mlock(phrase,128);


    loadPublicServerKey();

}

Client::~Client() {

   
}


#include <iostream>
#include <cstring>
#include <arpa/inet.h> // for htonl



int Client::connectToServer(const char* username, const char* password) {

    int sock;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // 1️⃣ Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    // 2️⃣ Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // 3️⃣ Connect to server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    // 4️⃣ Receive prompt from server
    memset(buffer  , 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    std::cout << buffer << std::endl;

    send(sock, VALID_USERNAME, strlen(VALID_USERNAME), 0);

    char* salt = new char[salt_BYTES];

    memset(salt, 0, salt_BYTES);
    recv(sock, salt, salt_BYTES, 0);

    unsigned char* encryptedPassword = encryptPassword(password,salt);

    delete[] salt;

    send(sock, encryptedPassword, hash_BYTES, 0);



    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    std::cout << buffer << std::endl;  // Prints "Login successful!" or "Login failed"



   // const char* message = "Hello, secure world!"
   // sendEncryptedMessage(sock, publicKey, message);


    close(sock);
    return 0;



}

void Client::loadPublicServerKey() {

    FILE* file = fopen("server_public_key.pam", "rb");
    if (!file) {
        std::cerr << "Error opeing user file!" << std::endl;
        return;
    }

    rewind(file);

  //  publicKey = PEM_read_PUBKEY(file, nullptr, nullptr, nullptr);

    fclose(file);
}




void Client::makeHash(const unsigned char* unhashed_password, int unhashed_password_length,unsigned char* hashed_password, int hashed_password_length) {

    crypto_generichash(hashed_password, hashed_password_length,
        unhashed_password, unhashed_password_length,
        NULL, 0);
}




unsigned char* Client::encryptPassword(const char* password,const char* salt) {

    unsigned char* stage_1_hash = new unsigned char[hash_BYTES];

    makeHash((const unsigned char*)password,strlen(password),stage_1_hash,hash_BYTES);

    unsigned char* combined = new unsigned char[hash_BYTES + salt_BYTES];

    unsigned char* stage_2_hash = new unsigned char[hash_BYTES];

    makeHash((const unsigned char*)stage_1_hash,hash_BYTES,stage_2_hash,hash_BYTES);

    memcpy(combined,salt,salt_BYTES);

    memcpy(&combined[salt_BYTES],stage_2_hash,hash_BYTES);

    unsigned char* stage_3_hash = new unsigned char[hash_BYTES];

    makeHash((const unsigned char*)combined,hash_BYTES + salt_BYTES, stage_3_hash,hash_BYTES);

    unsigned char* result = new unsigned char[hash_BYTES];

    for (int i = 0; i < hash_BYTES; i++) {
        
        result[i] = stage_1_hash[i] ^ stage_3_hash[i];
    
    }

    return result;

}
