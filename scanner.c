/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Lexikalni analyzator               */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "navratova_hodnota.h"

#define POC_VELIKOST 8
#define POC_KLICOVYCH_SLOV 18
#define POC_LOGICKYCH_HODNOT 2

tToken token;
FILE *f;

void setSourceFile(FILE *source)
{
  f = source;
}

char *klicova_slova[POC_KLICOVYCH_SLOV] = {"begin", "boolean", "do", "else",
    "end", "find", "forward", "function", "if", "integer", "readln",
    "real", "sort", "string", "then", "var", "while", "write" };

char *logicke_hodnoty[POC_LOGICKYCH_HODNOT] = {"false", "true"};

int tokenInit(tToken *s) {
    if((s->data = (char*) malloc(POC_VELIKOST)) == NULL)
        return S_INTERNI_CHYBA;
    s->stav = s_start;
    s->delka = 0;
    s->alokovano = POC_VELIKOST;
    s->data[0] = '\0';
    return 0;
}

void tokenFree(tToken *s) {
    free(s->data);
}

int tokenEdit(tToken *s, char c) {
    if (s->alokovano <= (s->delka + 1)) {
        if ((s->data = (char*) realloc(s->data, s->delka + POC_VELIKOST)) == NULL)
            return S_INTERNI_CHYBA;
        s->alokovano = s->delka + POC_VELIKOST;
    }
    s->data[s->delka] = c;
    s->delka++;
    s->data[s->delka] = '\0';
    return 0;
}

int tokenChangeState(tToken *s, tStav stav) {
    s->stav = stav;
    return 0;
}

int checkKeyWords(char *s) {
    int i;
    for (i = 0; i < POC_KLICOVYCH_SLOV; i++) {
        if (strcmp(s, klicova_slova[i]) == 0) {
            return s_klicove;
        }
    }
    for (i = 0; i < POC_LOGICKYCH_HODNOT; i++) {
        if (strcmp(s, logicke_hodnoty[i]) == 0) {
            return s_logicka_hodnota;
        }
    }
    return s_identifikator;
}

void tokenClear(tToken *s) {
   s->data[0] = '\0';
   s->delka = 0;
}

