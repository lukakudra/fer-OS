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

int ID;
int *TRAZIM;
int *BROJ;
int *STOLOVI;
int zadnji_broj=0;

static int broj_stolova, broj_procesa;

void oslobodi(int sig) {
	
	(void)shmdt((char*)TRAZIM);
	(void)shmdt((char*)BROJ);
	(void)shmctl(ID, IPC_RMID, NULL);
	exit(0);
	
}

void ulaz_u_kriticni_odsjecak(int i) {
	
	int j, max;
	TRAZIM[i]=1;
	
	max=BROJ[0];
	for(j=0; j<broj_procesa; j++) {
		 if(BROJ[j]>max) {
		 	max=BROJ[j];
		 }
	}
	
	BROJ[i]=max+1;
	TRAZIM[i]=0;
	
	for(j=0; j<broj_procesa; j++) {
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

void rezervacija(int proces, int broj_stolova) {
	
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
		//srand((unsigned)time(NULL));
		srand(getpid());
		trenutno_zauzet=rand()%broj_slobodnih;
		
		printf("Proces %d: odabirem stol %d\n", proces, id_stola[trenutno_zauzet]+1);
		
		if(STOLOVI[id_stola[trenutno_zauzet]]==0) {
			//stol je slobodan, proces ga pokusava rezervirati
			ulaz_u_kriticni_odsjecak(proces);
			STOLOVI[id_stola[trenutno_zauzet]]=proces;
			printf("Proces %d: rezerviram stol %d", proces, id_stola[trenutno_zauzet]+1);
			ispis_stanja(broj_stolova);
			broj_zauzetih++;
		} else {
			//stol je vec rezerviran
			printf("Proces %d: neuspjela rezervacija stola %d", proces, id_stola[trenutno_zauzet]+1);
			ispis_stanja(broj_stolova);
		}
		
		izlaz_iz_kriticnog_odsjecka(proces);
	}
}
	
int main(int argc, char *argv[]) {

	int i;
	broj_procesa=atoi(argv[1]);
	broj_stolova=atoi(argv[2]);
	
	sigset(SIGINT, oslobodi); //u slucaju prekida brisi memoriju

	ID=shmget(IPC_PRIVATE, sizeof(int)*(broj_procesa+1)+sizeof(int)*(broj_stolova+1), 0600);

	TRAZIM=(int*)shmat(ID, NULL, 0);
	BROJ=(int*)shmat(ID, NULL, 0)+(broj_procesa+1);
	STOLOVI=(int*)shmat(ID, NULL, 0)+2*(broj_procesa+1);
	
	//inicijalno postavljanje vrijednosti na  nulu
	for(i=0; i<broj_stolova; i++) {
		TRAZIM[i]=0;
		BROJ[i]=0;
		STOLOVI[i]=0;
	}
	
	for(i=0; i<broj_procesa; i++) {
		if(fork()==0) {
			//uspjesno stvoren proces dijete
			rezervacija(i+1, broj_stolova);
			exit(0);
		} else if(fork()==-1) {
			//greska, proces nije stvoren
			printf("Greška, proces se nije stvorio\n");
			exit(1);
		}
	}
	
	for(i=0; i<broj_procesa; i++) {
		wait(NULL);
	}
	
	//oslobadjanje zauzetog dijela memorije
	oslobodi(0);
	
	return 0;
}

	

