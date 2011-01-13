// #include "interface.h"
#include "interfaceN.h"
#include "read_ini.h"
#include "system.hh"
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>

char *OptGetStr(const char *section, const char *key, const char *def);
double OptGetDbl(const char *section, const char *key, const char *def); //def as default value
long int OptGetInt(const char *section, const char *key, const char *def);
//char *OptGetStrArray(const char *section,const char *key,const char *defval, int sb_count, struct TS_lat *dest);

float che_poradi (int typ_reakce, double max, double K)
{
   if (typ_reakce==4)
   {
      return 1.9 - K/max/2.0;
   }
   else              // tj. 0,1,3
   {
      return (float) typ_reakce;
   }
}
// Cte CHEMIE-OBECNE ze souboru parametru .ich
void ctiich_obecne( void )
{
	int	i;
	const char *section = "CHEMIE-OBECNE";
	char  buffer[ 1024 ];
	char *pString;

	G_prm.T = OptGetDbl(section,"Teplota","0.0");
	//GetPrivateProfileString( (jmeno_sekce), "Teplota", "0.0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.T = atol( buffer );
	if ( G_prm.T <= 0.0 )
	{
   	xprintf(Msg,"\nTeplota musi byt kladna!");
      exit(133);
   	}
/*------------------------------------------------------------------*/
	G_prm.TGf = OptGetDbl(section,"TeplotaGf","-1.0");
	//GetPrivateProfileString( jmeno_sekce, "TeplotaGf", "-1.0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.TGf = atol( buffer );
	if ( G_prm.TGf == -1.0 )
   	{
      G_prm.TGf = G_prm.T;
   	}
	if ( G_prm.T <= 0.0 )
	{
   	xprintf(Msg,"\nTeplota pro zadani dGf musi byt kladna!");
      exit(133);
   	}
/*------------------------------------------------------------------*/
	G_prm.epsilon = OptGetDbl(section,"Epsilon","0.0");
	//GetPrivateProfileString( jmeno_sekce, "Epsilon", "0.0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.epsilon = atol( buffer );
	if ( G_prm.epsilon<= 0.0 )
	{
   	xprintf(Msg,"\nEpsilon musi byt kladne!");
      exit(133);
   	}
/*------------------------------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(section, "Typ_normy", "Absolutni")); 
   //GetPrivateProfileString( jmeno_sekce, "Typ_normy", "Absolutni", buffer, 1024, G_prm.jmeno_ich );
   if ( pString == NULL )
   {
      xprintf(Msg,"\nChybi typ normy!");
      exit(133);
   }
   G_prm.abs_norma = -1;
   if ( strcmp( buffer, "relativni" ) == 0 ) G_prm.abs_norma = 0;
   if ( strcmp( buffer, "Relativni" ) == 0 ) G_prm.abs_norma = 0;
   if ( strcmp( buffer, "rel" ) == 0 ) G_prm.abs_norma = 0;
   if ( strcmp( buffer, "Rel" ) == 0 ) G_prm.abs_norma = 0;
   if ( strcmp( buffer, "r" ) == 0 ) G_prm.abs_norma = 0;
   if ( strcmp( buffer, "R" ) == 0 ) G_prm.abs_norma = 0;
   if ( strcmp( buffer, "0" ) == 0 ) G_prm.abs_norma = 0;
   if ( strcmp( buffer, "absolutni" ) == 0 ) G_prm.abs_norma = 1;
   if ( strcmp( buffer, "Absolutni" ) == 0 ) G_prm.abs_norma = 1;
   if ( strcmp( buffer, "abs" ) == 0 ) G_prm.abs_norma = 1;
   if ( strcmp( buffer, "Abs" ) == 0 ) G_prm.abs_norma = 1;
   if ( strcmp( buffer, "a" ) == 0 ) G_prm.abs_norma = 1;
   if ( strcmp( buffer, "A" ) == 0 ) G_prm.abs_norma = 1;
   if ( strcmp( buffer, "1" ) == 0 ) G_prm.abs_norma = 1;
   if (G_prm.abs_norma == -1)
   {
      xprintf(Msg,"\nTyp normy neni platny!");
      exit(133);
   }
   //delete(pString);
   //pString = NULL;
/*-----------------------------------------------*/
/*
	GetPrivateProfileString( jmeno_sekce, "Omega", "1.0", buffer, 1024, G_prm.jmeno_ich );
	G_prm.omega = _atold( buffer );
	if ( G_prm.omega <= 0.0 )
	{
   	printf("\nOmega musi byt kladna!");
      exit(133);
   }
*/
   G_prm.omega = 1.0;
/*------------------------------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(section,"Skalovani","No"));
   //GetPrivateProfileString( jmeno_sekce, "Skalovani", "Ne", buffer, 1024, G_prm.jmeno_ich );
   if ( pString == NULL )
   {
      xprintf(Msg,"\nChybi definice skalovani!");
      exit(133);
   }
   G_prm.skaluj_matici = -1;
   if ( strcmp( buffer, "N" ) == 0 ) G_prm.skaluj_matici = 0;
   if ( strcmp( buffer, "Ne" ) == 0 ) G_prm.skaluj_matici = 0;
   if ( strcmp( buffer, "No" ) == 0 ) G_prm.skaluj_matici = 0;
   if ( strcmp( buffer, "0" ) == 0 ) G_prm.skaluj_matici = 0;
   if ( strcmp( buffer, "A" ) == 0 ) G_prm.skaluj_matici = 1;
   if ( strcmp( buffer, "Ano" ) == 0 ) G_prm.skaluj_matici = 1;
   if ( strcmp( buffer, "Y" ) == 0 ) G_prm.skaluj_matici = 1;
   if ( strcmp( buffer, "Yes" ) == 0 ) G_prm.skaluj_matici = 1;
   if ( strcmp( buffer, "1" ) == 0 ) G_prm.skaluj_matici = 1;
   if (G_prm.skaluj_matici == -1)
   {
      xprintf(Msg,"\nSkalovani matice neni platne!");
      exit(133);
   }
/*-----------------------------------------------*/
	G_prm.Afi = OptGetDbl(section,"Param_Afi","-1.0");
	//GetPrivateProfileString( jmeno_sekce, "Param_Afi", "-1.0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.Afi = atol( buffer );
	if ( G_prm.Afi < 0.0 )
	{
   	xprintf(Msg,"\nAfi musi byt nezaporne!");
      exit(133);
   }
/*------------------------------------------------------------------*/
	G_prm.objem = OptGetDbl(section,"Objem", "0.0");
	//GetPrivateProfileString( jmeno_sekce, "Objem", "0.0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.objem = atol( buffer );
	if ( G_prm.objem <= 0.0 )
	{
   	xprintf(Msg,"\nObjem musi byt kladny!");
      exit(133);
   }
/*------------------------------------------------------------------*/
	G_prm.splocha = OptGetDbl(section,"Sorpcni_plocha","0.0");
	//GetPrivateProfileString( jmeno_sekce, "Sorpcni_plocha", "0.0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.splocha = atol( buffer );
	if ( G_prm.splocha <= 0.0 )
	{
	xprintf(Msg,"\nSorpcni plocha musi byt kladna!");
	  exit(133);
   }
/*------------------------------------------------------------------*/
	G_prm.b = OptGetDbl(section,"Param_b","0.0");
	//GetPrivateProfileString( jmeno_sekce, "Param_b", "0.0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.b = atol( buffer );
	if ( G_prm.b <= 0.0 )
	{
   	xprintf(Msg,"\nb musi byt kladne!");
      exit(133);
   }
/*------------------------------------------------------------------*/
	G_prm.deltaT = OptGetDbl(section,"deltaT","0.0");
	//GetPrivateProfileString( jmeno_sekce, "deltaT", "0.0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.deltaT = atol( buffer );
	if ( G_prm.deltaT <= 0.0 )
	{
   	xprintf(Msg,"\nCasovy krok musi byt kladny!");
      exit(133);
   }
/*------------------------------------------------------------------*/
	G_prm.cas_kroku = OptGetInt(section,"Cas_kroku","0");
	//GetPrivateProfileString( jmeno_sekce, "Cas_kroku", "0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.cas_kroku = atoi( buffer );
	if ( G_prm.cas_kroku <= 0 )
	{
   	xprintf(Msg,"\nPocet casovych kroku musi byt kladny!");
      exit(133);
   }
/*------------------------------------------------------------------*/
	G_prm.vypisy = OptGetInt(section,"Uroven_vypisu","0");
	//GetPrivateProfileString( jmeno_sekce, "Uroven_vypisu", "0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.vypisy = atoi( buffer );
/*------------------------------------------------------------------*/
	G_prm.pocet_latekvefazi = OptGetInt(section,"Pocet_latek_ve_fazi","0");
	//GetPrivateProfileString( jmeno_sekce, "Pocet_latek_ve_fazi", "0", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.pocet_latekvefazi = atoi( buffer );
	if ( G_prm.pocet_latekvefazi <= 0 )
	{
   	xprintf(Msg,"\nPocet latek ve fazi musi byt kladny!");
      exit(133);
   }
/*------------------------------------------------------------------*/
	i = OptGetInt(section, "Pocet_dalsich_latek","0");
	//GetPrivateProfileString( jmeno_sekce, "Pocet_dalsich_latek", "0", buffer, 1024, G_prm.jmeno_ich );
	//i = atoi( buffer );
	if ( i < 0 )
	{
   	xprintf(Msg,"\nPocet dalsich latek nesmi byt zaporny!");
      exit(133);
   }
   G_prm.pocet_latek = i+G_prm.pocet_latekvefazi;
   if (G_prm.pocet_latek>MAX_POC_LATEK)
   {
	   printf ("Celkovy pocet latek muze byt maximalne %d!\n", MAX_POC_LATEK);
	   exit(121);
   }
/*------------------------------------------------------------------*/
	G_prm.deleni_RK = OptGetInt(section,"Kroku_pomale_kinetiky","1");
	//GetPrivateProfileString( jmeno_sekce, "Kroku_pomale_kinetiky", "1", buffer, 1024, G_prm.jmeno_ich );
	//G_prm.deleni_RK = atoi( buffer );
	if ( G_prm.deleni_RK < 1 )
	{
   	xprintf(Msg,"\nPocet kroku pomale kinetiky musi byt kladny!");
      exit(133);
   }
/*------------------------------------------------------------------*/
/*
	GetPrivateProfileString( jmeno_sekce, "Pocet_reakci", "0", buffer, 1024, G_prm.jmeno_ich );
	G_prm.celkovy_pocet_reakci = atoi( buffer );
	if ( G_prm.celkovy_pocet_reakci <= 0 )
	{
   	printf("\nPocet reakci musi byt kladny!");
      exit(133);
   }
*/
}
/********************************************************************/
/*                      Cte LATKY_VE_FAZI ze souboru parametru .ich */
/********************************************************************/
void ctiich_latkyvefazi( void )
{
   char  buffer[1024];
   int   i,j;
   char  nazev[30], nazev1[30], *pom_buf;
   const char *separators = " ,\t";
   char *pString;

// Alokace seznamu latek
   P_lat = (TS_lat *)malloc( (G_prm.pocet_latek)*sizeof( TS_lat ) );
   if ( P_lat == NULL )
   {
	   printf ("Malo pameti!\n");
	   exit(0);
   }
// Nacteni obsahu seznamu latek
   sprintf( nazev, "LATKY_VE_FAZI" );

   //pString =
   //strcpy(buffer,OptGetStrArray(nazev, "Nazev", G_prm.pocet_latekvefazi, P_lat); 
   strcpy(buffer,OptGetStr(nazev,"Nazev","<NeplatnyNazev>"));
   //GetPrivateProfileString( nazev, "Nazev", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      for (j=0; j<G_prm.pocet_latekvefazi; j++)
      {
         sprintf( P_lat[j].nazev, "Latka_ve_fazi_%d", j+1 );
      }
   }
   else
   {
      for (j=0; j<G_prm.pocet_latekvefazi; j++)
      {
         if ( pom_buf == NULL )
         {
            xprintf(Msg,"\nChybi nazev %d. latky ve fazi!", j+1);
            exit(133);
         }else{
	 //printf("\n NAZEV %d-TE LATKY JE %s\n",j,pom_buf);
         strcpy(P_lat[j].nazev, pom_buf); //tmp_copy() instead of strcpy()
	 //printf("\n NAZEV %d-TE P_LATKY JE %s\n",j,P_lat[j].nazev);
         pom_buf = strtok( NULL, separators );
	 }
      }
      if ( pom_buf != NULL )
      {
         xprintf(Msg,"\nPrilis mnoho nazvu latek ve fazi!\n");
         exit(133);
      }
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"dGf","<NeplatnyNazev>"));
   //GetPrivateProfileString( nazev, "dGf", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      pom_buf = NULL;
   }
   for (j=0; j<G_prm.pocet_latekvefazi; j++)
   {
      if ( pom_buf == NULL )
      {
         xprintf(Msg,"\nChybi dGf %d. latky ve fazi!", j+1);
         exit(133);
      }
      P_lat[j].dGf = atof(pom_buf);
	//printf("\n P_lat[%d].dGf %Lf\n",j,P_lat[j].dGf);
      pom_buf = strtok( NULL, separators );
   }
   if ( pom_buf != NULL )
   {
      xprintf(Msg,"\nPrilis mnoho dGf pro latky ve fazi!");
      exit(133);
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"dHf","<NeplatnyNazev>"));
   //GetPrivateProfileString( nazev, "dHf", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      for (j=0; j<G_prm.pocet_latekvefazi; j++)
      {
         P_lat[j].dHf = 0.0;
      }
      pom_buf = NULL;
   }
   else for (j=0; j<G_prm.pocet_latekvefazi; j++)
   {
      if ( pom_buf == NULL )
      {
         xprintf(Msg,"\nChybi dHf %d. latky ve fazi!", j+1);
         exit(133);
      }
      P_lat[j].dHf = atof(pom_buf);
      pom_buf = strtok( NULL, separators );
   }
   if ( pom_buf != NULL )
   {
      xprintf(Msg,"\nPrilis mnoho dHf pro latky ve fazi!");
      exit(133);
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"Mol_hmotnost","<NeplatnyNazev>"));
   //GetPrivateProfileString( nazev, "Mol_hmotnost", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      pom_buf = NULL;
   }
      for (j=0; j<G_prm.pocet_latekvefazi; j++)
      {
         if ( pom_buf == NULL )
         {
            xprintf(Msg,"\nChybi molarni hmotnost %d. latky ve fazi!", j+1);
            exit(133);
         }
		 P_lat[j].M = atof(pom_buf);
		 //printf("\n molarni hmotnost latky %d je %Lf",P_lat[j].M);
         pom_buf = strtok( NULL, separators );
      }
      if ( pom_buf != NULL )
      {
         xprintf(Msg,"\nPrilis mnoho molarnich hmotnosti pro latky ve fazi!");
         exit(133);
      }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"Naboj","<NeplatnyNazev>"));
   //GetPrivateProfileString( nazev, "Naboj", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      pom_buf = NULL;
   }
   for (j=0; j<G_prm.pocet_latekvefazi; j++)
   {
      if ( pom_buf == NULL )
      {
         xprintf(Msg,"\nChybi naboj %d. latky ve fazi!", j+1);
         exit(133);
      }
      P_lat[j].Q = atoi(pom_buf);
      pom_buf = strtok( NULL, separators );
   }
   if ( pom_buf != NULL )
   {
      xprintf(Msg,"\nPrilis mnoho naboju pro latky ve fazi!");
      exit(133);
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"Poc_molalita","<NeplatnyNazev>"));
   //GetPrivateProfileString( nazev, "Poc_molalita", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      for (j=0; j<G_prm.pocet_latekvefazi; j++)
      {
         P_lat[j].m0 = 0.0;
      }
   }
   else
   {
      for (j=0; j<G_prm.pocet_latekvefazi; j++)
      {
         if ( pom_buf == NULL )
         {
            xprintf(Msg,"\nChybi pocatecni molalita %d. latky ve fazi!", j+1);
            exit(133);
         }
         P_lat[j].m0 = atof(pom_buf);
         pom_buf = strtok( NULL, separators );
      }
      if ( pom_buf != NULL )
      {
         xprintf(Msg,"\nPrilis mnoho pocatecnich molalit pro latky ve fazi!");
         exit(133);
      }
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"Poc_obsah_sorbovany","<NeplatnyNazev>"));
   //GetPrivateProfileString( nazev, "Poc_obsah_sorbovany", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      for (j=0; j<G_prm.pocet_latekvefazi; j++)
      {
         P_lat[j].m0_sorb = 0.0;
      }
   }
   else
   {
      for (j=0; j<G_prm.pocet_latekvefazi; j++)
      {
         if ( pom_buf == NULL )
         {
            xprintf(Msg,"\nChybi pocatecni sorbovany obsah %d. latky ve fazi!", j+1);
            exit(133);
         }
         P_lat[j].m0_sorb = atof(pom_buf);
         pom_buf = strtok( NULL, separators );
      }
      if ( pom_buf != NULL )
      {
         xprintf(Msg,"\nPrilis mnoho pocatecnich sorbovanych obsahu pro latky ve fazi!");
         exit(133);
	  }
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"Typ_sorpce","<NeplatnyNazev>")); 
   //GetPrivateProfileString( nazev, "Typ_sorpce", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
	  for (j=0; j<G_prm.pocet_latekvefazi; j++)
	  {
		 P_lat[j].typ_sorpce = 0;
	  }
   }
   else
   {
	  for (j=0; j<G_prm.pocet_latekvefazi; j++)
	  {
		 if ( pom_buf == NULL )
		 {
			xprintf(Msg,"\nChybi typ sorpce %d. latky ve fazi!", j+1);
			exit(133);
		 }
		 P_lat[j].typ_sorpce = -888;
		 if ( strcmp( pom_buf, "Zadna" ) == 0 ) P_lat[j].typ_sorpce = 0;
		 if ( strcmp( pom_buf, "zadna" ) == 0 ) P_lat[j].typ_sorpce = 0;
		 if ( strcmp( pom_buf, "0" ) == 0 ) P_lat[j].typ_sorpce = 0;
		 if ( strcmp( pom_buf, "Kon" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "kon" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "Kons" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "kons" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "Konst" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "konst" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "Konstantni" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "konstantni" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "1" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "Lin" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "lin" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "Linearni" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "linearni" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "Linear" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "linear" ) == 0 ) P_lat[j].typ_sorpce = 1;
		 if ( strcmp( pom_buf, "2" ) == 0 ) P_lat[j].typ_sorpce = 2;
		 if ( strcmp( pom_buf, "Freund" ) == 0 ) P_lat[j].typ_sorpce = 2;
		 if ( strcmp( pom_buf, "Freun" ) == 0 ) P_lat[j].typ_sorpce = 2;
		 if ( strcmp( pom_buf, "Freundlich" ) == 0 ) P_lat[j].typ_sorpce = 2;
		 if ( strcmp( pom_buf, "3" ) == 0 ) P_lat[j].typ_sorpce = 3;
		 if ( strcmp( pom_buf, "Langmuir" ) == 0 ) P_lat[j].typ_sorpce = 3;
		 if ( strcmp( pom_buf, "Lang" ) == 0 ) P_lat[j].typ_sorpce = 3;
		 if (P_lat[j].typ_sorpce == -888)
		 {
			xprintf(Msg,"\nTyp sorpce %d. latky ve fazi neni platny!", j+1);
			exit(133);
		 }
		 pom_buf = strtok( NULL, separators );
	  }
	  if ( pom_buf != NULL )
	  {
		 xprintf(Msg,"\nPrilis mnoho typu sorpce pro latky ve fazi!");
		 exit(133);
	  }
   }
