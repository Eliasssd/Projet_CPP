#!/bin/bash

if [ "$1" = "clean" ]; then
        rm *.o ib cs && echo "directory cleaned" || { echo "already cleaned"; exit 1; }
        exit 1
fi

FLAGS="-Wall -Werror"
echo "==============================PREPARING THE BACKEND================================================"
g++ -c servermanager.cpp bdd.cpp $FLAGS -ljsoncpp && echo "servermanager         compiled" || { echo "bdd failed"; exit 1; }
g++ -c bdd.cpp datetime.cpp hashcode.cpp servermanager.cpp $FLAGS -lsqlite3 -lpthread -ljsoncpp -lcrypt && echo "bdd                   compiled" || { echo "bdd failed"; exit 1; }
g++ -c sendmail.cpp $FLAGS && echo "sendmail              compiled           TWO FACTORS AUTHENTICATION IS ACTIVE !!!!!" || { echo "sendmail failed"; exit 1; }
g++ -c hashcode.cpp $FLAGS -lcrypt && echo "hashcode              compiled           PASSWORDS ARE SAFE !!!!!" || { echo "hashcode failed"; exit 1; }
g++ -c initBDDS.cpp $FLAGS -lsqlite3 -lpthread -ljsoncpp -lcrypt && echo "initBDDS              compiled" || { echo "initBDDS failed"; exit 1; }
g++ -o ib initBDDS.cpp bdd.cpp datetime.cpp hashcode.cpp servermanager.cpp $FLAGS -lsqlite3 -lpthread -ljsoncpp -lcrypt && echo "**database initializer created" || { echo "bdd object failed"; exit 1; }
echo "**Preparing the server (may take a few seconds...)"
g++ -o cs crowserver.cpp bdd.cpp sendmail.cpp datetime.cpp hashcode.cpp servermanager.cpp $FLAGS -I/home/user18/ProjetCPP/crow/include -lsqlite3 -lboost_system -lpthread -ljsoncpp -lcrypt && echo "crowserver            compiled" || { echo "crowserver failed"; exit 1; }
rm Database.db && echo "**database removed" || { echo "**nice place for a database found"; exit 1; }
echo "**Creation of clean database"
./ib && echo "**database created           SUPERUSER GETS OVERPOWERED !!!!!" || { echo "**database creation failed"; exit 1; }
echo "==============================DONE PREPARING THE BACKEND==========================================="
echo "Launch the server ? (y|n)"
read RESPONSE
if test $RESPONSE == 'y'
    then ./cs
fi
echo "==============================SERVER STOPPED======================================================="
