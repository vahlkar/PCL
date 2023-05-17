//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.8
// ----------------------------------------------------------------------------
//
// This file is part of the astjpl2pcl ephemeris generation utility.
//
// Copyright (c) 2017-2021 Pleiades Astrophoto S.L.
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
 * A command-line utility for conversion of JPL asteroid ephemerides in SPICE
 * binary kernel format (.bsp) to a PixInsight/PCL ephemeris data file in XEPH
 * format (.xeph).
 *
 * Written by Juan Conejero, PTeam.
 * Copyright (c) 2017-2022, Pleiades Astrophoto S.L.
 */

#include <pcl/Arguments.h>
#include <pcl/EphemerisFile.h>
#include <pcl/Random.h>

#include <iostream>
#include <wordexp.h>

#include "../cspice/include/SpiceUsr.h"

#define SOURCE_DENUM 441

#define au_km 149597870.7 // astronomical unit (km)

using namespace pcl;

// ----------------------------------------------------------------------------

#define PROGRAM_NAME    "astjpl2pcl"
#define PROGRAM_VERSION "1.40"
#define PROGRAM_YEAR    "2022"

#define TESTS_PER_DAY   4

#define SEPARATOR       IsoString( '-', 80 )

// ----------------------------------------------------------------------------

struct PhysicalData { double H, G, B_V, D; };

struct BodyData
{
   virtual int NumberOfObjects() const = 0;
   virtual EphemerisConstantList Constants() const = 0;
   virtual int BSPObjectIdentifier( int index ) const = 0;
   virtual int ObjectNumber( int index ) const = 0;
   virtual const char* ObjectName( int index ) const = 0;
   virtual const PhysicalData& ObjectData( int index ) const = 0;

   bool IsNumberedObject( int index ) const
   {
      return ObjectNumber( index ) > 0;
   }
};

// ----------------------------------------------------------------------------

struct AsteroidData : public BodyData
{
   int NumberOfObjects() const override
   {
      return 343;
   }

   EphemerisConstantList Constants() const
   {
      int n = 0;
      while( !s_constants[n].name.IsEmpty() )
         ++n;
      return EphemerisConstantList( s_constants, s_constants+n );
   }

   int BSPObjectIdentifier( int index ) const override
   {
      return s_bspIds[index];
   }

   int ObjectNumber( int index ) const override
   {
      return s_bspIds[index] - 2000000;
   }

   const char* ObjectName( int index ) const override
   {
      return s_names[index];
   }

   const PhysicalData& ObjectData( int index ) const override
   {
      return s_data[index];
   }

private:

   static const int s_bspIds[];
   static const char* s_names[];
   static const EphemerisConstant s_constants[];
   static const PhysicalData s_data[];
};

/*
 * SPICE body identifiers present in sb441-n373.bsp
 */
const int AsteroidData::s_bspIds[] =
{
   2000001, 2000002, 2000003, 2000004, 2000005, 2000006, 2000007, 2000008,
   2000009, 2000010, 2000011, 2000012, 2000013, 2000014, 2000015, 2000016,
   2000017, 2000018, 2000019, 2000020, 2000021, 2000022, 2000023, 2000024,
   2000025, 2000026, 2000027, 2000028, 2000029, 2000030, 2000031, 2000032,
   2000034, 2000035, 2000036, 2000037, 2000038, 2000039, 2000040, 2000041,
   2000042, 2000043, 2000044, 2000045, 2000046, 2000047, 2000048, 2000049,
   2000050, 2000051, 2000052, 2000053, 2000054, 2000056, 2000057, 2000058,
   2000059, 2000060, 2000062, 2000063, 2000065, 2000068, 2000069, 2000070,
   2000071, 2000072, 2000074, 2000075, 2000076, 2000077, 2000078, 2000079,
   2000080, 2000081, 2000082, 2000083, 2000084, 2000085, 2000086, 2000087,
   2000088, 2000089, 2000090, 2000091, 2000092, 2000093, 2000094, 2000095,
   2000096, 2000097, 2000098, 2000099, 2000100, 2000102, 2000103, 2000104,
   2000105, 2000106, 2000107, 2000109, 2000110, 2000111, 2000112, 2000113,
   2000114, 2000115, 2000117, 2000118, 2000120, 2000121, 2000124, 2000127,
   2000128, 2000129, 2000130, 2000132, 2000134, 2000135, 2000137, 2000139,
   2000140, 2000141, 2000143, 2000144, 2000145, 2000146, 2000147, 2000148,
   2000150, 2000154, 2000156, 2000159, 2000160, 2000162, 2000163, 2000164,
   2000165, 2000168, 2000171, 2000172, 2000173, 2000175, 2000176, 2000177,
   2000181, 2000185, 2000187, 2000191, 2000192, 2000194, 2000195, 2000196,
   2000198, 2000200, 2000201, 2000203, 2000205, 2000206, 2000209, 2000210,
   2000211, 2000212, 2000213, 2000216, 2000221, 2000223, 2000224, 2000225,
   2000227, 2000230, 2000233, 2000236, 2000238, 2000240, 2000241, 2000247,
   2000250, 2000259, 2000266, 2000268, 2000275, 2000276, 2000283, 2000287,
   2000303, 2000304, 2000308, 2000313, 2000322, 2000324, 2000326, 2000328,
   2000329, 2000334, 2000335, 2000336, 2000337, 2000338, 2000344, 2000345,
   2000346, 2000347, 2000349, 2000350, 2000354, 2000356, 2000357, 2000358,
   2000360, 2000362, 2000363, 2000365, 2000366, 2000369, 2000372, 2000373,
   2000375, 2000377, 2000381, 2000385, 2000386, 2000387, 2000388, 2000389,
   2000393, 2000404, 2000405, 2000407, 2000409, 2000410, 2000412, 2000415,
   2000416, 2000419, 2000420, 2000423, 2000424, 2000426, 2000431, 2000432,
   2000433, 2000442, 2000444, 2000445, 2000449, 2000451, 2000454, 2000455,
   2000464, 2000465, 2000466, 2000469, 2000471, 2000476, 2000481, 2000485,
   2000488, 2000489, 2000490, 2000491, 2000498, 2000503, 2000505, 2000506,
   2000508, 2000511, 2000514, 2000516, 2000517, 2000521, 2000532, 2000535,
   2000536, 2000545, 2000547, 2000554, 2000566, 2000568, 2000569, 2000584,
   2000585, 2000591, 2000593, 2000595, 2000596, 2000598, 2000599, 2000602,
   2000604, 2000618, 2000623, 2000626, 2000635, 2000654, 2000663, 2000667,
   2000674, 2000675, 2000680, 2000683, 2000690, 2000691, 2000694, 2000696,
   2000702, 2000704, 2000705, 2000709, 2000712, 2000713, 2000735, 2000739,
   2000740, 2000747, 2000751, 2000752, 2000760, 2000762, 2000769, 2000772,
   2000773, 2000776, 2000778, 2000780, 2000784, 2000786, 2000788, 2000790,
   2000791, 2000804, 2000814, 2000849, 2000895, 2000909, 2000914, 2000980,
   2001015, 2001021, 2001036, 2001093, 2001107, 2001171, 2001467
};

/*
 * Asteroid designation names.
 *
 * Source:
 *
 *    https://asteroid.lowell.edu/main/astorb/
 */
