/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Syntakticka analyza                */
/**  Autor: Karel Fajkus                */
/**  email: xfajku05@stud.fit.vutbr.cz  */
/****************************************/

#ifndef _SYN_H_
#define _SYN_H_


#include "precedencni_analyza.h"
#include "frame.h"

tHTable *pomocna;
int dtype;
bool init;
bool prirovnani;
int pocitadlo;
bool bylo_id;
char* term_id;
char* porovnani;
char* id;
char* funkce;
char* params;
tToken token;
bool byla_funkce;
int pocitani_parametru;
int lab;
int lab_while;


//hlavicky funkci
int syntakticka_anal();
void htPrintTable( tHTable *ptrht );
tChyba DTYPE2();
tChyba DRUH();
tChyba FUNKCE();
tChyba PARAMS();
tChyba DTYPE();
tChyba FORW();
tChyba ACT_LIST();
tChyba DEKLARACE();
tChyba DALSI_P();
tChyba PROM();
tChyba ROZHODNI();
tChyba VESTAV();
tChyba LEXEM();
tChyba CYKLUS();
tChyba TERM();
tChyba UKONCOVACI();
tChyba DOPREDNE();
tChyba TERM2();

#endif
