/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Main interpretu                    */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "precedencni_analyza.h"
#include "navratova_hodnota.h"

#define INTERNI_CHYBA 99

int main (int argc, char *argv[]) {

    /** Osetreni vstupnich parametru */
    if (argc != 2) {
        fprintf(stderr,"Byly spatne zadany parametry.\n");
        return INTERNI_CHYBA;
    }
    /** Otevreni zdrojoveho souboru */
    FILE *f;
    if ((f = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Soubor nejde otevrit\n");
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

    int navrat;
    navrat = precedencniSA();
    if (navrat == S_BEZ_CHYB)
        printf("Syntakticka analyza OK\n");
    else
        printf("Syntakticka analyza NO\n");

    return 0;
}