const char* AsteroidData::s_names[] =
{
   "Ceres",             //      1 Ceres
   "Pallas",            //      2 Pallas
   "Juno",              //      3 Juno
   "Vesta",             //      4 Vesta
   "Astraea",           //      5 Astraea
   "Hebe",              //      6 Hebe
   "Iris",              //      7 Iris
   "Flora",             //      8 Flora
   "Metis",             //      9 Metis
   "Hygiea",            //     10 Hygiea
   "Parthenope",        //     11 Parthenope
   "Victoria",          //     12 Victoria
   "Egeria",            //     13 Egeria
   "Irene",             //     14 Irene
   "Eunomia",           //     15 Eunomia
   "Psyche",            //     16 Psyche
   "Thetis",            //     17 Thetis
   "Melpomene",         //     18 Melpomene
   "Fortuna",           //     19 Fortuna
   "Massalia",          //     20 Massalia
   "Lutetia",           //     21 Lutetia
   "Kalliope",          //     22 Kalliope
   "Thalia",            //     23 Thalia
   "Themis",            //     24 Themis
   "Phocaea",           //     25 Phocaea
   "Proserpina",        //     26 Proserpina
   "Euterpe",           //     27 Euterpe
   "Bellona",           //     28 Bellona
   "Amphitrite",        //     29 Amphitrite
   "Urania",            //     30 Urania
   "Euphrosyne",        //     31 Euphrosyne
   "Pomona",            //     32 Pomona
   "Circe",             //     34 Circe
   "Leukothea",         //     35 Leukothea
   "Atalante",          //     36 Atalante
   "Fides",             //     37 Fides
   "Leda",              //     38 Leda
   "Laetitia",          //     39 Laetitia
   "Harmonia",          //     40 Harmonia
   "Daphne",            //     41 Daphne
   "Isis",              //     42 Isis
   "Ariadne",           //     43 Ariadne
   "Nysa",              //     44 Nysa
   "Eugenia",           //     45 Eugenia
   "Hestia",            //     46 Hestia
   "Aglaja",            //     47 Aglaja
   "Doris",             //     48 Doris
   "Pales",             //     49 Pales
   "Virginia",          //     50 Virginia
   "Nemausa",           //     51 Nemausa
   "Europa",            //     52 Europa
   "Kalypso",           //     53 Kalypso
   "Alexandra",         //     54 Alexandra
   "Melete",            //     56 Melete
   "Mnemosyne",         //     57 Mnemosyne
   "Concordia",         //     58 Concordia
   "Elpis",             //     59 Elpis
   "Echo",              //     60 Echo
   "Erato",             //     62 Erato
   "Ausonia",           //     63 Ausonia
   "Cybele",            //     65 Cybele
   "Leto",              //     68 Leto
   "Hesperia",          //     69 Hesperia
   "Panopaea",          //     70 Panopaea
   "Niobe",             //     71 Niobe
   "Feronia",           //     72 Feronia
   "Galatea",           //     74 Galatea
   "Eurydike",          //     75 Eurydike
   "Freia",             //     76 Freia
   "Frigga",            //     77 Frigga
   "Diana",             //     78 Diana
   "Eurynome",          //     79 Eurynome
   "Sappho",            //     80 Sappho
   "Terpsichore",       //     81 Terpsichore
   "Alkmene",           //     82 Alkmene
   "Beatrix",           //     83 Beatrix
   "Klio",              //     84 Klio
   "Io",                //     85 Io
   "Semele",            //     86 Semele
   "Sylvia",            //     87 Sylvia
   "Thisbe",            //     88 Thisbe
   "Julia",             //     89 Julia
   "Antiope",           //     90 Antiope
   "Aegina",            //     91 Aegina
   "Undina",            //     92 Undina
   "Minerva",           //     93 Minerva
   "Aurora",            //     94 Aurora
   "Arethusa",          //     95 Arethusa
   "Aegle",             //     96 Aegle
   "Klotho",            //     97 Klotho
   "Ianthe",            //     98 Ianthe
   "Dike",              //     99 Dike
   "Hekate",            //    100 Hekate
   "Miriam",            //    102 Miriam
   "Hera",              //    103 Hera
   "Klymene",           //    104 Klymene
   "Artemis",           //    105 Artemis
   "Dione",             //    106 Dione
   "Camilla",           //    107 Camilla
   "Felicitas",         //    109 Felicitas
   "Lydia",             //    110 Lydia
   "Ate",               //    111 Ate
   "Iphigenia",         //    112 Iphigenia
   "Amalthea",          //    113 Amalthea
   "Kassandra",         //    114 Kassandra
   "Thyra",             //    115 Thyra
   "Lomia",             //    117 Lomia
   "Peitho",            //    118 Peitho
   "Lachesis",          //    120 Lachesis
   "Hermione",          //    121 Hermione
   "Alkeste",           //    124 Alkeste
   "Johanna",           //    127 Johanna
   "Nemesis",           //    128 Nemesis
   "Antigone",          //    129 Antigone
   "Elektra",           //    130 Elektra
   "Aethra",            //    132 Aethra
   "Sophrosyne",        //    134 Sophrosyne
   "Hertha",            //    135 Hertha
   "Meliboea",          //    137 Meliboea
   "Juewa",             //    139 Juewa
   "Siwa",              //    140 Siwa
   "Lumen",             //    141 Lumen
   "Adria",             //    143 Adria
   "Vibilia",           //    144 Vibilia
   "Adeona",            //    145 Adeona
   "Lucina",            //    146 Lucina
   "Protogeneia",       //    147 Protogeneia
   "Gallia",            //    148 Gallia
   "Nuwa",              //    150 Nuwa
   "Bertha",            //    154 Bertha
   "Xanthippe",         //    156 Xanthippe
   "Aemilia",           //    159 Aemilia
   "Una",               //    160 Una
   "Laurentia",         //    162 Laurentia
   "Erigone",           //    163 Erigone
   "Eva",               //    164 Eva
   "Loreley",           //    165 Loreley
   "Sibylla",           //    168 Sibylla
   "Ophelia",           //    171 Ophelia
   "Baucis",            //    172 Baucis
   "Ino",               //    173 Ino
   "Andromache",        //    175 Andromache
   "Iduna",             //    176 Iduna
   "Irma",              //    177 Irma
   "Eucharis",          //    181 Eucharis
   "Eunike",            //    185 Eunike
   "Lamberta",          //    187 Lamberta
   "Kolga",             //    191 Kolga
   "Nausikaa",          //    192 Nausikaa
   "Prokne",            //    194 Prokne
   "Eurykleia",         //    195 Eurykleia
   "Philomela",         //    196 Philomela
   "Ampella",           //    198 Ampella
   "Dynamene",          //    200 Dynamene
   "Penelope",          //    201 Penelope
   "Pompeja",           //    203 Pompeja
   "Martha",            //    205 Martha
   "Hersilia",          //    206 Hersilia
   "Dido",              //    209 Dido
   "Isabella",          //    210 Isabella
   "Isolda",            //    211 Isolda
   "Medea",             //    212 Medea
   "Lilaea",            //    213 Lilaea
   "Kleopatra",         //    216 Kleopatra
   "Eos",               //    221 Eos
   "Rosa",              //    223 Rosa
   "Oceana",            //    224 Oceana
   "Henrietta",         //    225 Henrietta
   "Philosophia",       //    227 Philosophia
   "Athamantis",        //    230 Athamantis
   "Asterope",          //    233 Asterope
   "Honoria",           //    236 Honoria
   "Hypatia",           //    238 Hypatia
   "Vanadis",           //    240 Vanadis
   "Germania",          //    241 Germania
   "Eukrate",           //    247 Eukrate
   "Bettina",           //    250 Bettina
   "Aletheia",          //    259 Aletheia
   "Aline",             //    266 Aline
   "Adorea",            //    268 Adorea
   "Sapientia",         //    275 Sapientia
   "Adelheid",          //    276 Adelheid
   "Emma",              //    283 Emma
   "Nephthys",          //    287 Nephthys
   "Josephina",         //    303 Josephina
   "Olga",              //    304 Olga
   "Polyxo",            //    308 Polyxo
   "Chaldaea",          //    313 Chaldaea
   "Phaeo",             //    322 Phaeo
   "Bamberga",          //    324 Bamberga
   "Tamara",            //    326 Tamara
   "Gudrun",            //    328 Gudrun
   "Svea",              //    329 Svea
   "Chicago",           //    334 Chicago
   "Roberta",           //    335 Roberta
   "Lacadiera",         //    336 Lacadiera
   "Devosa",            //    337 Devosa
   "Budrosa",           //    338 Budrosa
   "Desiderata",        //    344 Desiderata
   "Tercidina",         //    345 Tercidina
   "Hermentaria",       //    346 Hermentaria
   "Pariana",           //    347 Pariana
   "Dembowska",         //    349 Dembowska
   "Ornamenta",         //    350 Ornamenta
   "Eleonora",          //    354 Eleonora
   "Liguria",           //    356 Liguria
   "Ninina",            //    357 Ninina
   "Apollonia",         //    358 Apollonia
   "Carlova",           //    360 Carlova
   "Havnia",            //    362 Havnia
   "Padua",             //    363 Padua
   "Corduba",           //    365 Corduba
   "Vincentina",        //    366 Vincentina
   "Aeria",             //    369 Aeria
   "Palma",             //    372 Palma
   "Melusina",          //    373 Melusina
   "Ursula",            //    375 Ursula
   "Campania",          //    377 Campania
   "Myrrha",            //    381 Myrrha
   "Ilmatar",           //    385 Ilmatar
   "Siegena",           //    386 Siegena
   "Aquitania",         //    387 Aquitania
   "Charybdis",         //    388 Charybdis
   "Industria",         //    389 Industria
   "Lampetia",          //    393 Lampetia
   "Arsinoe",           //    404 Arsinoe
   "Thia",              //    405 Thia
   "Arachne",           //    407 Arachne
   "Aspasia",           //    409 Aspasia
   "Chloris",           //    410 Chloris
   "Elisabetha",        //    412 Elisabetha
   "Palatia",           //    415 Palatia
   "Vaticana",          //    416 Vaticana
   "Aurelia",           //    419 Aurelia
   "Bertholda",         //    420 Bertholda
   "Diotima",           //    423 Diotima
   "Gratia",            //    424 Gratia
   "Hippo",             //    426 Hippo
   "Nephele",           //    431 Nephele
   "Pythia",            //    432 Pythia
   "Eros",              //    433 Eros
   "Eichsfeldia",       //    442 Eichsfeldia
   "Gyptis",            //    444 Gyptis
   "Edna",              //    445 Edna
   "Hamburga",          //    449 Hamburga
   "Patientia",         //    451 Patientia
   "Mathesis",          //    454 Mathesis
   "Bruchsalia",        //    455 Bruchsalia
   "Megaira",           //    464 Megaira
   "Alekto",            //    465 Alekto
   "Tisiphone",         //    466 Tisiphone
   "Argentina",         //    469 Argentina
   "Papagena",          //    471 Papagena
   "Hedwig",            //    476 Hedwig
   "Emita",             //    481 Emita
   "Genua",             //    485 Genua
   "Kreusa",            //    488 Kreusa
   "Comacina",          //    489 Comacina
   "Veritas",           //    490 Veritas
   "Carina",            //    491 Carina
   "Tokio",             //    498 Tokio
   "Evelyn",            //    503 Evelyn
   "Cava",              //    505 Cava
   "Marion",            //    506 Marion
   "Princetonia",       //    508 Princetonia
   "Davida",            //    511 Davida
   "Armida",            //    514 Armida
   "Amherstia",         //    516 Amherstia
   "Edith",             //    517 Edith
   "Brixia",            //    521 Brixia
   "Herculina",         //    532 Herculina
   "Montague",          //    535 Montague
   "Merapi",            //    536 Merapi
   "Messalina",         //    545 Messalina
   "Praxedis",          //    547 Praxedis
   "Peraga",            //    554 Peraga
   "Stereoskopia",      //    566 Stereoskopia
   "Cheruskia",         //    568 Cheruskia
   "Misa",              //    569 Misa
   "Semiramis",         //    584 Semiramis
   "Bilkis",            //    585 Bilkis
   "Irmgard",           //    591 Irmgard
   "Titania",           //    593 Titania
   "Polyxena",          //    595 Polyxena
   "Scheila",           //    596 Scheila
   "Octavia",           //    598 Octavia
   "Luisa",             //    599 Luisa
   "Marianna",          //    602 Marianna
   "Tekmessa",          //    604 Tekmessa
   "Elfriede",          //    618 Elfriede
   "Chimaera",          //    623 Chimaera
   "Notburga",          //    626 Notburga
   "Vundtia",           //    635 Vundtia
   "Zelinda",           //    654 Zelinda
   "Gerlinde",          //    663 Gerlinde
   "Denise",            //    667 Denise
   "Rachele",           //    674 Rachele
   "Ludmilla",          //    675 Ludmilla
   "Genoveva",          //    680 Genoveva
   "Lanzia",            //    683 Lanzia
   "Wratislavia",       //    690 Wratislavia
   "Lehigh",            //    691 Lehigh
   "Ekard",             //    694 Ekard
   "Leonora",           //    696 Leonora
   "Alauda",            //    702 Alauda
   "Interamnia",        //    704 Interamnia
   "Erminia",           //    705 Erminia
   "Fringilla",         //    709 Fringilla
   "Boliviana",         //    712 Boliviana
   "Luscinia",          //    713 Luscinia
   "Marghanna",         //    735 Marghanna
   "Mandeville",        //    739 Mandeville
   "Cantabia",          //    740 Cantabia
   "Winchester",        //    747 Winchester
   "Faina",             //    751 Faina
   "Sulamitis",         //    752 Sulamitis
   "Massinga",          //    760 Massinga
   "Pulcova",           //    762 Pulcova
   "Tatjana",           //    769 Tatjana
   "Tanete",            //    772 Tanete
   "Irmintraud",        //    773 Irmintraud
   "Berbericia",        //    776 Berbericia
   "Theobalda",         //    778 Theobalda
   "Armenia",           //    780 Armenia
   "Pickeringia",       //    784 Pickeringia
   "Bredichina",        //    786 Bredichina
   "Hohensteina",       //    788 Hohensteina
   "Pretoria",          //    790 Pretoria
   "Ani",               //    791 Ani
   "Hispania",          //    804 Hispania
   "Tauris",            //    814 Tauris
   "Ara",               //    849 Ara
   "Helio",             //    895 Helio
   "Ulla",              //    909 Ulla
   "Palisana",          //    914 Palisana
   "Anacostia",         //    980 Anacostia
   "Christa",           //   1015 Christa
   "Flammario",         //   1021 Flammario
   "Ganymed",           //   1036 Ganymed
   "Freda",             //   1093 Freda
   "Lictoria",          //   1107 Lictoria
   "Rusthawelia",       //   1171 Rusthawelia
   "Mashona",           //   1467 Mashona
};

/*
 * Asteroid constants (DE440)
 */
