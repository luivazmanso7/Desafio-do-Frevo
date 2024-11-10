#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_JOGADORES 100
#define MAX_SIMBOLOS 8

// Estrutura da pilha de salas do labirinto
typedef struct Sala {
    int numeroSala;
    char descricao[200];
    struct Sala *prox;
} Sala;

// Estrutura da fila de eventos
typedef struct Evento {
    char descricao[200];
    struct Evento *prox;
} Evento;

// Estrutura para armazenar as perguntas
typedef struct {
    char pergunta[200];
    char respostaCorreta[50];
    char curiosidade[200];
    char opcaoA[100];
    char opcaoB[100];
    char opcaoC[100];
    char opcaoD[100];
} Questao;

// Estrutura para pontuação
typedef struct {
    char nomeJogador[50];
    int pontuacao;
} Pontuacao;

Pontuacao ranking[MAX_JOGADORES];
int totalJogadores = 0;

// Lista de símbolos diferentes
char *simbolosFrevo[MAX_SIMBOLOS] = {
    "Sombrinha Colorida",
    "Passo de Frevo",
    "Figurino Tradicional",
    "Orquestra de Frevo",
    "Máscara de Carnaval",
    "Abadá",
    "Estandarte",
    "Chapéu de Palha"
};

int indiceSimboloAtual = 0; // Para controlar qual símbolo será dado ao jogador

// Funções de manipulação da pilha de salas

// Empilha uma sala no labirinto
void empilharSala(Sala **topo, int numeroSala, const char *descricao) {
    Sala *novaSala = (Sala *)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        fprintf(stderr, "Erro de alocação de memória para a sala.\n");
        exit(EXIT_FAILURE);
    }
    novaSala->numeroSala = numeroSala;
    strcpy(novaSala->descricao, descricao);
    novaSala->prox = *topo;
    *topo = novaSala;
}

// Desempilha a sala atual
void desempilharSala(Sala **topo) {
    if (*topo == NULL) return;
    Sala *temp = *topo;
    *topo = (*topo)->prox;
    free(temp);
}

// Libera a memória alocada para a pilha de salas
void liberarPilhaSalas(Sala **topo) {
    while (*topo != NULL) {
        desempilharSala(topo);
    }
}

// Retorna o tamanho da pilha de salas
int tamanhoPilhaSalas(Sala *topo) {
    int count = 0;
    while (topo != NULL) {
        count++;
        topo = topo->prox;
    }
    return count;
}

// Exibe o caminho percorrido pelo jogador
void exibirCaminhoPercorrido(Sala *topo) {
    printf("\nCaminho percorrido no labirinto:\n");
    Sala *atual = topo;
    while (atual != NULL) {
        printf("Sala %d: %s\n", atual->numeroSala, atual->descricao);
        atual = atual->prox;
    }
}

// Funções de manipulação da fila de eventos

