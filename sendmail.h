#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <fstream>

const std::string genCode(const int len);

int sendmail(const std::string receiverMail,const std::string code);
