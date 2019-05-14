#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>
#include <sys/msg.h>
#include <values.h>
#include <unistd.h>

int *TRAZIM;
int *BROJ;
int *STOLOVI;

static int broj_stolova, broj_dretvi;

void ulaz_u_kriticni_odsjecak(int i) {
	
	int j, max;
	TRAZIM[i]=1;
	
	max=BROJ[0];
	for(j=0; j<broj_dretvi; j++) {
		 if(BROJ[j]>max) {
		 	max=BROJ[j];
		 }
	}
	
	BROJ[i]=max+1;
	TRAZIM[i]=0;
	
	for(j=0; j<broj_dretvi; j++) {
		while(TRAZIM[j]!=0);
		while(BROJ[j]!=0 && (BROJ[j]<BROJ[i] || (BROJ[j]==BROJ[i] && j<i)));
	}
}

void izlaz_iz_kriticnog_odsjecka(int i) {
	BROJ[i]=0;
}

void ispis_stanja(int broj_stolova) {
	printf(", stanje:\n");
	int i;
	for(i=0; i<broj_stolova; i++) {
		if(STOLOVI[i]!=0) {
			printf("%d", STOLOVI[i]);
		} else {
			printf("-");
		}
	}
	printf("\n");
}

void rezervacija(char*arg) {
	
	int dretva=*((int*)arg);
	int i;
	int trenutno_zauzet=0;
	int broj_zauzetih=0;
	int broj_slobodnih=0;
	int id_stola[broj_stolova];
	
	while(broj_zauzetih<broj_stolova) {
		broj_slobodnih=0;
		
		for(i=0; i<broj_stolova; i++) {
			if(STOLOVI[i]==0) {
				//stol je slobodan
				id_stola[broj_slobodnih]=i;
				broj_slobodnih++;
			}
		}
		
		//opasnost od dijeljenja s nulom
		if(broj_slobodnih==0) {
			break;
		}
		
		sleep(1);
		trenutno_zauzet=rand()%broj_slobodnih;
		
		printf("Dretva %d: odabirem stol %d\n", dretva, id_stola[trenutno_zauzet]+1);
		
		if(STOLOVI[id_stola[trenutno_zauzet]]==0) {
			//stol je slobodan, dretva ga pokusava rezervirati
			ulaz_u_kriticni_odsjecak(dretva);
			STOLOVI[id_stola[trenutno_zauzet]]=dretva;
			printf("Dretva %d: rezerviram stol %d", dretva, id_stola[trenutno_zauzet]+1);
			ispis_stanja(broj_stolova);
			broj_zauzetih++;
		} else {
			//stol je vec rezerviran
			printf("Dretva %d: neuspjela rezervacija stola %d", dretva, id_stola[trenutno_zauzet]+1);
			ispis_stanja(broj_stolova);
		}
		
		izlaz_iz_kriticnog_odsjecka(dretva);
	}
}

	
int main(int argc, char *argv[]) {

	int i;
	broj_dretvi=atoi(argv[1]);
	broj_stolova=atoi(argv[2]);
	int ARG[broj_dretvi];
	pthread_t id[broj_dretvi];
	
	srand((unsigned)time(NULL));
	
	TRAZIM=(int*)malloc(broj_dretvi*broj_stolova*sizeof(int));
	BROJ=(int*)malloc(broj_dretvi*broj_stolova*sizeof(int));
	STOLOVI=(int*)malloc(broj_dretvi*broj_stolova*sizeof(int));

	//inicijalno postavljanje na vrijednosti na nulu
	for(i=0; i<broj_stolova; i++) {
		TRAZIM[i]=0;
		BROJ[i]=0;
		STOLOVI[i]=0;
	}
	
	for(i=0; i<broj_dretvi; i++) {
		ARG[i]=i+1;
		if(pthread_create(&id[i], NULL, rezervacija, &ARG[i])) {
			//greska
			printf("Greska\n");
			exit(1);
		} else {	
			//uspjesno stvorena dretva
			printf("Dretva uspjesno kreirana\n");
		}
	}
	
	for(i=0; i<broj_dretvi; i++) {
		pthread_join(id[i], NULL);
	}
	
	free(TRAZIM);
	free(BROJ);
	free(STOLOVI);

	return 0;
}
		


