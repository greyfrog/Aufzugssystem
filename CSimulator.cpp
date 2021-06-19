/** \file CSimulator.cpp
 * \brief CSimulator class source file.
 * \details Enthaelt die Implementierung der Klasse CSimulator.
 * \author Reimund
 * \date 2016
 */

#include "CSimulator.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>

#include "../elevatorSystem/CElevatorSystem.h"

CTime t;

/*! \fn CSimulator::CSimulator(CElevatorSystem* pElevatorSystem)
 *  \brief Konstruktor; Stellt die Verbindung mit dem Visualisierungsclient
 *   und dem uebergebenen Aufzugsystem her
 *  \param pElevatorSystem Pointer auf das zu simulierende Aufzugsystem
 */
CSimulator::CSimulator(CElevatorSystem* pElevatorSystem)
{
#ifdef DEACTIVATE_TCP
    std::cout << ">>> Server-only-mode! Undefine DEACTIVATE_TCP in CSimulator.h! <<<\n\n" << std::flush;
#endif
#ifdef DBG_TCP
    std::cout << ">>> TCP-debug-mode! TCP messages will be output to stdout. Undefine DBG_TCP in CSimulator.h to suppress. <<<\n\n" << std::flush;
#endif
    m_pElevatorSystem = pElevatorSystem;
    c=s=new_socket=0;
    m_tcpConnectionOK=connectToClient(TCP_PORT);
    isStarted=false;
    clientClosed=false;
#if defined(LABOR2_VOR) || defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
    m_retardStepCtr=0;
    srand(time(NULL));
#endif
}

/*! \fn bool CSimulator::isConnected()
 *  \brief Liefert zurueck, ob die TCP-Verbindung zum Client steht und das Aufzugsystem
 *   mit dem Simulator verbunden ist
 *  \return true, falls TCP-Verbindung zum Client steht und das Aufzugsystem
 *   mit dem Simulator verbunden ist, false andernfalls
 */
bool CSimulator::isConnected()
{
#ifndef DEACTIVATE_TCP
    return (bool)m_pElevatorSystem && m_tcpConnectionOK;
#endif
#ifdef DEACTIVATE_TCP
    return true;
#endif
}

/*! \fn void CSimulator::closeTcpConnection()
 *  \brief Schliesst die TCP-Verbindung zum Client
 */
void CSimulator::closeTcpConnection()
{
    std::string msg="<SERVER><MESSAGE>CONNECTION_CLOSE</MESSAGE></SERVER>";
    if(isConnected()) sendString(msg);
#ifndef DEACTIVATE_TCP
    closesocket(s);
    WSACleanup();
#endif
}

/*! \fn void CSimulator::addPassenger(unsigned int startingFloorNo,
 * 									  unsigned int destinationFloorNo,
 * 									  unsigned int elevatorNo,
    								  unsigned int activationTime_sec,
    								  float weight,
    								  bool usesHighPriorityKey);
 * \brief Fuegt der Simulation einen automatisch gesteuerten Passagier hinzu
 * \param startingFloorNo Stockwerk, auf dem der Passagier abgeholt werden soll
 * \param destinationFloorNo Fahrtwunsch (Zielstockwerk)
 * \param elevatorNo Aufzugnummer
 * \param activationTime_sec Zeitpunkt, zu dem der Passagier aktiv wird, in Sek.
 * \param weight Gewicht des Passagiers in kg
 * \param usesHighPriorityKey Gibt an, ob der Passagier die Vorzugstaste benutzt
 */
void CSimulator::addPassenger(unsigned int startingFloorNo,
		 					  unsigned int destinationFloorNo,
	 						  unsigned int elevatorNo,
							  unsigned int activationTime_sec,
							  float weight,
							  bool usesHighPriorityKey)
{
	std::stringstream ss;
	ss << "<SERVER>";
		ss << "<ADDPASSENGER>";
		ss << "<STARTINGFLOOR>" << startingFloorNo << "</STARTINGFLOOR>";
		ss << "<DESTINATIONFLOOR>" << destinationFloorNo << "</DESTINATIONFLOOR>";
		ss << "<ELEVATOR>" << elevatorNo << "</ELEVATOR>";
		ss << "<ACTIVATIONTIME>" << activationTime_sec << "</ACTIVATIONTIME>";
		ss << "<WEIGHT>" << weight << "</WEIGHT>";
		if(usesHighPriorityKey)
			ss << "<USESHIGHPRIO>true</USESHIGHPRIO>";
		else
			ss << "<USESHIGHPRIO>false</USESHIGHPRIO>";
		ss << "</ADDPASSENGER>";
	ss << "</SERVER>";
	sendString(ss.str());
}