const EphemerisConstant AsteroidData::s_constants[] =
{
   { "DENUM", 441 },
   { "LENUM", 441 },
   { "MA0001", 1.39645181230811e-13 },
   { "MA0002", 3.04711463300432e-14 },
   { "MA0003", 4.2823439677995e-15 },
   { "MA0004", 3.85480002252579e-14 },
   { "MA0005", 3.21619332418551e-16 },
   { "MA0006", 1.4423591761343e-15 },
   { "MA0007", 2.54160149734715e-15 },
   { "MA0008", 5.74026664750182e-16 },
   { "MA0009", 1.44901380258673e-15 },
   { "MA0010", 1.25425307616408e-14 },
   { "MA0011", 1.02585304874317e-15 },
   { "MA0012", 3.2843482824405e-16 },
   { "MA0013", 1.13894367618095e-15 },
   { "MA0014", 1.18637951399481e-15 },
   { "MA0015", 4.51077990514368e-15 },
   { "MA0016", 3.5445002842489e-15 },
   { "MA0017", 1.33986830978408e-16 },
   { "MA0018", 4.81811498481748e-16 },
   { "MA0019", 1.23961751827942e-15 },
   { "MA0020", 5.71341843864351e-16 },
   { "MA0021", 1.97600289138557e-16 },
   { "MA0022", 8.80549450358822e-16 },
   { "MA0023", 2.76730672117019e-16 },
   { "MA0024", 1.31091704442499e-15 },
   { "MA0025", 3.25608002006949e-17 },
   { "MA0026", 1.78126168141549e-16 },
   { "MA0027", 2.56138658041522e-16 },
   { "MA0028", 3.40439312360134e-16 },
   { "MA0029", 1.77887595703788e-15 },
   { "MA0030", 5.70572637578975e-17 },
   { "MA0031", 2.40670122189376e-15 },
   { "MA0032", 7.874507166651941e-17 },
   { "MA0034", 3.1392144602052e-16 },
   { "MA0035", 8.27092316788029e-17 },
   { "MA0036", 5.01359738348698e-16 },
   { "MA0037", 2.42813055082719e-16 },
   { "MA0038", 8.16620722110261e-17 },
   { "MA0039", 1.62380801682844e-15 },
   { "MA0040", 3.73938266300153e-16 },
   { "MA0041", 1.20556160511445e-15 },
   { "MA0042", 1.87352579647547e-16 },
   { "MA0043", 1.26835029625616e-16 },
   { "MA0044", 8.1199988686105e-17 },
   { "MA0045", 8.03753516955151e-16 },
   { "MA0046", 8.29241098968398e-16 },
   { "MA0047", 9.548452182565239e-16 },
   { "MA0048", 1.90851619564856e-15 },
   { "MA0049", 8.03090195615854e-16 },
   { "MA0050", 9.06581360094101e-17 },
   { "MA0051", 5.596605208563e-16 },
   { "MA0052", 5.98243152648698e-15 },
   { "MA0053", 7.70956850256512e-17 },
   { "MA0054", 2.56136438567356e-16 },
   { "MA0056", 4.33343892852681e-16 },
   { "MA0057", 7.47264128935916e-17 },
   { "MA0058", 1.80125058118294e-16 },
   { "MA0059", 6.12424400382788e-16 },
   { "MA0060", 1.48260785652372e-17 },
   { "MA0062", 1.52134282239418e-16 },
   { "MA0063", 2.8598833773737e-16 },
   { "MA0065", 2.09171759551337e-15 },
   { "MA0068", 1.17065701912549e-16 },
   { "MA0069", 1.12154899587192e-15 },
   { "MA0070", 1.22101330213601e-16 },
   { "MA0071", 1.31207839384676e-16 },
   { "MA0072", 1.50869699373602e-16 },
   { "MA0074", 1.90117169367528e-16 },
   { "MA0075", 5.802745629798351e-17 },
   { "MA0076", 7.19653486720343e-16 },
   { "MA0077", 5.27246011683316e-17 },
   { "MA0078", 2.03680975588021e-16 },
   { "MA0079", 4.82422691817461e-17 },
   { "MA0080", 5.64711542750602e-17 },
   { "MA0081", 1.26592779662381e-16 },
   { "MA0082", 3.82856576672486e-17 },
   { "MA0083", 1.84599026937674e-16 },
   { "MA0084", 7.495697601430941e-17 },
   { "MA0085", 8.41056811344799e-16 },
   { "MA0086", 1.43675770234523e-16 },
   { "MA0087", 4.83456065461055e-15 },
   { "MA0088", 2.65294366103564e-15 },
   { "MA0089", 5.69980324451091e-16 },
   { "MA0090", 2.64288160058928e-16 },
   { "MA0091", 1.41959551770215e-16 },
   { "MA0092", 2.02224992859183e-16 },
   { "MA0093", 5.89093236875739e-16 },
   { "MA0094", 1.94764996096595e-15 },
   { "MA0095", 4.60264620685128e-16 },
   { "MA0096", 9.429421213058231e-16 },
   { "MA0097", 1.02680483535917e-16 },
   { "MA0098", 3.18643126547546e-16 },
   { "MA0099", 5.01460721119004e-17 },
   { "MA0100", 8.55086214093481e-17 },
   { "MA0102", 7.27126209812424e-17 },
   { "MA0103", 1.28735527456932e-16 },
   { "MA0104", 4.41372619240651e-16 },
   { "MA0105", 1.40493436967981e-16 },
   { "MA0106", 5.93398507009177e-17 },
   { "MA0107", 3.21913920758786e-15 },
   { "MA0109", 8.73429262293014e-17 },
   { "MA0110", 1.12732776239529e-16 },
   { "MA0111", 1.26859892785605e-16 },
   { "MA0112", 3.05196264584504e-17 },
   { "MA0113", 3.08199252056106e-17 },
   { "MA0114", 2.48840670065409e-16 },
   { "MA0115", 6.97354060243925e-17 },
   { "MA0117", 8.7956326624913e-16 },
   { "MA0118", 1.3157171164549e-17 },
   { "MA0120", 1.00492838679778e-15 },
   { "MA0121", 3.00277555811353e-16 },
   { "MA0124", 1.40825167421588e-16 },
   { "MA0127", 2.47549013467068e-16 },
   { "MA0128", 8.58478613114104e-16 },
   { "MA0129", 1.36944412847691e-16 },
   { "MA0130", 1.32728357170794e-15 },
   { "MA0132", 1.7504352843147e-17 },
   { "MA0134", 1.20414937852489e-16 },
   { "MA0135", 1.45160533468279e-16 },
   { "MA0137", 2.62916675709007e-16 },
   { "MA0139", 9.259555216971149e-16 },
   { "MA0140", 3.34517527522678e-16 },
   { "MA0141", 2.18893338454578e-16 },
   { "MA0143", 2.49831855261886e-16 },
   { "MA0144", 4.07383807077759e-16 },
   { "MA0145", 3.13194935034274e-16 },
   { "MA0146", 7.41455178208835e-16 },
   { "MA0147", 2.48585784236813e-16 },
   { "MA0148", 2.76351178724323e-16 },
   { "MA0150", 2.6906654264657e-16 },
   { "MA0154", 1.90628822336245e-15 },
   { "MA0156", 2.79079964280365e-16 },
   { "MA0159", 1.92362190199697e-16 },
   { "MA0160", 6.51425133155023e-17 },
   { "MA0162", 9.90126891655138e-17 },
   { "MA0163", 1.35014251190632e-16 },
   { "MA0164", 2.68563632620583e-16 },
   { "MA0165", 2.64542996155253e-16 },
   { "MA0168", 8.28621512309755e-16 },
   { "MA0171", 3.02625313061994e-16 },
   { "MA0172", 4.65882712210888e-17 },
   { "MA0173", 3.4944452970189e-16 },
   { "MA0175", 7.312424159555e-17 },
   { "MA0176", 1.57319446399408e-16 },
   { "MA0177", 3.1948857613519e-17 },
   { "MA0181", 5.12333989772635e-16 },
   { "MA0185", 1.45410927125493e-16 },
   { "MA0187", 6.99887622171429e-16 },
   { "MA0191", 9.27005095952984e-17 },
   { "MA0192", 8.14212914029534e-17 },
   { "MA0194", 2.45615281440433e-16 },
   { "MA0195", 8.83169773790907e-17 },
   { "MA0196", 7.18836917178432e-16 },
   { "MA0198", 3.46909649380599e-17 },
   { "MA0200", 1.50637432266567e-16 },
   { "MA0201", 1.66630457714335e-16 },
   { "MA0203", 5.18047904379015e-16 },
   { "MA0205", 5.23643121338054e-17 },
   { "MA0206", 1.74859108074354e-16 },
   { "MA0209", 1.95110081345066e-15 },
   { "MA0210", 7.663486323409291e-17 },
   { "MA0211", 7.56017985074422e-17 },
   { "MA0212", 4.46051068920035e-16 },
   { "MA0213", 7.28141021017853e-17 },
   { "MA0216", 4.14272884440808e-16 },
   { "MA0221", 1.45350638822454e-16 },
   { "MA0223", 5.52108338255617e-17 },
   { "MA0224", 4.63845032277471e-17 },
   { "MA0225", 6.14884891365313e-17 },
   { "MA0227", 5.29497278932255e-16 },
   { "MA0230", 2.65468963098069e-16 },
   { "MA0233", 1.99796243310735e-16 },
   { "MA0236", 6.25172630260285e-17 },
   { "MA0238", 2.21885614973532e-16 },
   { "MA0240", 9.25549528015023e-17 },
   { "MA0241", 8.25789188551896e-16 },
   { "MA0247", 7.21664731245258e-16 },
   { "MA0250", 4.69449662479354e-16 },
   { "MA0259", 4.52510894350013e-16 },
   { "MA0266", 1.65942825802938e-16 },
   { "MA0268", 3.86230426137087e-16 },
   { "MA0275", 1.25604156361746e-16 },
   { "MA0276", 1.99221951461379e-16 },
   { "MA0283", 3.83548152460466e-16 },
   { "MA0287", 5.10654257680056e-17 },
   { "MA0303", 1.22506307360402e-16 },
   { "MA0304", 8.00596320733248e-17 },
   { "MA0308", 2.16478043709593e-16 },
   { "MA0313", 7.187570798689381e-17 },
   { "MA0322", 8.07283772777276e-17 },
   { "MA0324", 1.38086815862224e-15 },
   { "MA0326", 6.832658092241449e-17 },
   { "MA0328", 4.99824683086338e-16 },
   { "MA0329", 5.02257992032068e-17 },
   { "MA0334", 2.41362203396293e-16 },
   { "MA0335", 2.08431326670006e-16 },
   { "MA0336", 1.42521176468771e-17 },
   { "MA0337", 7.310312768200539e-17 },
   { "MA0338", 2.86498270917452e-17 },
   { "MA0344", 1.31669858225866e-16 },
   { "MA0345", 1.58914794461121e-16 },
   { "MA0346", 1.09853286291396e-16 },
   { "MA0347", 2.48421401829459e-17 },
   { "MA0349", 7.77703920720892e-16 },
   { "MA0350", 2.78579991447994e-16 },
   { "MA0354", 6.97994275886899e-16 },
   { "MA0356", 2.518981520463e-16 },
   { "MA0357", 2.87212631880474e-16 },
   { "MA0358", 9.67079840085141e-17 },
   { "MA0360", 2.04879876408272e-16 },
   { "MA0362", 1.12767227654201e-16 },
   { "MA0363", 2.2392126408314e-16 },
   { "MA0365", 8.43960085860973e-17 },
   { "MA0366", 7.97894683254266e-17 },
   { "MA0369", 8.33678241022542e-17 },
   { "MA0372", 1.23405643592201e-15 },
   { "MA0373", 1.2907423434479e-16 },
   { "MA0375", 1.77168752949087e-15 },
   { "MA0377", 9.30341596097548e-17 },
   { "MA0381", 4.49332166347459e-16 },
   { "MA0385", 9.97618562851644e-17 },
   { "MA0386", 1.67445203431828e-15 },
   { "MA0387", 3.43830444971767e-16 },
   { "MA0388", 2.68662091450757e-16 },
   { "MA0389", 8.00593392574199e-17 },
   { "MA0393", 3.35722794024334e-16 },
   { "MA0404", 1.61447604740328e-16 },
   { "MA0405", 2.83434420275902e-16 },
   { "MA0407", 1.21596496918498e-16 },
   { "MA0409", 9.30713018029893e-16 },
   { "MA0410", 3.15273193703125e-16 },
   { "MA0412", 1.2572594904493e-16 },
   { "MA0415", 8.05730774756706e-17 },
   { "MA0416", 1.65118521510885e-16 },
   { "MA0419", 2.37735782092059e-16 },
   { "MA0420", 2.36701785210818e-16 },
   { "MA0423", 1.15114521954007e-15 },
   { "MA0424", 1.10201568256769e-16 },
   { "MA0426", 3.2507694999798e-16 },
   { "MA0431", 1.39813367334854e-16 },
   { "MA0432", 1.36018885581729e-17 },
   { "MA0433", 9.95060015546485e-19 },
   { "MA0442", 3.76810350984612e-17 },
   { "MA0444", 4.24710309935973e-16 },
   { "MA0445", 8.53156150352362e-17 },
   { "MA0449", 1.13817153283164e-16 },
   { "MA0451", 1.29737970460976e-15 },
   { "MA0454", 6.679819241829379e-17 },
   { "MA0455", 1.14813333450226e-16 },
   { "MA0464", 5.82330264028416e-17 },
   { "MA0465", 5.18671952851021e-17 },
   { "MA0466", 7.52835065502048e-17 },
   { "MA0469", 3.64784501100281e-16 },
   { "MA0471", 1.01549463170003e-15 },
   { "MA0476", 9.32908494870791e-16 },
   { "MA0481", 1.42324789182302e-16 },
   { "MA0485", 4.58811477212881e-17 },
   { "MA0488", 7.952288319351e-16 },
   { "MA0489", 2.8775529812776e-16 },
   { "MA0490", 1.42680680510889e-16 },
   { "MA0491", 9.03272139992083e-17 },
   { "MA0498", 1.61661640337002e-16 },
   { "MA0503", 8.20767517722816e-17 },
   { "MA0505", 1.14345247459548e-16 },
   { "MA0506", 1.90698398460577e-16 },
   { "MA0508", 2.60037470335652e-16 },
   { "MA0511", 8.68362534922865e-15 },
   { "MA0514", 1.88612957553386e-16 },
   { "MA0516", 5.66302763855171e-17 },
   { "MA0517", 1.76559296118362e-16 },
   { "MA0521", 1.55841136478987e-16 },
   { "MA0532", 1.76512956511264e-15 },
   { "MA0535", 6.05066378625397e-17 },
   { "MA0536", 2.7333272315776e-16 },
   { "MA0545", 1.95198500832437e-16 },
   { "MA0547", 3.28901485233798e-17 },
   { "MA0554", 1.59355217115579e-16 },
   { "MA0566", 1.70078189252838e-15 },
   { "MA0568", 7.974665095828319e-17 },
   { "MA0569", 5.56926103195199e-17 },
   { "MA0584", 3.88492431566867e-17 },
   { "MA0585", 9.502726030864261e-18 },
   { "MA0591", 1.56500829305499e-17 },
   { "MA0593", 4.31526602257911e-17 },
   { "MA0595", 2.24650302401692e-16 },
   { "MA0596", 8.75038546237458e-16 },
   { "MA0598", 9.20338366615999e-17 },
   { "MA0599", 1.01237773161802e-16 },
   { "MA0602", 2.28096749090148e-16 },
   { "MA0604", 4.6458511951547e-17 },
   { "MA0618", 2.51932048892072e-16 },
   { "MA0623", 1.01209115870928e-17 },
   { "MA0626", 7.022076906428379e-17 },
   { "MA0635", 1.09243838693739e-16 },
   { "MA0654", 1.82711538450282e-16 },
   { "MA0663", 1.97255377298637e-16 },
   { "MA0667", 1.71747505961314e-16 },
   { "MA0674", 9.93535725748886e-17 },
   { "MA0675", 1.0490232752479e-16 },
   { "MA0680", 8.150019061382231e-17 },
   { "MA0683", 1.20018469405368e-16 },
   { "MA0690", 2.58590895566818e-16 },
   { "MA0691", 5.12097485348418e-17 },
   { "MA0694", 5.10097980488896e-17 },
   { "MA0696", 6.7737156689916e-17 },
   { "MA0702", 1.67302623984412e-15 },
   { "MA0704", 6.31103434208789e-15 },
   { "MA0705", 3.91167523951147e-16 },
   { "MA0709", 1.19320458542741e-16 },
   { "MA0712", 1.2600845433724e-16 },
   { "MA0713", 1.11491557045087e-16 },
   { "MA0735", 7.56555019263405e-17 },
   { "MA0739", 1.18957042352097e-16 },
   { "MA0740", 1.15082712238747e-16 },
   { "MA0747", 8.916216061479979e-16 },
   { "MA0751", 1.53750647381045e-16 },
   { "MA0752", 5.5029875850101e-17 },
   { "MA0760", 6.80501360411106e-17 },
   { "MA0762", 4.11462295954211e-16 },
   { "MA0769", 1.25385560131581e-16 },
   { "MA0772", 1.73654475813481e-16 },
   { "MA0773", 1.14960180315083e-16 },
   { "MA0776", 6.91447736192669e-16 },
   { "MA0778", 2.0592519759347e-17 },
   { "MA0780", 5.68377326281212e-16 },
   { "MA0784", 5.31893374214433e-17 },
   { "MA0786", 1.95344287455169e-16 },
   { "MA0788", 1.77118715499427e-16 },
   { "MA0790", 5.04314819473507e-16 },
   { "MA0791", 2.10760852048033e-16 },
   { "MA0804", 2.55278304700499e-16 },
   { "MA0814", 5.74507208542706e-17 },
   { "MA0849", 9.512075519862281e-17 },
   { "MA0895", 1.89424933184357e-16 },
   { "MA0909", 3.96469326144629e-17 },
   { "MA0914", 4.44928312019159e-17 },
   { "MA0980", 6.34702763844688e-17 },
   { "MA1015", 1.47740185245346e-16 },
   { "MA1021", 8.02042864972286e-17 },
   { "MA1036", 2.11276162101851e-17 },
   { "MA1093", 1.37741215896616e-16 },
   { "MA1107", 1.29247336294479e-16 },
   { "MA1171", 1.30766320202629e-16 },
   { "MA1467", 9.53947556947391e-17 },
   { "", 0 }
};

