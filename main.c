/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Main interpretu                    */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ilist.h"
#include "precedencni_analyza.h"
#include "interpret.h"
#include "syntakticka_analyza.h"

void printdata( TItem *tmp ){
    if( tmp->type == TYPESTR ){
        fprintf(stderr, "retezec: %s\n", tmp->data->str );
    }
	else if( tmp->type == TYPEBOOL ){
        fprintf(stderr, "boolean: %s\n", tmp->data->boolValue ? "true" : "false");
    }
    else if( tmp->type == TYPEINT ){
        fprintf(stderr, "integer: %d\n", tmp->data->intNumber );
    }
    else if( tmp->type == TYPEDOUBLE ){
        fprintf(stderr, "double: %lf\n", tmp->data->floatNumber );
    }
}

void printList () {
    listIntrukci.Active = listIntrukci.First;

    while (listIntrukci.Active != NULL) {
        fprintf(stderr, "V listu je operace %d\n", listIntrukci.Active->instruction.instructionType);
        TItem *tmp = (TItem*)listIntrukci.Active->instruction.address1;
        if (tmp != NULL) {
            fprintf(stderr, "Operand 1\t");
            printdata(tmp);
        }
        tmp = (TItem*)listIntrukci.Active->instruction.address2;
        if (tmp != NULL){
            fprintf(stderr, "Operand 2\t");
            printdata(tmp);
        }
        /** Pristup k dalsimu prvku seznamu */
        listIntrukci.Active = listIntrukci.Active->nextItem;
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

// -----------------------------------------------------------------------------------------------------
    /** Test lexikalniho analyzatoru */
    /*tToken navrat;
    tokenInit(&token);
    int stav = s_start;
    while (stav != s_eof) {
        navrat = getNextToken();
        stav = navrat.stav;
        printf("%d:\t%s\n", navrat.stav, navrat.data);
    }*/
// -----------------------------------------------------------------------------------------------------

    /** Tabulky symbolu */
    htInit(&ptrhtLocal);
	htInit(&ptrhtGlobal);

    /** Seznam instrukci */
    InitList (&listIntrukci);



    /** Zkouska instrukce prirazeni */
    /*  1. operand co se ma priradit, 2. operand NULL,
        3. operand vysledek, tzn. kam se ma priradit */
    /*  Je potreba pridat do globalni tabulky polozku promenne,
        do ktere se bude prirazovat napr. vysledek vyrazu */

	TData *datavysl = malloc(sizeof(TData));
	htInsert(ptrhtGlobal, "vysledek", datavysl, TYPEDOUBLE, ID_GLOBAL);
    //htPrintTable(ptrhtGlobal);


    /** Test precedencni analyzy*/
    int navrat;
    tokenInit(&token);
    token = getNextToken();
    navrat = precedencniSA();
    if (navrat == S_BEZ_CHYB)
        fprintf(stderr, "Syntakticka analyza OK\n");
    else
        fprintf(stderr, "Syntakticka analyza NO\n");

    generateInstruction(OC_PRIRAZENI, htSearch(ptrhtLocal, neterminal.polozkaTS.key), NULL, htSearch(ptrhtGlobal, "vysledek"));

    tChyba navr_kod = interpret();
    fprintf(stderr, "Navratovy kod interpretu: %d\n", navr_kod);

    /** Vytisk seznamu instrukci */
    //printList();

    TItem *tmp = htSearch(ptrhtGlobal, "vysledek");
    if (tmp != NULL) {
        printdata(tmp);
    }


// -----------------------------------------------------------------------------------------------------
        /** Takto se pristupuje k vysledku vyrazu */
    /*TItem *vysledek = htSearch(ptrhtLocal, neterminal.polozkaTS.key);

    if (vysledek != NULL) {
        if (vysledek->type == TYPEINT) {
                printf("Vysledek typu int %d\n", vysledek->data->intNumber);
            }
        else if (vysledek->type == TYPEBOOL) {
            printf("Vysledek typu boolean %s\n", vysledek->data->boolValue ? "true" : "false");
        }
        else if (vysledek->type == TYPEDOUBLE) {
            printf("Vysledek typu double %lf\n", vysledek->data->floatNumber);
        }
    }*/
// -----------------------------------------------------------------------------------------------------

    /* Syntakticka analyza
    int navrat;
    navrat = syntakticka_anal();
    if(navrat == S_BEZ_CHYB)
        fprintf(stderr, "SYN OK\n");
    else
        fprintf(stderr, "SYN NO\n");

	fprintf(stderr, "Navratova hodnota parseru: %d\n", navrat);*/


    return 0;
}
