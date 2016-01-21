/****************************************************************
 cs2123p4.c written by: Kurt King                     3-20-2015
 
 Purpose:
     This program takes widgets from a file and runs simulations on
     them. The output of the program shows the statistics for each
     simulation. Judging by the statistics, the user would be able
     to specify which simulation is the most efficient.
 Input:
     FILE for simulation A, B, and Current:
 
     Widget File:
         Input file stream which contains widget information:
            -Widget Name, Step 1 time units, Step 2 tu, and arrival time
 
         lWidgetNr  iStep1tu    iStep2tu    iArrivalDelta
           %ld         %d          %d            %d
 
 Results/Returns:
    Runs a simulation on the widget information scanned in from file
 Notes:
    1. THIS FILE CONTAINS FUNCTIONS I WROTE FOR PROGRAM 4
 ****************************************************************/
// If compiling using visual studio, tell the compiler not to give its warnings
// about the safety of scanf and printf

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cs2123p4.h"

/******************** newSimulation ******************************
 Simulation newSimulation()
 Purpose:
     Creates a new Simulation.
 Parameters:
     N/A
 Notes:
    1. Initially sets sim variables to 0
    2. Uses newLinkedList function
 *****************************************************************/
Simulation newSimulation()
{
    Simulation sim = (Simulation) malloc(sizeof(SimulationImp));
    sim->iClock = 0;
    sim->lSystemTimeSum = 0;
    sim->lWidgetCount = 0;
    sim->eventList = newLinkedList();
    return sim;
}

/************************  newServer *******************************
 Server newServer()
 Purpose:
     Creates a new Server and copies name for new server
 Parameters:
     I  char szServerNm[]   // pointer to name for new server
 Notes:
     1. Initially sets server->bBusy to FALSE
*******************************************************************/
Server newServer(char szServerNm[])
{
    Server s = (Server)malloc(sizeof(ServerImp));
    strcpy(s->szServerName, szServerNm);
    s->bBusy = FALSE;
    return s;
}

/********************** generateArrival ****************************
 void getTravelerData(Simulation sim)
 Purpose:
     Reads traveler information from file. The values are stored into
     an event and the event is then inserted into an eventList (linked list)
 Parameters:
     O   Simulation sim      // event list
 Returns:
     VIA PARAMETER:
         1. Returns a populated event list
 Notes:
    1. Uses interOrderedLL function
 *******************************************************************/
void generateArrival(Simulation sim)
{
    Event event;                        // creates a new event to store widgeti
                                        // and event information into
    char szInputBuffer[100];            // input buffer for fgets (entire input line)
    int iDeltaNextArrival = 0;          // next widgets arrival
    int iTmpTime = 0;                   // Saves current widget + next widgets arrival time
                                        // to get true arrival time for each widget

    // read text lines containing widget name, step 1 time units, step 2 time units, 
    //    and next widget's arrival time in time units until EOF
    while(fgets(szInputBuffer, 100, stdin) != NULL)
    {
        // copies widget information from file and stores it into the newly created widget
        sscanf(szInputBuffer, "%ld %d %d %d", &event.widget.lWidgetNr  // widget name
                                            , &event.widget.iStep1tu   // widget step 1 time units
                                            , &event.widget.iStep2tu   // widget step 2 time units
                                            , &iDeltaNextArrival);     // next widget's arrival time
      
        // creates the arrival node for each widget
        event.iEventType = EVT_ARRIVAL;
        event.widget.iArrivalTime = iTmpTime;
        event.iTime = iTmpTime;           
        
        // inserts the event into the event list
        insertOrderedLL(sim->eventList, event);
    
        iTmpTime += iDeltaNextArrival;   // increment iTmpTime by the next widget's arrival time
    }
}

/*******************************************************************
 void runSimulationBC(Simulation sim, int iTimeLimit)
 Purpose:
    Runs a simulation on the event list. Prints a table displaying
    the arrival and departures of travelers
 Parameters:
     I   Simulation simulation
     I   int iTimeLimit
 Returns:
     1. Does not return anything functionally
 Notes:
     1. Uses removeLL function
 *******************************************************************/
