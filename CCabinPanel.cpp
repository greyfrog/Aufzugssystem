/** \file CCabinPanel.cpp
 * \brief CCabinPanel class source file.
 * \details Enthaelt die Implementierung der Klasse CCabinPanel.
 * \author Reimund
 * \date 2016
 */

#include "CCabinPanel.h"
#include "CCabinController.h"


/*! \fn CCabinPanel::CCabinPanel()
 *  \brief Konstruktor; Initialisiert alle Attribute mit
 *  Standardwerten (0, false) und die Pointer zu 0. m_floorKeySignals
 *  wird nicht hier, sondern erst in setNumFloors(...) initialisiert, siehe dort
 */
CCabinPanel::CCabinPanel()
{
	m_pCabinController = 0;
	m_openDoorsKeySignal = 0;
	m_closeDoorsKeySignal= 0;
	m_highPriorityKeySignal = 0;
	m_overloadSignal = 0;
	m_ascendingSignal = 0;
    m_descendingSignal = 0;
    m_currentFloorSignal = 0;
}

/*! \fn CCabinPanel::setNumFloors(unsigned short numFloors)
 *  \brief Erstellt (resize) so viele Stockwerk-Wahltastensignale wie
 *  das Aufzugsystem Stockwerke hat (numFloors) und initialisiert diese mit false
 *  ( = AUS)
 *  \param numFloors Anzahl der Stockwerke des Aufzugsystems
 */
void CCabinPanel::setNumFloors(unsigned short numFloors)
{
	m_floorKeySignals.resize(numFloors);
	for(unsigned short i=0; i<m_floorKeySignals.size(); i++)
		m_floorKeySignals[i]=false;
}

/*! \fn void CCabinPanel::work()
 *  \brief Laesst das Kabinenpanel einen Simulationsschritt weiterarbeiten.
 *   Wird periodisch durch den Simulator aufgerufen
 */
void CCabinPanel::work()
{

}

/*! \fn void CCabinPanel::connect(CCabinController* pCabinController)
 *  \brief Verbindet das Kabinenpanel mit dem Kabinencontroller
 *  \param pCabinController Pointer auf den Kabinencontroller
 */
void CCabinPanel::connect(CCabinController* pCabinController)
{
	m_pCabinController = pCabinController;
}

/*! \fn void CCabinPanel::evKeyFloor(unsigned short floorNumber)
 *  \brief Loest das Event "Stockwerk-Wahltaste betaetigt" aus
 *  \param floorNumber Stockwerk-Nummer
 */
void CCabinPanel::evKeyFloor(unsigned short floorNumber)
{
    SEvent m_Event;
    m_Event.m_eventType = KEY_FLOOR;
    m_Event.m_additionalInfo = floorNumber;
    m_pCabinController->pushEvent(m_Event);
}

/*! \fn void CCabinPanel::evKeyOpenDoors()
 *  \brief Loest das Event "Tuer-Oeffnen-Taste betaetigt" aus
 */
void CCabinPanel::evKeyOpenDoors()
{
    SEvent m_Event;
    m_Event.m_eventType = KEY_OPEN_DOORS;
    m_Event.m_additionalInfo = 0;
    m_pCabinController->pushEvent(m_Event);
}

/*! \fn void CCabinPanel::evKeyCloseDoors()
 *  \brief Loest das Event "Tuer-Schliessen-Taste betaetigt" aus
 */
void CCabinPanel::evKeyCloseDoors()
{
    SEvent m_Event;
    m_Event.m_eventType = KEY_CLOSE_DOORS;
    m_Event.m_additionalInfo = 0;
    m_pCabinController->pushEvent(m_Event);
}

/*! \fn void CCabinPanel::evKeyHighPriority()
 *  \brief Loest das Event "Vorzugsschalter betaetigt" aus
 */
void CCabinPanel::evKeyHighPriority()
{
    SEvent m_Event;
    m_Event.m_eventType = KEY_HIGH_PRIORITY;
    m_Event.m_additionalInfo = 0;
    m_pCabinController->pushEvent(m_Event);
}




