/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Main interpretu                    */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/


#include <stdio.h>
#include <stdlib.h>
#include "syntakticka_analyza.h"


/*void htPrintTable( tHTable *ptrht ) {
int maxlen = 0;
int sumcnt = 0;
printf ("------------HASH TABLE--------------\n");
for ( int i=0; i<HTSIZE; i++ ) {
printf ("%i:",i);
int cnt = 0;
TItem* ptr = (*ptrht)[i];
while ( ptr != NULL ) {
printf (" (%s)",ptr->key); // vytiskne klic
if( ptr->druh == 5 ){ // string
printf(" %d ", ptr->druh);

for (int i = 0; i < ptr->data.param.numParam; i++){
		printf("%s ", ptr->data.param.param[i]);
		
	}
}
else if( ptr->druh == 6 ){ // bool
printf(" %d", ptr->druh);
}
else if( ptr->druh == 7 ){ // int
printf(" %d", ptr->druh);
}
else if( ptr->druh == 8 ){ // double

printf(" %d", ptr->druh);
}
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
}*/


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
    

    /*int navrat;
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
	}*/

   

    int navrat;
    navrat = syntakticka_anal();
    htPrintTable(ptrhtGlobal);
    htPrintTable(func[0].table);
    htPrintTable(ptrhtLastLocal);
    htPrintTable(ptrTables[0].table);
    if(navrat == S_BEZ_CHYB)
        printf("SYN OK\n");
    else
        printf("SYN NO\n %d", navrat);
    //htPrintTable( ptrhtGlobal );

    return 0;
}
