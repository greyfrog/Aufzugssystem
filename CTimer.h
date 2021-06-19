/** \file CTimer.h
 * \brief CTimer class header file.
 * \details Enthaelt die Deklaration der Klasse CTimer.
 * \author Reimund
 * \date 2016
 */

#ifndef CTIMER_H_
#define CTIMER_H_

class CCabinController;

/*! \class CTimer
 * \brief Modelliert einen Timer. Er kann auf eine bestimmte Zeit
 *  gesetzt und dann gestartet werden. Er sendet das Event TIMER,
 *  sobald er abgelaufen ist.
 */
class CTimer
{
public:
	CTimer();
	void connect(CCabinController* pCabinController);
	void set(unsigned long msec);
	void work();

private:
	unsigned long long m_startTime; /**< Zeitpunkt, zu dem der Timer das letzte
		Mal gestartet wurde */
	unsigned long m_delay; 			/**< Zeit in Millisekunden, nach der der Timer ablaufen
		soll */
	bool m_finished; 				/**< true, falls Timer schon abgelaufen, sonst false */

	CCabinController* m_pCabinController; /**< Pointer auf den Aufzugcontroller */
};

#endif /* CTIMER_H_ */
