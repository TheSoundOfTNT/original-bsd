/*	va.c	4.13.1.3	82/11/27	*/

#include "va.h"
#if NVA > 0
/*
 * Varian printer plotter
 */
#include "../h/param.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/buf.h"
#include "../h/systm.h"
#include "../h/map.h"
#include "../h/pte.h"
#include "../h/vcmd.h"
#include "../h/uio.h"

#include "../vaxuba/ubareg.h"
#include "../vaxuba/ubavar.h"

unsigned minvaph();

#define	VAPRI	(PZERO-1)

struct	vadevice {
	u_short	vaba;			/* buffer address */
	short	vawc;			/* word count (2's complement) */
	union {
		short	Vacsw;		/* control status as word */
		struct {		/* control status as bytes */
			char Vacsl;
			char Vacsh;
		} vacsr;
	} vacs;
	short	vadata;			/* programmed i/o data buffer */
};

#define	vacsw	vacs.Vacsw
#define	vacsh	vacs.vacsr.Vacsh
#define	vacsl	vacs.vacsr.Vacsl

/* vacsw bits */
#define	VA_ERROR	0100000		/* some error has occurred */
#define	VA_NPRTIMO	0001000		/* DMA timeout error */
#define	VA_NOTREADY	0000400		/* something besides NPRTIMO */
#define	VA_DONE		0000200
#define	VA_IENABLE	0000100		/* interrupt enable */
#define	VA_DMAGO	0000010		/* DMA go bit */
#define	VA_SUPPLIESLOW	0000004
#define	VA_BOTOFFORM	0000002
#define	VA_BYTEREVERSE	0000001		/* reverse byte order in words */

/* vacsh command bytes */
#define	VAPLOT		0000340
#define	VAPRINT		0000100
#define	VAPRINTPLOT	0000160
#define	VAAUTOSTEP	0000244
#define	VANOAUTOSTEP	0000045
#define	VAFORMFEED	0000263
#define	VASLEW		0000265
#define	VASTEP		0000064

struct va_softc {
	char	sc_openf;
	char	sc_busy;
	int	sc_state;
	int	sc_wc;
	struct	buf *sc_bp;
	int	sc_ubinfo;
} va_softc[NVA];

#define	VAUNIT(dev)	(minor(dev))

struct	buf rvabuf[NVA];

int	vaprobe(), vaattach();
struct	uba_device *vadinfo[NVA];
u_short	vastd[] = { 0764000, 0 };
struct	uba_driver vadriver =
    { vaprobe, 0, vaattach, 0, vastd, "va", vadinfo };

vaprobe(reg)
	caddr_t reg;
{
	register int br, cvec;		/* value-result */
	register struct vadevice *vaaddr = (struct vadevice *)reg;

#ifdef lint
	br = 0; cvec = br; br = cvec;
	vaintr(0);
#endif
	vaaddr->vacsl = VA_IENABLE;
	vaaddr->vaba = 0;
	vaaddr->vacsh = VAPLOT;
#ifndef VARIANGOBIT
	vaaddr->vacsl = 0;
#endif
	vaaddr->vawc = -1;
	DELAY(100000);
	vaaddr->vacsl = 0;
	return (sizeof (struct vadevice));
}

/*ARGSUSED*/
vaattach(ui)
	struct uba_device *ui;
{

}

vaopen(dev)
	dev_t dev;
{
	register struct va_softc *sc;
	register struct vadevice *vaaddr;
	register struct uba_device *ui;
	int error;

	if (VAUNIT(dev) >= NVA || (sc = &va_softc[minor(dev)])->sc_openf ||
	    (ui = vadinfo[VAUNIT(dev)]) == 0 || ui->ui_alive == 0)
		return (ENXIO);
	vaaddr = (struct vadevice *)ui->ui_addr;
	sc->sc_openf = 1;
	vaaddr->vawc = 0;
	sc->sc_wc = 0;
	sc->sc_state = 0;
	vaaddr->vacsl = VA_IENABLE;
	vatimo(dev);
	error = vacmd(dev, VPRINT);
	if (error)
		vaclose(dev);
	return (error);
}

vastrategy(bp)
	register struct buf *bp;
{
	register int e;
	register struct va_softc *sc = &va_softc[VAUNIT(bp->b_dev)];
	register struct uba_device *ui = vadinfo[VAUNIT(bp->b_dev)];
	register struct vadevice *vaaddr = (struct vadevice *)ui->ui_addr;

	(void) spl4();
	while (sc->sc_busy)
		sleep((caddr_t)sc, VAPRI);
	sc->sc_busy = 1;
	sc->sc_bp = bp;
	sc->sc_ubinfo = ubasetup(ui->ui_ubanum, bp, UBA_NEEDBDP);
	if (e = vawait(bp->b_dev))
		goto brkout;
	sc->sc_wc = -(bp->b_bcount/2);
	vastart(bp->b_dev);
	e = vawait(bp->b_dev);
	sc->sc_wc = 0;
	if (sc->sc_state & VPRINTPLOT) {
		sc->sc_state = (sc->sc_state & ~VPRINTPLOT) | VPLOT;
		vaaddr->vacsh = VAAUTOSTEP;
		e |= vawait(bp->b_dev);
	}
	(void) spl0();
brkout:
	ubarelse(ui->ui_ubanum, &sc->sc_ubinfo);
	sc->sc_bp = 0;
	sc->sc_busy = 0;
	if (e)
		bp->b_flags |= B_ERROR;
	iodone(bp);
	wakeup((caddr_t)sc);
}

