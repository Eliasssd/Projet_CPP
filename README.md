INSTALLATION DU SERVEUR BACKEND

1] Installation des dépendances nécessaires (les autres librairies sont de base fournies sur linux)

#commandes CROW et BOOST 1.69 (1.69 important sinon depreciation de certaines fonctions de crow)

install :
//sudo apt-get install g++ libboost-all-dev
git clone https://github.com/ipkn/crow.git
//sudo apt-get remove libboost-all-dev
wget https://boostorg.jfrog.io/artifactory/main/release/1.69.0/source/boost_1_69_0.tar.gz
tar -xzvf boost_1_69_0.tar.gz
cd boost_1_69_0/
./bootstrap.sh
sudo ./b2 install

chemin : usr/local/

#commande SQLITE3

sudo apt-get install sqlite3 libsqlite3-dev

LANCER LE SERVEUR AVEC : 

./compil.sh

EQUIVALENT DE CLEAN AVEC :

./compil.sh clean
