/** \file SEvent.cpp
 * \brief SEvent helper functions.
 * \details Enthaelt Helper-Funktionen fuer die Struktur SEvent.
 * \author Reimund
 * \date 2016
 */

#include "SEvent.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../simulation/CTime.h"
extern CTime t;

/*! \fn std::string evToStr(SEvent ev)
 *  \brief Gibt den Event-Typ des uebergebenen Events als string
 *   zurueck.
 *  \param ev Das Event
 *  \return Event-Typ als string
 */
std::string evToStr(SEvent ev)
{
    switch(ev.m_eventType)
    {
    case KEY_UP:                        return "KEY_UP";
    case KEY_DOWN:                      return "KEY_DOWN";
    case KEY_OPEN_DOORS:                return "KEY_OPEN_DOORS";
    case KEY_CLOSE_DOORS:               return "KEY_CLOSE_DOORS";
    case KEY_FLOOR:                     return "KEY_FLOOR";
    case KEY_HIGH_PRIORITY:             return "KEY_HIGH_PRIORITY";
    case CABINDOOR_CLOSES:             	return "CABINDOOR_CLOSES";
    case CABINDOOR_OPENS:              	return "CABINDOOR_OPENS";
    case CABINDOOR_FULLY_CLOSED:       	return "CABINDOOR_FULLY_CLOSED";
    case CABINDOOR_FULLY_OPEN:         	return "CABINDOOR_FULLY_OPEN";
    case ENTRANCEDOOR_CLOSES:           return "ENTRANCEDOOR_CLOSES";
    case ENTRANCEDOOR_OPENS:            return "ENTRANCEDOOR_OPENS";
    case ENTRANCEDOOR_FULLY_CLOSED:     return "ENTRANCEDOOR_FULLY_CLOSED";
    case ENTRANCEDOOR_FULLY_OPEN:       return "ENTRANCEDOOR_FULLY_OPEN";
    case REACHED_FLOOR:                 return "REACHED_FLOOR";
    case MOTOR_STARTS_RUNNING:          return "MOTOR_STARTS_RUNNING";
    case MOTOR_STOPPED:                 return "MOTOR_STOPPED";
    case DOOR_BLOCKED:					return "DOOR_BLOCKED";
    case DOOR_UNBLOCKED:				return "DOOR_UNBLOCKED";
    case TIMER:							return "TIMER";
    case LOADSENSOR_OVERLOAD_DETECTED:	return "LOADSENSOR_OVERLOAD_DETECTED";
    case LOADSENSOR_OVERLOAD_RESOLVED:	return "LOADSENSOR_OVERLOAD_RESOLVED";
    case FIREALERT:						return "FIREALERT";
    default:                            return "UNDEFINED_EVENT";
    }
}

/*! \fn void printEvent(SEvent event, unsigned long long who)
 *  \brief Gibt das uebergebene Event auf der Konsole aus sowie die Adresse
 *  des Controllers, der das Event haelt
 *  \param event Das Event
 *  \param who Die Adresse des Controllers
 */
void printEvent(SEvent event, void* who)
{
    // Get current time
    unsigned long minutes, seconds, milliseconds;
    t.get(&minutes, &seconds, &milliseconds);
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << minutes
              << ":" << std::setfill('0') << std::setw(2) << seconds
              << ":" << std::setfill('0') << std::setw(4) << milliseconds
              << "        Controller " << std::hex << ((unsigned long long)who & 0x00ffff)
              << ": Received event " << evToStr(event);
    ss << ", info: " << event.m_additionalInfo << "\n";
    std::string outstr=ss.str();
    std::replace(outstr.begin(), outstr.begin()+11, '0', 'O');
    std::cout << outstr << std::flush;
}