// Enfileira um evento
void enfileirarEvento(Evento **head, Evento **tail, const char *descricao) {
    Evento *novoEvento = (Evento *)malloc(sizeof(Evento));
    if (novoEvento == NULL) {
        fprintf(stderr, "Erro de alocação de memória para o evento.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(novoEvento->descricao, descricao);
    novoEvento->prox = NULL;

    if (*tail != NULL) (*tail)->prox = novoEvento;
    *tail = novoEvento;
    if (*head == NULL) *head = novoEvento;
}

// Desenfileira um evento
Evento* desenfileirarEvento(Evento **head) {
    if (*head == NULL) return NULL;
    Evento *temp = *head;
    *head = (*head)->prox;
    return temp;
}

// Libera a memória alocada para a fila de eventos
void liberarFilaEventos(Evento **head) {
    while (*head != NULL) {
        Evento *temp = *head;
        *head = (*head)->prox;
        free(temp);
    }
}

// Funções relacionadas ao ranking

// Adiciona a pontuação do jogador ao ranking
void adicionar_pontuacao(const char *nome, int pontos) {
    if (totalJogadores < MAX_JOGADORES) {
        strcpy(ranking[totalJogadores].nomeJogador, nome);
        ranking[totalJogadores].pontuacao = pontos;
        totalJogadores++;
    } else {
        printf("Ranking cheio!\n");
    }
}

// Algoritmo Bubble Sort para ordenar as pontuações
void ordenar_pontuacoes() {
    for (int i = 0; i < totalJogadores - 1; i++) {
        for (int j = 0; j < totalJogadores - i - 1; j++) {
            if (ranking[j].pontuacao < ranking[j + 1].pontuacao) {
                Pontuacao temp = ranking[j];
                ranking[j] = ranking[j + 1];
                ranking[j + 1] = temp;
            }
        }
    }
}

// Exibe o ranking dos jogadores
void exibir_ranking() {
    ordenar_pontuacoes(); // Utiliza Bubble Sort para ordenar o ranking
    printf("\n--- Ranking dos Jogadores ---\n");
    for (int i = 0; i < totalJogadores; i++) {
        printf("%d. %s - %d pontos\n", i + 1, ranking[i].nomeJogador, ranking[i].pontuacao);
    }
}


// Exibe o menu principal e retorna a opção escolhida
int mostrar_menu() {
    int opcao;
    printf("\n--- Labirinto do Frevo ---\n");
    printf("1. Iniciar Jogo\n");
    printf("2. Ver Ranking\n");
    printf("3. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);
    return opcao;
}

// Compara a resposta do jogador com a resposta correta
int comparar_respostas(const char *respostaJogador, const char *respostaCorreta) {
    char rJogador = tolower(respostaJogador[0]);
    char rCorreta = tolower(respostaCorreta[0]);
    return rJogador == rCorreta;
}

void iniciar_jogo() {
    Sala *topoSala = NULL;
    Evento *headEvento = NULL, *tailEvento = NULL;
    char nomeJogador[50];
    int pontuacao = 0;
    int numeroSalaAtual = 0;
    indiceSimboloAtual = 0;

    printf("\nDigite o seu nome: ");
    scanf("%s", nomeJogador);

   
    empilharSala(&topoSala, ++numeroSalaAtual, "Entrada do Labirinto do Paco do Frevo");

    
    enfileirarEvento(&headEvento, &tailEvento, "Você encontrou um mestre do frevo que oferece uma dica.");
    enfileirarEvento(&headEvento, &tailEvento, "Um desafio musical aparece em seu caminho.");
    enfileirarEvento(&headEvento, &tailEvento, "Você tropeçou em um obstáculo.");
    

   
    Questao questoes[] = {
        {
            "Qual é o instrumento principal no frevo?",
            "a",
            "O trombone é um dos instrumentos principais nas orquestras de frevo.",
            "a) Trombone",
            "b) Violino",
            "c) Flauta",
            "d) Piano"
        },
        {
            "Em que cidade o frevo se originou?",
            "b",
            "O frevo se originou na cidade do Recife, Pernambuco.",
            "a) Olinda",
            "b) Recife",
            "c) Salvador",
            "d) Rio de Janeiro"
        },
        {
            "O que significa a palavra 'frevo'?",
            "c",
            "A palavra 'frevo' deriva de 'ferver', referindo-se à energia da dança.",
            "a) Alegria",
            "b) Dança",
            "c) Ferver",
            "d) Música"
        },
        // Adicione mais perguntas conforme necessário
    };

    int numQuestoes = sizeof(questoes) / sizeof(questoes[0]);
    int indicePergunta = 0;

    // Embaralha as perguntas
    srand(time(NULL));
    for (int i = numQuestoes - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Questao temp = questoes[i];
        questoes[i] = questoes[j];
        questoes[j] = temp;
    }

    // Loop principal do jogo
    while (topoSala != NULL) {
        printf("\nVocê está na sala %d: %s\n", topoSala->numeroSala, topoSala->descricao);

        // Processa eventos
        Evento *eventoAtual = desenfileirarEvento(&headEvento);
        if (eventoAtual != NULL) {
            printf("Evento: %s\n", eventoAtual->descricao);
            // Lógica para lidar com o evento
            // Eventos podem afetar o jogo de outras maneiras
            free(eventoAtual);
        }

        // Apresenta pergunta ao jogador
        if (indicePergunta < numQuestoes) {
            Questao *perguntaAtual = &questoes[indicePergunta++];
            char respostaJogador[10];

            printf("\nPergunta:\n%s\n", perguntaAtual->pergunta);
            printf("%s\n", perguntaAtual->opcaoA);
            printf("%s\n", perguntaAtual->opcaoB);
            printf("%s\n", perguntaAtual->opcaoC);
            printf("%s\n", perguntaAtual->opcaoD);
            printf("Sua resposta (a, b, c ou d): ");
            scanf(" %s", respostaJogador);

            if (comparar_respostas(respostaJogador, perguntaAtual->respostaCorreta)) {
                printf("Resposta correta!\n");
                printf("Curiosidade: %s\n", perguntaAtual->curiosidade);
                pontuacao += 10;

                // Coleta um símbolo do frevo e exibe ao jogador
                if (indiceSimboloAtual < MAX_SIMBOLOS) {
                    printf("Você coletou o símbolo: %s!\n", simbolosFrevo[indiceSimboloAtual]);
                    indiceSimboloAtual++;
                }

                // Verifica se o jogador coletou todos os símbolos
                if (indiceSimboloAtual >= MAX_SIMBOLOS) {
                    printf("\nParabéns, %s! Você coletou todos os símbolos e chegou ao final do labirinto!\n", nomeJogador);
                    break;
                }

                // Avança para a próxima sala
                empilharSala(&topoSala, ++numeroSalaAtual, "Sala seguinte no Labirinto do Frevo");
            } else {
                printf("Resposta incorreta.\n");
                // Retrocede uma sala
                desempilharSala(&topoSala);
                if (topoSala == NULL) {
                    printf("Você saiu do labirinto. Fim de jogo.\n");
                    break;
                }
            }
        } else {
            printf("\nNão há mais perguntas disponíveis.\n");
            break;
        }
    }

    printf("\nSua pontuação final foi: %d pontos.\n", pontuacao);
    exibirCaminhoPercorrido(topoSala);

    // Adiciona a pontuação ao ranking
    adicionar_pontuacao(nomeJogador, pontuacao);

    // Libera memória
    liberarPilhaSalas(&topoSala);
    liberarFilaEventos(&headEvento);
}

// Finaliza o jogo e exibe o ranking
void finalizar_jogo() {
    printf("\nObrigado por jogar!\n");
    exibir_ranking();
}

int main() {
    int opcao;
    do {
        opcao = mostrar_menu();
        switch (opcao) {
            case 1:
                iniciar_jogo();
                break;
            case 2:
                exibir_ranking();
                break;
            case 3:
                finalizar_jogo();
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 3);

    return 0;
}