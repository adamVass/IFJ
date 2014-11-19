/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Precedencni syntakticka analyza    */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "navratova_hodnota.h"
#include "precedencni_analyza.h"

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

int prevedToken(tToken token) {
    if (token.stav == s_plus)
        return PLUS;
    else if (token.stav == s_minus)
        return MINUS;
    else if (token.stav == s_krat)
        return KRAT;
    else if (token.stav == s_deleno)
        return DELENO;
    else if (token.stav == s_leva_zavorka)
        return LZAVORKA;
    else if (token.stav == s_identifikator)
        return ID;
    else if (token.stav == s_prava_zavorka)
        return PZAVORKA;
    else if (token.stav == s_mensi)
        return MENSI;
    else if (token.stav == s_vetsi)
        return VETSI;
    else if (token.stav == s_mensi_rovno)
        return MENSIROVNO;
    else if (token.stav == s_vetsi_rovno)
        return VETSIROVNO;
    else if (token.stav == s_rovno)
        return ROVNO;
    else if (token.stav == s_nerovno)
        return NEROVNO;
    else if (token.stav == s_cele_cislo)
        return ID;
    else if (token.stav == s_desetinne_cislo)
        return ID;
    else if (token.stav == s_strednik)
        return DOLAR;
    else if (token.stav == s_klicove && (!strcmp(token.data, "then") || !strcmp(token.data, "do")))
        return DOLAR;
    //pridat jeste klicova slova, jako true a false apod.
    else
        return CHYBA;
}

int redukuj(tZasobnik *zasobnik1, tZasobnik *zasobnik2) {
    tData presyp;               /** Pomocna promenna na presunuti terminalu mezi zasobniky */
    tData neterminal;           /** Napr. E, budeme na nej redukovat */
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
            neterminal = prectiTerminal;                            // bude obsahovat informace o (zatim tokenu) pozdeji hodnotach apod
            neterminal.symbol = NETERMINAL;                         /** Prepiseme na neterminal */
            zasobnikPush(zasobnik1, neterminal);                    /** Po redukci vlozime neterminal E na prvni zasobnik */
            return S_BEZ_CHYB;
        }

        /** Pravidlo E -> E operator E */
        else if (prectiTerminal.symbol == NETERMINAL) {
            if(!zasobnikEmpty(zasobnik2)) {
                zasobnikPrectiVrchol(zasobnik2, &prectiTerminal2);
                zasobnikPop(zasobnik2);

                if ((prectiTerminal2.symbol == PLUS) || (prectiTerminal2.symbol== MINUS) || (prectiTerminal2.symbol == KRAT) || (prectiTerminal2.symbol == DELENO) ||
                        (prectiTerminal2.symbol == MENSI) || (prectiTerminal2.symbol == VETSI) || (prectiTerminal2.symbol == MENSIROVNO) ||
                        (prectiTerminal2.symbol == VETSIROVNO) || (prectiTerminal2.symbol == ROVNO) || (prectiTerminal2.symbol == NEROVNO)) {
                    if (!zasobnikEmpty(zasobnik2)) {
                        zasobnikPrectiVrchol(zasobnik2, &prectiTerminal3);
                        zasobnikPop(zasobnik2);
                    }
                    else {
                        fprintf(stderr, "Chybi E\n");
                        return S_SYNTAKTICKA_CHYBA;
                    }
                        //rozdelit na operatory aritmeticke, porovnavaci, konkatenace -> jine semanticke akce
                    if ((prectiTerminal2.symbol == PLUS) || (prectiTerminal2.symbol == MINUS) || (prectiTerminal2.symbol == KRAT) || (prectiTerminal2.symbol == DELENO)) {
                        if (prectiTerminal3.symbol == NETERMINAL) {
                            if (zasobnikEmpty(zasobnik2)) {
                                zasobnikPop(zasobnik1);     // odstraneni < zarazky
                                neterminal = prectiTerminal3;
                                neterminal.symbol = NETERMINAL;
                                zasobnikPush(zasobnik1, neterminal);
                                return S_BEZ_CHYB;
                            }
                            else {
                                fprintf(stderr, "Za E operator E jsou dalsi polozky\n");
                                return S_SYNTAKTICKA_CHYBA;
                            }
                        }
                        // neni neterminal -> osetrit
                    }
                    else if ((prectiTerminal2.symbol == MENSI) || (prectiTerminal2.symbol == VETSI) || (prectiTerminal2.symbol == MENSIROVNO) ||
                            (prectiTerminal2.symbol == VETSIROVNO) || (prectiTerminal2.symbol == ROVNO) || (prectiTerminal2.symbol == NEROVNO)) {
                        if (prectiTerminal3.symbol == NETERMINAL) {
                            if (zasobnikEmpty(zasobnik2)) {
                                zasobnikPop(zasobnik1);             /** Odstraneni < zarazky */
                                neterminal = prectiTerminal3;
                                neterminal.symbol = NETERMINAL;
                                zasobnikPush(zasobnik1, neterminal);
                                return S_BEZ_CHYB;
                            }
                            else {
                                fprintf(stderr, "Za E operator E jsou dalsi polozky\n");
                                return S_SYNTAKTICKA_CHYBA;
                            }
                        }
                        // neni neterminal -> osetrit
                    }
                    // spatny operator? -> osetrit
                }
                else {
                    fprintf(stderr, "Spatny operator\n");
                    return S_SYNTAKTICKA_CHYBA;
                }
            }
            else {
                fprintf(stderr, "Chybi operator\n");
                return S_SYNTAKTICKA_CHYBA;
            }
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
                        neterminal = prectiTerminal3;       // k tomuto se jeste vratit
                        neterminal.symbol = NETERMINAL;
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
        else {
            fprintf(stderr, "Na zasobniku jsou polozky, ktere tam nemaji co delat\n");
            return S_SYNTAKTICKA_CHYBA;
        }
    }
    else {
        fprintf(stderr,"Na zasobniku chybi <\n");
        return S_SYNTAKTICKA_CHYBA;
    }
    return 0;
}

