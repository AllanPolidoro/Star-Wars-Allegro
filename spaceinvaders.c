#include "spaceinvaders.h"
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "tiro.h"

#define CHANCE_TIRO 100
#define VY_PLAYER 10
#define V_INIMIGO 1.8
#define VT_INIMIGO 13
#define DELAY 300
#define DEADLINE 125
#define HEART 500
#define T_EXPLOSAO 50
#define INTERVALO_NIVEL 60

volatile int exit_program;
volatile int ticks;
volatile int timer;
volatile int timerMS;

void fecha_programa(){ exit_program = TRUE; }
END_OF_FUNCTION(fecha_programa)

void tick_counter(){ ticks++; }
END_OF_FUNCTION(tick_counter)

void incrementa_timer(){ timer++; }
END_OF_FUNCTION(incrementa_timer)

void incrementa_timerMS(){ timerMS++; }
END_OF_FUNCTION(incrementa_timerMS)

void init(){                                        ///DONE
  allegro_init();
  install_timer();
  install_keyboard();
  set_color_depth(32);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1200, 600, 0, 0);
  set_window_title("Space Invaders");
  install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);

  exit_program = FALSE;
  LOCK_FUNCTION(fecha_programa);
  LOCK_VARIABLE(exit_program);
  set_close_button_callback(fecha_programa);

  ticks = 0;
  LOCK_FUNCTION(tick_counter);
  LOCK_VARIABLE(ticks);
  install_int_ex(tick_counter, BPS_TO_TIMER(60));

  timer = 0;
  LOCK_FUNCTION(incrementa_timer);
  LOCK_VARIABLE(timer);
  install_int_ex(incrementa_timer, SECS_TO_TIMER(1));
  install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);

  timerMS = 0;
  LOCK_FUNCTION(incrementa_timerMS);
  LOCK_VARIABLE(timerMS);
  install_int_ex(incrementa_timerMS, MSEC_TO_TIMER(T_EXPLOSAO));
}
END_OF_FUNCTION(init)

int max(int a, int b){                              ///DONE
  if(a >= b)
   return a;
  else
   return b;
}
END_OF_FUNCTION(max)

int min(int a, int b){                              ///DONE
  if(a <= b)
   return a;
  else
   return b;
}
END_OF_FUNCTION(min)

int pixel_perfect_colision(int x1, int y1, BITMAP* obj1, int x2, int y2, BITMAP* obj2){     ///DONE
   int i, j;
   int colisao = FALSE;

   //Se os retângulos estiverem se intersectando
   if(!( (x1 > x2 + obj2->w) || (y1 > y2 + obj2->h) || (x2 > x1 + obj1->w) || (y2 > y1 + obj1->h) ))
   {
     int cima = max(y1, y2);
	 int baixo = min(y1 + obj1->h, y2 + obj2->h);
	 int esquerda = max(x1, x2);
     int direita = min(x1 + obj1->w, x2 + obj2->w);

	 for(i = cima; i < baixo && !colisao; i++)
	 {
		for(j = esquerda; j < direita && !colisao; j++)
		{
		  if(getpixel(obj1, j-x1, i-y1) != makecol(255,0,255) && getpixel(obj2, j-x2, i-y2) != makecol(255,0,255))
           colisao = TRUE;
		}
	 }
   }

   return colisao;
}
END_OF_FUNCTION(pixel_perfect_colision)

int bounding_box_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){ ///DONE
    if( (x1 > x2 + w2) || (y1 > y2 + h2) || (x2 > x1 + w1) || (y2 > y1 + h1) )
     return FALSE;
    else
     return TRUE;
}

