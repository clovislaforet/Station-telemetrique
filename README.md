# Station télémétrique
Projet de station de télémétrie longue portée utilisant des modules LoRa pour transmettre des données entre un système embarqué (type avion RC) et une station sol.

Technologies :
- Arduino Nano Every et Arduino Mega 2560
- ESP32-S3 IA Camera
- 2 modules LoRa SX1278
- GPS U-blox NEO-7M
- BME280
- UBEC 5V 3A et convertisseur de tension DC-DC 5V-3.3V

Architecture :
- "Station sol" composée de l'Arduino Mega qui gère le module LoRa, et utilisée via un PC
- "Station air" composée de l'Arduino Nano qui gère le GPS, le BME280 et l'autre module LoRa
- La station air contient aussi l'ESP32 mais celui-ci est indépendant du reste, on peut ou non l'utiliser

Utilisation :
- Câbler le système
- Flasher sur les deux Arduino leur code respectif, ainsi que sur l'ESP32
- Alimenter le système, importer et lancer le code Python sur le PC, et ouvrir la carte via le lien HTML généré dans le dossier

État du projet :
En cours de développement

Licence :
MIT License
