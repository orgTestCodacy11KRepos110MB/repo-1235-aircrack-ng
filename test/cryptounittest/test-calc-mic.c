/*
 *
 * test-calc-mic.c
 *
 * Copyright (C) 2012 Carlos Alberto Lopez Perez <clopez@igalia.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "aircrack-ng/defs.h"
#include "crypto.h"
#include "tests.h"
#include "aircrack-ng/aircrack-ng.h"
#include <string.h>

int main(int argc, char ** argv)
{
	if (argc < 1) return 1;

	int error = 0;
	static unsigned char opmk[32]
		= "\x6f\xe8\x57\xc0\xb7\x42\xdf\xc2\xda\x8a\x1f\xe8\xb1\xb4\xb4\x62"
		  "\x8d\x9f\xbb\xb0\x60\x82\x6b\x83\xcb\x43\xb6\x4b\x13\xe1\x03\xe8";

	static unsigned char optk[80]
		= "\x28\x00\x00\x00\x00\x00\x00\x00\x18\x81\xd5\x96\xff\x7f\x00\x00"
		  "\x64\x00\x00\x00\x00\x00\x00\x00\x9f\x84\x00\x51\xd8\x7f\x00\x00"
		  "\x58\x33\xcf\x01\x00\x00\x00\x00\x21\x00\x00\x00\x00\x00\x00\x00"
		  "\x10\xb3\xd3\x01\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00"
		  "\x30\x9d\xd3\x01\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00";

	static unsigned char omic[20]
		= "\x01\x00\x00\x00\x00\x00\x00\x00\x64\x00\x00\x00\x00\x00\x00\x00"
		  "\x30\x9d\xd3\x01";

	//expected
	static unsigned char eptk[80]
		= "\x9a\x75\xef\x0b\xde\x7c\x20\x9c\xca\xe1\x3f\x54\xb1\xb3\x3e\xa3"
		  "\xfa\x01\x66\x16\x6e\x30\x81\x1f\x1b\xaa\xff\x8d\x1c\x1a\xc1\x63"
		  "\x82\xa4\x9b\x50\xb8\x60\x88\xc5\xcb\x79\x3b\x54\x26\x61\xa2\x8a"
		  "\x64\x69\x3c\x72\xb8\x66\x73\xe0\xa8\x2d\x48\xc7\x47\x68\xcb\xf9"
		  "\xa1\xbf\x86\xcc\x30\xb9\xd6\x3d\xe8\x82\x59\x25\xec\x1f\x4d\x3f";
	// expected emic when wpa.keyver=2
	static unsigned char emic2[20]
		= "\x6f\x04\x89\xcf\x74\x06\xac\xf0\xae\x8f\xcb\x32\xbc\xe5\x7c\x37"
		  "\x79\x3a\x23\x61";
	// expected emic when wpa.keyver=1
	static unsigned char emic1[20]
		= "\x9c\xc3\xfa\xa0\xc6\x85\x96\x1d\x84\x06\xbb\x65\x77\x45\x13\x5d"
		  "\x30\x9d\xd3\x01";
	// AP structure
	struct AP_info * ap;
	ap = (struct AP_info *) malloc(sizeof(struct AP_info));
	ALLEGE(ap != NULL);
	bzero(ap, sizeof(struct AP_info));

	unsigned char stmac[6] = "\x00\x13\x46\xfe\x32\x0c";
	unsigned char snonce[32]
		= "\x59\x16\x8b\xc3\xa5\xdf\x18\xd7\x1e\xfb\x64\x23\xf3\x40\x08\x8d"
		  "\xab\x9e\x1b\xa2\xbb\xc5\x86\x59\xe0\x7b\x37\x64\xb0\xde\x85\x70";
	unsigned char anonce[32]
		= "\x22\x58\x54\xb0\x44\x4d\xe3\xaf\x06\xd1\x49\x2b\x85\x29\x84\xf0"
		  "\x4c\xf6\x27\x4c\x0e\x32\x18\xb8\x68\x17\x56\x86\x4d\xb7\xa0\x55";
	unsigned char keymic[16]
		= "\xd5\x35\x53\x82\xb8\xa9\xb8\x06\xdc\xaf\x99\xcd\xaf\x56\x4e\xb6";
	unsigned char eapol[256]
		= "\x01\x03\x00\x75\x02\x01\x0a\x00\x10\x00\x00\x00\x00\x00\x00\x00"
		  "\x01\x59\x16\x8b\xc3\xa5\xdf\x18\xd7\x1e\xfb\x64\x23\xf3\x40\x08"
		  "\x8d\xab\x9e\x1b\xa2\xbb\xc5\x86\x59\xe0\x7b\x37\x64\xb0\xde\x85"
		  "\x70\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x16\x30\x14\x01\x00\x00\x0f\xac\x04\x01\x00\x00\x0f\xac"
		  "\x04\x01\x00\x00\x0f\xac\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	unsigned char bssid[6] = "\x00\x14\x6c\x7e\x40\x80";
	unsigned char essid[33] = "Harkonen";

	struct WPA_hdsk wpa; /* valid WPA handshake data     */
	memset(&wpa, 0, sizeof(wpa));
	memcpy(&wpa.stmac, &stmac, 6);
	memcpy(&wpa.snonce, &snonce, 32);
	memcpy(&wpa.anonce, &anonce, 32);
	memcpy(&wpa.keymic, &keymic, 16);
	memcpy(&wpa.eapol, &eapol, 256);
	wpa.eapol_size = 121;
	wpa.state = 7;
	memcpy(&ap->bssid, bssid, 6);
	memcpy(&ap->essid, essid, 33);

	unsigned char pmk[32];
	memcpy(&pmk, &opmk, 32);
	unsigned char ptk[80];
	memcpy(&ptk, &optk, 80);
	unsigned char mic[20];
	memcpy(&mic, &omic, 20);

	wpa.keyver = 1;
	ap->wpa = wpa;
	calc_mic(ap, pmk, ptk, mic);

	error += test(pmk, opmk, 32, argv[0]);
	error += test(ptk, eptk, 80, argv[0]);
	error += test(mic, emic1, 20, argv[0]);

	wpa.keyver = 2;
	ap->wpa = wpa;
	calc_mic(ap, pmk, ptk, mic);
	error += test(pmk, opmk, 32, argv[0]);
	error += test(ptk, eptk, 80, argv[0]);
	error += test(mic, emic2, 20, argv[0]);

	free(ap);

	return error;
}
