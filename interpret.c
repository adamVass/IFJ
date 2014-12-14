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

        switch (listIntrukci->Active->instruction.instructionType) {
            case OC_CPY:
                tmp1->data = copyData( tmp1->type, tmp1->data );
                tmp1->init = true;
                break;

            case OC_RET:
                labelField[label] = false;
                do{
                    label--;
                } while(!labelField[label]);
                //printf("\nlabel%d\n", label);
                listIntrukci = &(ptrTables[label].stackItem->field);

                break;
            case OC_CALL:
                    //posunuti,abychom se nezacyklili volanim funkce porad dokola, az se budem vracet
                do{
                    label++;
                } while(!labelField[label]);
                //printf("\nlabel%d\n", label);
                listIntrukci = &(ptrTables[label].stackItem->field);
                listIntrukci->Active = listIntrukci->First;
                jump = false;
                break;
            case OC_GOTO_WHILE:
                //loop jump?
                if( listIntrukci->Active->nextItem->instruction.instructionType == OC_WHILE_AFTER ){
                    //finding right OC_WHILE_PRED label
                    listIntrukci->Active = listIntrukci->First;
                    while( 1 ){
                        TItem *lab = (TItem*)listIntrukci->Active->instruction.address3;
                        if( listIntrukci->Active->instruction.instructionType == OC_WHILE_PRED
                            && lab->data->intNumber == tmp3->data->intNumber ){
                            break;
                        }
                        listIntrukci->Active = listIntrukci->Active->nextItem;
                    }
                }
                // while condition
                else if( !tmp1->data->boolValue ){
                    //finding right OC_WHILE_AFTER label
                    while( 1 ){
                        TItem *lab = (TItem*)listIntrukci->Active->instruction.address3;
                        if( listIntrukci->Active->instruction.instructionType == OC_WHILE_AFTER
                            && lab->data->intNumber == tmp3->data->intNumber ){
                            break;
                        }
                        listIntrukci->Active = listIntrukci->Active->nextItem;
                    }
                }
                break;
            case OC_GOTO:
                if(!tmp1->data->boolValue){
                    tItemPtr n = listIntrukci->Active;
                    while(n){
                        TItem *lab = (TItem*)n->instruction.address3;
                        if( n->instruction.instructionType == OC_ELSE
                            && lab->data->intNumber ==  tmp3->data->intNumber )
                        {
                            listIntrukci->Active = n;
                            break;
                        }
                        n = n->nextItem;
                    }
                }
                break;
            case OC_GOTOAFTER:
                if(1){ //declaration after statement hack
                    tItemPtr n = listIntrukci->Active;
                    while(n){
                        TItem *lab = (TItem*)n->instruction.address3;
                        if( n->instruction.instructionType == OC_AFTER_ELSE
                            && lab->data->intNumber ==  tmp3->data->intNumber )
                        {
                            listIntrukci->Active = n;
                            break;
                        }
                        n = n->nextItem;
                    }
                }
                break;

            /** Aritmeticke instrukce */
                /** Instrukce scitani */
            case OC_ADD:
                if (tmp1->init && tmp2->init) {
                    if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                        tmp3->type = TYPEINT;
                        tmp3->data->intNumber = tmp1->data->intNumber + tmp2->data->intNumber;
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
                        char *tmp = allocString(tmp1->data->str);;
                        tmp3->type = TYPESTR;
                        tmp3->data->str = strcat(tmp, tmp2->data->str);
                    }
                    else {
                        /** Jine kombinace operandu nejsou dovoleny */
                        return S_SEMANTICKA_CHYBA_TYPOVA;
                    }
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

                /** Instrukce odcitani */
            case OC_SUB:
                if (tmp1->init && tmp2->init) {
                    if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                        tmp3->data->intNumber = tmp1->data->intNumber - tmp2->data->intNumber;
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
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

                /** Instrukce nasobeni */
            case OC_MUL:
                if (tmp1->init && tmp2->init) {
                    if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                        tmp3->data->intNumber = tmp1->data->intNumber * tmp2->data->intNumber;
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
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

                /** Instrukce deleni */
            case OC_DIV:
                if (tmp1->init && tmp2->init) {
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
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

            /** Realcni instrukce */
                /** Instrukce rovno */
            case OC_ROVNO:
                if (tmp1->init && tmp2->init) {
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
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

                /** Instrukce nerovno */
            case OC_NEROVNO:
                if (tmp1->init && tmp2->init) {
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
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

            case OC_MENSI:
                if (tmp1->init && tmp2->init) {
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
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

            case OC_MENSI_ROVNO:
                if (tmp1->init && tmp2->init) {
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
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

            case OC_VETSI:
                if (tmp1->init && tmp2->init) {
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
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

            case OC_VETSI_ROVNO:
                if (tmp1->init && tmp2->init) {
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
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

            case OC_PRIRAZENI:
                if (!tmp1->init) {
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                if (tmp1->type == tmp3->type) {////////////////////////////////////////////
                    if (tmp1->type == TYPESTR) {///////////////////////////////
                        tmp3->data->str = allocString(tmp1->data->str);////////////////////////////////
                    }
                    else {
                        *tmp3->data = *tmp1->data;///////////////////////
                    }
                    tmp3->init = true;
                }
                else {
                    return S_SEMANTICKA_CHYBA_TYPOVA;
                }

                break;

            case OC_READ:
                if (tmp3->type == TYPEINT) {
                    tmp3->init = true;
                    if (scanf("%i", &(tmp3->data->intNumber)) != 1) {
                        fprintf(stderr, "Promenna je jineho typu\n");
                        return S_CHYBA_PRI_VSTUPU;
                    }
                }
                else if (tmp3->type == TYPEDOUBLE) {
                    tmp3->init = true;
                    if (scanf("%lf", &(tmp3->data->floatNumber)) != 1) {
                        fprintf(stderr, "Promenna je jineho typu\n");
                        return S_CHYBA_PRI_VSTUPU;
                    }
                }
                else if (tmp3->type == TYPESTR) {
                    tmp3->init = true;
                    tToken tmp;
                    int znak;
                    tokenInit(&tmp);
                    tokenClear(&tmp);
                    while (1) {
                        znak = getchar();
                        if (znak == EOF || znak == '\n') {
                            break;
                        }
                        else {
                            tokenEdit(&tmp, (char) znak);
                        }
                    }
                    tmp3->data->str = allocString(tmp.data);
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
                if (tmp1->druh == DRUHDOCAS){//////////////////////////////////////////////////////////////////////
                    free(tmp1->data->str);//////////////////////////////////////////////////////////
                    free(tmp1->data);//////////////////////////////////////////////////////////
                    free(tmp1);//////////////////////////////////////////////////////////////////////////////////
                }///////////////////////////////////////////////////////////////////////////////////////////////////
                break;

            case OC_LENGTH:
                if (tmp1->init) {
                    if (tmp1->type == TYPESTR) {
                        tmp3->type = TYPEINT;
                        tmp3->data->intNumber = strLength(tmp1->data->str);
                        tmp3->init = true;
                    }
                    else {
                        fprintf(stderr, "Funkce length ma jeden parametr a to typu char*\n");
                        return S_SEMANTICKA_CHYBA_TYPOVA;
                    }
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna u length\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

            case OC_COPY:
                    /** Ulozeni vraceneho podretezce */
                    if (vysledekCopy == false) {
                        if (tmp1->init && tmp2->init && tmp3->init) {
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
                        else {
                            /** Neinicializovana promenna */
                            fprintf(stderr, "Neinicializvana promenna u copy\n");
                            return S_NEINICIALIZOVANA_PROMENNA;
                        }
                    }
                    /** Vraceni ulozeneho podretezce, zde ma instrukce OC_COPY jine parametry */
                    else if (vysledekCopy == true) {
                        tmp3->type = TYPESTR;
                        tmp3->data->str = mezivysledekCopy;
                        tmp3->init = true;
                        vysledekCopy = false;
                    }
                break;

            case OC_SORT:
                if (tmp1->init) {
                    if (tmp1->type == TYPESTR) {
                        tmp3->type = TYPESTR;
                        tmp3->data->str = strSort(tmp1->data->str);
                        tmp3->init = true;
                    }
                    else {
                        fprintf(stderr, "Funkce sort ocekava jako parametr term typu char*\n");
                        return S_SEMANTICKA_CHYBA_TYPOVA;
                    }
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna u sort\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;

            case OC_FIND:
                if (tmp1->init && tmp2->init) {
                    if (tmp1->type == TYPESTR && tmp2->type == TYPESTR) {
                        tmp3->type = TYPEINT;
                        tmp3->data->intNumber = kmp(tmp1->data->str, tmp2->data->str);
                        tmp3->init = true;
                    }
                    else {
                        fprintf(stderr, "Funkce find ocekava dva parametry typu char*\n");
                        return S_SEMANTICKA_CHYBA_TYPOVA;
                    }
                }
                else {
                    /** Neinicializovana promenna */
                    fprintf(stderr, "Neinicializvana promenna u find\n");
                    return S_NEINICIALIZOVANA_PROMENNA;
                }
                break;
        }

        /** Posun na dalsi instrukci */
        if (jump) {
            listIntrukci->Active = listIntrukci->Active->nextItem;
        } else jump = true;
    }

    return S_BEZ_CHYB;
}
