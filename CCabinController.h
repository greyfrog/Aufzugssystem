/** \file CCabinController.h
 * \brief CCabinController class header file.
 * \details Enthaelt die Deklaration der Klasse CCabinController.
 * \author Reimund
 * \date 2016
 */

#ifndef CABINCONTROLLER_H_
#define CABINCONTROLLER_H_
#define DOOR_OPEN_DELAY 3000
#include "CEventQueue.h"
#include "map"
#include "CDoor.h"
#include "CEntrance.h"
#include "list"

class CSystemController;
class CMotor;
class CTimer;
class CHeightSensor;
class CCabinDoor;
class CCabinPanel;


/*! \class CCabinController
 * \brief Modelliert einen Kabinencontroller. Er ist verbunden mit den Komponenten
 *  der Kabine. Er steuert diese an, kann ihren Zustand abfragen und erhaelt Events
 *  von diesen. Er beherbergt die aufzugspezifische Zustandssteuerung.
 */
class CCabinController
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion
		dieser Klasse */

public:
    CCabinController();
    void connectSystemController(CSystemController* pSystemController);
    void connectTimer(CTimer* pTimer);
    void connectMotor(CMotor* pMotor);
    void connectHeightSensor(CHeightSensor* pHeightSensor);
    void connectCabinDoor(CCabinDoor* pCabinDoor);
    void connectCabinPanel(CCabinPanel* pCabinPannel);
    void connectEntrance(CEntrance* pEntrance);

    void openAllDoorsCurrentFloor();
    void closeAllDoorsCurrentFloor();
    void addToAscendingList(unsigned int floor);
    void addToDescendingList(unsigned int floor);
    bool allDoorsClosedCurrentFloor();
    bool allDoorsOpenCurrentFloor();
    void driveToFloor(unsigned short destinationFloorNumber);
    unsigned int currentFloor();

    void turnOffAllSignal();
    void signalFlashing();
    bool allSignalOff();
    bool allSignalOn();
    void pushEvent(SEvent event);
    void work();

private:
    enum STATE_TYPE 									// Zustand-Typen des Aufzugs
    {
    	IDLE_DOORS_CLOSED, 								/**< Startzustand, beide Listen leer, Aufzug steht,T�ren geschlossen*/
    	IDLE_DOORS_OPENING, 							/**< Beide Listen leer, Aufzug steht, T�ren �ffnen gerade */
    	IDLE_DOORS_OPEN, 								/**< Beide Listen leer, Aufzug steht,T�ren offen */
    	IDLE_DOORS_CLOSING, 							/**< Beide Listen leer, Aufzug steht, T�ren schlie�en gerade */
    	ASCENDING_DRIVING, 								/**< Aufsteigende Liste wird abgearbeitet, Aufzug f�hrt nach oben,T�ren geschlossen */
    	ASCENDING_DOORS_OPENING, 						/**< Aufsteigende Liste wird abgearbeitet, Aufzug steht,T�ren �ffnen gerade */
    	ASCENDING_DOORS_OPEN, 							/**< Aufsteigende Liste wird abgearbeitet, Aufzug steht, T�ren offen */
    	ASCENDING_DOORS_CLOSING, 						/**< Aufsteigende Liste wird abgearbeitet, Aufzug steht,T�ren schlie�en gerade */
    	DESCENDING_DRIVING, 							/**< Absteigende Liste wird abgearbeitet, Aufzug f�hrt nach unten,T�ren geschlossen */
    	DESCENDING_DOORS_OPENING, 						/**< Absteigende Liste wird abgearbeitet, Aufzug steht,T�ren �ffnen gerade */
    	DESCENDING_DOORS_OPEN, 							/**< Absteigende Liste wird abgearbeitet, Aufzug steht, T�ren offen */
    	DESCENDING_DOORS_CLOSING, 						/**< Absteigende Liste wird abgearbeitet, Aufzug steht,T�ren schlie�en gerade */
		FIREALARM_DRIVING,								/**< Feueralarm wird geloest waehrend des Fahrens */
		FIREALARM_FINAL									/**< Feueralarm wird geloest */
    };
    CSystemController* m_pSystemController; 			/**< Pointer auf den uebergeordneten Systemcontroller */
    CMotor* m_pMotor; 									/**< Pointer auf den Motor */
    CTimer* m_pTimer; 									/**< Pointer auf den Timer */
    CHeightSensor* m_pHeightSensor;						/**< Pointer auf den Hoehensensor */

    CEventQueue m_eventQueue;							/**< Queue der aufgelaufenen Events */
    STATE_TYPE m_state;									/**< Zustand des Aufzug */
    bool hochfahren;									/**< Fahrrichtung*/
    std::map<CDoor::SIDE, CCabinDoor*> m_pCabinDoors;	/**< Pointer auf Kabinentuer mit Tuerseite */
    std::map<unsigned short , CEntrance*> m_pEntrances;	/**< Pointer auf Zugang mit Stockwerknummer*/
    CCabinPanel* m_pCabinPanel;							/**< Pointer auf Kabinentableau*/
	bool cabinDoorChecked;								/**< Pr�ft, ob die Kabinentueren schon geschlossen ist*/
	bool entranceDoorChecked;							/**< Pr�ft, ob die Zugangtueren schon geschlossen ist*/
	bool first_Start;									/**< Pr�ft, ob die Aufzugsystem erst mal gestartet wird*/
	std::list<unsigned int> m_ascendingList;			/**< aufsteigende Liste beim Hochfahren anzufahrender Stockwerk*/
	std::list<unsigned int> m_descendingList;			/**< absteigende Liste beim Herunterfahren anzufahrender Stockwerk*/
};


#endif /* CABINCONTROLLER_H_ */
