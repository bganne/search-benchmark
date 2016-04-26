/*
%Z%  FILE: %M%  RELEASE: %I%  DATE: %G%, %U%
*/
/*******************************************************************************

Process:

    nerd

    NEtwoRk Drainer.


Author:    Alex Measday, ISI


Purpose:

    Program NERD drains data from a network connection.


    Invocation (TCP/IP Client):

        % nerd [-ascii <outputFile>] [-binary <outputFile>] [-debug]
               [-timeout <seconds>] [-total] [-vperror] <port>[@<host>]

    Invocation (TCP/IP Server):

        % nerd [-ascii <outputFile>] [-binary <outputFile>] [-debug]
               [-timeout <seconds>] [-total] [-vperror] -listen <port>

    Invocation (UDP/IP Server):

        % nerd [-ascii <outputFile>] [-binary <outputFile>] [-debug]
               [-timeout <seconds>] [-total] [-vperror] -udp -listen <port>

    where

        "-ascii <outputFile>"
            specifies that NERD should output the data it receives in the
            form of a hexadecimal/ASCII memory dump.  <outputFile> is the
            base pathname of the files to which the dump is written.
        "-binary <outputFile>"
            specifies that NERD should output the data it receives in raw,
            binary form.  <outputFile> is the base pathname of the files
            to which the dump is written.
        "-debug"
            enables debug output (written to STDOUT).
        "-timeout <seconds>"
            specifies a timeout used to terminate the connection.
            Initially, NERD waits as long as necessary for the first
            block of data to be received.  Subsequent reads, however,
            use this timeout; if the connection falls idle for the
            specified interval, the connection is terminated and NERD
            exits.  The default timeout interval is forever.
        "-total"
            tells NERD to display, upon exit, the total number of bytes read.
        "-udp"
            specifies UDP/IP communications; the default is TCP/IP.
        "-vperror"
            turns VPERROR() message output on.  VPERROR() messages are
            low-level error messages generated by LIBGPL library functions;
            normally, they are disabled.  If enabled, the messages are
            output to STDERR.
        "<port>[@<host>]"
            causes NERD to function as a client and specifies the network
            port (i.e., service name or port number) of the server and,
            optionally, the name of the host on which the server is running,
            of the network server to which NERD is to connect and from which
            NERD will drain data.  If no host is specified, the local host
            is assumed.
        "-listen <port>"
            causes NERD to function as a server and specifies the network
            server port (i.e., service name or port number) at which NERD
            will listen for connection requests from clients.

*******************************************************************************/


#ifdef sccs
    static  char  sccsid[] = "File: %M%  Release: %I%  Date: %G%, %U%" ;
#endif

#include  <errno.h>			/* System error definitions. */
#include  <signal.h>			/* Signal definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#if defined(sun) && !__STDC__
#    define  atexit(f)  on_exit (f, (char *) NULL)
#endif
#if defined(VMS)
#    include  <socket.h>		/* Socket-related definitions. */
#elif defined(VXWORKS)
#    include  <socket.h>		/* Socket-related definitions. */
#    include  <sockLib.h>		/* Socket library definitions. */
#    define  exit  return
#else
#    include  <sys/types.h>		/* System type definitions. */
#    include  <sys/socket.h>		/* Socket-related definitions. */
#endif
#include  "fnm_util.h"			/* Filename utilities. */
#include  "meo_util.h"			/* Memory operations. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "tcp_util.h"			/* TCP/IP network utilities. */
#include  "udp_util.h"			/* UDP/IP network utilities. */
#include  "vperror.h"			/* VPERROR() definitions. */

int  debug = 0 ;			/* Debug switch (1/0 = yes/no). */
int  displayTotal = 0 ;			/* Display total on exit? */
int  isClient = 1 ;			/* Client (the default) or server? */
int  useTCP = 1 ;			/* TCP (the default) or UDP? */
long  totalBytesRead = 0 ;		/* Total number of bytes read from network. */
TcpEndpoint  connection = NULL ;	/* TCP/IP connection to server. */
TcpEndpoint  listeningPoint = NULL ;	/* TCP/IP listening point (if !isClient). */
UdpEndpoint  endpoint = NULL ;		/* NERD's UDP endpoint. */

#define  MAX_INPUT  (32*1024)


/*******************************************************************************
    Private Functions
*******************************************************************************/

static  void  exitHandler (
#    if __STDC__
        void
#    endif
    ) ;

static  void  hangupHandler (
#    if __STDC__
        int  sig
#    endif
    ) ;

static  void  interruptHandler (
#    if __STDC__
        int  sig
#    endif
    ) ;

/*******************************************************************************
    Main Program.
*******************************************************************************/

#ifdef VXWORKS

int  nerd (

#    if __STDC__
        char  *commandLine)
#    else
        commandLine)

        char  *commandLine ;
#    endif

#else

int  main (

#    if __STDC__
        int  argc,
        char  *argv[])
#    else
        argc, argv)

        int  argc ;
        char  *argv[] ;
