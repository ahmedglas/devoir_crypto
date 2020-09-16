#include <gmp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>



void M_Pow(mpz_t s,mpz_t S_pow) //  S_pow --> 2^s
{
  int i = 0;
  mpz_set_ui(S_pow,1); // S_pow = 2^0
  while(mpz_cmp_si(s,i) > 0 ) // multiplying S_pow * 2 while i < s
  {
    mpz_mul_ui(S_pow,S_pow,2);
    i++;
  }
}
//When multiplying large numbers we end up multiplying each "digit" (word, 32-bit piece, ...) of the first number with each "digit" of the second number.



// i wrote that function  thinking that s needs to handle numbers that are larger than the number of bits of RAM my computer has.......
//function count the number of "least significant clear bits" (to find s), then do a single right shift to find t (like "t = X >> s)
void Decmp(mpz_t x,mpz_t s,mpz_t t)
{
	mpz_t y,S_pow;
	mpz_init(y);
	mpz_set_ui(y,0);



	mpz_init(S_pow);

	while(mpz_cmp(y,x)!= 0) // while 2^s * t != x
	{
        mpz_set_ui(t,1); // t = 1
        mpz_mul(y,S_pow,t);// y = 2^s * t
       while(mpz_cmp(y,x) < 0)// if 2^s * t > x then stops
        {
			mpz_add_ui(t,t,2); // t impaire increment 2 by 2
			M_Pow(s,S_pow);
			mpz_mul(y,S_pow,t);
        }
        mpz_add_ui(s,s,1); // we test with the next 2 ^
	}

	mpz_sub_ui(s,s,1);
	mpz_clear(y);
	mpz_clear(S_pow);
}

void Decmp2(mpz_t x,mpz_t s,mpz_t t){

s = mpz_scan1(x,0) ;        // if erreur  force  avec la commange gcc -dm.o -c -w  dm.c -lgmp
mpz_tdiv_q_2exp(t, x, s);


}
void Squar_and_Multiply(mpz_t a,mpz_t z,mpz_t e, mpz_t r)
{

    char * bin = mpz_get_str(NULL,2,e);  //transform binary to base 10
    int i;                               // save in bin
    mpz_set(r,a); // initialite a value
    for(i = 1; i < strlen(bin);i++) // for every bits in bin do
    {                              // using the algorithm
		mpz_mul(r,r,r);
		mpz_mod(r,r,z);
		if(bin[i] == '1')
		{
			mpz_mul(r,r,a);
			mpz_mod(r,r,z); // modulo = X - (X * reciprocal); if(modulo => divisor) { modulo -= divisor}; return modulo;
                            // shifted_reciprocal = (1 << bits) / divisor" to get a reciprocal that has been shifted left by bits bits;
                            //and then  "modulo = X - ((X * shifted_reciprocal) >> bits);".
		}                       //this will be used insted of mpz_mod() cause a lot of time will be spent trying to find the modulus
		}
}


void test_Fermat(mpz_t n, mpz_t rep)
{
	gmp_randstate_t state; 	//initializing time
	gmp_randinit_mt(state);
	gmp_randseed_ui(state, time(NULL));

	mpz_t i;
	mpz_init(i);
	mpz_set_si(i,1);

	mpz_t n2;
	mpz_init(n2);
	mpz_sub_ui(n2,n,1);

	mpz_t a;
	mpz_init(a);

	mpz_t r;
	mpz_init(r);

	mpz_t n3;
	mpz_init(n3);
	mpz_sub_ui(n3,n,3);

	while(mpz_cmp(i,rep)<=0 && mpz_cmp_si(n,2)!= 0  && mpz_cmp_si(n,3)!=0)
	{
		mpz_urandomm(a,state,n3);
		mpz_add_ui(a,a,2);
		Squar_and_Multiply(a,n,n2,r);
		if(mpz_cmp_si(r,1)!=0)
		{
			printf("Le nombre est composé \n");
			mpz_clear(i);mpz_clear(n2);
			mpz_clear(a);mpz_clear(r);
			mpz_clear(n3);gmp_randclear(state);
			return ;
		}
		mpz_add_ui(i,i,1);
	}

	printf("Le nombre est premier \n");
	mpz_clear(i);mpz_clear(n2);
	mpz_clear(a);mpz_clear(r);
	mpz_clear(n3);gmp_randclear(state);
}

