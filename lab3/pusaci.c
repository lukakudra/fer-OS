#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NISTA 0
#define DUHAN 1
#define SIBICE 2
#define PAPIR 3

const char* niz_stanja[]={"NISTA", "DUHAN", "SIBICE", "PAPIR"};

typedef struct {
	int polje_stanja[2];
	pthread_mutex_t kljuc;
	pthread_cond_t prazan;
	pthread_cond_t pun;
} Stol;

Stol stol;

void *trgovac(void *arg) {
	
	while(1) {
		sleep(1);
		pthread_mutex_lock(&stol.kljuc);
		while(stol.polje_stanja[0]!=NISTA && stol.polje_stanja[1]!=NISTA) {
			pthread_cond_wait(&stol.prazan, &stol.kljuc);
		}
		stol.polje_stanja[1]=rand()%3+1;
		stol.polje_stanja[0]=rand()%3+1;
		while(stol.polje_stanja[0]==stol.polje_stanja[1]) {
			stol.polje_stanja[0]=rand()%3+1;
		}
		printf("Trgovac stavlja: %s i %s\n", niz_stanja[stol.polje_stanja[0]], niz_stanja[stol.polje_stanja[1]]);
		pthread_cond_broadcast(&stol.pun);
		pthread_mutex_unlock(&stol.kljuc);
	}
}


void *pusac(void *arg) {
	
	int pusac_id=*(int*)arg;
	while(1) {
		sleep(1);
		pthread_mutex_lock(&stol.kljuc);
		while(stol.polje_stanja[0]==NISTA && stol.polje_stanja[1]==NISTA) {
			pthread_cond_wait(&stol.pun, &stol.kljuc);
		}
		if(stol.polje_stanja[0]!=pusac_id && stol.polje_stanja[1]!=pusac_id) {
			printf("Pusac %d koji je zahtijevao %s i %s sada pusi\n",pusac_id , niz_stanja[stol.polje_stanja[0]], niz_stanja[stol.polje_stanja[1]]);
			stol.polje_stanja[0]=NISTA;
			stol.polje_stanja[1]=NISTA;
			pthread_cond_broadcast(&stol.prazan);
		}
		pthread_mutex_unlock(&stol.kljuc);
	}
}


int main(void) {
	
	int i;
	int broj_pusaca=3;
	int broj_trgovaca=1;
	
	int arg_pusac[broj_pusaca];
	pthread_t id_pusac[broj_pusaca];
	
	int arg_trgovac[broj_trgovaca];
	pthread_t id_trgovac[broj_trgovaca];
	
	srand((unsigned)time(NULL));
	
	for(i=0; i<broj_trgovaca; i++) {
		arg_trgovac[i]=i+1;
		if(pthread_create(&id_trgovac[i], NULL, trgovac, &arg_trgovac[i])) {
			printf("Greska\n");
			exit(1);
		} else {
			printf("Uspjesno stvorena dretva trgovac\n");
		}
	}
	
	for(i=0; i<broj_pusaca; i++) {
		arg_pusac[i]=i+1;
		if(pthread_create(&id_pusac[i], NULL, pusac, &arg_pusac[i])) {
			printf("Greska\n");
			exit(1);
		} else {
			printf("Pusac %d: ima %s\n", i+1, niz_stanja[i+1]);
		}
	}
	
	for(i=0; i<broj_pusaca; i++) {
		pthread_join(id_pusac[i], NULL);
	}
	
	for(i=0; i<broj_trgovaca; i++) {
		pthread_join(id_trgovac[i], NULL);
	}
	
	return 0;	
}