/*! \fn void CSimulator::handleClientActions()
 * \brief Nimmt die auf dem TCP-Buffer abgelegten Client-Aktionen entgegen
 *  und reagiert auf diese (Beaufschlagen der Aufzugsystemkomponenten mit dem
 *  Passagiereinfluss wie Knopfdruecken, Gewicht...)
 */
void CSimulator::handleClientActions()
{
#ifndef DEACTIVATE_TCP
    FD_ZERO(&readSet);
    FD_SET(new_socket, &readSet);
    struct sockaddr_in address;
    char buffer[MAXRECV + 1];
    //buffer =  (char*) malloc((MAXRECV + 1) * sizeof(char));
    timeval timeout;
    timeout.tv_sec = 0;  // Zero read timeout (poll)
    timeout.tv_usec = 0;
    if(select( 0 , &readSet , NULL , NULL , &timeout) == SOCKET_ERROR)
    {
        std::cout << "ERROR: CSimulator::handleClientActions(): Select call failed, error code "
        	<< WSAGetLastError() << std::flush;
        m_tcpConnectionOK=false;
        return;
    }
    if (FD_ISSET( new_socket , &readSet))
    {
        // get details of the client
        getpeername(new_socket , (struct sockaddr*)&address , (int*)&c);
        // Check if it was for closing , and also read the incoming message
        // recv does not place a null terminator at the end of the string (whilst printf %s
        // assumes there is one).
        int valread = recv( new_socket, buffer, MAXRECV, 0);
        if(valread == SOCKET_ERROR)
        {
            int error_code = WSAGetLastError();
            if(error_code == WSAECONNRESET)
            {
                std::cout << ">>> Client disconnected.\n" << std::flush;
            }
            else
            {
                std::cerr << "ERROR: CSimulator::handleClientActions(): Receive failed, error code "
                	<< error_code << "\n" << std::flush;
            }
            m_tcpConnectionOK=false;
            return;
        }
        if (valread == 0)
        {
        	if(!isStarted)
        		Sleep(250);
            return;
        }
        // Echo the message that came in
        else
        {
            // add null character, if you want to use with printf/puts or other string
        	// handling functions
            buffer[valread] = '\0';
            //std::cout << "Read: " << buffer << "\n" << std::flush;
            // send( new_socket , buffer , valread , 0 );
            std::string str(buffer);
            while(str.find("<CLIENT><ACTIONTYPE>")!=std::string::npos)
            {
				int actionType_i, info1_i, info2_i;
				sscanf(str.c_str(), "<CLIENT><ACTIONTYPE>%d</ACTIONTYPE><INFO1>%d</INFO1><INFO2>%d</INFO2></CLIENT>",
						&actionType_i, &info1_i, &info2_i);
				CLIENT_ACTION_TYPE actionType = (CLIENT_ACTION_TYPE)actionType_i;
				unsigned short elevatorNumber = info1_i;
				float passengerWeight = info2_i;
				std::list<CCabin>::iterator it=m_pElevatorSystem->m_cabins.begin();
				std::advance(it, elevatorNumber);
				switch(actionType)
				{
#if defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
				case CSimulator::KEY_DOWN:
					it->m_cabinController.m_pEntrances[(unsigned short)info2_i]->m_entrancePanel.evKeyDown();
					break;
				case CSimulator::KEY_UP:
					it->m_cabinController.m_pEntrances[(unsigned short)info2_i]->m_entrancePanel.evKeyUp();
					break;
#endif
#if defined(LABOR2_VOR) || defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
				case CSimulator::DOOR_BLOCKED:
					if((unsigned short)info2_i==0)
					{
						it->m_cabinController.m_pCabinDoors[CDoor::LEFT]->evDoorBlocked();
						it->m_cabinController.m_pCabinDoors[CDoor::LEFT]->m_unblocked=false;
					}
					if((unsigned short)info2_i==1)
					{
						it->m_cabinController.m_pCabinDoors[CDoor::RIGHT]->evDoorBlocked();
						it->m_cabinController.m_pCabinDoors[CDoor::RIGHT]->m_unblocked=false;
					}

					break;
				case CSimulator::DOOR_UNBLOCKED:
					if((unsigned short)info2_i==0)
					{
						it->m_cabinController.m_pCabinDoors[CDoor::LEFT]->evDoorUnblocked();
						it->m_cabinController.m_pCabinDoors[CDoor::LEFT]->m_unblocked=true;
					}
					if((unsigned short)info2_i==1)
					{
						it->m_cabinController.m_pCabinDoors[CDoor::RIGHT]->evDoorUnblocked();
						it->m_cabinController.m_pCabinDoors[CDoor::RIGHT]->m_unblocked=true;
					}
					break;
				case CSimulator::KEY_FLOOR:
					it->m_cabinPanel.evKeyFloor((unsigned short)info2_i);
					break;
				case CSimulator::KEY_OPEN_DOORS:
					it->m_cabinPanel.evKeyOpenDoors();
					break;
				case CSimulator::KEY_CLOSE_DOORS:
					it->m_cabinPanel.evKeyCloseDoors();
					break;
#endif
#if defined(LABOR3_DURCH)
				case CSimulator::FIREALARM_ACTIVATED:
                {
					SEvent ev;
					ev.m_eventType = FIREALERT;
					ev.m_additionalInfo = 0;
					it->m_cabinController.pushEvent(ev);
					break;
                }
#endif
				case CSimulator::ENTER_ELEVATOR:
					it->m_load += passengerWeight;
					break;
				case CSimulator::LEAVE_ELEVATOR:
					it->m_load -= passengerWeight;
					break;
				case CSimulator::START:
					isStarted=true;
					break;
				case CSimulator::QUIT:
					clientClosed=true;
					break;

				default:
					break;
				}
				unsigned int end=str.find("</CLIENT>");
				str=str.substr(end+10, str.length());
            }
        }
    }
    //free(buffer);
#endif

    return;
}

