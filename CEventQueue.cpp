/** \file CEventQueue.cpp
 * \brief CEventQueue class source file.
 * \details Enthaelt die Implementierung der Klasse CEventQueue.
 * \author Reimund
 * \date 2018
 */

#include "CEventQueue.h"

/*! \fn std::vector<SEvent> CEventQueue::popEvents(EVENT_TYPE eventType)
	\brief Liefert einen chronologisch geordneten Vektor (FIFO) der aufgelaufenen
	 Events in dieser Queue zurueck. Diese werden daraufhin aus der Queue geloescht
	\param eventType Falls nicht UNDEFINED, werden nur Events des angegebenen Typs
	 zurueckgegeben und aus der Queue geloescht, andernfalls alle
	\return Chronologisch sortierter Vektor der Events (aelteste zuerst)
 */
std::vector<SEvent> CEventQueue::popEvents(EVENT_TYPE eventType)
{
    std::vector<SEvent> ret;

    switch(eventType)
    {
    case UNDEFINED:
        ret = m_events;
        m_events.clear();
        break;

    default:
    {
		std::vector<SEvent> without;
        for(std::vector<SEvent>::iterator it=m_events.begin(); it!=m_events.end(); it++)
        {
            if(it->m_eventType==eventType)
            {
                ret.push_back(*it);
            }
			else
			{
				without.push_back(*it);
			}
        }
		m_events = without;
    }
    	break;
    }
    return ret;
}

/*! \fn SEvent CEventQueue::popEvent(bool* pValid)
	\brief Liefert das chronologisch letzte (neueste) Event in der Queue zurueck
	\param pValid true, falls ein Event aus der Queue zurueckgegeben wird,
	 false andernfalls (dann war die Queue naemlich leer)
	\return Das chronologisch letzte (neueste) Event in der Queue
 */
SEvent CEventQueue::popEvent(bool* pValid)
{
	return popEvent(m_events.size()-1, pValid);
}

/*! \fn SEvent CEventQueue::popEvent(unsigned short eventNumber,
 *   bool* pValid)
	\brief Liefert das Event mit dem angegebenen Index in der Queue der gespeicherten
	 Events zurueck. Das Event wird daraufhin aus der Queue geloescht
	\param eventNumber Index des zurueckzuliefernden Events in der Queue
	\param pValid true, falls ein Event aus der Queue zurueckgegeben wird,
	 false andernfalls (dann existiert kein Event mit dem angegebenen Index)
	\return Falls eventNumber nicht INVALID_EVENT_NUMBER, das Event mit dem
	 angegebenen Index in der Queue, das chronologisch erste (aelteste) Event
	 andernfalls
 */
SEvent CEventQueue::popEvent(unsigned short eventNumber, bool* pValid)
{
	if(eventNumber<m_events.size())
	{
		*pValid=true;
		std::vector<SEvent>::iterator it;
		SEvent ret;
		it=(m_events.begin() + eventNumber);
		ret=*it;
		m_events.erase(it);
		return ret;
	}
	else
	{
		*pValid=false;
		SEvent e;
		e.m_eventType=UNDEFINED;
		e.m_additionalInfo=0;
		return e;
	}
}

/*! \fn void CEventQueue::pushEvent(SEvent event)
	\brief Fuegt der Queue ein Event hinzu. Sensoren und Aktoren
	 rufen diese Methode auf, um ueber ein Event zu informieren
	\param event Hinzuzufuegendes Event
 */
void CEventQueue::pushEvent(SEvent event)
{
    m_events.push_back(event);
}



