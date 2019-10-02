#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

//BITS USADOS:
//bola
//17,16,15
//14,13,12
//11,10,9

//xis
//8,7,6
//5,4,3
//2.1.0

//#define DEBUG

#ifdef DEBUG
	#define DPRINTF(fmt, args...) fprintf(stderr,"%s %s " fmt,__FILE__, __FUNCTION__, ##args )
#else
	#define DPRINTF(fmt, args...) 
#endif

//#define DEBUG2

#ifdef DEBUG2
	#define DPRINTF2(fmt, args...) fprintf(stderr,"%s %s " fmt,__FILE__, __FUNCTION__, ##args )
#else
	#define DPRINTF2(fmt, args...) 
#endif

//DEFINES FIXOS (NAO MUDE)
#define LINS 3
#define COLS 3
#define QTD_PATTERN 8
#define QTD_SQUARES (LINS*COLS)
#define QTD_JOGS 2
#define XIS   0
#define BOL   1
#define VELHA 2
#define WIN   2048
#define VITXIS  1
#define VITBOL -1

//CONFIGURAVEIS
#define MAXNIVEL 4

#define RANDOM


//typedefs
typedef int TipoFuncao(int,int,int);

//Variaveis Globais
const int  Gpatterns[QTD_PATTERN]={7,56,448,73,146,292,84,273};
	
const char Gjogador[QTD_JOGS]={'X','O'};

static int Gpos;

static int Glinha;

static int Ganalise;

//Declaracoes de funcoes
void ShowPos( int pos );
int  TestaFim( int pos );
int  PutPos( char *pos );
void ShowPatterns();
int  CPUjoga( int nivel, int pos, int jog );
int  JogJoga( int nivel, int pos, int jog );
char Prompt( char *msg, char *range );



//Implementacao das funcoes
void ShowPos( int pos, int mode )
{
	int i, j, k;

	for ( i=0; i<LINS; i++ )
	{
		for ( j=0; j<COLS; j++ )
		{
			k = i*LINS+j;
			if ( pos & (1<<k) )
			{
				if ( mode==2 && Glinha & (1<<k) )
				{
					printf("*X* ");					
				}
				else
				{
					printf(" X  ");					
				}

			}
			else
			if ( pos & (1<<(k+QTD_SQUARES) ) )
			{
				if ( mode==2 && Glinha & (1<<k) )
				{
					printf("*O* ");					
				}
				else
				{
					printf(" O  ");					
				}

			}
			else
			{
				if(mode==1)
				{
					printf("(%d) ", k );
				}
				else
				{
					printf("  . ");
				}
			}
			
		}
		printf("\n");
	}
	printf("\n");
}


int TestaFim( int pos )
{
	//0 nao acabou
	//1 vitoria xis
	//-1 vitoria bola
	//2 velha (empate)
	int i, j;

	Glinha=0;
	
	for ( j=0; j<QTD_JOGS; j++ )
	for ( i=0; i<QTD_PATTERN; i++)
	{
		DPRINTF("Testando pattern[%d]=%d\n",i, Gpatterns[i] );
		
		if ( ( ( pos >> (QTD_SQUARES*j) ) & Gpatterns[i] ) == Gpatterns[i]   ) 
		{
			DPRINTF( "Ganhou %c!\n", Gjogador[j] );
			
			Glinha = Gpatterns[i];
			//return 1-2*j;
			return 1 - (j<<1);
		}		
	} 

	if (((  pos & 511 ) | (pos>>QTD_SQUARES )& 511 )== 511 )
	{
		DPRINTF("Velha\n");
		return VELHA;
	}

	DPRINTF("Sem ganhador ainda\n");
	return 0;
	
}

int  PutPos( char *pos )
{
	int i, res;
	
	res=0;
	for ( i=0; i<QTD_SQUARES; i++ )
	{
		switch (pos[i])
		{
			case 'X':
				res |= (1 << i);
				break;
			case 'O':
				res |= (1 << (i+QTD_SQUARES));
				break;
			default:
				break;
		}
	}
	
	return res;
}

void ShowPatterns()
{
	int i;
	for ( i=0; i<QTD_PATTERN; i++ )
		ShowPos(Gpatterns[i],0);	
}


