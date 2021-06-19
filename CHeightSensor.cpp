/** \file CHeightSensor.cpp
 * \brief CHeightSensor class source file.
 * \details Enthaelt die Implementierung der Klasse CHeightSensor.
 * \author Reimund
 * \date 2016
 */

#include "CHeightSensor.h"
#include "CCabinController.h"
#include "SEvent.h"
#include <cmath>

/*! \fn CHeightSensor::CHeightSensor()
 *  \brief Konstruktor; Belegt die Attribute mit Standardwerten (0) und die
 *   Pointer mit 0
 */
CHeightSensor::CHeightSensor()
{
    m_pCabinController=0;
    m_pHeight = 0;
    m_lastFloor = 0;
}

/*! \fn void CHeightSensor::connect(CCabinController* pCabinController, float* pHeight)
 *  \brief Verbindet den Hoehensensor mit der physikalischen Groesse "Aufzughoehe"
 *   und dem Kabinencontroller
 *  \param pCabinController Pointer auf den Kabinencontroller
 *  \param pHeight Pointer auf die physikalische Groesse "Kabinenhoehe"
 */
void CHeightSensor::connect(CCabinController* pCabinController, float* pHeight)
{
    m_pCabinController=pCabinController;
    m_pHeight = pHeight;
}

/*! \fn float CHeightSensor::height()
 *  \brief Liefert die Kabinenhoehe zurueck
 *  \return Kabinenhoehe in m
 */
float CHeightSensor::height()
{
    return *m_pHeight;
}

/*! \fn unsigned short CHeightSensor::currentFloor()
 *  \brief Liefert die Nummer des der Kabine naechstgelegenen Stockwerks zurueck
 *  \return Nummer des naechstgelegenen Stockwerks
 */
unsigned short CHeightSensor::currentFloor()
{
    return round(((double)(*m_pHeight))/METERS_PER_FLOOR);
}

/*! \fn float CHeightSensor::metersPerFloor()
 *  \brief Liefert die geometrische Groesse "Meter pro Stockwerk" zurueck
 *  \return Geometrische Groesse "Meter pro Stockwerk"
 */
float CHeightSensor::metersPerFloor()
{
    return METERS_PER_FLOOR;
}

/*! \fn void CHeightSensor::work()
 *  \brief Laesst den Hoehensensor einen Simulationsschritt weiterarbeiten.
 *   Wird periodisch durch den Simulator aufgerufen
 */
void CHeightSensor::work()
{
    if(currentFloor()!=m_lastFloor)
    {
        SEvent newEvent;
        newEvent.m_eventType=REACHED_FLOOR;
        newEvent.m_additionalInfo=currentFloor();
        m_pCabinController->pushEvent(newEvent);
        m_lastFloor=newEvent.m_additionalInfo;
    }
}
