#include "tiro.h"
#include <allegro.h>
#include "spaceinvaders.h"

Tiro* Create_Tiro(int x, int y){
    Tiro* t = (Tiro*) malloc(sizeof(Tiro));
    SAMPLE* laser = load_sample("sons/laser_jogador.wav");
    t->ativo = TRUE;
    t->x = x;
    t->y = y;
    t->speed = 10;
    t->tiro_player = load_bitmap ("Bitmaps/tiro_jogador.bmp", NULL);
    play_sample(laser, 140, 128, 1000, FALSE);
    return t;
}
END_OF_FUNCTION(Create_Tiro)

void Update_Tiro(Tiro* t){
    t->x += t->speed;
    if(t-> y < -10)
        t->ativo = FALSE;
}
END_OF_FUNCTION(Update_Tiro)

void Draw_Tiro(Tiro* t, BITMAP* buffer){
    draw_sprite(buffer, t->tiro_player, t->x, t->y);
}
END_OF_FUNCTION(Draw_Tiro)

void Destroy_Tiro(Tiro* t){
    free(t);
}
END_OF_FUNCTION(Destroy_Tiro)

///------------------------------------------------------------------------------------

Lista_tiros* Create_Lista_Tiros(){
    Lista_tiros* l = (Lista_tiros*) malloc(sizeof(Lista_tiros));
    l->inicio = NULL;
    return l;
}
END_OF_FUNCTION(Create_Lista_Tiros)

void Insere_Tiro(Lista_tiros* l, int x, int y){
    No_Tiro* novo = (No_Tiro*) malloc(sizeof(No_Tiro));
    novo->t = Create_Tiro(x, y);
    novo->prox = l->inicio;
    l->inicio = novo;
}
END_OF_FUNCTION(Insere_Tiro)