tToken getNextToken() {
    int c, pom, pom_escape;
    int stav = s_start;

    /** Vymazeme obsah tokenu a v pripade retezce jej budeme ukladat */
    tokenClear(&token);

    while (1) {
    c = getc(f);

        switch (stav) {

        case s_start: {
            if (isspace(c)) {                   /** Bile znaky budeme ignorovat */
                stav = s_start;
                break;
            }
            else if (isdigit(c)) {
                if (c == '0') {
                    stav = s_pocatecni_nuly;    /** Pocatecni nuly budeme ignorovat */
                    break;
                }
                else {
                    stav = s_cele_cislo;
                }
            }
            else if ((isalnum(c)) || c == '_') {
                stav = s_identifikator;
            }
            else if (c == ':') {
                stav = s_dvojtecka;
            }
            else if (c == '.') {
                stav = s_tecka;
            }
            else if (c == '+') {
                stav = s_plus;
            }
            else if (c == '-') {
                stav = s_minus;
            }
            else if (c == '*') {
                stav = s_krat;
            }
            else if (c == '/') {
                stav = s_deleno;
            }
            else if (c == EOF) {
                stav = s_eof;
            }
            else if (c == '{') {                /** Komentare */
                stav = s_komentar;
                break;
            }
            else if (c == '(') {
                stav = s_leva_zavorka;
            }
            else if (c == ')') {
                stav = s_prava_zavorka;
            }
            else if (c == '<') {
                stav = s_mensi;
            }
            else if (c == '>') {
                stav = s_vetsi;
            }
            else if (c == '=') {
                stav = s_rovno;
            }
            else if (c == ';') {
                stav = s_strednik;
            }
            else if (c == ',') {
                stav = s_carka;
            }
            else if (c == 39) {
                stav = s_string;
                break;
            }
            else {
                stav = s_lex_error;
                break;
            }
            tokenEdit(&token, c);
            break;
        }

        case s_identifikator:
            if ((isalnum(c)) || (isdigit(c)) || c == '_') {
                stav = s_identifikator;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {
                ungetc(c, f);                   /** Je potreba vratit posledni nacteny znak */
                stav = checkKeyWords(token.data);
                tokenChangeState(&token, stav);
                stav = s_konec;
            }
            break;

        case s_pocatecni_nuly:                  /** Pocatecni nuly u cisel budeme ignorovat */
            if (c == '0') {
                stav = s_pocatecni_nuly;
            }
            else if ((c != '0') && isdigit(c)) {
                stav = s_cele_cislo;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else if (c == '.') {
                stav = s_deset_cislo_pom;
                tokenEdit(&token, '0');
                tokenEdit(&token, c);
            }
            else if (c == 'e' || c == 'E') {
                tokenEdit(&token, '0');
                stav = s_exponent_pom;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {                              /** Pokud je zapsano vice nul, tak token bude jen jedna 0*/
                ungetc(c, f);
                stav = s_cele_cislo;
                tokenEdit(&token, '0');
                tokenChangeState(&token, stav);
                return token;
            }
            break;

        case s_cele_cislo:
            if (isdigit(c)) {
                stav = s_cele_cislo;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else if (c == '.') {                /** Prisel znak tecka, takze dostavame desetinne cislo */
                stav = s_deset_cislo_pom;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else if (c == 'e' || c == 'E') {
                stav = s_exponent_pom;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {
                ungetc(c, f);
                stav = s_cele_cislo;
                tokenChangeState(&token, stav);
                stav = s_konec;
            }
            break;

        case s_deset_cislo_pom:                 /** Mezistav - kontroluje chybu, kdyz za teckou neni cislo */
            if (isdigit(c)) {
                stav = s_desetinne_cislo;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {
                stav = s_lex_error;
            }
            break;

        case s_desetinne_cislo:
            if (isdigit(c)) {
                stav = s_desetinne_cislo;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else if (c == 'e' || c == 'E') {
                stav = s_exponent_pom;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {
                ungetc(c, f);
                stav = s_konec;
            }
            break;

        case s_poc_nuly_exp:
            if (c == '0') {
                stav = s_poc_nuly_exp;
            }
            else if ((c != '0') && isdigit(c)) {
                stav = s_desetinne_cislo;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {
                stav = s_lex_error;
            }
            break;

        case s_exponent_pom:                    /** Pomocny mezistav - za e/E musi nasledovat znamenko nebo cislo */
            if (c == '-' || c == '+') {
                tokenEdit(&token, c);
                stav = s_exponent_pom2;
            }
            else if (c == '0') {
                stav = s_poc_nuly_exp;          /** Kontrola nul u exponentu */
            }
            else if (isdigit(c)) {
                tokenEdit(&token, c);
                stav = s_exponent;
            }
            else {                              /** Jinak chyba */
                stav = s_lex_error;
                ungetc(c, f);
            }
            break;

        case s_exponent_pom2:                   /** Po exponentu (a pripadne znamenku) musi nasledovat cislo */
            if (isdigit(c) && c != '0') {
                stav = s_desetinne_cislo;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else if (c == '0') {
                stav = s_poc_nuly_exp;          /** Kontrola nul u exponentu */
            }
            else {                              /** Jinak chyba */
                stav = s_lex_error;
                ungetc(c, f);
            }
            break;

        case s_exponent:
            if (isdigit(c)) {
                stav = s_desetinne_cislo;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {
                stav = s_desetinne_cislo;
                tokenChangeState(&token, stav);
                ungetc(c, f);
                stav = s_konec;
            }
            break;

        case s_dvojtecka:
            if (c == '=') {
                stav = s_prirazeni;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {
                stav = s_dvojtecka;
                tokenChangeState(&token, stav);
                ungetc(c, f);
                stav = s_konec;
            }
            break;

        case s_prirazeni:
            ungetc(c, f);
            stav = s_konec;
            break;

        case s_mensi:
            if (c == '=') {
                stav = s_mensi_rovno;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else if (c == '>') {
                stav = s_nerovno;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {
                ungetc(c, f);
                stav = s_mensi;
                tokenChangeState(&token, stav);
                stav = s_konec;
            }
            break;

        case s_nerovno:
            stav = s_konec;
            break;

        case s_vetsi:
            if (c == '=') {
                stav = s_vetsi_rovno;
                tokenEdit(&token, c);
                tokenChangeState(&token, stav);
            }
            else {
                ungetc(c, f);
                stav = s_vetsi;
                tokenChangeState(&token, stav);
                stav = s_konec;
            }
            break;

        case s_vetsi_rovno:
            stav = s_konec;
            break;

        case s_komentar:                            /** Komentar, do tokenu jej nebudeme ukladat */
            if (c == '}') {
                stav = s_start;
            }
            else {
                /** Komentar pokracuje libovolnymi znaky, do tokenu je nebudeme ukladat,
                    stav bude stale s_komentar. Musime kontrolovat,
                    zda neprijde EOF (neukonceny komentar). */
                if (c == EOF) {
                    stav = s_lex_error;
                }
            }
            break;

        case s_string:
            if (c == 39) {
                stav = s_string_escape_pom;         /** Overime, jestli retezec konci nebo pokracuje escape sekvenci*/
                pom = 0;
                pom_escape = 0;
            }
            else if (c < 31) {                      /** Nepovolene znaky */
                stav = s_lex_error;
            }
            else {                                  /** Jinak normalne ukladame znaky do retezce */
                tokenEdit(&token, c);
            }
            break;

        case s_string_escape_pom:
            if (c == '#') {
                int znak = '\\';
                pom = pom*10+znak;
                stav = s_string_escape_nuly;        /** Prebytecne pocatecni nuly v escape sekvenci ignorujeme */
                tokenChangeState(&token, stav);
            }
            else if (c == 39) {
                tokenEdit(&token, 39);
                stav = s_string;
            }
            else {                                  /** Neni escape sekvence ale konec retezce */
                ungetc(c, f);                       /** Je nutne vratit jeden znak */
                stav = s_string;
                tokenChangeState(&token, stav);
                stav = s_konec;
            }
            break;

        case s_string_escape:
            if (c == 39) {                          /** Konci escape sekvence, takze pridame znak */
                if (pom_escape >= 1 && pom_escape <= 255) {
                    tokenEdit(&token, pom);         /** Jestli cislo i (z #i) nalezi <1;255>, vlozime znak do retezce */
                    stav = s_string;
                    break;
                }
                else {
                    stav = s_lex_error;
                }
            }
            else if (isdigit(c)) {
                    pom = pom*10+c;
                    pom_escape = pom_escape*10+(c - '0');
            }
            else {
                stav = s_lex_error;
            }
            break;

        case s_string_escape_nuly:                  /** Odstraneni pocatecnich nul v escape sekvenci */
            if (c == '0') {
                stav = s_string_escape_nuly;
            }
            else if (c != '0' && isdigit(c)) {
                stav = s_string_escape;
                pom = pom*10+c;
                pom_escape = pom_escape*10+(c - '0');
            }
            else {
                stav = s_lex_error;
            }
            break;

        case s_plus:
        case s_minus:
        case s_krat:
        case s_deleno:
        case s_rovno:
        case s_strednik:
        case s_carka:
        case s_tecka:
        case s_leva_zavorka:
        case s_prava_zavorka:
        case s_eof:
            ungetc(c, f);
            tokenChangeState(&token, stav);
            stav = s_konec;
            break;

        case s_lex_error:
            tokenChangeState(&token, stav);
            return token;

        case s_konec:
            ungetc(c, f);
            return token;
        }
    }
}
