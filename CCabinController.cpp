/** \file CCabinController.cpp
 * \brief CCabinController class source file.
 * \details Enthaelt die Implementierung der Klasse CCabinController.
 * \author Reimund
 * \date 2016
 */
#include "iostream"
#include "algorithm"
#include "CCabinController.h"
#include "CSystemController.h"
#include "CCabinDoor.h"
#include "CMotor.h"
#include "CTimer.h"
#include "CHeightSensor.h"
#include "CEntrance.h"
#include "CCabinPanel.h"
#include "CEntrancePanel.h"
#include "../simulation/CTime.h"
extern CTime t;	/**< Verweis auf globale Simulationszeit */

/*! \fn void CCabinController::work()
 *  \brief Laesst den Kabinencontroller einen Simulationsschritt
 *   weiterarbeiten. Hier ist der Zustandsautomat fuer den einzelnen Aufzug
 *   zu implementieren. Die Methode wird periodisch durch den Simulator aufgerufen
 */
void CCabinController::work()
{
	// Neue Events abholen
    // Neue Events abholen
    std::vector<SEvent> events=m_eventQueue.popEvents();
    for(unsigned int i=0; i<events.size(); i++)
    {
        SEvent ev=events[i];// Event Nr. i abarbeiten

        // Wenn die Ruftaste oder die Stockwektaste gedrueckt werden, wird LED der Taste eingeschaltet
        if(ev.m_eventType == KEY_UP && m_state != FIREALARM_FINAL)
            m_pEntrances[ev.m_additionalInfo]->m_entrancePanel.m_upSignal = true;
        if(ev.m_eventType == KEY_DOWN && m_state != FIREALARM_FINAL)
            m_pEntrances[ev.m_additionalInfo]->m_entrancePanel.m_downSignal = true;
        if(ev.m_eventType == KEY_FLOOR && m_state != FIREALARM_FINAL)
            m_pCabinPanel->m_floorKeySignals[ev.m_additionalInfo] = true;


        switch(m_state)     //Je nach Zustaende
        {
        case IDLE_DOORS_CLOSED: // Aufzug in dem Wartezustand mit geschlossene Tueren
        {
            std::cout << "In state IDLE_DOORS_CLOSED: " << std::flush;
            switch(ev.m_eventType)  // Je nach Ereignisse
            {
            case KEY_FLOOR:   // oder
            case KEY_UP:      // oder (Wunschrichtung wird nicht beruecksichtigt)
            case KEY_DOWN:
            {
                // Wenn Signal kommt von gleichen Stockwerk dann öffnen alle Tueren in der Stockwerk
                if(ev.m_additionalInfo == currentFloor())
                {
                    openAllDoorsCurrentFloor();
                    m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                    m_state = IDLE_DOORS_OPENING;
                }
                // Wenn Signal kommt von hoeheren Stockwerk dann faehrt der Aufzug hoch, aufsteigende Signal an
                if(ev.m_additionalInfo > currentFloor())
                {
                    addToAscendingList(ev.m_additionalInfo);
                    m_pCabinPanel->m_ascendingSignal = true;
                    driveToFloor(ev.m_additionalInfo);
                    m_state = ASCENDING_DRIVING;
                }
                // Wenn Signal kommt von niedrigeren Stockwerk dann faehrt der Aufzug unten, absteigende Signal an
                if(ev.m_additionalInfo < currentFloor())
                {
                    addToDescendingList(ev.m_additionalInfo);
                    m_pCabinPanel->m_descendingSignal = true;
                    driveToFloor(ev.m_additionalInfo);
                    m_state = DESCENDING_DRIVING;
                }
                break;
            }
            case KEY_OPEN_DOORS:
            {
                // Mach alle Tueren auf, LED des openDoorKeySignal an und closeDoorKeySignal aus
                openAllDoorsCurrentFloor();
                m_pCabinPanel->m_closeDoorsKeySignal = false;
                m_pCabinPanel->m_openDoorsKeySignal = true;
                m_state = IDLE_DOORS_OPENING;
                break;
            }
            case FIREALERT:
            {
                // melde die FIREALERT an SystemController
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case IDLE_DOORS_OPENING: // Auzug in Wartezustand, Tueren offnen
        {
            std::cout << "In state IDLE_DOORS_OPENING: " << std::flush;
            switch(ev.m_eventType)
            {
            case KEY_FLOOR:
            {
                // Wenn Signal kommt von hoeheren Stockwerk, fuegt den Stockwerk von Signal in aufsteigende Liste
                // und macht alle Tueren zu
                if(ev.m_additionalInfo > currentFloor())
                {
                    addToAscendingList(ev.m_additionalInfo);
                    closeAllDoorsCurrentFloor();
                    m_state = ASCENDING_DOORS_CLOSING;
                }
                // Wenn Signal kommt von niedrigeren Stockwerk, fuegt den Stockwerk von Signal in absteigende Liste
                // und macht alle Tueren zu
                if(ev.m_additionalInfo < currentFloor())
                {
                    addToDescendingList(ev.m_additionalInfo);
                    closeAllDoorsCurrentFloor();
                    m_state = DESCENDING_DOORS_CLOSING;
                }
                // Wenn Signal kommt von gleichen Stockwerk, LED des Stockwerk leuchtet nicht.
                if(ev.m_additionalInfo == currentFloor())
                    m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                break;
            }
            case KEY_UP:       // oder
            case KEY_DOWN:
            {
                // Wenn Signal kommt von hoeheren Stockwerk, fuegt den Stockwerk von Signal in aufsteigende Liste
                if(ev.m_additionalInfo > currentFloor())
                {
                    addToAscendingList(ev.m_additionalInfo);
                    m_state = ASCENDING_DOORS_OPENING;
                }
                // Wenn Signal kommt von niedrigeren Stockwerk, fuegt den Stockwerk von Signal in absteigende Liste
                if(ev.m_additionalInfo < currentFloor())
                {
                    addToDescendingList(ev.m_additionalInfo);
                    m_state = DESCENDING_DOORS_OPENING;
                }
                break;
            }
            case KEY_CLOSE_DOORS:
            {
                // Macht alle Tueren zu. LED des closeDoorKeySignal an und openDoorKeySignal aus
                closeAllDoorsCurrentFloor();
                m_pCabinPanel->m_closeDoorsKeySignal = true;
                m_pCabinPanel->m_openDoorsKeySignal = false;
                m_state = IDLE_DOORS_CLOSING;
                break;
            }
            case CABINDOOR_FULLY_OPEN:    // oder
            case ENTRANCEDOOR_FULLY_OPEN:
            {
                // Wenn alle Turen geoeffnet sind. Set timer fuer 3s. Schaltet aufsteigende, absteigende Signal und openKeyDoorSignal aus
                if(allDoorsOpenCurrentFloor())
                {
                    m_pTimer->set(DOOR_OPEN_DELAY);
                    m_state = IDLE_DOORS_OPEN;
                    m_pEntrances[currentFloor()]->m_entrancePanel.m_upSignal = false;
                    m_pEntrances[currentFloor()]->m_entrancePanel.m_downSignal = false;
                    m_pCabinPanel->m_openDoorsKeySignal = false;
                }
                break;
            }
            case FIREALERT:
            {
                // melde die FIREALERT an SystemController
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case IDLE_DOORS_OPEN: // Auzug in Wartezustand mit geoeffneten Tueren
        {
            std::cout << "In state IDLE_DOORS_OPEN: " << std::flush;
            switch(ev.m_eventType)
            {
            case KEY_UP:      // oder
            case KEY_DOWN:
            {
                // Wenn Signal kommt von gleichen Stockwerk, dann setzen timer fuer 3s
                if(ev.m_additionalInfo == currentFloor())
                    m_pTimer->set(DOOR_OPEN_DELAY);
                // Wenn Signal kommt von hoeheren Stockwerk, fuegt den Stockwerk von Signal in aufsteigende Liste
                if(ev.m_additionalInfo > currentFloor())
                {
                    addToAscendingList(ev.m_additionalInfo);
                    m_state = ASCENDING_DOORS_OPEN;
                }
                // Wenn Signal kommt von hoeheren Stockwerk, fuegt den Stockwerk von Signal in aufsteigende Liste
                if(ev.m_additionalInfo < currentFloor())
                {
                    addToDescendingList(ev.m_additionalInfo);
                    m_state = DESCENDING_DOORS_OPEN;
                }
                break;
            }
            case KEY_FLOOR:
            {
                // Wenn Signal kommt von gleichen Stockwerk, dann setzen timer fuer 3s, Die Stockwerktaste leuchtet nicht
                if(ev.m_additionalInfo == currentFloor())
                {
                    m_pTimer->set(DOOR_OPEN_DELAY);
                    m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                }

                if(ev.m_additionalInfo > currentFloor())
                {
                    addToAscendingList(ev.m_additionalInfo);
                    closeAllDoorsCurrentFloor();
                    m_state = ASCENDING_DOORS_CLOSING;
                }
                if(ev.m_additionalInfo < currentFloor())
                {
                    addToDescendingList(ev.m_additionalInfo);
                    closeAllDoorsCurrentFloor();
                    m_state = DESCENDING_DOORS_CLOSING;
                }

                break;
            }
            case KEY_OPEN_DOORS:
            {
                m_pTimer->set(DOOR_OPEN_DELAY);
                break;
            }
            case KEY_CLOSE_DOORS:
                // LED des closeDoorKeySignal an und openDoorKeySignal aus , dann werden alle Tueren geschlossen
                m_pCabinPanel->m_closeDoorsKeySignal = true;
                m_pCabinPanel->m_openDoorsKeySignal = false;
            // nach 3s
            case TIMER:
            {
                // alle Tueren wurde geschlossen.
                closeAllDoorsCurrentFloor();
                m_state = IDLE_DOORS_CLOSING;
                break;
            }
            case FIREALERT:
            {
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case IDLE_DOORS_CLOSING: // Aufzug in Wartezustand und die Tueren schließen
        {
            std::cout << "In state IDLE_DOORS_CLOSING: " << std::flush;
            switch(ev.m_eventType)
            {
            case KEY_UP:          // oder
            case KEY_DOWN:        // oder
            case KEY_FLOOR:
            {
                if(ev.m_additionalInfo == currentFloor())
                {
                    openAllDoorsCurrentFloor();
                    m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                    m_state = IDLE_DOORS_OPENING;
                }
                if(ev.m_additionalInfo > currentFloor())
                {
                    addToAscendingList(ev.m_additionalInfo);
                    m_state = ASCENDING_DOORS_CLOSING;
                }
                if(ev.m_additionalInfo < currentFloor())
                {
                    addToDescendingList(ev.m_additionalInfo);
                    m_state = DESCENDING_DOORS_CLOSING;
                }
                break;
            }
            case KEY_OPEN_DOORS:
            {
                // alle Tueren werden geoeffnet. LED des openDoorKeySignal an und closeDoorKeySignal aus
                openAllDoorsCurrentFloor();
                m_pCabinPanel->m_closeDoorsKeySignal = false;
                m_pCabinPanel->m_openDoorsKeySignal = true;
                m_state = IDLE_DOORS_OPENING;
                break;
            }
            case CABINDOOR_FULLY_CLOSED:    // oder
            case ENTRANCEDOOR_FULLY_CLOSED:
            {
                // Wenn alle Tueren geschlossen sind, dann closeDoorsKeySignal zuruecksetzen
                if(allDoorsClosedCurrentFloor())
                {
                    m_pCabinPanel->m_closeDoorsKeySignal = false;
                    m_state = IDLE_DOORS_CLOSED;
                }
                break;
            }
            case FIREALERT:
            {
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case ASCENDING_DRIVING: // Der Aufzug faehrt gerade nach oben
        {
            std::cout << "In state ASCENDING_DRIVING: " << std::flush;
            switch(ev.m_eventType)
            {
            case REACHED_FLOOR:
            {
                // m_currentFloorSignal ( Panelanzeige) bekommt den Wert des Stockwerk jedes mal die Kabinensteuerung das Ereignis REACHED_FLOOR bekommt
                m_pCabinPanel->m_currentFloorSignal = ev.m_additionalInfo;
                for(unsigned short i=0; i<m_pCabinPanel->m_floorKeySignals.size(); i++)
                    m_pEntrances[i]->m_entrancePanel.m_currentFloorSignal =ev.m_additionalInfo;
                break;
            }
            case KEY_UP:      // oder
            case KEY_DOWN:    // oder
            case KEY_FLOOR:
            {
                 if(ev.m_additionalInfo < currentFloor())
                     addToDescendingList(ev.m_additionalInfo);

                 if(ev.m_additionalInfo > currentFloor())
                 {
                     addToAscendingList(ev.m_additionalInfo);
                     driveToFloor(m_ascendingList.front());
                 }
                 if(ev.m_additionalInfo == currentFloor())
                     m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                break;
            }
            case MOTOR_STOPPED:
            {
                // Stockwerksignal ausschalten, alle Tueren oeffnen dann der Stockwerk wird von die Liste geloesht, LED der Stockwerktaste
                // aus.
                openAllDoorsCurrentFloor();
                m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                m_ascendingList.pop_front();
                m_state = ASCENDING_DOORS_OPENING;
                break;
            }
            case FIREALERT:
            {
                // Melde die FIREALERT event an Systemsteuerung
                m_state = FIREALARM_DRIVING;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case ASCENDING_DOORS_OPENING: // Aufzug arbeiten mit der aufsteigenden List. Tueren oefnnen gerade
        {
            std::cout << "In state ASCENDING_DOORS_OPENING: " << std::flush;
            switch(ev.m_eventType)
            {
            case KEY_UP:     // oder
            case KEY_DOWN:   // oder
            case KEY_FLOOR:
            {
                 if(ev.m_additionalInfo > currentFloor())
                     addToAscendingList(ev.m_additionalInfo);

                 if(ev.m_additionalInfo < currentFloor())
                     addToDescendingList(ev.m_additionalInfo);

                 if(ev.m_additionalInfo == currentFloor())
                     m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                break;
            }
            case KEY_CLOSE_DOORS:
            {
                closeAllDoorsCurrentFloor();
                m_pCabinPanel->m_closeDoorsKeySignal = true;
                m_pCabinPanel->m_openDoorsKeySignal = false;
                m_state = ASCENDING_DOORS_CLOSING;

                break;
            }
            case CABINDOOR_FULLY_OPEN:    // oder
            case ENTRANCEDOOR_FULLY_OPEN:
            {
                // Wenn alle Tueren geoeffnet sind, setzen Timer fuer 3s. Dannach aufsteigendes, absteigendes Signal der Ruftasten
                // und openDoorsKeySignal nicht mehr leuchten.
                if(allDoorsOpenCurrentFloor())
                {
                    m_pTimer->set(DOOR_OPEN_DELAY);
                    m_state = ASCENDING_DOORS_OPEN;
                    m_pEntrances[currentFloor()]->m_entrancePanel.m_upSignal = false;
                    m_pEntrances[currentFloor()]->m_entrancePanel.m_downSignal = false;
                    m_pCabinPanel->m_openDoorsKeySignal = false;
                }

                break;
            }
            case FIREALERT:
            {
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case ASCENDING_DOORS_OPEN: // Aufzug arbeiten mit der aufsteigenden List. Tueren sind geoeffnet
        {
            std::cout << "In state ASCENDING_DOORS_OPEN: " << std::flush;
            switch(ev.m_eventType)
            {
            case KEY_UP:       // oder
            case KEY_DOWN:     // oder
            case KEY_FLOOR:
            {
                if(ev.m_additionalInfo == currentFloor())
                {
                    m_pTimer->set(DOOR_OPEN_DELAY);
                    m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                }

                if(ev.m_additionalInfo > currentFloor())
                    addToAscendingList(ev.m_additionalInfo);

                if(ev.m_additionalInfo < currentFloor())
                    addToDescendingList(ev.m_additionalInfo);

                break;
            }
            case KEY_OPEN_DOORS:
            {
                m_pTimer->set(DOOR_OPEN_DELAY);
                break;
            }
            case KEY_CLOSE_DOORS:
                m_pCabinPanel->m_closeDoorsKeySignal = true;
                m_pCabinPanel->m_openDoorsKeySignal = false;
            // nach 3s
            case TIMER:
            {
                closeAllDoorsCurrentFloor();
                m_state = ASCENDING_DOORS_CLOSING;
                break;
            }
            case FIREALERT:
            {
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case ASCENDING_DOORS_CLOSING: // Aufzug arbeiten mit der aufsteigenden List. Tueren schließen gerade
        {
            std::cout << "In state ASCENDING_DOORS_CLOSING: " << std::flush;
            switch(ev.m_eventType)
            {
            case KEY_UP:      // oder
            case KEY_DOWN:    // oder
            case KEY_FLOOR:
            {
                if(ev.m_additionalInfo == currentFloor())
                {
                    openAllDoorsCurrentFloor();
                    m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                    m_state = ASCENDING_DOORS_OPENING;
                }
                if(ev.m_additionalInfo > currentFloor())
                    addToAscendingList(ev.m_additionalInfo);

                if(ev.m_additionalInfo < currentFloor())
                    addToDescendingList(ev.m_additionalInfo);

                break;
            }
            case KEY_OPEN_DOORS:
            {
                openAllDoorsCurrentFloor();
                m_pCabinPanel->m_closeDoorsKeySignal = false;
                m_pCabinPanel->m_openDoorsKeySignal = true;
                m_state = ASCENDING_DOORS_OPENING;

                break;
            }
            case CABINDOOR_FULLY_CLOSED:    // oder
            case ENTRANCEDOOR_FULLY_CLOSED:
            {
                // aufsteigendes, absteigendes Signal und closeDoorsKeySignal zuruecksetzen
                m_pEntrances[currentFloor()]->m_entrancePanel.m_downSignal = false;
                m_pEntrances[currentFloor()]->m_entrancePanel.m_upSignal = false;
                m_pCabinPanel->m_closeDoorsKeySignal = false;
                // Wenn aufsteigende Liste leer ist, aufsteigendes Signal aus.
                // Wenn aufsteigende Liste leer ist, und absteigende List nicht leer ist, aufsteigendes Signal aus, absteigendes Signal leuchtet.
                if(m_ascendingList.empty())
                {
                    m_pCabinPanel->m_ascendingSignal = false;
                    if(!m_descendingList.empty())
                        m_pCabinPanel->m_descendingSignal = true;
                }
                // Wenn alle Tueren geschlossen sind und die aufsteigende List nicht leer ist, dann aufsteigendes Signal leuchtet und der Aufzug faehrt
                // zu den naeschte Stockwerk in aufsteigenden List.
                if(allDoorsClosedCurrentFloor() && !m_ascendingList.empty())
                {
                    m_pCabinPanel->m_ascendingSignal = true;
                    driveToFloor(m_ascendingList.front());
                    m_state = ASCENDING_DRIVING;
                }
                // Wenn alle Tueren geschlossen sind, die aufsteigende List leer ist und die absteigende Liste nicht leer ist, dann absteigendes Signal
                // leuchtet und der Aufzug faehrt zu den naeschte Stockwerk in aufsteigenden List.
                if(allDoorsClosedCurrentFloor() && m_ascendingList.empty() && !m_descendingList.empty())
                {
                    m_pCabinPanel->m_descendingSignal = true;
                    driveToFloor(m_descendingList.front());
                    m_state =  DESCENDING_DRIVING;
                }
                // Wenn alle Tueren geschlossen sind, die aufsteigende List leer ist und die absteigende Liste auch leer ist
                if(allDoorsClosedCurrentFloor() && m_ascendingList.empty() && m_descendingList.empty())
                    m_state = IDLE_DOORS_CLOSED;

                break;
            }
            case FIREALERT:
            {
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case DESCENDING_DRIVING: // Der Aufzug faehrt gerade nach unten
        {
            std::cout << "In state DESCENDING_DRIVING: " << std::flush;
            switch(ev.m_eventType)
            {
            case REACHED_FLOOR:
            {
                m_pCabinPanel->m_currentFloorSignal = ev.m_additionalInfo;
                for(unsigned short i=0; i<m_pCabinPanel->m_floorKeySignals.size(); i++)
                    m_pEntrances[i]->m_entrancePanel.m_currentFloorSignal =ev.m_additionalInfo;
                break;
            }
            case KEY_UP:        // oder
            case KEY_DOWN:      // oder
            case KEY_FLOOR:
            {
                 if(ev.m_additionalInfo > currentFloor())
                     addToAscendingList(ev.m_additionalInfo);

                 if(ev.m_additionalInfo < currentFloor())
                 {
                     addToDescendingList(ev.m_additionalInfo);
                     driveToFloor(m_descendingList.front());
                 }

                 if(ev.m_additionalInfo == currentFloor())
                     m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                break;
            }
            case MOTOR_STOPPED:
            {
                m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                openAllDoorsCurrentFloor();
                m_descendingList.pop_front();
                m_state = DESCENDING_DOORS_OPENING;
                break;
            }
            case FIREALERT:
            {
                m_state = FIREALARM_DRIVING;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case DESCENDING_DOORS_OPENING: // der Aufzug arbeiten mit der absteigende Liste, die Tueren oeffnen gerade
        {
            std::cout << "In state DESCENDING_DOORS_OPENING: " << std::flush;
            switch(ev.m_eventType)
            {
            case KEY_UP:       // oder
            case KEY_DOWN:     // oder
            case KEY_FLOOR:
            {
                 if(ev.m_additionalInfo > currentFloor())
                     addToAscendingList(ev.m_additionalInfo);

                 if(ev.m_additionalInfo < currentFloor())
                     addToDescendingList(ev.m_additionalInfo);

                 if(ev.m_additionalInfo == currentFloor())
                     m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                break;
            }
            case KEY_CLOSE_DOORS:
            {
                closeAllDoorsCurrentFloor();
                m_pCabinPanel->m_closeDoorsKeySignal = true;
                m_pCabinPanel->m_openDoorsKeySignal = false;
                m_state = DESCENDING_DOORS_CLOSING;
                break;
            }
            case CABINDOOR_FULLY_OPEN:    // oder
            case ENTRANCEDOOR_FULLY_OPEN:
            {
                if(allDoorsOpenCurrentFloor())
                {
                    m_pTimer->set(DOOR_OPEN_DELAY);
                    m_state = DESCENDING_DOORS_OPEN;
                    m_pEntrances[currentFloor()]->m_entrancePanel.m_upSignal = false;
                    m_pEntrances[currentFloor()]->m_entrancePanel.m_downSignal = false;
                    m_pCabinPanel->m_openDoorsKeySignal = false;
                }

                break;
            }
            case FIREALERT:
            {
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case DESCENDING_DOORS_OPEN: // der Aufzug arbeiten mit der absteigende Liste, die Tueren ist geoeffnet
        {
            std::cout << "In state DESCENDING_DOORS_OPEN: " << std::flush;
            switch(ev.m_eventType)
            {
            case KEY_UP:       // oder
            case KEY_DOWN:     // oder
            case KEY_FLOOR:
            {
                if(ev.m_additionalInfo == currentFloor())
                {
                    m_pTimer->set(DOOR_OPEN_DELAY);
                    m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                }

                if(ev.m_additionalInfo > currentFloor())
                    addToAscendingList(ev.m_additionalInfo);

                if(ev.m_additionalInfo < currentFloor())
                    addToDescendingList(ev.m_additionalInfo);

                break;
            }
            case KEY_OPEN_DOORS:
            {
                m_pTimer->set(DOOR_OPEN_DELAY);
                break;
            }
            case KEY_CLOSE_DOORS:            // oder
                m_pCabinPanel->m_closeDoorsKeySignal = true;
                m_pCabinPanel->m_openDoorsKeySignal = false;
            // nach 3s
            case TIMER:
            {
                closeAllDoorsCurrentFloor();
                m_state = DESCENDING_DOORS_CLOSING;
                break;
            }
            case FIREALERT:
            {
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case DESCENDING_DOORS_CLOSING: // der Aufzug arbeiten mit der absteigenden Liste, die Tueren schließen gerade
        {
            std::cout << "In state DESCENDING_DOORS_CLOSING: " << std::flush;
            switch(ev.m_eventType)
            {
            case KEY_UP:       // oder
            case KEY_DOWN:     // oder
            case KEY_FLOOR:
            {
                if(ev.m_additionalInfo == currentFloor())
                {
                    openAllDoorsCurrentFloor();
                    m_pCabinPanel->m_floorKeySignals[currentFloor()] = false;
                    m_state = DESCENDING_DOORS_OPENING;
                }
                if(ev.m_additionalInfo > currentFloor())
                    addToAscendingList(ev.m_additionalInfo);

                if(ev.m_additionalInfo < currentFloor())
                    addToDescendingList(ev.m_additionalInfo);

                break;
            }
            case KEY_OPEN_DOORS:
            {
                openAllDoorsCurrentFloor();
                m_pCabinPanel->m_closeDoorsKeySignal = false;
                m_pCabinPanel->m_openDoorsKeySignal = true;
                m_state = DESCENDING_DOORS_OPENING;

                break;
            }
            case CABINDOOR_FULLY_CLOSED:    // oder
            case ENTRANCEDOOR_FULLY_CLOSED:
            {
                m_pEntrances[currentFloor()]->m_entrancePanel.m_downSignal = false;
                m_pEntrances[currentFloor()]->m_entrancePanel.m_upSignal = false;
                m_pCabinPanel->m_closeDoorsKeySignal = false;
                // Wenn die absteigende Liste leer ist, das absteigende Signal aus
                // Wenn die absteigende Liste leer und aufsteigende Liste auch leer ist, das aufsteigende und absteigende Signal aus
                if(m_descendingList.empty())
                {
                    m_pCabinPanel->m_descendingSignal = false;
                    if(!m_ascendingList.empty())
                        m_pCabinPanel->m_ascendingSignal = false;
                }
                // Wenn alle Tueren geschlossen sind und die absteigende Liste nicht leer ist, dann arbeiten der Aufzug mit absteigende
                // Liste, absteigendes Signal an
                if(allDoorsClosedCurrentFloor() && !m_descendingList.empty())
                {
                    m_pCabinPanel->m_descendingSignal = true;
                    driveToFloor(m_descendingList.front());
                    m_state = DESCENDING_DRIVING;
                }
                // Wenn alle Tueren geschlossen sind, die absteigende Liste leer ist und aufsteigende Liste nicht leer ist, dann arbeiten
                // der Aufzug mit aufsteigende Liste, aufsteigendes Signal an
                if(allDoorsClosedCurrentFloor() && m_descendingList.empty() && !m_ascendingList.empty())
                {
                    m_pCabinPanel->m_ascendingSignal = true;
                    driveToFloor(m_ascendingList.front());
                    m_state =  ASCENDING_DRIVING;
                }
                // Wenn alle Tueren geschlossen sind, aufsteigende und absteigende Liste leer sind
                if(allDoorsClosedCurrentFloor() && m_ascendingList.empty() && m_descendingList.empty())
                    m_state = IDLE_DOORS_CLOSED;
                break;
            }
            case FIREALERT:
            {
                m_state = FIREALARM_FINAL;
                m_pSystemController->pushEvent(ev);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }

        case FIREALARM_DRIVING:         // Firealert-Ereigniss ausgeloest wird waehrend des Fahrens
        {
            std::cout << "In state FIREALARM_DRIVING: " << std::flush;
            // wenn Motor gestoppt ist, alle Tueren oeffen lassen
            if(ev.m_eventType == MOTOR_STOPPED)
            {
                openAllDoorsCurrentFloor();
                m_state = FIREALARM_FINAL;
            }
            break;
        }

        // Ich habe Ihr Kommentar nicht ganz versthen. Ich habe hier gemacht, dass es geprueft wird, ob die Tueren geschlossen oder nicht
        // wenn ja dann mach die Tueren auf. Wenn alle Signale ausgeschaltet oder eingeschaltet werden, dann setzen ich die Timer für 500ms
        // nach diese Zeit erhalten wir das Ereignis Timer und alle Signal blinken. Wenn es nicht der Fall ist, dann schaltet alle Signal aus
        // damit die Signale gleichzeitig blinken. Es funktioniert bei mir beim testen
        // Habe ich was false verstehen oder es ist schlimm wenn ich die Set-Methode in jedem Deurchgang der Work-Methode benutzen(und warum).
        // Danke fuer Ihre Feedback
        case FIREALARM_FINAL:
        {
            std::cout << "In state FIREALARM_FINAL: " << std::flush;
            // Wenn alle Tueren geschlossen ist, dann werden die Tueren geoeffnet
            if(allDoorsClosedCurrentFloor())
            {
                openAllDoorsCurrentFloor();
            }
            if(allDoorsOpenCurrentFloor())
            {
            	// Wenn alle Signale ein/ausgeschlatet, Setzen Timer fuer 500ms
            	if(allSignalOn() || allSignalOff())
            		m_pTimer->set(500);
            	// Wenn nicht, mach alle Signale aus
            	else
            		turnOffAllSignal();
            }
        	// 500ms abgelaufen ist, fangen Signale an zu blinken
        	if(ev.m_eventType == TIMER )
        		signalFlashing();
            break;
        }

        default: // ###################################################
        {
            break;
        }
        }
    }
}

/*! \fn CCabinController::CCabinController()
 *  \brief Konstruktor; Belegt alle Attribute mit Standardwerten (0, false), die Pointer
 *   mit 0 
 */
CCabinController::CCabinController()
{
	m_pMotor = 0;
	m_pTimer = 0;
	m_pHeightSensor = 0;
	m_pSystemController = 0;
	m_state = IDLE_DOORS_CLOSED;
	hochfahren = false;
	cabinDoorChecked = false;
	entranceDoorChecked = false;
	m_pCabinPanel = 0;
	first_Start = true;
}

/*! \fn void CCabinController::connectSystemController(CSystemController* pSystemController)
	\brief Verbindet den Kabinencontroller mit dem Systemcontroller
	\param pSystemController Pointer auf den zu verbindenden Systemcontroller
 */
void CCabinController::connectSystemController(CSystemController* pSystemController)
{
	m_pSystemController = pSystemController;
}

/*! \fn void CCabinController::connectHeightSensor(CHeightSensor* pHeightSensor)
	\brief Verbindet den Kabinencontroller mit dem Hoehensensor
	\param pHeightSensor Pointer auf den Hoehensensor
 */
void CCabinController::connectHeightSensor(CHeightSensor* pHeightSensor)
{
	m_pHeightSensor = pHeightSensor;
}

/*! \fn void CCabinController::connectMotor(CMotor* pMotor)
	\brief Verbindet den Kabinencontroller mit dem Motor
	\param pMotor Pointer auf den zu verbindenden Motor
 */
void CCabinController::connectMotor(CMotor* pMotor)
{
	m_pMotor=pMotor;
}

/*! \fn void CCabinController::connectTimer(CTimer* pTimer)
	\brief Verbindet den Kabinencontroller mit dem Timer
	\param pTimer Pointer auf den zu verbindenden Timer
 */
void CCabinController::connectTimer(CTimer* pTimer)
{
	m_pTimer=pTimer;
}

/*! \fn void CCabinController::connectCabinDoor(CCabinDoor* pCabinDoor)
	\brief Verbindet den Kabinencontroller mit den Kabinentueren
	\param pCabinDoor Pointer auf den zu verbindenden Kabintuer
 */
void CCabinController::connectCabinDoor(CCabinDoor* pCabinDoor)
{
	m_pCabinDoors[pCabinDoor->side()] = pCabinDoor;
}

/*! \fn void CCabinController::connectCabinPanel(CCabinPanel* pCabinPanel)
	\brief Verbindet den Kabinencontroller mit dem Kabinentableau
	\param pCabinPanel Pointer auf den zu verbindenden Kabinentableau
 */
void CCabinController::connectCabinPanel(CCabinPanel* pCabinPanel)
{
	m_pCabinPanel = pCabinPanel;
}

/*! \fn void CCabinController::connectEntrance(CEntrance* pEntrance)
	\brief Verbindet den Kabinencontroller mit dem Zugang
	\param pEntrance Pointer auf den zu verbindenden Zugang
 */
void CCabinController::connectEntrance(CEntrance* pEntrance)
{
	m_pEntrances[pEntrance->m_floorNumber] = pEntrance;
}

/*! \fn void CCabinController::driveToFloor(unsigned short destinationFloorNumber)
	\brief Veranlasst die Kabine, das angegebene Stockwerk anzufahren
	\param destinationFloorNumber Stockwerknummer, die angefahren werden soll
 */
void CCabinController::driveToFloor(unsigned short destinationFloorNumber)
{
	float meters_diff = ((m_pHeightSensor->metersPerFloor())*(destinationFloorNumber)) - m_pHeightSensor->height();
	m_pMotor->lift(meters_diff);
	return;
}

/*! \fn unsigned int CCabinController::currentFloor()
	\brief Liefert die der Kabine naechstgelegene Stockwerknummer zurueck
	\return Die der Kabine naechstgelegene Stockwerknummer
 */
unsigned int CCabinController::currentFloor()
{
	return m_pHeightSensor->currentFloor();
}

/*! \fn void CCabinController::pushEvent(SEvent event)
	\brief Fuegt dem Kabinencontroller ein Event hinzu
	\param event Das hinzuzufuegende Event
 */
void CCabinController::pushEvent(SEvent event)
{
	printEvent(event, (void*)this);
	m_eventQueue.pushEvent(event);
}

/*! \fn void CCabinController::openAllDoorsCurrentFloor()
	\brief machen alle Tueren auf
 */
void CCabinController::openAllDoorsCurrentFloor()
{
	m_pEntrances[currentFloor()]->m_entranceDoor.open();
	m_pCabinDoors[m_pEntrances[currentFloor()]->m_entranceDoor.side()]->open();
}

/*! \fn void CCabinController::closeAllDoorsCurrentFloor()
	\brief machen alle Tueren zu
 */
void CCabinController::closeAllDoorsCurrentFloor()
{
	m_pEntrances[currentFloor()]->m_entranceDoor.close();
	m_pCabinDoors[m_pEntrances[currentFloor()]->m_entranceDoor.side()]->close();
}

/*! \fn void CCabinController::addToAscendingList(unsigned int floor)
	\brief Wenn der Stockwerk noch nicht in Liste eigetragen ist, er wird eingetragen und ganze Liste wird
			aufsteigend sortiert
	\param floor der Stockwerk in der aufsteigenden Liste hinzugefuegt wird
 */
void CCabinController::addToAscendingList(unsigned int floor)
{
	if(std::find(m_ascendingList.begin(),m_ascendingList.end(),floor)==m_ascendingList.end()
			&& std::find(m_descendingList.begin(),m_descendingList.end(),floor)==m_descendingList.end())
	{
		m_ascendingList.push_back(floor);
		m_ascendingList.sort();
	}

}

/*! \fn void CCabinController::addToDescendingList(unsigned int floor)
	\brief Wenn der Stockwerk noch nicht in Liste eigetragen ist, er wird eingetragen und ganze Liste wird
			absteigend sortiert
	\param floor der Stockwerk in der aufsteigenden Liste hinzugefuegt wird
 */
void CCabinController::addToDescendingList(unsigned int floor)
{
	if(std::find(m_ascendingList.begin(),m_ascendingList.end(),floor)==m_ascendingList.end()
			&& std::find(m_descendingList.begin(),m_descendingList.end(),floor)==m_descendingList.end())
	{
		m_descendingList.push_back(floor);
		m_descendingList.sort(std::greater<unsigned int>());
	}

}

/*! \fn bool CCabinController::allDoorsOpenCurrentFloor()
	\brief Wenn alle Tueren in dem aktuellen Stockwerk geoeffnet sind, liefert true. Falls nicht liefert false
 */
bool CCabinController::allDoorsOpenCurrentFloor()
{
	if( m_pEntrances[currentFloor()]->m_entranceDoor.state() == CDoor::IS_OPEN
			&& m_pCabinDoors[m_pEntrances[currentFloor()]->m_entranceDoor.side()]->state() == CDoor::IS_OPEN )
	{
		return true;
	}
	return false;
}

/*! \fn bool CCabinController::allDoorsClosedCurrentFloor()
	\brief Wenn alle Tueren in dem aktuellen Stockwerk geschlossen sind, liefert true. Falls nicht liefert false
 */
bool CCabinController::allDoorsClosedCurrentFloor()
{
	if( m_pEntrances[currentFloor()]->m_entranceDoor.state() == CDoor::IS_CLOSED
			&& m_pCabinDoors[m_pEntrances[currentFloor()]->m_entranceDoor.side()]->state() == CDoor::IS_CLOSED )
	{
		return true;
	}
	return false;
}

/*! \fn void CCabinController::turnOffAllSignal()
	\brief alle Signale werden ausgeschaltet
 */
void CCabinController::turnOffAllSignal()
{
	m_pCabinPanel->m_ascendingSignal = false;
	m_pCabinPanel->m_closeDoorsKeySignal = false;
	m_pCabinPanel->m_descendingSignal = false;
	m_pCabinPanel->m_highPriorityKeySignal = false;
	m_pCabinPanel->m_overloadSignal = false;
	m_pCabinPanel->m_openDoorsKeySignal = false;
	for(unsigned short i=0; i<m_pCabinPanel->m_floorKeySignals.size(); i++)
	{
		m_pCabinPanel->m_floorKeySignals[i]= false;
		m_pEntrances[i]->m_entrancePanel.m_downSignal = false;
		m_pEntrances[i]->m_entrancePanel.m_upSignal = false;
	}
}

/*! \fn void CCabinController::signalFlashing()
	\brief alle Signale werden geblinkt
 */
void CCabinController::signalFlashing()
{
	m_pCabinPanel->m_ascendingSignal = !m_pCabinPanel->m_ascendingSignal;
	m_pCabinPanel->m_closeDoorsKeySignal = !m_pCabinPanel->m_closeDoorsKeySignal;
	m_pCabinPanel->m_descendingSignal = !m_pCabinPanel->m_descendingSignal;
	m_pCabinPanel->m_highPriorityKeySignal = !m_pCabinPanel->m_highPriorityKeySignal;
	m_pCabinPanel->m_overloadSignal = !m_pCabinPanel->m_overloadSignal;
	m_pCabinPanel->m_openDoorsKeySignal = !m_pCabinPanel->m_openDoorsKeySignal;
	for(unsigned short i=0; i<m_pCabinPanel->m_floorKeySignals.size(); i++)
	{
		m_pCabinPanel->m_floorKeySignals[i]= !m_pCabinPanel->m_floorKeySignals[i];
		m_pEntrances[i]->m_entrancePanel.m_downSignal = !m_pEntrances[i]->m_entrancePanel.m_downSignal;
		m_pEntrances[i]->m_entrancePanel.m_upSignal = !m_pEntrances[i]->m_entrancePanel.m_upSignal;
	}
}

/*! \fn bool CCabinController::allSignalOff()
	\brief Wenn alle Signale aus, liefert true. Falls nicht, liefert false
 */
bool CCabinController::allSignalOff()
{
	for(unsigned short i=0; i<m_pCabinPanel->m_floorKeySignals.size(); i++)
	{
		if(!m_pCabinPanel->m_ascendingSignal && !m_pCabinPanel->m_closeDoorsKeySignal &&
			!m_pCabinPanel->m_descendingSignal && !m_pCabinPanel->m_highPriorityKeySignal &&
			!m_pCabinPanel->m_overloadSignal && !m_pCabinPanel->m_openDoorsKeySignal &&
			!m_pCabinPanel->m_floorKeySignals[i] && !m_pEntrances[i]->m_entrancePanel.m_downSignal &&
			!m_pEntrances[i]->m_entrancePanel.m_upSignal)
			return true;
	}
	return false;
}

/*! \fn bool CCabinController::allSignalOn()
	\brief Wenn alle Signale an, liefert true. Falls nicht, liefert false
 */
bool CCabinController::allSignalOn()
{
	for(unsigned short i=0; i<m_pCabinPanel->m_floorKeySignals.size(); i++)
	{
		if(m_pCabinPanel->m_ascendingSignal && m_pCabinPanel->m_closeDoorsKeySignal &&
			m_pCabinPanel->m_descendingSignal && m_pCabinPanel->m_highPriorityKeySignal &&
			m_pCabinPanel->m_overloadSignal && m_pCabinPanel->m_openDoorsKeySignal &&
			m_pCabinPanel->m_floorKeySignals[i] && m_pEntrances[i]->m_entrancePanel.m_downSignal &&
			m_pEntrances[i]->m_entrancePanel.m_upSignal)
			return true;
	}
	return false;
}
