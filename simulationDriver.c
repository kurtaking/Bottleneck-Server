/****************************************************************
 cs2123p4Driver.c written by: Kurt King              3-20-2015
 
 Purpose:
     The driver file for this program
 Input:
     1. Input A (same for alternatives A and current) - p4ACinput.txt
          lWidgetNr   iStep1tu    iStep2tu    iArrivalDelta
              %ld         %d          %d            %d
        
        Sample Input from p4ACinput.txt
               1  15  13  13
               2  31  15  16
               3  13  12  19
               4  10  12   5
               5   5  18   7

    2. Input B (for alternative B) - p4Binput.txt
         
          lWidgetNr   iStep1tu    iStep2tu    iArrivalDelta
              %ld         %d          %d            %d
 
        Sample Input from p4ACinput.txt

               1  25  13  13
               2  24  15  16
               3  17  12  19
               4  25  12   5
               5  18  18   7
 
 Results:
 Returns:
     Statistics produced running simulation:
         - Number of Widgets processed
         - Average time in system (from arrival to completion at step 2)
         - Average queue time step 1
         - Average queue time step 2 (not produced for Alternative B or C
 Notes:
     1. DRIVER file includes main function for program
     2. Contains functions provided by Dr. Clark for program 4
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "simulation.h"


// MAIN PROGRAM FOR DRIVER
int main(int argc, char *argv[])
{
    Simulation sim;

    // get the file name from the command argument switches
    sim = processSwitches(argc, argv);  

    // generates the arrival of the widgets
    generateArrival(sim);
    
    // runs simulation based on the sim->cRunType (Either, A, B, or C)
    if (sim->cRunType == 'A')
        runSimulationA(sim, 300);
    else
        runSimulationBC(sim, 300);

    // free's the sim
    free(sim->eventList);
    free(sim);
    return 0;
}

/****************** Queue Functions  ****************************/

/*********************** newQueue ********************************
 Queue newQueue(char szQueueNm[])
 Purpose:
     Creates a new queue
 Parameters:
     I   szQueueNm[]        // pointer to name for new Queue        
 Returns:
     Array of queues via parameter
 Notes:
 ****************************************************************/
Queue newQueue(char szQueueNm[])
{
    Queue q = (Queue)malloc(sizeof(QueueImp));
    // Mark the list as empty
    q->pHead = NULL;   // empty list
    q->pFoot = NULL;   // empty list
    q->lQueueWaitSum = 0;
    q->lQueueWidgetTotalCount = 0;
    strcpy(q->szQName, szQueueNm);
    return q;
}

/********************** allocNodeQ *******************************
 NodeQ *allocNodeQ(Queue q, QElement value)
 Purpose:
     Allocates memory for a new Queue Node
 Parameters:
     Queue q
     Element value
 Returns:
     pNew, which is a new queue node
 Notes:
 ****************************************************************/
NodeQ *allocNodeQ(Queue q, QElement value)
{
    NodeQ *pNew;
    pNew = (NodeQ *)malloc(sizeof(NodeQ));
    if (pNew == NULL)
        ErrExit(ERR_ALGORITHM, "No available memory for queue");
    pNew->element = value;
    pNew->pNext = NULL;
    return pNew;
}

/*********************** insertQ ********************************
 void insertQ(Queue queue, QElement element)
 Purpose:
     Inserts a new element value into the foot of the list
 Parameters:
     Queue queue
     Element element
 Returns:
     N/A
 Notes:
     1. Insertion is at the foot of the list so it impacts foot. Head
        can also be affected when the queue is empty.
     2. Uses allocNodeQ function
 ****************************************************************/
void insertQ(Queue queue, QElement element)
{
    NodeQ *pNew;
    pNew = allocNodeQ(queue, element);
    // check for empty
    if (queue->pFoot == NULL)
    {
        queue->pFoot = pNew;
        queue->pHead = pNew;
    }
    else
    {   // insert after foot
        queue->pFoot->pNext = pNew;
        queue->pFoot = pNew;
    }
}

