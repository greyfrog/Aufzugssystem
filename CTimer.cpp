/** \file CTimer.cpp
 * \brief CTimer class source file.
 * \details Enthaelt die Implementierung der Klasse CTimer.
 * \author Reimund
 * \date 2016
 */

#include "CTimer.h"
#include "CCabinController.h"
#include "SEvent.h"
#include "../simulation/CTime.h"
extern CTime t;	/**< Verweis auf globale Simulationszeit */


/*! \fn CTimer::CTimer()
 *  \brief Konstruktor; Belegt die Attribute mit Standardwerten (0) und die
 *   Pointer mit 0; Der Timer ist zu Beginn abgelaufen (sodass er nicht zu Beginn 
 *   von selbst loslaeuft)
 */
CTimer::CTimer()
{
	m_finished=true;
	m_delay=0;
	m_startTime=0;
	m_pCabinController=0;
}

/*! \fn void CTimer::connect(CCabinController* pCabinController)
 *  \brief Verbindet den Timer mit dem angegebenen Kabinencontroller
 *  \param pCabinController Pointer auf den Kabinencontroller
 */
void CTimer::connect(CCabinController* pCabinController)
{
	m_pCabinController=pCabinController;
}

/*! \fn void CTimer::set(unsigned long msec)
 *  \brief Startet den Timer, sodass er das TIMER-Event nach der angegebenen
 *   Zeit senden wird
 *  \param msec Zeit in Millisekunden, nach der der Timer ablaeuft (das Event sendet)
 */
void CTimer::set(unsigned long msec)
{
	m_delay=msec;
	m_startTime=t.get_ms();
	m_finished=false;
}

/*! \fn void CTimer::work()
 *  \brief Laesst den Timer einen Simulationsschritt weiterarbeiten.
 *   Wird periodisch durch den Simulator aufgerufen
 */
void CTimer::work()
{
	if(!m_finished)
	{
		if((t.get_ms()-m_startTime)>=m_delay)
		{
			m_finished=true;
			SEvent newEvent;
			newEvent.m_eventType=TIMER;
			newEvent.m_additionalInfo=0;
			m_pCabinController->pushEvent(newEvent);
		}
	}
}


