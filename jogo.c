#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>

#define MAX_JOGADORES 100
#define MAX_SIMBOLOS 8
#define MAX_SALAS 8

// Estrutura para armazenar as perguntas
typedef struct {
    char pergunta[300];
    char respostaCorreta[50];
    char curiosidade[400];
    char opcaoA[150];
    char opcaoB[150];
    char opcaoC[150];
    char opcaoD[150];
} Questao;

// Estrutura da sala do labirinto
typedef struct Sala {
    int numeroSala;
    char descricao[200];
    Questao *questoes;
    int numQuestoes;
    int *perguntasUsadas; // Array para controlar perguntas já usadas na sala
} Sala;

// Estrutura do nó da pilha de salas
typedef struct StackNode {
    Sala *sala;
    struct StackNode *prox;
} StackNode;

// Estrutura da fila de eventos
typedef struct Evento {
    char descricao[200];
    struct Evento *prox;
} Evento;

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

// Lista de nomes das salas
char *nomesDasSalas[MAX_SALAS] = {
    "Entrada do Paço do Frevo",            // Sala 1 (Fácil)
    "Galeria das Sombrinhas",              // Sala 2 (Fácil)
    "Salão dos Passistas",                 // Sala 3 (Médio)
    "Hall dos Mestres do Frevo",           // Sala 4 (Médio)
    "Terraço das Orquestras",              // Sala 5 (Difícil)
    "Exposição dos Estandartes",           // Sala 6 (Difícil)
    "Camarote das Máscaras",               // Sala 7 (Muito Difícil)
    "Saída para o Carnaval de Recife"      // Sala 8 (Muito Difícil)
};

int indiceSalaAtual = 0; // Para controlar qual sala será apresentada ao jogador

// Funções de manipulação da pilha de salas do labirinto

// Empilha uma sala no labirinto
void empilharSala(StackNode **topo, Sala *sala) {
    StackNode *novoNode = (StackNode *)malloc(sizeof(StackNode));
    if (novoNode == NULL) {
        fprintf(stderr, "Erro de alocação de memória para o StackNode.\n");
        exit(EXIT_FAILURE);
    }
    novoNode->sala = sala;
    novoNode->prox = *topo;
    *topo = novoNode;
}

// Desempilha a sala atual
void desempilharSala(StackNode **topo) {
    if (*topo == NULL) return;
    StackNode *temp = *topo;
    *topo = (*topo)->prox;
    free(temp);
}