/*
 * Asteroid photometric and physical parameters.
 *
 * Source:
 *
 *    https://asteroid.lowell.edu/main/astorb/
 *
 * Asteroid database updated as of 2022 December 7
 */
const PhysicalData AsteroidData::s_data[] =
{
   {  3.33,  0.15,  0.72, 848.4 }, //      1 Ceres
   {  4.12,  0.15,  0.66, 498.1 }, //      2 Pallas
   {  5.14,  0.15,  0.81, 233.9 }, //      3 Juno
   {  3.21,  0.15,  0.80, 468.3 }, //      4 Vesta
   {  7.01,  0.15,  0.83, 119.1 }, //      5 Astraea
   {  5.59,  0.15,  0.83, 185.2 }, //      6 Hebe
   {  5.62,  0.15,  0.85, 199.8 }, //      7 Iris
   {  6.60,  0.15,  0.89, 135.9 }, //      8 Flora
   {  6.32,  0.15,  0.86,  -1.0 }, //      9 Metis
   {  5.60,  0.15,  0.69, 407.1 }, //     10 Hygiea
   {  6.73,  0.15,  0.85, 153.3 }, //     11 Parthenope
   {  7.34,  0.15,  0.88, 112.8 }, //     12 Victoria
   {  6.95,  0.15,  0.75, 207.6 }, //     13 Egeria
   {  6.55,  0.15,  0.84,  -1.0 }, //     14 Irene
   {  5.38,  0.15,  0.84, 255.3 }, //     15 Eunomia
   {  6.14,  0.15,  0.70, 253.2 }, //     16 Psyche
   {  7.82,  0.15,  0.83,  90.0 }, //     17 Thetis
   {  6.57,  0.15,  0.85, 140.6 }, //     18 Melpomene
   {  7.42,  0.15,  0.75,  -1.0 }, //     19 Fortuna
   {  6.59,  0.15,  0.81, 145.5 }, //     20 Massalia
   {  7.52,  0.15,  0.70,  95.8 }, //     21 Lutetia
   {  6.64,  0.15,  0.69, 181.0 }, //     22 Kalliope
   {  7.17,  0.15,  0.85, 107.5 }, //     23 Thalia
   {  7.27,  0.15,  0.68,  -1.0 }, //     24 Themis
   {  7.88,  0.15,  0.94,  75.1 }, //     25 Phocaea
   {  7.58,  0.15,  0.88,  95.1 }, //     26 Proserpina
   {  7.07,  0.15,  0.87,  -1.0 }, //     27 Euterpe
   {  7.27,  0.15,  0.85, 120.9 }, //     28 Bellona
   {  5.97,  0.15,  0.83, 212.2 }, //     29 Amphitrite
   {  7.61,  0.15,  0.87,  99.7 }, //     30 Urania
   {  6.88,  0.15,  0.67, 255.9 }, //     31 Euphrosyne
   {  7.73,  0.15,  0.86,  80.8 }, //     32 Pomona
   {  8.69,  0.15,  0.70, 113.5 }, //     34 Circe
   {  8.72,  0.15,  0.70, 103.1 }, //     35 Leukothea
   {  8.61,  0.15,  0.73, 105.6 }, //     36 Atalante
   {  7.42,  0.15,  0.84, 108.3 }, //     37 Fides
   {  8.52,  0.15,  0.72, 115.9 }, //     38 Leda
   {  6.26,  0.15,  0.89, 149.5 }, //     39 Laetitia
   {  7.27,  0.15,  0.85, 107.6 }, //     40 Harmonia
   {  7.52,  0.15,  0.73, 174.0 }, //     41 Daphne
   {  7.62,  0.15,  0.88, 100.2 }, //     42 Isis
   {  8.00,  0.15,  0.87,  65.9 }, //     43 Ariadne
   {  7.00,  0.15,  0.70,  70.6 }, //     44 Nysa
   {  7.67,  0.15,  0.66, 214.6 }, //     45 Eugenia
   {  8.59,  0.15,  0.70, 124.1 }, //     46 Hestia
   {  8.19,  0.15,  0.66, 127.0 }, //     47 Aglaja
   {  7.19,  0.15,  0.72, 221.8 }, //     48 Doris
   {  7.92,  0.15,  0.75, 149.8 }, //     49 Pales
   {  9.38,  0.15,  0.71,  99.8 }, //     50 Virginia
   {  7.66,  0.15,  0.77, 147.9 }, //     51 Nemausa
   {  6.57,  0.15,  0.66, 302.5 }, //     52 Europa
   {  8.90,  0.15,  0.71, 115.4 }, //     53 Kalypso
   {  7.87,  0.15,  0.70, 165.8 }, //     54 Alexandra
   {  8.57,  0.15,  0.69, 113.2 }, //     56 Melete
   {  7.03,  0.15,  0.83, 112.6 }, //     57 Mnemosyne
   {  9.05,  0.15,  0.69,  93.4 }, //     58 Concordia
   {  8.07,  0.15,  0.67, 164.8 }, //     59 Elpis
   {  8.57,  0.15,  0.85,  60.2 }, //     60 Echo
   {  8.83,  0.15,  0.71,  95.4 }, //     62 Erato
   {  7.57,  0.15,  0.90, 103.1 }, //     63 Ausonia
   {  6.90,  0.15,  0.67, 237.3 }, //     65 Cybele
   {  7.07,  0.15,  0.84, 122.6 }, //     68 Leto
   {  7.25,  0.15,  0.70, 138.1 }, //     69 Hesperia
   {  8.20,  0.15,  0.74, 122.2 }, //     70 Panopaea
   {  7.25,  0.15,  0.82,  83.4 }, //     71 Niobe
   {  9.11,  0.15,  0.78,  86.1 }, //     72 Feronia
   {  8.83,  0.15,  0.69, 118.7 }, //     74 Galatea
   {  9.15,  0.15,  0.71,  55.7 }, //     75 Eurydike
   {  8.06,  0.15,  0.70, 183.7 }, //     76 Freia
   {  8.68,  0.15,  0.75,  69.2 }, //     77 Frigga
   {  8.35,  0.15,  0.71, 120.6 }, //     78 Diana
   {  7.97,  0.15,  0.88,  66.5 }, //     79 Eurynome
   {  8.14,  0.15,  0.90,  78.4 }, //     80 Sappho
   {  8.65,  0.15,  0.71, 119.1 }, //     81 Terpsichore
   {  8.30,  0.15,  0.81,  61.0 }, //     82 Alkmene
   {  8.78,  0.15,  0.70,  81.4 }, //     83 Beatrix
   {  9.39,  0.15,  0.73,  79.2 }, //     84 Klio
   {  7.84,  0.15,  0.66, 154.8 }, //     85 Io
   {  8.77,  0.15,  0.71, 120.6 }, //     86 Semele
   {  6.98,  0.15,  0.70, 260.9 }, //     87 Sylvia
   {  7.30,  0.15,  0.66, 200.6 }, //     88 Thisbe
   {  6.76,  0.15,  0.88, 151.5 }, //     89 Julia
   {  8.49,  0.15,  0.69, 120.1 }, //     90 Antiope
   {  8.95,  0.15,  0.73, 109.8 }, //     91 Aegina
   {  6.83,  0.15,  0.73, 126.4 }, //     92 Undina
   {  7.96,  0.15,  0.73, 141.0 }, //     93 Minerva
   {  7.69,  0.15,  0.66, 204.9 }, //     94 Aurora
   {  8.04,  0.15,  0.71, 136.0 }, //     95 Arethusa
   {  7.71,  0.15,  0.77, 169.9 }, //     96 Aegle
   {  7.85,  0.15,  0.70,  82.8 }, //     97 Klotho
   {  8.95,  0.15,  0.75, 104.5 }, //     98 Ianthe
   {  9.51,  0.15,  0.70,  71.9 }, //     99 Dike
   {  7.71,  0.15,  0.83,  88.7 }, //    100 Hekate
   {  9.41,  0.15,  0.72,  83.0 }, //    102 Miriam
   {  7.72,  0.15,  0.86,  91.2 }, //    103 Hera
   {  8.50,  0.15,  0.68, 123.7 }, //    104 Klymene
   {  8.75,  0.15,  0.70, 119.1 }, //    105 Artemis
   {  7.64,  0.15,  0.74, 146.6 }, //    106 Dione
   {  7.13,  0.15,  0.70, 222.6 }, //    107 Camilla
   {  9.04,  0.15,  0.73,  89.4 }, //    109 Felicitas
   {  7.89,  0.15,  0.71,  86.1 }, //    110 Lydia
   {  8.23,  0.15,  0.70, 134.6 }, //    111 Ate
   {  9.83,  0.15,  0.70,  72.2 }, //    112 Iphigenia
   {  8.62,  0.15,  0.94,  46.1 }, //    113 Amalthea
   {  8.36,  0.15,  0.76,  99.6 }, //    114 Kassandra
   {  7.69,  0.15,  0.86,  79.8 }, //    115 Thyra
   {  8.09,  0.15,  0.68, 148.7 }, //    117 Lomia
   {  8.90,  0.15,  0.86,  41.7 }, //    118 Peitho
   {  7.79,  0.15,  0.70, 174.1 }, //    120 Lachesis
   {  7.46,  0.15,  0.72, 209.0 }, //    121 Hermione
   {  8.14,  0.15,  0.85,  76.4 }, //    124 Alkeste
   {  8.58,  0.15,  0.70,  -1.0 }, //    127 Johanna
   {  7.77,  0.15,  0.68, 188.2 }, //    128 Nemesis
   {  7.04,  0.15,  0.72, 125.0 }, //    129 Antigone
   {  7.32,  0.15,  0.75, 182.3 }, //    130 Elektra
   {  8.95,  0.15,  0.68,  42.7 }, //    132 Aethra
   {  8.82,  0.15,  0.68, 123.3 }, //    134 Sophrosyne
   {  8.26,  0.15,  0.70,  79.2 }, //    135 Hertha
   {  8.23,  0.15,  0.70, 145.4 }, //    137 Meliboea
   {  8.05,  0.15,  0.70, 156.6 }, //    139 Juewa
   {  8.50,  0.15,  0.72, 109.8 }, //    140 Siwa
   {  8.46,  0.15,  0.66, 131.0 }, //    141 Lumen
   {  9.17,  0.15,  0.74,  89.9 }, //    143 Adria
   {  8.15,  0.15,  0.72, 141.8 }, //    144 Vibilia
   {  8.21,  0.15,  0.69, 151.1 }, //    145 Adeona
   {  8.42,  0.15,  0.68, 132.2 }, //    146 Lucina
   {  8.80,  0.15,  0.69, 132.9 }, //    147 Protogeneia
   {  7.65,  0.15,  0.86,  97.7 }, //    148 Gallia
   {  8.53,  0.15,  0.71, 151.1 }, //    150 Nuwa
   {  7.71,  0.15, -1.00, 184.9 }, //    154 Bertha
   {  8.78,  0.15,  0.70, 121.0 }, //    156 Xanthippe
   {  8.36,  0.15,  0.69, 125.0 }, //    159 Aemilia
   {  9.15,  0.15,  0.72,  81.2 }, //    160 Una
   {  8.97,  0.15,  0.76,  99.1 }, //    162 Laurentia
   {  9.76,  0.15,  0.70,  72.6 }, //    163 Erigone
   {  8.80,  0.15,  0.68, 104.9 }, //    164 Eva
   {  7.80,  0.15,  0.74, 155.2 }, //    165 Loreley
   {  8.12,  0.15,  0.75, 148.4 }, //    168 Sibylla
   {  8.65,  0.15,  0.69, 116.7 }, //    171 Ophelia
   {  8.76,  0.15,  0.90,  62.4 }, //    172 Baucis
   {  8.00,  0.15,  0.70, 154.1 }, //    173 Ino
   {  8.61,  0.15,  0.70, 100.9 }, //    175 Andromache
   {  8.34,  0.15, -1.00, 121.0 }, //    176 Iduna
   {  9.72,  0.15,  0.73,  73.2 }, //    177 Irma
   {  7.96,  0.15,  0.80, 106.0 }, //    181 Eucharis
   {  7.71,  0.15,  0.68, 157.5 }, //    185 Eunike
   {  8.30,  0.15,  0.71, 131.2 }, //    187 Lamberta
   {  9.03,  0.15,  0.68, 101.0 }, //    191 Kolga
   {  7.38,  0.15,  0.94, 103.3 }, //    192 Nausikaa
   {  7.88,  0.15,  0.73, 168.4 }, //    194 Prokne
   {  9.18,  0.15,  0.69,  85.7 }, //    195 Eurykleia
   {  6.64,  0.15,  0.86, 136.3 }, //    196 Philomela
   {  8.53,  0.15,  0.88,  57.2 }, //    198 Ampella
   {  8.46,  0.15,  0.71, 128.4 }, //    200 Dynamene
   {  8.49,  0.15,  0.71,  68.4 }, //    201 Penelope
   {  8.89,  0.15,  0.70, 116.3 }, //    203 Pompeja
   {  9.49,  0.15,  0.69,  80.9 }, //    205 Martha
   {  8.84,  0.15,  0.70,  -1.0 }, //    206 Hersilia
   {  8.29,  0.15,  0.69, 159.9 }, //    209 Dido
   {  9.42,  0.15,  0.67,  86.7 }, //    210 Isabella
   {  8.03,  0.15,  0.72, 143.2 }, //    211 Isolda
   {  8.42,  0.15,  0.70, 136.1 }, //    212 Medea
   {  9.01,  0.15,  0.64,  83.0 }, //    213 Lilaea
   {  7.08,  0.15,  0.70, 135.1 }, //    216 Kleopatra
   {  7.81,  0.15,  0.79, 103.9 }, //    221 Eos
   {  9.90,  0.15,  0.71,  87.6 }, //    223 Rosa
   {  8.69,  0.15,  0.75,  61.8 }, //    224 Oceana
   {  8.86,  0.15,  0.66, 120.5 }, //    225 Henrietta
   {  9.18,  0.15, -1.00,  87.3 }, //    227 Philosophia
   {  7.55,  0.15,  0.85, 109.0 }, //    230 Athamantis
   {  8.46,  0.15,  0.78, 102.8 }, //    233 Asterope
   {  8.29,  0.15,  0.85,  86.2 }, //    236 Honoria
   {  8.11,  0.15,  0.73, 148.5 }, //    238 Hypatia
   {  9.20,  0.15,  0.70, 103.9 }, //    240 Vanadis
   {  7.82,  0.15,  0.69, 168.9 }, //    241 Germania
   {  8.33,  0.15,  0.69, 134.4 }, //    247 Eukrate
   {  7.58,  0.15,  0.72,  79.8 }, //    250 Bettina
   {  7.90,  0.15,  0.67, 178.6 }, //    259 Aletheia
   {  8.81,  0.15,  0.71, 109.1 }, //    266 Aline
   {  8.51,  0.15,  0.64, 139.9 }, //    268 Adorea
   {  8.95,  0.15,  0.72, 121.0 }, //    275 Sapientia
   {  8.63,  0.15,  0.71, 121.6 }, //    276 Adelheid
   {  8.70,  0.15,  0.71, 148.1 }, //    283 Emma
   {  8.34,  0.15,  0.87,  67.6 }, //    287 Nephthys
   {  8.96,  0.15, -1.00,  99.3 }, //    303 Josephina
   {  9.90,  0.15,  0.71,  67.9 }, //    304 Olga
   {  8.17,  0.15,  0.79, 140.7 }, //    308 Polyxo
   {  9.09,  0.15,  0.71,  96.3 }, //    313 Chaldaea
   {  9.12,  0.15,  0.72,  70.8 }, //    322 Phaeo
   {  7.11,  0.15,  0.70, 229.4 }, //    324 Bamberga
   {  9.34,  0.15,  0.71,  93.0 }, //    326 Tamara
   {  8.86,  0.15,  0.89, 122.9 }, //    328 Gudrun
   {  9.66,  0.15,  0.69,  77.8 }, //    329 Svea
   {  7.73,  0.15,  0.72, 155.8 }, //    334 Chicago
   {  9.11,  0.15,  0.62,  89.1 }, //    335 Roberta
   {  9.86,  0.15,  0.73,  69.3 }, //    336 Lacadiera
   {  8.82,  0.15,  0.69,  59.1 }, //    337 Devosa
   {  8.60,  0.15,  0.70,  63.1 }, //    338 Budrosa
   {  8.36,  0.15,  0.71, 132.2 }, //    344 Desiderata
   {  8.98,  0.15,  0.72,  94.1 }, //    345 Tercidina
   {  7.42,  0.15,  0.85, 106.5 }, //    346 Hermentaria
   {  9.08,  0.15,  0.68,  51.3 }, //    347 Pariana
   {  6.03,  0.15,  0.93, 139.8 }, //    349 Dembowska
   {  8.44,  0.15,  0.69, 118.3 }, //    350 Ornamenta
   {  6.46,  0.15,  0.95, 155.2 }, //    354 Eleonora
   {  8.43,  0.15,  0.73, 131.3 }, //    356 Liguria
   {  8.90,  0.15,  0.72, 106.1 }, //    357 Ninina
   {  9.20,  0.15, -1.00,  89.5 }, //    358 Apollonia
   {  8.57,  0.15,  0.68, 115.8 }, //    360 Carlova
   {  9.18,  0.15,  0.71,  -1.0 }, //    362 Havnia
   {  9.08,  0.15,  0.75,  -1.0 }, //    363 Padua
   {  9.26,  0.15,  0.72, 105.9 }, //    365 Corduba
   {  8.71,  0.15, -1.00,  93.8 }, //    366 Vincentina
   {  8.70,  0.15,  0.71,  60.0 }, //    369 Aeria
   {  7.41,  0.15, -1.00, 188.6 }, //    372 Palma
   {  9.17,  0.15,  0.67,  95.8 }, //    373 Melusina
   {  7.51,  0.15,  0.68,  -1.0 }, //    375 Ursula
   {  9.10,  0.15,  0.75,  91.0 }, //    377 Campania
   {  8.44,  0.15,  0.67, 120.6 }, //    381 Myrrha
   {  7.61,  0.15,  0.90,  91.5 }, //    385 Ilmatar
   {  7.74,  0.15,  0.74, 165.0 }, //    386 Siegena
   {  7.59,  0.15,  0.88, 100.5 }, //    387 Aquitania
   {  8.71,  0.15,  0.72, 114.2 }, //    388 Charybdis
   {  7.93,  0.15,  0.86,  79.0 }, //    389 Industria
   {  8.44,  0.15,  0.74,  96.9 }, //    393 Lampetia
   {  9.17,  0.15,  0.66,  97.7 }, //    404 Arsinoe
   {  8.66,  0.15,  0.69, 124.9 }, //    405 Thia
   {  9.13,  0.15,  0.70,  95.1 }, //    407 Arachne
   {  7.70,  0.15,  0.72, 161.6 }, //    409 Aspasia
   {  8.38,  0.15,  0.75, 123.5 }, //    410 Chloris
   {  9.20,  0.15, -1.00,  91.0 }, //    412 Elisabetha
   {  9.39,  0.15,  0.71,  76.3 }, //    415 Palatia
   {  7.82,  0.15,  0.88,  85.5 }, //    416 Vaticana
   {  8.63,  0.15,  0.64, 129.0 }, //    419 Aurelia
   {  8.38,  0.15,  0.69, 141.2 }, //    420 Bertholda
   {  7.36,  0.15,  0.67, 208.8 }, //    423 Diotima
   {  9.63,  0.15, -1.00,  87.2 }, //    424 Gratia
   {  8.59,  0.15,  0.71, 127.1 }, //    426 Hippo
   {  9.02,  0.15,  0.64,  95.0 }, //    431 Nephele
   {  8.94,  0.15,  0.87,  46.9 }, //    432 Pythia
   { 10.31,  0.15, -1.00,  -1.0 }, //    433 Eros
   { 10.00,  0.15,  0.69,  65.7 }, //    442 Eichsfeldia
   {  8.07,  0.15,  0.68, 159.6 }, //    444 Gyptis
   {  9.22,  0.15,  0.68,  87.2 }, //    445 Edna
   {  9.81,  0.15,  0.70,  85.6 }, //    449 Hamburga
   {  6.85,  0.15,  0.65, 225.0 }, //    451 Patientia
   {  9.34,  0.15,  0.66,  81.6 }, //    454 Mathesis
   {  8.94,  0.15,  0.70,  84.4 }, //    455 Bruchsalia
   {  9.79,  0.15,  0.64,  74.0 }, //    464 Megaira
   {  9.91,  0.15, -1.00,  73.3 }, //    465 Alekto
   {  8.49,  0.15,  0.66, 115.5 }, //    466 Tisiphone
   {  8.73,  0.15,  0.65, 125.6 }, //    469 Argentina
   {  6.74,  0.15,  0.83, 134.2 }, //    471 Papagena
   {  8.65,  0.15,  0.71, 116.8 }, //    476 Hedwig
   {  8.81,  0.15,  0.70, 116.0 }, //    481 Emita
   {  8.29,  0.15, -1.00,  63.9 }, //    485 Genua
   {  7.92,  0.15,  0.70, 150.1 }, //    488 Kreusa
   {  8.51,  0.15,  0.69, 139.4 }, //    489 Comacina
   {  8.60,  0.15,  0.75, 115.5 }, //    490 Veritas
   {  9.07,  0.15, -1.00,  97.3 }, //    491 Carina
   {  8.91,  0.15,  0.75,  82.8 }, //    498 Tokio
   {  9.35,  0.15,  0.72,  81.7 }, //    503 Evelyn
   {  8.80,  0.15,  0.66,  -1.0 }, //    505 Cava
   {  8.92,  0.15,  0.71, 105.9 }, //    506 Marion
   {  8.42,  0.15,  0.73, 142.3 }, //    508 Princetonia
   {  6.43,  0.15,  0.72, 326.1 }, //    511 Davida
   {  9.23,  0.15,  0.65, 106.2 }, //    514 Armida
   {  8.29,  0.15,  0.74,  73.1 }, //    516 Amherstia
   {  9.61,  0.15,  0.71,  91.1 }, //    517 Edith
   {  8.58,  0.15,  0.71, 115.7 }, //    521 Brixia
   {  5.90,  0.15,  0.85, 222.2 }, //    532 Herculina
   {  9.45,  0.15,  0.74,  74.5 }, //    535 Montague
   {  8.14,  0.15,  0.69, 151.4 }, //    536 Merapi
   {  8.85,  0.15,  0.69, 111.3 }, //    545 Messalina
   {  9.85,  0.15,  0.76,  69.7 }, //    547 Praxedis
   {  9.12,  0.15,  0.66,  95.9 }, //    554 Peraga
   {  8.17,  0.15,  0.70, 168.2 }, //    566 Stereoskopia
   {  9.50,  0.15, -1.00,  87.0 }, //    568 Cheruskia
   { 10.09,  0.15,  0.74,  73.0 }, //    569 Misa
   {  8.55,  0.15,  0.89,  54.0 }, //    584 Semiramis
   { 10.48,  0.15,  0.70,  58.1 }, //    585 Bilkis
   { 10.69,  0.15,  0.69,  51.9 }, //    591 Irmgard
   {  9.43,  0.15,  0.65,  75.3 }, //    593 Titania
   {  7.95,  0.15, -1.00, 109.1 }, //    595 Polyxena
   {  8.97,  0.15,  0.72, 113.3 }, //    596 Scheila
   {  9.68,  0.15,  0.74,  72.3 }, //    598 Octavia
   {  8.80,  0.15,  0.88,  64.9 }, //    599 Luisa
   {  8.52,  0.15,  0.70, 124.7 }, //    602 Marianna
   {  9.52,  0.15, -1.00,  65.2 }, //    604 Tekmessa
   {  8.37,  0.15,  0.70, 120.3 }, //    618 Elfriede
   { 10.81,  0.15,  0.71,  44.1 }, //    623 Chimaera
   {  9.19,  0.15,  0.70, 100.7 }, //    626 Notburga
   {  9.17,  0.15,  0.68,  98.2 }, //    635 Vundtia
   {  8.66,  0.15,  0.68, 127.4 }, //    654 Zelinda
   {  9.21,  0.15,  0.68, 100.9 }, //    663 Gerlinde
   {  9.38,  0.15, -1.00,  81.3 }, //    667 Denise
   {  7.48,  0.15,  0.88,  97.3 }, //    674 Rachele
   {  8.13,  0.15,  0.85,  -1.0 }, //    675 Ludmilla
   {  9.50,  0.15,  0.69,  83.9 }, //    680 Genoveva
   {  8.74,  0.15, -1.00,  82.0 }, //    683 Lanzia
   {  8.07,  0.15,  0.66, 135.0 }, //    690 Wratislavia
   {  9.30,  0.15,  0.74,  87.7 }, //    691 Lehigh
   {  9.20,  0.15,  0.72,  90.8 }, //    694 Ekard
   {  9.52,  0.15, -1.00,  75.8 }, //    696 Leonora
   {  7.43,  0.15,  0.66, 194.7 }, //    702 Alauda
   {  6.29,  0.15,  0.64, 316.6 }, //    704 Interamnia
   {  8.44,  0.15,  0.70, 134.2 }, //    705 Erminia
   {  9.13,  0.15,  0.72,  96.6 }, //    709 Fringilla
   {  8.67,  0.15,  0.73, 127.6 }, //    712 Boliviana
   {  9.02,  0.15,  0.66, 105.5 }, //    713 Luscinia
   {  9.84,  0.15,  0.70,  74.3 }, //    735 Marghanna
   {  8.72,  0.15,  0.71, 107.4 }, //    739 Mandeville
   {  9.16,  0.15,  0.72,  90.9 }, //    740 Cantabia
   {  7.88,  0.15,  0.71, 171.7 }, //    747 Winchester
   {  8.75,  0.15,  0.68, 110.5 }, //    751 Faina
   { 10.35,  0.15, -1.00,  62.8 }, //    752 Sulamitis
   {  7.95,  0.15,  0.95,  71.3 }, //    760 Massinga
   {  8.28,  0.15,  0.65, 137.1 }, //    762 Pulcova
   {  9.07,  0.15, -1.00, 106.4 }, //    769 Tatjana
   {  8.39,  0.15,  0.67, 117.7 }, //    772 Tanete
   {  9.13,  0.15,  0.70,  95.9 }, //    773 Irmintraud
   {  7.76,  0.15,  0.70, 151.2 }, //    776 Berbericia
   {  9.79,  0.15,  0.62,  64.1 }, //    778 Theobalda
   {  9.05,  0.15, -1.00,  94.4 }, //    780 Armenia
   {  9.34,  0.15, -1.00,  89.4 }, //    784 Pickeringia
   {  8.98,  0.15,  0.69,  91.6 }, //    786 Bredichina
   {  8.73,  0.15, -1.00, 103.7 }, //    788 Hohensteina
   {  8.10,  0.15,  0.70, 170.4 }, //    790 Pretoria
   {  9.23,  0.15,  0.71, 103.5 }, //    791 Ani
   {  8.00,  0.15,  0.71, 157.2 }, //    804 Hispania
   {  8.80,  0.15,  0.68, 109.5 }, //    814 Tauris
   {  8.21,  0.15,  0.70,  61.8 }, //    849 Ara
   {  8.38,  0.15, -1.00, 141.9 }, //    895 Helio
   {  8.78,  0.15,  0.69, 116.4 }, //    909 Ulla
   {  9.13,  0.15,  0.74,  76.6 }, //    914 Palisana
   {  7.96,  0.15,  0.91,  86.2 }, //    980 Anacostia
   {  9.25,  0.15,  0.69,  96.9 }, //   1015 Christa
   {  9.09,  0.15,  0.66,  99.4 }, //   1021 Flammario
   {  9.21,  0.15,  0.84,  31.7 }, //   1036 Ganymed
   {  8.98,  0.15,  0.68, 116.7 }, //   1093 Freda
   {  9.57,  0.15, -1.00,  79.2 }, //   1107 Lictoria
   {  9.92,  0.15,  0.69,  70.1 }, //   1171 Rusthawelia
   {  8.72,  0.15,  0.72, 112.0 }  //   1467 Mashona
};

