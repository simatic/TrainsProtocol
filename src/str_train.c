#include "str_train.h"




wagon* nextWagon (train_extended* t, wagon* w) {
  wagon *w2= (wagon*)((char*)w+(w->header.len)); 
	if ((char*)w2 - (char*)t >= t->pfx.len)
    return NULL;
  else
    return w2;
}


