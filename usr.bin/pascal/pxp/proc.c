/*-
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#ifndef lint
static char sccsid[] = "@(#)proc.c	8.1 (Berkeley) 06/06/93";
#endif /* not lint */

/*
 * pxp - Pascal execution profiler
 *
 * Bill Joy UCB
 * Version 1.2 January 1979
 */

#include "0.h"
#include "tree.h"

proc(r)
	int *r;
{

	call(r[2], r[3]);
}
