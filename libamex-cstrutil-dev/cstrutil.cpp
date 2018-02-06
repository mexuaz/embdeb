#include "cstrutil.h"


#include <ctype.h> // tolower() toupper() isspace()
#include <string.h> // strlen()

using namespace amex;

char *strtolower(char *str) {
    for(; *str; str++) {
        *str = tolower(*str);
    }
    return str;
}

char *strtoupper(char *str) {
    for(; *str; str++) {
        *str = toupper(*str);
    }
    return str;
}

char *trimleft(char *str) {
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    return str;
}

char *trimright(char *str) {
    // Trim trailing space
    char* end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) {
        end--;
    }

    // Write new null terminator
    *(end+1) = 0;

    return str;
}


char *trim(char *str) {


    // Trim leading space
    amex::trimleft(str);


    if(*str == 0)  // All spaces?
      return str;

    // Trim trailing space
    amex::trimright(str);

    return str;
}
