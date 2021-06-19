/** \file CCabinDoor.cpp
 * \brief CCabinDoor class source file.
 * \details Enthaelt die Implementierung der Klasse CCabinDoor.
 * \author Reimund
 * \date 2016
 */

#include "CCabinDoor.h"
#include "CCabinController.h"
#include "SEvent.h"


/*! \fn CCabinDoor::CCabinDoor(CDoor::SIDE side)
 *  \brief Konstruktor. Legt die Tuerseite fest, intitialisiert alle anderen
 *  Attribute sowie die Pointer mit 0. Zu Beginn ist die Kabinentuer geschlossen 
 *  und nicht blockiert
 */
CCabinDoor::CCabinDoor(CDoor::SIDE side)
{
	m_unblocked = true;
	m_pCabinController = 0;
	m_side = side;
	m_state = IS_CLOSED;
	m_step = 0;
}

/*! \fn void CCabinDoor::connect(CCabinController* pCabinController)
 *  \brief Verbindet die Kabinentuer mit dem uebergeordneten Kabinencontroller
 *  \param pCabinController Pointer auf den uebergeordneten Kabinencontroller
 */
void CCabinDoor::connect(CCabinController* pCabinController)
{
	m_pCabinController = pCabinController;
}

/*! \fn CCabinDoor::~CCabinDoor()
 *  \brief Destruktor (leer)
 */
CCabinDoor::~CCabinDoor()
{

}

/*! \fn void CCabinDoor::unblocked()
 *  \brief Liefert zurueck, ob die Lichtschranke frei ist
 *  \return true, falls die Lichtschranke frei ist, false andernfalls
 */
bool CCabinDoor::unblocked()
{
	return m_unblocked;
}

/*! \fn void CCabinDoor::evDoorBlocked()
 *  \brief Loest das Event "Lichtschranke wurde gerade blockiert" aus
 */
void CCabinDoor::evDoorBlocked()
{
	m_unblocked = false;
	SEvent m_Event;
	m_Event.m_eventType = DOOR_BLOCKED;
	m_Event.m_additionalInfo=0;
	m_pCabinController->pushEvent(m_Event);
}

/*! \fn void CCabinDoor::evDoorBlocked()
 *  \brief Loest das Event "Lichtschranke jetzt wieder frei" aus
 */
void CCabinDoor::evDoorUnblocked()
{
	m_unblocked = true;
	SEvent m_Event;
	m_Event.m_eventType = DOOR_UNBLOCKED;
	m_Event.m_additionalInfo=0;
	m_pCabinController->pushEvent(m_Event);
}

/*! \fn void CCabinDoor::evDoorOpens()
 *  \brief Loest das Event "Kabinentuer oeffnet jetzt" aus
 */
void CCabinDoor::evDoorOpens()
{
	m_state = IS_OPENING;
	SEvent m_Event;
	m_Event.m_eventType = CABINDOOR_OPENS;
	m_Event.m_additionalInfo=0;
	m_pCabinController->pushEvent(m_Event);
}

/*! \fn void CCabinDoor::evDoorCloses()
 *  \brief Loest das Event "Kabinentuer schliesst jetzt" aus
 */
void CCabinDoor::evDoorCloses()
{
	m_state = IS_CLOSING;
	SEvent m_Event;
	m_Event.m_eventType = CABINDOOR_CLOSES;
	m_Event.m_additionalInfo=0;
	m_pCabinController->pushEvent(m_Event);
}

/*! \fn void CCabinDoor::evDoorFullyOpen()
 *  \brief Loest das Event "Kabinentuer jetzt vollstaendig offen" aus
 */
void CCabinDoor::evDoorFullyOpen()
{
	m_state = IS_OPEN;
	SEvent m_Event;
	m_Event.m_eventType = CABINDOOR_FULLY_OPEN;
	m_Event.m_additionalInfo=0;
	m_pCabinController->pushEvent(m_Event);
}

/*! \fn void CCabinDoor::evDoorFullyClosed()
 *  \brief Loest das Event "Kabinentuer jetzt vollstaendig geschlossen" aus
 */
void CCabinDoor::evDoorFullyClosed()
{
	m_state = IS_CLOSED;
	SEvent m_Event;
	m_Event.m_eventType = CABINDOOR_FULLY_CLOSED;
	m_Event.m_additionalInfo=0;
	m_pCabinController->pushEvent(m_Event);
}
