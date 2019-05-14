#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>


static int pauza=0;
static unsigned long int broj=1000000001L;
static unsigned long int zadnji=1000000001L;

void periodicki_ispis(int signal) {
	printf("zadnji prosti broj: %lu\n", zadnji);
}

int postavi_pauzu(int signal) {
	pauza=1-pauza;
	return pauza;
}

void prekini(int signal) {
	printf("zadnji: %lu\n", zadnji);
	exit(0);
}

int prost(unsigned long int n) {
	unsigned long int i, max;
	
	if((n%2)==0) {    /*je li paran? */
		return 0;
	}
	max=sqrt(n);
	for(i=3; i<=max; i=i+2) {
		if((n%i)==0) {
			return 0;
		}
	}
	return 1;  /*vrati 1 ako je broj prost*/
}

int main(void) {
	
	struct itimerval t;
	
	sigset(SIGINT, postavi_pauzu);
	sigset(SIGTERM, prekini);
	sigset(SIGALRM, periodicki_ispis);
	
	/*definiranje periodickog slanja signala*/
	/*prvi puta nakon: */
	t.it_value.tv_sec=0;
	t.it_value.tv_usec=500000;
	
	/*nakon prvog puta, periodicki s periodom: */
	t.it_interval.tv_sec=1;
	t.it_interval.tv_usec=500000;

	setitimer(ITIMER_REAL, &t, NULL);

	while(1) {
		if(prost(broj)==1) {
			zadnji=broj;
		}
		broj++;
		while(pauza==1) {
			pause();
		}
	}
	return 0;
}
		