end_of_game jogo(char* nome, int tipo, novo_jogo game){                                     ///DONE
    srand(time(NULL));
    FILE *jogos_salvos;
    end_of_game this_game;
    char nome_do_arquivo[30];
    float v = V_INIMIGO;
    int menu = 0;
    int comando, i, prev_key_space, j = 0;
    int musica_aleatoria = 1 + (rand() % (12 - 1 + 1));
    int pontuacao = 0;
    int divPont = 1;
    int flagHeart = 0;
    int y_heart = 300;
    int x_heart = 1400;
    int aux = 0;
    int flagTempo = 0;
    int num_frames = 14;
    int frame_atual, vida_antes, vida_depois, nivel=0, contNivel=200;
    int vader = 0;
    int r2d2 = 0;
    int frame_w;

    init();
    install_mouse();

    Lista_tiros* lista = Create_Lista_Tiros();
    player_st player = {3, 10, SCREEN_H/2, VY_PLAYER};
    bonus_st b1 = {1600, (rand() % 300)+200, 0};
    bonus_st vb1 = {1600, (rand() % 300)+200, 0};

    naveE_st pequenas[6];
    for(i = 0; i < 6; i++){
        pequenas[i].vida = 1;
        pequenas[i].x = 1130;
        pequenas[i].y = 100 + (rand() % (500 - 100 + 1));
        pequenas[i].ty = 1400;
        pequenas[i].tx = 1400;
        pequenas[i].tv = VT_INIMIGO;
        pequenas[i].aleatorio_mov = 1;
        pequenas[i].aleatorio_tiro = 0;
        pequenas[i].flag = 0;
        pequenas[i].contH = 0;
        pequenas[i].contV = 0;
        pequenas[i].flagAB = 1;
        pequenas[i].flagT = 0;
        pequenas[i].cont_t = 0;
        pequenas[i].flagEx = 0;
        pequenas[i].auxx = 0;
        pequenas[i].auxy = 0;
        pequenas[i].flagTempo = 1;
        pequenas[i].auxTEx = 0;
        pequenas[i].cronEx = 0;
    }

    naveE_st medias[3];
    for(i = 0; i < 3; i++){
        medias[i].vida = 2;
        medias[i].x = 1130;
        medias[i].y = 100 + (rand() % (500 - 100 + 1));
        medias[i].ty = 1400;
        medias[i].tx = 1400;
        medias[i].tv = VT_INIMIGO;
        medias[i].aleatorio_mov = 1;
        medias[i].aleatorio_tiro = 0;
        medias[i].flag = 0;
        medias[i].contH = 0;
        medias[i].contV = 0;
        medias[i].flagAB = 1;
        medias[i].flagT = 0;
        medias[i].cont_t = 0;
        medias[i].flagEx = 0;
        medias[i].auxx = 0;
        medias[i].auxy = 0;
        medias[i].flagTempo = 1;
        medias[i].auxTEx = 0;
        medias[i].cronEx = 0;
    }

    naveE_st grande[1];
    for(i = 0; i < 1; i++){
        grande[i].vida = 4;
        grande[i].x = 1130;
        grande[i].y = 100 + (rand() % (500 - 100 + 1));
        grande[i].ty = 1400;
        grande[i].tx = 1400;
        grande[i].tv = VT_INIMIGO;
        grande[i].aleatorio_mov = 1;
        grande[i].aleatorio_tiro = 0;
        grande[i].flag = 0;
        grande[i].contH = 0;
        grande[i].contV = 0;
        grande[i].flagAB = 1;
        grande[i].flagT = 0;
        grande[i].cont_t = 0;
        grande[i].flagEx = 0;
        grande[i].auxx = 0;
        grande[i].auxy = 0;
        grande[i].flagTempo = 1;
        grande[i].auxTEx = 0;
        grande[i].cronEx = 0;
        }

    novo_jogo jogo_atual;

    ///BITMAPS

    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* fundo = load_bitmap ("Bitmaps/wallpaper.bmp", NULL);
    BITMAP* nave = load_bitmap ("Bitmaps/nave.bmp", NULL);
    BITMAP* v3 = load_bitmap ("Bitmaps/v3.bmp", NULL);
    BITMAP* v2 = load_bitmap ("Bitmaps/v2.bmp", NULL);
    BITMAP* v1 = load_bitmap ("Bitmaps/v1.bmp", NULL);

    BITMAP* B1 = load_bitmap ("Bitmaps/b1.bmp", NULL);
    BITMAP* VB1 = load_bitmap ("Bitmaps/vb1.bmp", NULL);

    BITMAP* pausar = load_bitmap ("Bitmaps/pausar.bmp", NULL);

    BITMAP* inimigo_pequeno = load_bitmap ("Bitmaps/inimigo_pequeno.bmp", NULL);
    BITMAP* inimigo_medio = load_bitmap ("Bitmaps/inimigo_medio.bmp", NULL);
    BITMAP* inimigo_grande = load_bitmap ("Bitmaps/inimigo_grande.bmp", NULL);

    BITMAP* tiro_inimigo = load_bitmap ("Bitmaps/tiro_inimigo.bmp", NULL);

    BITMAP* cursor = load_bitmap ("Bitmaps/transparente.bmp", NULL);

    BITMAP* explosao = load_bitmap("Bitmaps/explosao.bmp", NULL);
    BITMAP* heart = load_bitmap("Bitmaps/heart.bmp", NULL);

    frame_w = explosao->w / num_frames;

    ///MUSICAS
    FONT* arcade   = load_font("verdana.pcx", NULL, NULL);
    MIDI* musica1  = load_midi("sons/1.mid");
    MIDI* musica2  = load_midi("sons/2.mid");
    MIDI* musica3  = load_midi("sons/3.mid");
    MIDI* musica4  = load_midi("sons/4.mid");
    MIDI* musica5  = load_midi("sons/5.mid");
    MIDI* musica6  = load_midi("sons/6.mid");
    MIDI* musica7  = load_midi("sons/7.mid");
    MIDI* musica8  = load_midi("sons/8.mid");
    MIDI* musica9  = load_midi("sons/9.mid");
    MIDI* musica10 = load_midi("sons/10.mid");
    MIDI* musica11 = load_midi("sons/11.mid");
    MIDI* musica12 = load_midi("sons/12.mid");
    SAMPLE* laser  = load_sample("sons/laser_jogador.wav");
    SAMPLE* explosion = load_sample("sons/explosao.wav");
    SAMPLE* bad_bonus = load_sample("sons/bad_bonus.wav");
    SAMPLE* good_bonus = load_sample("sons/good_bonus.wav");
    SAMPLE* death = load_sample("sons/death.wav");
    SAMPLE* vida_extra = load_sample("sons/life.wav");

    if(musica_aleatoria == 1)
        play_midi(musica1, TRUE);
    else
        if(musica_aleatoria == 2)
            play_midi(musica2, TRUE);
        else
            if(musica_aleatoria == 3)
                play_midi(musica3, TRUE);
            else
                if(musica_aleatoria == 4)
                    play_midi(musica4, TRUE);
                else
                    if(musica_aleatoria == 5)
                        play_midi(musica5, TRUE);
                    else
                        if(musica_aleatoria == 6)
                            play_midi(musica6, TRUE);
                        else
                            if(musica_aleatoria == 7)
                                play_midi(musica7, TRUE);
                            else
                                if(musica_aleatoria == 8)
                                    play_midi(musica8, TRUE);
                                else
                                    if(musica_aleatoria == 9)
                                        play_midi(musica9, TRUE);
                                    else
                                        if(musica_aleatoria == 10)
                                            play_midi(musica10, TRUE);
                                        else
                                            if(musica_aleatoria == 11)
                                                play_midi(musica11, TRUE);
                                            else
                                                if(musica_aleatoria == 12)
                                                    play_midi(musica12, TRUE);

    for(i = 0; i < 6; i++){
        pequenas[i].x = 1400 + (rand() % (1800 - 1400 + 1));
    }
    for(i = 0; i < 3; i++){
        medias[i].x = 1400 + (rand() % (1800 - 1400 + 1));
    }

    if(tipo != 0){
        jogo_atual.tamanho_nome = game.tamanho_nome;
        jogo_atual.vez_jogada = game.vez_jogada+1;
        i = 0;
        strncpy(nome, game.nome_jogador, 10);
        pontuacao = game.pontuacao;
        player = game.jogador;
        grande [0] = game.inimigos[9];
        for(i = 0; i < 6; i++)
                pequenas[i] = game.inimigos[i];
        for(i = 6; i < 9; i++){
             medias[j] = game.inimigos[i];
            j++;
            }
        timer = game.timer;
        b1 = game.posicao_bonus[0];
        vb1 = game.posicao_bonus[1];
    }else{
        jogo_atual.vez_jogada = 49;
    }
    ///GAME LOOP
    while(!exit_program)
    {
        while(ticks > 0 && !exit_program){
            fflush(stdout);
            vida_antes=player.vida;

            if(pontuacao < 0)
                pontuacao = 0;

            ///Configuração inicial dos bônus
            b1.flagTela = r2d2;
            vb1.flagTela = vader;
            if((b1.flagTela == 1) || (b1.flagTela == 0))
                r2d2 = b1.flagTela;
            if((vb1.flagTela == 1) || (vb1.flagTela == 0))
                vader = vb1.flagTela;

            ///INPUT
            prev_key_space = key[KEY_SPACE];
            poll_keyboard();
            if(player.vida == 0){
                this_game.game_played = jogo_atual;
                this_game.comando = 1;
                this_game.game_played.pontuacao = pontuacao;
                this_game.game_played.tamanho_nome = game.tamanho_nome;
                strncpy(this_game.game_played.nome_jogador, nome, strlen(nome));
                fecha_programa();
            }

            ///Movimentos do jogador
            if(key[KEY_UP]||key[KEY_A])
                player.y = player.y-player.vy;
            if(key[KEY_DOWN]||key[KEY_Z])
                player.y = player.y+player.vy;
            if(player.y <= 40)
                player.y = 40;
            else
                if(player.y >= SCREEN_H-80)
                    player.y = SCREEN_H-80;
            if(key[KEY_SPACE] && !prev_key_space)
                Insere_Tiro(lista, player.x+80, player.y+25);

            ///NAVES INIMIGAS
            ///Naves P
            for(i = 0; i < 6; i++){
                if(pequenas[i].flagT == 0){
                    if((timer%2)!=0)
                        pequenas[i].flag=0;
                    if((timer%2)==0 && pequenas[i].flag==0){
                        pequenas[i].aleatorio_mov =(rand()%3);
                        pequenas[i].flag=1;
                        if((pequenas[i].aleatorio_mov==0) || (pequenas[i].aleatorio_mov==2)){
                            pequenas[i].contH=0;
                            pequenas[i].contV++;
                        }else{
                            pequenas[i].contH++;
                            pequenas[i].contV=0;
                        }
                    }
                    if(pequenas[i].contV<=4){
                        if(pequenas[i].aleatorio_mov==0||pequenas[i].aleatorio_mov==2){
                            if(pequenas[i].y < 80)
                                pequenas[i].aleatorio_mov = 2;
                            else{
                                if(pequenas[i].y > SCREEN_H-80)
                                    pequenas[i].aleatorio_mov = 0;
                            }
                        }
                    }else
                        pequenas[i].aleatorio_mov = 1;
                    if(pequenas[i].aleatorio_mov==0)
                        pequenas[i].y -= v;
                    else
                        if(pequenas[i].aleatorio_mov==1)
                            pequenas[i].x -= v;
                        else
                            if(pequenas[i].aleatorio_mov==2)
                                pequenas[i].y += v;
                }
                if(pequenas[i].flagT==1){
                    pequenas[i].cont_t++;
                        if(pequenas[i].cont_t>DELAY){
                            pequenas[i].cont_t=0;
                            pequenas[i].flagT=0;
                        }
                }
                //Colisão da P com o Player
                if (pequenas[i].x<DEADLINE){
                    player.vida--;
                    pequenas[i].x=1400;
                    pequenas[i].flagT=1;
                    player.flagHit=1;
                    flagTempo=1;
                }
                //Ataque da nave P
                pequenas[i].aleatorio_tiro = 1+(rand()%(1000-1+1));
                if(pequenas[i].tx > -350 && pequenas[i].tx < SCREEN_W){
                    pequenas[i].tx -= pequenas[i].tv;
                }else{
                    if((pequenas[i].aleatorio_tiro % CHANCE_TIRO == 0) && pequenas[i].x < 1200){
                        play_sample(laser, 140, 128, 1000, FALSE);
                        pequenas[i].tx = pequenas[i].x;
                        pequenas[i].ty = pequenas[i].y;
                    }
                }
                //Acerto do tiro da P no Player
                if(pixel_perfect_colision(pequenas[i].tx, pequenas[i].ty, tiro_inimigo, player.x, player.y-35, nave) == TRUE){
                    play_sample(death, 255, 128, 1000, FALSE);
                    player.vida--;
                    pequenas[i].ty = 1400;
                    pequenas[i].tx = 1400;
                    player.flagHit=1;
                    flagTempo=1;
                }
            }

            ///Naves Medias
            for(i = 0; i < 3; i++){
                if(medias[i].flagT == 0){
                    if((timer%2)!=0)
                        medias[i].flag=0;
                    if((timer%2)==0 && medias[i].flag==0){
                        medias[i].aleatorio_mov =(rand()%3);
                        medias[i].flag=1;
                        if((medias[i].aleatorio_mov==0) || (medias[i].aleatorio_mov==2)){
                            medias[i].contH=0;
                            medias[i].contV++;
                        }else{
                            medias[i].contH++;
                            medias[i].contV=0;
                        }
                    }
                    if(medias[i].contV<=4){
                        if(medias[i].aleatorio_mov==0 || medias[i].aleatorio_mov==2){
                            if(medias[i].y < 60)
                                medias[i].aleatorio_mov = 2;
                            else{
                                if(medias[i].y > SCREEN_H-140)
                                    medias[i].aleatorio_mov = 0;
                            }
                        }
                    }else
                        medias[i].aleatorio_mov = 1;
                    if(medias[i].aleatorio_mov==0)
                        medias[i].y -= v;
                    else
                        if(medias[i].aleatorio_mov==1)
                            medias[i].x -= v;
                        else
                            if(medias[i].aleatorio_mov==2)
                                medias[i].y += v;
                }
                if(medias[i].flagT==1){
                    medias[i].cont_t++;
                        if (medias[i].cont_t>DELAY){
                            medias[i].cont_t=0;
                            medias[i].flagT=0;
                        }
                }

                //Colisão da M com o Player
                if (medias[i].x<DEADLINE){
                    player.vida--;
                    medias[i].x=1400;
                    medias[i].flagT=1;
                    player.flagHit=1;
                    flagTempo=1;
                }
                //Ataque da nave inimiga M
                medias[i].aleatorio_tiro = 1+(rand()%(1000-1+1));
                if(medias[i].tx > -350 && medias[i].tx < SCREEN_W){
                    medias[i].tx -= medias[i].tv;
                }else{
                    if((medias[i].aleatorio_tiro % CHANCE_TIRO == 0) && medias[i].x < 1200){
                        play_sample(laser, 140, 128, 1000, FALSE);
                        medias[i].tx = medias[i].x;
                        medias[i].ty = medias[i].y;
                    }
                }
                //Acerto do tiro da M no Player
                if(pixel_perfect_colision(medias[i].tx, medias[i].ty, tiro_inimigo, player.x, player.y-35, nave) == TRUE){
                    play_sample(death, 255, 128, 1000, FALSE);
                    player.vida--;
                    medias[i].ty = 1400;
                    medias[i].tx = 1400;
                    player.flagHit=1;
                    flagTempo=1;
                }
            }

            ///Naves Grandes
            for(i = 0; i < 1; i++){
                if(grande[i].flagT == 0){
                    if((timer%2)!=0)
                        grande[i].flag=0;
                    if((timer%2)==0 && grande[i].flag==0){
                        grande[i].aleatorio_mov =(rand()%3);
                        grande[i].flag=1;
                        if((grande[i].aleatorio_mov==0) || (grande[i].aleatorio_mov==2)){
                            grande[i].contH=0;
                            grande[i].contV++;
                        }else{
                            grande[i].contH++;
                            grande[i].contV=0;
                        }
                    }
                    if(grande[i].contV<=4){
                        if(grande[i].aleatorio_mov==0 || grande[i].aleatorio_mov==2){
                            if(grande[i].y < 60)
                                grande[i].aleatorio_mov = 2;
                            else{
                                if(grande[i].y > SCREEN_H-140)
                                grande[i].aleatorio_mov = 0;
                            }
                        }
                    }else
                        grande[i].aleatorio_mov = 1;
                    if(grande[i].aleatorio_mov==0)
                        grande[i].y -= v;
                    else
                        if(grande[i].aleatorio_mov==1)
                            grande[i].x -= v;
                        else
                            if(grande[i].aleatorio_mov==2)
                                grande[i].y += v;
                }
                if(grande[i].flagT==1){
                    grande[i].cont_t++;
                    if (grande[i].cont_t>DELAY){
                        grande[i].cont_t=0;
                        grande[i].flagT=0;
                    }
                }
                //Colisão de G com o Player
                if (grande[i].x<DEADLINE){
                    player.vida--;
                    grande[i].x=1400;
                    grande[i].flagT=1;
                    player.flagHit=1;
                    flagTempo=1;
                }
                //Ataque da nave G
                grande[i].aleatorio_tiro = 1+(rand()%(1000-1+1));
                if(grande[i].tx > -350 && grande[i].tx < SCREEN_W){
                    grande[i].tx -= grande[i].tv;
                }else{
                    if((grande[i].aleatorio_tiro % CHANCE_TIRO == 0) && grande[i].x < 1200){
                        play_sample(laser, 140, 128, 1000, FALSE);
                        grande[i].tx = grande[i].x;
                        grande[i].ty = grande[i].y;
                    }
                }
                //Acerto do tiro da G no Player
                if(pixel_perfect_colision(grande[i].tx, grande[i].ty, tiro_inimigo, player.x, player.y-35, nave) == TRUE){
                    play_sample(death, 255, 128, 1000, FALSE);
                    player.vida--;
                    grande[i].ty = 1400;
                    grande[i].tx = 1400;
                    player.flagHit=1;
                    flagTempo=1;
                }
            }

            ///Controle de nível
            vida_depois=player.vida;
            if(vida_antes>vida_depois)
                nivel=0;
            if(pontuacao>=contNivel){
                pequenas[0].tv += ((float)nivel)/8;
                pequenas[1].tv += ((float)nivel)/8;
                pequenas[2].tv += ((float)nivel)/8;
                pequenas[3].tv += ((float)nivel)/8;
                pequenas[4].tv += ((float)nivel)/8;
                pequenas[5].tv += ((float)nivel)/8;
                pequenas[6].tv += ((float)nivel)/8;
                medias[0].tv += ((float)nivel)/8;
                medias[1].tv += ((float)nivel)/8;
                medias[2].tv += ((float)nivel)/8;
                grande[0].tv += ((float)nivel)/8;
                v += ((float)nivel)/8;
                contNivel+=200;
                nivel++;
            }
            if(nivel==0){
                 pequenas[0].tv = VT_INIMIGO;
                 pequenas[1].tv = VT_INIMIGO;
                 pequenas[2].tv = VT_INIMIGO;
                 pequenas[3].tv = VT_INIMIGO;
                 pequenas[4].tv = VT_INIMIGO;
                 pequenas[5].tv = VT_INIMIGO;
                 pequenas[6].tv = VT_INIMIGO;
                 medias[0].tv = VT_INIMIGO;
                 medias[1].tv = VT_INIMIGO;
                 medias[2].tv = VT_INIMIGO;
                 grande[0].tv = VT_INIMIGO;
                 v = V_INIMIGO;
                 nivel++;
            }

            ///UPDATE
            Update_Lista_Tiros(&pontuacao, buffer, &timerMS, lista, explosion, &pequenas[0], &pequenas[1], &pequenas[2], &pequenas[3], &pequenas[4], &pequenas[5],
                               &medias[0], &medias[1], &medias[2], &grande[0], inimigo_pequeno, inimigo_medio, inimigo_grande);

            ///DRAW
            draw_sprite(buffer, fundo, 0, 0);
            Draw_Lista_Tiros(lista, buffer);
            for(i = 0; i < 6; i++){
                draw_sprite(buffer, inimigo_pequeno, pequenas[i].x, pequenas[i].y);
                draw_sprite(buffer, tiro_inimigo, pequenas[i].tx, pequenas[i].ty+32);
            }
            for(i = 0; i < 3; i++){
                draw_sprite(buffer, inimigo_medio, medias[i].x, medias[i].y);
                draw_sprite(buffer, tiro_inimigo, medias[i].tx, medias[i].ty+62);
            }
            for(i = 0; i < 1; i++){
                draw_sprite(buffer, inimigo_grande, grande[i].x, grande[i].y);
                draw_sprite(buffer, tiro_inimigo, grande[i].tx, grande[i].ty+62);
            }
            //Se houve dano no Player
            if(player.flagHit==1){
                if (flagTempo==1){
                    aux=timer-20;
                    flagTempo=0;
                }
                if(aux<=timer){
                    set_trans_blender(45, 150, 255, 128);
                    draw_sprite_ex(buffer, nave, player.x, player.y, DRAW_SPRITE_TRANS, DRAW_SPRITE_NO_FLIP);
                    aux+=2;
                }else player.flagHit=0;
            }
            if(player.flagHit==0)
                draw_sprite_ex(buffer, nave, player.x, player.y, DRAW_SPRITE_NORMAL, DRAW_SPRITE_NO_FLIP);
            //Se o player perdeu vida
            if(player.vida>=1){
                draw_sprite(buffer, v1, (SCREEN_W-150), 5);
                if(player.vida>=2){
                    draw_sprite(buffer, v2, (SCREEN_W-100), 5);
                    if(player.vida>=3){
                        draw_sprite(buffer, v3, (SCREEN_W-50), 5);
                    }
                }
            }

            ///Explosões
            //explosao das naves P
            for(i = 0; i < 6; i++){
                pequenas[i].cronEx=timerMS;
                if((pequenas[i].flagEx==1) && (pequenas[i].flagTempo==1)){
                    pequenas[i].auxTEx=timerMS;
                    pequenas[i].flagTempo=1;
                }
                if(pequenas[i].flagEx==1){
                    pequenas[i].flagTempo=0;
                    frame_atual = pequenas[i].cronEx-pequenas[i].auxTEx;
                    masked_blit(explosao, buffer, frame_atual*frame_w, 0, pequenas[i].auxx, pequenas[i].auxy, frame_w, explosao->h);
                    if(frame_atual>=14){
                        pequenas[i].flagEx=0;
                        pequenas[i].flagTempo=1;
                    }
                }
            }
            //explosão das naves M
            for(i = 0; i < 3; i++){
                medias[i].cronEx=timerMS;
                if((medias[i].flagEx==1) && (medias[i].flagTempo==1)){
                    medias[i].auxTEx=timerMS;
                    medias[i].flagTempo=1;
                }
                if(medias[i].flagEx==1){
                    medias[i].flagTempo=0;
                    frame_atual = medias[i].cronEx-medias[i].auxTEx;
                    masked_blit(explosao, buffer, frame_atual*frame_w, 0, medias[i].auxx, medias[i].auxy, frame_w, explosao->h);
                    if(frame_atual>=14){
                        medias[i].flagEx=0;
                        medias[i].flagTempo=1;
                    }
                }
            }
            //explosao das naves G
            for(i = 0; i < 1; i++){
                grande[i].cronEx=timerMS;
                if((grande[i].flagEx == 1) && (grande[i].flagTempo == 1)){
                    grande[i].auxTEx = timerMS;
                    grande[i].flagTempo = 1;
                }
                if(grande[i].flagEx==1){
                    grande[i].flagTempo=0;
                    frame_atual = grande[i].cronEx-grande[i].auxTEx;
                    masked_blit(explosao, buffer, frame_atual*frame_w, 0, grande[i].auxx+200, grande[i].auxy+70, frame_w, explosao->h);        //LEMBRETE: calibrar explosão da nave grande
                    if(frame_atual >= 14){
                        grande[i].flagEx = 0;
                        grande[i].flagTempo = 1;
                    }
                }
            }

            ///Surgimento de Bônus
            // Surgimento de VB1
            if((((rand()%200000)%1000) == 0) && (vader == 0))
                vader = 1;
            if (vader == 1){
                vb1.y += 2*sin((timer*40)%360);
                vb1.x -= 3*v;
                draw_sprite(buffer, VB1, vb1.x, vb1.y);
            }
            if (vb1.x <= 0){
                vb1.y = (rand() % 300)+200;
                vb1.x = 1400;
                vader = 0;
            }
            if((pixel_perfect_colision(player.x, player.y, nave, vb1.x, vb1.y, VB1) == TRUE)){
                play_sample(bad_bonus, 255, 128, 1000, FALSE);
                vb1.y = (rand()%300)+200;
                vb1.x = 1400;
                vader = 0;
                pontuacao = pontuacao - 30;
            }
            // Surgimento de B1
            if(((rand()%100000)%1000)==0 && r2d2==0)
                r2d2 = 1;
            if (r2d2 == 1){
                b1.y += 2*sin((timer*40)%360);
                b1.x -= 3*v;
                draw_sprite(buffer, B1, b1.x, b1.y);
            }
            if (b1.x <= 0){
                b1.y = (rand() % 300)+200;
                b1.x = 1400;
                r2d2 = 0;
            }
            if((pixel_perfect_colision(player.x, player.y, nave, b1.x, b1.y, B1) == TRUE)){
                b1.y = (rand() % 300)+200;
                b1.x = 1400;
                r2d2 = 0;
                pontuacao += 30;}

            ///Ganhar vidas
            if(player.vida <= 0)
                player.vida = 0;
            if((pontuacao/HEART)==divPont){
                flagHeart = 1;
                y_heart += 2*sin((timer*40)%360);
                x_heart -= 3*v;
                draw_sprite(buffer, heart, x_heart, y_heart);
            }
            if ((pixel_perfect_colision(player.x, player.y, nave, x_heart, y_heart, heart)==TRUE) && (pontuacao/HEART) == divPont){
                divPont++;
                player.vida++;
                play_sample(vida_extra, 255, 128, 1000, FALSE);
                y_heart = (rand() % 300) + 200;
                x_heart = 1400;
            }
            if(player.vida >= 3)
                player.vida = 3;
            if(x_heart <= 0){
                divPont++;
                y_heart = (rand() % 300) + 200;
                x_heart = 1400;
            }

            ///SAVE
            if(key[KEY_P]){
                while(!exit_program){
                    draw_sprite(screen, pausar, 240, 110);
                    clear_keybuf();
                    if(key[KEY_R]){
                        comando = 0;
                        fecha_programa();
                    }else
                        if(key[KEY_S]){
                            strncpy(jogo_atual.nome_jogador, nome, 10);
                            i = 0;
                            while(nome[i] != '\0'){
                                nome_do_arquivo[i] = nome[i];
                                i++;
                            }
                            nome_do_arquivo[i] = (jogo_atual.vez_jogada);
                            i++;
                            nome_do_arquivo[i] = '.';
                            i++;
                            nome_do_arquivo[i] = 'b';
                            i++;
                            nome_do_arquivo[i] = 'i';
                            i++;
                            nome_do_arquivo[i] = 'n';
                            i++;
                            nome_do_arquivo[i] = '\0';
                            i = 0;

                            jogo_atual.timer = timer;
                            jogo_atual.existencia = 1;
                            jogo_atual.pontuacao = pontuacao;
                            jogo_atual.jogador = player;
                            jogo_atual.divPont = divPont;
                            jogo_atual.x_heart = x_heart;
                            jogo_atual.y_heart = y_heart;
                            jogo_atual.vader = vader;
                            jogo_atual.r2d2 = r2d2;
                            jogo_atual.nivel = nivel-1;
                            jogo_atual.contNivel = contNivel-200;
                            for(i = 0; i < 6; i++)
                                jogo_atual.inimigos[i] = pequenas[i];
                            for(i = 6; i < 9; i++){
                                jogo_atual.inimigos[i] = medias[j];
                                j++;
                            }
                            jogo_atual.inimigos[9] = grande[0];
                            jogo_atual.posicao_bonus[0] = b1;
                            jogo_atual.tamanho_nome = game.tamanho_nome;
                            jogo_atual.posicao_bonus[1] = vb1;
                            if(!(jogos_salvos = fopen(nome_do_arquivo, "wb")))
                                printf("Erro ao abrir o arquivo %s\n", nome_do_arquivo);
                            else{
                                if(fwrite(&jogo_atual, sizeof(novo_jogo), 1, jogos_salvos) != 1)
                                    printf("Erro ao escrever no arquivo\n");
                            }
                            fclose(jogos_salvos);
                            this_game.game_played = jogo_atual;
                            this_game.game_played.tamanho_nome = game.tamanho_nome;
                            this_game.game_played.pontuacao = pontuacao;
                            strncpy(this_game.game_played.nome_jogador, nome, strlen(nome));
                            this_game.comando = 0;
                            fecha_programa();
                        }
                }
            }

            ///DADOS EM TELA
            draw_sprite(buffer, cursor, mouse_x-6, mouse_y);
            textprintf_ex(buffer, arcade, 20, 20, makecol(255,255,255), -1, "NOME: %s", nome);
            textout_centre_ex(buffer, arcade, "VIDAS:", SCREEN_W-200, 20, makecol(255,255,255), -1);
            textprintf_centre_ex(buffer, arcade, 8*SCREEN_W/16+100, 20,  makecol(255,255,255), -1, "TEMPO: %d min e %d s", timer/60, timer%60);
            textprintf_centre_ex(buffer, arcade, 4*SCREEN_W/16+100, 20,  makecol(255,255,255), -1, "PONTUACAO: %d", pontuacao);
            draw_sprite(screen, buffer, 0, 0);
            clear(buffer);
            vsync();
            ticks--;
        }
    }

    ///FINALIZAÇÂO
    Destroy_Lista_Tiros(lista);
    destroy_bitmap(buffer);
    destroy_bitmap(fundo);
    destroy_bitmap(nave);
    destroy_bitmap(v3);
    destroy_bitmap(v2);
    destroy_bitmap(v1);
    destroy_bitmap(B1);
    destroy_bitmap(VB1);
    destroy_bitmap(pausar);
    destroy_bitmap(inimigo_pequeno);
    destroy_bitmap(inimigo_medio);
    destroy_bitmap(inimigo_grande);
    destroy_bitmap(tiro_inimigo);
    destroy_bitmap(cursor);
    destroy_bitmap(explosao);
    destroy_bitmap(heart);

    clear_keybuf();
    allegro_exit();
    return this_game;
}
END_OF_FUNCTION(jogo)

