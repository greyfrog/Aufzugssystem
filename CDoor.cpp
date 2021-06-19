/** \file CDoor.cpp
 * \brief CDoor class source file.
 * \details Enthaelt die Implementierung der abstrakten Klasse CDoor.
 * \author Reimund
 * \date 2016
 */

#include "CDoor.h"
#include "CCabinController.h"


/*! \fn void CDoor::work()
 *  \brief Laesst die Tuer einen Simulationsschritt weiterarbeiten;
 *   Wird periodisch durch den Simulator aufgerufen
 */
void CDoor::work()
{
    switch(m_state)
    {
    case IS_OPEN:    break;
    case IS_CLOSED:  break;
    case IS_CLOSING:
        m_step --;
        if(m_step<=0)
        {
			m_step=0;
            evDoorFullyClosed();
            m_state=IS_CLOSED;
        }
        break;
    case IS_OPENING:
        m_step ++;
        if(m_step>=OPENING_STEPS-1)
        {
			m_step=OPENING_STEPS-1;
        	evDoorFullyOpen();
            m_state=IS_OPEN;
        }
        break;
    }
}

/*! \fn bool CDoor::open()
 *  \brief Veranlasst die Tuer, zu oeffnen
 *  \return true, falls die Tuer zuvor geschlossen war oder gerade schloss,
 *   false andernfalls
 */
bool CDoor::open()
{

	switch(m_state)
	{
	case IS_OPEN:
		evDoorFullyOpen();
		return false;
	case IS_OPENING:
		evDoorOpens();
		return false;
	case IS_CLOSED:
		m_state = IS_OPENING;
		evDoorOpens();
		return true;
	case IS_CLOSING:
		m_state = IS_OPENING;
		evDoorOpens();
		return true;
	default:
		return false;
	}
}

/*! \fn bool CDoor::close()
 *  \brief Veranlasst die Tuer, zu schliessen
 *  \return true, falls die Tuer zuvor offen war oder gerade oeffnete,
 *   false andernfalls
 */
bool CDoor::close()
{
	switch(m_state)
	{
	case IS_CLOSED:
		evDoorFullyClosed();
		return false;
	case IS_CLOSING:
		evDoorCloses();
		return false;
	case IS_OPEN:
		m_state = IS_CLOSING;
		evDoorCloses();
		return true;
	case IS_OPENING:
		m_state = IS_CLOSING;
		evDoorCloses();
		return true;
	default:
		return false;
	}
	
}

/*! \fn CDoor::STATE CDoor::state()
 *  \brief Liefert den Zustand der Tuer zurueck
 *  \return Der Zustand der Tuer (geoeffnet, oeffnend, geschlossen oder schliessend)
 */
CDoor::STATE CDoor::state()
{
    return m_state;
}

/*! \fn float CDoor::openingPercentage()
 *  \brief Liefert die Oeffnungsweite der Tuer zurueck
 *  \return Oeffnungsweite in Prozent als Gleitkommawert zw. 0.0 und 1.0 (1.0 =
 *  vollstaendig geoeffnet)
 */
float CDoor::openingPercentage()
{
    return (float)m_step/((float)OPENING_STEPS-1.0);
}

/*! \fn CDoor::SIDE CDoor::side()
 *  \brief Liefert die Tuerseite zurueck
 *  \return Tuerseite (links oder rechts)
 */
CDoor::SIDE CDoor::side()
{
	return m_side;
}
