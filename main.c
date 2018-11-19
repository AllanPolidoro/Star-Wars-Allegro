#include <stdio.h>
#include "spaceinvaders.h"

novo_jogo carregar_jogo(char*);

int main()
{
    int operacao, opcao, tipo, comando, i = 0, tamanho_nome;
    end_of_game jogo_jogado;
    novo_jogo game;
    char nome[11];
    char arquivo_carregado[30];
    nome[0] = '\0';
    while(operacao != 4){
        operacao = menu();
        if(operacao == 1){
            tipo = 0;
            game.tamanho_nome = ler_nome(nome);
            tutorial();
            jogo_jogado = jogo(nome, tipo, game);
            if(jogo_jogado.comando == 1)
                gameover(jogo_jogado);
        }else
            if(operacao == 2){
                tipo = 1;
                selecionar_jogo(arquivo_carregado);
                game = carregar_jogo(arquivo_carregado);
                if(game.existencia == 1){
                    jogo_jogado = jogo(nome, tipo, game);
                    if(jogo_jogado.comando == 1)
                        gameover(jogo_jogado);
                }else
                    naoencontrado();
            }else
                if(operacao == 3){
                    highscores();
                }else
                    if(operacao == 4){
                        printf("THANKS FOR PLAYING!\n");
                    }
    }

    return 0;
}
