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


void free_wagon(wagon_watcher ww){
  pthread_mutex_lock(&(ww.p_pfx->mutex)); //mutex locked
  ww.p_pfx->counter --; //decrementation of the counter
  if (ww.p_pfx->counter <= 0)
    free(ww.p_wagon);
  pthread_mutex_unlock(&(ww.p_pfx->mutex)); //mutex unlocked
  free(ww.p_pfx);
  free(&ww);
}
