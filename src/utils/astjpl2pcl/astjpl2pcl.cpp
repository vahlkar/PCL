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
 * Copyright (c) 2017-2021, Pleiades Astrophoto S.L.
 */

#include <pcl/Arguments.h>
#include <pcl/EphemerisFile.h>
#include <pcl/Random.h>

#include <iostream>
#include <wordexp.h>

#include "../cspice/include/SpiceUsr.h"

#define SOURCE_DENUM 430

using namespace pcl;

// ----------------------------------------------------------------------------

#define PROGRAM_NAME    "astjpl2pcl"
#define PROGRAM_VERSION "1.35"
#define PROGRAM_YEAR    "2021"

#define TESTS_PER_DAY   4

#define SEPARATOR       IsoString( '-', 80 )

// ----------------------------------------------------------------------------

/*
 * SPICE body identifiers present in ast343de430.bsp
 */
static int bspBodies[] =
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
 *    ftp://ftp.lowell.edu/pub/elgb/astorb.html
 *    ftp://ftp.lowell.edu/pub/elgb/astorb.dat
 */
const char* asteroidNames[] =
{
   "Ceres", "Pallas", "Juno", "Vesta", "Astraea", "Hebe", "Iris", "Flora", "Metis", "Hygiea",
   "Parthenope", "Victoria", "Egeria", "Irene", "Eunomia", "Psyche", "Thetis", "Melpomene", "Fortuna", "Massalia",
   "Lutetia", "Kalliope", "Thalia", "Themis", "Phocaea", "Proserpina", "Euterpe", "Bellona", "Amphitrite", "Urania",
   "Euphrosyne", "Pomona", "Circe", "Leukothea", "Atalante", "Fides", "Leda", "Laetitia", "Harmonia", "Daphne",
   "Isis", "Ariadne", "Nysa", "Eugenia", "Hestia", "Aglaja", "Doris", "Pales", "Virginia", "Nemausa",
   "Europa", "Kalypso", "Alexandra", "Melete", "Mnemosyne", "Concordia", "Elpis", "Echo", "Erato", "Ausonia",
   "Cybele", "Leto", "Hesperia", "Panopaea", "Niobe", "Feronia", "Galatea", "Eurydike", "Freia", "Frigga",
   "Diana", "Eurynome", "Sappho", "Terpsichore", "Alkmene", "Beatrix", "Klio", "Io", "Semele", "Sylvia",
   "Thisbe", "Julia", "Antiope", "Aegina", "Undina", "Minerva", "Aurora", "Arethusa", "Aegle", "Klotho",
   "Ianthe", "Dike", "Hekate", "Miriam", "Hera", "Klymene", "Artemis", "Dione", "Camilla", "Felicitas",
   "Lydia", "Ate", "Iphigenia", "Amalthea", "Kassandra", "Thyra", "Lomia", "Peitho", "Lachesis", "Hermione",
   "Alkeste", "Johanna", "Nemesis", "Antigone", "Elektra", "Aethra", "Sophrosyne", "Hertha", "Meliboea", "Juewa",
   "Siwa", "Lumen", "Adria", "Vibilia", "Adeona", "Lucina", "Protogeneia", "Gallia", "Nuwa", "Bertha",
   "Xanthippe", "Aemilia", "Una", "Laurentia", "Erigone", "Eva", "Loreley", "Sibylla", "Ophelia", "Baucis",
   "Ino", "Andromache", "Iduna", "Irma", "Eucharis", "Eunike", "Lamberta", "Kolga", "Nausikaa", "Prokne",
   "Eurykleia", "Philomela", "Ampella", "Dynamene", "Penelope", "Pompeja", "Martha", "Hersilia", "Dido", "Isabella",
   "Isolda", "Medea", "Lilaea", "Kleopatra", "Eos", "Rosa", "Oceana", "Henrietta", "Philosophia", "Athamantis",
   "Asterope", "Honoria", "Hypatia", "Vanadis", "Germania", "Eukrate", "Bettina", "Aletheia", "Aline", "Adorea",
   "Sapientia", "Adelheid", "Emma", "Nephthys", "Josephina", "Olga", "Polyxo", "Chaldaea", "Phaeo", "Bamberga",
   "Tamara", "Gudrun", "Svea", "Chicago", "Roberta", "Lacadiera", "Devosa", "Budrosa", "Desiderata", "Tercidina",
   "Hermentaria", "Pariana", "Dembowska", "Ornamenta", "Eleonora", "Liguria", "Ninina", "Apollonia", "Carlova", "Havnia",
   "Padua", "Corduba", "Vincentina", "Aeria", "Palma", "Melusina", "Ursula", "Campania", "Myrrha", "Ilmatar",
   "Siegena", "Aquitania", "Charybdis", "Industria", "Lampetia", "Arsinoe", "Thia", "Arachne", "Aspasia", "Chloris",
   "Elisabetha", "Palatia", "Vaticana", "Aurelia", "Bertholda", "Diotima", "Gratia", "Hippo", "Nephele", "Pythia",
   "Eros", "Eichsfeldia", "Gyptis", "Edna", "Hamburga", "Patientia", "Mathesis", "Bruchsalia", "Megaira", "Alekto",
   "Tisiphone", "Argentina", "Papagena", "Hedwig", "Emita", "Genua", "Kreusa", "Comacina", "Veritas", "Carina",
   "Tokio", "Evelyn", "Cava", "Marion", "Princetonia", "Davida", "Armida", "Amherstia", "Edith", "Brixia",
   "Herculina", "Montague", "Merapi", "Messalina", "Praxedis", "Peraga", "Stereoskopia", "Cheruskia", "Misa", "Semiramis",
   "Bilkis", "Irmgard", "Titania", "Polyxena", "Scheila", "Octavia", "Luisa", "Marianna", "Tekmessa", "Elfriede",
   "Chimaera", "Notburga", "Vundtia", "Zelinda", "Gerlinde", "Denise", "Rachele", "Ludmilla", "Genoveva", "Lanzia",
   "Wratislavia", "Lehigh", "Ekard", "Leonora", "Alauda", "Interamnia", "Erminia", "Fringilla", "Boliviana", "Luscinia",
   "Marghanna", "Mandeville", "Cantabia", "Winchester", "Faina", "Sulamitis", "Massinga", "Pulcova", "Tatjana", "Tanete",
   "Irmintraud", "Berbericia", "Theobalda", "Armenia", "Pickeringia", "Bredichina", "Hohensteina", "Pretoria", "Ani", "Hispania",
   "Tauris", "Ara", "Helio", "Ulla", "Palisana", "Anacostia", "Christa", "Flammario", "Ganymed", "Freda",
   "Lictoria", "Rusthawelia", "Mashona"
};

