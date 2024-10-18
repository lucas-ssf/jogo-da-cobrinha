#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "config.h"

#define limpa_tela() system("clear");
#define gotoxy(x,y) printf("%c[%d;%df",0x1B,y,x)

/* Tela:
 1
 2
 3
 4
 5             x(5,5)
 6             
 7
 8
 9
10
  1  2  3  4  5  6  7  8  9 10
*/

// verifica se alguma tecla foi pressionada
int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

void desenha_borda(){
	for(int i = 2; i < ALTURA; i++){ //vertical
		gotoxy(1,i);
		printf("||");
		gotoxy(LARGURA,i);
		printf("||");
	}
	for(int i = 1; i < LARGURA+2; i++){ //horizontal
		gotoxy(i,1);
		printf("=");	
		gotoxy(i,ALTURA);
		printf("=");
	} 
}

void barra_info(Jogador_t j){
	gotoxy(1,ALTURA+2);
	printf("[%s]",j.nome);
	gotoxy(1,ALTURA+3);
	printf("|Vida:%d",j.vida);
	gotoxy(1,ALTURA+4);
	printf("|Pontos:%d",j.pontos);
}

void coloca_objetos(Objeto_t**p){
	for(int j = 0; j < QTD_OBJETOS; j++){
		if(p[j]->qtd){
			for(int i = 0; i < p[j]->qtd; i++){
				gotoxy(p[j]->pos[i].x,p[j]->pos[i].y);
				printf("%c\n", p[j]->identificador);
			}
		}
	}
}

void desenha_cauda(Jogador_t*j){
	for(int i = 1; i <= j->pontos; i++){
		gotoxy(j->cauda[i].atual.x,j->cauda[i].atual.y);
        j->cauda[i].identificador = CAUDA;
		printf("%c", j->cauda[i].identificador);
	}
}

void desenha_tela(Jogador_t j, Objeto_t**p){
	limpa_tela();
	barra_info(j);
	desenha_borda();
	coloca_objetos(p);
	desenha_cauda(&j);
	gotoxy(j.pos.x,j.pos.y);
	printf("%c\n", j.identificador);
}

void gerar_objeto(Objeto_t*o){
	if(rand()%o->raridade == 1 && o->qtd < o->qtd_max){
		o->pos[o->qtd] = (Posicao_t){rand()%(LARGURA-3)+3,rand()%(ALTURA-2)+2};
		o->qtd++;
	}
}

void tocou_objeto(Jogador_t*j, Objeto_t*o){
	for(int i = 0; i < o->qtd; i++){
		if((j->pos.x==o->pos[i].x) && (j->pos.y==o->pos[i].y)){
			if(o->identificador == ALIMENTO){
				j->pontos++; 
				for(int j = i; j < (o->qtd-1); j++) o->pos[j] = o->pos[j+1];
				o->qtd--;
			}
			else if(o->identificador == BOMBA) {
				j->vida--; 
				for(int j = i; j < (o->qtd-1); j++) o->pos[j] = o->pos[j+1];
				o->qtd--;
				printf("BOOOOM!!!!\n");
			}
		}
	}
}

void tocou_cauda(Jogador_t*j){
	for(int i = 1; i < j->pontos; i++){
		if((j->pos.x==j->cauda[i].atual.x) && (j->pos.y==j->cauda[i].atual.y)){
            printf("OUCH!\n");
            j->vida=0;        
        }
	}
}

void movimento(char c, Jogador_t*j){
	if(c == 'j'){
        if(j->pos.y <(ALTURA-1))j->pos.y++;
        else j->pos.y=2;
    }
	if(c == 'k') {
        if(j->pos.y > 2)j->pos.y--;
        else j->pos.y=ALTURA-1;
    }
	if(c == 'l'){ 
        if(j->pos.x <(LARGURA-1))j->pos.x++;
        else j->pos.x=3;
    }
	if(c == 'h'){ 
        if(j->pos.x > INICIO)j->pos.x--;
        else j->pos.x=LARGURA-1;
    }
}

int main(){
	FILE*leaderboard = fopen("./leaderboard.txt", "a+");
	Jogador_t j = {.vida=5, .pontos=0, .pos={INICIO,2}, .identificador=JOGADOR};
	Objeto_t alimento = {.qtd = 0, .identificador=ALIMENTO, .qtd_max=5, .raridade=10}, bomba = {.qtd=0, .identificador=BOMBA, .qtd_max=50, .raridade=3};
	Objeto_t*p_obj[QTD_OBJETOS] = {&alimento, &bomba};
	char c = 0, anterior;
	printf("Digite seu nome: ");
	scanf("%s", j.nome);

	srand(time(NULL));
	system ("/bin/stty raw");
	while(j.vida){
		if(kbhit()){
			anterior = c;
			c = getc(stdin);
			if(c == 'q') break;
			else if(c != 'h' && c != 'j' && c != 'k' && c != 'l')c=anterior;
		}
		j.cauda[0].passada = j.pos;
		movimento(c, &j);
		j.cauda[0].atual = j.pos;
		for(int i = 1; i <= j.pontos; i++){
			j.cauda[i].passada = j.cauda[i].atual;
			j.cauda[i].atual = j.cauda[i-1].passada;
		}
		desenha_tela(j, p_obj);
        tocou_cauda(&j);
		for(int i = 0; i < QTD_OBJETOS; i++){
			gerar_objeto(p_obj[i]);
			tocou_objeto(&j,p_obj[i]);
		}
		gotoxy(1,ALTURA+4);
		printf("\n");
		usleep(VELOCIDADE*1000000);
	}
	limpa_tela();
	printf("GAME OVER!!! >:(\n\n");
	system ("/bin/stty cooked");
	char jogadores_pontos[100];
	puts("======LEADERBOARD=====");
	while(fgets(jogadores_pontos, 100, leaderboard))printf("%s",jogadores_pontos);
	printf("%s: %d\n",j.nome, j.pontos);
	fprintf(leaderboard,"%s: %d\n",j.nome, j.pontos);
	fclose(leaderboard);
	return 0;
}