/*! \fn void CSimulator::sendElevatorSystemConfiguration()
 * \brief Sendet dem Client die Grundkonfiguration des Aufzugsystems
 */
void CSimulator::sendElevatorSystemConfiguration()
{
    std::stringstream ss;
    ss << "<SERVER>";
        ss << "<SETUP>";
            ss << "<NUMFLOORS>" << m_pElevatorSystem->m_numFloors << "</NUMFLOORS>";
            ss << "<NUMELEVATORS>" << m_pElevatorSystem->m_cabins.size() << "</NUMELEVATORS>";
    unsigned short i=0;
    for(std::list<CCabin>::iterator it=m_pElevatorSystem->m_cabins.begin();
    		it!=m_pElevatorSystem->m_cabins.end();
    		it++)
    {
            ss << "<ELEVATOR_" << i << ">";
#if defined(LABOR2_VOR) || defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
            if(it->m_cabinController.m_pCabinDoors.count(CDoor::LEFT))
            {
            	ss << "<DOORLEFT_" << i << ">1</DOORLEFT_" << i << ">";
            	m_retard[(CDoor*)(it->m_cabinController.m_pCabinDoors[CDoor::LEFT])]=(rand()%2);
            }
            else ss << "<DOORLEFT_" << i << ">0</DOORLEFT_" << i << ">";
            if(it->m_cabinController.m_pCabinDoors.count(CDoor::RIGHT))
            {
            	ss << "<DOORRIGHT_" << i << ">1</DOORRIGHT_" << i << ">";
            	m_retard[(CDoor*)(it->m_cabinController.m_pCabinDoors[CDoor::RIGHT])]=(rand()%2);
            }
            else ss << "<DOORRIGHT_" << i << ">0</DOORRIGHT_" << i << ">";
#endif
#if defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
            for(unsigned short k=0; k<m_pElevatorSystem->m_numFloors; k++)
            {
            	if(it->m_cabinController.m_pEntrances.count(k))
            	{
            		ss << "<ENTRANCE_" << i << "_" << k << ">";
						ss << "<SIDE_" << i << "_" << k << ">"
            				<< it->m_cabinController.m_pEntrances[k]->m_entranceDoor.side()
            			<< "</SIDE_" << i << "_" << k << ">";
            		ss << "</ENTRANCE_" << i << "_" << k << ">";
            		m_retard[(CDoor*)(&(it->m_cabinController.m_pEntrances[k]->m_entranceDoor))]=(rand()%2);
            	}
            }
#endif
            ss << "</ELEVATOR_" << i << ">";
            i++;
    }
        ss << "</SETUP>";
    ss << "</SERVER>";
    sendString(ss.str());
}

