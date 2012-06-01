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


bool is_recent_train(stamp tr_st,lts_array * plts_array, char last_id, int nb_train){
  int waiting_id;
  int diff;

  waiting_id=(last_id++)%ntr;//FIXME -> last_id is a char ?? 0_o
  if(tr_st.id==waiting_id){
    diff=tr_st.lc - ((*plts_array)[waiting_id]).stamp.lc;
    if(diff>0)
      return(diff<((1+NP)/2));//FIXME -> check the NP
    else{ return(diff<((1-NP)/2)); }
  }
  else{ return(false); }
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