/*********************** removeQ ********************************
 int removeQ(Queue queue, QElement *pFromQElement)
 Purpose:
     Removes a node from the front of the list and returns the
     element (via the parameter list)
 Parameters:
     Queue queue
     QElement *pFromQElement
 Returns:
     FUCTIONALLY:
        1. Returns TRUE if there is an element returned
        2. Otherwise, returns FALSE
 Notes:
     1. removeQ must also free the memory occupied by that node
     2. Special cases to consider:
         a. Queue is empty when removeQ is called
         b. Queue is empty after removing the last node
 ****************************************************************/
int removeQ(Queue queue, QElement *pFromQElement)
{
    NodeQ *pRemove;
    // check for empty
    if (queue->pHead == NULL)
        return FALSE;
    pRemove = queue->pHead;
    *pFromQElement = pRemove->element;
    queue->pHead = pRemove->pNext;
    // Removing the node could make the list empty.
    // See if we need to update pFoot, due to empty list
    if (queue->pHead == NULL)
        queue->pFoot = NULL;
    free(pRemove);
    return TRUE;
}


/***************** Linked List Functions  **********************/

/******************** removeLL **********************************
 int removeLL(LinkedList list, Event *pValue)
 Purpose:
    Removes a node from the front of a linked list and returns
    the Element structure (via the parameter list).
 Parameters:
    LinkedList list
    Event *pValue
 Returns:
    FALSE - If the list is empty before removing the first node
    TRUE - Otherwise (from statement above)
 Notes:
    1. Returns the Element structure via the parameter list
    2. Uses free function
 ****************************************************************/
int removeLL(LinkedList list, Event  *pValue)
{
    NodeLL *p;
    // list is empty
    if (list->pHead == NULL)
        return FALSE;
    *pValue = list->pHead->event;
    p = list->pHead;
    list->pHead = list->pHead->pNext;
    free(p);
    return TRUE;
}

/******************** insertOrderedLL ****************************
 NodeLL *insertOrderedLL(LinkedList list, Event value)
 Purpose:
     Inserts an element into an ordered linked list.
 Parameters:
     LinkedList list
     Event value
 Returns:
     1. If it already exists in list, it returns the pointer to that
     specific node.
     2. Otherwise, it returns the pointer to the new node.
 Notes:
     1. Uses searchLL function
     2. Uses allocateNodeLL function
 *****************************************************************/
NodeLL *insertOrderedLL(LinkedList list, Event value)
{
    NodeLL *pNew, *pFind, *pPrecedes;
    
    // see if it already exists
    pFind = searchLL(list, value.iTime, &pPrecedes);
    
    // doesn't already exist.  Allocate a node and insert.
    pNew = allocateNodeLL(list, value);
    
    // Check for inserting at the beginning of the list
    // this will also handle when the list is empty
    if (pPrecedes == NULL)   //want to check if value of pPrecedes is null
    {
        pNew->pNext = list->pHead;
        list->pHead = pNew;
    }
    else
    {
        pNew->pNext = pPrecedes->pNext;
        pPrecedes->pNext = pNew;
    }
    return pNew;
}

/******************** searchLL **********************************
 NodeLL *searchLL(LinkedList list, int match, NodeLL **ppPrecedes)
 Purpose:
     Searches for match in the list.
 Parameters:
     LinkedList list
     int match
     NodeLL **ppPrecedes
 Returns:
     FUNCTIONALLY:
         1. If found, returns the address of the node containing
         it.
         2. If NOT found, it returns NULL
     ELSE
         1. Also will return a pointer to the node that precedes
         where match exists (or should exist if not found)
 Notes:
 ****************************************************************/
NodeLL *searchLL(LinkedList list, int match, NodeLL **ppPrecedes)
{
    NodeLL *p, *pHead;
    // used when the list is empty or we need to insert at the beginning
    *ppPrecedes = NULL;
    
    // Traverse through the list looking for where the key belongs or
    // the end of the list.
    for (p = list->pHead; p != NULL; p = p->pNext)
    {
        if(match < p->event.iTime)
            return NULL;
        *ppPrecedes = p;
    }
    
    // Not found return NULL
    return NULL;
}

