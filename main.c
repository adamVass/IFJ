/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Main interpretu                    */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "syntakticka_analyza.h"
#include "interpret.h"
#include "navratova_hodnota.h"

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

    /** Provedeni syntakticke analyzy */
    int navrat;
    navrat = syntakticka_anal();

    /** Pokud vse probehne v poradku, tak se zavola interpret */
    if(navrat == S_BEZ_CHYB)
        navrat = interpret();
    else {
        fprintf(stderr, "Navrat syntakticke: %d\n", navrat);
        return navrat;
    }

    fprintf(stderr, "Navrat interpretu: %d\n", navrat);
    return navrat;
}
