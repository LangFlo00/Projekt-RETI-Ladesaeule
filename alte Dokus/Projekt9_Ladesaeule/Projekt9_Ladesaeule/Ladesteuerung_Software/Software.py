# coding: utf8

# Importieren benötigter Bibliotheken 
import RPi.GPIO as GPIO
import MFRC522
import signal
import lcdlib
import os
from time import sleep
import time
#Bibliotheken Pymodbus
from pyModbusTCP.client import ModbusClient

#write_string
def write_stringf(stringliste,sleeptime):
	lcd.clear()
	n = len(stringliste)
	for x in range(0,n):
		lcd.set_cursor(x,0)
		lcd.write_string(stringliste[x])
	sleep(sleeptime)

Verbrauchsspeicher = []
Verbrauchsobjekt = []
#Status
state = 0
readystate = 0
accountstate = 1
loadstate = 2

firstready = 1
firstaccount = 1
firstload = 1

#Debug-Mode
debug = 0
debugtime = 1
fehler = []
# Wert 16 Bit 
fehler.append("Kabelabweisung 13 A und 20 A")
fehler.append("Kabelabweisung 13 A")
fehler.append("Ungültiger PP-Wert")
fehler.append("Ungültiger CP-Wert")
fehler.append("Status F wegen fehlender Verfügbarkeit der Ladestation")
fehler.append("Verriegelung")
fehler.append("Entriegelung")
fehler.append("LD ist während Verriegelung weggefallen")
fehler.append("Überstromabschaltung")
fehler.append("Kommunikationsproble Ladesteuerung - Messgerät bei aktivierter Überstromabschaltung")
fehler.append("Status D, Fahrzeug abgewiesen")
fehler.append("Schützfehler erkannt")
fehler.append("Fahrzeugseitig keine Diode im Control Pilot Kreis")

#Compare UID
lastUID = []
currentUID = []
flagload = 0
Verbrauch = 0

#ASCII Werte

#Key fuer NFC Authentifizierung
key = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]
#LCD-Adresse
ADDRESS = 0x3f
#Objekt erzeugen und Display initialisieren 
lcd = lcdlib.lcd(ADDRESS, 4, 16)

#Config EV Charge Control

#IP-Adresse EV Charge Control 
SERVER_HOST = "10.27.210.76"
#TCP-Port
SERVER_PORT = 502
#UID EV Charge Control
UNIT_ID = 180

ChargeControl = ModbusClient()
ChargeControl.host(SERVER_HOST)
ChargeControl.port(SERVER_PORT)
ChargeControl.unit_id(UNIT_ID)
ChargeControl.open()

if(debug):
	if not ChargeControl.host(SERVER_HOST):
		print("Host ERRROR")
	if not ChargeControl.port(SERVER_PORT):
		print("PORT ERROR")
	if ChargeControl.is_open():
		print("Verbindung steht")

#Bildschirm loeschen 
lcd.clear()

continue_reading = True

#Capture SIGINT for cleanup when the script is aborted
def end_read(signal, frame):
	global continue_reading
	continue_reading = False
	GPIO.cleanup()
	
#Hook the SIGINT
signal.signal(signal.SIGINT, end_read)