/*-----------------------------------------------*/
   for (i = 0; i<MAX_POC_PARAM_SORPCE ; i++)
   {
      sprintf(nazev1, "Param_sorpce%d", i+1 );
      pString = strcpy(buffer,OptGetStr(nazev,nazev1,"<NeplatnyNazev>"));      
      //GetPrivateProfileString( nazev, nazev1, "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
      pom_buf = strtok( buffer, separators );
      if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
      {
         for (j=0; j<G_prm.pocet_latekvefazi; j++)
         {
            P_lat[j].param_sorpce[i] = 0.0;
         }
      }
      else
      {
         for (j=0; j<G_prm.pocet_latekvefazi; j++)
         {
            if ( pom_buf == NULL )
            {
               xprintf(Msg,"\nChybi %d. parametr sorpce %d. latky ve fazi!", i+1, j+1);
               exit(133);
            }
            P_lat[j].param_sorpce[i] = atof(pom_buf);
            if (P_lat[j].param_sorpce[i]<0.0)
            {
               xprintf(Msg,"\nParametry sorpce nesmeji byt zaporne!");
               exit(100);
            }
            pom_buf = strtok( NULL, separators );
         }
         if ( pom_buf != NULL )
         {
            xprintf(Msg,"\nPrilis mnoho %d. parametru sorpce pro latky ve fazi!", i+1);
            exit(133);
         }
      }
   }