int menu(){                                         ///DONE
    int operacao;       // 1 para novo jogo, 2 para carregar jogo, 3 para ver ranking e 4 para sair
    int guia;

    int newgamex=30;
    int newgamey=300;

    int load_gamex=30;
    int load_gamey=370;

    int highscorex=30;
    int highscorey=440;

    int quitx=30;
    int quity=510;

    init();
    install_mouse();

    MIDI* musica = load_midi("sons/main.mid");
    play_midi(musica, TRUE);

    ///BITMAPS
    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* fundo = load_bitmap ("Bitmaps/ST_SW.bmp", NULL);

    BITMAP* new_game = load_bitmap ("Bitmaps/new_game.bmp", NULL);
    BITMAP* load_game = load_bitmap ("Bitmaps/load_game.bmp", NULL);
    BITMAP* highscore = load_bitmap ("Bitmaps/highscore.bmp", NULL);
    BITMAP* quit = load_bitmap ("Bitmaps/quit.bmp", NULL);

    BITMAP* new_gameg = load_bitmap ("Bitmaps/new_gameg.bmp", NULL);
    BITMAP* load_gameg = load_bitmap ("Bitmaps/load_gameg.bmp", NULL);
    BITMAP* highscoreg = load_bitmap ("Bitmaps/highscoreg.bmp", NULL);
    BITMAP* quitg = load_bitmap ("Bitmaps/quitg.bmp", NULL);

    BITMAP* cursor = load_bitmap ("Bitmaps/cursor.bmp", NULL);

    ///GAME LOOP
    while(!exit_program)
    {
        while(ticks > 0 && !exit_program){
            fflush(stdout);

            ///INPUT
            draw_sprite(buffer, fundo, 0, 0);
            if(key[KEY_N]){
                operacao = 1;
                fecha_programa();
                }else
                    if(key[KEY_C]){
                        operacao = 2;
                        fecha_programa();
                    }else
                        if(key[KEY_T]){
                            operacao = 3;
                            fecha_programa();
                        }else
                            if(key[KEY_Q]){
                                operacao = 4;
                                fecha_programa();
                            }
            if( (bounding_box_collision(newgamex, newgamey, new_game->w, new_game->h, mouse_x-6, mouse_y, cursor->w, cursor->h) == TRUE)){              //new game
                guia=1;
                draw_sprite(buffer, new_gameg, newgamex, newgamey);
                if(mouse_b == 1){
                    operacao = 1;
                    fecha_programa();
                }
            }else{
                if((bounding_box_collision(load_gamex, load_gamey, load_game->w, load_game->h, mouse_x-6, mouse_y, cursor->w, cursor->h) == TRUE)){     //load game
                    guia=2;
                    draw_sprite(buffer, load_gameg, load_gamex, load_gamey);
                    if(mouse_b == 1){
                        operacao = 2;
                        fecha_programa();
                    }
                }else{
                    if((bounding_box_collision(highscorex, highscorey, highscore->w, highscore->h, mouse_x-6, mouse_y, cursor->w, cursor->h) == TRUE)){ //highscore
                        guia=3;
                        draw_sprite(buffer, highscoreg, highscorex, highscorey);
                        if(mouse_b == 1){
                            operacao = 3;
                            fecha_programa();
                        }
                    }else{
                        if((bounding_box_collision(quitx, quity, quit->w, quit->h, mouse_x-6, mouse_y, cursor->w, cursor->h) == TRUE)){                 //Quit
                            guia=4;
                            draw_sprite(buffer, quitg, quitx, quity);
                            if(mouse_b == 1){
                                operacao = 4;
                                fecha_programa();
                            }
                        }else guia=0;
                    }
                }
            }

            ///DRAW (Desenha na tela)
            if(guia!=1)
                draw_sprite(buffer, new_game, newgamex, newgamey);
            if(guia!=2)
                draw_sprite(buffer, load_game, load_gamex, load_gamey);
            if(guia!=3)
                draw_sprite(buffer, highscore, highscorex, highscorey);
            if(guia!=4)
                draw_sprite(buffer, quit, quitx, quity);
            draw_sprite(buffer, cursor, mouse_x-6, mouse_y);

            draw_sprite(screen, buffer, 0, 0);
            clear(buffer);
            ticks--;
        }
    }
    ///FINALIZAÇÃO
    destroy_bitmap(buffer);
    destroy_bitmap(fundo);
    destroy_bitmap(new_game);
    destroy_bitmap(new_gameg);
    destroy_bitmap(load_game);
    destroy_bitmap(load_gameg);
    destroy_bitmap(highscore);
    destroy_bitmap(highscoreg);
    destroy_bitmap(quit);
    destroy_bitmap(quitg);
    destroy_bitmap(cursor);
    clear_keybuf();
    allegro_exit();
    return operacao;
}
END_OF_FUNCTION(menu)

