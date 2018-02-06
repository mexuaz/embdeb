#ifndef _CSTRUTIL_H
#define _CSTRUTIL_H


namespace amex {


    /*
     * Note: All these functions return a pointer to (a substring of) the original string.
     * If the given string was allocated dynamically, the caller must not overwrite
     *  that pointer with the returned value, since the original pointer must be
     *  deallocated using the same allocator with which it was allocated.
     * The return value must NOT be deallocated using free() etc.
     */
    char* strtolower(char* str);
    char* strtoupper(char* str);

    char* trimleft(char* str);
    char* trimright(char* str);
    char* trim(char* str);


}

#endif // _CSTRUTIL_H
