/** \file CMotor.cpp
 * \brief CMotor class source file.
 * \details Enthaelt die Implementierung der Klasse CMotor.
 * \author Reimund
 * \date 2016
 */

#include "CMotor.h"
#include "CCabinController.h"
#include "SEvent.h"

/*! \fn CMotor::CMotor()
 *  \brief Konstruktor; Belegt die Attribute mit Standardwerten (0, false) und die
 *   Pointer mit 0; Der Zustand ist zu Beginn "stillstehend"
 */
CMotor::CMotor()
{
    m_pHeight = 0;
    m_pIsDriving=0;
    m_metersToLift=0.0;
    m_metersLifted=0.0;
    m_state=IS_STANDING_STILL;
    m_pCabinController=0;
}

/*! \fn void CMotor::connect(CCabinController* pCabinController,
     float* pHeight, bool* pIsDriving)
 *  \brief Verbindet den Motor mit den physikalischen Groessen "Kabinenhoehe" und
 *   "Aufzug faehrt" sowie mit dem Kabinencontroller
 *  \param pCabinController Pointer auf den Kabinencontroller
 *  \param pHeight Pointer auf die physikalische Groesse "Kabinenhoehe"
 *  \param pIsDriving Pointer auf die physikalische Groesse "Aufzug faehrt"
 */
void CMotor::connect(CCabinController* pCabinController,
    float* pHeight, bool* pIsDriving)
{
    m_pCabinController=pCabinController;
    m_pHeight=pHeight;
    m_pIsDriving=pIsDriving;
}

/*! \fn bool CMotor::lift(float meters)
 *  \brief Veranlasst den Motor, so lange zu laufen, bis die Kabine die angegebene 
 *	Hoehenstrecke zurueckgelegt hat
 *  \param meters Zurueckzulegende Hoehenstrecke der Kabine
 *  \return true, falls Motor in gueltigem Zustand
 */
bool CMotor::lift(float meters)
{
    switch(m_state)
    {
    case IS_STANDING_STILL:
    {
        evMotorStartsRunning();
        m_metersToLift=meters;
        m_metersLifted=0.0;
        (*m_pIsDriving)=true;
        m_state=IS_RUNNING;
        return true;
    }

    case IS_RUNNING:
    {
    	m_metersToLift=meters;
    	m_metersLifted=0.0;
        m_state=IS_RUNNING;
        return true;
    }

    default:
    	return false;
    }
    return false;
}

/*! \fn void CMotor::work()
 *  \brief Laesst den Motor einen Simulationsschritt weiterarbeiten.
 *   Wird periodisch durch den Simulator aufgerufen
 */
void CMotor::work()
{
    switch(m_state)
    {
    case IS_RUNNING:
    {
    	if(m_metersToLift > 0)
    	{
    		m_metersLifted += METERS_PER_STEP;
    		(*m_pHeight) += METERS_PER_STEP;
    		if(m_metersLifted >= m_metersToLift)
    		{
    			(*m_pHeight) -= (m_metersLifted-m_metersToLift);
    			m_metersLifted = m_metersToLift;
    			evMotorStopped();
    			m_state=IS_STANDING_STILL;
    			(*m_pIsDriving)=false;
    		}
    	}
    	else if(m_metersToLift < 0)
    	{
    		m_metersLifted -= METERS_PER_STEP;
        	(*m_pHeight) -= METERS_PER_STEP;
        	if(m_metersLifted <= m_metersToLift)
        	{
        		(*m_pHeight) -= (m_metersLifted-m_metersToLift);
        		m_metersLifted = m_metersToLift;
        		evMotorStopped();
        		m_state=IS_STANDING_STILL;
        		(*m_pIsDriving)=false;
        	}
    	}
    	else
    	{
    		evMotorStopped();
    		(*m_pIsDriving)=false;
    		m_state=IS_STANDING_STILL;
    	}
    	break;
    }
    case IS_STANDING_STILL:
        break;
    default:
        break;
    }
}

/*! \fn void CMotor::evMotorStartsRunning()
 *  \brief Loest das Event "Motor beginnt zu laufen" aus
 */
void CMotor::evMotorStartsRunning()
{
	SEvent newEvent;
	newEvent.m_eventType=MOTOR_STARTS_RUNNING;
	newEvent.m_additionalInfo=0;
	m_pCabinController->pushEvent(newEvent);
}

/*! \fn void CMotor::evMotorStopped()
 *  \brief Loest das Event "Motor hat soeben gestoppt" aus
 */
void CMotor::evMotorStopped()
{
	SEvent newEvent;
	newEvent.m_eventType=MOTOR_STOPPED;
	newEvent.m_additionalInfo=0;
	m_pCabinController->pushEvent(newEvent);
}