/******************** newLinkedList ******************************
 LinkedList newLinkedList()
 Purpose:
     Creates a new linked list.
 Parameters:
     N/A
 Notes:
     Since the heap memory manager is managing free memory, we
     don't need to initialize the free list.
 *****************************************************************/
LinkedList newLinkedList()
{
    LinkedList list = (LinkedList) malloc(sizeof(LinkedListImp));
    // Mark the list as empty
    list->pHead = NULL;   // empty list
    return list;
}

/******************** allocateNodeLL *****************************
 NodeLL *allocateNodeLL(LinkedList list, Event value)
 Purpose:
     Allocates memory for a new node.
 Parameters:
     LinkedList list
     Event value
 Returns:
     1. Error if no available memory for linked list
     2. pNew - a new node
 Notes:
 *****************************************************************/
NodeLL *allocateNodeLL(LinkedList list, Event value)
{
    NodeLL *pNew;
    pNew = (NodeLL *)malloc(sizeof(NodeLL));
    if (pNew == NULL)
        //ErrExit(ERR_ALGORITHM, "No available memory for linked list");
    pNew->event = value;
    pNew->pNext = NULL;
    return pNew;
}

/******************** ErrExit **************************************
 void ErrExit(int iexitRC, char szFmt[], ... )
 Purpose:
     Prints an error message defined by the printf-like szFmt and the
     corresponding arguments to that function.  The number of
     arguments after szFmt varies dependent on the format codes in
     szFmt.
     It also exits the program with the specified exit return code.
 Parameters:
     I   int iexitRC             Exit return code for the program
     I   char szFmt[]            This contains the message to be printed
                                 and format codes (just like printf) for
                                 values that we want to print.
     I   ...                     A variable-number of additional arguments
                                 which correspond to what is needed
                                 by the format codes in szFmt.
 Notes:
     - Prints "ERROR: " followed by the formatted error message specified
       in szFmt.
     - Prints the file path and file name of the program having the error.
       This is the file that contains this routine.
     - Requires including <stdarg.h>
 Returns:
     Returns a program exit return code:  the value of iexitRC.
 **************************************************************************
void ErrExit(int iexitRC, char szFmt[], ... )
{
    va_list args;               // This is the standard C variable argument list type
    va_start(args, szFmt);      // This tells the compiler where the variable arguments
                                // begins.  They begin after szFmt.
    printf("ERROR: ");
    vprintf(szFmt, args);       // vprintf receives a printf format string and  a
                                // va_list argument
    va_end(args);               // let the C environment know we are finished with the
                                // va_list argument
    printf("\n");
    exit(iexitRC);
} */

/******************** exitUsage *****************************
 void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
 Purpose:
     In general, this routine optionally prints error messages and diagnostics.
     It also prints usage information.
 
     If this is an argument error (iArg >= 0), it prints a formatted message
     showing which argument was in error, the specified message, and
     supplemental diagnostic information.  It also shows the usage. It exits
     with ERR_COMMAND_LINE.
 
     If this is a usage error (but not specific to the argument), it prints
     the specific message and its supplemental diagnostic information.  It
     also shows the usage and exist with ERR_COMMAND_LINE.
 
     If this is just asking for usage (iArg will be -1), the usage is shown.
     It exits with USAGE_ONLY.
 Parameters:
     I int iArg                      command argument subscript or control:
                                     > 0 - command argument subscript
                                     0 - USAGE_ONLY - show usage only
                                     -1 - USAGE_ERR - show message and usage
     I char *pszMessage              error message to print
     I char *pszDiagnosticInfo       supplemental diagnostic information
 Notes:
     This routine causes the program to exit.
 **************************************************************************/
void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
{
    switch (iArg)
    {
        case USAGE_ERR:
            fprintf(stderr, "Error: %s %s\n"
                    , pszMessage
                    , pszDiagnosticInfo);
            break;
        case USAGE_ONLY:
            break;
        default:
            fprintf(stderr, "Error: bad argument #%d.  %s %s\n"
                    , iArg
                    , pszMessage
                    , pszDiagnosticInfo);
    }
    // print the usage information for any type of command line error
    fprintf(stderr, "p2 -c customerFileName -q queryFileName\n");
    if (iArg == USAGE_ONLY)
        exit(USAGE_ONLY);
    else
        exit(ERR_COMMAND_LINE);
}


