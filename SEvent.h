/** \file SEvent.h
 * \brief SEvent struct header file.
 * \details Enthaelt die Deklaration der Struktur SEvent.
 * \author Reimund
 * \date 2016
 */

#ifndef SEVENT_H_
#define SEVENT_H_

#include <string>

class CCabinController;

/** Eventbezeichner (Typ / Art des Events)
 */
enum EVENT_TYPE
{
    KEY_UP=100,							/**< Zugangspanel: Ruftaste "Nach oben"
    	wurde gedrueckt */
    KEY_DOWN=101,						/**< Zugangspanel: Ruftaste "Nach unten"
    	wurde gedrueckt */
		
    KEY_OPEN_DOORS=102,					/**< Kabinenpanel: Tuer-oeffnen-Taste wurde
    	gedrueckt */
    KEY_CLOSE_DOORS=103,				/**< Kabinenpanel: Tuer-Schliessen-Taste
    	wurde gedrueckt */
    KEY_FLOOR=104,						/**< Kabinenpanel: Eine der Stockwerk-
    	Wahltasten wurde gedrueckt */
    KEY_HIGH_PRIORITY=105,				/**< Kabinenpanel: Vorzugstaste wurde
    	gedrueckt */
		
	CABINDOOR_CLOSES=200,				/**< Kabinentuer beginnt zu schliessen */
	CABINDOOR_OPENS=201,				/**< Kabinentuer beginnt zu oeffnen */
	CABINDOOR_FULLY_CLOSED=202,			/**< Kabinentuer jetzt vollstaendig
	 	geschlossen */
	CABINDOOR_FULLY_OPEN=203,			/**< Kabinentuer jetzt vollstaendig
	  	geoeffnet */
	ENTRANCEDOOR_CLOSES=700,			/**< Zugangstuer beginnt zu schliessen */
	ENTRANCEDOOR_OPENS=701,				/**< Zugangstuer beginnt zu oeffnen */
	ENTRANCEDOOR_FULLY_CLOSED=702,		/**< Zugangstuer jetzt vollstaendig
	   	geschlossen */
	ENTRANCEDOOR_FULLY_OPEN=703,			/**< Zugangstuer jetzt vollstaendig
	  	geoeffnet */
		
	DOOR_BLOCKED=800,					/**< Lichtschranke wurde gerade blockiert */
	DOOR_UNBLOCKED=801,					/**< Lichtschranke jetzt wieder frei */

    REACHED_FLOOR=300,					/**< Naechstgelegenes Stockwerk hat sich
    	geaendert */

    MOTOR_STARTS_RUNNING=400,			/**< Der Motor beginnt, zu laufen */
    MOTOR_STOPPED=401,					/**< Der Motor hat gestoppt */

    TIMER=900,							/**< Der Timer ist abgelaufen */

    FIREALERT=901,						/**< Feueralarm hat ausgeloest */

    LOADSENSOR_OVERLOAD_DETECTED=600,	/**< Die maximale Zuladung wurde soeben ueberschritten */
    LOADSENSOR_OVERLOAD_RESOLVED=601,	/**< Die maximale Zuladung wird jetzt wieder eingehalten */

    UNDEFINED=999						/**< Undefiniertes Event */
};

/*! \struct SEvent
 * \brief Realisiert ein Event
 */
struct SEvent
{
    EVENT_TYPE m_eventType;				/**< Eventbezeichner */
    unsigned int m_additionalInfo;		/**< Zusaetzliche Information zum Event,
    	z. B. Adresse des Senders, Stockwerk etc. */
};

std::string evToStr(SEvent ev);
void printEvent(SEvent event, void* who);

#endif /* SEVENT_H_ */
