#pragma once
#include <iostream>
#include <string>
#include <crypt.h>
#include <random>

std::string generateSaltString(const int len);

std::string hash(const std::string password,const std::string salt,const bool genSalt ,const int saltLength,const std::string hashAlg);

const std::vector<float> generateCote(const int numberOfTeams);
