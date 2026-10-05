Dieses Repository beinhaltet die Ergebnisse der Erweiterung einer Ladesäule an der HM für das Projekt RETI im SoSe25.



Im Projekt wurde ein Kartenleser an die Ladesäule angebunden, um eine Authentifizierung für angemeldete User zu implementieren. Für den Kartenleser wurde eine Gehäuse designt. Für die Authentifizierung wurde eine SQL-Datenbank verwendet und die Datenbankabfragen und in Node-red eingebaut. Die gesamte Steuerung der Ladesäule findet über Node-RED statt. Das Herz der Ladesäule bildet ein Raspberry Pi mit einem Linux-Betriebssystem.



In der Ladesäule werden zwei EVSE-Laderegler verwendet, sodass die Ladesäule zwei Autos gleichzeitig laden kann. Auf der dritten Phase liegen zwei Schuko-Stecker. Über Messgeräte von Janitza und Eastron werden Spannungs, -strom und Leistungsdaten gemessen und in der Datenbank geloggt.



Sowohl das Anwendermanual als auch die Entwicklerdokumentation sind im Repo zu finden.



Die Programmierung erfolgte über das grafische Flow-basierte Programmiertool Node-RED. Den Flow zum Projekt findet man im Ordner "Node-red".



Im Ordner "3D-Druck" findet man die CAD-Dateien für die Bildschirmhalterung und die Halterung für den Kartenleser.



Projekte müssen an der HM auf der Projektvernissage vorgestellt werden. Die beinhaltet einen 60-sekündigen Pitch und Ausstellungsstand. Dokumente dazu im Ordner "Projektvernissage".



Die Dokumentation über beispielsweise die Konfiguration des Kartenlesers und Anwendungen für den Kartenleser sind im Ordner "Kartenleser\_TWN4DevPack492\_1" zu finden, wobei der Inhalt öffentlich zugängliche Daten von ELATEC sind.



!\[Verkabelung der Ladesäule](Sonstiges/Bilder/Verkabelung\_Ladesaeule.png)



!\[Einpoliger Schaltplan der Ladesäule](Sonstiges/Bilder/Schaltplan\_einpolig.png)



!\[Komponenten der Ladesäule](Sonstiges/Bilder/Ladesaeule\_innen\_Komponenten.png)



!\[Außenansicht der Ladesäule](Sonstiges/Bilder/Ladesaeule\_außen.png)

