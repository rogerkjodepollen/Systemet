
#include "server.h"

#include "hemmelig.h"

#include "loke.h"


#include <fstream>
#include <random>
#include <iostream>
#include <cstring>      // For memset()
#include <unistd.h>     // For close()
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <pthread.h>    // For multi-threading
#include <sys/mman.h>
#include <sodium.h>



static int noice_len = 2040;

#define MAX_PASSWORD_LENGTH 128

#define MAX_ABOUT_LENGTH 128

#define hash_BYTES 64

#define salt_BYTES 64


#define KEY_SIZE 4096  //???

//EVP_PKEY* Server::privateKey = nullptr;

struct ThreadArgs {
    int* clientSocket;
    unsigned char* key;
};

const int LOKEKEY = 13;
const int LOKEPHRASE = 7;




Server::Server() {

    mlock(key,256);
    mlock(iv,128);
    mlock(phrase,128);

    initialSetup();

   // loadPrivateKey();

}



void* Server::handleClient(void* args) {

    ThreadArgs* data = static_cast<ThreadArgs*>(args);

    int clientSock = *(int*)data->clientSocket;
    //delete (int*)clientSocket; // Free memory after casting

    char username[1024];
    memset(username, 0, sizeof(username));

       // Ask for username
    send(clientSock, "Hvem banker på min dør: \n", 25, 0);
    recv(clientSock, username, sizeof(username), 0);

    std::cout << "Vis deg verdig " << username << std::endl;

    loke(data->key,256,LOKEKEY);

    UserInfo* userinfo = findInfo(username,data->key);

    loke(data->key,256,LOKEKEY);

    if (userinfo != nullptr)
        std::cout << "Password is: " << userinfo->password << std::endl;

    if (userinfo == nullptr) {

        send(clientSock, "Vik fra meg du ukjende draug!", 27, 0);

        close(clientSock);

        return nullptr;

    }


    char* salt = generateSalt(salt_BYTES);

    char* recived_hash = new char[hash_BYTES];

    memset(recived_hash,0,hash_BYTES);

    send(clientSock, salt, salt_BYTES, 0);
    recv(clientSock, recived_hash, hash_BYTES, 0);

    std::cout << "pass: " << strlen((char*)userinfo->password) << std::endl;

    char* expected_hash = encryptPassword((char*)userinfo->password,salt);

    bool result = (memcmp(recived_hash,expected_hash,hash_BYTES));

    delete[] salt;
    delete[] recived_hash;
 
    
    if (result) {

        std::cout << "User logged inn!"  << std::endl;

        send(clientSock, "Velkommen inn!", 15, 0);

    } else {

        std::cout << "User rejected!" << std::endl;

        send(clientSock, "Vik fra meg!", 12, 0);

    }

    close(clientSock);

    return nullptr;
}


void Server::secureEraseString(std::string& str) {
    volatile char* p = const_cast<char*>(str.data());  // Get raw string memory
    std::memset(const_cast<char*>(str.data()), 0, str.size());  // Overwrite memory
    str.clear();  // Clear the string
    str.shrink_to_fit();  // Reduce capacity to free memory
}



void Server::initialSetup() {

    if (access("server.data", F_OK) == 0) {
        return;
    } else {

        std::string password;


        std::cout << "Skriv inn administartor passord!"  << std::endl;
        std::cin >> password;

        addUser("admin",password.c_str(),"Allherskaren");

        secureEraseString(password);

    }

    return;

}

std::string Server::generatePrintableString(size_t length) {
    const std::string printable_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#$%^&*-_=+|;:',.<>?/`~ ";

    std::random_device rd;  // Seed for randomness
    std::mt19937 gen(rd()); // Mersenne Twister PRNG
    std::uniform_int_distribution<size_t> dist(0, printable_chars.size() - 1);

    std::string randomString;
    randomString.reserve(length);
    for (size_t i = 0; i < length; i++) {
        randomString += printable_chars[dist(gen)];
    }
    return randomString;
}

