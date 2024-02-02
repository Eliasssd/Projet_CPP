# installation QT
Avant d'installer, assurez-vous de mettre à jour vos paquets en exécutant `sudo apt update`.

Ensuite pour installer QT5 faites `sudo apt-get install qtbase5-dev qtdeclarative5-dev`.

Aller dans le dossier test

Éxécutez `qmake test.pro`.

Ensuite faites `make` pour compiler le Front END

Une fois compilé vous pouvez lancer l'application avec `./test`
