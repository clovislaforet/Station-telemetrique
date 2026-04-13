### Station télémétrique - station air
### Clovis Laforêt et Clément Letranchant
### FIMI 2A ENSIL-ENSCI

### Code à lancer sur le PC une fois la station sol branchée au port série


import folium
import serial
import re
import os
import csv
import matplotlib.pyplot as plt
plt.ion()

PORT = "COM6"       # À mettre à jour selon le port série utilisé
BAUDRATE = 115200

points = []
ser = serial.Serial(PORT, BAUDRATE, timeout=1)

# Variables télémétriques
temperature = None
humidite = None
pression = None
altitude = None
rssi = None

# Création du fichier CSV
with open("telemetrie.csv", "w", newline="") as f:
    csv.writer(f).writerow(["temperature", "humidite", "pression", "altitude", "rssi"])

print("En attente de données GPS...")
print("Carte sauvegardée :", os.path.abspath("carte.html"))

while True:
    ligne = ser.readline().decode("utf-8", errors="ignore").strip()
    if ligne:
        print(ligne)
        
    # Lecture des données télémétriques
    if re.search(r"Temp", ligne):
        temperature = float(re.search(r"([-\d.]+)$", ligne).group(1))
    if re.search(r"Humid", ligne):
        humidite = float(re.search(r"([-\d.]+)$", ligne).group(1))
    if re.search(r"Pression", ligne):
        pression = float(re.search(r"([-\d.]+)$", ligne).group(1))
    if re.search(r"Altitude", ligne):
        altitude = float(re.search(r"([-\d.]+)$", ligne).group(1))
    if re.search(r"RSSI", ligne):
        rssi = float(re.search(r"([-\d.]+)$", ligne).group(1))
    
    # Ajout au fichier CSV
    if temperature is not None and humidite is not None and pression is not None and altitude is not None and rssi is not None:
        with open("telemetrie.csv", "a", newline="") as f:
            csv.writer(f).writerow([temperature, humidite, pression, altitude, rssi])
    
        # Lecture du CSV
        temperatures, humidites, pressions, altitudes, rssis = [], [], [], [], []
        with open("telemetrie.csv", "r") as f:
            reader = csv.DictReader(f)
            for ligne_csv in reader:
                temperatures.append(float(ligne_csv["temperature"]))
                humidites.append(float(ligne_csv["humidite"]))
                pressions.append(float(ligne_csv["pression"]))
                altitudes.append(float(ligne_csv["altitude"]))
                rssis.append(float(ligne_csv["rssi"]))
    
        # Tracé
        plt.clf()
        fig, axes = plt.subplots(2, 2, num=1)  # num=1 : toujours la même fenêtre
        
        axes[0,0].plot(temperatures, color="red")
        axes[0,0].set_title("Température (°C)")
        axes[0,0].set_ylim(min(temperatures)-1, max(temperatures)+1)
        
        ax_pres = axes[0,1]
        ax_alt = ax_pres.twinx()  # deuxième axe Y qui partage le même axe X
        ax_pres.plot(pressions, color="green", label="Pression (hPa)")
        ax_alt.plot(altitudes, color="purple", label="Altitude (m)")
        ax_pres.set_title("Pression & Altitude")
        ax_pres.set_ylabel("Pression (hPa)", color="green")
        ax_alt.set_ylabel("Altitude (m)", color="purple")
        ax_pres.tick_params(axis='y', labelcolor="green")
        ax_alt.tick_params(axis='y', labelcolor="purple")
        ax_pres.set_ylim(min(pressions) - 10, max(pressions) + 10)
        ax_alt.set_ylim(min(altitudes) - 5, max(altitudes) + 5)
        
        axes[1,0].plot(humidites, color="blue")
        axes[1,0].set_title("Humidité (%)")
        axes[1,0].set_ylim(min(humidites)-10, max(humidites)+10)
        
        axes[1,1].plot(rssis, color="orange")
        axes[1,1].set_title("RSSI (dBm)")
        axes[1,1].set_ylim(min(rssis)-10, max(rssis)+10)
        
        for ax in axes.flat:
            ax.set_xlabel("Paquets reçus")
            ax.grid(True)
        plt.tight_layout()
        plt.pause(0.1)
        print("")
        print("Graphiques mis à jour")
        print("")
        
        temperature = None
        humidite = None
        pression = None
        altitude = None
        rssi = None
    
    # Carte GPS
    # Format affiché par la station sol : "GPS : 48.856602, 2.352200"
    match = re.search(r"GPS\s*:\s*([-\d.]+),\s*([-\d.]+)", ligne)
    if match:
        lat = float(match.group(1))
        lon = float(match.group(2))
        if lat == 0.0 and lon == 0.0:
            print("GPS pas encore fixé...")
            continue
        points.append((lat, lon))
        print(f"Position reçue | Lat: {lat:.5f} | Lon: {lon:.5f}")
        carte = folium.Map(location=[lat, lon], tiles="CartoDB positron")
        folium.Marker(points[0], popup="Départ", icon=folium.Icon(color="green")).add_to(carte)
        folium.Marker([lat, lon], popup=f"Lat: {lat:.5f} | Lon: {lon:.5f}", icon=folium.Icon(color="red")).add_to(carte)
        folium.PolyLine(points, color="blue", weight=3).add_to(carte)
        lats = [p[0] for p in points]
        lons = [p[1] for p in points]
        carte.fit_bounds([[min(lats), min(lons)], [max(lats), max(lons)]])
        carte.save("carte.html")
        print("Carte à jour")

