#	@(#)Makefile	8.2 (Berkeley) 05/19/94

DIR=	usd/27.invert
SRCS=	refer
MACROS=	-ms
REFER=	refer -e -p /usr/old/dict/papers/Ind

paper.ps: ${SRCS}
	${REFER} ${SRCS} | ${TBL} | ${ROFF} > ${.TARGET}

.include <bsd.doc.mk>
