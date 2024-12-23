//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.1
// ----------------------------------------------------------------------------
// eval_tab.h - Released 2024-12-23T11:33:12Z
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

/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with FF_ or ff_.  They are
   private implementation details that can be changed or removed.  */

#ifndef FF_FF_Y_TAB_H_INCLUDED
# define FF_FF_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef FFDEBUG
# define FFDEBUG 0
#endif
#if FFDEBUG
extern int ffdebug;
#endif

/* Token kinds.  */
#ifndef FFTOKENTYPE
# define FFTOKENTYPE
  enum fftokentype
  {
    FFEMPTY = -2,
    FFEOF = 0,                     /* "end of file"  */
    FFerror = 256,                 /* error  */
    FFUNDEF = 257,                 /* "invalid token"  */
    BOOLEAN = 258,                 /* BOOLEAN  */
    LONG = 259,                    /* LONG  */
    DOUBLE = 260,                  /* DOUBLE  */
    STRING = 261,                  /* STRING  */
    BITSTR = 262,                  /* BITSTR  */
    FUNCTION = 263,                /* FUNCTION  */
    BFUNCTION = 264,               /* BFUNCTION  */
    IFUNCTION = 265,               /* IFUNCTION  */
    GTIFILTER = 266,               /* GTIFILTER  */
    GTIOVERLAP = 267,              /* GTIOVERLAP  */
    REGFILTER = 268,               /* REGFILTER  */
    COLUMN = 269,                  /* COLUMN  */
    BCOLUMN = 270,                 /* BCOLUMN  */
    SCOLUMN = 271,                 /* SCOLUMN  */
    BITCOL = 272,                  /* BITCOL  */
    ROWREF = 273,                  /* ROWREF  */
    NULLREF = 274,                 /* NULLREF  */
    SNULLREF = 275,                /* SNULLREF  */
    OR = 276,                      /* OR  */
    AND = 277,                     /* AND  */
    EQ = 278,                      /* EQ  */
    NE = 279,                      /* NE  */
    GT = 280,                      /* GT  */
    LT = 281,                      /* LT  */
    LTE = 282,                     /* LTE  */
    GTE = 283,                     /* GTE  */
    XOR = 284,                     /* XOR  */
    POWER = 285,                   /* POWER  */
    NOT = 286,                     /* NOT  */
    INTCAST = 287,                 /* INTCAST  */
    FLTCAST = 288,                 /* FLTCAST  */
    UMINUS = 289,                  /* UMINUS  */
    ACCUM = 290,                   /* ACCUM  */
    DIFF = 291                     /* DIFF  */
  };
  typedef enum fftokentype fftoken_kind_t;
#endif
/* Token kinds.  */
#define FFEMPTY -2
#define FFEOF 0
#define FFerror 256
#define FFUNDEF 257
#define BOOLEAN 258
#define LONG 259
#define DOUBLE 260
#define STRING 261
#define BITSTR 262
#define FUNCTION 263
#define BFUNCTION 264
#define IFUNCTION 265
#define GTIFILTER 266
#define GTIOVERLAP 267
#define REGFILTER 268
#define COLUMN 269
#define BCOLUMN 270
#define SCOLUMN 271
#define BITCOL 272
#define ROWREF 273
#define NULLREF 274
#define SNULLREF 275
#define OR 276
#define AND 277
#define EQ 278
#define NE 279
#define GT 280
#define LT 281
#define LTE 282
#define GTE 283
#define XOR 284
#define POWER 285
#define NOT 286
#define INTCAST 287
#define FLTCAST 288
#define UMINUS 289
#define ACCUM 290
#define DIFF 291

/* Value type.  */
#if ! defined FFSTYPE && ! defined FFSTYPE_IS_DECLARED
union FFSTYPE
{
#line 199 "eval.y"

    int    Node;        /* Index of Node */
    double dbl;         /* real value    */
    long   lng;         /* integer value */
    char   log;         /* logical value */
    char   str[MAX_STRLEN];    /* string value  */

#line 147 "y.tab.h"

};
typedef union FFSTYPE FFSTYPE;
# define FFSTYPE_IS_TRIVIAL 1
# define FFSTYPE_IS_DECLARED 1
#endif


extern FFSTYPE fflval;

int ffparse (void);

#endif /* !FF_FF_Y_TAB_H_INCLUDED  */

// ----------------------------------------------------------------------------
// EOF eval_tab.h - Released 2024-12-23T11:33:12Z