// ----------------------------------------------------------------------------

struct KBOData : public BodyData
{
   int NumberOfObjects() const override
   {
      return 30;
   }

   EphemerisConstantList Constants() const
   {
      int n = 0;
      while( !s_constants[n].name.IsEmpty() )
         ++n;
      return EphemerisConstantList( s_constants, s_constants+n );
   }

   int BSPObjectIdentifier( int index ) const override
   {
      return s_bspIds[index];
   }

   int ObjectNumber( int index ) const override
   {
      int n = s_bspIds[index];
      if ( n < 3000000 )
         return n - 2000000;
      if ( n == 3308265 )
         return 612911;
      if ( n == 3545742 )
         return 589683;
      return 0;
   }

   const char* ObjectName( int index ) const override
   {
      return s_names[index];
   }

   const PhysicalData& ObjectData( int index ) const override
   {
      return s_data[index];
   }

private:

   static const int s_bspIds[];
   static const char* s_names[];
   static const EphemerisConstant s_constants[];
   static const PhysicalData s_data[];
};

/*
 * SPICE body identifiers present in sb441-n373.bsp
 */
const int KBOData::s_bspIds[] =
{
   2019521, 2020000, 2028978, 2042301, 2050000, 2055565, 2055637, 2084522,
   2090377, 2090482, 2090568, 2120347, 2136108, 2136199, 2136472, 2145452,
   2174567, 2208996, 2225088, 2230965, 2278361, 2307261, 2455502, 2523639,
   2528381, 3308265, 3361580, 3515022, 3525142, 3545742
};