int ler_nome(char* nome){                           ///DONE
    int i = 0;
    int j;
    int esperar;
    char jogador[11];

    srand(time(NULL));
    init();
    install_mouse();
    FONT* arcade = load_font("verdana.pcx", NULL, NULL);
    MIDI* musica = load_midi("sons/theforce.mid");
    play_midi(musica, TRUE);

    ///BITMAPS
    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* fundo = load_bitmap ("Bitmaps/nome.bmp", NULL);
    BITMAP* cursor = load_bitmap ("Bitmaps/transparente.bmp", NULL);

    ///GAME LOOP
    while(!exit_program)
    {
        while(ticks > 0 && !exit_program){
            fflush(stdout);
            ///INPUT
            if(i < 11){
                if(key[KEY_A]){
                    jogador[i] = 'a';
                    esperar = 1;
                    i++;
                }else
                    if(key[KEY_B]){
                        jogador[i] = 'b';
                        esperar = 1;
                        i++;
                    }else
                        if(key[KEY_C]){
                            jogador[i] = 'c';
                            esperar = 1;
                            i++;
                        }else
                            if(key[KEY_D]){
                                jogador[i] = 'd';
                                esperar = 1;
                                i++;
                            }else
                                if(key[KEY_E]){
                                    jogador[i] = 'e';
                                    esperar = 1;
                                    i++;
                                }else
                                    if(key[KEY_F]){
                                        jogador[i] = 'f';
                                        esperar = 1;
                                        i++;
                                    }else
                                        if(key[KEY_G]){
                                            jogador[i] = 'g';
                                            esperar = 1;
                                            i++;
                                        }else
                                            if(key[KEY_H]){
                                                jogador[i] = 'h';
                                                esperar = 1;
                                                i++;
                                            }else
                                                if(key[KEY_I]){
                                                    jogador[i] = 'i';
                                                    esperar = 1;
                                                    i++;
                                                }else
                                                    if(key[KEY_J]){
                                                        jogador[i] = 'j';
                                                        esperar = 1;
                                                        i++;
                                                    }else
                                                        if(key[KEY_K]){
                                                            jogador[i] = 'k';
                                                            esperar = 1;
                                                            i++;
                                                        }else
                                                            if(key[KEY_L]){
                                                                jogador[i] = 'l';
                                                                esperar = 1;
                                                                i++;
                                                            }else
                                                                if(key[KEY_M]){
                                                                    jogador[i] = 'm';
                                                                    esperar = 1;
                                                                    i++;
                                                                }else
                                                                    if(key[KEY_N]){
                                                                        jogador[i] = 'n';
                                                                        esperar = 1;
                                                                        i++;
                                                                    }else
                                                                        if(key[KEY_O]){
                                                                            jogador[i] = 'o';
                                                                            esperar = 1;
                                                                            i++;
                                                                        }else
                                                                            if(key[KEY_P]){
                                                                                jogador[i] = 'p';
                                                                                esperar = 1;
                                                                                i++;
                                                                            }else
                                                                                if(key[KEY_Q]){
                                                                                    jogador[i] = 'q';
                                                                                    esperar = 1;
                                                                                    i++;
                                                                                }else
                                                                                    if(key[KEY_R]){
                                                                                        jogador[i] = 'r';
                                                                                        esperar = 1;
                                                                                        i++;
                                                                                    }else
                                                                                        if(key[KEY_S]){
                                                                                            jogador[i] = 's';
                                                                                            esperar = 1;
                                                                                            i++;
                                                                                        }else
                                                                                            if(key[KEY_T]){
                                                                                                jogador[i] = 't';
                                                                                                esperar = 1;
                                                                                                i++;
                                                                                            }else
                                                                                                if(key[KEY_U]){
                                                                                                    jogador[i] = 'u';
                                                                                                    esperar = 1;
                                                                                                    i++;
                                                                                                }else
                                                                                                    if(key[KEY_V]){
                                                                                                        jogador[i] = 'v';
                                                                                                        esperar = 1;
                                                                                                        i++;
                                                                                                    }else
                                                                                                        if(key[KEY_W]){
                                                                                                            jogador[i] = 'w';
                                                                                                            esperar = 1;
                                                                                                            i++;
                                                                                                        }else
                                                                                                            if(key[KEY_X]){
                                                                                                                jogador[i] = 'x';
                                                                                                                esperar = 1;
                                                                                                                i++;
                                                                                                            }else
                                                                                                                if(key[KEY_Y]){
                                                                                                                    jogador[i] = 'y';
                                                                                                                    esperar = 1;
                                                                                                                    i++;
                                                                                                                }else
                                                                                                                    if(key[KEY_Z]){
                                                                                                                        jogador[i] = 'z';
                                                                                                                        esperar = 1;
                                                                                                                        i++;
                                                                                                                    }else
                                                                                                                        if(key[KEY_BACKSPACE]){
                                                                                                                            i--;
                                                                                                                            esperar = 1;
                                                                                                                            jogador[i] = ' ';
                                                                                                                        }else
                                                                                                                            if(key[KEY_ENTER] && jogador[0] != ' '){
                                                                                                                                strncpy(nome, jogador, 10);
                                                                                                                                fecha_programa();
                                                                                                                            }else{
                                                                                                                                esperar = 0;
                                                                                                                            }
            }
            if(esperar == 1)
                rest(100);

            jogador[0] = toupper(jogador[0]);
            jogador[i] = '\0';

            for(j = 0; j < 11; j++){
                if((nome[j] < 65 && nome[j] > 90) && (nome[j] < 97 && nome[j] > 122))
                    nome[j] = '\0';
            }

            ///DRAW (Desenha na tela)
            draw_sprite(buffer, fundo, 0, 0);
            draw_sprite(buffer, cursor, mouse_x-6, mouse_y);
            textprintf_ex(buffer, arcade, 500, SCREEN_H/2, makecol(255,242,0), -1, "Digite seu nome: %s", jogador);
            textout_centre_ex(buffer, font, nome, SCREEN_W/16, SCREEN_W/2+20, makecol(255, 255, 255), -1);

            draw_sprite(screen, buffer, 0, 0);
            clear(buffer);
            ticks--;
        }
    }

    ///FINALIZAÇÃO
    destroy_bitmap(buffer);
    destroy_bitmap(fundo);
    destroy_bitmap(cursor);
    clear_keybuf();
    allegro_exit();
    return i;
}
END_OF_FUNCTION(ler_nome)