void Server::addUser(const char* username, const char* password,const char* about) {

    std::cout << "Add user" << std::endl;

    char* hashed_password = (char*)malloc(crypto_pwhash_STRBYTES);

    makeHash((const unsigned char*)password,strlen(password),(unsigned char*)hashed_password,hash_BYTES);

    loke(key,256,LOKEKEY);

    cryptData* encrypted_password = encrypt(hashed_password);

    cryptData* encrypted_about = encrypt(about);


    loke(key,256,LOKEKEY);


    std::vector<char> write_data;

    write_data.clear();

    FILE* file = fopen("server.data", "ab");  // Open for writing (overwrite mode)
    if (!file) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    fseek(file,0,SEEK_END);


    int len = strlen(username) + 1;

    ullint ulllen;

    write_data.insert(
        write_data.end(),
        reinterpret_cast<const unsigned char*>(&len),
        reinterpret_cast<const unsigned char*>(&len) + sizeof(len)
    );

    write_data.insert(write_data.end(), username, username + len + 1);

    write_data.insert(
        write_data.end(),
        reinterpret_cast<const unsigned char*>(&encrypted_password->length),
        reinterpret_cast<const unsigned char*>(&encrypted_password->length) + sizeof(ullint)
    );


    write_data.insert(write_data.end(), encrypted_password->encrypted_data, encrypted_password->encrypted_data + encrypted_password->length);



    write_data.insert(
        write_data.end(),
        reinterpret_cast<const unsigned char*>(&encrypted_about->length),
        reinterpret_cast<const unsigned char*>(&encrypted_about->length) + sizeof(ullint)
    );



    write_data.insert(write_data.end(), encrypted_about->encrypted_data, encrypted_about->encrypted_data + encrypted_about->length);


    long jump_pos = write_data.size() + sizeof(long) + ftell(file);

    std::cout << "Write pos: " << jump_pos << std::endl;

    fwrite((char*)&jump_pos,sizeof(long),1,file);

    fwrite((char*)write_data.data(),1,write_data.size(),file);

    fclose(file);

    return;

}





char* Server::generateSalt(int salt_size) {
    char* salt = new char[salt_size];
    std::random_device rd;
    std::mt19937 gen(rd());  // Mersenne Twister RNG
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    for (size_t i = 0; i < salt_size; i++) {
        salt[i] = dist(gen);
    }
    return salt;
}




int Server::startServer() {


    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket!" << std::endl;
        return 1;
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt(SO_REUSEADDR) failed!" << std::endl;
        return 1;
    }

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Binding failed!" << std::endl;
        return 1;
    }

    // Start listening for connections (max queue: 5)
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listening failed!" << std::endl;
        return 1;
    }

    std::cout << "Server started. Listening on port 8080..." << std::endl;

    while (true) {
        // Accept client connections
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addr_size);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client!" << std::endl;
            continue;
        }

        unsigned char* keycopy = new unsigned char[256];

        mlock(keycopy,256);
        memcpy(keycopy,key,256);

        ThreadArgs* args = new ThreadArgs;
        args->clientSocket = &clientSocket;
        args->key = keycopy;

        // Create a new thread for each client
        pthread_t thread;
        int* clientSockPtr = new int(clientSocket);
        pthread_create(&thread, nullptr, handleClient, (void*)args);
        pthread_detach(thread);  // Automatically free resources when thread exits
    }

    close(serverSocket);

}