/*
 * Asteroid designation names.
 *
 * Source:
 *
 *    https://asteroid.lowell.edu/main/astorb/
 */
const char* KBOData::s_names[] =
{
   "Chaos",             //  19521 Chaos
   "Varuna",            //  20000 Varuna
   "Ixion",             //  28978 Ixion
   "2001 UR163",        //  42301 2001 UR163
   "Quaoar",            //  50000 Quaoar
   "2002 AW197",        //  55565 2002 AW197
   "2002 UX25",         //  55637 2002 UX25
   "2002 TC302",        //  84522 2002 TC302
   "Sedna",             //  90377 Sedna
   "Orcus",             //  90482 Orcus
   "2004 GV9",          //  90568 2004 GV9
   "Salacia",           // 120347 Salacia
   "Haumea",            // 136108 Haumea
   "Eris",              // 136199 Eris
   "Makemake",          // 136472 Makemake
   "2005 RN43",         // 145452 2005 RN43
   "Varda",             // 174567 Varda
   "2003 AZ84",         // 208996 2003 AZ84
   "Gonggong",          // 225088 Gonggong
   "2004 XA192",        // 230965 2004 XA192
   "2007 JJ43",         // 278361 2007 JJ43
   "2002 MS4",          // 307261 2002 MS4
   "2003 UZ413",        // 455502 2003 UZ413
   "2010 RE64",         // 523639 2010 RE64
   "2008 ST291",        // 528381 2008 ST291
   "2004 XR190",        // 612911 2004 XR190    SPICE id = 3308265
   "2006 QH181",        // SPICE id = 3361580
   "2010 FX86",         // SPICE id = 3515022
   "2010 KZ39",         // SPICE id = 3525142
   "2010 RF43"          // 589683 2010 RF43     SPICE id = 3545742
};

/*
 * KBO constants (DE440)
 */
const EphemerisConstant KBOData::s_constants[] =
{
   { "DENUM", 441 },
   { "LENUM", 441 },
   { "MA8020", 2.53837964562973e-14 },
   { "MA8008", 5.49507522341186e-14 },
   { "MA8010", 4.51766562250976e-14 },
   { "MA8025", 2.72519389203377e-14 },
   { "MA8005", 1.52948774597511e-13 },
   { "MA8015", 6.02473242459976e-14 },
   { "MA8009", 1.86035124454487e-14 },
   { "MA8006", 2.28199634312045e-13 },
   { "MA8003", 1.49348678842449e-13 },
   { "MA8007", 9.409624643156111e-14 },
   { "MA8017", 3.71935023738244e-14 },
   { "MA8021", 6.51170207936313e-14 },
   { "MA8002", 5.96163005450428e-13 },
   { "MA8001", 2.48544803028732e-12 },
   { "MA8004", 3.43185550062921e-13 },
   { "MA8016", 3.62854799177896e-14 },
   { "MA8012", 3.96577688696419e-14 },
   { "MA8018", 6.08212352839477e-14 },
   { "MA8019", 2.58567882770904e-13 },
   { "MA8024", 3.64502260200911e-15 },
   { "MA8022", 2.67691589753377e-14 },
   { "MA8011", 7.6535017490672e-14 },
   { "MA8026", 6.48385807558172e-14 },
   { "MA8028", 1.91288669686944e-14 },
   { "MA8029", 2.13389035100458e-14 },
   { "MA8014", 2.90409951818848e-14 },
   { "MA8013", 6.99160258973788e-14 },
   { "MA8030", 2.57103425708102e-14 },
   { "MA8023", 2.95798751569601e-14 },
   { "MA8027", 1.75614259349073e-14 },
   { "", 0 }
};

