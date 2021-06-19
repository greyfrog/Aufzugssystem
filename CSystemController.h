/** \file CSystemController.h
 * \brief CSystemController class header file.
 * \details Enthaelt die Deklaration der Klasse CSystemController.
 * \author Reimund
 * \date 2016
 */

#ifndef CSYSTEMCONTROLLER_H_
#define CSYSTEMCONTROLLER_H_

#include "CEventQueue.h"
#include <list>

class CCabinControllerProxy;
class CCabinController;

/*! \class CSystemController
 * \brief Modelliert den Systemcontroller. Er uebernimmt die uebergeordnete Steuerung 
 * des Aufzugsystems. Dafuer ist er mit allen untergeordneten Kabinencontrollern verbunden
 */
class CSystemController
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion
		dieser Klasse */

public:
	CSystemController();

    void connectCabinController(CCabinControllerProxy cabinControllerProxy);
	
	void pushEvent(SEvent event);

private:
    enum STATE
	{
    	NORMAL,		/**< Transportbetrieb "normal" */
		FIREALARM   /**< Feueralarm */
	};

    STATE m_state;	/**< Aktueller Systemzustand */
    void work();
    bool evExist;
    CEventQueue m_eventQueue;							/**< Queue der aufgelaufenen Events */
    std::list<CCabinController*> m_pCabinControllers; /**< Pointer auf die
    	untergeordneten Kabinencontroller */

};

#endif /* CSYSTEMCONTROLLER_H_ */