/************************* processSwitches *****************************
 Simulation processSwitches(int argc, char *argv[])
 Purpose:
     Processes the command line arguements for the program. 
 Parameters:
     I    int argc
     I    char *arg[]
 Returns:
     A newly allocated sim containing information over verbose and which
     simulation the user chose to run
 Notes:
     1. -v for verbose
     2. -aA for running simulation A
     3. -aB for running simulation B
     4. -aC for runnign simulation C (or current)
***********************************************************************/ 
Simulation processSwitches(int argc, char *argv[])
{
    Simulation sim = newSimulation();       // need to create a new sim here to store whether verbose
                                            // and the run type the user chooses
    

    // checks immediatley if user used too many command line arguments
    if (argc > 3)
        ErrExit(ERR_COMMAND_LINE, "Too many arguments, see usage \n", "p4 [-v] [-aC, -aB, -aA]\n");

    // if user only has one commany line argument, run type will automatically be alternative A
    // and verbose will be set to FALSE
    if (argc == 1)
    {
        printf("Running Default: alternative A \n");
        sim->bVerbose = FALSE;
        sim->cRunType = 'A';
    }

    // run type is default (alternative A)
    if (argc == 2)
    {
        if (strcmp(argv[1], "-v") == 0)  
        {
            sim->bVerbose = TRUE;
            sim->cRunType = 'A';
        }
        else if (strcmp(argv[1], "-aA") == 0)
        {
            sim->bVerbose = FALSE;
            sim->cRunType = 'A';
        }
        else if (strcmp(argv[1], "-aB") == 0)
        {
            sim->bVerbose = FALSE;
            sim->cRunType = 'B';
        }
        else if (strcmp(argv[1], "-aC") == 0)
        {
            sim->bVerbose = FALSE;
            sim->cRunType = 'C';
        }
        else
        {
            ErrExit(ERR_COMMAND_LINE, "Argument should be a run type [-aA -aB -aC] or verbose switch [-v] \n");
        }
    }
    
    // expect a run type as arg 2
    if (argc == 3)
    {
        if (strcmp(argv[1], "-v") == 0)  
        {
            sim->bVerbose = TRUE;
        }
        else if (strcmp(argv[1], "-aA") == 0)
        {
            sim->cRunType = 'A';
        }
        else if (strcmp(argv[1], "-aB") == 0)
        {
            sim->cRunType = 'B';
        }
        else if (strcmp(argv[1], "-aC") == 0)
        {
            sim->cRunType = 'C';
        } 
        else
        {
            ErrExit(ERR_COMMAND_LINE, "Argument 1 should be a run type [-aA -aB -aC] or verbose switch [-v]\n");
        }

        if((strcmp(argv[2], "-v") == 0) && (sim->cRunType != 0))    //arg 1 should have been a run type
        {
            sim->bVerbose = TRUE;
        }
        else if((strcmp(argv[2], "-aA") == 0) && (sim->bVerbose != 0))
        {
            sim->cRunType = 'A';
        }
        else if((strcmp(argv[2], "-aB") == 0) && (sim->bVerbose != 0))
        {
            sim->cRunType = 'B';
        }
        else if((strcmp(argv[2], "-aC") == 0) && (sim->bVerbose != 0)) 
        {
            sim->cRunType = 'C';
        } 
        else
        {
            ErrExit(ERR_COMMAND_LINE, "Argument should be a run type [-aA -aB -aC] or verbose switch [-v]\n");
        }
    }
    
    // print off whether the user signaled for verbose and which run type the user chose
    if(sim->bVerbose == TRUE)
        printf("Simulation Arguments: Verbose: ON Run Type: %c \n\n", sim->cRunType);
    else
        printf("Simulation Arguments: Verbose: OFF Run Type: %c \n\n", sim->cRunType);

    return sim;
}
