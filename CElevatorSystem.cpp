/** \file CElevatorSystem.cpp
 * \brief CElevatorSystem class source file.
 * \details Enthaelt die Implementierung der Klasse CElevatorSystem.
 * \author Reimund
 * \date 2016
 */

#include "CElevatorSystem.h"
#include <iostream>


/*! \fn CElevatorSystem::CElevatorSystem(unsigned short numFloors)
 *  \brief Konstruktor; Legt die Anzahl der Stockwerke fuer das Aufzugsystem fest.
 *   MAX_FLOORS darf nicht ueberschritten und 2 nicht unterschritten werden
 */
CElevatorSystem::CElevatorSystem(unsigned short numFloors)
{
    if(numFloors>MAX_FLOORS)
    {
        std::cerr << "ERROR: CElevatorSystem(...): Max amount of floors is " << MAX_FLOORS
        	<< ", set to that\n" << std::flush;
        numFloors=MAX_FLOORS;
    }
    if(numFloors<2)
    {
    	std::cerr << "ERROR: CElevatorSystem(...): Min amount of floors is 2, set to that\n"
    		<< std::flush;
    	numFloors=2;
    }
    m_numFloors=numFloors;
}

/*! \fn void CElevatorSystem::addElevator(CCabinDoor::LAYOUT TurLayout)
 *  \brief Fuegt dem Aufzugsystem einen Aufzug
 *  hinzu und verbindet den Systemcontroller mit dem hinzugekommenen Aufzugcontroller
 *  \param TurLayout: link, recht oder beide Seite
 *
 */
void CElevatorSystem::addElevator(CCabinDoor::LAYOUT TurLayout)
{
	if((m_cabins.size()+1) <= MAX_ELEVATORS)
	{
		CCabin newCabin(TurLayout);
		m_cabins.push_back(newCabin);
		m_cabins.back().setup(m_numFloors);
		m_systemController.connectCabinController(m_cabins.back().cabinControllerProxy());
	}
	else
	{
		std::cerr << "ERROR: CElevatorSystem::addElevator() Max amount of elevators reached ("
		<< MAX_ELEVATORS << ")\n" << std::flush;
	}
	return;
}

/*! \fn void CElevatorSystem::addEntrance(unsigned short elevatorNumber, unsigned short floorNumber, CDoor::SIDE entranceside)
 * 	\brief fuer jeder Kabinne wird eine Zugangtuer in dem gegebenen Stockwerk(elevatornumber), mit der gegebenen Tuerseite
 */
void CElevatorSystem::addEntrance(unsigned short elevatorNumber, unsigned short floorNumber, CDoor::SIDE entranceside)
{
	std::list<CCabin>::iterator cabin;
	cabin = m_cabins.begin();
	std::advance(cabin,elevatorNumber);
	cabin->addEntrance(floorNumber,entranceside);
}
