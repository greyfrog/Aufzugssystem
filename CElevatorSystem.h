/** \file CElevatorSystem.h
 * \brief CElevatorSystem class header file.
 * \details Enthaelt die Deklaration der Klasse CElevatorSystem.
 * \author Reimund
 * \date 2016
 */

#ifndef CELEVATORSYSTEM_H_
#define CELEVATORSYSTEM_H_

#include <list>
#include "CCabin.h"
#include "CSystemController.h"
#include "CCabinDoor.h"
#include "CDoor.h"


#define MAX_FLOORS    32	/**< Maximal zulaessige Anzahl Stockwerke */
#define MAX_ELEVATORS  5	/**< Maximal zulaessige Anzahl Aufzuege */

class CSimulator;

/*! \class CElevatorSystem
 * \brief Modelliert das Aufzugsystem. Diese uebergeordnete Klasse besteht aus
 *  den Aufzuegen (Kabinen) und dem uebergeordneten Systemcontroller
 */
class CElevatorSystem
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion
		dieser Klasse */

public:
	CElevatorSystem(unsigned short numFloors);

    void addElevator(CCabinDoor::LAYOUT TurLayout);
    void addEntrance(unsigned short elevatorNumber,
    				 unsigned short floorNumber,
					 CDoor::SIDE);

private:
    std::list<CCabin> m_cabins;				/**< Container, enthaelt die Aufzuege (Kabinen) */
    unsigned short m_numFloors;				/**< Anzahl der Stockwerke */
    CSystemController m_systemController;	/**< Uebergeordneter Aufzugsystemcontroller */
};

#endif /* CELEVATORSYSTEM_H_ */
