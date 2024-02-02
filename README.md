# installation QT
Avant d'installer, assurez-vous de mettre à jour vos paquets en exécutant `sudo apt update`.

Ensuite pour installer QT5 faites `sudo apt-get install qtbase5-dev qtdeclarative5-dev`.

Aller dans le dossier test

Éxécutez `qmake test.pro`.

Ensuite faites `make` pour compiler le Front END

Une fois compilé vous pouvez lancer l'application avec `./test`

# Lancer en Localhost
Si vous voulez lancer le Front end et le Back end sur le même ordinateur vous devrez changer l'adresse ip des requetes et mettre l'adresse ip de votre Local host à la place.
