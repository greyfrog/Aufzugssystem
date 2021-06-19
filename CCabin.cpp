/** \file CCabin.cpp
 * \brief CCabin class source file.
 * \details Enthaelt die Implementierung der Klasse CCabin.
 * \author Reimund
 * \date 2016
 */

#include "CCabin.h"
#include "CCabinDoor.h"
#include "CDoor.h"
#include "CCabinPanel.h"

/*! \fn CCabin::CCabin()
 *  \brief Konstruktor. Belegt die Attribute mit Standardwerten (0, false). Erstellt die Tueren mit
 *  gegebene CCabinDoor::lAYOUT
 *
 */
CCabin::CCabin(CCabinDoor::LAYOUT layout)
{
    m_height = 0.0;
    m_isDriving=false;
    m_load=0.0;
    if(layout == CCabinDoor::LEFT)
    {
    	CCabinDoor CabinDoor(CDoor::LEFT);
    	m_cabinDoors[CabinDoor.side()] = CabinDoor;
    }
    if(layout == CCabinDoor::RIGHT)
    {
    	CCabinDoor CabinDoor(CDoor::RIGHT);
    	m_cabinDoors[CabinDoor.side()] = CabinDoor;
    }

    else
    {
    	CCabinDoor CabinDoorL(CDoor::LEFT);
    	m_cabinDoors[CabinDoorL.side()] = CabinDoorL;
    	CCabinDoor CabinDoorR(CDoor::RIGHT);
    	m_cabinDoors[CabinDoorR.side()] = CabinDoorR;
    }
}

/*! \fn void CCabin::setup()
 *  \brief Verbindet die kabineninternen Sensoren und Aktoren mit dem Kabinencontroller und den
 *   physikalischen Groessen der Kabine (z. B. Hoehensensor mit der Kabinenhoehe usw.) und
 *   verbindet im Gegenzug den Kabinencontroller mit den Sensoren und Aktoren.
 *   Erstellt soviel Stockwerk-Walhtastensginale wie die gegebene numFloor.
 */
void CCabin::setup(unsigned short numFloor)
{

    m_heightSensor.connect(&m_cabinController, &m_height);
    m_motor.connect(&m_cabinController, &m_height,  &m_isDriving);
    m_timer.connect(&m_cabinController);
    m_cabinPanel.connect(&m_cabinController);

    if(m_cabinDoors.find(CDoor::LEFT) != m_cabinDoors.end())
    {
    	m_cabinDoors[CDoor::LEFT].connect(&m_cabinController);
    	m_cabinController.connectCabinDoor(&m_cabinDoors[CDoor::LEFT]);
    }
    if(m_cabinDoors.find(CDoor::RIGHT) != m_cabinDoors.end())
    {
    	m_cabinDoors[CDoor::RIGHT].connect(&m_cabinController);
    	m_cabinController.connectCabinDoor(&m_cabinDoors[CDoor::RIGHT]);
    }


	m_cabinPanel.setNumFloors(numFloor);
    m_cabinController.connectHeightSensor(&m_heightSensor);
    m_cabinController.connectMotor(&m_motor);
    m_cabinController.connectTimer(&m_timer);
    m_cabinController.connectCabinPanel(&m_cabinPanel);

}

/*! \fn void CCabin::addEntrance(unsigned short FloorNumber, CDoor::SIDE entranceSide)
 *  \brief Erstellt eine Zugangstuer in dem gegebenen Stockwerk auf der Seite CDoor::SIDE entranceSide
 *	 Verbindet die Zugängen mit dem Kabinencontroller und verbindet im Gegenzug den Kabinencontroller mit den Zugänge.
 */
void CCabin::addEntrance(unsigned short FloorNumber, CDoor::SIDE entranceSide)
{
	CEntrance Entrance(FloorNumber,entranceSide);
	m_entrances[FloorNumber] = Entrance;
	m_entrances[FloorNumber].connect(&m_cabinController);
	m_cabinController.connectEntrance(&m_entrances[FloorNumber]);

}

/*! \fn CCabinControllerProxy CCabin::cabinControllerProxy()
 *  \brief Gibt ein Proxy-Objekt zurueck, das dem Systemcontroller gestattet,
 *  auf den Kabinencontroller zuzugreifen
 *  \return Ein Kabinencontroller-Proxy
 */
CCabinControllerProxy CCabin::cabinControllerProxy()
{
	return CCabinControllerProxy(&m_cabinController);
}
