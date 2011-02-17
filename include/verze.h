/********************************************************************/
/*  VERZE.H       Zjisteni verze MODULU - hlavicka                  */
/********************************************************************/
#ifndef VERZE_MODULU
#define VERZE_MODULU
/*                                                                  */
/*  Pouzite knihovny :                                              */
#include <dirent.h>
/*                                                                  */
/*  Pouzite definice :                                              */
/*                                                                  */
/*  Volane funkce z MODULU :                                        */
/*                                                                  */
/*  Pouzite struktury, prvky struktury a pristup :                  */

#ifndef MAXPATH
#define MAXPATH 2048
#endif

struct S_verze
  {
    char  jmeno[ 9 ];
    char  datum[ 11 ];
    char  soubor[ MAXPATH ];
    int   verze;
    int   oprava;
    char  typ;
  };
/*                                                                  */
/*  Pouzite globalni promenne a pristup :                           */
/*                                                                  */
/*  Definice funkci uvnitr modulu :                                 */
void ctihdm_verze( struct S_verze * );
void ctioke_verze( struct S_verze * );
void ctipop_verze( struct S_verze * );
void ctiste_verze( struct S_verze * );
void ctistm_verze( struct S_verze * );
void ctistu_verze( struct S_verze * );
void dest_verze( struct S_verze * );
void fileutil_verze( struct S_verze * );
void lokal_verze( struct S_verze * );
void memutil_verze( struct S_verze * );
void mhrl_f_verze( struct S_verze * );
void mhrl_on_verze( struct S_verze * );
void novykrok_verze( struct S_verze * );
void nulovani_verze( struct S_verze * );
void objemy_verze( struct S_verze * );
void otocsys_verze( struct S_verze * );
void pisbin_verze( struct S_verze * );
void pishdm_verze( struct S_verze * );
void pishdn_verze( struct S_verze * );
void pislog_verze( struct S_verze * );
void pists34_verze( struct S_verze * );
void plocha_verze( struct S_verze * );
void pravotoc_verze( struct S_verze * );
void prevod_verze( struct S_verze * );
void runutil_verze( struct S_verze * );
void satfce_1_verze( struct S_verze * );
void sestav_a_verze( struct S_verze * );
void sestmat_verze( struct S_verze * );
void smume_verze( struct S_verze * );
void steny_verze( struct S_verze * );
void timeutil_verze( struct S_verze * );
void topol_mh_verze( struct S_verze * );
void tstelm_verze( struct S_verze * );
void uzlelm_verze( struct S_verze * );
void vody_verze( struct S_verze * );
void wregutil_verze( struct S_verze * );
/*                                                                  */
/*  Definice funkci uvnitr objektu :                                */
void binbin_o_verze( struct S_verze * );
void skriv_o_verze( struct S_verze * );
/*                                                                  */
/*  Definice promennych v modulu :                                  */
/*                                                                  */
/*  Opravy a zmeny :                                                */
/*  Kdy       Kdo R   Co                                            */
/*  17.09.1999 DF 00  Vytvoreni hlavicky                            */
/*  21.09.1999 DF 01  Doplneni skriv_o                              */
/*  01.10.1999 DF 02  Doplneni binbin_o                             */
/*                                                                  */
/********************************************************************/
#endif // VERZE_MODULU
/********************************************************************/
/*  VERZE.H       Konec souboru                                     */
/********************************************************************/
