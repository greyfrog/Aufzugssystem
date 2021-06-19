/** \file CMotor.h
 * \brief CMotor class header file.
 * \details Enthaelt die Deklaration der Klasse CMotor.
 * \author Reimund
 * \date 2016
 */

#ifndef CMOTOR_H_
#define CMOTOR_H_

#define METERS_PER_STEP 0.05	/**< Kabinen-Hebegeschwindigkeit in m / Simulationsschritt */

class CCabinController;

/*! \class CMotor
 * \brief Modelliert einen Aufzugmotor. Dieser setzt die Kabine in
 *  Bewegung
 */
class CMotor
{
	friend class CSimulator;	/**< Simulator hat Vollzugriff auf private-Sektion
		dieser Klasse */

public:
	/** Zustand des Motors
	 */
    enum STATE
    {
        IS_RUNNING=0,		/**< Motor laeuft */
        IS_STANDING_STILL=1	/**< Motor steht */
    };

    CMotor();
    void connect(CCabinController* pCabinController, float* pHeight, bool* pIsDriving);
    bool lift(float meters);

private:
    void work();

    void evMotorStopped();
    void evMotorStartsRunning();

    STATE m_state;								/**< Zustand des Motors */
    float m_metersToLift;						/**< Interner Merker fuer die aktuell
    	insgesamt zurueckzulegende Hoehenstrecke */
    float m_metersLifted;						/**< Interner Merker fuer die aktuell
    	bisher zurueckgelegte Hoehenstrecke */
    float* m_pHeight;							/**< Pointer auf die physikalische Groesse
    	"Kabinenhoehe" */
    bool* m_pIsDriving;							/**< Pointer auf die physikalische Groesse
    	"Kabine faehrt" */
    CCabinController* m_pCabinController;		/**< Pointer auf den Kabinencontroller */
};

#endif /* CMOTOR_H_ */
