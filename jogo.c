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
    struct FilaNode *prox;
} FilaNode;

// Funções para manipulação da pilha
void push(PilhaNode **topo, char *simbolo) {
    PilhaNode *novo = (PilhaNode *)malloc(sizeof(PilhaNode));
    if (novo != NULL) {
        strcpy(novo->simbolo, simbolo);
        novo->prox = *topo;
        *topo = novo;
    }
}

char* pop(PilhaNode **topo) {
    if (*topo != NULL) {
        PilhaNode *aux = *topo;
        char *simbolo = aux->simbolo;
        *topo = aux->prox;
        free(aux);
        return simbolo;
    }
    return NULL;
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

// Funções para manipulação da fila
void enqueue(FilaNode **head, FilaNode **tail, char *pergunta) {
    FilaNode *novo = (FilaNode *)malloc(sizeof(FilaNode));
    if (novo != NULL) {
        strcpy(novo->pergunta, pergunta);
        novo->prox = NULL;
        if (*tail != NULL) {
            (*tail)->prox = novo;
        }
        *tail = novo;
        if (*head == NULL) {
            *head = novo;
        }
    }
}

char* dequeue(FilaNode **head, FilaNode **tail) {
    if (*head != NULL) {
        FilaNode *aux = *head;
        char *pergunta = aux->pergunta;
        *head = aux->prox;
        if (*head == NULL) {
            *tail = NULL;
        }
        free(aux);
        return pergunta;
    }
    return NULL;
}

void printFila(FilaNode *head) {
    printf("Perguntas na fila: ");
    while (head != NULL) {
        printf("\"%s\" -> ", head->pergunta);
        head = head->prox;
    }
    printf("NULL\n");
}

// Função principal do jogo
int main() {
    PilhaNode *jogador1 = NULL;
    PilhaNode *jogador2 = NULL;
    FilaNode *headPerguntas = NULL;
    FilaNode *tailPerguntas = NULL;

    // Adicionando perguntas à fila
    enqueue(&headPerguntas, &tailPerguntas, "Qual é a cor tradicional da sombrinha de frevo?");
    enqueue(&headPerguntas, &tailPerguntas, "Em que cidade fica o Paço do Frevo?");
    enqueue(&headPerguntas, &tailPerguntas, "Qual instrumento é representativo no frevo?");
    enqueue(&headPerguntas, &tailPerguntas, "Quantas cores tem o estandarte do frevo?");
    enqueue(&headPerguntas, &tailPerguntas, "Quem é o patrono do frevo?");

    int turno = 1; // 1 para jogador 1, 2 para jogador 2
    char resposta[50];

    // Loop principal do jogo
    while (headPerguntas != NULL) {
        printf("\nTurno do Jogador %d\n", turno);
        char *pergunta = dequeue(&headPerguntas, &tailPerguntas);
        printf("Pergunta: %s\n", pergunta);
        printf("Digite a resposta: ");
        scanf(" %[^\n]s", resposta); // Lê a resposta do usuário

        // Lógica de verificação de resposta (simplificada para exemplo)
        if (strcmp(resposta, "correta") == 0) {
            printf("Resposta correta!\n");
            if (turno == 1) {
                push(&jogador1, "Símbolo Coletado");
                printPilha(jogador1);
                if (tamanhoPilha(jogador1) == 3) {
                    printf("Jogador 1 venceu!\n");
                    break;
                }
            } else {
                push(&jogador2, "Símbolo Coletado");
                printPilha(jogador2);
                if (tamanhoPilha(jogador2) == 3) {
                    printf("Jogador 2 venceu!\n");
                    break;
                }
            }
        } else {
            printf("Resposta incorreta!\n");
        }

        // Alterna o turno
        turno = (turno == 1) ? 2 : 1;
    }

    return 0;
}