void Miller_Rabin(mpz_t n, mpz_t rep)
{
	if(mpz_get_ui(n) % 2 == 0)   //is n!=2  and if n %2 =0 => composé ,
	{
		if(mpz_cmp_ui(n,2) == 0)
			printf("Le nombre est premier\n");
        else
            printf("Le nombre est composé\n");
        return ;
	}

	int i=1;
	mpz_t a,y,s,t,n1,n2,deux;
	gmp_randstate_t state;     //initializing time
	gmp_randinit_mt(state);
	gmp_randseed_ui(state, time(NULL));

	mpz_init(a);

	mpz_init(y);

	mpz_init(s);
	mpz_set_ui(s,1);

	mpz_init(deux);
	mpz_set_ui(deux,2);

	mpz_init(t);

	mpz_init(n1);
	mpz_sub_ui(n1,n,1);

	mpz_init(n2);
	mpz_sub_ui(n2,n,2);

	Decmp(n1,s,t);
    Decmp2(n1,s,t);

	mpz_sub_ui(s,s,1);
	while(mpz_cmp_ui(rep,i)>=0)
	{
		mpz_urandomm(a,state,n1);
		mpz_add_ui(a,a,1);
		Squar_and_Multiply(a,n,t,y);
		if(mpz_cmp_si(y,1)!=0 && mpz_cmp(y,n1)!=0) // y tetaba9 m3a a 1 mod n or n-1 mod n = -1 mod n
		{
			for(int j=1;mpz_cmp_ui(s,j)>=0;j++)
			{
				mpz_set(n2,y);
				Squar_and_Multiply(y,n,deux,y);
				if(mpz_cmp_si(y,1)==0) //if  y tetaba9 m3a a 1 mod n ==> composé
				{
					printf("Le nombre est composé\n");
					mpz_clear(a);mpz_clear(y);
					mpz_clear(s);mpz_clear(t);
					mpz_clear(n1);mpz_clear(n2);
					mpz_clear(deux);gmp_randclear(state);
					return;
				}
				if(mpz_cmp(y,n1)==0) //Si y ==!? a -1 mod n existe
				break;
			}
			if(mpz_cmp(y,n1)!=0)
			{
				printf("Le nombre est composé \n");
				mpz_clear(a);mpz_clear(y);
				mpz_clear(s);mpz_clear(t);
				mpz_clear(n1);mpz_clear(n2);
				mpz_clear(deux);gmp_randclear(state);
				return;
			}

		}
		i++;
	}
	printf("Le nombre est premier \n");

	mpz_clear(a); mpz_clear(y);
	mpz_clear(s); mpz_clear(t);
	mpz_clear(n1); mpz_clear(n2);
	mpz_clear(deux); gmp_randclear(state);
}

int main()
{
	int t=1;
	mpz_t n;
	mpz_init(n);
	mpz_t rep;
	mpz_init(rep);
	printf("########## Test de primalité ##########\n");
    while(t==1)
    {
        printf("\n");
        printf("AHMED SAADI");
         printf("\n");
        printf("Choisir votre entier à tester : ");
        gmp_scanf("%Zd", &n);
        if(mpz_cmp_ui(n,1)<=0)
        {
			printf("\n choisir un nombre superieur a 1 : ");
		}
		else
		{
			printf("donner le nombre de répétitions  : ");
			gmp_scanf("%Zd", &rep);
			printf("**********************************************************\n");
			printf("Miller Rabin : ");
			Miller_Rabin(n,rep);
			printf("\n");
			printf("Fermat : ");
			test_Fermat(n,rep);
			printf("**********************************************************\n");
			printf("pour tester un autre nombre  tape 1");
			scanf("%d",&t);
		}
	}
	mpz_clear(n);
	mpz_clear(rep);
	return 0;
}
