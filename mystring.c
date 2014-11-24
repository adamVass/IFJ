#include "mystring.h"

#include <string.h>

char* allocString( char *str )
{
	if( !str ){
		return NULL;
	}
	char *tmp = (char*)malloc( strlen(str)*sizeof(char)+1 );
	if( !tmp ){
		return NULL;
	}
	strcpy( tmp, str );
	return tmp;
}
