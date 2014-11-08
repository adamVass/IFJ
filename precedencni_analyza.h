/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Precedencni syntakticka analyza    */
/**  Autor: Martin Josef�k              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdbool.h>

/** Enumy pro vyplneni precedencni tabulky */
typedef enum {
    R,          /** 00. rovno -> =   */
    M,          /** 01. mensi -> <   */
    V,          /** 02. vetsi -> >   */
    E           /** 03. chyba -> nic */
} tPrecTabulka;

typedef enum {
    PLUS,       /** 00. +   */
    MINUS,      /** 01. -   */
    KRAT,       /** 02. *   */
    DELENO,     /** 03. /   */
    LZAVORKA,   /** 04. (   */
    ID,         /** 05. ID  */
    PZAVORKA,   /** 06. )   */
    MENSI,      /** 07. <   */
    VETSI,      /** 08. >   */
    MENSIROVNO, /** 09. <=  */
    VETSIROVNO, /** 10. >=  */
    ROVNO,      /** 11. ==  */  // nebo =
    NEROVNO,    /** 12. !=  */  // nebo <>
    DOLAR,      /** 13. $ -> oznaceni dna zasobniku a konce vstupniho retezce  */
    CHYBA,      /** 14. jine -> chyba   */
    NETERMINAL, /** 15. E -> neco   */
    ZARAZKA     /** 16. reprezentuje < na zasobniku */
} tZahlavi;

/** Zasobnik */

/** Struktura uchovavajici data polozky zasobniku */
typedef struct {
    tZahlavi symbol;
    //tToken token;   // spis polozka tabulky symbolu, pro overovani semantiky apod
} tData;

/** Struktura reprezentujici polozku zasobniku */
typedef struct polozka {
    tData data;
    struct polozka *dalsi;
} tPolozka;

/** Struktura reprezentujici zasobnik */
typedef struct {
    tPolozka *vrchol;
} tZasobnik;

/**
 *  Hlavicky funkci
 */
/** Funkce inicializuje zasobnik */
void zasobnikInit(tZasobnik *zasobnik);

/** Funkce vrati true, pokud je zasobnik prazdny */
bool zasobnikEmpty(tZasobnik *zasobnik);

/** Funkce vlozi polozku na vrchol zasobniku */
int zasobnikPush(tZasobnik *zasobnik, tData polozka);

/** Funkce vystrci polozku z vrcholu zasobniku */
void zasobnikPop(tZasobnik *zasobnik);

/** Funkce precte data z vrcholu zasobniku a preda je odkazem */
void zasobnikPrectiVrchol(tZasobnik *zasobnik, tData *data);

/** Funkce presype polozky zasobniku1 na zasobnik2 a to
    az po prvni terminal smerem od vrcholu zasobniku1.
    Na vrcholu zasobniku1 tim padem zustane prvni terminal. */
void presypZasobnikyPoTerminal(tZasobnik *zasobnik1, tZasobnik *zasobnik2);

/** Funkce presype vsechny polozky zasobniku2 na zasobnik1 */
void presypZasobnikZpet(tZasobnik *zasobnik2, tZasobnik *zasobnik1);

/** Funkce prevede token na terminal */
int prevedToken(tToken token);

/** Funkce na overeni spravne funkcnosti zasobniku
    a precedencni tabulky */
void vypisZasobnik(tZasobnik zasobnik);
void vypisPrecedencniTabulku();

/** Funkce provede redukci */
int redukuj(tZasobnik *zasobnik1, tZasobnik *zasobnik2);

/** Funkce provede analyzu vyrazu */
int precedencniSA();
