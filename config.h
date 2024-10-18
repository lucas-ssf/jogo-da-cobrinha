#define VELOCIDADE 0.5 
#define ALIMENTO 'A'
#define BOMBA 'B'
#define JOGADOR '@'
#define CAUDA 'x'
#define QTD_OBJETOS 2 // sempre que for adicionado um novo tipo de objeto, deve-se aumentar esse valor
#define MAX_OBJ 50 
#define MAX_CAUDA 50 
#define INICIO 3
#define ALTURA 20
#define LARGURA 20

typedef struct{int x,y;}Posicao;

typedef struct{
	Posicao atual,passada;
    char identificador;
}Cauda;

typedef struct{
	int vida, pontos;
	char nome[12], identificador;
	Posicao pos;
	Cauda cauda[MAX_CAUDA];
}Jogador;


typedef struct{
	int qtd, qtd_max, raridade; // quanto menor a raridade, maior a chance de gerar o objeto
	char identificador;
	Posicao pos[MAX_OBJ];
}Objeto;

