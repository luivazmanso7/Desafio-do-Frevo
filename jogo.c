#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PilhaNode {
    char simbolo[50];
    struct PilhaNode *prox;
} PilhaNode;

typedef struct FilaNode {
    char pergunta[100];
    struct FilaNode *prox;
} FilaNode;

void inicializarPerguntas(FilaNode **head, FilaNode **tail);
void iniciarJogo(PilhaNode **jogador1, PilhaNode **jogador2, FilaNode **headPerguntas, FilaNode **tailPerguntas);
void jogarTurno(PilhaNode **jogador, FilaNode **headPerguntas, FilaNode **tailPerguntas);
void mostrarSimbolos(PilhaNode *jogador);
void ordenarSimbolos(PilhaNode *jogador);
void exibirMenu();

void push(PilhaNode **topo, char *simbolo);
char* pop(PilhaNode **topo);
int tamanhoPilha(PilhaNode *topo);
void enqueue(FilaNode **head, FilaNode **tail, char *pergunta);
char* dequeue(FilaNode **head, FilaNode **tail);

int main() {
    PilhaNode *jogador1 = NULL;
    PilhaNode *jogador2 = NULL;
    FilaNode *headPerguntas = NULL;
    FilaNode *tailPerguntas = NULL;

    int opcao;
    do {
        exibirMenu();
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                inicializarPerguntas(&headPerguntas, &tailPerguntas);
                iniciarJogo(&jogador1, &jogador2, &headPerguntas, &tailPerguntas);
                break;
            case 2:
                printf("Regras do jogo: Responda corretamente para coletar símbolos do Frevo!\n");
                break;
            case 3:
                printf("Saindo do jogo...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 3);

    return 0;
}

void inicializarPerguntas(FilaNode **head, FilaNode **tail) {
    enqueue(head, tail, "Qual é a cor tradicional da sombrinha de frevo?");
    enqueue(head, tail, "Em que cidade fica o Paço do Frevo?");
    enqueue(head, tail, "Qual instrumento é representativo no frevo?");
    enqueue(head, tail, "Quantas cores tem o estandarte do frevo?");
    enqueue(head, tail, "Quem é o patrono do frevo?");
}

void iniciarJogo(PilhaNode **jogador1, PilhaNode **jogador2, FilaNode **headPerguntas, FilaNode **tailPerguntas) {
    int turno = 1;

    while (*headPerguntas != NULL) {
        printf("\nTurno do Jogador %d\n", turno);
        if (turno == 1) {
            jogarTurno(jogador1, headPerguntas, tailPerguntas);
            if (tamanhoPilha(*jogador1) == 3) {
                printf("Jogador 1 venceu!\n");
                break;
            }
        } else {
            jogarTurno(jogador2, headPerguntas, tailPerguntas);
            if (tamanhoPilha(*jogador2) == 3) {
                printf("Jogador 2 venceu!\n");
                break;
            }
        }
        turno = (turno == 1) ? 2 : 1;
    }
}

void jogarTurno(PilhaNode **jogador, FilaNode **headPerguntas, FilaNode **tailPerguntas) {
    char resposta[50];
    char *pergunta = dequeue(headPerguntas, tailPerguntas);
    printf("Pergunta: %s\n", pergunta);
    printf("Digite a resposta: ");
    scanf(" %[^\n]s", resposta);

    if (strcmp(resposta, "correta") == 0) {
        printf("Resposta correta! Símbolo coletado.\n");
        push(jogador, "Símbolo Coletado");
        mostrarSimbolos(*jogador);
    } else {
        printf("Resposta incorreta!\n");
    }
}

void mostrarSimbolos(PilhaNode *jogador) {
    printf("Símbolos coletados: ");
    while (jogador != NULL) {
        printf("%s -> ", jogador->simbolo);
        jogador = jogador->prox;
    }
    printf("NULL\n");
}

void ordenarSimbolos(PilhaNode *jogador) {
    // Algoritmo Bubble Sort para ordenar os símbolos (exemplo simplificado)
    if (jogador == NULL) return;

    PilhaNode *ptr1, *ptr2;
    char temp[50];
    for (ptr1 = jogador; ptr1 != NULL; ptr1 = ptr1->prox) {
        for (ptr2 = ptr1->prox; ptr2 != NULL; ptr2 = ptr2->prox) {
            if (strcmp(ptr1->simbolo, ptr2->simbolo) > 0) {
                strcpy(temp, ptr1->simbolo);
                strcpy(ptr1->simbolo, ptr2->simbolo);
                strcpy(ptr2->simbolo, temp);
            }
        }
    }
}

void exibirMenu() {
    printf("\n--- Menu ---\n");
    printf("1. Iniciar Jogo\n");
    printf("2. Exibir Regras\n");
    printf("3. Sair\n");
    printf("Escolha uma opção: ");
}

// Funções de pilha e fila

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