// Libera a memória alocada para a pilha de salas
void liberarPilhaSalas(StackNode **topo) {
    while (*topo != NULL) {
        desempilharSala(topo);
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
    char entrada[10];
    do {
        printf("\n--- Desafio do Frevo ---\n");
        printf("1. Iniciar Jogo\n");
        printf("2. Ver Ranking\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");
        fgets(entrada, sizeof(entrada), stdin);
        // Remove o '\n' do final da string
        entrada[strcspn(entrada, "\n")] = '\0';
        if (sscanf(entrada, "%d", &opcao) != 1) {
            printf("Entrada inválida. Por favor, digite um número correspondente à opção.\n");
            opcao = 0; // Define uma opção inválida para repetir o loop
        } else if (opcao < 1 || opcao > 3) {
            printf("Opção inválida. Por favor, escolha entre 1 e 3.\n");
        }
    } while (opcao < 1 || opcao > 3);
    return opcao;
}

// Compara a resposta do jogador com a resposta correta
int comparar_respostas(const char *respostaJogador, const char *respostaCorreta) {
    char rJogador = tolower(respostaJogador[0]);
    char rCorreta = tolower(respostaCorreta[0]);
    return rJogador == rCorreta;
}

// Função para selecionar uma pergunta que ainda não foi usada na sala
int selecionarQuestao(Questao questoes[], int numQuestoes, int perguntasUsadas[]) {
    int indiceQuestao;
    int tentativas = 0;
    int todasUsadas = 1;

    // Verifica se todas as perguntas foram usadas
    for (int i = 0; i < numQuestoes; i++) {
        if (perguntasUsadas[i] == 0) {
            todasUsadas = 0;
            break;
        }
    }

    if (todasUsadas) {
        return -1; // Indica que todas as perguntas foram usadas
    }

    do {
        indiceQuestao = rand() % numQuestoes;
        tentativas++;
    } while (perguntasUsadas[indiceQuestao]);

    perguntasUsadas[indiceQuestao] = 1;
    return indiceQuestao;
}

// Inicia o jogo
void iniciar_jogo() {
    StackNode *topoSala = NULL;
    Evento *headEvento = NULL, *tailEvento = NULL;
    char nomeJogador[50];
    int pontuacao = 0;
    int vidas = 3;
    indiceSimboloAtual = 0;
    indiceSalaAtual = 0;

    // Array para armazenar símbolos coletados
    char *simbolosColetados[MAX_SIMBOLOS];
    int totalSimbolosColetados = 0;

    printf("\nDigite o seu nome: ");
    fgets(nomeJogador, sizeof(nomeJogador), stdin);
    // Remove o '\n' do final da string
    nomeJogador[strcspn(nomeJogador, "\n")] = '\0';

    // Embaralha o gerador de números aleatórios
    srand(time(NULL));

    // Defina as perguntas para cada sala com dificuldade crescente

    // Sala 1: Entrada do Paço do Frevo (Fácil)
    Questao questoesSala1[] = {
        {
            "Qual cidade é conhecida como o berço do frevo?",
            "b",
            "Recife é considerada o berço do frevo, onde a dança e o ritmo surgiram e se desenvolveram.",
            "a) Salvador",
            "b) Recife",
            "c) Rio de Janeiro",
            "d) São Paulo"
        },
        {
            "O que é o frevo?",
            "a",
            "O frevo é um ritmo musical e dança tradicional de Pernambuco, conhecido por sua energia e agilidade.",
            "a) Um ritmo musical e dança",
            "b) Uma comida típica",
            "c) Um tipo de vestimenta",
            "d) Uma celebração religiosa"
        },
        {
            "Qual instrumento é frequentemente usado no frevo?",
            "c",
            "Os metais, como o trombone, são característicos no frevo, dando-lhe um som vibrante.",
            "a) Violino",
            "b) Piano",
            "c) Trombone",
            "d) Flauta"
        }
    };
    int numQuestoesSala1 = sizeof(questoesSala1) / sizeof(questoesSala1[0]);

    // Sala 2: Galeria das Sombrinhas (Fácil)
    Questao questoesSala2[] = {
        {
            "Qual objeto é um símbolo do frevo?",
            "a",
            "A sombrinha colorida é um símbolo icônico do frevo, usada pelos dançarinos em suas performances.",
            "a) Sombrinha colorida",
            "b) Pandeiro",
            "c) Berimbau",
            "d) Violão"
        },
        {
            "As sombrinhas do frevo geralmente têm quais cores?",
            "d",
            "As sombrinhas têm cores vibrantes e multicoloridas, representando a alegria do frevo.",
            "a) Preto e branco",
            "b) Azul e branco",
            "c) Verde e amarelo",
            "d) Multicoloridas"
        },
        {
            "Para que serve a sombrinha no frevo?",
            "b",
            "A sombrinha é usada como adereço nas coreografias, adicionando graça e complexidade aos movimentos.",
            "a) Proteger do sol",
            "b) Adereço na dança",
            "c) Instrumento musical",
            "d) Marcações no chão"
        }
    };
    int numQuestoesSala2 = sizeof(questoesSala2) / sizeof(questoesSala2[0]);

    // [Definição das demais salas permanece igual]

    // Atualize os arrays com as perguntas por sala
    Questao *questoesPorSala[MAX_SALAS] = {
        questoesSala1,
        questoesSala2,
        // [Adicionar as demais salas]
    };
    int numQuestoesPorSala[MAX_SALAS] = {
        numQuestoesSala1,
        numQuestoesSala2,
        // [Adicionar as quantidades de questões das demais salas]
    };

    // Cria todas as salas previamente e armazena em um array
    Sala salas[MAX_SALAS];
    for (int i = 0; i < MAX_SALAS; i++) {
        salas[i].numeroSala = i + 1;
        strcpy(salas[i].descricao, nomesDasSalas[i]);
        salas[i].questoes = questoesPorSala[i];
        salas[i].numQuestoes = numQuestoesPorSala[i];
        // Inicializa o array perguntasUsadas
        salas[i].perguntasUsadas = (int *)calloc(salas[i].numQuestoes, sizeof(int));
        if (salas[i].perguntasUsadas == NULL) {
            fprintf(stderr, "Erro de alocação de memória para perguntasUsadas da sala %d.\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }

    // Empilha a sala inicial
    empilharSala(&topoSala, &salas[indiceSalaAtual]);

    // Enfileira alguns eventos
    enfileirarEvento(&headEvento, &tailEvento, "Você encontrou um mestre que lhe ensinou um novo passo!");
    enfileirarEvento(&headEvento, &tailEvento, "Uma multidão animada o empurra para frente.");
    enfileirarEvento(&headEvento, &tailEvento, "Você parou para apreciar uma apresentação.");
    enfileirarEvento(&headEvento, &tailEvento, "Uma chuva repentina molha o chão, tenha cuidado!");

    // Loop principal do jogo
    while (topoSala != NULL) {
        Sala *salaAtual = topoSala->sala;
        printf("\nVocê está na sala %d: %s\n", salaAtual->numeroSala, salaAtual->descricao);

        // Processa eventos
        Evento *eventoAtual = desenfileirarEvento(&headEvento);
        if (eventoAtual != NULL) {
            printf("Evento: %s\n", eventoAtual->descricao);
            // Lógica para lidar com o evento (pode ser expandida conforme necessário)
            free(eventoAtual);
        }

        // Seleciona uma pergunta da sala atual usando o perguntasUsadas da sala
        int indicePergunta = selecionarQuestao(salaAtual->questoes, salaAtual->numQuestoes, salaAtual->perguntasUsadas);

        if (indicePergunta == -1) {
            printf("Você já respondeu todas as perguntas desta sala.\n");
            printf("Avançando automaticamente para a próxima sala.\n");
            // Avança para a próxima sala
            indiceSalaAtual++; // Incrementa para apontar para a próxima sala
            if (indiceSalaAtual >= MAX_SALAS) {
                printf("\nParabéns, %s! Você chegou ao final do labirinto!\n", nomeJogador);
                printf("Parabéns, %s! Você recebeu o título de Aprendiz do Frevo!\n", nomeJogador);
                break;
            }
            empilharSala(&topoSala, &salas[indiceSalaAtual]);
            continue; // Volta ao início do loop
        }

        Questao *perguntaAtual = &salaAtual->questoes[indicePergunta];
        char respostaJogador[10];

        printf("\nPergunta:\n%s\n", perguntaAtual->pergunta);
        printf("%s\n", perguntaAtual->opcaoA);
        printf("%s\n", perguntaAtual->opcaoB);
        printf("%s\n", perguntaAtual->opcaoC);
        printf("%s\n", perguntaAtual->opcaoD);
        printf("Sua resposta (a, b, c ou d): ");
        fgets(respostaJogador, sizeof(respostaJogador), stdin);
        respostaJogador[strcspn(respostaJogador, "\n")] = '\0';

        if (comparar_respostas(respostaJogador, perguntaAtual->respostaCorreta)) {
            printf("Resposta correta!\n");
            printf("Curiosidade: %s\n", perguntaAtual->curiosidade);
            printf("Vidas restantes: %d\n", vidas);
            pontuacao += 10;

            // Coleta um símbolo do frevo e exibe ao jogador
            if (indiceSimboloAtual < MAX_SIMBOLOS) {
                printf("Você coletou o símbolo: %s!\n", simbolosFrevo[indiceSimboloAtual]);
                simbolosColetados[totalSimbolosColetados] = simbolosFrevo[indiceSimboloAtual];
                totalSimbolosColetados++;
                indiceSimboloAtual++;
            }

            // Verifica se o jogador chegou à última sala
            indiceSalaAtual++; // Incrementa para apontar para a próxima sala
            if (indiceSalaAtual >= MAX_SALAS) {
                printf("\nParabéns, %s! Você chegou ao final do labirinto!\n", nomeJogador);
                printf("Parabéns, %s! Você recebeu o título de Aprendiz do Frevo!\n", nomeJogador);
                break;
            }

            // Avança para a próxima sala
            empilharSala(&topoSala, &salas[indiceSalaAtual]);

        } else {
            printf("Resposta incorreta.\n");
            vidas--;
            // Retrocede uma sala
            desempilharSala(&topoSala);
            if (vidas == 0) {
                printf("Suas vidas acabaram. Fim de jogo.\n");
                break;
            } else if (topoSala == NULL) {
                printf("Você saiu do labirinto. Fim de jogo.\n");
                break;
            }
            printf("Vidas restantes: %d\n", vidas);
            indiceSalaAtual--; // Decrementa o índice da sala
        }
    }

    printf("\nSua pontuação final foi: %d pontos.\n", pontuacao);

    // Exibe os símbolos coletados
    printf("\nSímbolos coletados:\n");
    if (totalSimbolosColetados > 0) {
        for (int i = 0; i < totalSimbolosColetados; i++) {
            printf("- %s\n", simbolosColetados[i]);
        }
    } else {
        printf("Nenhum símbolo foi coletado.\n");
    }

    // Adiciona a pontuação ao ranking
    adicionar_pontuacao(nomeJogador, pontuacao);

    // Libera memória
    liberarPilhaSalas(&topoSala);
    liberarFilaEventos(&headEvento);

    // Libera o array perguntasUsadas de cada sala
    for (int i = 0; i < MAX_SALAS; i++) {
        free(salas[i].perguntasUsadas);
    }
}

// Finaliza o jogo e exibe o ranking
void finalizar_jogo() {
    printf("\nObrigado por jogar!\n");
    exibir_ranking();
}

int main() {
    setlocale(LC_ALL, ""); // Configura a localidade para suportar caracteres especiais
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