/*
 * Asteroid photometric and physical parameters.
 *
 * Source:
 *
 *    https://asteroid.lowell.edu/main/astorb/
 *
 * Asteroid database updated as of 2022 December 7
 */
const PhysicalData KBOData::s_data[] =
{
   {  4.63,  0.15, -1.00,  -1.0 }, //  19521 Chaos
   {  3.79,  0.15, -1.00,  -1.0 }, //  20000 Varuna
   {  3.47,  0.15, -1.00,  -1.0 }, //  28978 Ixion
   {  4.17,  0.15, -1.00,  -1.0 }, //  42301 2001 UR163
   {  2.42,  0.15, -1.00,  -1.0 }, //  50000 Quaoar
   {  3.47,  0.15, -1.00,  -1.0 }, //  55565 2002 AW197
   {  3.85,  0.15, -1.00,  -1.0 }, //  55637 2002 UX25
   {  3.93,  0.15, -1.00,  -1.0 }, //  84522 2002 TC302
   {  1.53,  0.15, -1.00,  -1.0 }, //  90377 Sedna
   {  2.19,  0.15, -1.00,  -1.0 }, //  90482 Orcus
   {  4.03,  0.15, -1.00,  -1.0 }, //  90568 2004 GV9
   {  4.28,  0.15, -1.00,  -1.0 }, // 120347 Salacia
   {  0.23,  0.15, -1.00,  -1.0 }, // 136108 Haumea
   { -1.21,  0.15, -1.00,  -1.0 }, // 136199 Eris
   { -0.20,  0.15, -1.00,  -1.0 }, // 136472 Makemake
   {  3.70,  0.15, -1.00,  -1.0 }, // 145452 2005 RN43
   {  3.46,  0.15, -1.00,  -1.0 }, // 174567 Varda
   {  3.77,  0.15, -1.00,  -1.0 }, // 208996 2003 AZ84
   {  1.86,  0.15, -1.00,  -1.0 }, // 225088 Gonggong
   {  4.26,  0.15, -1.00,  -1.0 }, // 230965 2004 XA192
   {  4.49,  0.15, -1.00,  -1.0 }, // 278361 2007 JJ43
   {  3.62,  0.15, -1.00,  -1.0 }, // 307261 2002 MS4
   {  4.33,  0.15, -1.00,  -1.0 }, // 455502 2003 UZ413
   {  4.29,  0.15, -1.00,  -1.0 }, // 523639 2010 RE64
   {  4.29,  0.15, -1.00,  -1.0 }, // 528381 2008 ST291
   {  4.41,  0.15, -1.00,  -1.0 }, // 612911 2004 XR190
   {  4.30,  0.15, -1.00,  -1.0 }, //        2006 QH181
   {  4.55,  0.15, -1.00,  -1.0 }, //        2010 FX86
   {  3.99,  0.15, -1.00,  -1.0 }, //        2010 KZ39
   {  3.87,  0.15, -1.00,  -1.0 }  // 589683 2010 RF43
};

// ----------------------------------------------------------------------------

File logFile;

void LogLn( const IsoString& text = IsoString(), bool showOnCout = true )
{
   if ( logFile.IsOpen() )
   {
      logFile.OutTextLn( text );
      logFile.Flush();
   }

   if ( showOnCout )
      std::cout << text << '\n';
}

void LogLn( const char* text, bool showOnCout = true )
{
   if ( logFile.IsOpen() )
   {
      logFile.OutTextLn( text );
      logFile.Flush();
   }

   if ( showOnCout )
      std::cout << text << '\n';
}

void LogLn( const String& text, bool showOnCout = true )
{
   IsoString text8 = text.ToUTF8();

   if ( logFile.IsOpen() )
   {
      logFile.OutTextLn( text8 );
      logFile.Flush();
   }

   if ( showOnCout )
      std::cout << text8 << '\n';
}

// ----------------------------------------------------------------------------

void MakeObjectData( SerializableEphemerisObjectData& object,
                     int startJDI, int endJDI, const BodyData& data, int index, int order = 0 )
{
   IsoString objectNumberAndName =
      (data.IsNumberedObject( index ) ? IsoString( data.ObjectNumber( index ) ) + ' ' : IsoString())
     + data.ObjectName( index );

   LogLn( SEPARATOR );
   LogLn( "* Generating ephemeris data for " + objectNumberAndName );
   LogLn();

//   const double truncation_km = 5.0e-03; // 5 m
   const double truncation_km = 7.5e-03; // 7.5 m

   // Maximum allowed expansion length
   const int maxLength = 120;

   int delta = 400;
   int minDelta = int32_max;
   int maxDelta = 0;

   double maxError[ 3 ] = { 0, 0, 0 };
   int totalCoefficients = 0;

   for ( int jdi1 = startJDI, count = 0; jdi1 < endJDI; ++count )
   {
      int jdi2;
      int length = 25;
      double epsilon = 0.65*truncation_km/au_km;
      ChebyshevFit T;

      for ( bool reduce = false, truncated = false;; )
      {
         jdi2 = jdi1 + delta;
         if ( jdi2 > endJDI )
         {
            jdi2 = endJDI;
            delta = jdi2 - jdi1;
         }

         T = ChebyshevFit( [jdi1, &data, index, order]( double dt )
                           {
                              TimePoint t( jdi1, 0.5+dt );
                              double et = t.SecondsSinceJ2000();
                              double lt;
                              double state[ 6 ];
                              spkez_c( data.BSPObjectIdentifier( index ), et, "J2000", "NONE"/*abcorr*/, 0/*observer=SSB*/, state, &lt );
                              if ( order == 0 )
                                 return Vector( state[0]/au_km,
                                                state[1]/au_km,
                                                state[2]/au_km );
                              return Vector( state[3]/au_km*86400,
                                             state[4]/au_km*86400,
                                             state[5]/au_km*86400 );
                           },
                           0, delta, 3, 2*length );

         T.Truncate( epsilon );
         if ( T.TruncatedLength() > length )
         {
            --delta;
            reduce = true;
         }
         else if ( !reduce && jdi2 < endJDI )
            ++delta;
         else
         {
            if ( truncated )
               LogLn( String().Format( "** Warning: Increased truncation error to %.3e", epsilon ) );
            break;
         }

         if ( delta == 0 )
         {
            // Fast movement: try with longer coefficient series.
            length += 10;
            if ( length > maxLength )
            {
               // Extremely fast movement: try a larger truncation error.
               // N.B. This should not happen under normal working conditions
               // with core asteroid ephemerides.
               length = 50;
               epsilon *= 2;
               truncated = true;
            }

            delta = 12;
            reduce = false;
         }
      }

      if ( T.TruncationError() > epsilon )
         throw Error( String().Format( "Internal error: %d -> %d (%d) : nx=%d ny=%d nz=%d e=%.3e",
                        jdi1, jdi2, delta,
                        T.TruncatedLength( 0 ), T.TruncatedLength( 1 ), T.TruncatedLength( 2 ), T.TruncationError() ) );

      object.data[order] << SerializableEphemerisData( TimePoint( jdi1, 0.5 ), T );

      LogLn( String().Format( "%5d : %+10.1f -> %+10.1f (%5d) : %3d %3d %3d %.3e %.3e %.3e"
                              , count, jdi1+0.5, jdi2+0.5, delta
                              , T.TruncatedLength( 0 ), T.TruncatedLength( 1 ), T.TruncatedLength( 2 )
                              , T.TruncationError( 0 ), T.TruncationError( 1 ), T.TruncationError( 2 ) ) );

      for ( int i = 0; i < 3; ++i )
         if ( T.TruncationError( i ) > maxError[i] )
            maxError[i] = T.TruncationError( i );

      totalCoefficients += T.NumberOfTruncatedCoefficients();

      if ( delta < minDelta )
         if ( jdi2 < endJDI || count == 0 )
            minDelta = delta;
      if ( delta > maxDelta )
         maxDelta = delta;

      jdi1 = jdi2;
   }

   LogLn();
   LogLn( "Object ....................... " + objectNumberAndName );
   LogLn( "Derivative order ............. " + String( order ) );
   LogLn( "Total Chebyshev expansions ... " + String( object.data[order].Length() ) );
   LogLn( "Smallest time span ........... " + String( minDelta ) + " (days)" );
   LogLn( "Largest time span ............ " + String( maxDelta ) + " (days)" );
   LogLn( "Largest truncation errors .... " + String().Format( "%.3e  %.3e  %.3e  (%s)"
                                                         , maxError[0], maxError[1], maxError[2], order ? "au/day" : "au" ) );
   LogLn( "Total coefficients ........... " + String( totalCoefficients ) );
   LogLn();
}

// ----------------------------------------------------------------------------

SerializableEphemerisObjectData MakeObject( int startJDI, int endJDI, const BodyData& data, int index, bool withDerivative = false )
{
   SerializableEphemerisObjectData object( data.IsNumberedObject( index ) ? IsoString( data.ObjectNumber( index ) ) : data.ObjectName( index ),
                                           "SSB",
                                           data.IsNumberedObject( index ) ? data.ObjectName( index ) : IsoString() );

   const PhysicalData& phys = data.ObjectData( index );
   object.H = phys.H;
   object.G = phys.G;
   if ( phys.B_V >= 0 )
      object.B_V = phys.B_V;
   if ( phys.D >= 0 )
      object.D = phys.D;

   MakeObjectData( object, startJDI, endJDI, data, index, 0/*order: position*/ );
   if ( withDerivative )
      MakeObjectData( object, startJDI, endJDI, data, index, 1/*order: velocity*/ );

   return object;
}

// ----------------------------------------------------------------------------

void TestEphemerisFile( const String filePath, int testsPerDay, const BodyData& data, const Array<int>& index )
{
   LogLn( SEPARATOR );
   LogLn( "* Testing ephemeris file: " );
   LogLn( filePath );
   LogLn();

   EphemerisFile E( filePath );

   TimePoint t0 = E.StartTime();
   TimePoint t1 = E.EndTime();
   double ts = t1 - t0;

   XorShift1024 R;

   for ( int i = 0; i < int( index.Length() ); ++i )
   {
      IsoString objectNumberAndName =
         (data.IsNumberedObject( index[i] ) ? IsoString( data.ObjectNumber( index[i] ) ) + ' ' : IsoString())
        + data.ObjectName( index[i] );

      IsoString objectId = data.IsNumberedObject( index[i] ) ?
         IsoString( data.ObjectNumber( index[i] ) ) : IsoString( data.ObjectName( index[i] ) );

      LogLn( objectNumberAndName );

      EphemerisFile::Handle H( E, objectId, "SSB" );

      Vector er( 0.0, 3 );
      Vector er1( 3 );
      Vector er2( 3 );
      Vector ev( 0.0, 3 );
      Vector ev1( 3 );
      Vector ev2( 3 );
      Array<TimePoint> ert( 3 );
      Array<TimePoint> evt( 3 );

      for ( int j = 0, N = testsPerDay*int( t1-t0 ); j < N; ++j )
      {
         TimePoint t( t0 + R()*ts );

         MultiVector s = H.StateVectors( t );

         double et = t.SecondsSinceJ2000();
         double lt;
         double state[ 6 ];
         spkez_c( data.BSPObjectIdentifier( index[i] ), et, "J2000", "NONE"/*abcorr*/, 0/*observer=SSB*/, state, &lt );
         for ( int k = 0; k < 3; ++k )
            state[k] = state[k]/au_km;
         for ( int k = 3; k < 6; ++k )
            state[k] = state[k]/au_km*86400;

         for ( int k = 0; k < 3; ++k )
         {
            double dr = Abs( s[0][k] - state[k] );
            if ( dr > er[k] )
            {
               er[k] = dr;
               er1[k] = state[k];
               er2[k] = s[0][k];
               ert[k] = t;
            }
            double dv = Abs( s[1][k] - state[k+3] );
            if ( dv > ev[k] )
            {
               ev[k] = dv;
               ev1[k] = state[k+3];
               ev2[k] = s[1][k];
               evt[k] = t;
            }
         }
      }

      LogLn( "Position errors    : " + IsoString().Format( "%.3e  %.3e  %.3e", er[0], er[1], er[2] ) );
      LogLn( IsoString().Format( "JPL: %+.12e  %+.12e  %+.12e", er1[0], er1[1], er1[2] ) );
      LogLn( IsoString().Format( "EPH: %+.12e  %+.12e  %+.12e", er2[0], er2[1], er2[2] ) );
      LogLn( ert[0].ToIsoString() + "  " + ert[1].ToIsoString() + "  " + ert[2].ToIsoString() );
      LogLn( "Velocity errors    : " + IsoString().Format( "%.3e  %.3e  %.3e", ev[0], ev[1], ev[2] ) );
      LogLn( IsoString().Format( "JPL: %+.12e  %+.12e  %+.12e", ev1[0], ev1[1], ev1[2] ) );
      LogLn( IsoString().Format( "EPH: %+.12e  %+.12e  %+.12e", ev2[0], ev2[1], ev2[2] ) );
      LogLn( evt[0].ToIsoString() + "  " + evt[1].ToIsoString() + "  " + evt[2].ToIsoString() );
      LogLn();
   }
}