int precedencniSA() {
    int precti = 1;
    tChyba navrat;
    tData dolar;
    tPrecTabulka akce;
    tData pomocna;
    tToken token;
    tData terminal;
    int prevedenyToken;
    tData nejvrchTermSymbol;
    tData zarazka;
    zarazka.symbol = ZARAZKA;

    /** Vytvoreni a inicializace hlavniho a pomocneho zasobniku */
    tZasobnik zasobnik1, zasobnik2;
    zasobnikInit(&zasobnik1);
    zasobnikInit(&zasobnik2);

    /** Vloz na zasobnik symbol $ */
    dolar.symbol = DOLAR;
    zasobnikPush(&zasobnik1, dolar);

    /** Dokud aktualni vstupni symbol a nejvrchnejsi terminalni symbol neni dolar, proved dalsi smycku */
    while (!((prevedenyToken == DOLAR) && (nejvrchTermSymbol.symbol == DOLAR))) {
        /** Nacteme token a prevedeme na terminalni symbol: a */
        if (precti == 1) {
            token = getNextToken();
            prevedenyToken = prevedToken(token);
            /** Uvolnime token */
            tokenFree(&token);
        }

        /** Je taky potreba najit nejvrchnejsi terminalni symbol na zasobniku: b */
        presypZasobnikyPoTerminal(&zasobnik1, &zasobnik2);
        zasobnikPrectiVrchol(&zasobnik1, &nejvrchTermSymbol);
        presypZasobnikZpet(&zasobnik2, &zasobnik1);

        /** Podle obsahu policka precedencni tabulky na souradnicich [b, a] se rozhodne */
        akce = precedencniTabulka[nejvrchTermSymbol.symbol][prevedenyToken];

        //printf("Akce: [%d][%d] %d\n", nejvrchTermSymbol.symbol, prevedenyToken,akce);

        /** Po dokonceni syntakticke analyzy vstupniho retezce opoustime cyklus */
        if ((prevedenyToken == DOLAR) && (nejvrchTermSymbol.symbol == DOLAR))
            break;

        switch (akce) {

        case R:
            pomocna.symbol = prevedenyToken;
            zasobnikPush(&zasobnik1, pomocna);
            precti = 1;
            break;

        case M:
            presypZasobnikyPoTerminal(&zasobnik1, &zasobnik2);
            zasobnikPush(&zasobnik1, zarazka);              /** Vlozime < na zasobnik */
            presypZasobnikZpet(&zasobnik2, &zasobnik1);
            terminal.symbol = prevedenyToken;               /** Precteme token ze vstupu a dame na vrchol zasobniku */
            zasobnikPush(&zasobnik1, terminal);
            precti = 1;
            break;

        case V:
            navrat = redukuj(&zasobnik1, &zasobnik2);
            precti = 0;
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
    }

    while (!zasobnikEmpty(&zasobnik1))
        zasobnikPop(&zasobnik1);
    while (!zasobnikEmpty(&zasobnik2))
        zasobnikPop(&zasobnik2);

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