while continue_reading:
	if(debug):
		print("Fehlercodes:")
		fehlercodes = ChargeControl.read_input_registers(107,1)
		print(fehlercodes)
		fehlercodesint = fehlercodes[0]
		for x in range(0, 13):
			maske = (x & fehlercodesint)
			if (maske > 0):
				print(fehler[x])
		sleep(debugtime)
		
	if(debug):
		evstatus = ChargeControl.read_input_registers(100,1)
		evstatusint = evstatus[0]			
		if(evstatusint >= 65 and evstatusint <= 70):
			print(str(unichr(evstatusint)))
		else:
			print("ERROR EV-Status")


	#create an object of the class
	MIFAREReader = MFRC522.MFRC522()
	
	sleep(1)
	#Scan for Cards
	(status,TagType) = MIFAREReader.MFRC522_Request(MIFAREReader.PICC_REQIDL)
	
	#If a Card is found
	if status == MIFAREReader.MI_OK:
		#delete lcd Display
		lcd.clear()
		sleep(0.05)
		lcd.set_cursor(0,0)
		lcd.write_string("Card detected")
		
		#Test
		if debug:
			print("Card detected Raspberry")
		
		#Get the UID of the card
		(status,uid) = MIFAREReader.MFRC522_Anticoll()
		# Authentifizierung
        status = MIFAREReader.MFRC522_Auth(MIFAREReader.PICC_AUTHENT1A, 8, key, uid)
	
	#State-Machine
	
	# readystate
	if state == readystate:
		if(firstready):
			strings = ["Ladestation","frei"]
			write_stringf(strings,1)
			firstready = 0
			firstaccount = 1
			firstload = 1
			
		#Falls UID erkannt
		if status == MIFAREReader.MI_OK:
			timeout = time.time() + 120 
			
			lastUID = str(uid[0])+","+str(uid[1])+","+str(uid[2])+","+str(uid[3])
			
			# Display Anweisungen
			strings = ["Card detected","Login:","UID:",lastUID]
			write_stringf(strings,3)		
			state = accountstate
		elif status == MIFAREReader.MI_OK:
			if(debug):
				lcd.write_string("Authentifizierung Fehlgeschlagen")
				
			#strings = ["Card detected", "Login","fehlgeschlagen"]
			write_stringf(strings,2)
			firstready = 1
	# accountstate
	elif state == accountstate:
		if(debug):
			print("accountstate")
			sleep(debugtime)
		
		if(firstaccount):
			strings = ["Angemeldet als:",lastUID,"Bitte Fahrzeug","anstecken!"]
			write_stringf(strings, 1)
			firstaccount = 0
			firstload = 1
			firstready = 1
			
		timenow = time.time()
		# wait > 2 minutes
		if timenow > timeout:
			state = readystate
			strings = ["Keine Aktivität", "abgemeldet"]
			write_stringf(strings, 1)	
			
		
		# EV-Status muss C oder D sein
		evstatusint = evstatus[0]
		if evstatusint == 67 or evstatusint == 68:
			state = loadstate
			# Berechnung wird bei 0 begonnen
			#ChargeControl.write_single_registers(341,0);
			Logintime = time.strftime("%d.%m.%Y %H:%M:%S")
		else:
			if(debug):
				print(str(unichr(evstatusint)))
				
		#Falls UID erkannt
		if  status == MIFAREReader.MI_OK:
			currentUID = str(uid[0])+","+str(uid[1])+","+str(uid[2])+","+str(uid[3])
			if currentUID == lastUID:
				state = readystate
				strings = ["Abgemeldet:",lastUID]
				write_stringf(strings, 3)
			else:
				strings = ["Ladesäule","belegt"]
				write_stringf(strings,3)
				firstaccount = 1
	# loadstate					
	elif state == loadstate:
		
		if(firstload):
			#Freigabe zum Laden
			ChargeControl.write_single_coil(0x190,1)
			
			if(debug):
				print("loadstate")
				sleep(debugtime)
				if (ChargeControl.read_coils(0x190,1)):
					lcd.write_string("Laden")
				else:
					lcd.write_string("Freigabe fehlt")
			Verbrauch = ChargeControl.read_holding_registers(341,1)
			strings = ["Lädt...",lastUID,"Verbrauch:",Verbrauch[0]]
			write_stringf(strings, 1)
			firstload = 0
			firstaccount = 1
			firstready = 1
			
		evstatus = ChargeControl.read_input_registers(100,1)
		#Falls UID erkannt
		if status == MIFAREReader.MI_OK:
			currentUID = str(uid[0])+","+str(uid[1])+","+str(uid[2])+","+str(uid[3])
			if currentUID != lastUID:
				state = loadstate
				strings = ["Ladesäule","belegt"]
				write_stringf(strings,3)
				firstload = 1
			elif currentUID == lastUID:
				state = readystate
				ChargeControl.write_single_coil(0x190,1)
				Verbrauch =ChargeControl.read_holding_registers(341,1)
				strings = ["Abgemeldet","Gesamter","Verbrauch",Verbrauch[0]]
				write_stringf(strings, 3)
				
				#Datei Ausgabe
				Logouttime = time.strftime("%d.%m.%Y %H:%M:%S")
				Verbrauchsobjekt = [currentUID,Verbrauch[0],Logintime,Logouttime]
				Verbrauchsspeicher.append(Verbrauchsobjekt)
				if os.path.isfile('./savefile.csv'):
				existflag = 1
				else :
				existflag = 0
				file = open("savefile.csv","ab+")
				if existflag == 1 :
					file.write("UID:;Verbrauch;Geladen von;Geladen bis\n")
				js = open("savefile.js","w+")
				js.write("var datafile = \"")
				for x in Verbrauchsspeicher:
					for y in x:
						file.write(Verbrauchsobjekt(y))
						file.write(";")
						js.write(str(y))
						js.write("|")
					#Newline
					js.write("<")
					file.write("\n")
				
				js.write("\";")
				file.close()
				js.close()
				ChargeControl.write_single_coil(0x190,0)
				
		if(evstatus == 67 or evstatus == 68):
			state = loadstate
		elif status == MIFAREReader.MI_OK:
			state = readystate
			Verbrauch = ChargeControl.read_holding_registers(341,1)
			strings = ["Abgemeldet","Gesamter","Verbrauch:",Verbrauch[0]]
			write_stringf(strings,3)
			
			#Datei Ausgabe
			Logouttime = time.strftime("%d.%m.%Y %H:%M:%S")
			Verbrauchsobjekt = [currentUID,Verbrauch[],Logintime,Logouttime]
			Verbrauchsspeicher.append(Verbrauchsobjekt)
			if os.path.isfile('./savefile.csv'):
			existflag = 1
			else :
			existflag = 0
			
			file = open("savefile.csv","ab+")
			if existflag == 1 :
				file.write("UID:;Verbrauch;Geladen von;Geladen bis\n")
			
			js = open("savefile.js","w+")
			js.write("var datafile = \"")
			for x in Verbrauchsspeicher:
				for y in x:
					file.write(Verbrauchsobjekt(y))
					file.write(";")
					js.write(str(y))
					js.write("|")
				#Newline
				js.write("<")
				file.write("\n")
			js.write("\";")
			file.close()
			js.close()
			ChargeControl.write_single_coil(0x190,0)
ChargeControl.close()
			
			
			
				
				
				
				
	
