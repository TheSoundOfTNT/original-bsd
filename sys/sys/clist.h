/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)clist.h	7.4 (Berkeley) 06/04/93
 */

struct cblock {
	struct cblock *c_next;		/* next cblock in queue */
	char c_quote[CBQSIZE];		/* quoted characters */
	char c_info[CBSIZE];		/* characters */
};

#ifdef KERNEL
extern	struct cblock *cfree, *cfreelist;
extern	int cfreecount, nclist;
#endif
