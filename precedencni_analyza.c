/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Precedencni syntakticka analyza    */
/**  Autor: Martin Josef�k              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "precedencni_analyza.h"
#include "ilist.h"
#include "mystring.h"
#include "frame.h"

#define VELIKOST_TABULKY 14

tZahlavi precedencniTabulka [VELIKOST_TABULKY][VELIKOST_TABULKY] = {
    /**         +   -   *   /   (   ID  )   <   >   <=  >=  ==  !=  $   */
    /** +  */  {V,  V,  M,  M,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** -  */  {V,  V,  M,  M,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** *  */  {V,  V,  V,  V,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** /  */  {V,  V,  V,  V,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** (  */  {M,  M,  M,  M,  M,  M,  R,  M,  M,  M,  M,  M,  M,  E},
    /** ID */  {V,  V,  V,  V,  E,  E,  V,  V,  V,  V,  V,  V,  V,  V},
    /** )  */  {V,  V,  V,  V,  E,  E,  V,  V,  V,  V,  V,  V,  V,  V},
    /** <  */  {M,  M,  M,  M,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** >  */  {M,  M,  M,  M,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** <= */  {M,  M,  M,  M,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** >= */  {M,  M,  M,  M,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** == */  {M,  M,  M,  M,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** != */  {M,  M,  M,  M,  M,  M,  V,  V,  V,  V,  V,  V,  V,  V},
    /** $  */  {M,  M,  M,  M,  M,  M,  E,  M,  M,  M,  M,  M,  M,  E},
};

/** Globalni promenne */
int counterVar = 1;
tToken newVar;

/** Generuje jedinecne nazvy identifikatoru. Nazev se sklada ze znaku $, ktery nasleduje cislo.
    Postupne se tu generuji prirozena cisla a do nazvu promenne se ukladaji v reverzovanem poradi. */
void generateVariable(tToken *var) {
    tokenClear(var);
    tokenEdit(var, '$');
    int i;
    i = counterVar;
    while (i != 0) {
        tokenEdit(var, (char)(i % 10 + '0'));
        i = i / 10;
    }
    counterVar ++;
}

/** Funkce vlozi instrukci do seznamu instrukci */
void generateCopyInstr( int typInstrukce, void *addr1, void *addr2, void *addr3) {
   tInst I;
   I.instructionType = typInstrukce;
   I.address1 = addr1;
   I.address2 = addr2;
   I.address3 = addr3;
   InsertFirst(I);
}

/** Funkce vlozi instrukci do seznamu instrukci */
void generateInstruction( int typInstrukce, void *addr1, void *addr2, void *addr3) {
   tInst I;
   I.instructionType = typInstrukce;
   I.address1 = addr1;
   I.address2 = addr2;
   I.address3 = addr3;
   InsertLast(I);
}

void zasobnikInit(tZasobnik *zasobnik) {
    zasobnik->vrchol = NULL;
}

bool zasobnikEmpty(tZasobnik *zasobnik) {
    if (zasobnik->vrchol == NULL)
        return true;
    return false;
}

int zasobnikPush(tZasobnik *zasobnik, tData polozka) {
    tPolozka *tmp;

    if ((tmp = malloc(sizeof(tPolozka))) == NULL) {
        return S_INTERNI_CHYBA;
    }
    else {
        tmp->data = polozka;
        tmp->dalsi = zasobnik->vrchol;      /** Dalsi prvek bude puvodni vrchol zasobniku */
        zasobnik->vrchol = tmp;             /** Novou polozku vkladame na vrchol zasobniku */
    }
    return 0;
}

void zasobnikPop(tZasobnik *zasobnik) {
    tPolozka *tmp;

    if (!zasobnikEmpty(zasobnik)) {
        tmp = zasobnik->vrchol;
        zasobnik->vrchol = zasobnik->vrchol->dalsi;
        free(tmp);
    }
}

void zasobnikPrectiVrchol(tZasobnik *zasobnik, tData *data) {
    if (!zasobnikEmpty(zasobnik)) {
        *data = zasobnik->vrchol->data;
    }
}

void presypZasobnikyPoTerminal(tZasobnik *zasobnik1, tZasobnik *zasobnik2) {
    while (zasobnik1->vrchol->data.symbol == NETERMINAL) {
        tData presyp;
        zasobnikPrectiVrchol(zasobnik1, &presyp);
        zasobnikPop(zasobnik1);
        zasobnikPush(zasobnik2, presyp);
    }
}

void presypZasobnikZpet(tZasobnik *zasobnik2, tZasobnik *zasobnik1) {
    while (zasobnik2->vrchol != NULL) {
        tData presyp;
        zasobnikPrectiVrchol(zasobnik2, &presyp);
        zasobnikPop(zasobnik2);
        zasobnikPush(zasobnik1, presyp);
    }
}

tChyba prevedToken(tToken token, tData *prevedenyToken) {
    if (token.stav == s_plus)
        prevedenyToken->symbol = PLUS;
    else if (token.stav == s_minus)
        prevedenyToken->symbol = MINUS;
    else if (token.stav == s_krat)
        prevedenyToken->symbol = KRAT;
    else if (token.stav == s_deleno)
        prevedenyToken->symbol = DELENO;
    else if (token.stav == s_leva_zavorka)
        prevedenyToken->symbol = LZAVORKA;
    else if (token.stav == s_identifikator) {

        /** kontrola, jestli se identifikator nachazi v tabulce
            symbolu, jestli ne tak semanticka chyba */
        TItem *tmp;
        if ((tmp = searchFrames(token.data, ptrhtLocal, ptrhtGlobal)) == NULL) {
            fprintf(stderr, "Nedeklarovana promenna\n");
            return S_SEMANTICKA_CHYBA_NEDEF;
        } else {
            //prevedenyToken->polozkaTS.key = allocString(tmp->key);
            //prevedenyToken->polozkaTS.data = tmp->data;
            prevedenyToken->symbol = ID;
            prevedenyToken->polozkaTS.key = tmp->key;
            prevedenyToken->polozkaTS.type = tmp->type;
            prevedenyToken->polozkaTS.data = tmp->data;
            prevedenyToken->polozkaTS.ptrnext = tmp->ptrnext;
        }
    }
    else if (token.stav == s_prava_zavorka)
        prevedenyToken->symbol = PZAVORKA;
    else if (token.stav == s_mensi)
        prevedenyToken->symbol = MENSI;
    else if (token.stav == s_vetsi)
        prevedenyToken->symbol = VETSI;
    else if (token.stav == s_mensi_rovno)
        prevedenyToken->symbol = MENSIROVNO;
    else if (token.stav == s_vetsi_rovno)
        prevedenyToken->symbol = VETSIROVNO;
    else if (token.stav == s_rovno)
        prevedenyToken->symbol = ROVNO;
    else if (token.stav == s_nerovno)
        prevedenyToken->symbol = NEROVNO;
    else if (token.stav == s_logicka_hodnota) {
    // **************************************************************************************
        generateVariable(&newVar);

        prevedenyToken->polozkaTS.key = malloc(sizeof(char)*16);    /** Alokace pameti pro klic */
        strcpy(prevedenyToken->polozkaTS.key, newVar.data);
        prevedenyToken->symbol = ID;
        prevedenyToken->polozkaTS.type = TYPEBOOL;
        prevedenyToken->polozkaTS.druh = 0;
        prevedenyToken->polozkaTS.data = (TData*)malloc( sizeof(TData) );
        if (!strcmp(token.data, "true"))
            prevedenyToken->polozkaTS.data->boolValue = true;
        else
            prevedenyToken->polozkaTS.data->boolValue = false;

        /** Existenci v TS resit nemusime, protoze promenna ma unikatni nazev */
        htInsert(ptrhtLocal, prevedenyToken->polozkaTS.key, prevedenyToken->polozkaTS.data, prevedenyToken->polozkaTS.type, prevedenyToken->polozkaTS.druh);
    }
    else if (token.stav == s_string) {
    // **************************************************************************************
        generateVariable(&newVar);

        prevedenyToken->polozkaTS.key = malloc(sizeof(char)*16);
        strcpy(prevedenyToken->polozkaTS.key, newVar.data);
        prevedenyToken->symbol = ID;
        prevedenyToken->polozkaTS.type = TYPESTR;
        prevedenyToken->polozkaTS.druh = 0;
        prevedenyToken->polozkaTS.data = (TData*)malloc(sizeof(TData));
        prevedenyToken->polozkaTS.data->str = allocString(token.data);
        htInsert(ptrhtLocal, prevedenyToken->polozkaTS.key, prevedenyToken->polozkaTS.data, prevedenyToken->polozkaTS.type, prevedenyToken->polozkaTS.druh);
    }
    else if (token.stav == s_cele_cislo) {
	// **************************************************************************************
        generateVariable(&newVar);

        /** Naplneni struktury pro tabulku symbolu */
        prevedenyToken->polozkaTS.key = malloc(sizeof(char)*16);    /** Alokace pameti pro klic */
        strcpy(prevedenyToken->polozkaTS.key, newVar.data);
        prevedenyToken->symbol = ID;
        prevedenyToken->polozkaTS.type = TYPEINT;
        prevedenyToken->polozkaTS.druh = 0;
        prevedenyToken->polozkaTS.data = (TData*)malloc(sizeof(TData));
        prevedenyToken->polozkaTS.data->intNumber = atoi(token.data);
        htInsert(ptrhtLocal, prevedenyToken->polozkaTS.key, prevedenyToken->polozkaTS.data, prevedenyToken->polozkaTS.type, prevedenyToken->polozkaTS.druh);
	}
    else if (token.stav == s_desetinne_cislo) {
	// **************************************************************************************
        generateVariable(&newVar);

        prevedenyToken->polozkaTS.key = malloc(sizeof(char)*16);    /** Alokace pameti pro klic */
        strcpy(prevedenyToken->polozkaTS.key, newVar.data);
        prevedenyToken->symbol = ID;
        prevedenyToken->polozkaTS.type = TYPEDOUBLE;
        prevedenyToken->polozkaTS.druh = 0;
        prevedenyToken->polozkaTS.data = (TData*)malloc(sizeof(TData));
        prevedenyToken->polozkaTS.data->floatNumber = atof(token.data);
        htInsert(ptrhtLocal, prevedenyToken->polozkaTS.key, prevedenyToken->polozkaTS.data, prevedenyToken->polozkaTS.type, prevedenyToken->polozkaTS.druh);
	}
    else if (token.stav == s_strednik)
        prevedenyToken->symbol = DOLAR;
    else if (token.stav == s_klicove && (!strcmp(token.data, "then") || !strcmp(token.data, "do") || !strcmp(token.data, "end")))
        prevedenyToken->symbol = DOLAR;
    else {
        prevedenyToken->symbol = CHYBA;
        return S_SYNTAKTICKA_CHYBA;
    }
    return S_BEZ_CHYB;
}

TOpCode zjistiOperator (int operace) {
    if (operace == PLUS) {
        return OC_ADD;
    }
    else if (operace == MINUS) {
        return OC_SUB;
    }
    else if (operace == KRAT) {
        return OC_MUL;
    }
    else if (operace == DELENO) {
        return OC_DIV;
    }
    else if (operace == MENSI) {
        return OC_MENSI;
    }
    else if (operace == VETSI) {
        return OC_VETSI;
    }
    else if (operace == MENSIROVNO) {
        return OC_MENSI_ROVNO;
    }
    else if (operace == VETSIROVNO) {
        return OC_VETSI_ROVNO;
    }
    else if (operace == ROVNO) {
        return OC_ROVNO;
    }
    else if (operace == NEROVNO) {
        return OC_NEROVNO;
    }
    return S_SYNTAKTICKA_CHYBA;
}

tChyba redukuj(tZasobnik *zasobnik1, tZasobnik *zasobnik2) {
    tData presyp;               /** Pomocna promenna na presunuti terminalu mezi zasobniky */
    int operace;
    tData hledameZarazku;       /** Potrebujeme najit na zasobniku < */
    zasobnikPrectiVrchol(zasobnik1, &hledameZarazku);

    /** Dokud nenarazime na zarazku, tak budem pressouvat term. i neterm. na druhy zasobnik */
    while ((hledameZarazku.symbol != ZARAZKA) && (!zasobnikEmpty(zasobnik1))) {
        zasobnikPrectiVrchol(zasobnik1, &presyp);
        zasobnikPush(zasobnik2, presyp);
        zasobnikPop(zasobnik1);
        zasobnikPrectiVrchol(zasobnik1, &hledameZarazku);   /** Precteme dalsi prvek */
    }
    /** Nasli jsme zarazku - je na vrcholu zasobniku */

    if (hledameZarazku.symbol == ZARAZKA) {
        tData prectiTerminal, prectiTerminal2, prectiTerminal3;     /** Pomocne promenne, do kterych nacteme jednotlive symboly z pomocneho zasobniku */
        zasobnikPrectiVrchol(zasobnik2, &prectiTerminal);           /** Precteme prvek z vrcholu zasobniku a nasledne jej vyjmeme */
        zasobnikPop(zasobnik2);

        /** pokud je terminal ID, tak redukujeme podle pravidla E -> i */
        if (prectiTerminal.symbol == ID) {
            zasobnikPop(zasobnik1);                                 /** Musime odstranit zarazku z prvniho zasobniku */

            neterminal = prectiTerminal;
            neterminal.symbol = NETERMINAL;                         /** Prepiseme na neterminal */

            /** Po redukci vlozime neterminal E na prvni zasobnik */
            zasobnikPush(zasobnik1, neterminal);
            return S_BEZ_CHYB;
        }

           /** Pravidlo E -> (E) */
        else if (prectiTerminal.symbol == LZAVORKA) {
            if (!zasobnikEmpty(zasobnik2)) {
                zasobnikPrectiVrchol(zasobnik2, &prectiTerminal2);
                zasobnikPop(zasobnik2);
            }
            else {
                /** Zasobnik je prazdny a byla tam jen leva zavorka...wtf? */
                fprintf(stderr, "Zavorka navic\n");
                return S_SYNTAKTICKA_CHYBA;
            }
                /** Po nacteni ze zasobniku musi byt dalsi v poradi E */
            if (prectiTerminal2.symbol == NETERMINAL) {
                if (!zasobnikEmpty(zasobnik2)) {        /** Nacteme dalsi terminal, tentokrat potrebujeme pravou zavorku */
                    zasobnikPrectiVrchol(zasobnik2, &prectiTerminal3);
                    zasobnikPop(zasobnik2);
                }
                else {
                    /** Zasobnik je prazdny, takze chybi prava zavorka */
                    fprintf(stderr, "Chybi prava zavorka\n");
                    return S_SYNTAKTICKA_CHYBA;
                }

                if (prectiTerminal3.symbol == PZAVORKA) {
                    if (zasobnikEmpty(zasobnik2)) {
                        zasobnikPop(zasobnik1);             /** Odstraneni < zarazky */

                        neterminal.symbol = NETERMINAL;

                        /** Po redukci vlozime neterminal E do prvniho zasobniku */
                        zasobnikPush(zasobnik1, neterminal);
                        return S_BEZ_CHYB;
                    }
                    else {
                        fprintf(stderr, "Za (E) jsou dalsi polozky\n");
                        return S_SYNTAKTICKA_CHYBA;
                    }
                }
                else {
                    fprintf(stderr, "Chybi prava zavorka\n");
                    return S_SYNTAKTICKA_CHYBA;
                }
            }
            else {
                fprintf(stderr, "Chybi E\n");
                return S_SYNTAKTICKA_CHYBA;
            }
        }

            /** Pravidlo E -> E operator E */
        else if (prectiTerminal.symbol == NETERMINAL) {
            if(!zasobnikEmpty(zasobnik2)) {
                zasobnikPrectiVrchol(zasobnik2, &prectiTerminal2);
                zasobnikPop(zasobnik2);
            }
            else {
                fprintf(stderr, "Chybi operator\n");
                return S_SYNTAKTICKA_CHYBA;
            }

                /** Za neterminalem E je operator */
            if ((prectiTerminal2.symbol == PLUS) || (prectiTerminal2.symbol== MINUS) || (prectiTerminal2.symbol == KRAT) || (prectiTerminal2.symbol == DELENO) ||
                    (prectiTerminal2.symbol == MENSI) || (prectiTerminal2.symbol == VETSI) || (prectiTerminal2.symbol == MENSIROVNO) ||
                    (prectiTerminal2.symbol == VETSIROVNO) || (prectiTerminal2.symbol == ROVNO) || (prectiTerminal2.symbol == NEROVNO)) {

                    /** Zjisteni operatoru */
                operace = zjistiOperator(prectiTerminal2.symbol);
            }
            else {
                fprintf(stderr, "Spatny operator\n");
                return S_SYNTAKTICKA_CHYBA;
            }

                /** Jako posledni v tomto pravidle musi byt znovu neterminal */
            if (!zasobnikEmpty(zasobnik2)) {
                zasobnikPrectiVrchol(zasobnik2, &prectiTerminal3);
                zasobnikPop(zasobnik2);
            }
            else {
                fprintf(stderr, "Chybi E\n");
                return S_SYNTAKTICKA_CHYBA;
            }

            if (prectiTerminal3.symbol == NETERMINAL) {
                if (zasobnikEmpty(zasobnik2)) {
                    zasobnikPop(zasobnik1);     // odstraneni < zarazky

                        /** Vygenerovani instrukce pro aritmeticke a relacni operatory */
                    if ((operace == OC_ADD) || (operace == OC_SUB) || (operace == OC_MUL) || (operace == OC_DIV) ||
                        (operace == OC_MENSI) || (operace == OC_VETSI) || (operace == OC_MENSI_ROVNO) ||
                        (operace == OC_VETSI_ROVNO) || (operace == OC_ROVNO) || (operace == OC_NEROVNO)) {

                        generateVariable(&newVar);
                        neterminal.polozkaTS.key = allocString(newVar.data);
                        neterminal.symbol = NETERMINAL;

                        zasobnikPush(zasobnik1, neterminal);

                        /** Vlozeni do TS */
                        htInsert(ptrhtLocal, neterminal.polozkaTS.key, copyData(neterminal.polozkaTS.type, neterminal.polozkaTS.data),
                                             neterminal.polozkaTS.type, neterminal.polozkaTS.druh);

                        /** Vlozeni instrukce do seznamu */
                        generateInstruction(operace, searchFrames(prectiTerminal.polozkaTS.key, ptrhtLocal, ptrhtGlobal),
                                                     searchFrames(prectiTerminal3.polozkaTS.key, ptrhtLocal, ptrhtGlobal),
                                                     searchFrames(neterminal.polozkaTS.key, ptrhtLocal, ptrhtGlobal));
                        return S_BEZ_CHYB;
                    }
                }
                else {
                    fprintf(stderr, "Za E operator E jsou dalsi polozky\n");
                    return S_SYNTAKTICKA_CHYBA;
                }
            }
        }
    }
    else {
        fprintf(stderr,"Na zasobniku chybi <\n");
        return S_SYNTAKTICKA_CHYBA;
    }
    return 0;
}

tChyba precedencniSA() {
    bool precti = true;
    tChyba navrat;
    tData dolar;
    tPrecTabulka akce;
    tData prevedenyToken = {.symbol = 0};
    tData nejvrchTermSymbol;
    tData zarazka;
    zarazka.symbol = ZARAZKA;

    /** Inicializace globalni promenne, do ktere se budou generovat unikatni nazvy promennych */
    tokenInit(&newVar);

    /** Vytvoreni a inicializace hlavniho a pomocneho zasobniku */
    tZasobnik zasobnik1, zasobnik2;
    zasobnikInit(&zasobnik1);
    zasobnikInit(&zasobnik2);

    /** Vloz na zasobnik symbol $ */
    dolar.symbol = DOLAR;
    zasobnikPush(&zasobnik1, dolar);

    /** Dokud aktualni vstupni symbol a nejvrchnejsi terminalni symbol neni dolar, proved dalsi smycku */
    while (1) {

        /** Prvni token dostaneme od SA rekurzivniho sestupu */
        if (precti) {
            navrat = prevedToken(token, &prevedenyToken);
            if (navrat != S_BEZ_CHYB) {
                return navrat;
            }
        }

        /** Je taky potreba najit nejvrchnejsi terminalni symbol na zasobniku: b */
        presypZasobnikyPoTerminal(&zasobnik1, &zasobnik2);
        zasobnikPrectiVrchol(&zasobnik1, &nejvrchTermSymbol);
        presypZasobnikZpet(&zasobnik2, &zasobnik1);

        /** Podle obsahu policka precedencni tabulky na souradnicich [b, a] se rozhodne */
        akce = precedencniTabulka[nejvrchTermSymbol.symbol][prevedenyToken.symbol];

        /** Po dokonceni syntakticke analyzy vstupniho retezce opoustime cyklus */
        if ((prevedenyToken.symbol == DOLAR) && (nejvrchTermSymbol.symbol == DOLAR)) {
            break;
        }

        switch (akce) {

        case R:
            zasobnikPush(&zasobnik1, prevedenyToken);
            precti = true;
            break;

        case M:
            presypZasobnikyPoTerminal(&zasobnik1, &zasobnik2);
            zasobnikPush(&zasobnik1, zarazka);              /** Vlozime < na zasobnik */
            presypZasobnikZpet(&zasobnik2, &zasobnik1);
            /** Precteme token ze vstupu a dame na vrchol zasobniku */
            zasobnikPush(&zasobnik1, prevedenyToken);
            precti = true;
            break;

        case V:
            navrat = redukuj(&zasobnik1, &zasobnik2);
            precti = false;
            if (navrat == S_SYNTAKTICKA_CHYBA)
                return S_SYNTAKTICKA_CHYBA;
            break;

        case E:
            fprintf(stderr, "Prazdne policko v precedencni tabulce -> syntakticka chyba\n");
            /** Uvolnime zbytek co zustal na zasobnicich */
            while (!zasobnikEmpty(&zasobnik1))
                zasobnikPop(&zasobnik1);
            while (!zasobnikEmpty(&zasobnik2))
                zasobnikPop(&zasobnik2);
            return S_SYNTAKTICKA_CHYBA;
        }

        /** Nacteme token a prevedeme na terminalni symbol: a */
        if (precti) {
            token = getNextToken();
        }
    }

    while (!zasobnikEmpty(&zasobnik1))
        zasobnikPop(&zasobnik1);
    while (!zasobnikEmpty(&zasobnik2))
        zasobnikPop(&zasobnik2);

    /** Uvolneni nagenerovane promenne */
    tokenFree(&newVar);

    return S_BEZ_CHYB;
}

void vypisZasobnik(tZasobnik zasobnik) {
    tZasobnik tmp = zasobnik;
    printf("\n----------------\n");
    while (tmp.vrchol != NULL) {
        printf("%d\n", tmp.vrchol->data.symbol);
        tmp.vrchol = tmp.vrchol->dalsi;
    }
}

void vypisPrecedencniTabulku() {
    int i,j;
    for (i = 0; i < VELIKOST_TABULKY; i++) {
        for (j = 0; j < VELIKOST_TABULKY; j++) {
            printf("%d ", precedencniTabulka[i][j]);
        }
        printf("\n");
    }
}
