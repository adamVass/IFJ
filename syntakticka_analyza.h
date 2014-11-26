/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Syntakticka analyza                */
/**  Autor: Karel Fajkus                */
/**  email: xfajku05@stud.fit.vutbr.cz  */
/****************************************/

#ifndef SYN.h
#define SYN.h


#include "precedencni_analyza.h"
#include "frame.h"

int syntakticka_anal();
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

#endif