void highscores(){                                  ///DONE
    init();
    install_mouse();

    int i;
    rank_game scores[10];
    FONT* arcade = load_font("verdana.pcx", NULL, NULL);
    FILE *arq;
    MIDI* musica = load_midi("sons/cantina.mid");
    play_midi(musica, TRUE);

    ///BITMAPS
    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* fundo = load_bitmap ("Bitmaps/fundo.bmp", NULL);
    BITMAP* highscores = load_bitmap ("Bitmaps/highscores.bmp", NULL);
    BITMAP* cursor = load_bitmap ("Bitmaps/cursor.bmp", NULL);

    ///LOOP PRINCIPAL
    while(!exit_program)
    {
        while(ticks > 0 && !exit_program){
            fflush(stdin);
            ///INPUT
            if(key[KEY_B])
                fecha_programa();

            ///DRAW (Desenha na tela)
            draw_sprite(buffer, fundo, 0, 0);
            draw_sprite(buffer, highscores, 140, 10);
            draw_sprite(buffer, cursor, mouse_x-6, mouse_y);

            if(!(arq = fopen("ranking.txt", "r")))
                printf("Erro ao abrir o ranking!\n");
            else{
                for(i = 0; i < 10; i++){
                    if(fscanf(arq, "%s %d\n", scores[i].nome, &scores[i].pontuacao) == -1)
                        printf("Erro ao abrir o arquivo!\n");
                }fclose(arq);
            }

            textprintf_centre_ex(buffer, arcade, 340, 150,  makecol(255,242,0), -1, "RANK");
            textprintf_centre_ex(buffer, arcade, 340, 180,  makecol(255,255,255), -1, "TOP");
            textprintf_centre_ex(buffer, arcade, 340, 220,  makecol(255,255,255), -1, "2ND");
            textprintf_centre_ex(buffer, arcade, 340, 260,  makecol(255,255,255), -1, "3RD");
            textprintf_centre_ex(buffer, arcade, 340, 300,  makecol(255,255,255), -1, "4TH");
            textprintf_centre_ex(buffer, arcade, 340, 340,  makecol(255,255,255), -1, "5TH");
            textprintf_centre_ex(buffer, arcade, 340, 380,  makecol(255,255,255), -1, "6TH");
            textprintf_centre_ex(buffer, arcade, 340, 420,  makecol(255,255,255), -1, "7TH");
            textprintf_centre_ex(buffer, arcade, 340, 460,  makecol(255,255,255), -1, "8TH");
            textprintf_centre_ex(buffer, arcade, 340, 500,  makecol(255,255,255), -1, "9TH");
            textprintf_centre_ex(buffer, arcade, 340, 540,  makecol(255,255,255), -1, "10TH");


            textprintf_centre_ex(buffer, arcade, 590, 150,  makecol(255,242,0), -1, "NAME");
            textprintf_centre_ex(buffer, arcade, 590, 180,  makecol(255,255,255), -1, "%s", scores[0].nome);
            textprintf_centre_ex(buffer, arcade, 590, 220,  makecol(255,255,255), -1, "%s", scores[1].nome);
            textprintf_centre_ex(buffer, arcade, 590, 260,  makecol(255,255,255), -1, "%s", scores[2].nome);
            textprintf_centre_ex(buffer, arcade, 590, 300,  makecol(255,255,255), -1, "%s", scores[3].nome);
            textprintf_centre_ex(buffer, arcade, 590, 340,  makecol(255,255,255), -1, "%s", scores[4].nome);
            textprintf_centre_ex(buffer, arcade, 590, 380,  makecol(255,255,255), -1, "%s", scores[5].nome);
            textprintf_centre_ex(buffer, arcade, 590, 420,  makecol(255,255,255), -1, "%s", scores[6].nome);
            textprintf_centre_ex(buffer, arcade, 590, 460,  makecol(255,255,255), -1, "%s", scores[7].nome);
            textprintf_centre_ex(buffer, arcade, 590, 500,  makecol(255,255,255), -1, "%s", scores[8].nome);
            textprintf_centre_ex(buffer, arcade, 590, 540,  makecol(255,255,255), -1, "%s", scores[9].nome);



            textprintf_centre_ex(buffer, arcade, 840, 150,  makecol(255,242,0), -1, "SCORE");
            textprintf_centre_ex(buffer, arcade, 840, 180,  makecol(255,255,255), -1, "%d", scores[0].pontuacao);
            textprintf_centre_ex(buffer, arcade, 840, 220,  makecol(255,255,255), -1, "%d", scores[1].pontuacao);
            textprintf_centre_ex(buffer, arcade, 840, 260,  makecol(255,255,255), -1, "%d", scores[2].pontuacao);
            textprintf_centre_ex(buffer, arcade, 840, 300,  makecol(255,255,255), -1, "%d", scores[3].pontuacao);
            textprintf_centre_ex(buffer, arcade, 840, 340,  makecol(255,255,255), -1, "%d", scores[4].pontuacao);
            textprintf_centre_ex(buffer, arcade, 840, 380,  makecol(255,255,255), -1, "%d", scores[5].pontuacao);
            textprintf_centre_ex(buffer, arcade, 840, 420,  makecol(255,255,255), -1, "%d", scores[6].pontuacao);
            textprintf_centre_ex(buffer, arcade, 840, 460,  makecol(255,255,255), -1, "%d", scores[7].pontuacao);
            textprintf_centre_ex(buffer, arcade, 840, 500,  makecol(255,255,255), -1, "%d", scores[8].pontuacao);
            textprintf_centre_ex(buffer, arcade, 840, 540,  makecol(255,255,255), -1, "%d", scores[9].pontuacao);
            textprintf_centre_ex(buffer, arcade, 590, 570,  makecol(255,242,0), -1, "PRESSIONE B PARA RETORNAR");
            draw_sprite(screen, buffer, 0, 0);
            clear(buffer);
            ticks--;
        }
    }

    ///FINALIZAÇÃO
    destroy_bitmap(buffer);
    destroy_bitmap(fundo);
    destroy_bitmap(cursor);
    destroy_bitmap(highscores);
    clear_keybuf();
    allegro_exit();
}
END_OF_FUNCTION(highscores)

