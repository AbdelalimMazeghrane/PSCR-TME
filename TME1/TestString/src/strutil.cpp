// strutil.cpp
#include "strutil.h"

namespace pr {

size_t length(const char* s) {
    size_t len=0;
    size_t i =0;
    while(s[i] != '\0'){
        len++;
        i++;
    }
    return len;
}

char* newcopy(const char* s) {
    char* n=new char[length(s)+1];
    size_t i =0;
    while(s[i] != '\0'){
        n[i]=s[i];
        i++;
    }
    n[i]='\0';
    return n;
}

int compare(const char* a, const char* b) {
    
    size_t i=0;
    
    while(a[i] != '\0' && b[i] != '\0'){
        if(a[i] != b[i]){
            return a[i] - b[i];
        }
        i++;
    }
    return a[i] - b[i];
}


char* newcat (const char *a, const char *b){
        int lena=length(a);
        int lenb=length(b);
        char* n=new char[lena+lenb+1];
        size_t i=0;
        while(a[i] != '\0'){
            n[i]=a[i];
            i++;
        }
        size_t j=0;
        while(b[j] != '\0'){
            n[i]=b[j];
            i++;
            j++;
        }
        n[i]='\0';
        return n;
}

}