/*! \fn void CSimulator::updateClient()
 * \brief Sendet dem Client den aktuellen Zustand des Aufzugsystems
 */
void CSimulator::updateClient()
{
#if defined(LABOR2_VOR) || defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
	m_retardStepCtr++;
	if(m_retardStepCtr>=5)
	{
		m_retardStepCtr=0;
		for(std::map<CDoor*, bool>::iterator it=m_retard.begin(); it!=m_retard.end(); it++)
			if(it->second)
			{
				if(it->first->state()==CDoor::IS_OPENING) it->first->m_step--;
				if(it->first->state()==CDoor::IS_CLOSING) it->first->m_step++;
			}
	}
#endif
    std::stringstream ss;
    ss << "<SERVER>";
        ss << "<UPDATE>";
        unsigned short i=0;
        	ss << "<TIMESTAMP_MS>" << t.get_ms() << "</TIMESTAMP_MS>";
        for(std::list<CCabin>::iterator it=m_pElevatorSystem->m_cabins.begin();
        	it!=m_pElevatorSystem->m_cabins.end();
        	it++)
        {
            ss << "<ELEVATOR_" << i << ">";
                ss << "<HEIGHT_" << i<< ">" << it->m_height << "</HEIGHT_" << i << ">";
#if defined(LABOR2_VOR) || defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
                ss << "<OVERLOAD_INDICATION_" << i << ">" << it->m_cabinPanel.m_overloadSignal << "</OVERLOAD_INDICATION_" << i << ">";
                ss << "<ASC_INDICATION_" << i << ">" << it->m_cabinPanel.m_ascendingSignal << "</ASC_INDICATION_" << i << ">";
                ss << "<DESC_INDICATION_" << i << ">" << it->m_cabinPanel.m_descendingSignal << "</DESC_INDICATION_" << i << ">";
                if(it->m_cabinController.m_pCabinDoors.count(CDoor::LEFT))
                	ss << "<CABINDOOR_OPENING_" << i<< "_L>"
                	<< it->m_cabinController.m_pCabinDoors[CDoor::LEFT]->openingPercentage()
                	<< "</CABINDOOR_OPENING_" << i << "_L>";
                if(it->m_cabinController.m_pCabinDoors.count(CDoor::RIGHT))
                    ss << "<CABINDOOR_OPENING_" << i<< "_R>"
                    << it->m_cabinController.m_pCabinDoors[CDoor::RIGHT]->openingPercentage()
                    << "</CABINDOOR_OPENING_" << i << "_R>";
                ss << "<CABINPANEL_" << i<< ">";
                for(unsigned short k=0; k<m_pElevatorSystem->m_numFloors; k++)
                {
                    ss << "<KEY_FLOOR_" << i<< "_" << k <<">"
                    	<< it->m_cabinPanel.m_floorKeySignals[k] << "</KEY_FLOOR_"
                    	<< i<< "_" << k << ">";
                }
                    ss << "<KEY_OPEN_DOORS_" << i<< ">"<< it->m_cabinPanel.m_openDoorsKeySignal
                    	<< "</KEY_OPEN_DOORS_" << i<< ">";
                    ss << "<KEY_CLOSE_DOORS_" << i<< ">" << it->m_cabinPanel.m_closeDoorsKeySignal
                    	<< "</KEY_CLOSE_DOORS_" << i<< ">";
                    ss << "<KEY_HIGH_PRIORITY_" << i<< ">"<< it->m_cabinPanel.m_highPriorityKeySignal
                    	<< "</KEY_HIGH_PRIORITY_" << i<< ">";
                    ss << "<FLOOR_INDICATION_" << i<< ">"<< it->m_cabinPanel.m_currentFloorSignal
                    	<< "</FLOOR_INDICATION_" << i<< ">";
                ss << "</CABINPANEL_" << i<< ">";
#endif
#if defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
                for(unsigned short k=0; k<m_pElevatorSystem->m_numFloors; k++)
                {
                	if(it->m_cabinController.m_pEntrances.count(k))
                	{
                		ss << "<ENTRANCEPANEL_" << i << "_" << k <<">";
                        	ss << "<KEY_UP_" << i<< "_" << k <<">"
                        		<< it->m_cabinController.m_pEntrances[k]->m_entrancePanel.m_upSignal
                        		<< "</KEY_UP_" << i << "_" << k <<">";
                        	ss << "<KEY_DOWN_" << i<< "_" << k <<">"
                        		<< it->m_cabinController.m_pEntrances[k]->m_entrancePanel.m_downSignal
                        		<< "</KEY_DOWN_" << i << "_" << k <<">";
                        	ss << "<EP_FLOOR_INDICATION_" << i <<  "_" << k << ">"
                        		<< it->m_cabinController.m_pEntrances[k]->m_entrancePanel.m_currentFloorSignal
                        	  	<< "</EP_FLOOR_INDICATION_" << i << "_" << k << ">";
                        ss << "</ENTRANCEPANEL_" << i<< "_" << k <<">";
                        ss << "<ENTRANCEDOOR_OPENING_" << i << "_" << k << ">"
                        	<< it->m_cabinController.m_pEntrances[k]->m_entranceDoor.openingPercentage()
                        	<< "</ENTRANCEDOOR_OPENING_" << i << "_" << k <<">";
                	}
                }
#endif
            ss << "</ELEVATOR_" << i << ">";
            i++;
        }
        ss << "</UPDATE>";
    ss << "</SERVER>";
    sendString(ss.str());
}

