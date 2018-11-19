#ifndef TIRO_H_
#define TIRO_H_

#include <allegro.h>
#include "spaceinvaders.h"

typedef struct tiro{
    BITMAP* tiro_player;
    int x;
    int y;
    int speed;
    int ativo;
}Tiro;

Tiro* Create_Tiro(int x, int y);
void Update_Tiro(Tiro* t);
void Draw_Tiro(Tiro* t, BITMAP* buffer);
void Destroy_Tiro(Tiro* t);

typedef struct no_tiro{
   Tiro* t;
   struct no_tiro* prox;
}No_Tiro;

typedef struct lista_tiros{
   No_Tiro* inicio;
}Lista_tiros;

Lista_tiros* Create_Lista_Tiros();
void Insere_Tiro(Lista_tiros* l, int x, int y);
void Update_Lista_Tiros(int* pontuacao, BITMAP* buffer, volatile int *timerMS, Lista_tiros* l, SAMPLE* explosion, naveE_st* nave1, naveE_st* nave2, naveE_st* nave3, naveE_st* nave4, naveE_st* nave5, naveE_st* nave6,
                        naveE_st* media1, naveE_st* media2, naveE_st* media3, naveE_st* grande, BITMAP* inimigo_pequeno, BITMAP* inimigo_medio, BITMAP* inimigo_grande);
void Draw_Lista_Tiros(Lista_tiros* l, BITMAP* buffer);
void Destroy_Lista_Tiros(Lista_tiros* l);

#endif