void runSimulationBC(Simulation sim, int iTimeLimit)
{
    Event event;                            // Creates a local event variable to store current
                                            // nodes event information into (uses in simulation evaluation)
    Server server1 = newServer("Server 1");           // Creates a new server - 
                                            //  - contains Server Name, whether it is busy or not, and a widget
 
    Queue queue1 = newQueue("Queue 1");                // Creates a new queue
   
    if (sim->bVerbose == TRUE)
        printf("%-4s %-6s %-10s\n", "TIME", "WIDGET", "EVENT");      //table header
    
    while (removeLL(sim->eventList, &event))
    {
        // buffer to stop arrivals after the time limit
        if (event.iEventType == EVT_ARRIVAL && event.iTime > iTimeLimit)
            continue;
        
        sim->iClock = event.iTime;  // advance clock to current event time
        
        // the switch evaluates the eventType 
        switch(event.iEventType)
        {
            case EVT_ARRIVAL:
                arrival(sim, &event.widget);
                queueUp(sim, queue1, &event.widget);
                seize(sim, queue1, server1);
                break;
            case EVT_SERVER1_COMPLETE:
                release(sim, queue1, server1, &event.widget);
                leaveSystem(sim, event.widget);
                break;
            default:
              ErrExit(ERR_ALGORITHM, "Unknown event type: %d\n", event.iEventType);
        }
    }
    // prints the averages produced by the simulation
    printStatistics(sim, queue1, queue1);

    // frees server and queue for simulation BC
    free(server1);
    free(queue1);
}

/*******************************************************************
 void runSimulationA(Simulation sim, int iTimeLimit)
 Purpose:
     Runs a simulation on the event list. Prints a table displaying
     the arrival and departures of travelers
 Parameters:
     I   Simulation simulation
     I   int iTimeLimit
 Returns:
     1. Does not return anything functionally
 Notes:
     1. Uses removeLL function
 *******************************************************************/
void runSimulationA(Simulation sim, int iTimeLimit)
{
    Event event;                            // Creates a local event variable to store current
                                            // nodes event information into (uses in simulation evaluation)
    Server server1 = newServer("Server 1");           // Creates a new server -
                                            //  - contains Server Name, whether it is busy or not, and a widget
    Server server2 = newServer("Server 2");            // creates the second server for our program (refer to server1 comments)
    Queue queue1 = newQueue("Queue 1");                // Creates a new queue
    Queue queue2 = newQueue("Queue 2");                // Creates a new queue
    
    if (sim->bVerbose == TRUE)
        printf("%-4s %-6s %-10s\n", "TIME", "WIDGET", "EVENT");      //table header
    
    while (removeLL(sim->eventList, &event))
    {
        // buffer to stop arrivals after the time limit
        if (event.iEventType == EVT_ARRIVAL && event.iTime > iTimeLimit)
            continue;
        
        sim->iClock = event.iTime;  // advance clock to current event time
        
        // the switch evaluates the eventType
        switch(event.iEventType)
        {
            case EVT_ARRIVAL:
                arrival(sim, &event.widget);
                queueUp(sim, queue1, &event.widget);
                seize(sim, queue1, server1);
                break;
            case EVT_SERVER1_COMPLETE:
                release(sim, queue1, server1, &event.widget);
                queueUp(sim, queue2, &event.widget);
                seize(sim, queue2, server2);
                break;
            case EVT_SERVER2_COMPLETE:
                release(sim, queue2, server2, &event.widget);
                leaveSystem(sim, event.widget);
                break;
            default:
                ErrExit(ERR_ALGORITHM, "Unknown event type: %d\n", event.iEventType);
        }
    }
    // prints the averages produced by the simulation
    printStatistics(sim, queue1, queue2);

    // frees servers and queues used in simulation A
    free(server1);
    free(server2);
    free(queue1);
    free(queue2);
}

