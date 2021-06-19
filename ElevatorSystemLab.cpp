/** \file ElevatorSystemLab.cpp
 * \brief Hauptprogramm mit Programmeinstiegspunkt main().
 * \author Reimund
 * \date 2016
 */

#include <iostream>
#include "simulation/CSimulator.h"
#include "elevatorSystem/CElevatorSystem.h"
#include "elevatorSystem/CCabinDoor.h"

using namespace std;

/*! \mainpage Labor Software Engineering: Dokumentation des Aufzugsystems
 * <br>
 * <a href="anforderungen.htm"><b>Anforderungsdokument</b></a><br>
 * <a href="tests.htm"><b>Testdokument</b></a><br>
 * <a href="textantworten.txt"><b>Textantworten</b></a> auf Fragen aus Vorbereitung und Durchfuehrung<br>
 * <a href="Zustandsuebergangstabelle.htm"><b>Zustandsuebergangstabelle</b></a><br>
 * <a href="Console.htm"><b>Konsole-Dokumentation</b></a><br>
 * <br>
 *  \image html Cover.png
 */

/*! \fn int main()
 *  \brief Hauptprogramm.
 *  \details Es wird ein Aufzugsystem definiert: Deklaration des Aufzugsystems,
 *   Hinzufuegen der Kabinen; Anschliessend wird ein
 *   Simulator deklariert und mit dem Aufzugsystem verbunden.
 *   Schliesslich wird die Simulation gestartet.
 */
int main()
{
    // Start
    cout << "ElevatorSystemLab" << endl;
    cout << "=================" << endl << endl;

    // Aufzugsystem definieren


    CElevatorSystem elevatorSystem(5);
    elevatorSystem.addElevator(CCabinDoor::BOTH);
    elevatorSystem.addElevator(CCabinDoor::BOTH);
    elevatorSystem.addEntrance(0, 0, CDoor::LEFT);
    elevatorSystem.addEntrance(0, 1, CDoor::LEFT);
    elevatorSystem.addEntrance(0, 2, CDoor::LEFT);
    elevatorSystem.addEntrance(0, 3, CDoor::RIGHT);
    elevatorSystem.addEntrance(0, 4, CDoor::RIGHT);
    elevatorSystem.addEntrance(1, 0, CDoor::LEFT);
    elevatorSystem.addEntrance(1, 1, CDoor::RIGHT);
    elevatorSystem.addEntrance(1, 2, CDoor::LEFT);
    elevatorSystem.addEntrance(1, 3, CDoor::LEFT);
    elevatorSystem.addEntrance(1, 4, CDoor::RIGHT);

    // Simulation definieren, verbinden, und, falls gewollt, Passagiere hinzufuegen
    CSimulator simulator(&elevatorSystem);

    simulator.addPassenger(0, 1, 0, 5, 80, false);
    simulator.addPassenger(0, 2, 0, 6, 80, false);
    simulator.addPassenger(2, 4, 0, 10, 80, false);
    simulator.addPassenger(0, 4, 0, 11, 80, false);
    simulator.addPassenger(1, 4, 0, 11, 80, false);
    simulator.addPassenger(1, 3, 0, 3, 80, false);
    simulator.addPassenger(0, 1, 1, 7, 80, false);
    simulator.addPassenger(2, 3, 1, 8, 80, false);
    simulator.addPassenger(1, 4, 1, 14, 80, false);
    simulator.addPassenger(1, 3, 1, 18, 80, false);



    // Simulation starten
    simulator.run();

    cout << ">>> Simulator returned from run(), return 0." << endl;
    return 0;
}

/*! \page Diagramme
 * - \subpage Klassendiagramm Klassendiagramm
 * - \subpage Aktivitaetdiagramm Aktivitaetdiagramm
 * - \subpage Usecasediagramm Use-case Diagramm
 * - \subpage Zustanddiagramm Zustanddiagramm
 * - \subpage Zusatzaufgabe Aktivitaetdiagramm
 */

/*! \page Klassendiagramm Klassendiagramm
 * \image html Klassendiagram.png Klassendiagramm
 * \brief Beschreibung des Klassendiagramm: Der haupt Klasse ist CElevatorSystem und es besteht aus 2 Klasse : CSystemController und CCabin. CCabin besteht aus CCabinController, CMotor, CTimer,
 * CHeightSenor, CCabinDoor,.. und alle Komponete des Kabine kennt die CCabinController( Dadurch wird alle Komponenten gesteuert). CCabinCOntroller und CSystemCOntroller kennt einander und habe gleiche Bestandteil( CEventQueue).
 * Ausserdem SEvent ist ein Bestandteil auf CEventQueue und CCabinDoor und CEntrancDoor ist ein CDoor.
 *
 */

/*! \page Aktivitaetdiagramm Aktivitaetdiagramm
 * \image html Aktivitaetdiagramm.png Aktivitaetdiagramm
 * \brief Beschreibung Aktivitaetsdiagramm: 1.2.e: Endlosschleife, die die globale Simulationszeit mit jedem Durchlauf hochzaelt
 *  und periodisch alle Komponenten des Aufzugssystems zum Weiterarbeiten veranlasst. Ausserdem werden die Client-Aktionen entgegengenommen und darauf reagiert.
 *  Schliesslich wird die Visualisierung aktualisiert.Die Schleife wird verlassen, sobald die Verbindung zum Client aufgehoben wird,
 *  anschliessend wird die TCP-Verbindung aufgeraemt.
 */

/*! \page Usecasediagramm Use-case Diagramm
 * \image html Usecasediagramm.png Usecasediagramm
 * \brief Beschreibung UseCase Diagramm: Der Akteur ist ein Passagier. Das Diagramm beschreibt die standardisierte Verhalten des Aufzugsystem,
 * somit der Passagier im Zielstockwerk abliefern kann. Wenn es ein Include Beziehung ist, muss das untergeordnete Usecase durchfuehren.
 * Damit das uebergeordnete Usecase ausfuehren kann. Im Fall extend Beziehung kann das untergeordnete Usecase durchfuehren, muss aber nicht.
 */

/*! \page Zustanddiagramm Zustanddiagramm
 * \image html Zustanddiagram.png Zustanddiagramm
 * \brief Beschreibung des Zusatnddiagramm: Es gibt 3 moegliche Zustaende fuer die Kabinensteuerung (Waiting for request, closing, driving) und starten mit das Zustand waiting for request. Wenn eine Ruftaste (Zugangspanel) oder
 * eine Stockwerkswahltaste (Kabinenpanel) gedrueckt wird, wird der Zustand zum Closing veraendert und die Tueren machen zu, bis sie vollstaendig geschlossen sind. Dann wechseln der Zustand zum Driving und faehrt der Aufzug zu den gewuenschte Stockwerk
 *  Wenn der Aufzug den Stockwerk erreicht, machen die Tueren auf und der Zustand wird wieder Waiting for request veraendert.
 */

/*! \page Zusatzaufgabe Aktivitaetdiagramm
 * \image html ZusatzDiagramm.png Aktivitaeteniagramm
 * \brief Zusatz Aufgabe Labor 2 Aktivitaetsdiagramm: beschreibt den Zustandsautomat fuer den einzelnen Aufzug und wird periodisch durch den Simulator aufgerufen.
 * Die Events mit jedem Durchlauf hochzaehlt. Sobald die Events gleich oder groesser als die Anzahl des Events, wird die Schleife verlassen. Sonst wird ein Event wird gewaelt,
 * verhaelt sich der Aufzug im entsprechenden Zustand. Dann wird der Zustand gewelchselt.
 */

