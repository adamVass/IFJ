/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Interpret                          */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include "interpret.h"
#include "frame.h"


tChyba interpret() {

    //pro meneni listu instrukci v poli pri call a return
    int label = 0;
    //nastaveni prvniho listu - fce main
    listIntrukci = &(ptrTables[label].stackItem->field);
    bool *labelField = (bool*)malloc(sizeof(bool)*currentSize);
    for (int i = 0; i < currentSize; ++i)
        labelField[i] = true;
    bool vysledekCopy = false;
    bool jump = true;
    char *mezivysledekCopy;

    listIntrukci->Active = listIntrukci->First;
    //printf("%d\n", currentSize );

    while (listIntrukci->Active != NULL) {

        TItem *tmp1 = (TItem*)listIntrukci->Active->instruction.address1;
        TItem *tmp2 = (TItem*)listIntrukci->Active->instruction.address2;
        TItem *tmp3 = (TItem*)listIntrukci->Active->instruction.address3;

        //printf("%d\n", listIntrukci->Active->instruction.instructionType);

        switch(listIntrukci->Active->instruction.instructionType) {
            case OC_CPY:
                tmp1->data = copyData( tmp1->type, tmp1->data );
                tmp1->init = true;
                break;

            case OC_RET:
                labelField[label] = false;
                do{
                    label--;
                }while(!labelField[label]);
                //printf("\nlabel%d\n", label);
                listIntrukci = &(ptrTables[label].stackItem->field);

                break;
            case OC_CALL:
                    //posunuti,abychom se nezacyklili volanim funkce porad dokola, az se budem vracet
                do{
                    label++;
                }while(!labelField[label] );
                //printf("\nlabel%d\n", label);
                listIntrukci = &(ptrTables[label].stackItem->field);
                listIntrukci->Active = listIntrukci->First;
                jump = false;
                break;

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
                if( !tmp1->init ){
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                if (tmp1->type == tmp3->type) {////////////////////////////////////////////
                    if (tmp1->type == TYPESTR) {///////////////////////////////
                        tmp3->data->str = allocString(tmp1->data->str);////////////////////////////////
                    }
                    else *tmp3->data = *tmp1->data;///////////////////////
                    tmp3->init = true;
                }
                else {
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }

                break;

            case OC_READ:
                if (tmp3->type == TYPEINT) {
                    if (scanf("%i", &(tmp3->data->intNumber)) != 1) {
                        fprintf(stderr, "Promenna je jineho typu\n");
                        return S_CHYBA_PRI_VSTUPU;
                    }
                }
                else if (tmp3->type == TYPEDOUBLE) {
                    if (scanf("%lf", &(tmp3->data->floatNumber)) != 1) {
                        fprintf(stderr, "Promenna je jineho typu\n");
                        return S_CHYBA_PRI_VSTUPU;
                    }
                }
                else if (tmp3->type == TYPEBOOL) {
                    fprintf(stderr, "Do boolovske promenne se neda cist\n");
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

            case OC_WRITE:
                if( !tmp1->init ) {
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                if (tmp1->type == TYPEINT) {
                    printf("%d", tmp1->data->intNumber);
                }
                else if (tmp1->type == TYPEDOUBLE) {
                    printf("%g", tmp1->data->floatNumber);
                }
                else if (tmp1->type == TYPEBOOL) {
                    printf("%s", tmp1->data->boolValue ? "TRUE" : "FALSE");
                }
                else if (tmp1->type == TYPESTR) {
                    printf("%s", tmp1->data->str);
                }
                if( tmp1->druh == DRUHDOCAS ){//////////////////////////////////////////////////////////////////////
                    free(tmp1->data->str);//////////////////////////////////////////////////////////
                    free(tmp1->data);//////////////////////////////////////////////////////////
                    free(tmp1);//////////////////////////////////////////////////////////////////////////////////
                }///////////////////////////////////////////////////////////////////////////////////////////////////
                break;


            case OC_LENGTH:
                if (tmp1->type == TYPESTR) {
                    tmp3->type = TYPEINT;
                    tmp3->data->intNumber = strLength(tmp1->data->str);
                    tmp3->init = true;
                }
                else {
                    fprintf(stderr, "Funkce length ma jeden parametr a to typu char*\n");
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

            case OC_COPY:
                // ulozeni vraceneho podretezce
                if (vysledekCopy == false) {
                    if (tmp1->type == TYPESTR && (tmp2->type == TYPEINT && tmp2->data->intNumber > 0) && (tmp3->type == TYPEINT && tmp3->data->intNumber)) {
                        mezivysledekCopy = malloc(strlen(tmp1->data->str + 1));
                        mezivysledekCopy = strCopy(tmp1->data->str, tmp2->data->intNumber, tmp3->data->intNumber);
                        vysledekCopy = true;

                    }
                    else {
                        fprintf(stderr, "Spatne zadane parametry funkci copy\n");
                        return S_SEMANTICKA_CHYBA_TYPOVA;
                    }
                }
                // vraceni ulozeneho podretezce, instrukce OC_COPY ma jine parametry
                else if (vysledekCopy == true) {
                    //fprintf(stderr, "Ulozeni podretezce\n");
                    tmp3->type = TYPESTR;
                    tmp3->data->str = mezivysledekCopy;
                    tmp3->init = true;
                    vysledekCopy = false;
                }

                break;

            case OC_SORT:
                if (tmp1->type == TYPESTR) {
                    tmp3->type = TYPESTR;
                    tmp3->data->str = strSort(tmp1->data->str);
                    tmp3->init = true;
                }
                else {
                    fprintf(stderr, "Funkce sort ocekava jako parametr term typu char*\n");
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;

            case OC_FIND:
                if (tmp1->type == TYPESTR && tmp2->type == TYPESTR) {
                    tmp3->type = TYPEINT;
                    tmp3->data->intNumber = kmp(tmp1->data->str, tmp2->data->str);
                    tmp3->init = true;
                }
                else {
                    fprintf(stderr, "Funkce find ocekava dva parametry typu char*\n");
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }
                break;
        }


        /** Posun na dalsi instrukci */
        if(jump){
            listIntrukci->Active = listIntrukci->Active->nextItem;
        } else jump = true;
    }

    return S_BEZ_CHYB;
}
