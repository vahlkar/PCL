//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 1.0.3
// ----------------------------------------------------------------------------
// jpc_util.c - Released 2024-12-23T11:33:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
//
// Copyright (c) 2003-2024 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (https://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

/*
 * Copyright (c) 1999-2000 Image Power, Inc. and the University of
 *   British Columbia.
 * Copyright (c) 2001-2003 Michael David Adams.
 * All rights reserved.
 */

/* __START_OF_JASPER_LICENSE__
 *
 * JasPer License Version 2.0
 *
 * Copyright (c) 2001-2006 Michael David Adams
 * Copyright (c) 1999-2000 Image Power, Inc.
 * Copyright (c) 1999-2000 The University of British Columbia
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person (the
 * "User") obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * 1.  The above copyright notices and this permission notice (which
 * includes the disclaimer below) shall be included in all copies or
 * substantial portions of the Software.
 *
 * 2.  The name of a copyright holder shall not be used to endorse or
 * promote products derived from the Software without specific prior
 * written permission.
 *
 * THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS
 * LICENSE.  NO USE OF THE SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
 * THIS DISCLAIMER.  THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  NO ASSURANCES ARE
 * PROVIDED BY THE COPYRIGHT HOLDERS THAT THE SOFTWARE DOES NOT INFRINGE
 * THE PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF ANY OTHER ENTITY.
 * EACH COPYRIGHT HOLDER DISCLAIMS ANY LIABILITY TO THE USER FOR CLAIMS
 * BROUGHT BY ANY OTHER ENTITY BASED ON INFRINGEMENT OF INTELLECTUAL
 * PROPERTY RIGHTS OR OTHERWISE.  AS A CONDITION TO EXERCISING THE RIGHTS
 * GRANTED HEREUNDER, EACH USER HEREBY ASSUMES SOLE RESPONSIBILITY TO SECURE
 * ANY OTHER INTELLECTUAL PROPERTY RIGHTS NEEDED, IF ANY.  THE SOFTWARE
 * IS NOT FAULT-TOLERANT AND IS NOT INTENDED FOR USE IN MISSION-CRITICAL
 * SYSTEMS, SUCH AS THOSE USED IN THE OPERATION OF NUCLEAR FACILITIES,
 * AIRCRAFT NAVIGATION OR COMMUNICATION SYSTEMS, AIR TRAFFIC CONTROL
 * SYSTEMS, DIRECT LIFE SUPPORT MACHINES, OR WEAPONS SYSTEMS, IN WHICH
 * THE FAILURE OF THE SOFTWARE OR SYSTEM COULD LEAD DIRECTLY TO DEATH,
 * PERSONAL INJURY, OR SEVERE PHYSICAL OR ENVIRONMENTAL DAMAGE ("HIGH
 * RISK ACTIVITIES").  THE COPYRIGHT HOLDERS SPECIFICALLY DISCLAIM ANY
 * EXPRESS OR IMPLIED WARRANTY OF FITNESS FOR HIGH RISK ACTIVITIES.
 *
 * __END_OF_JASPER_LICENSE__
 */

/*
 * $Id$
 */

/******************************************************************************\
* Includes
\******************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>

#include "jasper/jas_math.h"
#include "jasper/jas_malloc.h"

#include "jpc_fix.h"
#include "jpc_cs.h"
#include "jpc_flt.h"
#include "jpc_util.h"

/******************************************************************************\
* Miscellaneous Functions
\******************************************************************************/

int jpc_atoaf(char *s, int *numvalues, double **values)
{
	static char delim[] = ", \t\n";
	char buf[4096];
	int n;
	double *vs;
	char *cp;

	strncpy(buf, s, sizeof(buf));
	buf[sizeof(buf) - 1] = '\0';
	n = 0;
	if ((cp = strtok(buf, delim))) {
		++n;
		while ((cp = strtok(0, delim))) {
			if (cp != '\0') {
				++n;
			}
		}
	}

	if (n) {
		if (!(vs = jas_malloc(n * sizeof(double)))) {
			return -1;
		}

		strncpy(buf, s, sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';
		n = 0;
		if ((cp = strtok(buf, delim))) {
			vs[n] = atof(cp);
			++n;
			while ((cp = strtok(0, delim))) {
				if (cp != '\0') {
					vs[n] = atof(cp);
					++n;
				}
			}
		}
	} else {
		vs = 0;
	}

	*numvalues = n;
	*values = vs;

	return 0;
}

jas_seq_t *jpc_seq_upsample(jas_seq_t *x, int m)
{
	jas_seq_t *z;
	int i;

	if (!(z = jas_seq_create(jas_seq_start(x) * m, (jas_seq_end(x) - 1) * m + 1)))
		return 0;
	for (i = jas_seq_start(z); i < jas_seq_end(z); i++) {
		*jas_seq_getref(z, i) = (!JAS_MOD(i, m)) ? jas_seq_get(x, i / m) :
		  jpc_inttofix(0);
	}

	return z;
}

jpc_fix_t jpc_seq_norm(jas_seq_t *x)
{
	jpc_fix_t s;
	int i;

	s = jpc_inttofix(0);
	for (i = jas_seq_start(x); i < jas_seq_end(x); i++) {
		s = jpc_fix_add(s, jpc_fix_mul(jas_seq_get(x, i), jas_seq_get(x, i)));
	}

	return jpc_dbltofix(sqrt(jpc_fixtodbl(s)));
}

jas_seq_t *jpc_seq_conv(jas_seq_t *x, jas_seq_t *y)
{
	int i;
	int j;
	int k;
	jas_seq_t *z;
	jpc_fix_t s;
	jpc_fix_t v;

	z = jas_seq_create(jas_seq_start(x) + jas_seq_start(y),
	  jas_seq_end(x) + jas_seq_end(y) - 1);
	assert(z);
	for (i = jas_seq_start(z); i < jas_seq_end(z); i++) {
		s = jpc_inttofix(0);
		for (j = jas_seq_start(y); j < jas_seq_end(y); j++) {
			k = i - j;
			if (k < jas_seq_start(x) || k >= jas_seq_end(x)) {
				v = JPC_FIX_ZERO;
			} else {
				v = jas_seq_get(x, k);
			}
			s = jpc_fix_add(s, jpc_fix_mul(jas_seq_get(y, j), v));
		}
		*jas_seq_getref(z, i) = s;
	}

	return z;
}

// ----------------------------------------------------------------------------
// EOF jpc_util.c - Released 2024-12-23T11:33:12Z