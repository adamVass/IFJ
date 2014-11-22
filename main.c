/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Main interpretu                    */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "precedencni_analyza.h"
#include "navratova_hodnota.h"
#include "syntakticka_analyza.h"

int main (int argc, char *argv[]) {

    /** Osetreni vstupnich parametru */
    if (argc != 2) {
        fprintf(stderr,"Byly spatne zadany parametry.\n");
        return S_INTERNI_CHYBA;
    }
    /** Otevreni zdrojoveho souboru */
    FILE *f;
    if ((f = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Soubor nejde otevrit\n");
        return S_INTERNI_CHYBA;
    }
	setSourceFile(f);

    /** Test lexikalniho analyzatoru */
    /*tToken navrat;
    int stav = s_start;
    while (stav != s_eof) {
        navrat = getNextToken();
        stav = navrat.stav;
        printf("%d:\t%s\n", navrat.stav, navrat.data);
    }*/

    /** Tabulka symbolu */
    tHTable* ptrht;
    htInit(ptrht);

    int navrat;
    tokenInit(&token);
    token = getNextToken();
    navrat = precedencniSA();
    if (navrat == S_BEZ_CHYB)
        printf("Syntakticka analyza OK\n");
    else
        printf("Syntakticka analyza NO\n");


    /*int navrat;
    navrat = syntakticka_anal();
    if(navrat == S_BEZ_CHYB)
        printf("SYN OK\n");
    else
        printf("SYN NO\n");*/


    return 0;
}
