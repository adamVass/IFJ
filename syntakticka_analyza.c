/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Syntakticka analyza                */
/**  Autor: Karel Fajkus                */
/**  email: xfajku05@stud.fit.vutbr.cz  */
/****************************************/


//pouziti knihoven na precedencni analyzu, navratove hodnoty atd.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "syntakticka_analyza.h"
#include "ilist.h"





//funkce na vypis hash tabulky
void htPrintTable( tHTable *ptrht ) {
	int maxlen = 0;
	int sumcnt = 0;
	printf ("------------HASH TABLE--------------\n");
	for ( int i=0; i<HTSIZE; i++ ) {
	printf ("%i:",i);
	int cnt = 0;
	TItem* ptr = (*ptrht)[i];
	while ( ptr != NULL ) {
	printf (" (%s)",ptr->key); // vytiskne klic
	if( !strcmp(ptr->key, "$1") ){
		printf("%d\n", ptr->data->intNumber);
	}
	if( ptr->druh == 7 ){ 
	printf(" %d ", ptr->druh);
	printf("%d\n", ptr->data->param.numParam );

	for (int i = 0; i < ptr->data->param.numParam; i++){
		printf("%s ", ptr->data->param.param[i]);
		
	}
}
else if( ptr->druh == 8 ){ 
printf(" %d", ptr->druh);
}
else if( ptr->druh == 9 ){ 
printf(" %d", ptr->druh);
}
else if( ptr->druh == 10 ){ 

printf(" %d", ptr->druh);
}
cnt++;
ptr = ptr->ptrnext;
}
printf ("\n");
if (cnt > maxlen)
maxlen = cnt;
sumcnt+=cnt;
}
printf ("------------------------------------\n");
printf ("Items count %i The longest list %i\n",sumcnt,maxlen);
printf ("------------------------------------\n");
}

//funkce na kontrolu doprednych deklaraci funkci
tChyba TestFunction(tHTable *ptrht) {
    
    for (int i = 0; i < HTSIZE; ++i){
        TItem* tmp = (*ptrht)[i];
        while ( tmp != NULL ) {
            if(tmp->init == false && tmp->druh == ID_FUNCTION){
                return S_SEMANTICKA_CHYBA_NEDEF;
            }
        tmp = tmp->ptrnext;
        }
    }
    return S_BEZ_CHYB;
}



//start programu. PROGRAM-> FUNKCE . eof
tChyba PROGRAM () {					
	//inicializace labelu pro if a while na 0
	lab = 0;	
	lab_while = 0;						//nacitani prvniho tokenu je v komentu, protoze na konci je pomocna funkce, kde se uz nacita token, pri vyslednem kodu, bude toto nacteni tokenu odkomentovano
	/*token = getNextToken();
	if(token.stav == s_lex_error) {			
		return S_LEXIKALNI_CHYBA;
	}*/
	//lokalni promenna, do ni se nacita vystup funkci	
	int analyza;					
	//vytvoreni globalni a lokalni(main) hash tabulky
	int ret = initFrames();
	byla_funkce = false;
	pocitani_parametru = 0;

	if(ret != S_BEZ_CHYB ) {
		return S_INTERNI_CHYBA;
	}

	if(token.stav == s_klicove && (!strcmp(token.data, "function") || !strcmp(token.data, "var") || !strcmp(token.data, "begin"))) {
							//podminka kde se kontroluje, zda nacteny token je klicove slovo a nasledne se rozhodne, ktere konkretne. Pote dojde k zavolani funkce FUNKCE, kde jsou 3 pravidla, ktere zacinaji terminala vyse
		analyza = FUNKCE ();
		if (analyza != S_BEZ_CHYB) {		//pokud se vrati po zavolani FUNKCE() s_bez_chyb pokracuje se dal, jinak se vraci chybovy kod, ktery funkce vratila

		return analyza;
		}

		if(token.stav == s_tecka) {		//porovnava se ciselna hodnota tokenu s ciselnou hodnotu tecky (tecka neni primo nadefinovana jako tecka, ale je pro ni vyhrazen unikatni identifikator ciselny, ktery porovnavam s id prvku v nactenem tokenu)
			token = getNextToken();
			if(token.stav == s_lex_error) {	//jakmile nacitam token okamzite kontroluju, jestli se nevyskytla lexikalni chyba
				return S_LEXIKALNI_CHYBA;
			}

			if(token.stav == s_eof) {	//jakmile dojdeme ke konci souboru, vrati funkce syntakticka analyza ok a mame vyhrano
				return S_BEZ_CHYB;
			}
		}
	}
		return S_SYNTAKTICKA_CHYBA;		//pokud nejaky token neodpovida predem danym terminalum z pravidel, vraci chybu
}

