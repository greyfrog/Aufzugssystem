/** \file CEntranceDoor.cpp
 * \brief CEntranceDoor class source file.
 * \details Enthaelt die Implementierung der Klasse CEntranceDoor.
 * \author Reimund
 * \date 2016
 */

#include "CEntranceDoor.h"
#include "CCabinController.h"
#include "SEvent.h"

/*! \fn CEntranceDoor::CEntranceDoor()
 *  \brief Konstruktor; Belegt alle Attribute mit Standardwerten (0, false) und die Pointer
 *  mit 0; die Tuer ist zu Beginn geschlossen und es wird angenommen, sie befinde sich auf der 
 *  linken Seite
 */
CEntranceDoor::CEntranceDoor()
{
    m_pCabinController=0;
    m_state=IS_CLOSED;
    m_step=0;
    m_side=CDoor::LEFT;
    m_floorNumber=0;
}

/*! \fn void CEntranceDoor::connect(CCabinController* pCabinController)
 *  \brief Verbindet die Zugangstuer mit dem Kabinencontroller
 *  \param pCabinController Pointer auf den uebergeordneten Kabinencontroller
 */
void CEntranceDoor::connect(CCabinController* pCabinController)
{
    m_pCabinController = pCabinController;
}

/*! \fn void CEntranceDoor::setDoorSide(CDoor::SIDE side)
 *  \brief Legt die Tuerseite fest
 *  \param side Die Tuerseite
 */
void CEntranceDoor::setDoorSide(CDoor::SIDE side)
{
	m_side=side;
}

/*! \fn void CEntranceDoor::setFloorNumber(unsigned short floorNumber)
 *  \brief Legt die Stockwerksnummer fest, auf dem sich die Zugangstuer befindet
 *  \param floorNumber Die Stockwerksnummer
 */
void CEntranceDoor::setFloorNumber(unsigned short floorNumber)
{
	m_floorNumber=floorNumber;
}

/*! \fn void CEntranceDoor::evDoorOpens()
 *  \brief Loest das Event "Zugangstuer beginnt, zu oeffnen" aus
 */
void CEntranceDoor::evDoorOpens()
{
	SEvent newEvent;
	newEvent.m_eventType=ENTRANCEDOOR_OPENS;
	newEvent.m_additionalInfo=m_floorNumber;
	m_pCabinController->pushEvent(newEvent);
}

/*! \fn void CEntranceDoor::evDoorCloses()
 *  \brief Loest das Event "Zugangstuer beginnt, zu schliessen" aus
 */
void CEntranceDoor::evDoorCloses()
{
	SEvent newEvent;
	newEvent.m_eventType=ENTRANCEDOOR_CLOSES;
	newEvent.m_additionalInfo=m_floorNumber;
	m_pCabinController->pushEvent(newEvent);
}

/*! \fn void CEntranceDoor::evDoorFullyOpen()
 *  \brief Loest das Event "Zugangstuer jetzt vollstaendig offen" aus
 */
void CEntranceDoor::evDoorFullyOpen()
{
	SEvent newEvent;
	newEvent.m_eventType=ENTRANCEDOOR_FULLY_OPEN;
	newEvent.m_additionalInfo=m_floorNumber;
	m_pCabinController->pushEvent(newEvent);
}

/*! \fn void CEntranceDoor::evDoorFullyClosed()
 *  \brief Loest das Event "Zugangstuer jetzt vollstaendig geschlossen" aus
 */
void CEntranceDoor::evDoorFullyClosed()
{
	SEvent newEvent;
	newEvent.m_eventType=ENTRANCEDOOR_FULLY_CLOSED;
	newEvent.m_additionalInfo=m_floorNumber;
	m_pCabinController->pushEvent(newEvent);
}

/*! \fn CEntranceDoor::~CEntranceDoor()
 *  \brief Destruktor (leer)
 */
CEntranceDoor::~CEntranceDoor()
{

}
