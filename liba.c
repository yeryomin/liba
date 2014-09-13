/**
 * This file is part of liba library code.
 *
 * Copyright (C) 2014 Roman Yeryomin <roman@advem.lv>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENCE.txt file for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <sys/stat.h>
#include <signal.h>

#include "liba.h"


#define AE_OPEN_FAIL		-2

char *a_daemon_name = NULL;
char *a_daemon_pidfile = NULL;

static int a_fork( int nochdir, int noclose )
{
	pid_t pid, sid;

	pid = fork();
	if ( pid < 0 )
		return -1;

	if ( pid > 0 )
		exit(EXIT_SUCCESS);

	umask( A_DAEMON_UMASK );

	sid = setsid();
	if ( sid < 0 )
		exit(EXIT_FAILURE);

	if ( !nochdir ) {
		if ( chdir("/") < 0 )
			return -1;
	}

	if ( !noclose ) {
		if ( !freopen( "/dev/null", "r", stdin )  ||
		     !freopen( "/dev/null", "w", stdout ) ||
		     !freopen( "/dev/null", "w", stderr ) )
			return -1;
	}

	return 0;
}

static int a_daemon( const char *pidfile, int nochdir )
{
	int fd;
	int err;
	pid_t pid;
	char buf[16];

	fd = open( pidfile, O_WRONLY | O_CREAT | O_EXCL, A_LOCK_ACCESS_MODE );
	if ( fd < 0 )
		return AE_OPEN_FAIL;

	err = a_fork( nochdir, A_DAEMON_NOCLOSE );
	if ( err ) {
		close( fd );
		return err;
	}

	pid = getpid();

	memset( buf, 0, sizeof buf );
	snprintf( buf, sizeof buf, "%i\n", pid );
	err = write( fd, buf, strlen(buf) );
	close( fd );

	/* failed to write to pidfile? */
	if ( err < 0 ) {
		unlink( pidfile );
		return err;
	}

	return pid;
}

static void a_daemon_death_handler( int sig )
{
	signal( sig, SIG_IGN );
	syslog( LOG_INFO, "Cleaning up and dying..." );
	if ( unlink( a_daemon_pidfile ) < 0 )
		syslog( LOG_WARNING, "Cannot delete %s: %m", a_daemon_pidfile );

	exit(EXIT_FAILURE);
}

void a_signal( int sig, void (*handler)(  ) )
{
	struct sigaction action;

	action.sa_handler = handler;
	sigemptyset( &(action.sa_mask) );
	sigaddset( &(action.sa_mask), sig );
	action.sa_flags = 0;
	sigaction( sig, &action, NULL );
}

int daemonize( const char *pidpath, const char *name )
{
	int pid;
	char *tmp = NULL;

	if ( !a_daemon_name ) {
		if ( !name ) {
			printf( "Cannot turn into unnamed daemon!\n" );
			exit(EXIT_FAILURE);
		}
		tmp = strdup( name );
		a_daemon_name = strrchr( tmp, '/' );
		if ( !a_daemon_name )
			a_daemon_name = tmp;
		else
			a_daemon_name++;
	}

	a_daemon_pidfile = malloc( strlen(pidpath) +
				   strlen(a_daemon_name) +
				   strlen(A_PID_POSTFIX) + 3 );
	if ( !a_daemon_pidfile )
		exit(EXIT_FAILURE);

	sprintf( a_daemon_pidfile, "%s/%s.%s",  pidpath,
						a_daemon_name,
						A_PID_POSTFIX );

	/* register TERM signal handler */
	a_signal( SIGTERM, a_daemon_death_handler );

	/* try to daemonize */
	pid = a_daemon( a_daemon_pidfile, A_DAEMON_NOCHDIR );
	if ( pid < 0 ) {
		if ( pid == AE_OPEN_FAIL )
			printf( "Failed to start %s: %m: %s\n",
					a_daemon_name, a_daemon_pidfile );
		else
			syslog( LOG_WARNING, "Failed to start %s: %m (%i)",
							a_daemon_name, pid);
		exit(EXIT_FAILURE);
	}

	syslog( LOG_INFO, "Started %s with PID %i", a_daemon_name, pid );

	return pid;
}