tChyba FUNKCE() {
	int analyza;
	//opet lokalni promenna, stejna funkce jako ve funkci nahore
	TItem *nasel;
	// FUNKCE-> begin ACT_LIST
	if(!strcmp(token.data, "begin") && token.stav == s_klicove) {
		token = getNextToken();			//pres strcmp porovnavam data tokenu (prakticky primo to, co se nacte)
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		//po zavolani funkce se getToken nevola, protoze kazda funkce, nez vrati hodnotu, nacte dalsi token
		analyza = ACT_LIST();			
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return S_BEZ_CHYB;
	}
	//FUNKCE->function id PARAMS : DTYPE ; DOPREDNE ; FUNKCE
	else if(!strcmp(token.data, "function") && token.stav == s_klicove) {
		//pamatujeme si, ze jsme v definici funkce
		byla_funkce = true;				
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_identifikator) {	
			//pokud id ma nazev stejny jako tyto 2 funkce, jedna se o chybu
			if(!strcmp(token.data, "length") || !strcmp(token.data, "copy")) {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}

			if((funkce = malloc(strlen(token.data)+1)) == NULL) {	
				return S_INTERNI_CHYBA;
			}
			//nulovani poctu parametru funkce
			TData *dat = (TData*)malloc(sizeof(TData));
			dat->param.numParam = 0;
			
			strcpy(funkce, token.data);
			//pokud id funkce jiz je v globalni hash tabulce a nejedna se o doprednou deklaraci -> chyba
			nasel = htSearch(ptrhtGlobal,funkce);		
			if(nasel != NULL ) {
				if(nasel->init == true) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}
				nasel->init = true;
			}
			//vlozeni nazvu funkce do globalni hash
			htInsert(ptrhtGlobal,funkce,dat, TYPEUNDEF , ID_FUNCTION);		

			token = getNextToken();
			if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
			}
			//funkce na kontrolu a ukladani parametru funkce
			analyza = PARAMS();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
				
			else if(token.stav == s_dvojtecka) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
				//funkce na kontrolu datovych typu	
				analyza = DTYPE();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}
										
				if(!strcmp(token.data, "integer")) {
					dtype = TYPEINT;
				}
				else if(!strcmp(token.data, "real")) {				//do docasne promene nacteme datovy typ funkce
					dtype = TYPEDOUBLE;
				}
				else if(!strcmp(token.data, "string")) {
					dtype = TYPESTR;
				}
				else if(!strcmp(token.data, "boolean")) {
					dtype = TYPEBOOL;
				}
					
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
	
				else if(token.stav == s_strednik) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}
					//zjisteni, zda se jedna o doprednou deklaraci funkces
					analyza = DOPREDNE();
					if(analyza != S_BEZ_CHYB) {
						return analyza;
					}
					//pokud neni dopredna, vkladame do globalni hash tabulky a zaroven i do lokalni hash tabulky
					if(init == true) {
						htCompleteInsert(ptrhtGlobal,funkce, ID_FUNCTION, dtype, true);				
						htInsert(func[currFuncSize-1].table, funkce, dat, dtype, ID_FUNCTION);
							
					}//pokud se jedna o doprednou deklaraci, ukladame jen do globalky hlavicku funkce																			
					else if (init == false) {
						htCompleteInsert(ptrhtGlobal,funkce, ID_FUNCTION, dtype, false);	
						

							//vlozeni navratove hodnoty do lokalni tabulky
					}
						
					if(token.stav == s_strednik) {
							
						token = getNextToken();
						if(token.stav == s_lex_error) {
							return S_LEXIKALNI_CHYBA;
						}
						else {
							//nastaveni ukazatele lokalni tabulky na vrchol zasobniku
							ptrhtLocal = ptrStack->top->ptrht;
							listIntrukci = &(ptrStack->top->field);
							// nastaveni tabulky instrukci v mainu
							byla_funkce = false;
							//rekurzivni volani 
							return FUNKCE();
						}								
					}
				}
			}
		}
		//pokud se nacte token, ktery podle pravidla tam nepatri, vrati se syntakticka chyba
		return S_SYNTAKTICKA_CHYBA;		
	}
	//FUNKCE->var DEKLARACE FUNKCE
	else if(!strcmp(token.data, "var") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		//volani funkce na kontrolu a ukladani globalnich id
		analyza = DEKLARACE();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return FUNKCE();
	}
	return S_SYNTAKTICKA_CHYBA;
}
//funkce na vkladani parametru
tChyba PARAMS() {
	int analyza;
	//PARAMS->( id : DTYPE PARAMS
	if(token.stav == s_leva_zavorka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		else if(token.stav == s_identifikator) {
			if((params = malloc(strlen(token.data)+1)) == NULL) {
				return S_INTERNI_CHYBA;
			}
			//do globalni promenne params nacteme id parametru funkce
			strcpy(params, token.data);			
		
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			if(token.stav == s_dvojtecka) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				analyza = DTYPE();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}

				if(!strcmp(token.data, "integer")) {
					dtype = TYPEINT;
				}
				else if(!strcmp(token.data, "real")) {
					dtype = TYPEDOUBLE;
				}
				else if(!strcmp(token.data, "string")) {
					dtype = TYPESTR;
				}
				else if(!strcmp(token.data, "boolean")) {
					dtype = TYPEBOOL;
				}
				//vlozeni do pole parametru
				htParamInsert(ptrhtGlobal,funkce, params, dtype);
				//vlozeni parametru do lokalni hash
				htInsert(func[currFuncSize].table, params, NULL, dtype, ID_PARAM);			
				free(params);
				
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
				//rekurzivni volani
		        return PARAMS();
			}
			return S_SYNTAKTICKA_CHYBA;
		}
	}//PARAMS->; id : DTYPE PARAMS	
	else if(token.stav == s_strednik) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		if(token.stav == s_identifikator) {
			
			if((params = malloc(strlen(token.data)+1)) == NULL) {
				return S_INTERNI_CHYBA;
			}
			//vlozeni klice do globalni promenne
			strcpy(params, token.data);
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_dvojtecka) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
				
				analyza = DTYPE();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}
				
				if(!strcmp(token.data, "integer")) {
				dtype = TYPEINT;
				}
				else if(!strcmp(token.data, "string")) {
					dtype = TYPESTR;
				}
				else if(!strcmp(token.data, "real")) {
					dtype = TYPEDOUBLE;
				}
				else if(!strcmp(token.data, "boolean")){
					dtype = TYPEBOOL;
				}
				//vlozeni do globalni, do pole parametru funkce	
				htParamInsert(ptrhtGlobal,funkce, params, dtype);
				htInsert(func[currFuncSize].table, params, NULL, dtype, ID_PARAM);
				free(params);
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}	
				return PARAMS();
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(token.stav == s_prava_zavorka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba PROM() {
	int analyza;
	int ret;
	//TItem *pt;
	//PROM->id TERM2
	if(token.stav == s_identifikator) {
		if(byla_funkce == false) {
			//pokud nejsme v definici funkce vytvori se lokalni ramec
			ret = pushFrame();
			if(ret != S_BEZ_CHYB) {
				return ret;
			}
		}
		
		

		if(byla_funkce == false) {
			//ukazatel na id v tabulce
			TFunction *tmp = searchFuncTable( token.data );
			if( !tmp ){
				return S_SEMANTICKA_CHYBA_NEDEF;
			}
			//kopirovani dat z tabulky definice funkce do nove vznikleho lokalniho ramce
			ret = copyTable( tmp);//, ptrhtLocal );
			if(ret != S_BEZ_CHYB) {
				return ret;
			}
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
			//volani funkce na zpracovani argumentu
			analyza = TERM2();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			else {
				if(byla_funkce == false) {
					//pokud nejsme v definici funkce musime na konci vypopnout ukazatel na lokalni ramec
					popFrame();
				}	
				
				return S_BEZ_CHYB;
			}
			return S_SYNTAKTICKA_CHYBA;
	}
	return S_SYNTAKTICKA_CHYBA;		
}

tChyba VESTAV() {
	TItem *pt, *pt2, *pt3;
	//VESTAV->length ( string )
	if(!strcmp(token.data, "length")) {
		pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
		if(pt != NULL) {
			//pokud najdeme id v tabulkach vse ok
		}
		else {
			return S_SEMANTICKA_CHYBA_NEDEF;
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
			else if(token.stav == s_string || token.stav == s_identifikator) {
				if(token.stav == s_identifikator) {
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
					//prohledani lokalniho i globalniho ramce a pokud id neni nalezeno nebo ma jiny datovy typ nez string -> chyba			
					if(pt != NULL) {
						
						if(pt->type != s_string) {
							return S_SEMANTICKA_CHYBA_TYPOVA;
						}
					}
					else {
						return S_SEMANTICKA_CHYBA_NEDEF;
					}//generovani instrukce, tmp1 je id nebo string a tmp3 je id kam se ulozi vysledek
					generateInstruction(OC_LENGTH, searchFrames(token.data,ptrhtLocal, ptrhtGlobal), NULL, searchFrames(id,ptrhtLocal,ptrhtGlobal));	
				}
				else {
					//vytvoreni polozky TItem, kde se dava do dat hodnota, ktera byla zadana
					TItem *str = (TItem*)malloc(sizeof(TItem));
					str->druh = PRINTSTR;
					str->type = TYPESTR;
					str->init = true;
					str->data = (TData*)malloc(sizeof(TData));
					str->data->str = allocString(token.data);
					//stejne generovani jako nahore
					generateInstruction(OC_LENGTH, str, NULL, searchFrames(id,ptrhtLocal,ptrhtGlobal));
				}
				
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_prava_zavorka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}
					return S_BEZ_CHYB;
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	//VESTAV->copy (string , integer , integer)
	else if(!strcmp(token.data, "copy")) {
		pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
		if(pt != NULL) {
			
		}
		else {
			return S_SEMANTICKA_CHYBA_NEDEF;
		}


		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string || token.stav == s_identifikator) {
				if(token.stav == s_identifikator) {	
					//prohledani lokalniho i globalniho ramce a pokud id neni nalezeno nebo ma jiny datovy typ nez string -> chyba
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);			
					if(pt != NULL) {
						
						if(pt->type != s_string) {
							return S_SEMANTICKA_CHYBA_TYPOVA;
						}
											
					}
					else {
						return S_SEMANTICKA_CHYBA_NEDEF;
					}//ulozeni ukazatele na zadane id
					pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
				}
				else {//vytvoreni TItem a ulozeni stringu do TData
					pt2 = (TItem*)malloc(sizeof(TItem));
					pt2->druh = PRINTSTR;
					pt2->type = TYPESTR;
					pt2->init = true;
					pt2->data = (TData*)malloc(sizeof(TData));
					pt2->data->str = allocString(token.data);
				}

				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_carka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}

					else if(token.stav == s_cele_cislo || token.stav == s_identifikator) {
						if(token.stav == s_identifikator) {
							//prohledani lokalniho i globalniho ramce a pokud id neni nalezeno nebo ma jiny datovy typ nez string -> chyba
							pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);			
							if(pt != NULL) {
								//pokud datovy typ neodpovida int -> chyba
								if(pt->type != s_cele_cislo) {
									return S_SEMANTICKA_CHYBA_TYPOVA;
								}
							}
							else {
								return S_SEMANTICKA_CHYBA_NEDEF;
							}//ulozeni ukazatele na id
							pt3 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
						}
						else {//Vytvoreni polozky TItem a ulozeni hodnoty do TData
							pt3 = (TItem*)malloc(sizeof(TItem));
							pt3->type = TYPEINT;
							pt3->init = true;
							pt3->data = (TData*)malloc(sizeof(TData));
							pt3->data->intNumber = atoi(token.data);
						}

						token = getNextToken();
						if(token.stav == s_lex_error) {
							return S_LEXIKALNI_CHYBA;
						}

						else if(token.stav == s_carka) {
							token = getNextToken();
							if(token.stav == s_lex_error) {
								return S_LEXIKALNI_CHYBA;
							}

							else if(token.stav == s_cele_cislo || token.stav == s_identifikator) {
								if(token.stav == s_identifikator) {
									pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
									if(pt != NULL) {
										
										if(pt->type != s_cele_cislo) {
											return S_SEMANTICKA_CHYBA_TYPOVA;
										}
									}
									else {
										return S_SEMANTICKA_CHYBA_NEDEF;
									}//generovani 2 instrukci, prvni predava vsechny 3 parametry funkce
									generateInstruction(OC_COPY, pt2, pt3, searchFrames(token.data, ptrhtLocal, ptrhtGlobal));
									//generujeme, kam ma interpret dat vysledek
									generateInstruction(OC_COPY, NULL, NULL, searchFrames(id, ptrhtLocal, ptrhtGlobal));	
								}
								else {//pokud posledni parametr neni id, tak vytvorit TItem a pak ulozit do TData hodnotu
									TItem *cele = (TItem*)malloc(sizeof(TItem));
									cele->type = TYPEINT;
									cele->init = true;
									cele->data = (TData*)malloc(sizeof(TData));
									cele->data->intNumber = atoi(token.data);
									//to same jako nahore
									generateInstruction(OC_COPY, pt2, pt3, cele);
									generateInstruction(OC_COPY, NULL, NULL, searchFrames(id, ptrhtLocal, ptrhtGlobal));
								}

								token = getNextToken();
								if(token.stav == s_lex_error) {
									return S_LEXIKALNI_CHYBA;
								}

								else if(token.stav == s_prava_zavorka) {
									token = getNextToken();
									if(token.stav == s_lex_error) {
										return S_LEXIKALNI_CHYBA;
									}
									return S_BEZ_CHYB;
								}
							}
						}
					}
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	//VESTAV->find ( string, string ) 
	else if(!strcmp(token.data, "find") && token.stav == s_klicove) {
		pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
		if(pt != NULL) {
			
		}
		else {
			return S_SEMANTICKA_CHYBA_NEDEF;
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string || s_identifikator) {
				if(token.stav == s_identifikator) {
					//vyhledani id v tabulkach, pokud nenalezeno -> chyba
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
					if(pt != NULL) {
						
						if(pt->type != s_string) {
							return S_SEMANTICKA_CHYBA_TYPOVA;
						}
					}
					else {
						return S_SEMANTICKA_CHYBA_NEDEF;
					}//ulozeni ukazatele na parametr funkce
					pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
				}
				else {//vytvoreni TItem pokud nebylo zadano id ale primo hodnota
					pt2 = (TItem*)malloc(sizeof(TItem));
					pt2->druh = PRINTSTR;
					pt2->type = TYPESTR;
					pt2->init = true;
					pt2->data = (TData*)malloc(sizeof(TData));
					pt2->data->str = allocString(token.data);
				}

				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_carka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}

					else if(token.stav == s_string || token.stav == s_identifikator) {
						if(token.stav == s_identifikator) {
							//kontrola, zda je id v tabulkach
							pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
							if(pt != NULL) {
								
								if(pt->type != s_string) {
									return S_SEMANTICKA_CHYBA_TYPOVA;
								}
							}
							else {
								return S_SEMANTICKA_CHYBA_NEDEF;
							}//generovani find, tmp1 je ukazatel na prvni parametr, tmp2 na druhy parametr, tmp3 na id, kam se ulozi vysledek
							generateInstruction(OC_FIND, pt2, searchFrames(token.data, ptrhtLocal, ptrhtGlobal), searchFrames(id, ptrhtLocal, ptrhtGlobal));	
						}
						else {
							TItem *str = (TItem*)malloc(sizeof(TItem));
							str->druh = PRINTSTR;
							str->type = TYPESTR;
							str->init = true;
							str->data = (TData*)malloc(sizeof(TData));
							str->data->str = allocString(token.data);
							//jako nahore
							generateInstruction(OC_FIND, pt2, str, searchFrames(id,ptrhtLocal,ptrhtGlobal));
						}

						token = getNextToken();
						if(token.stav == s_lex_error) {
							return S_LEXIKALNI_CHYBA;
						}

						else if(token.stav == s_prava_zavorka) {
							token = getNextToken();
							if(token.stav == s_lex_error) {
								return S_LEXIKALNI_CHYBA;
							}
							return S_BEZ_CHYB;
						}
					}
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	//VESTAV->sort ( string )
	else if(!strcmp(token.data, "sort") && token.stav == s_klicove) {
		//kontrola zda je id v tabulkach
		pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
		if(pt != NULL) {
			
		}
		else {
			return S_SEMANTICKA_CHYBA_NEDEF;
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string || token.stav == s_identifikator) {
				if(token.stav == s_identifikator) {
					//kontrola zda je id v tabulkach
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
					if(pt != NULL) {
						
							if(pt->type != s_string) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}
					}
					else {
						return S_SEMANTICKA_CHYBA_NEDEF;
					}//generovani instrukce sort, tmp1 parametr funkce, tmp3 id kam se ulozi vysledek
					generateInstruction(OC_SORT, searchFrames(token.data, ptrhtLocal, ptrhtGlobal), NULL, searchFrames(id, ptrhtLocal, ptrhtGlobal));	
				}
				else {//dela se, pokud nebylo zadano id ale primo hodnota
					TItem *str = (TItem*)malloc(sizeof(TItem));
					str->druh = PRINTSTR;
					str->type = TYPESTR;
					str->init = true;
					str->data = (TData*)malloc(sizeof(TData));
					str->data->str = allocString(token.data);
					//stejne jako nahore
					generateInstruction(OC_SORT, str, NULL, searchFrames(id,ptrhtLocal,ptrhtGlobal));
				}

				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_prava_zavorka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}
					return S_BEZ_CHYB;
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	return S_EPS;
}

tChyba DOPREDNE() {
	int ret;
	TItem *nasel;
	//DOPREDNE->forward
	if(!strcmp(token.data, "forward") && token.stav == s_klicove) {			
		//neinicializovana pokud se jedna o doprednou deklaraci
		init = false;						
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	//DOPREDNE->ACT_LIST
	else {
		//initla, pokud se nejedna o doprednou
		init = true;
		// check velikost tabulky
		if( currFuncSize == funcSize ){
			ret = resizeFuncField();
			if(ret != S_BEZ_CHYB) {
				return ret;
			}
		}	

		
		//nastaveni klice funkce
		func[currFuncSize].key = allocString(funkce);

		//inicializace tabulky
		ret = htInit(&func[currFuncSize].table );
		if(ret != S_BEZ_CHYB) {
			return ret;
		}
		//vytovreni listu instrukci
		InitList( &(func[currFuncSize].instrList) );
		//nastaveni ukazatele na prave vytvoreny list instrukci
		listIntrukci = &(func[currFuncSize].instrList);
		//nastaveni ukazatele na aktualni tabulku symbolu
		ptrhtLocal = func[currFuncSize].table;
		//inkrementace indexu 
		currFuncSize++;
		
		nasel = htSearch(ptrhtGlobal, funkce);		
		if(nasel != NULL) {
			//nakopirovani parametru do lokalniho ramce
			for(int i=0; i < nasel->data->param.numParam; i++) {
				htDeclInsert(ptrhtLocal, nasel->data->param.param[i], nasel->data->param.typeParam[i], ID_PARAM);	
			}
		}	
		return ACT_LIST();
	}
}

/*
DTYPE2->integer
DTYPE2->real
DTYPE2->string
DTYPE2->boolean
*/
tChyba DTYPE2() {
	if(token.stav == s_cele_cislo) {
				
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_desetinne_cislo) {
		
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_string) {
		
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_logicka_hodnota) {
		
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

/*
DTYPE->integer							
DTYPE->real							
DTYPE->string							
DTYPE->boolean	
*/
tChyba DTYPE() {
	if(!strcmp(token.data, "integer") && token.stav == s_klicove) {
				
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "real") && token.stav == s_klicove) {
		
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "string") ) {
		
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "boolean") && token.stav == s_klicove) {
		
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba ROZHODNI() {
	
	int analyza2;
	int analyza;
	int analyza1;
	TItem *pt;
	//ROZHODNI->DTYPE	
	if((analyza = DTYPE()) == S_BEZ_CHYB) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}	
		return S_BEZ_CHYB;
	}
	//ROZHODNI->PROM VESTAV EXPR
	else {
		//volani funkce na vestavene funkce	
		analyza2 = VESTAV();
		if(analyza2 != S_BEZ_CHYB && analyza2 != S_EPS) {
			return analyza2;
		}
		//pokud je epsilon pokracujeme dal
		if(analyza2 == S_BEZ_CHYB) {
			return S_BEZ_CHYB;
		}
				
		if((porovnani = malloc(strlen(token.data)+1)) == NULL) {
			return S_INTERNI_CHYBA;
		}
		//ulozeni hodnoty token.data do globalni promenne porovnani
		strcpy(porovnani, token.data);
		pt = htSearch(ptrhtGlobal, token.data);		
		//kontrola jestli id je v tabulce globalni	
		if(pt != NULL && pt->druh == ID_FUNCTION) {		
			//pokud ano a je to funkce vola se prom
			analyza1 = PROM();
			if(analyza1 != S_BEZ_CHYB) {
				return analyza1;
			}
				
			free(porovnani);
			return S_BEZ_CHYB;	
		}
		else {//jinak se vola precedencni	
			analyza2 = precedencniSA();
			if(analyza2 != S_BEZ_CHYB) {
				return analyza2;
			}
			
			//instrukce pro prirazeni, tmp1 je vysledek precedencni, tmp3 cilove id
			generateInstruction( OC_PRIRAZENI, searchFrames(neterminal.polozkaTS.key, ptrhtLocal, ptrhtGlobal), NULL, searchFrames(id, ptrhtLocal, ptrhtGlobal) );
				
			return S_BEZ_CHYB;
				
		}
	}
}


tChyba LEXEM() {
	//LEXEM->:=
	if(token.stav == s_prirazeni) {
		prirovnani = true;				//zapamatovani, ze slo o prirovnani
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}	
		return S_BEZ_CHYB;
	}//LEXEM->:
	else if(token.stav == s_dvojtecka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba UKONCOVACI() {
	//UKONCOVACI->end	
	if(!strcmp(token.data, "end") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {								//osetreni stredniku pokud je dalsi token end
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}//UKONCOVACI->; ACT_LIST
	else if(token.stav == s_strednik) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		//pokud za strednikem je end -> chyba
		if(!strcmp(token.data, "end") && token.stav == s_klicove) {
			return S_SYNTAKTICKA_CHYBA;
		}

		return ACT_LIST();
	}
	return S_SYNTAKTICKA_CHYBA;
}


tChyba ACT_LIST() {
	int analyza;
	int dtype2=0;
	TItem *pt;
	//ACT_LIST->end	
	if(!strcmp(token.data, "end") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	//ACT_LIST->begin ACT_LIST 
	else if(!strcmp(token.data, "begin") && token.stav == s_klicove) {
		
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return ACT_LIST();
	}
	//ACT_LIST->id LEXEM ROZHODNI UKONCOVACI
	else if(token.stav == s_identifikator) {

		if((id = malloc(strlen(token.data)+1)) == NULL)	{
				return S_INTERNI_CHYBA;
			}
		//kopirovani z token.data do globalni		
		strcpy(id, token.data);	
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		analyza = LEXEM();

		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		
		//pokud se jedna o deklaraci id, dojde k ulozeni jeho datoveho typu
		if(!strcmp(token.data, "integer")) {
			dtype2 = TYPEINT;
		}
		else if(!strcmp(token.data, "string")) {			
			dtype2 = TYPESTR;
		}
		else if(!strcmp(token.data, "real")) {
			dtype2 = TYPEDOUBLE;
		}
		else if(!strcmp(token.data, "boolean")) {
			dtype2 = TYPEBOOL;
		}
		//pokud je prirazeni zkontroluj ze je v nejake tabulce
		if(prirovnani == true) {
			pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
			if(pt != NULL) {
				prirovnani = false;
			}
			else {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}
		}

		analyza = ROZHODNI();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		//pokud byla deklarace, ulozime id do lokalniho ramce
		if(dtype2 != 0) {									
			htDeclInsert(ptrhtLocal,id, dtype2, ID_LOCAL);			
			free(id);
		}
		
		analyza = UKONCOVACI();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "var") && token.stav == s_klicove) {
		//ACT_LIST->var id : DTYPE ; ACT_LIST

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		if(token.stav == s_identifikator) {
			if((id = malloc(strlen(token.data)+1)) == NULL)	{
				return S_INTERNI_CHYBA;
			}
			//nesmi se id jmenovat stejne jak tyto funkce
			if(!strcmp(token.data, "length") || !strcmp(token.data, "copy")) {			
				return S_SEMANTICKA_CHYBA_NEDEF;
			}
			//pokud jiz nazev existuje -> chyba
			pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);				
			if(pt != NULL) {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}

			strcpy(id, token.data);		
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_dvojtecka) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				analyza = DTYPE();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}
				
				if(!strcmp(token.data, "integer")) {
					dtype = TYPEINT;
				}
				else if(!strcmp(token.data, "string")) {
					dtype = TYPESTR;
				}
				else if(!strcmp(token.data, "real")) {
					dtype = TYPEDOUBLE;
				}
				else if(!strcmp(token.data, "boolean")) {
					dtype = TYPEBOOL;
				}
			
				//vlozeni id do lokalniho ramce
				htDeclInsert(ptrhtLocal,id, dtype, ID_LOCAL);		
				free(id);
				
				
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}	

				else if(token.stav == s_strednik) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}
					return ACT_LIST();
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	//ACT_LIST->if EXPR then CYKLUS else CYKLUS UKONCOVACI
	else if(!strcmp(token.data, "if") && token.stav == s_klicove) {
		
		//inkrementace labelu -> vime kolik je vnorenych ifu v sobe
		lab++;
		//vlozeni hodnoty lab do TItem data
		TItem *tmp = (TItem*)malloc(sizeof(TItem));
		tmp->type = TYPEINT;
		tmp->init = true;
		tmp->data = (TData*)malloc(sizeof(TData));
		tmp->data->intNumber = lab;

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		//po if nasleduje vyraz, zavolame precedencni na vyhodnoceni	
		analyza = precedencniSA();			
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		//generovani skoku, tmp1 je logicka hodnota, tmp3 je lab
		generateInstruction(OC_GOTO, searchFrames(neterminal.polozkaTS.key, ptrhtLocal, ptrhtGlobal), NULL, tmp );
		if(!strcmp(token.data, "then") && token.stav == s_klicove) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
		
			analyza = CYKLUS();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			//generovani skoku za else
			generateInstruction(OC_GOTOAFTER, searchFrames(neterminal.polozkaTS.key, ptrhtLocal, ptrhtGlobal), NULL, tmp );
			if(!strcmp(token.data, "else") && token.stav == s_klicove) {
				//generovani navesti skok do else
				generateInstruction(OC_ELSE, NULL, NULL, tmp);
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
				
				analyza = CYKLUS();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}
				//generovani navesti skok za else
				generateInstruction(OC_AFTER_ELSE, NULL, NULL, tmp);
				lab--;
				return UKONCOVACI();
			}
			return S_SYNTAKTICKA_CHYBA;
			
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	//ACT_LIST->while EXPR do CYKLUS  UKONCOVACI
	else if(!strcmp(token.data, "while") && token.stav == s_klicove) {
		
		//opet na zjisteni jak moc jsme zanoreni
		lab_while++;
		TItem *tmp = (TItem*)malloc(sizeof(TItem));
		tmp->type = TYPEINT;
		tmp->init = true;
		tmp->data = (TData*)malloc(sizeof(TData));
		tmp->data->intNumber = lab_while;
		
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		//generovani navesti skok pred vyhodnoceni podminky
		generateInstruction(OC_WHILE_PRED, NULL, NULL, tmp);
		
		analyza = precedencniSA();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		//generovani skoku, tmp1 je logicka hodnota, tmp3 je lab
		generateInstruction(OC_GOTO_WHILE, searchFrames(neterminal.polozkaTS.key, ptrhtLocal, ptrhtGlobal), NULL, tmp );
		if(!strcmp(token.data, "do") && token.stav == s_klicove) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			analyza = CYKLUS();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			//generovani skoku zpet na zacatek 
			generateInstruction(OC_GOTO_WHILE, searchFrames(neterminal.polozkaTS.key, ptrhtLocal, ptrhtGlobal), NULL, tmp );
			//navesti pro skonceni cyklu
			generateInstruction(OC_WHILE_AFTER, NULL, NULL, tmp);
			lab_while--;
			return UKONCOVACI();
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	//ACT_LIST->readln ( id ) UKONCOVACI
	else if(!strcmp(token.data, "readln") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_identifikator) {
				//pokud zadany parametr nebyl deklarovan -> chyba
				pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);				
				if(pt == NULL) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}
				//pokud je datovy typ parametru boolean -> chyba
				if(pt->type == TYPEBOOL) {
					return S_SEMANTICKA_CHYBA_TYPOVA;
				}
				//generovani instrukce read, v token.data je id
				generateInstruction(OC_READ, NULL, NULL, searchFrames(token.data,ptrhtLocal,ptrhtGlobal));		
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_prava_zavorka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}

					analyza = UKONCOVACI();
					if(analyza != S_BEZ_CHYB) {
						return analyza;
					}
					return S_BEZ_CHYB;
					
				}
			}
			else {
				return S_SEMANTICKA_CHYBA_TYPOVA;
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	//ACT_LIST->write TERM UKONCOVACI
	else if(!strcmp(token.data, "write") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		
			analyza = TERM();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			
			analyza = UKONCOVACI();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			
			return S_BEZ_CHYB;
		
	}
	return S_SYNTAKTICKA_CHYBA;
}
//CYKLUS->begin ACT_LIST
tChyba CYKLUS() {
	if(!strcmp(token.data, "begin") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return ACT_LIST();
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba DEKLARACE() {
	int analyza;
	TItem *pt;
	//DEKLARACE->id : DTYPE ; DEKLARACE
	if(token.stav == s_identifikator) {
		if((id = malloc(strlen(token.data)+1)) == NULL) {
			return S_INTERNI_CHYBA;
		}
		//id se nesmi jmenovat jako tyto funkce
		if(!strcmp(token.data, "length") || !strcmp(token.data, "copy")) {		
				return S_SEMANTICKA_CHYBA_NEDEF;
		}
		//id musi byt deklarovano jinak -> chyba
		pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
		if(pt != NULL) {								
			return S_SEMANTICKA_CHYBA_NEDEF;
		}

		strcpy(id, token.data);		
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_dvojtecka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			analyza = DTYPE();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			
			if(!strcmp(token.data, "integer")) {
				dtype = TYPEINT;
			}
			else if(!strcmp(token.data, "string")) {
				dtype = TYPESTR;
			}
			else if(!strcmp(token.data, "real")) {
				dtype = TYPEDOUBLE;
			}
			else if(!strcmp(token.data, "boolean")) {
				dtype = TYPEBOOL;
			}
			
			//vlozime do globalniho ramce globalni promennou		
			htDeclInsert(ptrhtGlobal,id, dtype, ID_GLOBAL);		
			free(id);
		
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}	

			else if(token.stav == s_strednik) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
				return DEKLARACE();
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	//DEKLARACE->eps	
	else if((!strcmp(token.data, "function")  || !strcmp(token.data, "begin")) && token.stav == s_klicove) {
		return S_BEZ_CHYB;		//pokud dalsi token je jeden z vyse uvedenych jedna se o epsilon pravidlo a vracime OK
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba TERM() {
	int analyza;
	TItem *pt, *pt2;
	
	//TERM->( DRUH TERM
	if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
		if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
			analyza = DRUH();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			
			if(token.stav == s_identifikator) {
				//pokud jsme v definici funkce
				if(byla_funkce == true) {
					pt = searchFrames(funkce, ptrhtLocal, ptrhtGlobal);
					if(pt != NULL) {
						//kontrolujeme, zda zadany argument je parametr funkce
						for (int i = 0; i < pt->data->param.numParam; ++i){	
							if(!strcmp(token.data, pt->data->param.param[i])){
								
								break;
							}
						}
					}
				}		
				//zjistime, zda id je v nejake tabulce
				pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
				if(pt2 == NULL) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}
				//generovani instrukce write
				generateInstruction( OC_WRITE, pt2 , NULL, NULL );
						
								
			}
			else{//pokud neni id, ale primo hodnota
				TItem *str = (TItem*)malloc(sizeof(TItem));
				str->druh = PRINTSTR;
				str->type = TYPESTR;
				str->init = true;
				str->data = (TData*)malloc(sizeof(TData));
				str->data->str = allocString(token.data);
				//jako nahore
				generateInstruction( OC_WRITE, str , NULL, NULL );
			}

			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
			return TERM();
		}	
	}
	//TERM->, DRUH TERM
	else if(token.stav == s_carka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
			analyza = DRUH();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}

			if(token.stav == s_identifikator) {
				//pokud jsme v definici funkce hledej i na parametry, jinak je ve "funkce" nic.
				if(byla_funkce == true) {
					pt = searchFrames(funkce, ptrhtLocal, ptrhtGlobal);
					if(pt != NULL) {
						for (int i = 0; i < pt->data->param.numParam; ++i){	
							if(!strcmp(token.data, pt->data->param.param[i])){
								
								break;
							}
						}
					}
				}		

				pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
				if(pt2 == NULL) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}//generovani instrukce write
				generateInstruction( OC_WRITE, pt2 , NULL, NULL );
			}
			else{//pokud je argument hodnota
				TItem *str = (TItem*)malloc(sizeof(TItem));
				str->druh = PRINTSTR;
				str->init = true;
				str->type = TYPESTR;
				str->data = (TData*)malloc(sizeof(TData));
				str->data->str = allocString(token.data);
				generateInstruction( OC_WRITE, str , NULL, NULL );
			}

			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
			return TERM();
		}
	}
	else if(token.stav == s_prava_zavorka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba TERM2() {
	int analyza;
	TItem *pt2;
	TItem *pt;
	bool nasel = false;
	//TERM2->( DRUH TERM2
	if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
		if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
			analyza = DRUH();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}//pokud nejsme v definici funkce, pridej do nove vznikleho ramce zadany argument
			if(byla_funkce == false) {
				int ret = addArgument(token.data);
				if( ret != S_BEZ_CHYB ){
					return ret;
				}
			}
			//vyhledani volane funkce
			pt = htSearch(ptrhtGlobal, porovnani);
			//vyhledani funkce, ve ktere se vola funkce
			pt2 = htSearch(ptrhtGlobal, funkce); 
			if(token.stav == s_identifikator) {
					//pokud to bylo id
				if(pt != NULL) {
					for (int i = 0; i < pt2->data->param.numParam; ++i){			//prochazime pole parametru
						if(!strcmp(token.data, pt2->data->param.param[i])) {		//pokud najdeme parametr tak nastavime na true
							if(pt->data->param.typeParam[i] != pt2->data->param.typeParam[i]) { //kontrolujeme jestli datove typy parametru jsou stejne
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}

							nasel = true;
							break;
						}				
					}

					if(nasel != true) { //POKUD NENI PARAMETR PROJDEME 
						pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal); 
						if(pt2 != NULL) {
							//kontrola datovych typu parametru funkce a argumentu		
							if(pt->data->param.typeParam[pocitadlo] != pt2->type) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}		
						}	
						else {
							return S_SEMANTICKA_CHYBA_NEDEF;
						}
					}		
				}	
				else {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}			

				nasel = false;
			}
			else {
				if(pt != NULL) {
					if(token.stav != pt->data->param.typeParam[pocitadlo]) {
						return S_SEMANTICKA_CHYBA_TYPOVA;
					}
				}
				else {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}	
			}

			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
			pocitadlo++;
			return TERM2();
		}	
	}
	//TERM2->, DRUH TERM2
	else if(token.stav == s_carka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		//pridani argumentu do vznikle tabulky
		if(byla_funkce == false) {
			int ret = addArgument(token.data);
			if( ret != S_BEZ_CHYB ){
				return ret;
			}
		}
		if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
			analyza = DRUH();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			//vyhledani volane funkce
			pt = htSearch(ptrhtGlobal, porovnani);
			//vyhledani funkce, ve ktere se vola funkce
			pt2 = htSearch(ptrhtGlobal, funkce); 
			if(token.stav == s_identifikator) {
				if(pt != NULL) {	
					for (int i = 0; i < pt2->data->param.numParam; ++i){
						if(!strcmp(token.data, pt2->data->param.param[i])) {
							//kontrola zda souhlasi datove typy parametru 2 funkci
							if(pt->data->param.typeParam[i] != pt2->data->param.typeParam[i]) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}

							nasel = true;
							break;
						}				
					}
					//pokud to neni parametr, kontrola jestli je id v tabulkach
					if(nasel != true) {
						pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
						if(pt2 != NULL) {			
							if(pt->data->param.typeParam[pocitadlo] != pt2->type) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}	
						}
						else {
							return S_SEMANTICKA_CHYBA_NEDEF;
						}
					}		
				}	
				else {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}			
				
				nasel = false;
			}
			else {//kontrola datovych typu
				if(token.stav != pt->data->param.typeParam[pocitadlo]) {
					return S_SEMANTICKA_CHYBA_TYPOVA;
				}
			}	

		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		pocitadlo++;
		return TERM2();
		
	}
	else if(token.stav == s_prava_zavorka) {
		pt = htSearch(ptrhtGlobal, porovnani);			
		//zjistime, zda pocet zadanych parametru odpovida poctu parametru pri deklaraci funkce
		if(pocitadlo != pt->data->param.numParam) {
			return S_SEMANTICKA_CHYBA_TYPOVA;
		}
		
		if(byla_funkce == true) {//pokud jsme v definici funkce
			//generovani instrukce fake call
			generateInstruction(OC_COLL, htSearch(ptrhtGlobal, porovnani ), NULL, NULL );
			generateInstruction(OC_NEPRIRAZENI, NULL, NULL, NULL );
		}
		else{
			//vygenerovani instrukci InsertFirst
			for (int i = 0; i < MAX_HTSIZE; ++i){
				if( (*ptrhtLocal)[i] ){
					if( (*ptrhtLocal)[i]->druh == ID_FUNCTION  ){
						generateCopyInstr(OC_CPY, (*ptrhtLocal)[i], NULL, NULL );
					}
				}
			}
			//predchozi list instrukci, protoze se volal push
			listIntrukci = &(ptrStack->top->lower->field);
			//htPrintTable(ptrhtGlobal);
			//param CALL, Funkce v GLOBALCE, NULL, NAVRATOVKA
			generateInstruction(OC_CALL, htSearch(ptrhtGlobal, porovnani ), NULL, htSearch( ptrhtLocal, porovnani ) );
			//PRIRAZ, NAVRATOVKA, NULL, IDCKO
			generateInstruction(OC_PRIRAZENI, htSearch( ptrhtLocal, porovnani ), NULL, searchFrames( id, ptrhtLastLocal, ptrhtGlobal ) );
			//nastaveni spravneho listu instrukci
			listIntrukci = &(ptrStack->top->field);
			//instrukce return na konci volane fce
			generateInstruction(OC_RET, NULL, NULL, NULL );
		}
		//globalni promenna -> musime nulovat
		pocitadlo = 0;								 
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba DRUH() {
	int analyza;
	//DRUH->DTYPE2	
	if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string) {
		analyza = DTYPE2();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return S_BEZ_CHYB;
	}
	//DRUH->id
	else if(token.stav == s_identifikator) {
					//zapamatujeme si, ze bylo zadano id
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;		
}



int syntakticka_anal() {		//pomocna funkce pro kontrolu cele syntakticke analyzy v main.c souboru
	int navrat;

	tokenInit(&token);
	token = getNextToken();
	if(token.stav == s_lex_error) {
		navrat = S_LEXIKALNI_CHYBA;
	}
	else
	navrat = PROGRAM();
    tokenFree(&token);
    if(navrat == S_BEZ_CHYB) {
    	navrat = TestFunction(ptrhtGlobal);
    }
  	return navrat;
}