/*************************** arrival ****************************
 arrival (Simulation sim, Widget *widget)
 Purpose:
     Prints when the widget first arrives
 Parameters:
     I Simulation sim       
     I Widget *widget       // a pointer to the widget in the current event
 Returns:
 Notes:
*****************************************************************/
void arrival(Simulation sim, Widget *widget)
{
    // prints the arrival time and name for current widge and an arrival statement
    if(sim->bVerbose == TRUE)
        printf("%4d %6ld %-10s\n", sim->iClock, widget->lWidgetNr, "Arrived");
}

/*************************** queueUp **********************************
 queueUp(Simulation sim, Queue queueTeller, Widget *widget)
 Purpose:
     This function inserts a widget into a queue based on 
     the queueTeller.
 Parameters:
     I  Simulation sim          // Simulation we are running on
     I  Queue queueTeller       // Current queue we are dealing with
                                //      - Queue 1 or Queue 2 
     I  Widget *widget          // The current widget we are processing
 Returns:
 Notes:
     1. Updates statistics about the queue
        a. iEnterQTime
        b. lQueueWaitSum
        c. lQueueWidgetTotalCount
***********************************************************************/
void queueUp(Simulation sim, Queue queueTeller, Widget *widget) 
{
    QElement qElement;          // newly created node to store into queue
    
    // update node with widget information
    qElement.widget= *widget;
    qElement.iEnterQTime = sim->iClock;
   
    // inserts the newly created node into the queue
    insertQ(queueTeller, qElement);

    if(sim->bVerbose == TRUE)
        printf("%4d %6ld Enter %7s\n", sim->iClock, widget->lWidgetNr, queueTeller->szQName);
}

/**************************** seize ******************************
 seize(Simulation sim, Queue queueTeller, Server serverTeller)
 Purpose:
     Seizes a widget from the queue.
 Parameters:
     I  Simulation sim          // current sim we are running
     I  Queue queueTeller       // current queue 
                                //  either queue1 or queue2
     I  Server serverTeller     // current server
                                //  either server1 or server2
 Returns:
     Does not return anything fuctionally, but does insert the newly
     created server1 or server2 complete event into the linked list.
 Notes:
    1. Uses removeQ function
*****************************************************************/
void seize(Simulation sim, Queue queueTeller, Server serverTeller)
{
   QElement fill;           // element returned from removeQ
   Event event;             // Server complete event
   int iWaitTime;           // The wait time for each widget in the queue
   
   // checks to see if widget is busy 
   if(serverTeller->bBusy == FALSE)
   {
         // if not busy
         // remove widget from queue
         if(removeQ(queueTeller, &fill))
         {
             iWaitTime = (sim->iClock - fill.iEnterQTime);   // wait is equal to current clock time minus the time the widget
                                                             // enters the system
             if(sim->bVerbose == TRUE)
             {
                 printf("%4d %6ld Leave %s Waited: %d\n", sim->iClock, fill.widget.lWidgetNr, queueTeller->szQName, iWaitTime);
                 printf("%4d %6ld Seized %s\n", sim->iClock, fill.widget.lWidgetNr, serverTeller->szServerName);
             }

             // set server to be busy 
             serverTeller->bBusy = TRUE;

             //create completion event for server
             event.widget = fill.widget;
       
             if(sim->cRunType == 'A')
             {
                //checks to see whether it is server 1 or 2 to set event type and event time properly
                if(strcmp(serverTeller->szServerName, "Server 1") == 0)
                {
                     event.iEventType = EVT_SERVER1_COMPLETE;
                     event.iTime = sim->iClock + fill.widget.iStep1tu;
                }
                else if(strcmp(serverTeller->szServerName, "Server 2") == 0)
                {
                     event.iEventType = EVT_SERVER2_COMPLETE;
                     event.iTime = sim->iClock + fill.widget.iStep2tu;
                }
             }
             else   // for alternative b and c - only have 1 server that completes both step1 and step2
             {
                 event.iEventType = EVT_SERVER1_COMPLETE;
                 event.iTime = (sim->iClock + fill.widget.iStep1tu + fill.widget.iStep2tu);

             }

             queueTeller->lQueueWaitSum += iWaitTime;   // add the wait time for each widget to the total wait sum
             queueTeller->lQueueWidgetTotalCount++;     // increment queue count by one each time you seize a widget
             
             // inserts the event into the list
             insertOrderedLL(sim->eventList, event);
         }
     }
}