#    endif

#endif

{  /* Local variables. */
    char  *argument, *baseFileName, buffer[MAX_INPUT], *serverName ;
    double  timeout ;
    FILE  *dataFile ;
    FileName  fileName ;
    int  asciiOutput, binaryOutput, bufferSize ;
    int  errflg, option, version, vperror_save ;
    long  numBytesRead ;




#ifdef VXWORKS
    char  **argv ;
    int  argc ;
		/* Parse command string into an ARGC/ARGV array of arguments. */
    opt_create_argv ("nerd", commandLine, &argc, &argv) ;
#endif


    vperror_save = vperror_print ;	/* Save state of VPERROR_PRINT flag. */
#ifdef NOT_NOW
    vperror_print = 1 ;			/* Enable VPERROR output during initialization. */
#else
    vperror_print = 0 ;			/* Disable VPERROR output during initialization. */
#endif


/* Set up a termination handler (called when the program exits) and an
   interrupt handler (called in response to SIGINT and SIGTERM signals).
   Ignore SIGPIPE signals (caused by attempting to write to a broken
   connection). */

    atexit (exitHandler) ;
    signal (SIGHUP, hangupHandler) ;
    signal (SIGINT, interruptHandler) ;
    signal (SIGTERM, interruptHandler) ;
    signal (SIGPIPE, SIG_IGN) ;

    setbuf (stdout, NULL) ;


/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    asciiOutput = 0 ;
    baseFileName = NULL ;  binaryOutput = 0 ;
    serverName = NULL ;  timeout = -1.0 ;

    opt_init (argc, argv, 0,
              "{ascii:}{binary:}{debug}{listen}{timeout:}{total}{udp}{vperror}",
              NULL) ;
    errflg = 0 ;

    while (option = opt_get (NULL, &argument)) {

        switch (option) {
        case 1:			/* "-ascii <outputFile>" */
            asciiOutput = 1 ;
            baseFileName = argument ;
            break ;
        case 2:			/* "-binary <outputFile>" */
            binaryOutput = 1 ;
            baseFileName = argument ;
            break ;
        case 3:			/* "-debug" */
            debug = 1 ;
            tcp_util_debug = 1 ;
            udp_util_debug = 1 ;
            vperror_save = 1 ;
            break ;
        case 4:			/* "-listen" */
            isClient = 0 ;
            break ;
        case 5:			/* "-timeout <seconds>" */
            timeout = atof (argument) ;
            break ;
        case 6:			/* "-total" */
            displayTotal = 1 ;
            break ;
        case 7:			/* "-udp" */
            useTCP = 0 ;
            break ;
        case 8:			/* "-vperror" */
            vperror_save = 1 ;
            break ;
        case NONOPT:		/* "<server>[@<host>]" */
            if (serverName == NULL)
                serverName = argument ;
            else
                errflg++ ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    if (errflg || (serverName == NULL)) {
        fprintf (stderr, "Usage (TCP/IP Client):  nerd [<option(s)>] <port>[@<host>]\n") ;
        fprintf (stderr, "      (TCP/IP Server):  nerd [<option(s)>] -listen <port>\n") ;
        fprintf (stderr, "      (UDP/IP Server):  nerd [<option(s)>] -udp -listen <port>\n") ;
        fprintf (stderr, "where <option(s)> are:  [-ascii <outputFile>] [-binary <outputFile>] [-debug]\n") ;
        fprintf (stderr, "                        [-timeout <seconds>] [-total] [-vperror]\n") ;
        exit (EINVAL) ;
    }


/*******************************************************************************
    Establish a network connection to the server and drain any data received
    on the connection.
*******************************************************************************/

    if (useTCP && isClient) {				/* TCP/IP client. */
        if (tcpCall (serverName, 0, &connection)) {
            vperror ("[nerd] Error connecting to %s.\ntcpCall: ", serverName) ;
            exit (errno) ;
        }
    } else if (useTCP && !isClient) {			/* TCP/IP server. */
        if (tcpListen (serverName, 99, &listeningPoint)) {
            vperror ("[nerd] Error listening for connection request.\ntcpListen: ") ;
            exit (errno) ;
        }
        if (tcpAnswer (listeningPoint, -1.0, &connection)) {
            vperror ("[nerd] Error answering connection request.\ntcpAnswer: ") ;
            exit (errno) ;
        }
    } else if (!useTCP && isClient) {			/* UDP/IP client. */
        errno = EINVAL ;
        vperror ("[nerd] UDP/IP client mode not supported - yet!\n") ;
        exit (errno) ;
    } else if (!useTCP && !isClient) {			/* UDP/IP server. */
        if (udpCreate (serverName, NULL, &endpoint)) {
            vperror ("[nerd] Error listening on %s.\nudpCreate: ", serverName) ;
            exit (errno) ;
        }
    }

    bufferSize = 32*1024 ;
    if (setsockopt (useTCP ? tcpFd (connection) : udpFd (endpoint), SOL_SOCKET,
                    SO_RCVBUF, (char *) &bufferSize, sizeof bufferSize)) {
        vperror ("[nerd] Error setting %s's system buffer size.\nsetsockopt: ",
                 serverName) ;
        exit (errno) ;
    }

    vperror_print = vperror_save ;
    totalBytesRead = 0 ;
    version = 0 ;

    for ( ; ; ) {

/* At the beginning of a new pass, open a new output file. */

        if ((totalBytesRead == 0) && (asciiOutput || binaryOutput)) {
            sprintf (buffer, ".%03d", version++) ;
            fileName = fnmCreate (buffer, baseFileName, NULL) ;
            printf ("[nerd] Opening: %s\n", fnmPath (fileName)) ;
            dataFile = fopen (fnmPath (fileName), "w") ;
            if (dataFile == NULL) {
                vperror ("[nerd] Error opening output file: %s\nfopen: ",
                         fnmPath (fileName)) ;
                break ;
            }
        }

/* Read as much data as we can from the network connection. */

        if ((useTCP && tcpRead (connection,
                                (totalBytesRead > 0) ? timeout : -1.0,
                                - sizeof buffer, buffer, &numBytesRead))
            ||
            (!useTCP && udpRead (endpoint,
                                 (totalBytesRead > 0) ? timeout : -1.0,
                                 sizeof buffer, buffer, &numBytesRead, NULL))) {
            vperror ("[nerd] Error reading from %s.\n%s: ",
                     serverName, useTCP ? "tcpRead" : "udpRead") ;
            if (errno != EWOULDBLOCK)  break ;
            if (asciiOutput || binaryOutput) {
                printf ("[nerd] Closing: %s (%ld bytes)\n",
                        fnmPath (fileName), totalBytesRead) ;
                fclose (dataFile) ;  dataFile = NULL ;
                fnmDestroy (fileName) ;   fileName = NULL ;
            }
            totalBytesRead = 0 ;
            continue ;
        }
        totalBytesRead += numBytesRead ;

/* Output the data. */

        if (asciiOutput) {
            meoDumpX (dataFile, NULL, 0, buffer, numBytesRead) ;
        }

        if (binaryOutput) {
            fwrite (buffer, 1, numBytesRead, dataFile) ;
        }

    }


/*******************************************************************************
    All done.
*******************************************************************************/

    if (useTCP) {
        tcpDestroy (connection) ;
        if (!isClient)  tcpDestroy (listeningPoint) ;
    } else {
        udpDestroy (endpoint) ;
    }

    if (displayTotal)  fprintf (stderr, "[nerd] Read %ld bytes.\n",
                                totalBytesRead) ;

    exit (errno) ;

}

/*******************************************************************************

Procedure:

    exitHandler ()


Purpose:

    The exit handler is automatically invoked when a process terminates.

*******************************************************************************/


static  void  exitHandler (

#    if __STDC__
        void)
#    elif defined(sun)
        status, arg)

        int  status ;
        char  *arg ;
#    else
        )