int CPUjoga( int nivel, int pos, int jog )
{
	int nota, i, jogada;
	int melhor_nota, melhor_jogada;
	int bit[QTD_JOGS];
	
	const int priority[QTD_SQUARES]={4,0,2,6,8,1,3,5,7};
	
	DPRINTF2("Passo0 nivel=%d pos=%d jog=%d\n", nivel, pos, jog );
	
	#ifdef DEBUG2
	ShowPos(pos,0);
	getchar();
	#endif
	
	nota=TestaFim(pos);

	switch (nota)
	{
		case VITXIS:
			DPRINTF2("Passo1 XIS GANHOU\n");
			return  (WIN >> nivel);
		case VITBOL:
			DPRINTF2("Passo2 BOLA GANHOU\n"); 
			return -(WIN >> nivel); 
		case VELHA :
			DPRINTF2("Passo3 VELHA...\n"); 
			return 0;
		default :
			DPRINTF2("Passo4 continua\n");
	}
	#ifdef RANDOM
	if ( nivel >= ( rand() % MAXNIVEL ) + 1 )
	#else
	if ( nivel >= MAXNIVEL )
	#endif
	{
		DPRINTF2("nivel=%d >= MAXNIVEL\n", nivel );
		return nota;
	}
	
	if (!jog)
	{
		melhor_nota = -WIN;	
	}
	else
	{
		melhor_nota = WIN;
	} 
 
	melhor_jogada = 0;
	
	for ( i=0; i<QTD_SQUARES; i++ )
	{
		bit[XIS] = ( 1<< priority[i] );
		bit[BOL] = ( bit[XIS]<<QTD_SQUARES );
		
		if ( ( pos & bit[XIS] ) ==0 && ( pos & bit[BOL] )==0 )
		{
			
			jogada = ( pos | bit[jog] ); 
			
			Ganalise++;
			
			DPRINTF2("Passo5 executando jogada[%d] jog=%d nivel=%d\n", i, jog, nivel );
			
			nota= CPUjoga( nivel+1, jogada, !jog );
			
			DPRINTF2("Passo6 nivel=%d pos=%d jog=%d nota=%d\n", nivel, pos, jog, nota );
			
			if ((  jog && nota < melhor_nota )||
			    ( !jog && nota > melhor_nota ) )
			{
				melhor_nota  = nota;
				melhor_jogada= jogada;
				DPRINTF2("Passo7 nivel=%d pos=%d jog=%d nota=%d\n", nivel, pos, jog, nota );
				//if ( nivel > 1 )break;
			}
		}		
	}
	
	if (nivel==0)
	{
		//printf("O computador jogou melhor_nota=%d: Ganalise=%d\n", melhor_nota, Ganalise );
		printf("O COMPUTADOR jogou:\n");
		ShowPos( melhor_jogada,0 );
		Gpos = melhor_jogada;		
	}
	
	return melhor_nota;
}
		
int JogJoga( int nivel,  int pos, int jog )
{
	int i, valid;
	int bit[QTD_JOGS];
	char letra;
	
		
	do {

		printf("Jogador HUMANO joga:\n");
		
		ShowPos(pos,1);
		
		valid = 0;
		printf("Digite ");
		for ( i=0; i<QTD_SQUARES; i++ )
		{
			bit[XIS] = ( 1<<i );
			bit[BOL] = ( bit[XIS]<<QTD_SQUARES );
			
			if ( ( pos & bit[XIS] ) ==0 && ( pos & bit[BOL] )==0 )
			{
				printf(" (%d)", i );
				valid |= (1<<i);
			}
		}
		printf(" para jogar\n");
		
		
		letra = getchar();
		fflush(stdin);
		
		if ( valid & ( 1<< (letra-'0')) )
		{
			break;
		}
		else if (! ( letra >= '0' && letra <= '8' ) )
		{
			printf("Caracter invalido: %c\n", letra );
		}
		else
		{
			printf("Essa posicao (%d) ja foi jogada!\n", letra-'0' );
		}

	} while (1);
	
	Gpos = pos |= (1<<(jog*QTD_SQUARES+letra-'0'));
	
	return pos |= (1<<(jog*QTD_SQUARES+letra-'0'));
	
}

char Prompt( char *msg, char *range )
{
	int i, len;
	char letra;
	
	do
	{
		printf(msg);
		letra = getchar();
		fflush(stdin);
		
		len = strlen(range);
		
		if (!len)
		{
			break;
		}
		
		for ( i=0; i<len; i++ )
		{
			if ( letra == range[i] )
			{
				return letra;
			}
		}
		
		printf("Opcao invalida: %c\n", letra );
		
					
	}
	while (1);
	
	return 0;
}

int main(int argc, char** argv) 
{

	int fim, pausa;
	char letra;
	
	TipoFuncao *jog1, *jog2;
	
	do
	{
		Ganalise =0;
		Gpos = 0;
		pausa=0;
	
		jog1 = jog2 = 0;
		
		letra = Prompt("Escolha o tipo de jogo:\n(1)Jogador X CPU (2)CPU X Jogador (3)CPU X CPU (4)Jogador X Jogador\n","1234");
		
		switch (letra)
		{
			case '1':
				jog1 = JogJoga;
				jog2 = CPUjoga;
				break;
			case '2':
				jog2 = JogJoga;
				jog1 = CPUjoga;			
				break;
			case '3':
				jog1 = CPUjoga;
				jog2 = CPUjoga;
				pausa= 1;		
				break;
			default:
				jog1 = JogJoga;
				jog2 = JogJoga;
				break;		
		}
	
		do 
		{
		
			(*jog1)(0,Gpos,XIS ); 
			
			fim = TestaFim( Gpos );			
			
			if (fim ) break;
			
			if (pausa) Prompt("Aperte tecla para continuar.\n","");
			
			(*jog2)(0,Gpos,BOL );
			
			fim = TestaFim( Gpos );
			
			if (fim ) break;
			
			if (pausa) Prompt("Aperte tecla para continuar.\n","");
			
		}
		while (1);
	
		ShowPos(Gpos,2);
		
		switch(fim)
		{
			case VITXIS:
				printf("XIS ganhou!\n");
				break;
			case VITBOL:
				printf("BOLA ganhou!\n");
				break;
			case VELHA:
				printf("Velha...\n");
				break;
		}
	
		Prompt("Aperte [ENTER] para jogar novamente.\n","");
	
	}
	while(1);
	
	return 0;
}