/*************************** release ****************************
 release(Simulation sim, Queue queueTeller, Server serverTeller, Widget *widget)
 Purpose:
     Releases a widget from the server, sets server to no longer busy, and checks
     to see if it can start working on another widget
 Parameters:
     I      Simulation sim              //simulation we are running
     I      Queue queueTeller           //current queue we are dealing with
     I      Server serverTeller         //current server we are working on
     I      Widget *widget              //current widget being released from server
 Returns:
 Notes:
     1. Calls seize
*****************************************************************/
void release(Simulation sim, Queue queueTeller, Server serverTeller, Widget *widget)
{
    // set server to no longer busy
    serverTeller->bBusy = FALSE;

    // update and print stats
    if (sim->bVerbose == TRUE)
        printf("%4d %6ld Released %6s\n", sim->iClock, widget->lWidgetNr, serverTeller->szServerName);
    
    // checks to see if Server could potentially start working on another widget
    seize(sim, queueTeller, serverTeller);
}

/************************* leaveSystem **************************
 leaveSystem(Simulation sim, Widget widget)
 Purpose:
     Displays when a widget leaves the system
 Parameters:
     I   Simulation sim         // current sim we are running
     I   Widget widget          // current widget we are processing
 Returns:
     N/A
 Notes:
     1. Updates System Time Sum by the current clock minus widget's arrival time
     2. Updates the System's widget count by one
*****************************************************************/
void leaveSystem(Simulation sim, Widget widget)
{
    if (sim->bVerbose == TRUE)
        printf("%4d %6ld Exit System, in system %2d\n", sim->iClock, widget.lWidgetNr, (sim->iClock - widget.iArrivalTime));
    sim->lSystemTimeSum += (sim->iClock - widget.iArrivalTime);     //Updates system time sum
    sim->lWidgetCount++;                                            //Updates system widget count
}

/************************* printStatistics **************************
 void printStatistics()
 Purpose:
     Prints the statistics the simulation produces
        -Total widgets processed
        -Average Queue Time for Server 1
        -Average Queue Time for Server 2
        -Average Time in System
 Parameters:
     I      Simulation sim          // the simulation we are printing stats for
     I      Queue queue1            // Queue 1's stats
     I      Queue queue2            // Queue 2's stats
 Returns:
     N/A
 Notes:
     1. Alternative B and C only contain 1 Server and 1 Queue
        Therefore, statistics printed will be different
*****************************************************************/
void printStatistics(Simulation sim, Queue queue1, Queue queue2)
{
    // if Alternative A, else Alternative B and C
    if(sim->cRunType == 'A')
    {
        printf("\t Total widgets processed         = %5.1lf \n", (sim->lWidgetCount)+0.0);
        printf("\t Average Queue Time for Server 1 = %5.1lf \n", ((queue1->lQueueWaitSum+0.0) / queue1->lQueueWidgetTotalCount));
        printf("\t Average Queue Time for Server 2 = %5.1lf \n", ((queue2->lQueueWaitSum+0.0) / queue2->lQueueWidgetTotalCount));
        printf("\t Average Time in System          = %5.1lf \n", ((sim->lSystemTimeSum+0.0) / sim->lWidgetCount));
    }
    else
    {
        printf("\t Total widgets processed         = %5.1lf \n", (sim->lWidgetCount+0.0));
        printf("\t Average Queue Time for Server 1 = %5.1lf \n", ((queue1->lQueueWaitSum+0.0) / queue1->lQueueWidgetTotalCount));
            printf("\t Average Time in System          = %4.1lf \n", ((sim->lSystemTimeSum+0.0) / sim->lWidgetCount));
    }
}
