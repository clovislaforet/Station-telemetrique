# Station télémétrique
*Projet de station de télémétrie longue portée utilisant des modules LoRa pour transmettre des données entre un système embarqué (type avion RC) et une station sol.*

<p align="center"><img src="https://github.com/user-attachments/assets/b3b350f1-5e75-4383-9223-99ee7aa91aef" width="600"/></p>

Vous trouverez ci-dessous quelques informations générales relatives au projet, mais un rapport complet et précis de celui-ci est disponible au téléchargement.

## État du projet
***En cours de développement...***

## Matériel
- Microcontroleurs : Arduino Nano Every et Arduino Mega 2560
- Module caméra : ESP32-S3 IA Camera
- Antennes longue portée : 2 modules LoRa SX1278
- GPS : U-blox NEO-7M
- Capteur de température, d'humidité et de pression : BME280
- UBEC 5V 3A et convertisseur de tension DC-DC 5V-3.3V

<table align="center">
  <tr>
    <td align="center"><b>ESP32-S3 IA CAM</b></td>
    <td align="center"><b>U-blox NEO-7M</b></td>
    <td align="center"><b>BME280</b></td>
    <td align="center"><b>LoRa SX1278</b></td>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/79d576df-7d1c-4194-9961-35b0c0bac1c4" width="200"></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/424f3e27-a695-4a6e-8e73-47f34aedb033" width="200"></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/dbe9c277-b218-4147-8888-864dabb42be2" width="200"></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/c46269ba-9891-492a-99f4-6581253c4417" width="200"></td>
  </tr>
</table>

## Architecture
- La "station air", composée de l'Arduino Nano, du GPS, du BME280 et de l'autre module LoRa, capte les informations télémétriques des capteurs et les envoie grâce au module LoRa.
- La "station sol", composée de l'Arduino Mega et d'un module LoRa, récupère les données transmises par la station air et les affiche de manière ergonomique via un PC.
- La station air contient aussi l'ESP32 avec sa caméra, mais celui-ci est indépendant du reste : on peut ou non l'allumer, voir ne pas l'installer du tout pour gagner de l'autonomie et du poids.

## Utilisation
1. Câbler le système, et fixer au modèle RC la caméra
2. Flasher sur les deux Arduino leur code respectif, ainsi que sur l'ESP32
3. Alimenter le système, et attendre un court moment la bonne initialisation des composants
4. Importer et lancer le code Python sur le PC
5. Observer les graphiques de télémétrie
6. Ouvrir la carte via le lien HTML généré dans le dossier

## Informations complémentaires
Masse totale de la station air : 234g

## Exemples
<table align="center">
  <tr>
    <td align="center"><b>Prototype (avec breadboard)</b></td>
    <td align="center"><b>Exemple d'intégration sur avion RC</b></td>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/5d0867f7-ace3-459e-99fc-64d67d1daf35" width="400"></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/17c0ac51-b55d-4dc8-84de-23ca42c5eb46" width="400"></td>
  </tr>
  <tr>
    <td align="center"><b>Exemple de visualisation sur la carte</b></td>
    <td align="center"><b>Exemple de graphiques des informations télémétriques</b></td>
  </tr>
    <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/32175f78-2bbd-4fbc-a6df-a8fbc5d14779" width="400"></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/2aeeec0a-edbc-4e5b-bbcc-e329dbfdbb26" width="400"></td>
  </tr>
</table>

## Licence
MIT License
