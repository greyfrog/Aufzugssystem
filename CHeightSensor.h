/** \file CHeightSensor.h
 * \brief CHeightSensor class header file.
 * \details Enthaelt die Deklaration der Klasse CHeightSensor.
 * \author Reimund
 * \date 2016
 */

#ifndef CHEIGHTSENSOR_H_
#define CHEIGHTSENSOR_H_

#define METERS_PER_FLOOR 2.0	/**< Geometrische Groesse "Meter pro Stockwerk" */

class CCabinController;

/*! \class CHeightSensor
 * \brief Modelliert einen Hoehensensor. Dieser liefert die Hoehenmessung fuer die Kabine
 */
class CHeightSensor
{
	friend class CSimulator; /**< Simulator hat Vollzugriff auf private-Sektion
		dieser Klasse */

public:
    CHeightSensor();

    void connect(CCabinController* pCabinController, float* pHeight);

    float height();
    unsigned short currentFloor();
    float metersPerFloor();

private:
    void work();

    unsigned short m_lastFloor;					/**< Interner Merker, um eine Aenderung des
    	der Kabine naechstgelegenen Stockwerks festzustellen */
    float* m_pHeight; 							/**< Pointer auf die physikalische Groesse
    	"Kabinenhoehe" */

    CCabinController* m_pCabinController;		/**< Pointer auf den Kabinencontroller */
};

#endif /* CHEIGHTSENSOR_H_ */
