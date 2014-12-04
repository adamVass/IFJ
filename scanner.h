/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Lexikalni analyzator               */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#ifndef _SCANNER_H_
#define _SCANNER_H_

/**
 *  Stavy konecneho automatu
 */
typedef enum {
    s_start,            /** 00. pocatecni stav */
    s_lex_error,        /** 01. chyba na urovni lexikalni analyzy */
    s_eof,              /** 02. konec souboru */
    s_klicove,          /** 03. klicove slovo */
    s_logicka_hodnota,  /** 04. true nebo false */
    s_cele_cislo,       /** 05. celociselny literal */
    s_desetinne_cislo,  /** 06. desetinny literal */
    s_deset_cislo_pom,  /** 07. desetinny literal mezistav */
    s_exponent,         /** 08. desetinny literal s exponentem */
    s_exponent_pom,     /** 09. desetinny literal mezistav */
    s_exponent_pom2,    /** 10. desetinny literal mezistav */
    s_identifikator,    /** 11. identifikator */
    s_string_escape,    /** 12. escape sekvence */
    s_string_escape_pom,/** 13. pomocna escape sekvence */
    s_string_escape_nuly,/**14. osetreni nul v escape sekvenci */
    s_string,           /** 15. retezec     */
    s_komentar,         /** 16.     {       */
    s_konec_komentare,  /** 17.     }       */
    s_leva_zavorka,     /** 18.     (       */
    s_prava_zavorka,    /** 19.     )       */
    s_prirazeni,        /** 20.     :=      */
    s_plus,             /** 21.     +       */
    s_minus,            /** 22.     -       */
    s_krat,             /** 23.     *       */
    s_deleno,           /** 24.     /       */
    s_mensi,            /** 25.     <       */
    s_vetsi,            /** 26.     >       */
    s_vetsi_rovno,      /** 27.     >=      */
    s_mensi_rovno,      /** 28.     <=      */
    s_rovno,            /** 29.     =       */
    s_nerovno,          /** 30.     <>      */
    s_tecka,            /** 31.     .       */
    s_dvojtecka,        /** 32.     :       */
    s_carka,            /** 33.     ,       */
    s_strednik,         /** 34.     ;       */
    s_pocatecni_nuly,   /** 35. ignorujeme  */
    s_poc_nuly_exp,     /** 36. ignorujeme  */
    s_konec             /** 37. konecny stav*/
} tStav;

/**
 *  Struktura pro ulozeni tokenu a informaci o nem
 */
typedef struct {
    char *data;         /** Data tokenu */
    int delka;          /** Delka tokenu */
    int alokovano;      /** Velikost alokovane pameti */
    tStav stav;
} tToken;

extern tToken token;

/**
 *  Hlavicky funkci
 */
/** Funkce vrati token */
tToken getNextToken();

/** Funkce vytvori novy retezec/token */
int tokenInit(tToken *s);

/** Funkce vymaze obsah tokenu */
void tokenClear(tToken *s);

/** Funkce uvolni retezec/token z pameti */
void tokenFree(tToken *s);

/** Funkce prida na konec retezce/tokenu jeden znak */
int tokenEdit(tToken *s, char c);

/** Funkce zmeni tokenu stav */
int tokenChangeState(tToken *s, tStav stav);

/** Funkce overi, jestli retezec patri do klicovych slov */
int checkKeyWords(char *s);

void setSourceFile(FILE *source);

#endif