/*! \fn void CSimulator::connectToClient(unsigned int port)
 * \brief Stellt die TCP-Verbindung zum Client her und initialisiert die
 *  Visualisierung
 * \param port Port-Nummer
 * \return true, falls der Client-Socket akzeptiert wurde, false andernfalls
 */
bool CSimulator::connectToClient(unsigned int port)
{
#ifndef DEACTIVATE_TCP
#ifdef RUN_LOCAL_CLIENT
	if((bool)FindWindow(NULL, "ElevatorSim"))
	{
		std::cout << "Simulator: ElevatorSim seems to be already open (found a window named 'ElevatorSim').\n" << std::flush;
	}
	else
	{
		std::cout << "Simulator: Run ElevatorSim...\n" << std::flush;
		HINSTANCE fileError;
		if((long long)(fileError=ShellExecute(NULL, "open", "ElevatorSim.exe", NULL, NULL, SW_SHOW)) <= 32)
		{
			std::cerr << "ERROR: CSimulator::connectToClient(): Could not open ElevatorSim.exe, error code "
				<< fileError << "\n" << std::flush;
			return false;
		}
		Sleep(2500);
	}
#endif

    std::cout << "Simulator: Initialize Winsock... " << std::flush;
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        std::cerr << "ERROR: CSimulator::connectToClient(): While initializing Winsock, error code "
        	<< WSAGetLastError() << "\n" << std::flush;
        return false;
    }
    std::cout << "done.\n" << std::flush;

    //Create server socket
    std::cout << "Simulator: Create socket... " << std::flush;
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        std::cerr << "ERROR: CSimulator::connectToClient(): While creating socket, error code "
        	<< WSAGetLastError() << "\n" << std::flush;
        return false;
    }
    std::cout << "done.\n" << std::flush;

    //Prepare the sockaddr_in structure for the server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );

    //Bind server
    std::cout << "Simulator: Bind on port " << port << "... " << std::flush;
    if(bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        std::cerr << "ERROR: CSimulator::connectToClient(): While binding, error code "
        	<< WSAGetLastError() << "\n" << std::flush;
        return false;
    }
    std::cout << "done.\n" << std::flush;
    std::cout << "Simulator: Listening...\n" << std::flush;
    // Server: Listen to incoming connections
    listen(s , 3);
    c = sizeof(struct sockaddr_in);

    // See if client connection pending
    FD_ZERO(&readSet);
    FD_SET(s, &readSet);
    timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;  // Timeout
    timeout.tv_usec = 0;
    if(select(s, &readSet, NULL, NULL, &timeout) == 1)
    {
        // Accept client socket
        new_socket = accept(s , (struct sockaddr *)&client, &c);
        if (new_socket == INVALID_SOCKET)
        {
            std::cerr << "ERROR: CSimulator::connectToClient(): Accept failed, error code "
            	<< WSAGetLastError() << "\n" << std::flush;
            return false;
        }
    }
    else
    {
        std::cerr << "ERROR: CSimulator::connectToClient(): Accept timeout\n" << std::flush;
        return false;
    }
    std::cout << "Connection accepted.\n" << std::flush;
