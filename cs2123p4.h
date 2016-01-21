/*********************************************************************
cs2123p4.h
Purpose:
    Defines constants:
        max constants
        error constants
        event type constants
        boolean constants
    Defines typedef for
        Widget
        Event (instead of Element)
        For Linked List
            NodeLL
            LinkedListImp
            LinkedList
        For Queues
            QElement
            NodeQ
            QueueImp
            Queue
        For the Servers
            ServerImp
            Server
         For the Simulation
            SimulationImp
            Simulation
 Protypes
        Linked List functions
        Queue functions
        Simulation functions
        Utility functions provided by Larry previously (program 2)
Notes:

**********************************************************************/

/*** constants ***/
// Maximum constants

#define MAX_TOKEN 50            // Maximum number of actual characters for a token
#define MAX_LINE_SIZE 100       // Maximum number of character per input line
#define MAX_ARRIVAL_TIME 300

// Error constants (program exit values)
#define ERR_COMMAND_LINE    900    // invalid command line argument
#define ERR_ALGORITHM       903    // Error in algorithm - almost anything else
#define ERR_BAD_INPUT       503    // Bad input 

// Error Messages 
#define ERR_MISSING_SWITCH          "missing switch"
#define ERR_EXPECTED_SWITCH         "expected switch, found"
#define ERR_MISSING_ARGUMENT        "missing argument for"

// Event Constants
#define EVT_ARRIVAL          1     // when a widget arrives
#define EVT_SERVER1_COMPLETE 2     // when a widget completes with server1
#define EVT_SERVER2_COMPLETE 3     // when a widget completes with server2

// exitUsage control 
#define USAGE_ONLY          0      // user only requested usage information
#define USAGE_ERR           -1     // usage error, show message and usage information

// boolean constants
#define FALSE 0
#define TRUE 1

/*** typedef ***/

// Widget typedef 
typedef struct
{
    long lWidgetNr;         // Identifies a widget
    int iStep1tu;           // Step 1 time units
    int iStep2tu;           // Step 2 time units
    int iArrivalTime;       // Arrival time in tu
} Widget;

// Event typedef
typedef struct
{
    int iEventType;         // The type of event as an integer:
                            //    EVT_ARRIVAL - arrival event
                            //    EVT_SERVER1_COMPLETE - servicing by server 1 is complete
                            //    EVT_SERVER2_COMPLETE - servicing by server 2 is complete
    int iTime;              // The time the event will occur 
    Widget widget;          // The widget involved in the event.
} Event;

// typedefs for the Linked Lists used for the event list
typedef struct NodeLL
{
    Event event;
    struct NodeLL *pNext;
} NodeLL;

typedef struct
{
    NodeLL *pHead;
} LinkedListImp;

typedef LinkedListImp *LinkedList;

// typedefs for the queues

typedef struct
{
    Widget widget;
    int iEnterQTime;                // time widget was inserted in queue
} QElement;

typedef struct NodeQ
{
    QElement element;
    struct NodeQ *pNext;
} NodeQ;

typedef struct 
{
    NodeQ *pHead;
    NodeQ *pFoot;
    long lQueueWaitSum;             // Sum of wait times for the queue
    long lQueueWidgetTotalCount;    // Total count of widgets that entered queue
    char szQName[12];
} QueueImp;

typedef QueueImp *Queue;

// typedefs for server
typedef struct
{
    char szServerName[12];
    int bBusy;                      // TRUE - server is busy, FALSE - server is free
    Widget widget;                  // Widget the server is currently working
} ServerImp;
typedef ServerImp *Server;

// typedefs for the Simulation
typedef struct
{
    int iClock;                     // clock time
    int bVerbose;                   // When TRUE, this causes printing of event information
    long lSystemTimeSum;            // Sum of times widgets are in the system
    long lWidgetCount;              // The number of widgets processed 
    char cRunType;                  // A - Alternative A, B - Alternative B, C - Current
    LinkedList eventList;
} SimulationImp;
typedef SimulationImp *Simulation;


/**********   prototypes ***********/

// linked list functions - you must provide the code for these (see course notes)
int removeLL(LinkedList list, Event  *pValue);
NodeLL *insertOrderedLL(LinkedList list, Event value);
NodeLL *searchLL(LinkedList list, int match, NodeLL **ppPrecedes);
LinkedList newLinkedList();
NodeLL *allocateNodeLL(LinkedList list, Event value);

// queue functions
int removeQ(Queue queue, QElement *pFromQElement);
void  insertQ(Queue queue, QElement element);
NodeQ *allocNodeQ(Queue queue, QElement value);
Queue newQueue(char szQueueNm[]);

// functions to create new Simulations and new Servers
Simulation newSimulation();
Server newServer(char szServerNm[]);

// simulation functions - you must provide code for this
void runSimulationA(Simulation simulation, int iTimeLimit);
void runSimulationBC(Simulation simulation, int iTimeLimit);
void generateArrival(Simulation sim);
void arrival(Simulation, Widget *widget);
void queueUp(Simulation sim, Queue queueTeller, Widget *widget);
void seize(Simulation sim, Queue queueTeller, Server serverTeller);
void release(Simulation sim, Queue queueTeller, Server serverTeller, Widget *widget);
void leaveSystem(Simulation sim, Widget widget);
void printStatistics(Simulation sim, Queue queue1, Queue queue2);

// functions in most programs, but require modifications
void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo);
Simulation processSwitches(int argc, char *argv[]);

// Utility routines provided by Larry (copy from program #2)
void ErrExit(int iexitRC, char szFmt[], ...);
