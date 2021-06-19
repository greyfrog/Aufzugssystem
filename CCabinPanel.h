/** \file CCabinPanel.h
 * \brief CCabinPanel class header file.
 * \details Enthaelt die Deklaration der Klasse CCabinPanel.
 * \author Reimund
 * \date 2016
 */

#ifndef CCABINPANEL_H_
#define CCABINPANEL_H_

#include <vector>

class CCabinController;


/*! \class CCabinPanel
 * \brief Modelliert ein Kabinenpanel mit Stockwerk-Wahltasten, weiteren
 *  Steuerungstasten und Signalanzeigen
 */
class CCabinPanel
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion
		dieser Klasse */

public:
    CCabinPanel();
    void connect(CCabinController* pCabinController);

    std::vector<bool> m_floorKeySignals;	/**< Container, der die Signale fuer
       												die Stockwerk-Wahltasten enthaelt */
    bool m_openDoorsKeySignal;				/**< Signal der Tuer-Oeffnen-Taste */
    bool m_closeDoorsKeySignal;				/**< Signal der Tuer-Schliessen-Taste */
    bool m_highPriorityKeySignal;			/**< Signal der Vorzugstaste */
    bool m_overloadSignal;					/**< Signal "Maximale Zuladung ueberschritten" */
    bool m_ascendingSignal;					/**< Signal "Aufzug faehrt hoch */
    bool m_descendingSignal;				/**< Signal "Aufzug faehrt herunter */
    unsigned short m_currentFloorSignal;	/**< Signal "Nummer des naechstgelegenen
       											Stockwerks" */

    void setNumFloors(unsigned short numFloors);

private:
    void work();
    void evKeyFloor(unsigned short floorNumber);
    void evKeyOpenDoors();
    void evKeyCloseDoors();
    void evKeyHighPriority();


    CCabinController* m_pCabinController; /**< Pointer auf den Kabinencontroller */
};

#endif /* CCABINPANEL_H_ */
