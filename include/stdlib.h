/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)stdlib.h	8.5 (Berkeley) 05/19/95
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <machine/ansi.h>

#ifdef	_BSD_SIZE_T_
typedef	_BSD_SIZE_T_	size_t;
#undef	_BSD_SIZE_T_
#endif

#ifdef	_BSD_WCHAR_T_
#ifndef _ANSI_SOURCE
typedef	_BSD_WCHAR_T_	rune_t;
#endif
typedef	_BSD_WCHAR_T_	wchar_t;
#undef	_BSD_WCHAR_T_
#endif

typedef struct {
	int quot;		/* quotient */
	int rem;		/* remainder */
} div_t;

typedef struct {
	long quot;		/* quotient */
	long rem;		/* remainder */
} ldiv_t;

#ifndef NULL
#define	NULL	0
#endif

#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0

#define	RAND_MAX	0x7fffffff

extern int __mb_cur_max;
#define	MB_CUR_MAX	__mb_cur_max

#include <sys/cdefs.h>

__BEGIN_DECLS
__dead void
	 abort __P((void));
__pure int
	 abs __P((int));
int	 atexit __P((void (*)(void)));
double	 atof __P((const char *));
int	 atoi __P((const char *));
long	 atol __P((const char *));
void	*bsearch __P((const void *, const void *, size_t,
	    size_t, int (*)(const void *, const void *)));
void	*calloc __P((size_t, size_t));
__pure div_t
	 div __P((int, int));
__dead void
	 exit __P((int));
void	 free __P((void *));
char	*getenv __P((const char *));
__pure long
	 labs __P((long));
__pure ldiv_t
	 ldiv __P((long, long));
void	*malloc __P((size_t));
void	 qsort __P((void *, size_t, size_t,
	    int (*)(const void *, const void *)));
int	 rand __P((void));
void	*realloc __P((void *, size_t));
void	 srand __P((unsigned));
double	 strtod __P((const char *, char **));
long	 strtol __P((const char *, char **, int));
unsigned long
	 strtoul __P((const char *, char **, int));
int	 system __P((const char *));

/* These are currently just stubs. */
int	 mblen __P((const char *, size_t));
size_t	 mbstowcs __P((wchar_t *, const char *, size_t));
int	 wctomb __P((char *, wchar_t));
int	 mbtowc __P((wchar_t *, const char *, size_t));
size_t	 wcstombs __P((char *, const wchar_t *, size_t));

#ifndef _ANSI_SOURCE
int	 putenv __P((const char *));
int	 setenv __P((const char *, const char *, int));
#endif

#if !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE)
void	*alloca __P((size_t));		/* built-in for gcc */
					/* getcap(3) functions */
char	*getbsize __P((int *, long *));
char	*cgetcap __P((char *, char *, int));
int	 cgetclose __P((void));
int	 cgetent __P((char **, char **, char *));
int	 cgetfirst __P((char **, char **));
int	 cgetmatch __P((char *, char *));
int	 cgetnext __P((char **, char **));
int	 cgetnum __P((char *, char *, long *));
int	 cgetset __P((char *));
int	 cgetstr __P((char *, char *, char **));
int	 cgetustr __P((char *, char *, char **));

int	 daemon __P((int, int));
char	*devname __P((int, int));
int	 getloadavg __P((double [], int));

char	*group_from_gid __P((unsigned long, int));
int	 heapsort __P((void *, size_t, size_t,
	    int (*)(const void *, const void *)));
char	*initstate __P((unsigned long, char *, long));
int	 mergesort __P((void *, size_t, size_t,
	    int (*)(const void *, const void *)));
int	 radixsort __P((const unsigned char **, int, const unsigned char *,
	    unsigned));
int	 sradixsort __P((const unsigned char **, int, const unsigned char *,
	    unsigned));
long	 random __P((void));
char	*realpath __P((const char *, char resolved_path[]));
char	*setstate __P((char *));
void	 srandom __P((unsigned long));
char	*user_from_uid __P((unsigned long, int));
#ifndef __STRICT_ANSI__
long long
	 strtoq __P((const char *, char **, int));
unsigned long long
	 strtouq __P((const char *, char **, int));
#endif
void	 unsetenv __P((const char *));
#endif
__END_DECLS

#endif /* _STDLIB_H_ */
