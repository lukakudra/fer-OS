#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DRETVI	6

int t=0; //simulacija vremena (trenutno vrijeme)
int brojac_dretvi=0;

struct dretva {
	int id; //1, 2, 3, ...
	int p;  //preostalo vrijeme rada
	int prio; //prioritet
	int rasp; //način raspoređivanja (za opcionalni zadatak)
};

int prioritet[DRETVI]={0}, id[DRETVI]={0}, t_d[DRETVI]={0}, v_t[DRETVI]={0}, raspored[DRETVI]={0};


#define MAX_DRETVI	5
struct dretva P[DRETVI]; //red pripravnih dretvi, P[0] = aktivna dretva

/* podaci o događajima pojave novih poslova - dretvi */
int nove[DRETVI][5] =
{
	/* trenutak dolaska, id, p, prio, rasp (prio i rasp se koriste ovisno o rasporedivacu) */
	{ 1,  3, 5, 3, 1 }, /* rasp = 0 => PRIO+FIFO; 1 => PRIO+RR */
	{ 3,  5, 6, 5, 1 },
	{ 7,  2, 3, 5, 1 },
	{ 12, 1, 5, 3, 0 },
	{ 20, 6, 3, 6, 1 },
	{ 20, 7, 4, 7, 1 },
};

void ispis_stanja ( int ispisi_zaglavlje ) {
	int i;

	if ( ispisi_zaglavlje ) {
		printf ( "  t    AKT" );
		for ( i = 1; i < MAX_DRETVI; i++ )
			printf ( "       PR%d", i );
		printf ( "\n" );
	} else {

		printf ( "%3d ", t );
		for ( i = 0; i < MAX_DRETVI; i++ )
			if ( P[i].id != -1 && P[i].prio!=-1 && P[i].p>0 && P[i].rasp!=-1 )
				printf ( "  %d/%d/%d/%d ",
					 P[i].id, P[i].prio, P[i].p, P[i].rasp );
			else
				printf ( "  -/-/-/- " );
		printf ( "\n");
	}
}

void swap_structs(struct dretva *i, struct dretva *j) {
	struct dretva pom=*i;
	*i=*j;
	*j=pom;
}

void shift_structs() {
	int i;
	for(i=1; i<brojac_dretvi; i++) {
		P[i-1]=P[i];
		P[i].id=-1;
		P[i].prio=-1;
		P[i].p=-1;
		P[i].rasp=-1;
	}
}

int main (void) {
	
	int i, j, brojac_zauzetih_mjesta=0;
	t=0;
	int kvant=1;
	ispis_stanja(1);
	
	for(i=0; i<DRETVI; i++) {
		
		prioritet[i]+=nove[i][3];
		id[i]+=nove[i][1];
		t_d[i]+=nove[i][0];
		v_t[i]+=nove[i][2];
		raspored[i]+=nove[i][4];
		
		P[i].id=-1;
		P[i].prio=-1;
		P[i].p=-1;
		P[i].rasp=-1;
		
	}
	
	while(1) {
		
		for(i=0; i<DRETVI; i++) {
			if(t==t_d[i]) {
				printf("%3d -- nova dretva id=%d, prio=%d, p=%d, rasp=%d\n", t, id[i], prioritet[i], v_t[i], raspored[i]);
				P[i].id=id[i];
				P[i].prio=prioritet[i];
				P[i].p=v_t[i];
				P[i].rasp=raspored[i];
				brojac_dretvi++;
				brojac_zauzetih_mjesta++;
				
			}	
		}
		
		if(brojac_zauzetih_mjesta>=5) {
			printf("Nema vise mjesta u redu pripravnih dretvi.\n");
			exit(1);
		}
		
		//slažem dretve u polju po prioritetu
		for(i=0; i<brojac_dretvi; i++) {
			for(j=i+1; j<brojac_dretvi; j++) {
				if(P[j].prio > P[i].prio) {
					swap_structs(&P[i], &P[j]);
				}
			}
		}
		
		ispis_stanja(0);
		
		//ako su dretve istog prioriteta, onda ih raspoređujemo po fifo ili rr
		if(P[0].prio==P[1].prio) {
			//fifo raspodjela
			if(P[0].rasp==0) {
				P[0].p--;
				if(P[0].p==0) {
					printf("Dretva %d zavrsila.\n", P[0].id);
					shift_structs();
					brojac_zauzetih_mjesta--;
				}
				
			//rr raspodjela	
			} else if(P[0].rasp==1) {
				P[0].p-=kvant;
				if(P[0].p > 0) {
					for(i=1; i<brojac_dretvi; i++) {
						swap_structs(&P[i-1], &P[i]);
					}
				} else if(P[0].p==0) {
					printf("Dretva %d zavrsila.\n", P[0].id);
					shift_structs();
					brojac_zauzetih_mjesta--;
				}
			}
		
		//prioritetna raspodjela(kao glavni kriterij, ako dretve nisu istog prioriteta)
		} else {
			P[0].p--;
			if(P[0].p==0) {
				printf("Dretva %d zavrsila.\n", P[0].id);
				shift_structs();
				brojac_zauzetih_mjesta--;
			}
		}
		
		t++;
		
		sleep(1);
	}
	
	return 0;	
}
