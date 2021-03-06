/*
 * Copyright (c) 1992 OMRON Corporation.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * OMRON Corporation.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)vectors.h	8.1 (Berkeley) 06/10/93
 */

/* vectors.h */
/* by A.Fujita, Nov-18-1991 */

#define	NVECTBL		0x400	/* size of vector table */

#define ILLGINST	0x010	/* Illegal Instruction */
#define TRACEVEC	0x024	/* Trace Vector offset */
#define	NMIVEC		0x07c	/* Level 7 Interrupt Auto Vector (NMI) offset */
#define	EVTRAPF		0x0bc	/* Trap #15 Instraction Vector */
