/*	$OpenBSD: if_umtvar.h,v 1.3 2009/03/26 20:17:27 damien Exp $	*/

/*-
 * Copyright (c) 2008,2009 Damien Bergamini <damien.bergamini@free.fr>
 * ported to FreeBSD by Akinori Furukoshi <moonlightakkiy@yahoo.ca>
 * USB Consulting, Hans Petter Selasky <hselasky@freebsd.org>
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
 *
 */

#ifndef _IF_UMTVAR_H_
#define	_IF_UMTVAR_H_

#define	UMT_MAX_RXSZ			\
	MIN(4096, MJUMPAGESIZE)

/* NB: "11" is the maximum number of padding bytes needed for Tx */
#define	UMT_MAX_TXSZ			\
	(sizeof (struct rt2870_txd) +	\
	 sizeof (struct rt2860_txwi) +	\
	 MCLBYTES + 11)

#define	UMT_TX_TIMEOUT	5000	/* ms */

/* Tx ring count was 8/endpoint, now 32 for all 4 (or 6) endpoints. */
#define	UMT_TX_RING_COUNT	32
#define	UMT_RX_RING_COUNT	1

#define	RT2870_WCID_MAX		64
#define	UMT_AID2WCID(aid)	((aid) & 0xff)

#define	UMT_VAP_MAX		8

struct umt_rx_radiotap_header {
	struct ieee80211_radiotap_header wr_ihdr;
	uint64_t	wr_tsf;
	uint8_t		wr_flags;
	uint8_t		wr_rate;
	uint16_t	wr_chan_freq;
	uint16_t	wr_chan_flags;
	int8_t		wr_dbm_antsignal;
	uint8_t		wr_antenna;
	uint8_t		wr_antsignal;
} __packed __aligned(8);

#define	UMT_RX_RADIOTAP_PRESENT				\
	(1 << IEEE80211_RADIOTAP_TSFT |			\
	 1 << IEEE80211_RADIOTAP_FLAGS |		\
	 1 << IEEE80211_RADIOTAP_RATE |			\
	 1 << IEEE80211_RADIOTAP_CHANNEL |		\
	 1 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL |	\
	 1 << IEEE80211_RADIOTAP_ANTENNA |		\
	 1 << IEEE80211_RADIOTAP_DB_ANTSIGNAL)

struct umt_tx_radiotap_header {
	struct ieee80211_radiotap_header wt_ihdr;
	uint8_t		wt_flags;
	uint8_t		wt_rate;
	uint16_t	wt_chan_freq;
	uint16_t	wt_chan_flags;
	uint8_t		wt_hwqueue;
} __packed __aligned(8);

#define IEEE80211_RADIOTAP_HWQUEUE 15

#define	UMT_TX_RADIOTAP_PRESENT				\
	(1 << IEEE80211_RADIOTAP_FLAGS |		\
	 1 << IEEE80211_RADIOTAP_RATE |			\
	 1 << IEEE80211_RADIOTAP_CHANNEL |		\
	 1 << IEEE80211_RADIOTAP_HWQUEUE)

struct umt_softc;

struct umt_tx_data {
	STAILQ_ENTRY(umt_tx_data)	next;
	struct umt_softc	*sc;
	struct mbuf		*m;
	struct ieee80211_node	*ni;
	uint32_t align[0];	/* dummy field */
	uint8_t	desc[sizeof(struct rt2870_txd) +
		     sizeof(struct rt2860_txwi)];
	uint8_t			ridx;
};
STAILQ_HEAD(umt_tx_data_head, umt_tx_data);

struct umt_node {
	struct ieee80211_node	ni;
	uint8_t			ridx[IEEE80211_RATE_MAXSIZE];
	uint8_t			ctl_ridx[IEEE80211_RATE_MAXSIZE];
	uint8_t			amrr_ridx;
	uint8_t			mgt_ridx;
	uint8_t			fix_ridx;
};
#define UMT_NODE(ni)		((struct umt_node *)(ni))

struct umt_cmdq {
	void			*arg0;
	void			*arg1;
	void			(*func)(void *);
	struct ieee80211_key	*k;
	struct ieee80211_key	key;
	uint8_t			mac[IEEE80211_ADDR_LEN];
	uint8_t			wcid;
};

struct umt_vap {
	struct ieee80211vap             vap;
	struct mbuf			*beacon_mbuf;

	int                             (*newstate)(struct ieee80211vap *,
                                            enum ieee80211_state, int);
	void				(*recv_mgmt)(struct ieee80211_node *,
					    struct mbuf *, int,
					    const struct ieee80211_rx_stats *,
					    int, int);

	uint8_t				rvp_id;
};
#define	UMT_VAP(vap)	((struct umt_vap *)(vap))

/*
 * There are 7 bulk endpoints: 1 for RX
 * and 6 for TX (4 EDCAs + HCCA + Prio).
 * Update 03-14-2009:  some devices like the Planex GW-US300MiniS
 * seem to have only 4 TX bulk endpoints (Fukaumi Naoki).
 */
