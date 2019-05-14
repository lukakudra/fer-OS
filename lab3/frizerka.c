#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/time.h>

#define N 25

//semafori
sem_t klijenti;
sem_t pristup_sjedalu;
sem_t frizerka_zaposlena;

//globalne varijable
int broj_slobodnih_mjesta=N;
int gotovo;
int vrijeme=0;
int broj_klijenata=0;
int broj_novih_klijenata=0;
int arg_klijent[N];
pthread_t id_klijent[N];
int arg_frizerka;
pthread_t id_frizerka[1];

//trajanje jednog šišanja
void cekaj(int n) {
	for(int i=0; i<n; i++) {
		sleep(i);
	}
}


void *frizerka(void *arg) {
	
	printf("Frizerka: Otvaram salon\n");
	gotovo=0;
	int i;
	while(gotovo==0) {
		
		sem_wait(&klijenti);
		printf("Frizerka: Spavam dok klijenti ne dodju\n");
			
		printf("Frizerka: Idem raditi\n");
		sem_wait(&pristup_sjedalu);
		broj_slobodnih_mjesta++;
		sem_post(&pristup_sjedalu);
		sem_post(&frizerka_zaposlena);
		cekaj(3); //šišanje
		printf("Frizerka: Klijent gotov\n");
		
	}
	for(i=0; i<N; i++) {
		sem_post(&frizerka_zaposlena);
	}
	printf("Frizerka: zatvaram salon\n");
}

void *klijent(void *arg) {

	int id_klijenta=*(int*)arg;
	
	printf("    Klijent[%d]: Zelim na frizuru\n", id_klijenta);
	sem_wait(&pristup_sjedalu);
	if(!gotovo && broj_slobodnih_mjesta > 0) {
		broj_slobodnih_mjesta--;
		sem_post(&klijenti);
		printf("    Klijent[%d]: Ulazim u cekaonu (%d)\n", id_klijenta, broj_slobodnih_mjesta);
		sem_post(&pristup_sjedalu);
		printf("    Klijent[%d]: frizerka mi radi frizuru\n", id_klijenta);
		sem_wait(&frizerka_zaposlena);
		//cekaj dok se ne ošišaš
	
	} else {
		
		sem_post(&pristup_sjedalu);
		printf("    Klijent[%d]: nista od frizure danas\n", id_klijenta);
		
	}
	
}


void stvori_klijenta(int sig) {
	
	int i;
	int razmak;
	
	srand(time(NULL));
	
	for(i=0; i<broj_novih_klijenata; i++) {
		if(pthread_create(&id_klijent[broj_klijenata], NULL, klijent, &broj_klijenata)!=0) {
			printf("Greska pri stvaranju novog klijenta\n");
			exit(1);
		}
		broj_klijenata++;
		razmak=rand()%5+1;
		sleep(razmak);
	}
}

//izlaz iz programa u slucaju isteka vremena
void izlaz(int sig) {
	printf("Radno vrijeme je GOTOVO, cekam da frizerka zavrsi trenutnog klijenta\n");
	gotovo=1;
	sem_post(&klijenti);
}



int main(int argc, char *argv[]) {
	
	if(argc!=4) {
		printf("Za pokretanje programa potrebno je unijeti: ");
		printf("pocetni broj klijenata, broj novih klijenata, radno vrijeme salona\n");
		exit(1);
	}
	
	broj_klijenata=atoi(argv[1]);
	if(broj_klijenata>N) {
		printf("Previse klijenata! MAX je 25\n");
		exit(1);
	}
	broj_novih_klijenata=atoi(argv[2]);
	vrijeme=atoi(argv[3]);
	
	int i;
	pthread_t id_frizerka[1];

	//inicijalizacija semafora
	sem_init(&klijenti, 0, 0);
	sem_init(&pristup_sjedalu, 0, 1);
	sem_init(&frizerka_zaposlena, 0, 0);
	
	
	//radno vrijeme salona
	signal(SIGALRM, izlaz);
	alarm(vrijeme);
	
	//postavljanje dretve koja stvara nove klijente
	signal(SIGINT, stvori_klijenta);
	
	//stvaranje dretve frizerka
	if(pthread_create(&id_frizerka[0], NULL, frizerka, &arg_frizerka)) {
		printf("Greska\n");
		exit(1);
	} else {
		printf("Uspjesno stvorena dretva frizerka\n");
	}
	
	//stvaranje pocetnih klijenata
	for(i=0; i<broj_klijenata; i++) {
		arg_klijent[i]=i+1;
		if(pthread_create(&id_klijent[i], NULL, klijent, &arg_klijent[i])) {
			printf("Greska\n");
			exit(1);
		} else {
			printf("Uspjesno stvorena dretva klijent\n");
		}
	}
	
	for(i=0; i<broj_klijenata; i++) {
		pthread_join(id_klijent[i], NULL);
	}
	
	//kad svi klijenti zavrse, onda signaliziraj frizerku da zavrsi s poslom
	gotovo=1;
	sem_post(&klijenti);

	pthread_join(id_frizerka[0], NULL);
		
	return 0;
}
