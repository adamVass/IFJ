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

int syntakticka_anal();
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
