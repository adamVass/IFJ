/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Interpret                          */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include "interpret.h"


tChyba interpret() {

    listIntrukci.Active = listIntrukci.First;

    while (listIntrukci.Active != NULL) {

        TItem *tmp1 = (TItem*)listIntrukci.Active->instruction.address1;
        TItem *tmp2 = (TItem*)listIntrukci.Active->instruction.address2;
        TItem *tmp3 = (TItem*)listIntrukci.Active->instruction.address3;

        switch(listIntrukci.Active->instruction.instructionType) {

            /** Aritmeticke instrukce */
                /** Instrukce scitani */
            case OC_ADD:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEINT;
                    tmp3->data->intNumber = tmp1->data->intNumber + tmp2->data->intNumber;
                    //printf("Scitani: operand1 %d, operand2 %d\n", tmp1->data.intNumber, tmp2->data.intNumber);
                }
                else if (tmp1->type == TYPEINT && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = tmp1->data->intNumber + tmp2->data->floatNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = tmp1->data->floatNumber + tmp2->data->intNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = tmp1->data->floatNumber + tmp2->data->floatNumber;
                }
                else if (tmp1->type == TYPESTR && tmp2->type == TYPESTR) {
                    /** Konkatenace retezcu */
                }
                else {
                    /** Jine kombinace operandu nejsou dovoleny */
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

                /** Instrukce odcitani */
            case OC_SUB:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->data->intNumber = tmp1->data->intNumber - tmp2->data->intNumber;
                    //printf("Odcitani: operand1 %d, operand2 %d\n", tmp1->data.intNumber, tmp2->data.intNumber);
                }
                else if (tmp1->type == TYPEINT && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = tmp1->data->intNumber - tmp2->data->floatNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = tmp1->data->floatNumber - tmp2->data->intNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = tmp1->data->floatNumber - tmp2->data->floatNumber;
                }
                else {
                    /** Jine kombinace operandu nejsou dovoleny */
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

                /** Instrukce nasobeni */
            case OC_MUL:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->data->intNumber = tmp1->data->intNumber * tmp2->data->intNumber;
                    //printf("Nasobeni: operand1 %d, operand2 %d\n", tmp1->data.intNumber, tmp2->data.intNumber);
                }
                else if (tmp1->type == TYPEINT && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = tmp1->data->intNumber * tmp2->data->floatNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = tmp1->data->floatNumber * tmp2->data->intNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = tmp1->data->floatNumber * tmp2->data->floatNumber;
                }
                else {
                    /** Jine kombinace operandu nejsou dovoleny */
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

                /** Instrukce deleni */
            case OC_DIV:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = (double) tmp1->data->intNumber / (double) tmp2->data->intNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = (double) tmp1->data->floatNumber / (double) tmp2->data->floatNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = (double) tmp1->data->floatNumber / (double) tmp2->data->intNumber;
                }
                else if (tmp1->type == TYPEINT && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data->floatNumber = (double) tmp1->data->intNumber / (double) tmp2->data->floatNumber;
                }
                else {
                    /** Jine kombinace operandu nejsou dovoleny */
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

            /** Realcni instrukce */
                /** Instrukce rovno */
            case OC_ROVNO:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->intNumber == tmp2->data->intNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEBOOL && tmp2->type == TYPEBOOL) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->boolValue == tmp2->data->boolValue) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->floatNumber == tmp2->data->floatNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPESTR && tmp2->type == TYPESTR) {
                    tmp3->type = TYPEBOOL;
                    if (!strcmp(tmp1->data->str, tmp2->data->str)) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else {
                    /** Operandy nejsou stejneho typu, tudiz dojde k semanticke chybe c. 4 */
                    tmp3->type = TYPEBOOL;
                    tmp3->data->boolValue = false;
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

                /** Instrukce nerovno */
            case OC_NEROVNO:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->intNumber != tmp2->data->intNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEBOOL && tmp2->type == TYPEBOOL) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->boolValue != tmp2->data->boolValue) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->floatNumber != tmp2->data->floatNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPESTR && tmp2->type == TYPESTR) {
                    tmp3->type = TYPEBOOL;
                    if (strcmp(tmp1->data->str, tmp2->data->str)) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else {
                    /** Operandy nejsou stejneho typu, tudiz dojde k semanticke chybe c. 4 */
                    tmp3->type = TYPEBOOL;
                    tmp3->data->boolValue = false;
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

            case OC_MENSI:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->intNumber < tmp2->data->intNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEBOOL && tmp2->type == TYPEBOOL) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->boolValue < tmp2->data->boolValue) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->floatNumber < tmp2->data->floatNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPESTR && tmp2->type == TYPESTR) {
                    tmp3->type = TYPEBOOL;
                    int rozdil;
                    if ((rozdil = strcmp(tmp1->data->str, tmp2->data->str)) < 0) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else {
                    /** Operandy nejsou stejneho typu, tudiz dojde k semanticke chybe c. 4 */
                    tmp3->type = TYPEBOOL;
                    tmp3->data->boolValue = false;
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

            case OC_MENSI_ROVNO:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->intNumber <= tmp2->data->intNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEBOOL && tmp2->type == TYPEBOOL) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->boolValue <= tmp2->data->boolValue) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->floatNumber <= tmp2->data->floatNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPESTR && tmp2->type == TYPESTR) {
                    tmp3->type = TYPEBOOL;
                    int rozdil;
                    if ((rozdil = strcmp(tmp1->data->str, tmp2->data->str)) <= 0) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else {
                    /** Operandy nejsou stejneho typu, tudiz dojde k semanticke chybe c. 4 */
                    tmp3->type = TYPEBOOL;
                    tmp3->data->boolValue = false;
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

            case OC_VETSI:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->intNumber > tmp2->data->intNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEBOOL && tmp2->type == TYPEBOOL) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->boolValue > tmp2->data->boolValue) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->floatNumber > tmp2->data->floatNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPESTR && tmp2->type == TYPESTR) {
                    tmp3->type = TYPEBOOL;
                    int rozdil;
                    if ((rozdil = strcmp(tmp1->data->str, tmp2->data->str)) > 0) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else {
                    /** Operandy nejsou stejneho typu, tudiz dojde k semanticke chybe c. 4 */
                    tmp3->type = TYPEBOOL;
                    tmp3->data->boolValue = false;
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

            case OC_VETSI_ROVNO:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->intNumber >= tmp2->data->intNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEBOOL && tmp2->type == TYPEBOOL) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->boolValue >= tmp2->data->boolValue) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEBOOL;
                    if (tmp1->data->floatNumber >= tmp2->data->floatNumber) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else if (tmp1->type == TYPESTR && tmp2->type == TYPESTR) {
                    tmp3->type = TYPEBOOL;
                    int rozdil;
                    if ((rozdil = strcmp(tmp1->data->str, tmp2->data->str)) >= 0) {
                        tmp3->data->boolValue = true;
                    }
                    else {
                        tmp3->data->boolValue = false;
                    }
                }
                else {
                    /** Operandy nejsou stejneho typu, tudiz dojde k semanticke chybe c. 4 */
                    tmp3->type = TYPEBOOL;
                    tmp3->data->boolValue = false;
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

            case OC_PRIRAZENI:
                if (tmp1->type == tmp3->type) {
                    if (tmp1->type == TYPEINT) {
                        tmp3->data->intNumber = tmp1->data->intNumber;
                    }
                    else if (tmp1->type == TYPEDOUBLE) {
                        tmp3->data->floatNumber = tmp1->data->floatNumber;
                    }
                    else if (tmp1->type == TYPEBOOL) {
                        tmp3->data->boolValue = tmp1->data->boolValue;
                    }
                    else if (tmp1->type == TYPESTR) {
                        tmp3->data->str = tmp1->data->str;
                    }
                }
                else {
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }

                break;
        }

        /** Posun na dalsi instrukci */
        listIntrukci.Active = listIntrukci.Active->nextItem;
    }

    return S_BEZ_CHYB;
}
