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

//deklarace funkci, ktere budou zmineny driv, nez budou implementovany
int dtype;
TData *dat;
bool init;
bool prirovnani;
int pocitadlo;
int pocitani_parametru=0;
bool bylo_id;

char* term_id;
char* porovnani;
char* id;
char* funkce;
char* params;
tToken token;


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
	if( ptr->druh == 7 ){ // string
	printf(" %d ", ptr->druh);

	for (int i = 0; i < ptr->data->param.numParam; i++){
		printf("%s ", ptr->data->param.param[i]);
		
	}
}
else if( ptr->druh == 8 ){ // bool
printf(" %d", ptr->druh);
}
else if( ptr->druh == 9 ){ // int
printf(" %d", ptr->druh);
}
else if( ptr->druh == 10 ){ // double

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






tChyba PROGRAM () {					//funkce na pravidlo PROGRAM -> FUNKCE . eof
							//nacitani prvniho tokenu je v komentu, protoze na konci je pomocna funkce, kde se uz nacita token, pri vyslednem kodu, bude toto nacteni tokenu odkomentovano
	/*token = getNextToken();
	if(token.stav == s_lex_error) {			//nacteni prvniho tokenu a overeni, zda byl spravne nacteny
		return S_LEXIKALNI_CHYBA;
	}*/
	int analyza;					//lokalni promenna, do ktere se bude nacitat vystup z volane funkce a v pripade chyby se bude obsah promenne vracet
	
	int ret = initFrames();
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
	dat = (TData*)malloc(sizeof(TData));
	dat->param.numParam = 0;				//nulovani pocitadla parametru funkce pokazde, kdyz se uklada funkce do tabulky

						//opet lokalni promenna, stejna funkce jako ve funkci nahore
	TItem *nasel;
	if(!strcmp(token.data, "begin") && token.stav == s_klicove) {
		token = getNextToken();			//pres strcmp porovnavam data tokenu (prakticky primo to, co se nacte)
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		analyza = ACT_LIST();			//po zavolani funkce se getToken nevola, protoze kazda funkce, nez vrati hodnotu, nacte dalsi token
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "function") && token.stav == s_klicove) {
		
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_identifikator) {	//pokud nacitame a porovnavame terminaly, tak po kazdem porovnani nacitame dalsi token, aby se neporovnavalo to same
			if(!strcmp(token.data, "length") || !strcmp(token.data, "copy")) {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}

			if((funkce = malloc(strlen(token.data)+1)) == NULL) {	
				return S_INTERNI_CHYBA;
			}
			
			
			strcpy(funkce, token.data);
			nasel = htSearch(ptrhtGlobal,funkce);		//pokud id funkce jiz je v globalni hash tabulce a nejedna se o doprednou deklaraci -> chyba
			if(nasel != NULL ) {
				if(nasel->init == false) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}
			}

			htInsert(ptrhtGlobal,funkce,dat, 4,0);		//vlozeni nazvu funkce do globalni hash
			token = getNextToken();
			if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_leva_zavorka) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				analyza = PARAMS();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
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

						analyza = DOPREDNE();
						if(analyza != S_BEZ_CHYB) {
							return analyza;
						}

												
						if(init == true) {
							htCompleteInsert(ptrhtGlobal,funkce, ID_FUNCTION, dtype, true);				
							free(funkce);
							
						}																				//dokonceni vkladani funkce do tabulky, konkretne datovy typ a zda se jedna o doprednou deklaraci ci nikoliv
						else if (init == false) {
							htCompleteInsert(ptrhtGlobal,funkce, ID_FUNCTION, dtype, false);	
							free(funkce);

						}

						if(token.stav == s_strednik) {
							
							token = getNextToken();
							if(token.stav == s_lex_error) {
								return S_LEXIKALNI_CHYBA;
							}								//volani interpretu
							return FUNKCE();
						}

								//jakmile je vse splneno, vola funkce sama sebe znovu, protoze podle pravidel, jakmile se dojede na konec, tak je potreba zavolat FUNKCE() znovu, protoze napriklad funkci muze byt v kodu deklarovano mnoho a ne jen jedna
						
					}
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;		//pokud se nacte token, ktery podle pravidla tam nepatri, vrati se syntakticka chyba
	}
	else if(!strcmp(token.data, "var") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		analyza = DEKLARACE();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return FUNKCE();
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba PARAMS() {
	int analyza;
	if(token.stav == s_identifikator) {
		if((params = malloc(strlen(token.data)+1)) == NULL) {
			return S_INTERNI_CHYBA;
		}
		
		strcpy(params, token.data);			//do globalni promenne params nacteme id parametru funkce
		
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
			
			htParamInsert(ptrhtGlobal,funkce, params, dtype);			//vkladame parametr funkce do globalni hash k dane funkci
			free(params);

			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
	
		        return PARAMS();
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(token.stav == s_strednik) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		if(token.stav == s_identifikator) {
			
			if((params = malloc(strlen(token.data)+1)) == NULL) {
				return S_INTERNI_CHYBA;
			}
			
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
					
				htParamInsert(ptrhtGlobal,funkce, params, dtype);
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
	
	if(token.stav == s_identifikator) {
		ret = pushFrame();
		if(ret != S_BEZ_CHYB) {
			return ret;
		}

		
		ret = copyTable( func[currFuncSize-1].table, ptrhtLocal );
		if(ret != S_BEZ_CHYB) {
			return ret;
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

			analyza = TERM2();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			else {
				popFrame();
				return S_BEZ_CHYB;
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	return S_SYNTAKTICKA_CHYBA;		
}

tChyba VESTAV() {
	TItem *pt;
	if(!strcmp(token.data, "length") && token.stav == s_klicove) {
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
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);				//prohledani lokalniho i globalniho ramce a pokud id neni nalezeno nebo ma jiny datovy typ nez string -> chyba
					if(pt != NULL) {
						if(pt->init != false) {
							if(pt->type != s_string) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}
						}
						else {
							return S_NEINICIALIZOVANA_PROMENNA;
						}	
					}
					else
						return S_SEMANTICKA_CHYBA_NEDEF;
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
	else if(!strcmp(token.data, "copy") && token.stav == s_klicove) {
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
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);			//prohledani lokalniho i globalniho ramce a pokud id neni nalezeno nebo ma jiny datovy typ nez string -> chyba
					if(pt != NULL) {
						if(pt->init != false) {
							if(pt->type != s_string) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}
						}
						else {
							return S_NEINICIALIZOVANA_PROMENNA;
						}	
					}
					else
						return S_SEMANTICKA_CHYBA_NEDEF;
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

					else if(token.stav == s_cele_cislo || token.stav == s_identifikator) {
						if(token.stav == s_identifikator) {
							pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);			//prohledani lokalniho i globalniho ramce a pokud id neni nalezeno nebo ma jiny datovy typ nez string -> chyba
							if(pt != NULL) {
								if(pt->init != false) {
									if(pt->type != s_string) {
										return S_SEMANTICKA_CHYBA_TYPOVA;
									}
								}
								else {
									return S_NEINICIALIZOVANA_PROMENNA;
								}	
							}
							else
								return S_SEMANTICKA_CHYBA_NEDEF;
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

							else if(token.stav == s_cele_cislo || token.stav == s_identifikator) {
								if(token.stav == s_identifikator) {
									pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
									if(pt != NULL) {
										if(pt->init != false) {
											if(pt->type != s_string) {
												return S_SEMANTICKA_CHYBA_TYPOVA;
											}
										}
										else {
											return S_NEINICIALIZOVANA_PROMENNA;
										}	
									}
									else
										return S_SEMANTICKA_CHYBA_NEDEF;
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
	else if(!strcmp(token.data, "find") && token.stav == s_klicove) {
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
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
					if(pt != NULL) {
						if(pt->init != false) {
							if(pt->type != s_string) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}
						}
						else {
							return S_NEINICIALIZOVANA_PROMENNA;
						}	
					}
					else
						return S_SEMANTICKA_CHYBA_NEDEF;
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

					else if(token.stav == s_string || token.stav == s_identifikator) {
						if(token.stav == s_identifikator) {
							pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
							if(pt != NULL) {
								if(pt->init != false) {
									if(pt->type != s_string) {
										return S_SEMANTICKA_CHYBA_TYPOVA;
									}
								}
								else {
									return S_NEINICIALIZOVANA_PROMENNA;
								}	
							}
							else
								return S_SEMANTICKA_CHYBA_NEDEF;
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
	else if(!strcmp(token.data, "sort") && token.stav == s_klicove) {
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
					if(pt != NULL) {
						if(pt->init != false) {
							if(pt->type != s_string) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}
						}
						else {
							return S_NEINICIALIZOVANA_PROMENNA;
						}	
					}
					else
						return S_SEMANTICKA_CHYBA_NEDEF;
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
	
	if(!strcmp(token.data, "forward") && token.stav == s_klicove) {			
		init = true;						//nastaveni na true pokud se jedna o doprednou deklaraci funkce
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	else {

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

		ptrhtLastLocal = ptrhtLocal;
		ptrhtLocal = func[currFuncSize].table;
		currFuncSize++;
		//kopirovani tabulky
		//ret = copyTable( ptrhtLocal, tmp );



		nasel = htSearch(ptrhtGlobal, funkce);		//nakopirovani parametru do lokalniho ramce
		if(nasel != NULL) {
			for(int i=0; i < nasel->data->param.numParam; i++) {
				htDeclInsert(ptrhtLocal, nasel->data->param.param[i], nasel->data->param.typeParam[i], ID_PARAM);	
			}
		}	
		return ACT_LIST();
	}
}



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

tChyba DTYPE() {
	if(!strcmp(token.data, "integer") && token.stav == s_klicove) {
				
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "real") && token.stav == s_klicove) {
		
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "string") || token.stav == s_string) {
		
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
	if((analyza = DTYPE()) == S_BEZ_CHYB) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}	
		return S_BEZ_CHYB;
	}
	else {
		
		analyza2 = VESTAV();
		if(analyza2 != S_BEZ_CHYB && analyza2 != S_EPS) {
			return analyza2;
		}

		if(analyza2 == S_BEZ_CHYB) {					//pokud vrati bez chyb ukonci se
			return S_BEZ_CHYB;
		}
			
		if(token.stav == s_identifikator) {
			if((porovnani = malloc(strlen(token.data)+1)) == NULL) {
				return S_INTERNI_CHYBA;
			}

			strcpy(porovnani, token.data);
			pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);		
			
			if(pt != NULL && pt->druh == ID_FUNCTION) {			//prirazeni, pokud dane id bylo deklarovane a zaroven to je funkce vola se dalsi pravidlo, jinak se vola precedencni
				analyza1 = PROM();
				if(analyza1 != S_BEZ_CHYB) {
					return analyza1;
				}
				
				free(porovnani);
				return S_BEZ_CHYB;	
			}
			else 		
			
			analyza = precedencniSA();
						
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			return S_BEZ_CHYB;
			
		}
		else if(token.stav == s_cele_cislo || token.stav == s_logicka_hodnota || token.stav == s_desetinne_cislo || token.stav == s_string || token.stav == s_leva_zavorka) {
			analyza = precedencniSA();
			if(analyza != S_BEZ_CHYB) {
				return analyza;				//pokud neni id vola se hned precedencni
			}
			return S_BEZ_CHYB;
		}
		return S_SYNTAKTICKA_CHYBA;
	}	

}

tChyba LEXEM() {
	if(token.stav == s_prirazeni) {
		prirovnani = true;				//zapamatovani, ze slo o prirovnani
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}	
		return S_BEZ_CHYB;
	}
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
	if(!strcmp(token.data, "end") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {								//osetreni stredniku pokud je dalsi token end
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_strednik) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
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
	if(!strcmp(token.data, "end") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "begin") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return ACT_LIST();
	}
	else if(token.stav == s_identifikator) {
		if((id = malloc(strlen(token.data)+1)) == NULL)	{
				return S_INTERNI_CHYBA;
			}
		
		strcpy(id, token.data);	
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		analyza = LEXEM();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		
		if(prirovnani == true) {				//pokud nejde o deklaraci zjistime, zda bylo id deklarovano
			pt = searchFrames( id, ptrhtLocal, ptrhtGlobal );
			if(pt != NULL) {
				pt->init = true;			//pokud sme nasli, nastavine inicializaci na true, protoze dojde k prirazeni hodnoty do id
				prirovnani = false;			//nastavime na false, aby pri dalsim pruchodu se mohlo zase overovat, jestli se jedna o prirazeni nebo deklaraci
			}
			else {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}
		}

		if(!strcmp(token.data, "integer")) {
			dtype2 = TYPEINT;
		}
		else if(!strcmp(token.data, "string")) {			//pokud se jedna o deklaraci id, dojde k ulozeni jeho datoveho typu
			dtype2 = TYPESTR;
		}
		else if(!strcmp(token.data, "real")) {
			dtype2 = TYPEDOUBLE;
		}
		else if(!strcmp(token.data, "boolean")) {
			dtype2 = TYPEBOOL;
		}

		analyza = ROZHODNI();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		
		if(dtype2 != 0) {									//pokud byla deklarace, ulozime id do lokalniho ramce
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
		ptrhtLocal = ptrhtLastLocal;
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_identifikator) {
			if((id = malloc(strlen(token.data)+1)) == NULL)	{
				return S_INTERNI_CHYBA;
			}
			
			if(!strcmp(token.data, "length") || !strcmp(token.data, "copy")) {			//nesmi se id jmenovat stejne jak tyto funkce
				return S_SEMANTICKA_CHYBA_NEDEF;
			}

			pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);				//pokud jiz nazev existuje -> chyba
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
			
				
				htDeclInsert(ptrhtLocal,id, dtype, ID_LOCAL);		//vlozeni id do lokalniho ramce
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
	else if(!strcmp(token.data, "if") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		

		analyza = precedencniSA();			//po if nasleduje vyraz, zavolame precedencni na vyhodnoceni
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		
		if(!strcmp(token.data, "then") && token.stav == s_klicove) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
		
			analyza = CYKLUS();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}

			else if(!strcmp(token.data, "else") && token.stav == s_klicove) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
				
				analyza = CYKLUS();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}
				return UKONCOVACI();
			}
			
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(!strcmp(token.data, "while") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		analyza = precedencniSA();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}

		if(!strcmp(token.data, "do") && token.stav == s_klicove) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			analyza = CYKLUS();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			return UKONCOVACI();
		}
		return S_SYNTAKTICKA_CHYBA;
	}
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
				pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);				//pokud zadany parametr nebyl deklarovan -> chyba
				if(pt == NULL) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}

				if(pt->init == false) {
					return S_NEINICIALIZOVANA_PROMENNA;
				}

				if(pt->type == TYPEBOOL) {
					return S_SEMANTICKA_CHYBA_TYPOVA;
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
	else if(!strcmp(token.data, "write") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		if(token.stav == s_leva_zavorka) {
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
	return S_SYNTAKTICKA_CHYBA;
}

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
	if(token.stav == s_identifikator) {
		if((id = malloc(strlen(token.data)+1)) == NULL) {
			return S_INTERNI_CHYBA;
		}
		
		if(!strcmp(token.data, "length") || !strcmp(token.data, "copy")) {		//id se nesmi jmenovat jako tyto funkce
				return S_SEMANTICKA_CHYBA_NEDEF;
		}

		pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
		if(pt != NULL) {								//id musi byt deklarovano jinak -> chyba
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
			
					
			htDeclInsert(ptrhtGlobal,id, dtype, ID_GLOBAL);		//vlozime do globalniho ramce globalni promennou
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
	else if((!strcmp(token.data, "function")  || !strcmp(token.data, "begin")) && token.stav == s_klicove) {
		return S_BEZ_CHYB;		//pokud dalsi token je jeden z vyse uvedenych jedna se o epsilon pravidlo a vracime OK
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba TERM() {
	int analyza;
	TItem *pt;
	if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
		analyza = DRUH();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}

		if(bylo_id == true) {
			pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
			if(pt != NULL) {
				if(pt->init == false) {
					return S_NEINICIALIZOVANA_PROMENNA;	
				}
			}
			else {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}
			bylo_id = false;
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return TERM();
	}
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

			if(bylo_id == true) {
				pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
				if(pt != NULL) {
					if(pt->init == false) {
						return S_NEINICIALIZOVANA_PROMENNA;	
					}
				}
				else {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}
				bylo_id = false;
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
	
	TItem *pt;
	TItem *pt2;
	if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
		analyza = DRUH();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		
		pt = htSearch(ptrhtGlobal, porovnani);			//pokud se jedna id, ktere davame jako parametr, vyhledame odpovidajici funkci a overime, zda datovy typ vkladaneho id odpovida typu parametru pri deklaraci funkce
		if(bylo_id == true) {							//slouzi k zapamatovani, zda bylo id 
			pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
			if(pt2 != NULL) {
				if(pt2->init != false) {
					if(pt->data->param.typeParam[pocitadlo] != pt2->type) {
						return S_SEMANTICKA_CHYBA_TYPOVA;
					}	
				}
				else {
					return S_NEINICIALIZOVANA_PROMENNA;
				}
			}
			else {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}
						
			bylo_id = false;
		}
		else {
			if(token.stav != pt->data->param.typeParam[pocitadlo]) {
			return S_SEMANTICKA_CHYBA_TYPOVA;
			}
		}
		
		
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		pocitadlo++;
		return TERM2();
	}
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
			
			pt = htSearch(ptrhtGlobal, porovnani);
			if(bylo_id == true) {
				pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
				if(pt2 != NULL) {
					if(pt->data->param.typeParam[pocitadlo] != pt2->type) {
						return S_SEMANTICKA_CHYBA_TYPOVA;
					}
				}
				else {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}
						
				bylo_id = false;
			}
			else {
				if(token.stav != pt->data->param.typeParam[pocitadlo]) {
				return S_SEMANTICKA_CHYBA_TYPOVA;
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
	else if(token.stav == s_prava_zavorka) {
		pt = htSearch(ptrhtGlobal, porovnani);			//zjistime, zda pocet zadanych parametru odpovida poctu parametru pri deklaraci funkce
		if(pocitadlo != pt->data->param.numParam) {
			return S_SEMANTICKA_CHYBA_TYPOVA;
		}
		
		pocitadlo = 0;								//globalni promenna -> musime nulovat 
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
	if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string) {
		analyza = DTYPE2();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_identifikator) {
		
		bylo_id = true;				//zapamatujeme si, ze bylo zadano id
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
	return navrat;
}



