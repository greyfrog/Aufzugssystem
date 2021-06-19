/** \file CEventQueue.h
 * \brief CEventQueue class header file.
 * \details Enthaelt die Deklaration der Klasse CEventQueue.
 * \author Reimund
 * \date 2018
 */

#ifndef CEVENTQUEUE_H_
#define CEVENTQUEUE_H_

#include "SEvent.h"
#include <vector>

/*! \class CEventQueue
 * \brief Modelliert eine Queue (Schlange) von Events. Die Controller hinterlegen
 * hier alle Events, die von ihren angebundenen Sensoren und Aktoren empfangen werden.
 * Es ist moeglich, neue Events zu hinterlegen (push) oder gespeicherte abzurufen und
 * zu loeschen (pop).
 */
class CEventQueue
{
public:
    std::vector<SEvent> popEvents(EVENT_TYPE eventType=UNDEFINED);
    SEvent popEvent(bool* pValid);
    SEvent popEvent(unsigned short eventNumber, bool* pValid);
    void pushEvent(SEvent event);

private:
	std::vector<SEvent> m_events;	/**< Die in dieser EventQueue zur Zeit hinterlegten Events */

};

#endif /* CEVENTQUEUE_H_ */