void gameover(end_of_game jogo_jogado){             ///DONE
    init();

    int adicionado = 0, posicao;
    int pontuacao_lida, j;
    int i, repeticao=0;
    char nome_lido[12];

    rank_game ranking[12];

    FILE *arq;
    FILE *arq2;

    MIDI* musica = load_midi("sons/imperial.mid");
    play_midi(musica, TRUE);

    FONT* arcade = load_font("verdana.pcx", NULL, NULL);

    ///BITMAPS
    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* fundo = load_bitmap ("Bitmaps/game_over.bmp", NULL);
    BITMAP* cursor = load_bitmap ("Bitmaps/cursor.bmp", NULL);

    if(!(arq = fopen("ranking.txt", "r+")))
        printf("Erro ao abrir o ranking!\n");
    else{
        for(i = 0; i < 10; i ++){
            if(fscanf(arq, "%s %d\n", ranking[i].nome, &ranking[i].pontuacao) == -1){
                printf("Erro ao ler o arquivo!\n");
            }
        }fclose(arq);
    }
    ranking[11].pontuacao = jogo_jogado.game_played.pontuacao;
    strncpy(ranking[11].nome, jogo_jogado.game_played.nome_jogador, jogo_jogado.game_played.tamanho_nome);
    for(i = 0; i < 11; i++){
        for(j = 0; j < 11; j++){
            if((ranking[i].nome[j] < 65 && ranking[i].nome[j] > 90) && (ranking[i].nome[j] < 97 && ranking[i].nome[j] > 122))
                ranking[i].nome[j] = '\0';
        }
    }
    i = 0;
    while(i < 10 && repeticao == 0){
        if(ranking[i].pontuacao < ranking[11].pontuacao){
            for(j = 9; j >= i; j--){
                ranking[j+1].pontuacao = ranking[j].pontuacao;
                memset(ranking[j+1].nome, '\0', 10);
                strncpy(ranking[j+1].nome, ranking[j].nome, 10);
            }
            ranking[i].pontuacao = ranking[11].pontuacao;
            strncpy(ranking[i].nome, ranking[11].nome, 10);
            repeticao = 1;
            adicionado = 1;
            posicao = i;
        }
        i++;
    }
    if(adicionado == 1){
        for(i = jogo_jogado.game_played.tamanho_nome; i < 11; i++)
            ranking[posicao].nome[i] = '\0';
        for(i = 0; i < 11; i++){
            for(j = 0; j < 11; j++){
                if((ranking[i].nome[j] < 65 && ranking[i].nome[j] > 90) && (ranking[i].nome[j] < 97 && ranking[i].nome[j] > 122))
                    ranking[i].nome[j] = '\0';
            }
        }
        if(!(arq2 = fopen("ranking.txt", "w")))
            printf("Erro ao abrir arquivo!\n");
        else{
            for(i = 0; i < 10; i++){
                if(fprintf(arq2, "%s %d\n", ranking[i].nome, ranking[i].pontuacao) == -1)
                    printf("Erro ao escrever no arquivo!\n");
            }fclose(arq2);
        }
    }

    ///GAME LOOP
    while(!exit_program)
    {
        while(ticks > 0 && !exit_program){
            fflush(stdin);

            ///INPUT
            if(key[KEY_B])
                fecha_programa();

            ///DRAW (Desenha na tela)
            draw_sprite(buffer, fundo, 0, 0);
            textprintf_centre_ex(buffer, arcade, 590, 570,  makecol(255,255,10), -1, "PRESS B TO RETURN");
            if(adicionado == 1)
                textprintf_centre_ex(buffer, arcade, 590, 30,  makecol(255,255,10), -1, "Parabens! Voce foi adicionado aos HIGHSCORES do Space Invaders!");
            draw_sprite(screen, buffer, 0, 0);
            clear(buffer);
            ticks--;
        }
    }

    ///FINALIZAÇÃO
    destroy_bitmap(buffer);
    destroy_bitmap(fundo);
    destroy_bitmap(cursor);
    clear_keybuf();
    allegro_exit();
}
END_OF_FUNCTION(gameover)

