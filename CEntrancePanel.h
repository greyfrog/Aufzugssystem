/** \file CEntrancePanel.h
 * \brief CEntrancePanel class header file.
 * \details Enthaelt die Deklaration der Klasse CEntrancePanel.
 * \author Reimund
 * \date 2016
 */

#ifndef CENTRANCEPANEL_H_
#define CENTRANCEPANEL_H_

class CCabinController;


/*! \class CEntrancePanel
 * \brief Modelliert ein Zugangspanel mit Ruftasten und Signalanzeigen
 */
class CEntrancePanel
{
	friend class CSimulator;	/**< Simulator hat Vollzugriff auf private-Sektion
		dieser Klasse */

public:
    CEntrancePanel();
    CEntrancePanel(unsigned short floorNumber);
    void connect(CCabinController* pCabinController);

    bool m_upSignal;	/**< Signal der Ruftaste "Nach oben" */
    bool m_downSignal;	/**< Signal der Ruftaste "Nach unten" */

    unsigned short m_currentFloorSignal; /**< Signalanzeige "Nummer des 
		naechstgelegenen Stockwerks" */

    void setFloorNumber(unsigned short floorNumber);

private:
    unsigned short m_floorNumber;	/**< Nummer des Stockwerks, auf dem sich das Zugangspanel
		befindet */

    void work();

    void evKeyUp();
    void evKeyDown();

    CCabinController* m_pCabinController;	/**< Pointer auf den Kabinencontroller */
};

#endif /* CENTRANCEPANEL_H_ */
