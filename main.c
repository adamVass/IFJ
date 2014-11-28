/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Main interpretu                    */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ilist.h"
#include "syntakticka_analyza.h"



void printdata( TItem *tmp ){
    if( tmp->type == 0 ){
        printf("retezec: %s\n", tmp->data.str );
    }
	else if( tmp->type == 1 ){
        printf("boolean: %s\n", tmp->data.boolValue ? "true" : "false");
    }
    else if( tmp->type == 2 ){
        printf("integer: %d\n", tmp->data.intNumber );
    }
    else if( tmp->type == 3 ){
        printf("double: %lf\n", tmp->data.floatNumber );
    }
}

void printList () {
    while (listIntrukci.First != NULL) {

        printf("V listu je operace %d\n", listIntrukci.First->instruction.instructionType);
        printf("Operand 1\n");
        TItem *tmp = (TItem*)listIntrukci.First->instruction.address1;
        printdata(tmp);
        printf("Operand 2\n");
        tmp = (TItem*)listIntrukci.First->instruction.address2;
        printdata(tmp);
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
    //htInit();

    /** Seznam instrukci */
    InitList (&listIntrukci);

    /*int navrat;
    tokenInit(&token);
    token = getNextToken();
    navrat = precedencniSA();
    if (navrat == S_BEZ_CHYB)
        printf("Syntakticka analyza OK\n");
    else
        printf("Syntakticka analyza NO\n");*/

    //printList();

    /*while (listIntrukci.First != NULL) {

        TItem *tmp1 = (TItem*)listIntrukci.First->instruction.address1;
        TItem *tmp2 = (TItem*)listIntrukci.First->instruction.address2;
        TItem *tmp3 = (TItem*)listIntrukci.First->instruction.address3;


        switch(listIntrukci.First->instruction.instructionType) {

            case OC_ADD:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->data.intNumber = tmp1->data.intNumber + tmp2->data.intNumber;
                    printf("Scitani: operand1 %d, operand2 %d\n", tmp1->data.intNumber, tmp2->data.intNumber);
                }
                break;

            case OC_SUB:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->data.intNumber = tmp1->data.intNumber - tmp2->data.intNumber;
                    printf("Odcitani: operand1 %d, operand2 %d\n", tmp1->data.intNumber, tmp2->data.intNumber);
                }
                break;

            case OC_MUL:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->data.intNumber = tmp1->data.intNumber * tmp2->data.intNumber;
                    printf("Nasobeni: operand1 %d, operand2 %d\n", tmp1->data.intNumber, tmp2->data.intNumber);
                }
                break;

            case OC_DIV:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data.floatNumber = (double) tmp1->data.intNumber / (double) tmp2->data.intNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data.floatNumber = (double) tmp1->data.floatNumber / (double) tmp2->data.floatNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data.floatNumber = (double) tmp1->data.floatNumber / (double) tmp2->data.intNumber;
                }
                else if (tmp1->type == TYPEINT && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data.floatNumber = (double) tmp1->data.intNumber / (double) tmp2->data.floatNumber;
                }
                break;
        }*/

        /** Posun na dalsi instrukci */
        /*listIntrukci.First = listIntrukci.First->nextItem;
    }*/

        /** Takto se pristupuje k vysledku vyrazu */
        /*TItem *vysledek = htSearch(neterminal.polozkaTS.key);

        if (vysledek->type == TYPEINT) {
            printf("Vysledek typu int %d\n", vysledek->data.intNumber);
        }
        else if (vysledek->type == TYPEBOOL) {
            printf("Vysledek typu boolean %d\n", vysledek->data.boolValue);
        }
        else if (vysledek->type == TYPEDOUBLE) {
            printf("Vysledek typu double %lf\n", vysledek->data.floatNumber);
        }*/

    //htPrintTable();

    int navrat;
    navrat = syntakticka_anal();
    if(navrat == S_BEZ_CHYB)
        printf("SYN OK\n");
    else
        printf("SYN NO\n");


    return 0;
}
