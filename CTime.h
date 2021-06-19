/** \file CTime.h
 * \brief CTime class header file.
 * \details Enthaelt Deklaration und Implementierung der Klasse CTime.
 * \author Reimund
 * \date 2016
 */

#ifndef CTIME_H_
#define CTIME_H_


#include <windows.h>

#define UPDATE_PERIOD_MS 30	/**< Simulationsschrittweite (Update-Periode) in ms */


/*! \class CTime
 * \brief Modelliert die Simulationszeit
 */
class CTime
{
public:

	/*! \fn CTime::CTime()
	 *  \brief Konstruktor; Setzt den Simulationsschrittzaehler zu 0
	 */
    CTime()
    {
        m_steps=0;
    }

    /*! \fn void CTime::get(unsigned long* pMinutes, unsigned long* pSeconds,
        unsigned long* pMilliseconds)
     *  \brief Liefert die Simulationszeit in vollen Minuten, vollen Sekunden
     *  	und Millisekunden zurueck (call by name)
     *  \param pMinutes Pointer zur Rueckgabe der vollen Minuten
     *  \param pSeconds Pointer zur Rueckgabe der vollen Sekunden
     *  \param pMilliseconds Pointer zur Rueckgabe der Millisekunden
     */
    void get(unsigned long* pMinutes, unsigned long* pSeconds,
    		 unsigned long* pMilliseconds)
    {
        double milliseconds_f = (double)m_steps * (double)UPDATE_PERIOD_MS;
        *pMinutes=milliseconds_f/(1000.0*60.0);
        milliseconds_f -= (double)(*pMinutes)*(1000.0*60.0);
        *pSeconds = milliseconds_f/1000.0;
        milliseconds_f -= (double)(*pSeconds)*1000.0;
        *pMilliseconds = milliseconds_f;
    }

    /*! \fn unsigned long CTime::get_ms()
     *  \brief Liefert die Simulationszeit in Millisekunden zurueck
     *  \return Simulationszeit in Millisekunden
     */
    unsigned long get_ms()
    {
    	return (m_steps * UPDATE_PERIOD_MS);
    }

    /*! \fn unsigned int CTime::update_period_ms()
     *  \brief Liefert die Simulationsschrittweite ( = Updateperiode)
     *   zurueck
     *  \return Simulationsschrittweite ( = Updateperiode) in ms
     */
    unsigned int update_period_ms()
    {
        return UPDATE_PERIOD_MS;
    }

    /*! \fn void CTime::operator++(int)
     *  \brief Wartet eine Updateperiode (UPDATE_PERIOD_MS) und inkrementiert
     *   den Simulationsschrittzaehler
     */
    void operator++(int)
    {
        Sleep(UPDATE_PERIOD_MS);
        m_steps++;
    }

	 /*! \fn void CTime::reset()
     *  \brief Setzt die Simulationszeit zurueck
     */
    void reset()
    {
    	m_steps=0;
    }

private:

    unsigned long long m_steps;	/**< Simulationsschrittzaehler */
};

#endif /* CTIME_H_ */
