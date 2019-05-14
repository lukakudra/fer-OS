#include <stdio.h>
#include <signal.h>

#define N 6  /*postoji 6 razina prioriteta*/

static int OZNAKA_CEKANJA[N];
static int PRIORITET[N];
static int TEKUCI_PRIORITET;

static int sig[]={SIGUSR1, SIGUSR2, SIGTERM, SIGQUIT, SIGINT};

void prekidna_rutina(int signal);

void zabrani_prekidanje() {
	int i;
	for(i=0; i<5; i++) {
		sighold(sig[i]);
	}
}

void dozvoli_prekidanje() {
	int i;
	for(i=0; i<5; i++) {
		sigrelse(sig[i]);
	}
}

void obrada_signala(int i) {
	
	int j=0;

	
	switch(i) {
		case(1):
			printf("  -  P  -  -  -  -\n");
			for(j=1; j<=5; j++) {
				printf("  -  %d  -  -  -  -\n", j);
				sleep(1);
			}
			printf("  -  K  -  -  -  -\n");
			break;
	
		case(2):
			printf("  -  -  P  -  -  -\n");
			for(j=1; j<=5; j++) {
				printf("  -  -  %d  -  -  -\n", j);
				sleep(1);
			}
			printf("  -  -  K  -  -  -\n");
			break;

		case(3):
			printf("  -  -  -  P  -  -\n");
			for(j=1; j<=5; j++) {
				printf("  -  -  -  %d  -  -\n", j);
				sleep(1);
			}
			printf("  -  -  -  K  -  -\n");
			break;
	
		case(4):
			printf("  -  -  -  -  P  -\n");
			for(j=1; j<=5; j++) {
				printf("  -  -  -  -  %d  -\n", j);
				sleep(1);
			}
			printf("  -  -  -  -  K  -\n");
			break;

		case(5):
			printf("  -  -  -  -  -  P\n");
			for(j=1; j<=5; j++) {
				printf("  -  -  -  -  -  %d\n", j);
				sleep(1);
			}
			printf("  -  -  -  -  -  K\n");
			break;
	}

}

void prekidna_rutina(int signal) {
	int n=-1;
	int i, j;
	zabrani_prekidanje();
	
	switch(signal) {
		case SIGUSR1:
			n=1;
			printf("  -  X  -  -  -  -\n");
			break;

		case SIGUSR2:
			n=2;
			printf("  -  -  X  -  -  -\n");
			break;

		case SIGTERM:
			n=3;
			printf("  -  -  -  X  -  -\n");
			break;
		
		case SIGQUIT:
			n=4;
			printf("  -  -  -  -  X  -\n");
			break;

		case SIGINT:
			n=5;
			printf("  -  -  -  -  -  X\n");
			break;

	}

	OZNAKA_CEKANJA[n]=1;
	do {
			
		//odrediti signal najveceg prioriteta koji ceka na obradu
		j=0;
		for(i=TEKUCI_PRIORITET+1; i<N; i++) {
			if(OZNAKA_CEKANJA[i]) {
				j=i;
			}
		}
		//ako postoji signal koji ceka i prioritetniji je od trenutnog posla, idi u obradu
		if(j>0) {
			OZNAKA_CEKANJA[j]=0;
			PRIORITET[j]=TEKUCI_PRIORITET;
			TEKUCI_PRIORITET=j;
			dozvoli_prekidanje();
			obrada_signala(j);
			zabrani_prekidanje();
			TEKUCI_PRIORITET=PRIORITET[j];
		}
	} while(j>0);
}

int main(void) {
	
	int i;
	
	sigset(SIGUSR1, prekidna_rutina);
	sigset(SIGUSR2, prekidna_rutina);
	sigset(SIGTERM, prekidna_rutina);
	sigset(SIGQUIT, prekidna_rutina);
	sigset(SIGINT, prekidna_rutina);

	printf("Proces obrade prekida, PID=%d\n", getpid());
	printf("GP S1 S2 S3 S4 S5\n");
	printf("-----------------\n");
	
	for(i=1; i<=20; i++) {
		sleep(1);
		printf(" %d  -  -  -  -  -\n", i);
	}

	printf("\nZavršio osnovni program\n");
	
	return 0;

}

