#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

#include <liba.h>

#define PIDPATH		"/tmp"
#define NAME		argv[0]
#define TIMER		30

int timer = TIMER;
int counter = 0;

void reset_timer( int sig )
{
	/* temporarily ingnore interrupts */
	signal( sig, SIG_IGN );
	counter++;
	syslog( LOG_INFO, "%i HUP received, resetting timer (was %i)...",
				counter, timer );
	timer = TIMER;
	/* reenable interrupt */
	a_signal( sig, reset_timer );
}

int main( int argc, char **argv )
{
	int pid = daemonize( PIDPATH, NAME );

	/* daemon stuff */
	a_signal( SIGHUP, reset_timer );
	syslog( LOG_INFO, "Working hard..." );
	while ( timer > 0 ) {
		sleep(1);
		timer--;
		if ( timer < 5 )
			syslog( LOG_INFO, "Dying in %i...", timer + 1 );
	}

	/* cleanup, if ever get here */
	syslog( LOG_INFO, "Exiting %s (%i)...", a_daemon_name, pid );
	unlink( a_daemon_pidfile );
	exit(EXIT_SUCCESS);
}
