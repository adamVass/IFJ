#ifndef _FRAME_H_
#define _FRAME_H_

#include "ial.h"

typedef struct TStackItem{
	tHTable *ptrht;
	TStackItem *lower;
}TStackItem;

typedef struct TStack{
    TStackItem *top;
} TStack;

TStack *ptrStack;

tChyba initFrames();

tChyba initStack();

tChyba pushFrame();

tChyba popFrame();


#endif