/*-----------------------------------------------*/
}
// Cte DALSI_LATKY ze souboru parametru .ich
void ctiich_dalsilatky( void )
{
   char  buffer[1024];
   int   j;
   char  nazev[30], *pom_buf;
   const char* separators = " ,\t";
   char *pString = NULL;

   if (G_prm.pocet_latekvefazi == 0)
   {
      return;
   }
// Nacteni obsahu seznamu latek
   sprintf( nazev, "DALSI_LATKY" );
   pString = strcpy(buffer,OptGetStr(nazev,"Nazev","<NeplatnyNazev>")); 
   //GetPrivateProfileString( nazev, "Nazev", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      for (j=G_prm.pocet_latekvefazi;j<G_prm.pocet_latek; j++)
      {
         sprintf( P_lat[j].nazev, "Dalsi_latka_%d", j+1-G_prm.pocet_latek );
      }
   }
   else
   {
      for (j=G_prm.pocet_latekvefazi;j<G_prm.pocet_latek; j++)
      {
         if ( pom_buf == NULL )
         {
            xprintf(Msg,"\nChybi nazev %d. dalsi latky!", j+1-G_prm.pocet_latek);
            exit(133);
         }
         strcpy (P_lat[j].nazev, pom_buf);
         pom_buf = strtok( NULL, separators );
      }
      if ( pom_buf != NULL )
      {
         xprintf(Msg,"\nPrilis mnoho nazvu dalsich latek!");
         exit(133);
      }
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"dGf","<NeplatnyNazev>")); 
   //GetPrivateProfileString( nazev, "dGf", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      pom_buf = NULL;
   }
   for (j=G_prm.pocet_latekvefazi;j<G_prm.pocet_latek; j++)
   {
      if ( pom_buf == NULL )
      {
         xprintf(Msg,"\nChybi dGf %d. dalsi latky!", j+1-G_prm.pocet_latek);
         exit(133);
      }
      P_lat[j].dGf = atof(pom_buf);
      pom_buf = strtok( NULL, separators );
   }
   if ( pom_buf != NULL )
   {
      xprintf(Msg,"\nPrilis mnoho dGf pro dalsi latky!");
      exit(133);
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"dHf","<NeplatnyNazev>")); 
   //GetPrivateProfileString( nazev, "dHf", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      for (j=G_prm.pocet_latekvefazi;j<G_prm.pocet_latek; j++)
      {
         P_lat[j].dHf = 0.0;
      }
      pom_buf = NULL;
   }
   else for (j=G_prm.pocet_latekvefazi;j<G_prm.pocet_latek; j++)
   {
      if ( pom_buf == NULL )
      {
         xprintf(Msg,"\nChybi dHf %d. dalsi latky!", j+1-G_prm.pocet_latek);
         exit(133);
      }
      P_lat[j].dHf = atof(pom_buf);
      pom_buf = strtok( NULL, separators );
   }
   if ( pom_buf != NULL )
   {
      xprintf(Msg,"\nPrilis mnoho dHf pro dalsi latky!");
      exit(133);
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"Mol_hmotnost","<NeplatnyNazev>")); 
   //GetPrivateProfileString( nazev, "Mol_hmotnost", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      pom_buf = NULL;
   }
   for (j=G_prm.pocet_latekvefazi;j<G_prm.pocet_latek; j++)
   {
      if ( pom_buf == NULL )
      {
         xprintf(Msg,"\nChybi molarni hmotnost %d. dalsi latky!", j+1-G_prm.pocet_latek);
         exit(133);
      }
      P_lat[j].M = atof(pom_buf);
      pom_buf = strtok( NULL, separators );
   }
   if ( pom_buf != NULL )
   {
      xprintf(Msg,"\nPrilis mnoho molarnich hmotnosti pro dalsi latky!");
      exit(133);
   }
