/* $nsh: main.c,v 1.35 2008/02/08 07:02:51 chris Exp $ */
/*
 * Copyright (c) 2002-2008 Chris Cappuccio <chris@nmedia.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/syslimits.h>
#include <sys/ttycom.h>
#include <sys/signal.h>
#include <histedit.h>
#include "externs.h"
#include "editing.h"

void usage(void);

char *vers = "20080207";
int bridge = 0;		/* bridge mode for interface() */
int verbose = 0;	/* verbose mode for lots of stuff*/
int priv = 0;
int editing;
pid_t pid;

History *histi = NULL;
History *histc = NULL;
HistEvent ev;
EditLine *elc = NULL;
EditLine *eli = NULL;
char *cursor_pos = NULL;

int
main(int argc, char *argv[])
{
	int ch, iflag = 0;
	char rc[PATH_MAX];

	if(getuid() != 0) 
		printf("%% Functionality may be limited without root privileges.\n");

	pid = getpid();

	while ((ch = getopt(argc, argv, "i:v")) != -1)
		switch (ch) {
		case 'i':
			iflag = 1;
			strlcpy(rc, optarg, PATH_MAX);
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			usage();
		}

	argc -= optind;
	argv += optind;

	printf("%% NSH v%s\n", vers);

	if (argc > 0)
		usage();

	if (iflag) {
		/*
		 * Run initialization and then exit.
		 */
		rmtemp(PFCONF_TEMP);
		rmtemp(OSPFCONF_TEMP);
		rmtemp(BGPCONF_TEMP);
		rmtemp(RIPCONF_TEMP);
		rmtemp(IPSECCONF_TEMP);
		rmtemp(DVMRPCONF_TEMP);
		rmtemp(RELAYCONF_TEMP);
		rmtemp(SASYNCCONF_TEMP);
		rmtemp(DHCPCONF_TEMP);
		rmtemp(SNMPCONF_TEMP);
		rmtemp(NTPCONF_TEMP);

		priv = 1;	/*
				 * Necessary today for 'enable secret' to
				 * work in -i mode, as CLI code is reworked
				 * this will disappear
				 */
		cmdrc(rc);
		exit(0);
	}

	(void)signal(SIGWINCH, setwinsize);
	setwinsize(0);

	for (;;) {
		command(1);
	}

	/* NOTREACHED */
	return 0;
}

void
usage(void)
{
	(void)fprintf(stderr, "usage: %s [-v] [-i rcfile]\n", __progname);
	(void)fprintf(stderr, "           -v indicates verbose operation\n");
	(void)fprintf(stderr, "           -i rcfile loads configuration from rcfile\n");
	exit(1);
}
