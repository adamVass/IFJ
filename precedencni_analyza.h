/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Precedencni syntakticka analyza    */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#ifndef _PREC_H_
#define _PREC_H_

#include <stdbool.h>
#include "scanner.h"
#include "ial.h"
#include "navratova_hodnota.h"
#include "ilist.h"


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
    TItem polozkaTS;
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

/** Globalni promenna - mezivysledek vyrazu */
tData neterminal;

/**
 *  Hlavicky funkci
 */
void generateInstruction( int typInstrukce, void *addr1, void *addr2, void *addr3);

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
tChyba prevedToken(tToken token, tData *prevedenyToken);

/** Funkce na overeni spravne funkcnosti zasobniku
    a precedencni tabulky */
void vypisZasobnik(tZasobnik zasobnik);
void vypisPrecedencniTabulku();

/** Funkce provede redukci */
tChyba redukuj(tZasobnik *zasobnik1, tZasobnik *zasobnik2);

/** Funkce provede analyzu vyrazu */
tChyba precedencniSA();

#endif
