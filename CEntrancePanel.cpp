/** \file CEntrancePanel.cpp
 * \brief CEntrancePanel class source file.
 * \details Enthaelt die Implementierung der Klasse CEntrancePanel.
 * \author Reimund
 * \date 2016
 */

#include "CEntrancePanel.h"
#include "CCabinController.h"
#include "SEvent.h"


/*! \fn CEntrancePanel::CEntrancePanel()
 *  \brief Konstruktor; Belegt alle Attribute mit Standardwerten (0, false) und die Pointer
 *   mit 0.
 */
CEntrancePanel::CEntrancePanel()
{
    m_pCabinController=0;
    m_upSignal=m_downSignal=false;
    m_currentFloorSignal=0;
    m_floorNumber=0;
}

/*! \fn void CEntrancePanel::connect(CCabinController* pCabinController)
 *  \brief Verbindet das Zugangspanel mit dem Kabinencontroller.
 *  \param pCabinController Pointer auf den Kabinencontroller
 */
void CEntrancePanel::connect(CCabinController* pCabinController)
{
    m_pCabinController=pCabinController;
}

/*! \fn void CEntrancePanel::setFloorNumber(unsigned short floorNumber)
 *  \brief Legt die Stockwerksnummer fest, auf dem sich das Zugangspanel befindet
 *  \param floorNumber Die Stockwerksnummer
 */
void CEntrancePanel::setFloorNumber(unsigned short floorNumber)
{
	m_floorNumber=floorNumber;
}

/*! \fn void CEntrancePanel::work()
 *  \brief Laesst das Zugangspanel einen Simulationsschritt weiterarbeiten.
 *   Wird periodisch durch den Simulator aufgerufen.
 */
void CEntrancePanel::work()
{

}

/*! \fn void CEntrancePanel::evKeyUp()
 *  \brief Loest das Event "Ruftaste nach oben betaetigt" aus.
 */
void CEntrancePanel::evKeyUp()
{

    SEvent newEvent;
    newEvent.m_eventType=KEY_UP;
    newEvent.m_additionalInfo=m_floorNumber;
    m_pCabinController->pushEvent(newEvent);
}

/*! \fn void CEntrancePanel::evKeyDown()
 *  \brief Loest das Event "Ruftaste nach unten betaetigt" aus.
 */
void CEntrancePanel::evKeyDown()
{
    SEvent newEvent;
    newEvent.m_eventType=KEY_DOWN;
    newEvent.m_additionalInfo=m_floorNumber;
    m_pCabinController->pushEvent(newEvent);
}