#endif

    std::string msg="<SERVER><MESSAGE>CONNECTION_ACCEPTED</MESSAGE></SERVER>";
    sendString(msg);
    sendElevatorSystemConfiguration();
    return true;
}

/*! \fn void CSimulator::sendString(std::string str)
 * \brief Sendet den angegebenen String an den Client
 * \param str Zu sendender String
 */
void CSimulator::sendString(std::string str)
{
#ifndef DEACTIVATE_TCP
    send(new_socket, (str+"\r\n").c_str(), (str.length())+2 , 0);
#endif
#ifdef DBG_TCP
    std::cout << str +"\r"+"\n" << std::flush;
#endif
}

/*! \fn void CSimulator::run()
 * \brief Endlosschleife, die die globale Simulationszeit mit jedem Durchlauf
 *  hochzaehlt und periodisch alle Komponenten des Aufzugssystems zum Weiterarbeiten
 *  veranlasst. Ausserdem werden die Client-Aktionen entgegengenommen und darauf reagiert
 *  (handleClientActions()). Schliesslich wird die Visualisierung aktualisiert
 *  (updateClient()).
 *  Die Schleife wird verlassen, sobald die Verbindung zum Client aufgehoben wird,
 *  anschliessend wird die TCP-Verbindung aufgeraeumt (Socket geschlossen)
 */
void CSimulator::run()
{
	while(!isStarted && !clientClosed && isConnected())
		handleClientActions();
    while(isConnected() && !clientClosed) // Naechster Simulationsschritt
    {
    	t++;
    	m_pElevatorSystem->m_systemController.work();
    	for(std::list<CCabinController*>::iterator it=m_pElevatorSystem->m_systemController.m_pCabinControllers.begin();
    			it!=m_pElevatorSystem->m_systemController.m_pCabinControllers.end();
    			it++)
    	{
    		(*it)->work();
    		(*it)->m_pTimer->work();
#if defined(LABOR2_VOR) || defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
    		if((*it)->m_pCabinDoors.count(CDoor::LEFT)) (*it)->m_pCabinDoors[CDoor::LEFT]->work();
    		if((*it)->m_pCabinDoors.count(CDoor::RIGHT)) (*it)->m_pCabinDoors[CDoor::RIGHT]->work();
    		(*it)->m_pCabinPanel->work();
#endif
#if defined(LABOR2_DURCH) || defined(LABOR3_VOR) || defined(LABOR3_DURCH)
    		for(unsigned short k=0; k<m_pElevatorSystem->m_numFloors; k++)
    		{
    			if((*it)->m_pEntrances.count(k))
    			{
    				(*it)->m_pEntrances[k]->m_entrancePanel.work();
    				(*it)->m_pEntrances[k]->m_entranceDoor.work();
    			}
    		}
#endif
    		(*it)->m_pHeightSensor->work();
    		(*it)->m_pMotor->work();
    	}
    	updateClient();
    	handleClientActions();
    }
    closeTcpConnection();
}