UserInfo* Server::findInfo(char* username,unsigned char* lokekey) {

    FILE* file = fopen("server.data", "rb");
    if (!file) {
        std::cerr << "Error opeing user file!" << std::endl;
        return nullptr;
    }



    fseek(file,0,SEEK_END);
    
    long file_length = ftell(file);

    fseek(file, 0, SEEK_SET);

    long jump = 0;

    int len = 0;


    std::string encrypted_username,encrypted_password;
    while (!feof(file)) {  // Read line by line

        fread((char*)&jump,sizeof(long),1,file);

        std::cout << "Foundjump: " << jump << std::endl;


        fread((char*)&len,sizeof(int),1,file);

        char* found_username = new char[len];

        fread(found_username,1,len,file);

        ullint longlongint;


        std::cout << "found: " << found_username << std::endl;

        if (strcmp(found_username,username) == 0) {

            UserInfo* userinfo = new UserInfo;


            cryptData* encrypted_password = new cryptData;

            fread((char*)&encrypted_password->length,sizeof(ullint),1,file);

            encrypted_password->encrypted_data = new uchar[encrypted_password->length];

            fread(encrypted_password->encrypted_data,1,encrypted_password->length,file);

            userinfo->password = new uchar[MAX_PASSWORD_LENGTH];

            userinfo->password = decrypt(encrypted_password,MAX_PASSWORD_LENGTH);

            cryptData* encrypted_about = new cryptData;

            fread((char*)&encrypted_about->length,sizeof(ullint),1,file);

            encrypted_about->encrypted_data = new uchar[encrypted_about->length];

            fread(encrypted_about->encrypted_data,1,encrypted_about->length,file);

            userinfo->about = new uchar[MAX_ABOUT_LENGTH];

            userinfo->about = decrypt(encrypted_about,MAX_ABOUT_LENGTH);

            fclose(file);

            return userinfo;

        }

        secureErase(encrypted_username);

        if (jump >= file_length) {

            fclose(file);

            return nullptr;


        }


        fseek(file,jump,SEEK_SET);

    }

    fclose(file);  // Close file

    return nullptr;

}


void Server::secureErase(std::string& str) {
    std::fill(str.begin(), str.end(), '\0');  // Overwrite memory with null bytes
    str.clear();  // Remove contents
    str.shrink_to_fit();  // Force reallocation
}

void Server::makeHash(const unsigned char* unhashed_password, int unhashed_password_length, unsigned char* hashed_password, int hashed_password_length) {

    crypto_generichash(hashed_password, hashed_password_length,
        unhashed_password, unhashed_password_length,
        NULL, 0);
}


cryptData* Server::encrypt(const char* message) {

    cryptData* new_data = new cryptData;

    new_data->encrypted_data = new unsigned char[crypto_aead_xchacha20poly1305_ietf_ABYTES + strlen(message) + 1];

    // Encrypt the message
    crypto_aead_xchacha20poly1305_ietf_encrypt(
        new_data->encrypted_data, &new_data->length,
        (unsigned char *)message, strlen(message)+1,
        NULL, 0, // Additional data (optional)
        NULL, iv, key);

    return new_data;
    
}

unsigned char* Server::decrypt(cryptData* in_data,ullint size) {

    unsigned char* message = new unsigned char[size];

    unsigned long long message_size;

    // Decrypt the message
    if (crypto_aead_xchacha20poly1305_ietf_decrypt(
            message, &message_size,
            NULL,
            in_data->encrypted_data, in_data->length,
            NULL, 0, // Additional data (optional)
            iv, key) != 0) {
        std:: cout << "Decryption failed!" << std::endl;
        return nullptr;
    }

    return message;

}

char* Server::encryptPassword(char* hashed_password,char* salt) {

    char* combined = new char[hash_BYTES + salt_BYTES];

    char* stage_2_hash = new char[crypto_generichash_BYTES];

    makeHash((const unsigned char*)hashed_password,hash_BYTES,(unsigned char*)stage_2_hash,hash_BYTES);

    memcpy(combined,salt,salt_BYTES);

    memcpy(&combined[salt_BYTES],stage_2_hash,hash_BYTES);

    char* stage_3_hash = new char[hash_BYTES];

    makeHash((const unsigned char*)combined,hash_BYTES + salt_BYTES, (unsigned char*)stage_3_hash,hash_BYTES);

    char* result = new char[hash_BYTES];

    for (int i = 0; i < hash_BYTES; i++) {
        
        result[i] = hashed_password[i] ^ stage_3_hash[i];
    
    }

    return result;

}