/*-----------------------------------------------*/
   pString = strcpy(buffer,OptGetStr(nazev,"Aktivita","<NeplatnyNazev>")); 
   //GetPrivateProfileString( nazev, "Aktivita", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
   pom_buf = strtok( buffer, separators );
   if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 )
   {
      pom_buf = NULL;
   }
   for (j=G_prm.pocet_latekvefazi;j<G_prm.pocet_latek; j++)
   {
      if ( pom_buf == NULL )
      {
         xprintf(Msg,"\nChybi aktivita %d. dalsi latky!", j+1-G_prm.pocet_latek);
         exit(133);
      }
      P_lat[j].aktivita = atof(pom_buf);
      pom_buf = strtok( NULL, separators );
   }
   if ( pom_buf != NULL )
   {
      xprintf(Msg,"\nPrilis mnoho aktivit pro dalsi latky!");
      exit(133);
   }
/*-----------------------------------------------*/
}
// Cte REAKCE ze souboru parametru .ich
void ctiich_reakce( void )
{
   char  buffer[1024];
   int   i,j;
   char  nazev[30], *pom_buf;
   const char* separators = " ,\t";
   TS_che pom_che;
   double max_poloc_rozp;
   
// Zjisteni delky seznamu reakci
   for ( i = 1;; ++i )
   {
      sprintf( nazev, "REAKCE_%d", i );
      strcpy(buffer,OptGetStr(nazev,"Typ_reakce","<NeplatnyNazev>")); xprintf(Msg,"probehlo zjisteni typu reakce %d: %s\n",i, buffer);  
      //GetPrivateProfileString( nazev, "Typ_reakce", "<NeplatnyNazev>", buffer, 1024, G_prm.jmeno_ich );
      if ( strcmp( buffer, "<NeplatnyNazev>" ) == 0 ) break;
   }
   G_prm.celkovy_pocet_reakci = i-1;
	if ( i==1 )
	{
   	xprintf(Msg,"\nNeni definovana zadna reakce!");
      exit(133);
   }
// Alokace seznamu reakci
   P_che = (TS_che *)malloc( (G_prm.celkovy_pocet_reakci)*sizeof( TS_che ) );
   if ( P_che == NULL )
   {
	   printf ("Malo pameti!\n");
	   exit(0);
   }
// Nacteni obsahu seznamu reakci
	G_prm.pocet_reakci_pro_matici = 0;
   G_prm.pocet_rozpadu = 0;
   G_prm.pocet_pom_kin = 0;
   max_poloc_rozp = 0.0;
   for (i=0; i<G_prm.celkovy_pocet_reakci; i++)
   {
      sprintf( nazev, "REAKCE_%d", i+1 );
      //strcpy(buffer,OptGetStr(nazev,"Nazev",nazev)); 
      //puvodni wokeni fce//GetPrivateProfileString( nazev, "Nazev", nazev, buffer, 1024, G_prm.jmeno_ich );
      //pom_buf = strtok( buffer, separators );
      //strcpy(P_che[i].nazev, pom_buf);
      sprintf(P_che[i].nazev,"REAKCE_%d",i+1);
/*-----------------------------------------------*/
      strcpy(buffer,OptGetStr(nazev,"Typ_reakce","<NeplatnyTyp>")); 
      //GetPrivateProfileString( nazev, "Typ_reakce", "<NeplatnyTyp>", buffer, 1024, G_prm.jmeno_ich );
      pom_buf = strtok( buffer, separators );
      P_che[i].typ_reakce = -888;
      if ( strcmp( pom_buf, "Radioaktivni_rozpad" ) == 0 ) P_che[i].typ_reakce = 4;
      if ( strcmp( pom_buf, "radioaktivni_rozpad" ) == 0 ) P_che[i].typ_reakce = 4;
      if ( strcmp( pom_buf, "Rozpad" ) == 0 ) P_che[i].typ_reakce = 4;
      if ( strcmp( pom_buf, "rozpad" ) == 0 ) P_che[i].typ_reakce = 4;
      if ( strcmp( pom_buf, "Rozp" ) == 0 ) P_che[i].typ_reakce = 4;
      if ( strcmp( pom_buf, "rozp" ) == 0 ) P_che[i].typ_reakce = 4;
      if ( strcmp( pom_buf, "RR" ) == 0 ) P_che[i].typ_reakce = 4;
      if ( strcmp( pom_buf, "rr" ) == 0 ) P_che[i].typ_reakce = 4;
      if ( strcmp( pom_buf, "4" ) == 0 ) P_che[i].typ_reakce = 4;
      if ( strcmp( pom_buf, "Pomala_kinetika" ) == 0 ) P_che[i].typ_reakce = 3;
      if ( strcmp( pom_buf, "pomala_kinetika" ) == 0 ) P_che[i].typ_reakce = 3;
      if ( strcmp( pom_buf, "3" ) == 0 ) P_che[i].typ_reakce = 3;
      if ( strcmp( pom_buf, "Kineticka" ) == 0 ) P_che[i].typ_reakce = 1;
      if ( strcmp( pom_buf, "kineticka" ) == 0 ) P_che[i].typ_reakce = 1;
      if ( strcmp( pom_buf, "Kinetika" ) == 0 ) P_che[i].typ_reakce = 1;
      if ( strcmp( pom_buf, "kinetika" ) == 0 ) P_che[i].typ_reakce = 1;
      if ( strcmp( pom_buf, "1" ) == 0 ) P_che[i].typ_reakce = 1;
      if ( strcmp( pom_buf, "Rovnovazna" ) == 0 ) P_che[i].typ_reakce = 0;
      if ( strcmp( pom_buf, "rovnovazna" ) == 0 ) P_che[i].typ_reakce = 0;
      if ( strcmp( pom_buf, "Rovnovaha" ) == 0 ) P_che[i].typ_reakce = 0;
      if ( strcmp( pom_buf, "rovnovaha" ) == 0 ) P_che[i].typ_reakce = 0;
      if ( strcmp( pom_buf, "0" ) == 0 ) P_che[i].typ_reakce = 0;
      if (P_che[i].typ_reakce == -888)
      {
         xprintf(Msg,"\nTyp reakce %d neni platny!", i);
         exit(133);
      }
      if (P_che[i].typ_reakce==0)
      {
         G_prm.pocet_rovnovah++;
      }
      if (P_che[i].typ_reakce==1)
      {
         G_prm.pocet_kinetik++;
      }
      if (P_che[i].typ_reakce==3)
      {
         G_prm.pocet_pom_kin++;
      }
      if (P_che[i].typ_reakce==4)
      {
         G_prm.pocet_rozpadu++;
      }
      G_prm.pocet_reakci_pro_matici = G_prm.pocet_rovnovah+G_prm.pocet_kinetik;
/*-----------------------------------------------*/
//      GetPrivateProfileString( nazev, "Poc_posunuti", "0.0", buffer, 1024, G_prm.jmeno_ich );
//      P_che[i].zeta0 = _atold( buffer );
/*------------------------------------------------------------------*/
      strcpy(buffer,OptGetStr(nazev,"Stechiometrie","")); 
      //GetPrivateProfileString( nazev, "Stechiometrie", "", buffer, 1024, G_prm.jmeno_ich );
      pom_buf = strtok( buffer, separators );
      for (j = 0; j<G_prm.pocet_latek; j++)
      {
         if ( pom_buf == NULL )
         {
            xprintf(Msg,"\nChybi %d. stechometricky koeficient v %d. rovnici!", j+1, i+1);
            exit(133);
         }
		 P_che[i].stech_koef_p[j] = atoi( pom_buf );
		 xprintf(Msg,"\nP_che[%d].stech_koef_p[%d]: %d",i,j,P_che[i].stech_koef_p[j]);
         pom_buf = strtok( NULL, separators );
      }
      if ( pom_buf != NULL )
      {
         xprintf(Msg,"\nV %d. rovnici je prilis mnoho stechiometrickych koeficientu!", i+1);
         exit(133);
      }
/*------------------------------------------------------------------*/
      if ((P_che[i].typ_reakce==1)||(P_che[i].typ_reakce==3))
      {
         P_che[i].K = OptGetDbl(nazev,"Kinet_konst","0.0");
	 if(P_che[i].typ_reakce==1)xprintf(Msg,"\nKineticka konstanta v %d. rovnici ma hodnotu %f", i+1, P_che[i].K);
	 //GetPrivateProfileString( nazev, "Kinet_konst", "0.0", buffer, 1024, G_prm.jmeno_ich );
         //P_che[i].K = atol( buffer );
         if ( P_che[i].K <= 0.0 )
         {
            xprintf(Msg,"\nKineticka konstanta v %d. rovnici neni kladna!", i+1);
            exit(133);
         }
/*------------------------------------------------------------------*/
	 strcpy(buffer,OptGetStr(nazev,"Kinet_mocniny","")); 
         //GetPrivateProfileString( nazev, "Kinet_mocniny", "", buffer, 1024, G_prm.jmeno_ich );
         pom_buf = strtok( buffer, separators );
         for (j = 0; j<G_prm.pocet_latek; j++)
         {
	    if(j >= G_prm.pocet_latekvefazi) ;//P_che[i].exponent[j] = 0.0;
            else
	    {
		if( pom_buf == NULL )
        	{
               xprintf(Msg,"\nChybi %d. mocnina pro kinetiku %d. rovnice!", j+1, i+1);
               exit(133);
            	}
            P_che[i].exponent[j] = atof( pom_buf );
	    xprintf(Msg,"\nP_che[%d].exponent[%d]: %f",i,j,P_che[i].exponent[j]);
            pom_buf = strtok( NULL, separators );
           }
	 }
         if ( pom_buf != NULL )
         {
            xprintf(Msg,"\nV %d. rovnici je prilis mnoho exponentu pro kinetiku!", i+1);
            exit(133);
         }
      }
/*------------------------------------------------------------------*/
      if (P_che[i].typ_reakce==0)
      {
	 P_che[i].K = OptGetDbl(nazev, "Rovnov_konst","-1.0");
         //GetPrivateProfileString( nazev, "Rovnov_konst", "-1.0", buffer, 1024, G_prm.jmeno_ich );
         //P_che[i].K = atol( buffer );
      }
/*------------------------------------------------------------------*/
      if (P_che[i].typ_reakce==4)
      {
	 P_che[i].K = OptGetDbl( nazev, "Polocas_rozp","-1.0");
         //GetPrivateProfileString( nazev, "Polocas_rozp", "-1.0", buffer, 1024, G_prm.jmeno_ich );
         //P_che[i].K = atol( buffer );
         if ( P_che[i].K <= 0.0 )
         {
            xprintf(Msg,"\n%d. rovnice ma definovan nekladny polocas rozpadu!", i+1);
            exit(133);
         }
         if ( P_che[i].K > max_poloc_rozp )
         {
            max_poloc_rozp = P_che[i].K;
         }
      }
/*------------------------------------------------------------------*/
   }
	if (G_prm.celkovy_pocet_reakci>1)
   {
      for (j=0; j<G_prm.celkovy_pocet_reakci; j++)
      {
         for (i=0; i<G_prm.celkovy_pocet_reakci-1; i++)
         {
            if (che_poradi(P_che[i].typ_reakce,max_poloc_rozp,P_che[i].K) > che_poradi(P_che[i+1].typ_reakce,max_poloc_rozp,P_che[i+1].K))
            {
            	pom_che = P_che[i];
               P_che[i] = P_che[i+1];
               P_che[i+1] = pom_che;
            }
         }
      }
   }
}

// Cte soubor parametru chemie .ICH
void ctiich( void )
{
	/*FILE *fw;
	fw = fopen("ctiVyst.txt", "w"); //vyccissteenii souboru
	fclose(fw);*/
	//if((freopen("ctiVyst.txt", "a", stdout)) == NULL) printf("\npresmerovani vystupu\n");

	xprintf(Msg, "Cteni parametru (%s): \n", G_prm.jmeno_ich );
	ctiich_obecne(); xprintf(Msg,"probehla funkce ctiich_obecne()\n");
	ctiich_latkyvefazi(); xprintf(Msg,"probehla funkce ctiich_latkyvefazi()\n");
	ctiich_dalsilatky(); xprintf(Msg,"probehla funkce ctiich_dalsilatky()\n");
	ctiich_reakce(); xprintf(Msg,"probehla funkce ctiich_reakce()\n");
	xprintf(Msg, "O.K.\n" );

	//freopen( "CON", "w", stdout );
}

/*void tmp_copy(char *dest, char *source)
{
const char *tmp_string = source;

strcpy(dest,tmp_string);

return;
}*/
