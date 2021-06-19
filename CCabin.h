/** \file CCabin.h
 * \brief CCabin class header file.
 * \details Enthaelt die Deklaration der Klasse CCabin.
 * \author Reimund
 * \date 2016
 */

#ifndef CCABIN_H_
#define CCABIN_H_

#include "CCabinController.h"
#include "CMotor.h"
#include "CTimer.h"
#include "CHeightSensor.h"
#include "CCabinDoor.h"
#include "CcabinPanel.h"
#include "CEntrance.h"
#include "map"

class CCabinControllerProxy;

/*! \class CCabin
 * \brief Modelliert eine Aufzugkabine. Zu ihr gehoeren die Sensoren und Aktoren
 *  wie z. B. Hoehensensor und Motor sowie die Verarbeitungseinheit, der Kabinencontroller.
 *  Sie ist Traegerin einiger physikalischer Groessen wie z. B. Hoehe und Gewicht
 */
class CCabin
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion
		dieser Klasse */

public:
    CCabin(CCabinDoor::LAYOUT layout);
    void setup(unsigned short numFloor);
    void addEntrance(unsigned short FloorNumber, CDoor::SIDE entranceSide);

    CCabinControllerProxy cabinControllerProxy();

private:
    CMotor m_motor; 									/**< Der Motor */
    CCabinController m_cabinController; 				/**< Der Aufzugcontroller */
    CTimer m_timer; 									/**< Der Timer */
    CHeightSensor m_heightSensor; 			 			/**< Der Hoehensensor */
    std::map<CDoor::SIDE, CCabinDoor> m_cabinDoors;		/**< Der Kabinentüren mit Türseite*/
    float m_load; 							 			/**< Aufzug-Zuladung / kg */
    float m_height; 									/**< Aufzughoehe / m */
    bool m_isDriving; 									/**< Gibt an, ob der Aufzug gerade 
		faehrt */
    CCabinPanel m_cabinPanel;							/**< Kabinentableau*/
    std::map<unsigned short, CEntrance> m_entrances;	/**< Zugang mit Sotckwerknummer*/
};

/*! \class CCabinControllerProxy
 * \brief Diese Proxy-Klasse wird benoetigt, um dem Aufzugsystem (CElevatorSystem) zu
 * ermoeglichen, den untergeordneten Kabinencontroller mit dem Systemcontroller
 * zu verbinden, ohne jedoch dem Aufzugsystem oder dem Systemcontroller
 * Vollzugriff auf die gesamte Kabine zu gestatten (Attorney-Client-Prinzip)
 */
class CCabinControllerProxy
{
	friend class CSystemController; /**< Nur der Systemcontroller darf den
		Kabinencontroller (das private Attribut dieser Klasse) verwenden */

public:
	/*! \fn CCabinControllerProxy::CCabinControllerProxy(CCabinController* pCabinController)
		\brief Konstruktor. Der Proxy wird durch die Kabine zur Verfuegung gestellt (instanziert)
		und dem Aufzugsystem uebergeben. Dieses gibt den Proxy an seinen Systemcontroller weiter
		\param pCabinController Pointer auf den Kabinencontroller
	 */
	CCabinControllerProxy(CCabinController* pCabinController)
	{
		m_pCabinController = pCabinController;
	}

private:
	CCabinController* m_pCabinController; /**< Pointer auf den Kabinencontroller, der verbunden
		werden soll*/
};


#endif /* CCABIN_H_ */