// ----------------------------------------------------------------------------

void SayHello()
{
   std::cout <<
   "\nPixInsight JPL Asteroid Ephemerides to XEPH Format Conversion Utility - " PROGRAM_NAME " version " PROGRAM_VERSION
   "\nCopyright (c) 2017-" PROGRAM_YEAR " Pleiades Astrophoto"
   "\n";
}

// ----------------------------------------------------------------------------

void ShowHelp()
{
   std::cout <<
"\nMandatory arguments:"
"\n"
"\n   -d=<file> | --data=<file>"
"\n"
"\n      <file> is the path to the input SPICE binary kernel file."
"\n"
"\n   -f=<file> | --fundamental=<file>"
"\n"
"\n      <file> is the path to a SPICE binary kernel file providing fundamental"
"\n      solar system ephemerides."
"\n"
"\nOptional arguments:"
"\n"
"\n   -K | --kbo"
"\n"
"\n      Generate and/or test a KBO ephemeris file instead of an asteroid"
"\n      ephemeris file."
"\n"
"\n   -o=<dir> | --output-dir=<dir>"
"\n"
"\n      <dir> is the path to a directory where all output files will be created."
"\n      If not specified, all output will be generated on the current directory."
"\n"
"\n   -t0=<jdi> | --start=<jdi>"
"\n"
"\n      The lower bound of the time span covered by the generated XEPH file."
"\n      <jdi> is the integer part of a Julian date. If this argument is not"
"\n      specified, the lower bound of the covered time span will be determined by"
"\n      the specified input SPICE kernel file."
"\n"
"\n   -t1=<jdi> | --end=<jdi>"
"\n"
"\n      The upper bound of the time span covered by the generated XEPH file."
"\n      <jdi> is the integer part of a Julian date. If this argument is not"
"\n      specified, the upper bound of the covered time span will be determined by"
"\n      the specified input SPICE kernel file."
"\n"
"\n   -v | --with-derivative"
"\n"
"\n      Store Chebyshev polynomial expansions for velocity components in the"
"\n      generated XEPH file."
"\n"
"\n   -T | --test-only"
"\n"
"\n      Do not generate a new XEPH file. Only perform a test by comparing"
"\n      position and velocity coordinates between values calculated at random"
"\n      time points directly from the input SPICE kernelfile and the XEPH file"
"\n      (which must exist, for example, after a previous execution of this"
"\n      program)."
"\n"
"\n   -N=<n> | --test-count=<n>"
"\n"
"\n      Number of random tests per day performed on average. The default value is"
"\n      4 tests per day. The valid range is [1,100]."
"\n"
"\n   --help"
"\n"
"\n      Show this help text and exit."
"\n"
"\nExample:"
"\n"
"\n   " PROGRAM_NAME " astjpl2pcl -d=/jpl/sb441-n373.bsp -f=/jpl/de440.bsp --output-dir=/jpl/xeph"
"\n"
"\n   Output file names are always generated automatically from DE/LE version"
"\n   numbers and time bounds. In the example above, the output files would be:"
"\n"
"\n   /jpl/xeph/AST441+2287184+2688976.xeph"
"\n   /jpl/xeph/AST441+2287184+2688976.log"
"\n"
"\n   This program always generates a log plain text file (.log) with information"
"\n   on working parameters, a summary of the loaded data, and exhaustive test"
"\n   results."
"\n";
}

// ----------------------------------------------------------------------------

int main( int argc, const char* argv[] )
{
   Exception::DisableGUIOutput();
   Exception::EnableConsoleOutput();

   try
   {
/*
      furnsh_c( "/data/de440/bsp/de440.bsp" );
      furnsh_c( "/data/kernels/latest_leapseconds.tls" );
      furnsh_c( "/data/kernels/pck00010.tpc" );

      double M[ 3 ][ 3 ];
      double et;
      str2et_c( "2022 December 26, 00:00:00 TDT", &et );
      tipbod_c( "J2000",
                399, // Jupiter
                et,
                M );

      std::cout << IsoString().Format( "%+.16f %+.16f %+.16f\n"
                                       "%+.16f %+.16f %+.16f\n"
                                       "%+.16f %+.16f %+.16f\n",
                                       M[0][0], M[0][1], M[0][2],
                                       M[1][0], M[1][1], M[1][2],
                                       M[2][0], M[2][1], M[2][2] );
      return 0;
*/

      SayHello();

      String dataFilePath;
      String fundamentalFilePath;
      String outputDir = "./";
      int    startJDI = int32_min;
      int    endJDI = int32_max;
      bool   workWithKBOs = false;
      bool   withDerivative = false;
      int    testsPerDay = 4;
      bool   testOnly = false;

      StringList args;
      for ( int i = 1; i < argc; ++i )
         args.Add( String::UTF8ToUTF16( argv[i] ) );

      for ( const Argument& arg : ExtractArguments( args, ArgumentItemMode::NoItems ) )
      {
         if ( arg.IsNumeric() )
         {
            if ( arg.Id() == "t0" || arg.Id() == "-start" )
               startJDI = TruncInt( arg.NumericValue() );
            else if ( arg.Id() == "t1" || arg.Id() == "-end" )
               endJDI = TruncInt( arg.NumericValue() );
            else if ( arg.Id() == "N" || arg.Id() == "-test-count" )
               testsPerDay = Range( TruncInt( arg.NumericValue() ), 1, 100 );
            else
               throw Error( "Unknown numeric argument: " + arg.Token() );
         }
         else if ( arg.IsString() )
         {
            if ( arg.Id() == "d" || arg.Id() == "-data" )
               dataFilePath = arg.StringValue();
            else if ( arg.Id() == "f" || arg.Id() == "-fundamental" )
               fundamentalFilePath = arg.StringValue();
            else if ( arg.Id() == "o" || arg.Id() == "-output-dir" )
               outputDir = arg.StringValue();
            else
               throw Error( "Unknown string argument: " + arg.Token() );
         }
         else if ( arg.IsSwitch() )
         {
            throw Error( "Unknown switch argument: " + arg.Token() );
         }
         else if ( arg.IsLiteral() )
         {
            if ( arg.Id() == "K" || arg.Id() == "-kbo" )
               workWithKBOs = true;
            else if ( arg.Id() == "v" || arg.Id() == "-with-derivative" )
               withDerivative = true;
            else if ( arg.Id() == "T" || arg.Id() == "-test-only" )
               testOnly = true;
            else if ( arg.Id() == "-help" )
            {
               ShowHelp();
               std::cout << '\n';
               return 0;
            }
            else
               throw Error( "Unknown argument: " + arg.Token() );
         }
      }

      if ( dataFilePath.IsEmpty() )
         throw Error( "No input SPICE kernel file has been specified." );
      if ( fundamentalFilePath.IsEmpty() )
         throw Error( "No fundamental ephemerides file has been specified." );

      std::cout << '\n' << "* Loading SPICE kernel file: " << dataFilePath << '\n';
      IsoString dataFilePath8 = dataFilePath.ToUTF8();
      furnsh_c( dataFilePath8.c_str() );
      std::cout << '\n' << "* Loading SPICE kernel file: " << fundamentalFilePath << '\n';
      dataFilePath8 = fundamentalFilePath.ToUTF8();
      furnsh_c( dataFilePath8.c_str() );

      const TimePoint bspStartTime( 1549, 12, 31.0 );
      const TimePoint bspEndTime  ( 2650,  1, 25.0 );

      if ( endJDI < startJDI )
         Swap( startJDI, endJDI );
      startJDI = Max( startJDI, bspStartTime.JDI() );
      endJDI = Min( endJDI, bspEndTime.JDI() );

      if ( startJDI == endJDI )
         throw Error( "Empty time interval defined." );

      if ( !outputDir.EndsWith( '/' ) )
         outputDir << '/';

      String outputFileName;
      outputFileName.Format( "%s%d%+d%+d", workWithKBOs ? "KBO" : "AST", SOURCE_DENUM, startJDI, endJDI );

      String ephFilePath = outputDir + outputFileName + ".xeph";

      String logFilePath = outputDir + outputFileName;
      if ( testOnly )
         logFilePath += "-test";
      logFilePath += ".log";
      logFile = File::CreateFileForWriting( logFilePath );

      LogLn( "Command line: " + String().ToSpaceSeparated( args ), false/*showOnCout*/ );
      LogLn();

      LogLn( SEPARATOR );
      LogLn( "* SPICE kernel file:" );
      LogLn( dataFilePath );
      LogLn();

      AsteroidData asteroids;
      KBOData kbos;
      const BodyData& bodyData = workWithKBOs ? static_cast<const BodyData&>( kbos ) : static_cast<const BodyData&>( asteroids );

      if ( !testOnly )
      {
         LogLn( SEPARATOR );
         LogLn( "* Generating output ephemeris file:" );
         LogLn( ephFilePath );
         LogLn();

         SerializableEphemerisObjectDataList objects;
         for ( int i = 0; i < bodyData.NumberOfObjects(); ++i )
            objects << MakeObject( startJDI, endJDI, bodyData, i, withDerivative );

         EphemerisMetadata metadata;
         metadata.creatorApplication = String( PROGRAM_NAME ) + " utility software version " + PROGRAM_VERSION;
         metadata.title = String().Format( "JPL DE%d %s Ephemerides", SOURCE_DENUM, workWithKBOs ? "KBO" : "Asteroid" );
//       metadata.briefDescription
         metadata.description = String().Format( "PixInsight %s Ephemerides. "
            "Barycentric coordinates computed with JPL DE%d / SB%d.",
            workWithKBOs ? "KBO" : "Asteroid",
            SOURCE_DENUM, SOURCE_DENUM );
         metadata.organizationName = "Pleiades Astrophoto S.L.";
         metadata.authors = "PTeam";
         metadata.copyright = String().Format( "Copyright (c) 2018-%d, Pleiades Astrophoto", TimePoint::Now().Year() );

         EphemerisFile::Serialize( ephFilePath,
                                   TimePoint( startJDI, 0.5 ), TimePoint( endJDI, 0.5 ),
                                   objects, metadata, EphemerisConstantList() );
      }

      TestEphemerisFile( ephFilePath, testsPerDay, bodyData,
                         workWithKBOs ? Array<int>{ 0, 1, 2, 3, 5, 7, 10, 15,  17,  20,  22,  25,  27,  29 }
                                      : Array<int>{ 0, 1, 2, 3, 4, 5, 50, 70, 100, 150, 250, 300, 340, 342 } );

      logFile.Close();

      return 0;
   }

   ERROR_HANDLER

   return -1;
}

// ----------------------------------------------------------------------------
// EOF pcl/astjpl2pcl.cpp - Released 2021-03-28T09:52:26Z
