/** \file CCabinDoor.h
 * \brief CCabinDoor class header file.
 * \details Enthaelt die Deklaration der Klasse CCabinDoor.
 * \author Reimund
 * \date 2016
 */

#ifndef CCABINDOOR_H_
#define CCABINDOOR_H_

#include "CDoor.h"

class CCabinController;

/*! \class CCabinDoor
 * \brief Modelliert eine Kabinentuer
 */
class CCabinDoor : public CDoor
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion dieser
			Klasse */

public:
	CCabinDoor(CDoor::SIDE side = CDoor::LEFT);

	void connect(CCabinController* pCabinController);

	/** Moegliche Kabinentuer-Layouts fuer einen Aufzug
	 */
    enum LAYOUT
    {
        LEFT,		/**< Nur links */
        RIGHT,		/**< Nur rechts */
        BOTH,		/**< Beidseitig */
        UNDEFINED	/**< Undefiniert */
    };

    bool unblocked();

    ~CCabinDoor();

private:
    void evDoorBlocked();
    void evDoorUnblocked();

    void evDoorOpens();
    void evDoorCloses();
    void evDoorFullyOpen();
    void evDoorFullyClosed();

    bool m_unblocked; /**< Lichtschranke unterbrochen / frei? */
};

#endif /* CCABINDOOR_H_ */