#    endif

{

/* Break the connection with the server. */

    if (connection != NULL)  tcpDestroy (connection) ;
    if (listeningPoint != NULL)  tcpDestroy (listeningPoint) ;
    if (endpoint != NULL)  udpDestroy (endpoint) ;

    return ;

}

/*******************************************************************************

Procedure:

    interruptHandler ()


Purpose:

    Invoked by the system in response to a SIGINT signal:

        interruptHandler (sig, code, scp, addr)

    where

        (see SIGVEC(2) for detailed descriptions of arguments)

        <sig>   is the signal causing invocation of the handler.
        <code>  provides additional information for certain signals.
        <scp>   points to the signal context prior to the signal.
        <addr>  provides additional address information for certain signals.

*******************************************************************************/


static  void  hangupHandler (

#    if __STDC__
        int  sig)
#    else
        sig)

        int  sig ;
#    endif

{
    fprintf (stderr, "\n** Hang Up **\n") ;
    if (displayTotal)  fprintf (stderr, "[nerd] Read %ld bytes.\n",
                                totalBytesRead) ;
    return ;
}

/*******************************************************************************

Procedure:

    interruptHandler ()


Purpose:

    Invoked by the system in response to a SIGINT signal:

        interruptHandler (sig, code, scp, addr)

    where

        (see SIGVEC(2) for detailed descriptions of arguments)

        <sig>   is the signal causing invocation of the handler.
        <code>  provides additional information for certain signals.
        <scp>   points to the signal context prior to the signal.
        <addr>  provides additional address information for certain signals.

*******************************************************************************/


static  void  interruptHandler (

#    if __STDC__
        int  sig)
#    else
        sig)

        int  sig ;
#    endif

{
    fprintf (stderr, "\n** Interrupt **\n") ;
    if (displayTotal)  fprintf (stderr, "[nerd] Read %ld bytes.\n",
                                totalBytesRead) ;
    exit (-1) ;
}
