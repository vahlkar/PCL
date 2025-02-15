//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.1
// ----------------------------------------------------------------------------
// eval_defs.h - Released 2024-12-28T16:54:05Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#if defined(__sgi) || defined(__hpux)
#include <alloca.h>
#endif
#ifdef sparc
#include <malloc.h>
#endif
#include "fitsio2.h"

#define MAXDIMS       5
#define MAXSUBS      10
#define MAXVARNAME   80
#define CONST_OP  -1000
#define pERROR       -1
#define MAX_STRLEN  256
#define MAX_STRLEN_S "255"

#ifndef FFBISON
#include "eval_tab.h"
#endif


typedef struct {
                  char   name[MAXVARNAME+1];
                  int    type;
                  long   nelem;
                  int    naxis;
                  long   naxes[MAXDIMS];
                  char   *undef;
                  void   *data;
                                } DataInfo;

typedef struct {
                  long   nelem;
                  int    naxis;
                  long   naxes[MAXDIMS];
                  char   *undef;
                  union {
                         double dbl;
                         long   lng;
                         char   log;
                         char   str[MAX_STRLEN];
                         double *dblptr;
                         long   *lngptr;
                         char   *logptr;
                         char   **strptr;
                         void   *ptr;
		  } data;
                                } lval;

typedef struct Node {
                  int    operation;
                  void   (*DoOp)(struct Node *this);
                  int    nSubNodes;
                  int    SubNodes[MAXSUBS];
                  int    type;
                  lval   value;
                                } Node;

typedef struct {
                  fitsfile    *def_fptr;
                  int         (*getData)( char *dataName, void *dataValue );
                  int         (*loadData)( int varNum, long fRow, long nRows,
					   void *data, char *undef );

                  int         compressed;
                  int         timeCol;
                  int         parCol;
                  int         valCol;

                  char        *expr;
                  int         index;
                  int         is_eobuf;

                  Node        *Nodes;
                  int         nNodes;
                  int         nNodesAlloc;
                  int         resultNode;

                  long        firstRow;
                  long        nRows;

                  int         nCols;
                  iteratorCol *colData;
                  DataInfo    *varData;
                  PixelFilter *pixFilter;

                  long        firstDataRow;
                  long        nDataRows;
                  long        totalRows;

                  int         datatype;
                  int         hdutype;

                  int         status;
                                } ParseData;

typedef enum {
                  rnd_fct = 1001,
                  sum_fct,
                  nelem_fct,
                  sin_fct,
                  cos_fct,
                  tan_fct,
                  asin_fct,
                  acos_fct,
                  atan_fct,
                  sinh_fct,
                  cosh_fct,
                  tanh_fct,
                  exp_fct,
                  log_fct,
                  log10_fct,
                  sqrt_fct,
                  abs_fct,
                  atan2_fct,
                  ceil_fct,
                  floor_fct,
                  round_fct,
		  min1_fct,
		  min2_fct,
		  max1_fct,
		  max2_fct,
                  near_fct,
                  circle_fct,
                  box_fct,
                  elps_fct,
                  isnull_fct,
                  defnull_fct,
                  gtifilt_fct,
                  regfilt_fct,
                  ifthenelse_fct,
                  row_fct,
                  null_fct,
		  median_fct,
		  average_fct,
		  stddev_fct,
		  nonnull_fct,
		  angsep_fct,
		  gasrnd_fct,
		  poirnd_fct,
		  strmid_fct,
		  strpos_fct,
		  setnull_fct,
		  gtiover_fct
                                } funcOp;

extern ParseData gParse;

#ifdef __cplusplus
extern "C" {
#endif

   int  ffparse(void);
   int  fflex(void);
   void ffrestart(FILE*);

   void Evaluate_Parser( long firstRow, long nRows );

#ifdef __cplusplus
    }
#endif

// ----------------------------------------------------------------------------
// EOF eval_defs.h - Released 2024-12-28T16:54:05Z