/*
 * Asteroid photometric and physical parameters.
 *
 * Source:
 *    ftp://ftp.lowell.edu/pub/elgb/astorb.html
 *    ftp://ftp.lowell.edu/pub/elgb/astorb.dat
 *
 * Asteroid database updated as of 2021 January 9
 */
const struct AsteroidData { double H, G, B_V, D; } asteroidData[] =
{
   {  3.52,  0.15,  0.72, 848.4 },
   {  4.20,  0.15,  0.66, 498.1 },
   {  5.26,  0.15,  0.81, 233.9 },
   {  3.28,  0.15,  0.80, 468.3 },
   {  6.99,  0.15,  0.83, 119.1 },
   {  5.65,  0.15,  0.83, 185.2 },
   {  5.58,  0.15,  0.85, 199.8 },
   {  6.51,  0.15,  0.89, 135.9 },
   {  6.31,  0.15,  0.86,  -1.0 },
   {  5.52,  0.15,  0.69, 407.1 },
   {  6.52,  0.15,  0.85, 153.3 },
   {  7.30,  0.15,  0.88, 112.8 },
   {  6.79,  0.15,  0.75, 207.6 },
   {  6.47,  0.15,  0.84,  -1.0 },
   {  5.20,  0.15,  0.84, 255.3 },
   {  6.03,  0.15,  0.70, 253.2 },
   {  7.77,  0.15,  0.83,  90.0 },
   {  6.50,  0.15,  0.85, 140.6 },
   {  7.35,  0.15,  0.75,  -1.0 },
   {  6.51,  0.15,  0.81, 145.5 },
   {  7.51,  0.15,  0.70,  95.8 },
   {  6.50,  0.15,  0.69, 181.0 },
   {  7.16,  0.15,  0.85, 107.5 },
   {  7.26,  0.15,  0.68,  -1.0 },
   {  7.85,  0.15,  0.94,  75.1 },
   {  7.49,  0.15,  0.88,  95.1 },
   {  7.08,  0.15,  0.87,  -1.0 },
   {  7.24,  0.15,  0.85, 120.9 },
   {  5.98,  0.15,  0.83, 212.2 },
   {  7.56,  0.15,  0.87,  99.7 },
   {  6.86,  0.15,  0.67, 255.9 },
   {  7.64,  0.15,  0.86,  80.8 },
   {  8.63,  0.15,  0.70, 113.5 },
   {  8.66,  0.15,  0.70, 103.1 },
   {  8.68,  0.15,  0.73, 105.6 },
   {  7.41,  0.15,  0.84, 108.3 },
   {  8.54,  0.15,  0.72, 115.9 },
   {  6.06,  0.15,  0.89, 149.5 },
   {  7.16,  0.15,  0.85, 107.6 },
   {  7.43,  0.15,  0.73, 174.0 },
   {  7.59,  0.15,  0.88, 100.2 },
   {  8.01,  0.15,  0.87,  65.9 },
   {  6.92,  0.15,  0.70,  70.6 },
   {  7.59,  0.15,  0.66, 214.6 },
   {  8.54,  0.15,  0.70, 124.1 },
   {  8.20,  0.15,  0.66, 127.0 },
   {  7.18,  0.15,  0.72, 221.8 },
   {  7.84,  0.15,  0.75, 149.8 },
   {  9.32,  0.15,  0.71,  99.8 },
   {  7.55,  0.15,  0.77, 147.9 },
   {  6.49,  0.15,  0.66, 302.5 },
   {  8.91,  0.15,  0.71, 115.4 },
   {  7.83,  0.15,  0.70, 165.8 },
   {  8.52,  0.15,  0.69, 113.2 },
   {  7.00,  0.15,  0.83, 112.6 },
   {  9.04,  0.15,  0.69,  93.4 },
   {  8.02,  0.15,  0.67, 164.8 },
   {  8.50,  0.15,  0.85,  60.2 },
   {  8.78,  0.15,  0.71,  95.4 },
   {  7.44,  0.15,  0.90, 103.1 },
   {  6.89,  0.15,  0.67, 237.3 },
   {  6.97,  0.15,  0.84, 122.6 },
   {  7.20,  0.15,  0.70, 138.1 },
   {  8.15,  0.15,  0.74, 122.2 },
   {  7.13,  0.15,  0.82,  83.4 },
   {  9.10,  0.15,  0.78,  86.1 },
   {  8.74,  0.15,  0.69, 118.7 },
   {  9.06,  0.15,  0.71,  55.7 },
   {  8.04,  0.15,  0.70, 183.7 },
   {  8.66,  0.15,  0.75,  69.2 },
   {  8.39,  0.15,  0.71, 120.6 },
   {  7.97,  0.15,  0.88,  66.5 },
   {  8.06,  0.15,  0.90,  78.4 },
   {  8.68,  0.15,  0.71, 119.1 },
   {  8.26,  0.15,  0.81,  61.0 },
   {  8.77,  0.15,  0.70,  81.4 },
   {  9.38,  0.15,  0.73,  79.2 },
   {  7.75,  0.15,  0.66, 154.8 },
   {  8.76,  0.15,  0.71, 120.6 },
   {  6.90,  0.15,  0.70, 260.9 },
   {  7.28,  0.15,  0.66, 200.6 },
   {  6.74,  0.15,  0.88, 151.5 },
   {  8.19,  0.15,  0.69, 120.1 },
   {  8.88,  0.15,  0.73, 109.8 },
   {  6.79,  0.15,  0.73, 126.4 },
   {  7.96,  0.15,  0.73, 141.0 },
   {  7.64,  0.15,  0.66, 204.9 },
   {  8.04,  0.15,  0.71, 136.0 },
   {  7.77,  0.15,  0.77, 169.9 },
   {  7.84,  0.15,  0.70,  82.8 },
   {  8.89,  0.15,  0.75, 104.5 },
   {  9.56,  0.15,  0.70,  71.9 },
   {  7.66,  0.15,  0.83,  88.7 },
   {  9.36,  0.15,  0.72,  83.0 },
   {  7.63,  0.15,  0.86,  91.2 },
   {  8.41,  0.15,  0.68, 123.7 },
   {  8.78,  0.15,  0.70, 119.1 },
   {  7.59,  0.15,  0.74, 146.6 },
   {  7.13,  0.15,  0.70, 222.6 },
   {  9.03,  0.15,  0.73,  89.4 },
   {  7.86,  0.15,  0.71,  86.1 },
   {  8.19,  0.15,  0.70, 134.6 },
   {  9.78,  0.15,  0.70,  72.2 },
   {  8.50,  0.15,  0.94,  46.1 },
   {  8.31,  0.15,  0.76,  99.6 },
   {  7.65,  0.15,  0.86,  79.8 },
   {  8.08,  0.15,  0.68, 148.7 },
   {  8.92,  0.15,  0.86,  41.7 },
   {  7.78,  0.15,  0.70, 174.1 },
   {  7.35,  0.15,  0.72, 209.0 },
   {  8.13,  0.15,  0.85,  76.4 },
   {  8.61,  0.15,  0.70,  -1.0 },
   {  7.72,  0.15,  0.68, 188.2 },
   {  7.07,  0.15,  0.72, 125.0 },
   {  7.21,  0.15,  0.75, 182.3 },
   {  9.00,  0.15,  0.68,  42.7 },
   {  8.82,  0.15,  0.68, 123.3 },
   {  8.20,  0.15,  0.70,  79.2 },
   {  8.18,  0.15,  0.70, 145.4 },
   {  8.05,  0.15,  0.70, 156.6 },
   {  8.46,  0.15,  0.72, 109.8 },
   {  8.46,  0.15,  0.66, 131.0 },
   {  9.17,  0.15,  0.74,  89.9 },
   {  8.13,  0.15,  0.72, 141.8 },
   {  8.22,  0.15,  0.69, 151.1 },
   {  8.37,  0.15,  0.68, 132.2 },
   {  8.76,  0.15,  0.69, 132.9 },
   {  7.64,  0.15,  0.86,  97.7 },
   {  8.47,  0.15,  0.71, 151.1 },
   {  7.71,  0.15, -1.00, 184.9 },
   {  8.70,  0.15,  0.70, 121.0 },
   {  8.36,  0.15,  0.69, 125.0 },
   {  9.15,  0.15,  0.72,  81.2 },
   {  8.94,  0.15,  0.76,  99.1 },
   {  9.72,  0.15,  0.70,  72.6 },
   {  8.86,  0.15,  0.68, 104.9 },
   {  7.89,  0.15,  0.74, 155.2 },
   {  8.09,  0.15,  0.75, 148.4 },
   {  8.60,  0.15,  0.69, 116.7 },
   {  8.76,  0.15,  0.90,  62.4 },
   {  7.98,  0.15,  0.70, 154.1 },
   {  8.48,  0.15,  0.70, 100.9 },
   {  8.30,  0.15, -1.00, 121.0 },
   {  9.67,  0.15,  0.73,  73.2 },
   {  7.92,  0.15,  0.80, 106.0 },
   {  7.68,  0.15,  0.68, 157.5 },
   {  8.27,  0.15,  0.71, 131.2 },
   {  9.00,  0.15,  0.68, 101.0 },
   {  7.35,  0.15,  0.94, 103.3 },
   {  7.80,  0.15,  0.73, 168.4 },
   {  9.18,  0.15,  0.69,  85.7 },
   {  6.56,  0.15,  0.86, 136.3 },
   {  8.53,  0.15,  0.88,  57.2 },
   {  8.41,  0.15,  0.71, 128.4 },
   {  8.41,  0.15,  0.71,  68.4 },
   {  8.84,  0.15,  0.70, 116.3 },
   {  9.52,  0.15,  0.69,  80.9 },
   {  8.76,  0.15,  0.70,  -1.0 },
   {  8.20,  0.15,  0.69, 159.9 },
   {  9.47,  0.15,  0.67,  86.7 },
   {  8.04,  0.15,  0.72, 143.2 },
   {  8.39,  0.15,  0.70, 136.1 },
   {  8.97,  0.15,  0.64,  83.0 },
   {  7.15,  0.15,  0.70, 135.1 },
   {  7.80,  0.15,  0.79, 103.9 },
   {  9.89,  0.15,  0.71,  87.6 },
   {  8.69,  0.15,  0.75,  61.8 },
   {  8.88,  0.15,  0.66, 120.5 },
   {  9.15,  0.15, -1.00,  87.3 },
   {  7.39,  0.15,  0.85, 109.0 },
   {  8.42,  0.15,  0.78, 102.8 },
   {  8.28,  0.15,  0.85,  86.2 },
   {  8.07,  0.15,  0.73, 148.5 },
   {  9.20,  0.15,  0.70, 103.9 },
   {  7.79,  0.15,  0.69, 168.9 },
   {  8.24,  0.15,  0.69, 134.4 },
   {  7.51,  0.15,  0.72,  79.8 },
   {  7.91,  0.15,  0.67, 178.6 },
   {  8.78,  0.15,  0.71, 109.1 },
   {  8.43,  0.15,  0.64, 139.9 },
   {  8.92,  0.15,  0.72, 121.0 },
   {  8.65,  0.15,  0.71, 121.6 },
   {  8.69,  0.15,  0.71, 148.1 },
   {  8.22,  0.15,  0.87,  67.6 },
   {  8.96,  0.15, -1.00,  99.3 },
   {  9.92,  0.15,  0.71,  67.9 },
   {  8.13,  0.15,  0.79, 140.7 },
   {  9.07,  0.15,  0.71,  96.3 },
   {  9.12,  0.15,  0.72,  70.8 },
   {  7.06,  0.15,  0.70, 229.4 },
   {  9.40,  0.15,  0.71,  93.0 },
   {  8.90,  0.15,  0.89, 122.9 },
   {  9.66,  0.15,  0.69,  77.8 },
   {  7.65,  0.15,  0.72, 155.8 },
   {  9.09,  0.15,  0.62,  89.1 },
   {  9.83,  0.15,  0.73,  69.3 },
   {  8.79,  0.15,  0.69,  59.1 },
   {  8.57,  0.15,  0.70,  63.1 },
   {  8.35,  0.15,  0.71, 132.2 },
   {  8.92,  0.15,  0.72,  94.1 },
   {  7.33,  0.15,  0.85, 106.5 },
   {  9.08,  0.15,  0.68,  51.3 },
   {  5.89,  0.15,  0.93, 139.8 },
   {  8.46,  0.15,  0.69, 118.3 },
   {  6.34,  0.15,  0.95, 155.2 },
   {  8.41,  0.15,  0.73, 131.3 },
   {  8.85,  0.15,  0.72, 106.1 },
   {  9.21,  0.15, -1.00,  89.5 },
   {  8.55,  0.15,  0.68, 115.8 },
   {  9.16,  0.15,  0.71,  -1.0 },
   {  9.07,  0.15,  0.75,  -1.0 },
   {  9.28,  0.15,  0.72, 105.9 },
   {  8.69,  0.15, -1.00,  93.8 },
   {  8.68,  0.15,  0.71,  60.0 },
   {  7.46,  0.15, -1.00, 188.6 },
   {  9.16,  0.15,  0.67,  95.8 },
   {  7.56,  0.15,  0.68,  -1.0 },
   {  9.03,  0.15,  0.75,  91.0 },
   {  8.43,  0.15,  0.67, 120.6 },
   {  7.60,  0.15,  0.90,  91.5 },
   {  7.68,  0.15,  0.74, 165.0 },
   {  7.60,  0.15,  0.88, 100.5 },
   {  8.70,  0.15,  0.72, 114.2 },
   {  7.85,  0.15,  0.86,  79.0 },
   {  8.43,  0.15,  0.74,  96.9 },
   {  9.23,  0.15,  0.66,  97.7 },
   {  8.62,  0.15,  0.69, 124.9 },
   {  9.12,  0.15,  0.70,  95.1 },
   {  7.61,  0.15,  0.72, 161.6 },
   {  8.33,  0.15,  0.75, 123.5 },
   {  9.19,  0.15, -1.00,  91.0 },
   {  9.39,  0.15,  0.71,  76.3 },
   {  7.72,  0.15,  0.88,  85.5 },
   {  8.62,  0.15,  0.64, 129.0 },
   {  8.36,  0.15,  0.69, 141.2 },
   {  7.29,  0.15,  0.67, 208.8 },
   {  9.62,  0.15, -1.00,  87.2 },
   {  8.64,  0.15,  0.71, 127.1 },
   {  8.97,  0.15,  0.64,  95.0 },
   {  8.95,  0.15,  0.87,  46.9 },
   { 10.42,  0.15, -1.00,  -1.0 },
   { 10.02,  0.15,  0.69,  65.7 },
   {  8.01,  0.15,  0.68, 159.6 },
   {  9.23,  0.15,  0.68,  87.2 },
   {  9.85,  0.15,  0.70,  85.6 },
   {  6.81,  0.15,  0.65, 225.0 },
   {  9.30,  0.15,  0.66,  81.6 },
   {  8.94,  0.15,  0.70,  84.4 },
   {  9.72,  0.15,  0.64,  74.0 },
   {  9.84,  0.15, -1.00,  73.3 },
   {  8.52,  0.15,  0.66, 115.5 },
   {  8.71,  0.15,  0.65, 125.6 },
   {  6.64,  0.15,  0.83, 134.2 },
   {  8.67,  0.15,  0.71, 116.8 },
   {  8.86,  0.15,  0.70, 116.0 },
   {  8.28,  0.15, -1.00,  63.9 },
   {  7.98,  0.15,  0.70, 150.1 },
   {  8.49,  0.15,  0.69, 139.4 },
   {  8.59,  0.15,  0.75, 115.5 },
   {  9.07,  0.15, -1.00,  97.3 },
   {  8.90,  0.15,  0.75,  82.8 },
   {  9.35,  0.15,  0.72,  81.7 },
   {  8.85,  0.15,  0.66,  -1.0 },
   {  8.89,  0.15,  0.71, 105.9 },
   {  8.41,  0.15,  0.73, 142.3 },
   {  6.41,  0.15,  0.72, 326.1 },
   {  9.17,  0.15,  0.65, 106.2 },
   {  8.26,  0.15,  0.74,  73.1 },
   {  9.57,  0.15,  0.71,  91.1 },
   {  8.51,  0.15,  0.71, 115.7 },
   {  5.88,  0.15,  0.85, 222.2 },
   {  9.42,  0.15,  0.74,  74.5 },
   {  8.17,  0.15,  0.69, 151.4 },
   {  8.82,  0.15,  0.69, 111.3 },
   {  9.84,  0.15,  0.76,  69.7 },
   {  9.13,  0.15,  0.66,  95.9 },
   {  8.11,  0.15,  0.70, 168.2 },
   {  9.54,  0.15, -1.00,  87.0 },
   { 10.14,  0.15,  0.74,  73.0 },
   {  8.58,  0.15,  0.89,  54.0 },
   { 10.52,  0.15,  0.70,  58.1 },
   { 10.75,  0.15,  0.69,  51.9 },
   {  9.41,  0.15,  0.65,  75.3 },
   {  7.96,  0.15, -1.00, 109.1 },
   {  8.94,  0.15,  0.72, 113.3 },
   {  9.75,  0.15,  0.74,  72.3 },
   {  8.78,  0.15,  0.88,  64.9 },
   {  8.53,  0.15,  0.70, 124.7 },
   {  9.49,  0.15, -1.00,  65.2 },
   {  8.32,  0.15,  0.70, 120.3 },
   { 10.80,  0.15,  0.71,  44.1 },
   {  9.22,  0.15,  0.70, 100.7 },
   {  9.10,  0.15,  0.68,  98.2 },
   {  8.68,  0.15,  0.68, 127.4 },
   {  9.21,  0.15,  0.68, 100.9 },
   {  9.42,  0.15, -1.00,  81.3 },
   {  7.53,  0.15,  0.88,  97.3 },
   {  8.15,  0.15,  0.85,  -1.0 },
   {  9.51,  0.15,  0.69,  83.9 },
   {  8.71,  0.15, -1.00,  82.0 },
   {  8.07,  0.15,  0.66, 135.0 },
   {  9.34,  0.15,  0.74,  87.7 },
   {  9.23,  0.15,  0.72,  90.8 },
   {  9.58,  0.15, -1.00,  75.8 },
   {  7.35,  0.15,  0.66, 194.7 },
   {  6.38,  0.15,  0.64, 316.6 },
   {  8.48,  0.15,  0.70, 134.2 },
   {  9.15,  0.15,  0.72,  96.6 },
   {  8.67,  0.15,  0.73, 127.6 },
   {  8.99,  0.15,  0.66, 105.5 },
   {  9.82,  0.15,  0.70,  74.3 },
   {  8.76,  0.15,  0.71, 107.4 },
   {  9.18,  0.15,  0.72,  90.9 },
   {  7.81,  0.15,  0.71, 171.7 },
   {  8.78,  0.15,  0.68, 110.5 },
   { 10.30,  0.15, -1.00,  62.8 },
   {  7.94,  0.15,  0.95,  71.3 },
   {  8.02,  0.15,  0.65, 137.1 },
   {  9.05,  0.15, -1.00, 106.4 },
   {  8.43,  0.15,  0.67, 117.7 },
   {  9.15,  0.15,  0.70,  95.9 },
   {  7.72,  0.15,  0.70, 151.2 },
   {  9.79,  0.15,  0.62,  64.1 },
   {  9.08,  0.15, -1.00,  94.4 },
   {  9.35,  0.15, -1.00,  89.4 },
   {  8.90,  0.15,  0.69,  91.6 },
   {  8.74,  0.15, -1.00, 103.7 },
   {  8.11,  0.15,  0.70, 170.4 },
   {  9.26,  0.15,  0.71, 103.5 },
   {  7.94,  0.15,  0.71, 157.2 },
   {  8.82,  0.15,  0.68, 109.5 },
   {  8.20,  0.15,  0.70,  61.8 },
   {  8.43,  0.15, -1.00, 141.9 },
   {  8.78,  0.15,  0.69, 116.4 },
   {  9.17,  0.15,  0.74,  76.6 },
   {  7.93,  0.15,  0.91,  86.2 },
   {  9.20,  0.15,  0.69,  96.9 },
   {  9.12,  0.15,  0.66,  99.4 },
   {  9.26,  0.15,  0.84,  31.7 },
   {  9.02,  0.15,  0.68, 116.7 },
   {  9.55,  0.15, -1.00,  79.2 },
   {  9.87,  0.15,  0.69,  70.1 },
   {  8.75,  0.15,  0.72, 112.0 }
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

int AsteroidNumberForIndex( int idx )
{
   return bspBodies[idx] - 2000000;
}

// ----------------------------------------------------------------------------

void MakeObjectData( SerializableEphemerisObjectData& object,
                     EphemerisFile::Handle& sun,
                     int startJDI, int endJDI, int index, int order = 0 )
{
   LogLn( SEPARATOR );
   LogLn( "* Generating ephemeris data for "
            + IsoString( AsteroidNumberForIndex( index ) ) + ' ' + asteroidNames[index] );
   LogLn();

   const double au_km = sun.ParentFile().ConstantValue( "AU" );
   const double truncation_km = 5.0e-03; // 5 m

   // Maximum allowed expansion length
   const int maxLength = 120;

   int delta = 200;
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

         T = ChebyshevFit( [jdi1,index,&sun,order,au_km]( double dt )
                           {
                              TimePoint t( jdi1, 0.5+dt );
                              double et = t.SecondsSinceJ2000();
                              double lt;
                              double state[ 6 ];
                              spkez_c( bspBodies[index], et, "J2000", "NONE"/*abcorr*/, 10/*observer*/, state, &lt );
                              MultiVector s0 = sun.StateVectors( t );
                              if ( order == 0 )
                                 return Vector( state[0]/au_km + s0[0][0],
                                                state[1]/au_km + s0[0][1],
                                                state[2]/au_km + s0[0][2] );
                              return Vector( state[3]/au_km*86400 + s0[1][0],
                                             state[4]/au_km*86400 + s0[1][1],
                                             state[5]/au_km*86400 + s0[1][2] );
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
   LogLn( "Object ....................... " + IsoString( AsteroidNumberForIndex( index ) ) + ' ' + asteroidNames[index] );
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

SerializableEphemerisObjectData MakeObject( EphemerisFile::Handle& sun,
                        int startJDI, int endJDI, int index, bool withDerivative = false )
{
   SerializableEphemerisObjectData object( IsoString( AsteroidNumberForIndex( index ) ),
                                           "SSB",
                                           asteroidNames[index] );

   const AsteroidData* data = asteroidData + index;
   object.H = data->H;
   object.G = data->G;
   if ( data->B_V >= 0 )
      object.B_V = data->B_V;
   if ( data->D >= 0 )
      object.D = data->D;

   MakeObjectData( object, sun, startJDI, endJDI, index, 0/*order: position*/ );
   if ( withDerivative )
      MakeObjectData( object, sun, startJDI, endJDI, index, 1/*order: velocity*/ );

   return object;
}

// ----------------------------------------------------------------------------

void TestEphemerisFile( const String filePath, EphemerisFile::Handle& sun, int testsPerDay, const Array<int>& index )
{
   LogLn( SEPARATOR );
   LogLn( "* Testing ephemeris file: " );
   LogLn( filePath );
   LogLn();

   const double au_km = sun.ParentFile().ConstantValue( "AU" );

   EphemerisFile E( filePath );

   TimePoint t0 = E.StartTime();
   TimePoint t1 = E.EndTime();
   double ts = t1 - t0;

   XorShift1024 R;

   for ( int i = 0; i < int( index.Length() ); ++i )
   {
      IsoString objectId( AsteroidNumberForIndex( index[i] ) );

      LogLn( objectId + ' ' + asteroidNames[index[i]] );

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

         MultiVector s0 = sun.StateVectors( t );
         MultiVector s = H.StateVectors( t );

         double et = t.SecondsSinceJ2000();
         double lt;
         double state[ 6 ];
         spkez_c( bspBodies[index[i]], et, "J2000", "NONE"/*abcorr*/, 10/*observer*/, state, &lt );
         for ( int k = 0; k < 3; ++k )
            state[k] = state[k]/au_km + s0[0][k];
         for ( int k = 3; k < 6; ++k )
            state[k] = state[k]/au_km*86400 + s0[1][k-3];

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
   "\nCopyright (c) 2017-" PROGRAM_YEAR " Pleiades Astrophoto S.L."
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
"\n      <file> is the path to an XEPH file providing fundamental solar system"
"\n      ephemerides."
"\n"
"\nOptional arguments:"
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
"\n   " PROGRAM_NAME " --data=/eph/asteroid/ast343de430.bsp --output-dir=/jpl/xeph"
"\n"
"\n   Output file names are always generated automatically from DE/LE version"
"\n   numbers and time bounds. In the example above, the output files would be:"
"\n"
"\n   /jpl/xeph/AST430+2287184+2688976.xeph"
"\n   /jpl/xeph/AST430+2287184+2688976.log"
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
      SayHello();

      String dataFilePath;
      String fundamentalFilePath;
      String outputDir = "./";
      int    startJDI = int32_min;
      int    endJDI = int32_max;
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
            if ( arg.Id() == "v" || arg.Id() == "-with-derivative" )
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

      EphemerisFile fundamental( fundamentalFilePath );
      EphemerisFile::Handle sun( fundamental, "Sn" );

      const TimePoint bspStartTime( 1549, 12, 31.0 );
      const TimePoint bspEndTime  ( 2650,  1, 25.0 );

      if ( endJDI < startJDI )
         Swap( startJDI, endJDI );
      startJDI = Max( Max( fundamental.StartTime().JDI(), startJDI ), bspStartTime.JDI() );
      endJDI = Min( Min( endJDI, fundamental.EndTime().JDI() ), bspEndTime.JDI() );
      if ( startJDI == endJDI )
         throw Error( "Empty time interval defined." );

      if ( !outputDir.EndsWith( '/' ) )
         outputDir << '/';

      String outputFileName;
      outputFileName.Format( "AST%d%+d%+d", SOURCE_DENUM, startJDI, endJDI );

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

      if ( !testOnly )
      {
         LogLn( SEPARATOR );
         LogLn( "* Generating output ephemeris file:" );
         LogLn( ephFilePath );
         LogLn();

         SerializableEphemerisObjectDataList objects;
         for ( int i = 0; i < int( ItemsInArray( bspBodies ) ); ++i )
            objects << MakeObject( sun, startJDI, endJDI, i, withDerivative );

         EphemerisMetadata metadata;
         metadata.creatorApplication = String( PROGRAM_NAME ) + " utility software version " + PROGRAM_VERSION;
         metadata.title = String().Format( "JPL DE%d Asteroid Ephemerides", SOURCE_DENUM );
//       metadata.briefDescription
         metadata.description = String().Format( "PixInsight Asteroid Ephemerides. "
            "Heliocentric coordinates computed with JPL DE%d. "
            "Barycentric coordinates coherent with DE%d.",
            SOURCE_DENUM, TruncInt( fundamental.ConstantValue( "DENUM" ) ) );
         metadata.organizationName = "Pleiades Astrophoto S.L.";
         metadata.authors = "PTeam";
         metadata.copyright = String().Format( "Copyright (C) 2018-%d, Pleiades Astrophoto S.L.", TimePoint::Now().Year() );

         EphemerisFile::Serialize( ephFilePath,
                                   TimePoint( startJDI, 0.5 ), TimePoint( endJDI, 0.5 ),
                                   objects, metadata, EphemerisConstantList() );
      }

      TestEphemerisFile( ephFilePath, sun, testsPerDay, { 0, 1, 2, 3, 4, 5, 50, 70, 100, 150, 250, 300, 340, 342 } );

      logFile.Close();

      return 0;
   }

   ERROR_HANDLER

   return -1;
}

// ----------------------------------------------------------------------------
// EOF pcl/astjpl2pcl.cpp - Released 2021-03-28T09:52:26Z
