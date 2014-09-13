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
#ifndef _LIBA_H_
#define _LIBA_H_

#define A_PID_POSTFIX		"pid"
#define A_DAEMON_UMASK		0
#define A_DAEMON_NOCLOSE	0
#define A_DAEMON_NOCHDIR	1
#define A_LOCK_ACCESS_MODE	0640

/* strings containig daemon name and full path to pidfile */
extern char *a_daemon_name;
extern char *a_daemon_pidfile;

void a_signal( int sig, void (*handler)( int sig ) );
int daemonize( const char *path, const char *name );


#endif /* _LIBA_H_ */