int	vablock = 16384;

unsigned
minvaph(bp)
	struct buf *bp;
{

	if (bp->b_bcount > vablock)
		bp->b_bcount = vablock;
}

/*ARGSUSED*/
vawrite(dev, uio)
	dev_t dev;
	struct uio *uio;
{

	if (VAUNIT(dev) > NVA)
		return (ENXIO);
	return (physio(vastrategy, &rvabuf[VAUNIT(dev)], dev, B_WRITE,
		    minvaph, uio));
}

vawait(dev)
	dev_t dev;
{
	register struct vadevice *vaaddr =
	    (struct vadevice *)vadinfo[VAUNIT(dev)]->ui_addr;
	register int e;

	while (((e = vaaddr->vacsw) & (VA_DONE|VA_ERROR)) == 0)
		sleep((caddr_t)&va_softc[VAUNIT(dev)], VAPRI);
	if (e & VA_NPRTIMO)
		printf("va%d: npr timeout\n", VAUNIT(dev));
	return (e & VA_ERROR);
}

vastart(dev)
	dev_t;
{
	register struct va_softc *sc = &va_softc[VAUNIT(dev)];
	register struct vadevice *vaaddr =
	    (struct vadevice *)vadinfo[VAUNIT(dev)]->ui_addr;

	if (sc->sc_wc == 0)
		return;
	vaaddr->vaba = sc->sc_ubinfo;
#ifndef VARIANGOBIT
	vaaddr->vacsl = (sc->sc_ubinfo >> 12) & 0x30;
#else
	vaaddr->vacsl = (sc->sc_ubinfo >> 12) & 0x30 | VA_IENABLE | VA_DMAGO;
#endif
	vaaddr->vawc = sc->sc_wc;
}

/*ARGSUSED*/
vaioctl(dev, cmd, data, flag)
	register caddr_t data;
{
	register int vcmd;
	register struct va_softc *sc = &va_softc[VAUNIT(dev)];

	switch (cmd) {

	case VGETSTATE:
		*(int *)data = sc->sc_state;
		break;

	case VSETSTATE:
		return (vacmd(dev, *(int *)data));
		break;

	default:
		return (ENOTTY);
	}
	return (0);
}

vacmd(dev, vcmd)
	dev_t dev;
	int vcmd;
{
	register struct va_softc *sc = &va_softc[VAUNIT(dev)];
	register struct vadevice *vaaddr =
	    (struct vadevice *)vadinfo[VAUNIT(dev)]->ui_addr;
	int error = 0;

	(void) spl4();
	(void) vawait(dev);
	switch (vcmd) {

	case VPLOT:
		/* Must turn on plot AND autostep modes. */
		vaaddr->vacsh = VAPLOT;
		if (vawait(dev))
			error = EIO;
		vaaddr->vacsh = VAAUTOSTEP;
		break;

	case VPRINT:
		vaaddr->vacsh = VAPRINT;
		break;

	case VPRINTPLOT:
		vaaddr->vacsh = VAPRINTPLOT;
		break;
	}
	sc->sc_state = (sc->sc_state & ~(VPLOT|VPRINT|VPRINTPLOT)) | vcmd;
	if (vawait(dev))
		error = EIO;
	(void) spl0();
	return (error);
}

vatimo(dev)
	dev_t dev;
{
	register struct va_softc *sc = &va_softc[VAUNIT(dev)];

	if (sc->sc_openf)
		timeout(vatimo, (caddr_t)dev, hz/10);
	vaintr(dev);
}

/*ARGSUSED*/
vaintr(dev)
	dev_t dev;
{
	register struct va_softc *sc = &va_softc[VAUNIT(dev)];

	wakeup((caddr_t)sc);
}

vaclose(dev)
	dev_t dev;
{
	register struct va_softc *sc = &va_softc[VAUNIT(dev)];
	register struct vadevice *vaaddr =
	    (struct vadevice *)vadinfo[VAUNIT(dev)]->ui_addr;

	sc->sc_openf = 0;
	sc->sc_busy = 0;
	sc->sc_state = 0;
	sc->sc_ubinfo = 0;
	vaaddr->vacsl = 0;
}

vareset(uban)
	int uban;
{
	register int va11;
	register struct uba_device *ui;
	register struct va_softc *sc = va_softc;
	register struct vadevice *vaaddr;

	for (va11 = 0; va11 < NVA; va11++, sc++) {
		if ((ui = vadinfo[va11]) == 0 || ui->ui_alive == 0 ||
		    ui->ui_ubanum != uban || sc->sc_openf == 0)
			continue;
		printf(" va%d", va11);
		vaaddr = (struct vadevice *)ui->ui_addr;
		vaaddr->vacsl = VA_IENABLE;
		if (sc->sc_state & VPLOT) {
			vaaddr->vacsh = VAPLOT;
			DELAY(10000);
			vaaddr->vacsh = VAAUTOSTEP;
		} else if (sc->sc_state & VPRINTPLOT)
			vaaddr->vacsh = VPRINTPLOT;
		else
			vaaddr->vacsh = VAPRINTPLOT;
		DELAY(10000);
		if (sc->sc_busy == 0)
			continue;
		sc->sc_ubinfo = ubasetup(ui->ui_ubanum, sc->sc_bp, UBA_NEEDBDP);
		sc->sc_wc = -(sc->sc_bp->b_bcount/2);
		vastart(sc->sc_bp->b_dev);
	}
}

vaselect()
{

	return (1);
}
#endif
