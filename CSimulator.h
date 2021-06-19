/** \file CSimulator.h
 * \brief CSimulator class header file.
 * \details Enthaelt die Deklaration der Klasse CSimulator.
 * \author Reimund
 * \date 2016
 */

#ifndef CSIMULATOR_H_
#define CSIMULATOR_H_

#include <winsock2.h>
#include <string>
#include "CTime.h"

#define LABOR1_VOR /**< Labortermin-Identifier, bitte immer anpassen! */
#define LABOR2_VOR
#define LABOR2_DURCH
#define LABOR3_VOR
#define LABOR3_DURCH
//#define DEACTIVATE_TCP    1	/**< Deaktivieren der TCP-Verbindung, damit keine
	// Visualisierung (Server-only-mode) */
//#define DBG_TCP           1		/**< Aktivieren der Konsolenausgabe fuer versendete
	// und empfangene strings ueber TCP */
#define RUN_LOCAL_CLIENT  1		/**< client.exe aus lokalem Folder starten lassen
	// (statt remote-Client)*/
#define TCP_PORT       10815	/**< Portnummer der TCP-Verbindung */
#define MAXRECV     1048576		/**< Groesse des char-Buffers zum empfangen von
	// Client-Events */
#define TIMEOUT_SEC       15	/**< Timeout fuer den Verbindungsversuch in s */

#include "../elevatorSystem/CElevatorSystem.h"

/*! \class CSimulator
 * \brief Uebernimmt die TCP-Verbindung zum Client, veranlasst das zeitliche
 *  Voranschreiten aller Aufzugsystem-Komponenten, sendet dem Client die zur
 *  Visualisierung des Aufzugsystems notwendigen Daten und empfaengt Events vom
 *  Client, um diese weiterzuverarbeiten
 */
class CSimulator
{
public:
    CSimulator(CElevatorSystem* pElevatorSystem);
    void run();
    void addPassenger(unsigned int startingFloorNo,
			  	  	  unsigned int destinationFloorNo,
			  	  	  unsigned int elevatorNo,
			  	  	  unsigned int activationTime_sec,
			  	  	  float weight,
			  	  	  bool usesHighPriorityKey=false);

private:
    /** Moegliche Aktionen von Client-Seite
     */
    enum CLIENT_ACTION_TYPE
    {
    	START = 1,					/**< Benutzer hat Start-Button gedrückt */
    	QUIT = 2,					/**< Client wurde geschlossen (Quit-Button, ESC...) */
    	KEY_UP = 100,				/**< Passagier hat "Nach-oben"-Taste gedrueckt */
    	KEY_DOWN = 101,				/**< Passagier hat "Nach-unten"-Taste gedrueckt */
    	ENTER_ELEVATOR = 300,		/**< Passagier hat den Aufzug betreten
    		(Passagiergewicht wird wirksam!) */
    	LEAVE_ELEVATOR = 301,		/**< Passagier hat den Aufzug verlassen
    		(Passagiergewicht entfaellt!) */
    	DOOR_BLOCKED = 400,			/**< Eine Lichtschranke wurde unterbrochen */
    	DOOR_UNBLOCKED = 401,		/**< Eine Lichtschranke ist jetzt wieder frei */
    	KEY_FLOOR = 200,			/**< Passagier hat eine Stockwerkwahltaste gedrueckt */
    	KEY_HIGH_PRIORITY = 201,	/**< Passagier hat Vorzugsschalter ausgeloest */
    	KEY_OPEN_DOORS = 202,		/**< Passagier hat "Tuer-Oeffnen"-Taste gedrueckt */
    	KEY_CLOSE_DOORS = 203,		/**< Passagier hat "Tuer-Schliessen"-Taste gedrueckt */
		FIREALARM_ACTIVATED = 911   /**< Passagier hat den Feueralarm ausgeloest */
    };

    bool isConnected();
    bool connectToClient(unsigned int port);
    void sendElevatorSystemConfiguration();
    void updateClient();
    void handleClientActions();
    void closeTcpConnection();
    void sendString(std::string str);

#if defined(LABOR2_VOR) || defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH) || defined(LABOR4_VOR) || defined(LABOR4_DURCH) || defined(LABOR5_VOR) || defined(LABOR5_GEWICHT) || defined(LABOR5_RICHTUNG)
    std::map<CDoor*, bool> m_retard;
    short m_retardStepCtr;
#endif

    fd_set readSet;					/**< Socket-Konfiguration */
    WSADATA wsa;					/**< Winsock-Daten */
    SOCKET s;						/**< Server-Socket */
    SOCKET new_socket;				/**< Client-Socket */
    struct sockaddr_in server;		/**< Server-Socketadresse */
    struct sockaddr_in client;		/**< Client-Socketadresse */
    int c;							/**< sizeof(struct sockaddr_in) */
    bool m_tcpConnectionOK;			/**< Gibt an, ob die TCP-Verbindung zum Client steht */
	bool isStarted;					/**< Wird auf "true" gesetzt, sobald im Client der
		"Start"-Button gedrueckt wurde; Ab dann werden Updates an den client gesendet */
    bool clientClosed;				/**< True, sobald der Client beendet hat (QUIT-Button,
		ESC etc.) */

    CElevatorSystem* m_pElevatorSystem;	/**< Pointer auf das Aufzugsystem */
};

#endif /* CSIMULATOR_H_ */
