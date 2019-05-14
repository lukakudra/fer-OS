#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int A=0, N, M;

void *dretva(void *arg) {
	int i, rbr;
	rbr=*((int*)arg);
	for(i=0; i<M; i++) {
		//sleep(1);
		//printf("Dretva %d u iteraciji %d\n", rbr, i);
		A++;
	}
}

int main(int argc, char *argv[]) {

	if(argc!=3) {
		printf("Potrebno je 2 argumenta za pokretanje.\n");
		exit(1);
	}
	
	N=atoi(argv[1]);
	M=atoi(argv[2]);
	
	int i;
	int ARG[N];
	pthread_t ID[N];
	
	for(i=0; i<N; i++) {
		ARG[i]=i+1;
		
		if(pthread_create(&ID[i], NULL, dretva, &ARG[i])) {
			//greska
			printf("Greska\n");
			exit(1);
		} else {
			//uspjesno stvorena dretva
			printf("Uspjesno stvorena dretva\n");
		}
		
		
	}
	
	for(i=0; i<N; i++) {
		pthread_join(ID[i], NULL);
	}
	
	printf("A= %d\n", A);
	
	return 0;
}