void tutorial(){                                    ///DONE
    init();
    install_mouse();

    float up_x = 250;
    float up_y = 215;
    float up_v = 0.4;

    float down_x = 540;
    float down_y = 115;
    float down_v = 0.4;

    float naves_x = 780;
    float naves_y = 165;

    float shoot_x = 880;
    float shoot_y = 191;
    float shoot_v = 2;

    MIDI* musica = load_midi("sons/duelfate.mid");
    play_midi(musica, TRUE);

    ///BITMAPS
    BITMAP* cursor = load_bitmap ("Bitmaps/transparente.bmp", NULL);
    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* up = load_bitmap ("Bitmaps/nave.bmp", NULL);
    BITMAP* down = load_bitmap ("Bitmaps/nave.bmp", NULL);
    BITMAP* shoot_nave = load_bitmap ("Bitmaps/nave.bmp", NULL);
    BITMAP* shoot = load_bitmap ("Bitmaps/tiro_jogador.bmp", NULL);
    BITMAP* fundo = load_bitmap ("Bitmaps/fundo_tutorial.bmp", NULL);

    ///LAÇO PRINCIPAL
    while(!exit_program){
        while(ticks > 0 && !exit_program){
            fflush(stdin);

            if(key[KEY_SPACE])
                fecha_programa();

            up_y-=up_v;
            if(up_y<=115)
                up_y=215;

            down_y+=down_v;
            if(down_y>=215)
                down_y=115;

            shoot_x+=shoot_v;
            if(shoot_x>=1100)
                shoot_x=880;

            ///DRAW
            //Star Trek
            draw_sprite(buffer, fundo, 0, 0);
            draw_sprite(buffer, shoot_nave, naves_x, naves_y);
            draw_sprite(buffer, up, up_x, up_y);
            draw_sprite(buffer, down, down_x, down_y);
            draw_sprite(buffer, shoot, shoot_x, shoot_y);
            draw_sprite(buffer, cursor, mouse_x-6, mouse_y);
            draw_sprite(screen, buffer, 0, 0);
            }
    }

    ///FINALIZAÇÃO
    destroy_bitmap(buffer);
    destroy_bitmap(fundo);
    destroy_bitmap(cursor);
    destroy_bitmap(up);
    destroy_bitmap(down);
    destroy_bitmap(shoot);
    destroy_bitmap(shoot_nave);

    clear_keybuf();
    allegro_exit();
}
END_OF_FUNCTION(tutorial)

