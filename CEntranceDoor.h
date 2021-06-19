/** \file CEntranceDoor.h
 * \brief CEntranceDoor class header file.
 * \details Enthaelt die Deklaration der Klasse CEntranceDoor.
 * \author Reimund
 * \date 2016
 */

#ifndef CENTRANCEDOOR_H_
#define CENTRANCEDOOR_H_

#include "CDoor.h"

class CCabinController;

/*! \class CEntranceDoor
 * \brief Modelliert eine Zugangstuer
 */
class CEntranceDoor : public CDoor
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion dieser
		Klasse */

public:
	CEntranceDoor();

	void connect(CCabinController* pCabinController);

    ~CEntranceDoor();

    void setFloorNumber(unsigned short floorNumber);
    void setDoorSide(CDoor::SIDE side);

private:
    unsigned short m_floorNumber;	/**< Nummer des Stockwerks, auf dem sich die Zugangstuer
		befindet */

    void evDoorOpens();
    void evDoorCloses();
    void evDoorFullyOpen();
    void evDoorFullyClosed();
};


#endif /* CENTRANCEDOOR_H_ */
