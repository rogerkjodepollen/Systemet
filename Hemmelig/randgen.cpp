#include <iomanip>
#include <sstream>

#include <iostream>
#include <fstream>
#include <cstring>
#include <random>
#include <string>

#include "loke.h"

const int LOKEKEY = 13;
const int LOKEPHRASE = 7;


//std::string generatePrintableString(size_t length);
std::string generatePrintableHexString(size_t length);

std::string generateHexString(size_t length);

int main() {

	std::string random_string = generateHexString(256);

	std::string random_iv_string = generateHexString(128);

    std::string random_phrase = generatePrintableHexString(128);

	std::ofstream file("hemmelig.h");
    if (!file) {
        std::cerr << "Kan ikkje lage nøkkel!" << std::endl;
        return 1;
    }

    file << "#pragma once" << std::endl;
    file << "unsigned char key[256] = {" << random_string << "};" << std::endl;
    file << "unsigned char iv[128] = {" << random_iv_string << "};" << std::endl;
    file << "unsigned char phrase[128] = {" << random_phrase << "};" << std::endl;


    file.close();


    return 0;

}

std::string generateHexString(size_t length) {

    char* random_data = new char[length];

    std::random_device rd;  // Seed for randomness
    std::mt19937 gen(rd()); // Mersenne Twister PRNG
    std::uniform_int_distribution<size_t> dist(0, 255);

    std::ostringstream hexString;
    for (size_t i = 0; i < length; i++) {
        random_data[i] = (char)dist(gen);
    }

    loke((unsigned char*)random_data,length,LOKEKEY);

    for (size_t i = 0; i < length; i++) {

        unsigned char c = random_data[i];

        hexString << "0x";
        hexString << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)c;

        hexString << ",";

    }

    std::string result_str = hexString.str();

    result_str.pop_back();

    delete[] random_data;

    return result_str;
}


std::string generatePrintableHexString(size_t length) {
    const std::string printable_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    char* random_data = new char[length];

    std::random_device rd;  // Seed for randomness
    std::mt19937 gen(rd()); // Mersenne Twister PRNG
    std::uniform_int_distribution<size_t> dist(0, printable_chars.size() - 1);

    for (size_t i = 0; i < length; i++) {
        random_data[i] = printable_chars[dist(gen)];
    }

    random_data[length-1] = 0;

    loke((unsigned char*)random_data,length,LOKEPHRASE);

    std::ostringstream hexString;
    for (size_t i = 0; i < length; i++) {

        unsigned char c = random_data[i];

        hexString << "0x";
        hexString << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)c;

        hexString << ",";

    }

    std::string result_str = hexString.str();

    result_str.pop_back();

    delete[] random_data;

    return result_str;
}