void selecionar_jogo(char* nome){                   ///DONE
    int i = 0;
    int j;
    int esperar;
    char jogador[11];

    srand(time(NULL));
    init();
    install_mouse();
    FONT* arcade = load_font("verdana.pcx", NULL, NULL);
    MIDI* musica = load_midi("sons/vader.mid");
    play_midi(musica, TRUE);

    ///BITMAPS (Declarar imagens que serão utilizadas)
    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* fundo = load_bitmap ("Bitmaps/selecionar2.bmp", NULL);
    BITMAP* cursor = load_bitmap ("Bitmaps/transparente.bmp", NULL);

    ///GAME LOOP
    while(!exit_program)
    {
        while(ticks > 0 && !exit_program){
        ///INPUT (Detecta entrada do usuário)
        fflush(stdin);
        fflush(stdout);
        if(i < 11){
            if(key[KEY_A]){
                jogador[i] = 'a';
                esperar = 1;
                i++;
            }else
                if(key[KEY_B]){
                    jogador[i] = 'b';
                    esperar = 1;
                    i++;
                }else
                    if(key[KEY_C]){
                        jogador[i] = 'c';
                        esperar = 1;
                        i++;
                    }else
                        if(key[KEY_D]){
                            jogador[i] = 'd';
                            esperar = 1;
                            i++;
                        }else
                            if(key[KEY_E]){
                                jogador[i] = 'e';
                                esperar = 1;
                                i++;
                            }else
                                if(key[KEY_F]){
                                    jogador[i] = 'f';
                                    esperar = 1;
                                    i++;
                                }else
                                    if(key[KEY_G]){
                                        jogador[i] = 'g';
                                        esperar = 1;
                                        i++;
                                    }else
                                        if(key[KEY_H]){
                                            jogador[i] = 'h';
                                            esperar = 1;
                                            i++;
                                        }else
                                            if(key[KEY_I]){
                                                jogador[i] = 'i';
                                                esperar = 1;
                                                i++;
                                            }else
                                                if(key[KEY_J]){
                                                    jogador[i] = 'j';
                                                    esperar = 1;
                                                    i++;
                                                }else
                                                    if(key[KEY_K]){
                                                        jogador[i] = 'k';
                                                        esperar = 1;
                                                        i++;
                                                    }else
                                                        if(key[KEY_L]){
                                                            jogador[i] = 'l';
                                                            esperar = 1;
                                                            i++;
                                                        }else
                                                            if(key[KEY_M]){
                                                                jogador[i] = 'm';
                                                                esperar = 1;
                                                                i++;
                                                            }else
                                                                if(key[KEY_N]){
                                                                    jogador[i] = 'n';
                                                                    esperar = 1;
                                                                    i++;
                                                                }else
                                                                    if(key[KEY_O]){
                                                                        jogador[i] = 'o';
                                                                        esperar = 1;
                                                                        i++;
                                                                    }else
                                                                        if(key[KEY_P]){
                                                                            jogador[i] = 'p';
                                                                            esperar = 1;
                                                                            i++;
                                                                        }else
                                                                            if(key[KEY_Q]){
                                                                                jogador[i] = 'q';
                                                                                esperar = 1;
                                                                                i++;
                                                                            }else
                                                                                if(key[KEY_R]){
                                                                                    jogador[i] = 'r';
                                                                                    esperar = 1;
                                                                                    i++;
                                                                                }else
                                                                                    if(key[KEY_S]){
                                                                                        jogador[i] = 's';
                                                                                        esperar = 1;
                                                                                        i++;
                                                                                    }else
                                                                                        if(key[KEY_T]){
                                                                                            jogador[i] = 't';
                                                                                            esperar = 1;
                                                                                            i++;
                                                                                        }else
                                                                                            if(key[KEY_U]){
                                                                                                jogador[i] = 'u';
                                                                                                esperar = 1;
                                                                                                i++;
                                                                                            }else
                                                                                                if(key[KEY_V]){
                                                                                                    jogador[i] = 'v';
                                                                                                    esperar = 1;
                                                                                                    i++;
                                                                                                }else
                                                                                                    if(key[KEY_W]){
                                                                                                        jogador[i] = 'w';
                                                                                                        esperar = 1;
                                                                                                        i++;
                                                                                                    }else
                                                                                                        if(key[KEY_X]){
                                                                                                            jogador[i] = 'x';
                                                                                                            esperar = 1;
                                                                                                            i++;
                                                                                                        }else
                                                                                                            if(key[KEY_Y]){
                                                                                                                jogador[i] = 'y';
                                                                                                                esperar = 1;
                                                                                                                i++;
                                                                                                            }else
                                                                                                                if(key[KEY_Z]){
                                                                                                                    jogador[i] = 'z';
                                                                                                                    esperar = 1;
                                                                                                                    i++;
                                                                                                                }else
                                                                                                                    if(key[KEY_BACKSPACE]){
                                                                                                                        i--;
                                                                                                                        esperar = 1;
                                                                                                                        jogador[i] = ' ';
                                                                                                                    }else
                                                                                                                        if(key[KEY_ENTER] && jogador[0] != ' '){
                                                                                                                            strncpy(nome, jogador, 10);
                                                                                                                            fecha_programa();
                                                                                                                        }else
                                                                                                                            esperar = 0;

        }
        if(esperar == 1)
            rest(100);

        jogador[0] = toupper(jogador[0]);
        jogador[i] = '\0';

        ///DRAW
        draw_sprite(buffer, fundo, 0, 0);
        draw_sprite(buffer, cursor, mouse_x-6, mouse_y);

        textprintf_ex(buffer, arcade, 500, SCREEN_H/2, makecol(255,255,255), -1, "Digite o nome salvo: %s", jogador);
        textout_centre_ex(buffer, font, nome, SCREEN_W/16, SCREEN_W/2+20, makecol(255, 255, 255), -1);

        draw_sprite(screen, buffer, 0, 0);
        clear(buffer);
        ticks--;
        }
   }

   ///FINALIZAÇÃO
   destroy_bitmap(buffer);
   destroy_bitmap(fundo);
   destroy_bitmap(cursor);
   clear_keybuf();
   allegro_exit();
   return nome;
}
END_OF_FUNCTION(selecionar_jogo)

void naoencontrado(){                               ///DONE
    init();
    install_mouse();
    FONT* arcade = load_font("verdana.pcx", NULL, NULL);

    ///BITMAPS
    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* fundo = load_bitmap ("Bitmaps/naoencontrado.bmp", NULL);
    BITMAP* cursor = load_bitmap ("Bitmaps/transparente.bmp", NULL);
    SAMPLE* erro = load_sample("sons/erro.wav");
    play_sample(erro, 255, 128, 1000, FALSE);

    ///LOOP PRINCIPAL
    while(!exit_program)
    {
        while(ticks > 0 && !exit_program){
        fflush(stdin);

        ///INPUT
        if(key[KEY_B])
            fecha_programa();

        ///DRAW
        draw_sprite(buffer, fundo, 0, 0);
        draw_sprite(buffer, cursor, mouse_x-6, mouse_y);
        textprintf_centre_ex(buffer, arcade, 590, 570,  makecol(255,242,0), -1, "PRESSIONE B PARA RETORNAR");
        draw_sprite(screen, buffer, 0, 0);
        clear(buffer);
        ticks--;
        }
   }

   ///FINALIZAÇÃO
   destroy_bitmap(buffer);
   destroy_bitmap(fundo);
   destroy_bitmap(cursor);
   clear_keybuf();
   allegro_exit();
}
END_OF_FUNCTION(naoencontrado)

novo_jogo carregar_jogo(char* arquivo_carregado){   ///DONE
    FILE *jogos_salvos;
    novo_jogo jogo_salvo;
    int endereco, i = 0, j;
    char nome_do_arquivo[30];

    while(arquivo_carregado[i] != '\0'){
        nome_do_arquivo[i] = arquivo_carregado[i];
        i++;
    }
    j = i;
    nome_do_arquivo[j] = 57;
    i++;
    nome_do_arquivo[i] = '.';
    i++;
    nome_do_arquivo[i] = 'b';
    i++;
    nome_do_arquivo[i] = 'i';
    i++;
    nome_do_arquivo[i] = 'n';
    i++;
    nome_do_arquivo[i] = '\0';

    while(nome_do_arquivo[j] > 48){
        if(!(jogos_salvos = fopen(nome_do_arquivo, "rb"))){
            nome_do_arquivo[j]--;
        }else{
            if(fread(&jogo_salvo, sizeof(novo_jogo), 1, jogos_salvos) == 1)
                nome_do_arquivo[j] = 4;
        }
    }
    if(nome_do_arquivo[j] != 4)
        jogo_salvo.existencia = 0;
    else
        jogo_salvo.existencia = 1;
    fclose(jogos_salvos);
    return jogo_salvo;
}
