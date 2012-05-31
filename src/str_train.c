#include "str_train.h"




wagon* nextWagon (train_extended* t, wagon* w) {
  wagon *w2= (wagon*)((char*)w+(w->header.len)); 
	if ((char*)w2 - (char*)t >= t->pfx.len)
    return NULL;
  else
    return w2;
}


bool is_in_lts(address  ad, lts_array ltsarray) {
	bool result=false;
	int i;
	for (i=0;i<ntr;i++) {
		result=result && addr_ismember(ad,ltsarray[i].circuit);
	}
	return result;
}

