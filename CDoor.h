/** \file CDoor.h
 * \brief CDoor class header file.
 * \details Enthaelt die Deklaration der abstrakten Klasse CDoor.
 * \author Reimund
 * \date 2016
 */

#ifndef CDOOR_H_
#define CDOOR_H_

#define OPENING_STEPS 100 /**< Anzahl Simulationsschritte, die zum Oeffnen /
	Schliessen der Tuer benoetigt werden */

class CCabinController;


/*! \class CDoor
 * \brief Modelliert eine Tuer; Die Klasse ist abstrakt, kann also nicht
 * 	instanziert werden. CCabinDoor (Aufzugtuer) und CEntranceDoor (Zugangstuer)
 * 	erben hiervon
 */
class CDoor
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion dieser
		Klasse */

public:

	/** Oeffnungszustand
	 */
    enum STATE{IS_OPENING=0,	/**< Tuer oeffnet gerade */
               IS_OPEN=1,		/**< Tuer ist vollstaendig geoeffnet */
               IS_CLOSED=2,		/**< Tuer ist vollstaendig geschlossen */
               IS_CLOSING=3		/**< Tuer schliesst gerade */
    };

    /** Tuerseite
     */
    enum SIDE{LEFT, 	/**< Zustiegsseite links */
              RIGHT		/**< Zustiegsseite rechts */
    };

    bool open();
    bool close();

    STATE state();
    float openingPercentage();
    SIDE side();

	/*! \fn CDoor::~CDoor()
	 *  \brief Destruktor (leer)
	 */
    virtual ~CDoor(){}

protected:
    void work();

    virtual void evDoorOpens()=0;
    virtual void evDoorCloses()=0;
    virtual void evDoorFullyOpen()=0;
    virtual void evDoorFullyClosed()=0;

    SIDE m_side; /**< Tuerseite */
    STATE m_state;	/**< Zustand der Tuer */
    short m_step;	/**< Interner Simulationsschrittzaehler */
    CCabinController* m_pCabinController;	/**< Pointer auf den Kabinencontroller */
};

#endif /* CDOOR_H_ */
