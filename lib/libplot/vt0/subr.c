/*-
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.proprietary.c%
 */

#ifndef lint
static char sccsid[] = "@(#)subr.c	8.1 (Berkeley) 06/04/93";
#endif /* not lint */

extern float obotx;
extern float oboty;
extern float boty;
extern float botx;
extern float scalex;
extern float scaley;
xsc(xi){
	int xa;
	xa = (xi-obotx)*scalex+botx;
	return(xa);
}
ysc(yi){
	int ya;
	ya = (yi-oboty)*scaley+boty;
	return(ya);
}
