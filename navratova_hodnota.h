/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Navratova hodnota programu, chyby..*/
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

/** Enum pro navratove kody interpretu */
typedef enum {
    S_BEZ_CHYB,                 /** 00. */
    S_LEXIKALNI_CHYBA,          /** 01. */
    S_SYNTAKTICKA_CHYBA,        /** 02. */
    S_SEMANTICKA_CHYBA_NEDEF,   /** 03. */
    S_SEMANTICKA_CHYBA_TYPOVA,  /** 04. */
    S_SEMANTICKA_CHYBA_OSTATNI, /** 05. */
    S_CHYBA_PRI_VSTUPU,         /** 06. */
    S_NEINICIALIZOVANA_PROMENNA,/** 07. */
    S_DELENI_NULOU,             /** 08. */
    S_BEHOVA_CHYBA_OSTATNI,     /** 09. */
    S_INTERNI_CHYBA = 99        /** 99. */
} tChyba;

/** Globalni promenna pro navratovy kod */
extern tChyba navratovy_kod;