enum {
	UMT_BULK_TX_BE,		/* = WME_AC_BE */
	UMT_BULK_TX_BK,		/* = WME_AC_BK */
	UMT_BULK_TX_VI,		/* = WME_AC_VI */
	UMT_BULK_TX_VO,		/* = WME_AC_VO */
	UMT_BULK_TX_HCCA,
	UMT_BULK_TX_PRIO,
	UMT_BULK_RX,
	UMT_N_XFER,
};

#define	UMT_EP_QUEUES	UMT_BULK_RX

struct umt_endpoint_queue {
	struct umt_tx_data		tx_data[UMT_TX_RING_COUNT];
	struct umt_tx_data_head		tx_qh;
	struct umt_tx_data_head		tx_fh;
	uint32_t			tx_nfree;
};

struct umt_softc {
	struct mtx			sc_mtx;
	struct ieee80211com		sc_ic;
	//struct ieee80211_ratectl_tx_stats sc_txs;
	struct mbufq			sc_snd;
	device_t			sc_dev;
	struct usb_device		*sc_udev;
	int				sc_need_fwload;

	int				sc_flags;
#define	UMT_FLAG_FWLOAD_NEEDED		0x01
#define	UMT_UMTNING			0x02

	uint16_t			wcid_stats[RT2870_WCID_MAX + 1][3];
#define	UMT_TXCNT	0
#define	UMT_SUCCESS	1
#define	UMT_RETRY	2

	int				(*sc_srom_read)(struct umt_softc *,
					    uint16_t, uint16_t *);

	uint16_t			mac_ver;
	uint16_t			mac_rev;
	uint16_t			rf_rev;
	uint8_t				freq;
	uint8_t				ntxchains;
	uint8_t				nrxchains;

	uint8_t				bbp25;
	uint8_t				bbp26;
	uint8_t				rf24_20mhz;
	uint8_t				rf24_40mhz;
	uint8_t				patch_dac;
	uint8_t				rfswitch;
	uint8_t				ext_2ghz_lna;
	uint8_t				ext_5ghz_lna;
	uint8_t				calib_2ghz;
	uint8_t				calib_5ghz;
	uint8_t				txmixgain_2ghz;
	uint8_t				txmixgain_5ghz;
	int8_t				txpow1[54];
	int8_t				txpow2[54];
	int8_t				txpow3[54];
	int8_t				rssi_2ghz[3];
	int8_t				rssi_5ghz[3];
	uint8_t				lna[4];

	struct {
		uint8_t	reg;
		uint8_t	val;
	}				bbp[10], rf[10];
	uint8_t				leds;
	uint16_t			led[3];
	uint32_t			txpow20mhz[5];
	uint32_t			txpow40mhz_2ghz[5];
	uint32_t			txpow40mhz_5ghz[5];

	struct umt_endpoint_queue	sc_epq[UMT_EP_QUEUES];

	struct task                     ratectl_task;
	struct usb_callout              ratectl_ch;
	uint8_t				ratectl_umt;
#define	UMT_RATECTL_OFF	0

/* need to be power of 2, otherwise UMT_CMDQ_GET fails */
#define	UMT_CMDQ_MAX	16
#define	UMT_CMDQ_MASQ	(UMT_CMDQ_MAX - 1)
	struct umt_cmdq			cmdq[UMT_CMDQ_MAX];
	struct task			cmdq_task;
	uint32_t			cmdq_store;
	uint8_t				cmdq_exec;
	uint8_t				cmdq_umt;
	uint8_t				cmdq_key_set;
#define	UMT_CMDQ_ABORT	0
#define	UMT_CMDQ_GO	1

	struct usb_xfer			*sc_xfer[UMT_N_XFER];

	struct mbuf			*rx_m;

	uint8_t				fifo_cnt;

	uint8_t				umtning;
	uint8_t				umtbmap;
	uint8_t				ap_umtning;
	uint8_t				adhoc_umtning;
	uint8_t				sta_umtning;
	uint8_t				rvp_cnt;
	uint8_t				rvp_bmap;
	uint8_t				sc_detached;

	uint8_t				sc_bssid[IEEE80211_ADDR_LEN];

	union {
		struct umt_rx_radiotap_header th;
		uint8_t	pad[64];
	}				sc_rxtapu;
#define sc_rxtap	sc_rxtapu.th

	union {
		struct umt_tx_radiotap_header th;
		uint8_t	pad[64];
	}				sc_txtapu;
#define sc_txtap	sc_txtapu.th
};

#define	UMT_LOCK(sc)		mtx_lock(&(sc)->sc_mtx)
#define	UMT_UNLOCK(sc)		mtx_unlock(&(sc)->sc_mtx)
#define	UMT_LOCK_ASSERT(sc, t)	mtx_assert(&(sc)->sc_mtx, t)

#endif	/* _IF_UMTVAR_H_ */
