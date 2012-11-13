#ifndef _ERROR_TRAINS_H
#define _ERROR_TRAINS_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ERROR_AT_LINE(status,errnum,filename,linenum,...) {	\
      fprintf(stderr,"%s:%d:",    \
              filename,linenum);  \
      fprintf(stderr,__VA_ARGS__);  \
      fprintf(stderr,":%s\n",    \
              strerror(errnum));\
       abort();                             \
    }

#endif /* _ERROR_TRAINS_H */
