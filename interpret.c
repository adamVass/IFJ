/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Interpret                          */
/**  Autor: Martin Josefík              */
/**  email: xjosef00@stud.fit.vutbr.cz  */
/****************************************/

#include "interpret.h"


tChyba interpret() {
    while (listIntrukci.First != NULL) {

        TItem *tmp1 = (TItem*)listIntrukci.First->instruction.address1;
        TItem *tmp2 = (TItem*)listIntrukci.First->instruction.address2;
        TItem *tmp3 = (TItem*)listIntrukci.First->instruction.address3;

        switch(listIntrukci.First->instruction.instructionType) {

            case OC_ADD:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->data.intNumber = tmp1->data.intNumber + tmp2->data.intNumber;
                    printf("Scitani: operand1 %d, operand2 %d\n", tmp1->data.intNumber, tmp2->data.intNumber);
                }
                break;

            case OC_SUB:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->data.intNumber = tmp1->data.intNumber - tmp2->data.intNumber;
                    printf("Odcitani: operand1 %d, operand2 %d\n", tmp1->data.intNumber, tmp2->data.intNumber);
                }
                break;

            case OC_MUL:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->data.intNumber = tmp1->data.intNumber * tmp2->data.intNumber;
                    printf("Nasobeni: operand1 %d, operand2 %d\n", tmp1->data.intNumber, tmp2->data.intNumber);
                }
                break;

            case OC_DIV:
                if (tmp1->type == TYPEINT && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data.floatNumber = (double) tmp1->data.intNumber / (double) tmp2->data.intNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data.floatNumber = (double) tmp1->data.floatNumber / (double) tmp2->data.floatNumber;
                }
                else if (tmp1->type == TYPEDOUBLE && tmp2->type == TYPEINT) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data.floatNumber = (double) tmp1->data.floatNumber / (double) tmp2->data.intNumber;
                }
                else if (tmp1->type == TYPEINT && tmp2->type == TYPEDOUBLE) {
                    tmp3->type = TYPEDOUBLE;
                    tmp3->data.floatNumber = (double) tmp1->data.intNumber / (double) tmp2->data.floatNumber;
                }
                break;
        }

        /** Posun na dalsi instrukci */
        listIntrukci.First = listIntrukci.First->nextItem;
    }

}