void Update_Lista_Tiros(int* pontuacao, BITMAP* buffer, volatile int *timerMS, Lista_tiros* l, SAMPLE* explosion, naveE_st* nave1, naveE_st* nave2, naveE_st* nave3, naveE_st* nave4, naveE_st* nave5, naveE_st* nave6,
                        naveE_st* media1, naveE_st* media2, naveE_st* media3, naveE_st* grande, BITMAP* inimigo_pequeno, BITMAP* inimigo_medio, BITMAP* inimigo_grande){
    int i;
    No_Tiro* aux = l->inicio;
    No_Tiro* aux2 = l->inicio;
    while(aux != NULL){
        //Se o tiro ainda está ativo
        //Colisão do tiro com P1
        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, nave1->x, nave1->y, inimigo_pequeno) && (nave1->x < 1200)){
            play_sample(explosion, 255, 128, 1000, FALSE);
            nave1->auxx = nave1->x;
            nave1->auxy = nave1->y;
            aux->t->ativo = FALSE;
            nave1->x = 1600;
            *pontuacao += 15;
            nave1->flagEx = 1;
        }
        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, nave2->x, nave2->y, inimigo_pequeno) && (nave2->x < 1200)){
            play_sample(explosion, 255, 128, 1000, FALSE);
            nave2->auxx = nave2->x;
            nave2->auxy = nave2->y;
            aux->t->ativo = FALSE;
            nave2->x = 1600;
            *pontuacao += 15;
            nave2->flagEx = 1;
        }
        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, nave3->x, nave3->y, inimigo_pequeno) && (nave3->x < 1200)){
            play_sample(explosion, 255, 128, 1000, FALSE);
            nave3->auxx = nave3->x;
            nave3->auxy = nave3->y;
            aux->t->ativo = FALSE;
            nave3->x = 1600;
            *pontuacao += 15;
            nave3->flagEx = 1;
        }
        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, nave4->x, nave4->y, inimigo_pequeno) && (nave4->x < 1200)){
            play_sample(explosion, 255, 128, 1000, FALSE);
            nave4->auxx = nave4->x;
            nave4->auxy = nave4->y;
            aux->t->ativo = FALSE;
            nave4->x = 1600;
            *pontuacao += 15;
            nave4->flagEx = 1;
        }
        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, nave5->x, nave5->y, inimigo_pequeno) && (nave5->x < 1220)){
            play_sample(explosion, 255, 128, 1000, FALSE);
            nave5->auxx = nave5->x;
            nave5->auxy = nave5->y;
            aux->t->ativo = FALSE;
            nave5->x = 1600;
            *pontuacao += 15;
            nave5->flagEx = 1;
        }
        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, nave6->x, nave6->y, inimigo_pequeno) && (nave6->x < 1200)){
            play_sample(explosion, 255, 128, 1000, FALSE);
            nave6->auxx = nave6->x;
            nave6->auxy = nave6->y;
            aux->t->ativo = FALSE;
            nave6->x = 1600;
            *pontuacao += 15;
            nave6->flagEx = 1;
        }
        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, media1->x, media1->y, inimigo_medio) && (media1->x < 1200)){
            media1->vida--;
            play_sample(explosion, 255, 128, 1000, FALSE);
            media1->auxx = media1->x;
            media1->auxy = media1->y;
            aux->t->ativo = FALSE;
            media1->flagEx = 1;
            *pontuacao += 5;
            if(media1->vida == 0){
                play_sample(explosion, 255, 128, 1000, FALSE);
                media1->auxx = media1->x;
                media1->auxy = media1->y;
                aux->t->ativo = FALSE;
                media1->x = 1600;
                *pontuacao += 15;
                media1->flagEx = 1;
                media1->vida = 2;
            }
        }
        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, media2->x, media2->y, inimigo_medio) && (media2->x < 1200)){
            media2->vida--;
            play_sample(explosion, 255, 128, 1000, FALSE);
            media2->auxx = media2->x;
            media2->auxy = media2->y;
            aux->t->ativo = FALSE;
            media2->flagEx = 1;
            *pontuacao += 5;
            if(media2->vida == 0){
                play_sample(explosion, 255, 128, 1000, FALSE);
                media2->auxx = media2->x;
                media2->auxy = media2->y;
                aux->t->ativo = FALSE;
                media2->x = 1600;
                *pontuacao += 15;
                media2->flagEx = 1;
                media2->vida = 2;
            }
        }
        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, media3->x, media3->y, inimigo_medio) && (media3->x < 1200)){
            media3->vida--;
            play_sample(explosion, 255, 128, 1000, FALSE);
            media3->auxx = media3->x;
            media3->auxy = media3->y;
            aux->t->ativo = FALSE;
            media3->flagEx = 1;
            *pontuacao += 5;
            if(media3->vida == 0){
                play_sample(explosion, 255, 128, 1000, FALSE);
                media3->auxx = media3->x;
                media3->auxy = media3->y;
                aux->t->ativo = FALSE;
                media3->x = 1600;
                *pontuacao += 15;
                media3->flagEx = 1;
                media3->vida = 2;
            }
        }

        if(pixel_perfect_colision(aux->t->x, aux->t->y, aux->t->tiro_player, grande->x, grande->y, inimigo_grande) && (grande->x < 1200)){
            grande->vida--;
            play_sample(explosion, 255, 128, 1000, FALSE);
            grande->auxx = grande->x;
            grande->auxy = grande->y;
            aux->t->ativo = FALSE;
            grande->flagEx = 1;
            *pontuacao += 5;
            if(grande->vida == 0){
                play_sample(explosion, 255, 128, 1000, FALSE);
                grande->auxx = grande->x;
                grande->auxy = grande->y;
                aux->t->ativo = FALSE;
                grande->x = 1600;
                *pontuacao += 25;
                grande->flagEx = 1;
                grande->vida = 4;
            }
        }

        if(aux->t->ativo){
            Update_Tiro(aux->t);
            aux2 = aux;
            aux = aux->prox;
        }
        //Caso contrário é preciso removê-lo da lista
        else{
            //Início da lista
            if(aux == aux2){
                l->inicio = l->inicio->prox;
                Destroy_Tiro(aux->t);
                free(aux);
                aux2 = aux = l->inicio;
            }
            //Posição qualquer  (aux2 será o anterior a aux)
            else{
                aux2->prox = aux->prox;
                Destroy_Tiro(aux->t);
                free(aux);
                aux = aux2->prox;
            }
        }
    }
}
END_OF_FUNCTION(Update_Lista_Tiros)

void Draw_Lista_Tiros(Lista_tiros* l, BITMAP* buffer){
    No_Tiro* aux = l->inicio;
    while(aux != NULL){
        Draw_Tiro(aux->t, buffer);
        aux = aux->prox;
    }
}
END_OF_FUNCTION(Draw_Lista_Tiros)

void Destroy_Lista_Tiros(Lista_tiros* l){
    while(l->inicio != NULL){
        No_Tiro* aux = l->inicio;
        l->inicio = l->inicio->prox;
        Destroy_Tiro(aux->t);
        free(aux);
    }
}
END_OF_FUNCTION(Destroy_Lista_Tiros)
