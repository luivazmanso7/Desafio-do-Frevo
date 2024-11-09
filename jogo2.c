#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura da pilha (símbolos coletados)
typedef struct PilhaNode {
    char simbolo[50];
    struct PilhaNode *prox;
} PilhaNode;

// Estrutura da fila (perguntas)
typedef struct FilaNode {
    char pergunta[100];
    int dificuldade; // 1 para fácil, 2 para médio, 3 para difícil
    struct FilaNode *prox;
} FilaNode;

// Funções de manipulação da pilha
void push(PilhaNode **topo, const char *simbolo) {
    PilhaNode *novo = (PilhaNode *)malloc(sizeof(PilhaNode));
    if (novo == NULL) {
        fprintf(stderr, "Erro de alocação de memória para a pilha.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(novo->simbolo, simbolo);
    novo->prox = *topo;
    *topo = novo;
}

char* pop(PilhaNode **topo) {
    if (*topo == NULL) return NULL;

    PilhaNode *aux = *topo;
    char *simbolo = strdup(aux->simbolo);
    *topo = aux->prox;
    free(aux);
    return simbolo;
}

int tamanhoPilha(PilhaNode *topo) {
    int cont = 0;
    while (topo != NULL) {
        cont++;
        topo = topo->prox;
    }
    return cont;
}

void printPilha(PilhaNode *topo) {
    printf("Símbolos coletados: ");
    while (topo != NULL) {
        printf("%s -> ", topo->simbolo);
        topo = topo->prox;
    }
    printf("NULL\n");
}

// Funções de manipulação da fila
void enqueue(FilaNode **head, FilaNode **tail, const char *pergunta, int dificuldade) {
    FilaNode *novo = (FilaNode *)malloc(sizeof(FilaNode));
    if (novo == NULL) {
        fprintf(stderr, "Erro de alocação de memória para a fila.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(novo->pergunta, pergunta);
    novo->dificuldade = dificuldade;
    novo->prox = NULL;

    if (*tail != NULL) (*tail)->prox = novo;
    *tail = novo;
    if (*head == NULL) *head = novo;
}

FilaNode* dequeue(FilaNode **head, FilaNode **tail) {
    if (*head == NULL) return NULL;

    FilaNode *aux = *head;
    *head = aux->prox;
    if (*head == NULL) *tail = NULL;
    return aux;  // Retorna o nó inteiro para acessar pergunta e dificuldade
}

void printFila(FilaNode *head) {
    printf("Perguntas na fila: ");
    while (head != NULL) {
        printf("\"%s\" (Dificuldade: %d) -> ", head->pergunta, head->dificuldade);
        head = head->prox;
    }
    printf("NULL\n");
}

// Funções do jogo
void inicializarPerguntas(FilaNode **head, FilaNode **tail) {
    enqueue(head, tail, "Qual é a cor tradicional da sombrinha de frevo?", 1);
    enqueue(head, tail, "Em que cidade fica o Paço do Frevo?", 1);
    enqueue(head, tail, "Qual instrumento é representativo no frevo?", 2);
    enqueue(head, tail, "Quantas cores tem o estandarte do frevo?", 2);
    enqueue(head, tail, "Quem é o patrono do frevo?", 3);
}

void mostrarPontuacao(int pontos1, int pontos2) {
    printf("\nPontuação atual:\n");
    printf("Jogador 1: %d pontos\n", pontos1);
    printf("Jogador 2: %d pontos\n", pontos2);
}

void turnoJogador(int jogador, PilhaNode **pilhaJogador, int *pontuacao, int dificuldade) {
    char resposta[50];
    int pontosGanhos = dificuldade;  // Mais pontos para perguntas mais difíceis
    printf("Digite a resposta: ");
    scanf(" %[^\n]s", resposta);

    if (strcmp(resposta, "correta") == 0) {
        printf("Resposta correta!\n");
        push(pilhaJogador, "Símbolo Coletado");
        printPilha(*pilhaJogador);
        *pontuacao += pontosGanhos;
    } else {
        printf("Resposta incorreta!\n");
    }
}

int verificarVitoria(PilhaNode *pilhaJogador, int jogador) {
    if (tamanhoPilha(pilhaJogador) >= 3) {
        printf("Jogador %d venceu!\n", jogador);
        return 1;
    }
    return 0;
}

// Menu inicial para selecionar nível de dificuldade
int selecionarNivel() {
    int nivel;
    printf("Selecione o nível de dificuldade:\n");
    printf("1. Fácil\n2. Médio\n3. Difícil\nEscolha uma opção: ");
    scanf("%d", &nivel);
    return nivel;
}

// Função principal do jogo
int main() {
    PilhaNode *jogador1 = NULL;
    PilhaNode *jogador2 = NULL;
    FilaNode *headPerguntas = NULL;
    FilaNode *tailPerguntas = NULL;

    inicializarPerguntas(&headPerguntas, &tailPerguntas);

    int turno = 1;
    int pontuacao1 = 0;
    int pontuacao2 = 0;

    int nivel = selecionarNivel();

    while (headPerguntas != NULL) {
        printf("\nTurno do Jogador %d\n", turno);
        FilaNode *perguntaNode = dequeue(&headPerguntas, &tailPerguntas);
        if (perguntaNode == NULL) break;

        // Exibe apenas perguntas com dificuldade adequada ao nível selecionado
        if (perguntaNode->dificuldade <= nivel) {
            printf("Pergunta: %s (Dificuldade: %d)\n", perguntaNode->pergunta, perguntaNode->dificuldade);

            if (turno == 1) {
                turnoJogador(1, &jogador1, &pontuacao1, perguntaNode->dificuldade);
                if (verificarVitoria(jogador1, 1)) break;
            } else {
                turnoJogador(2, &jogador2, &pontuacao2, perguntaNode->dificuldade);
                if (verificarVitoria(jogador2, 2)) break;
            }
        }
        free(perguntaNode); // Libera a pergunta após uso

        mostrarPontuacao(pontuacao1, pontuacao2);

        // Alterna o turno
        turno = (turno == 1) ? 2 : 1;
    }

    // Exibe a pontuação final
    printf("\nPontuação Final:\n");
    printf("Jogador 1: %d pontos\n", pontuacao1);
    printf("Jogador 2: %d pontos\n", pontuacao2);

    if (pontuacao1 > pontuacao2) {
        printf("Jogador 1 é o vencedor!\n");
    } else if (pontuacao2 > pontuacao1) {
        printf("Jogador 2 é o vencedor!\n");
    } else {
        printf("O jogo terminou em empate!\n");
    }

    // Libera memória restante
    while (jogador1) pop(&jogador1);
    while (jogador2) pop(&jogador2);
    while (headPerguntas) {
        FilaNode *temp = headPerguntas;
        headPerguntas = headPerguntas->prox;
        free(temp);
    }

    return 0;
}
