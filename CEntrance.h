/** \file CEntrance.h
 * \brief CEntrance class header file.
 * \details Enthaelt die Deklaration der Klasse CEntrance.
 * \author Reimund
 * \date 2016
 */

#ifndef CENTRANCE_H_
#define CENTRANCE_H_

#include "CEntrancePanel.h"
#include "CEntranceDoor.h"

class CCabinController;

/*! \class CEntrance
 * \brief Modelliert einen Stockwerkszugang. Dieser besteht aus einem
 *  Zugangs-Rufpanel und einer Zugangstuer.
 */
class CEntrance
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion
		dieser Klasse */

public:

	/*! \fn void CEntrance::CEntrance()
	 *  \brief Default-Konstruktor. Belegt die Attribute mit Standardwerten (0)
	*/
	CEntrance()
	{
		m_floorNumber=0;
	}

	/*! \fn CEntrance::CEntrance(unsigned short floorNumber, CDoor::SIDE doorSide)
	 *  \brief Konstruktor. Initialisiert die Zugangstuer und das Zugangspanel gemaess der
	 *  uebergebenen Parameter
	 * \param floorNumber Stockwerksnummer, auf dem sich der Zugang befindet
	 * \param doorSide Tuerseite der Zugangstuer
	 */
	CEntrance(unsigned short floorNumber, CDoor::SIDE doorSide)
	{
		m_floorNumber=floorNumber;
		m_entranceDoor.setFloorNumber(floorNumber);
		m_entranceDoor.setDoorSide(doorSide);
		m_entrancePanel.setFloorNumber(floorNumber);
	}

	/*! \fn void CEntrance::connect(CCabinController* pCabinController)
	 *  \brief Verbindet den Zugang mit dem Kabinencontroller
	 *  \param pCabinController Pointer auf den Kabinencontroller
	 */
	void connect(CCabinController* pCabinController)
	{
		m_entranceDoor.connect(pCabinController);
		m_entrancePanel.connect(pCabinController);
	}

	unsigned short m_floorNumber; 		/**< Stockwerksnummer */
	CEntrancePanel m_entrancePanel;		/**< Das Zugangspanel */
	CEntranceDoor m_entranceDoor;		/**< Die Zugangstuer */
};

#endif /* CENTRANCE_H_ */
