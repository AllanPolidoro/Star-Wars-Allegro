#ifndef _SPACEINVADERS_H_
#define _SPACEINVADERS_H_

typedef struct naveE_str{
    int vida;
    int x;
    int y;
    int ty;
    int tx;
    float tv;
    unsigned int aleatorio_mov;
    unsigned int aleatorio_tiro;
    int flag;
    int contH;
    int contV;
    int flagAB;
    int flagT;
    unsigned int cont_t;
    int flagEx;
    int auxx;
    int auxy;
    int flagTempo;
    int auxTEx;
    int cronEx;
}naveE_st;

typedef struct bonus_str{
 int x;
 int y;
 int flagTela;
}bonus_st;

typedef struct player_str{
    int vida;
    int x;
    int y;
    int vy;
    int flagHit;
}player_st;

typedef struct new_game{
    char nome_jogador[11];
    char vez_jogada; // numero dado em ASCII
    int pontuacao;
    volatile int timer;
    player_st jogador;
    naveE_st inimigos[10];
    bonus_st posicao_bonus[2];
    int existencia;
    int vader;
    int r2d2;
    int x_heart;
    int y_heart;
    int divPont;
    int contNivel;
    int nivel;
    int tamanho_nome;
    char ultima_letra;
} novo_jogo;

typedef struct{
    char nome[11];
    int pontuacao;
} rank_game;

typedef struct{
    novo_jogo game_played;
    int comando;
} end_of_game;

end_of_game jogo(char*, int, novo_jogo);
int menu();
int ler_nome(char*);
void selecionar_jogo(char*);
void tutorial();
void highscores();
void gameover(end_of_game);
void naoencontrado();

#endif


