/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Main interpretu                    */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "precedencni_analyza.h"
#include "ilist.h"
//#include "syntakticka_analyza.h"

TItem* UNDEFPTR;

void htPrintTable() {
int maxlen = 0;
int sumcnt = 0;
printf ("------------HASH TABLE--------------\n");
for ( int i=0; i<HTSIZE; i++ ) {
printf ("%i:",i);
int cnt = 0;
TItem* ptr = (*ptrht)[i];
while ( ptr != NULL ) {
printf (" (%s)",ptr->key); // vytiskne klic
if( ptr->type == 0 ){ // string
printf(" %s", ptr->data.str);
}
else if( ptr->type == 1 ){ // bool
printf(" %d", ptr->data.boolValue);
}
else if( ptr->type == 2 ){ // int
printf(" %d", ptr->data.intNumber);
}
else if( ptr->type == 3 ){ // double
printf(" %f", ptr->data.floatNumber);
}
if ( ptr != UNDEFPTR )
cnt++;
ptr = ptr->ptrnext;
}
printf ("\n");
if (cnt > maxlen)
maxlen = cnt;
sumcnt+=cnt;
}
printf ("------------------------------------\n");
printf ("Items count %i The longest list %i\n",sumcnt,maxlen);
printf ("------------------------------------\n");
}

void printList () {
    while (listIntrukci.First != NULL) {

        printf("V listu je := %d\n", listIntrukci.First->instruction.instructionType);
        listIntrukci.First = listIntrukci.First->nextItem;
    }
}


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
    htInit();

    /** Seznam instrukci */
    InitList (&listIntrukci);

    int navrat;
    tokenInit(&token);
    token = getNextToken();
    navrat = precedencniSA();
    if (navrat == S_BEZ_CHYB)
        printf("Syntakticka analyza OK\n");
    else
        printf("Syntakticka analyza NO\n");

    TItem* tmp = htSearch("$3");
	if (tmp != NULL) {
		printf("TS KEY: %s\n", tmp->key);
        	printf("TS DATA.intNumber: %d\n", tmp->data.intNumber);
        	printf("TS TYPE: %d\n", tmp->type);
	}
	else {
		printf("neni v tabulce\n");
	}

	printList();

    //htPrintTable();

    /*int navrat;
    navrat = syntakticka_anal();
    if(navrat == S_BEZ_CHYB)
        printf("SYN OK\n");
    else
        printf("SYN NO\n");*/


    return 0;
}
