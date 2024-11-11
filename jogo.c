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

// Estrutura da pilha de salas do labirinto
typedef struct Sala {
    int numeroSala;
    char descricao[200];
    Questao *questoes;
    int numQuestoes;
    int *perguntasUsadas; // Array para controlar perguntas já usadas na sala
    struct Sala *prox;
} Sala;

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
void empilharSala(Sala **topo, int numeroSala, const char *descricao, Questao *questoes, int numQuestoes) {
    Sala *novaSala = (Sala *)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        fprintf(stderr, "Erro de alocação de memória para a sala.\n");
        exit(EXIT_FAILURE);
    }
    novaSala->numeroSala = numeroSala;
    strcpy(novaSala->descricao, descricao);
    novaSala->questoes = questoes;
    novaSala->numQuestoes = numQuestoes;
    novaSala->prox = *topo;

    // Inicializa o array perguntasUsadas
    novaSala->perguntasUsadas = (int *)calloc(numQuestoes, sizeof(int));
    if (novaSala->perguntasUsadas == NULL) {
        fprintf(stderr, "Erro de alocação de memória para o array de perguntas usadas da sala.\n");
        exit(EXIT_FAILURE);
    }

    *topo = novaSala;
}

// Desempilha a sala atual
void desempilharSala(Sala **topo) {
    if (*topo == NULL) return;
    Sala *temp = *topo;
    *topo = (*topo)->prox;
    // Libera o array perguntasUsadas
    free(temp->perguntasUsadas);
    free(temp);
}

// Libera a memória alocada para a pilha de salas
void liberarPilhaSalas(Sala **topo) {
    while (*topo != NULL) {
        desempilharSala(topo);
    }
}

// Exibe o caminho percorrido pelo jogador
void exibirCaminhoPercorrido(Sala *topo) {
    printf("\nCaminho percorrido no labirinto:\n");
    Sala *atual = topo;
    // Como a pilha está invertida, precisamos armazenar as salas em uma lista temporária
    Sala *salasPercorridas[MAX_SALAS];
    int totalSalas = 0;
    while (atual != NULL) {
        salasPercorridas[totalSalas] = atual;
        totalSalas++;
        atual = atual->prox;
    }
    // Exibe as salas na ordem correta
    for (int i = totalSalas - 1; i >= 0; i--) {
        printf("Sala %d: %s\n", salasPercorridas[i]->numeroSala, salasPercorridas[i]->descricao);
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
    Sala *topoSala = NULL;
    Evento *headEvento = NULL, *tailEvento = NULL;
    char nomeJogador[50];
    int pontuacao = 0;
    int numeroSalaAtual = 0;
    indiceSimboloAtual = 0;
    indiceSalaAtual = 0;

    // Array para armazenar símbolos coletados
    char *simbolosColetados[MAX_SIMBOLOS];
    int totalSimbolosColetados = 0;

    printf("\nDigite o seu nome: ");
    scanf("%s", nomeJogador);

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

    // Sala 3: Salão dos Passistas (Médio)
    Questao questoesSala3[] = {
        {
            "Qual é a origem da palavra 'frevo'?",
            "c",
            "A palavra 'frevo' deriva de 'ferver', referindo-se à agitação e efervescência das ruas durante o carnaval.",
            "a) Frivolidade",
            "b) Frevoar",
            "c) Ferver",
            "d) Freviar"
        },
        {
            "Quantos tipos principais de frevo existem?",
            "b",
            "Existem três tipos: frevo de rua, frevo canção e frevo de bloco, cada um com características distintas.",
            "a) Dois",
            "b) Três",
            "c) Quatro",
            "d) Cinco"
        },
        {
            "O frevo foi declarado Patrimônio Imaterial da Humanidade pela UNESCO em:",
            "d",
            "Em 2012, o frevo recebeu esse reconhecimento, destacando sua importância cultural.",
            "a) 2000",
            "b) 2006",
            "c) 2010",
            "d) 2012"
        }
    };
    int numQuestoesSala3 = sizeof(questoesSala3) / sizeof(questoesSala3[0]);

    // Sala 4: Hall dos Mestres do Frevo (Médio)
    Questao questoesSala4[] = {
        {
            "Quem é considerado um dos maiores compositores de frevo canção?",
            "a",
            "Nelson Ferreira é um renomado compositor de frevo, conhecido por clássicos como 'Evocação nº 1'.",
            "a) Nelson Ferreira",
            "b) Luiz Gonzaga",
            "c) Tom Jobim",
            "d) Chico Science"
        },
        {
            "Qual destes não é um passo tradicional do frevo?",
            "c",
            "O 'Sarrinho' não é um passo tradicional do frevo.",
            "a) Dobradiça",
            "b) Ferrolho",
            "c) Sarrinho",
            "d) Tesoura"
        },
        {
            "O Paço do Frevo está localizado em qual bairro do Recife?",
            "b",
            "Está localizado no Bairro do Recife, também conhecido como Recife Antigo, centro histórico da cidade.",
            "a) Boa Vista",
            "b) Recife Antigo",
            "c) Casa Amarela",
            "d) Várzea"
        }
    };
    int numQuestoesSala4 = sizeof(questoesSala4) / sizeof(questoesSala4[0]);

    // Sala 5: Terraço das Orquestras (Difícil)
    Questao questoesSala5[] = {
        {
            "Qual é a assinatura rítmica típica do frevo?",
            "d",
            "O frevo é geralmente escrito em 2/4, dando-lhe um ritmo acelerado e contagiante.",
            "a) 3/4",
            "b) 4/4",
            "c) 6/8",
            "d) 2/4"
        },
        {
            "Qual maestro é conhecido por fundir o frevo com elementos de jazz?",
            "b",
            "Maestro Spok inovou ao integrar jazz ao frevo, criando um som único.",
            "a) Maestro Duda",
            "b) Maestro Spok",
            "c) Maestro Forró",
            "d) Maestro Nunes"
        },
        {
            "Em que ano o frevo surgiu oficialmente?",
            "c",
            "O frevo surgiu por volta de 1907, evoluindo de outras manifestações culturais.",
            "a) 1889",
            "b) 1895",
            "c) 1907",
            "d) 1922"
        }
    };
    int numQuestoesSala5 = sizeof(questoesSala5) / sizeof(questoesSala5[0]);

    // Sala 6: Exposição dos Estandartes (Difícil)
    Questao questoesSala6[] = {
        {
            "Os estandartes no frevo servem para:",
            "a",
            "Identificar e representar as agremiações carnavalescas, cada um com seu design único.",
            "a) Identificar agremiações",
            "b) Decorar palcos",
            "c) Fazer coreografias",
            "d) Presentear turistas"
        },
        {
            "Qual material é tradicionalmente usado na confecção dos estandartes?",
            "b",
            "Tecido bordado com pedrarias e paetês é comum, enriquecendo visualmente os estandartes.",
            "a) Papel crepom",
            "b) Tecido bordado",
            "c) Plástico",
            "d) Madeira entalhada"
        },
        {
            "Os estandartes são carregados por:",
            "c",
            "O porta-estandarte é responsável por carregar o estandarte à frente do grupo.",
            "a) Mestre de cerimônias",
            "b) Maestro",
            "c) Porta-estandarte",
            "d) Passistas"
        }
    };
    int numQuestoesSala6 = sizeof(questoesSala6) / sizeof(questoesSala6[0]);

    // Sala 7: Camarote das Máscaras (Muito Difícil)
    Questao questoesSala7[] = {
        {
            "As máscaras no frevo têm influência de qual cultura?",
            "d",
            "Têm influência das tradições europeias, especialmente do carnaval de Veneza.",
            "a) Africana",
            "b) Indígena",
            "c) Asiática",
            "d) Europeia"
        },
        {
            "Qual é o nome dado aos bonecos gigantes que participam do carnaval de Olinda?",
            "b",
            "São conhecidos como Bonecos de Olinda, símbolos do carnaval pernambucano.",
            "a) Mamulengos",
            "b) Bonecos de Olinda",
            "c) Gigantes de Pernambuco",
            "d) Colossos do Frevo"
        },
        {
            "As máscaras de frevo são geralmente feitas de:",
            "a",
            "São feitas de papel machê, permitindo criações artísticas variadas.",
            "a) Papel machê",
            "b) Plástico",
            "c) Metal",
            "d) Madeira"
        }
    };
    int numQuestoesSala7 = sizeof(questoesSala7) / sizeof(questoesSala7[0]);

    // Sala 8: Saída para o Carnaval de Recife (Muito Difícil)
    Questao questoesSala8[] = {
        {
            "Qual é o maior bloco de carnaval do mundo, segundo o Guinness Book?",
            "c",
            "O Galo da Madrugada é reconhecido como o maior bloco carnavalesco do mundo.",
            "a) Bloco do Eu Sozinho",
            "b) Cordão da Bola Preta",
            "c) Galo da Madrugada",
            "d) Olodum"
        },
        {
            "Em que dia da semana o Galo da Madrugada tradicionalmente desfila?",
            "b",
            "O Galo da Madrugada sai no Sábado de Zé Pereira, primeiro dia oficial do carnaval.",
            "a) Sexta-feira",
            "b) Sábado",
            "c) Domingo",
            "d) Terça-feira"
        },
        {
            "Qual é o tema central do Carnaval de Recife?",
            "a",
            "O frevo é o ritmo que embala o Carnaval de Recife, sendo sua marca registrada.",
            "a) Frevo",
            "b) Samba",
            "c) Axé",
            "d) Maracatu"
        }
    };
    int numQuestoesSala8 = sizeof(questoesSala8) / sizeof(questoesSala8[0]);

    // Atualize os arrays com as perguntas por sala
    Questao *questoesPorSala[MAX_SALAS] = {
        questoesSala1,
        questoesSala2,
        questoesSala3,
        questoesSala4,
        questoesSala5,
        questoesSala6,
        questoesSala7,
        questoesSala8
    };
    int numQuestoesPorSala[MAX_SALAS] = {
        numQuestoesSala1,
        numQuestoesSala2,
        numQuestoesSala3,
        numQuestoesSala4,
        numQuestoesSala5,
        numQuestoesSala6,
        numQuestoesSala7,
        numQuestoesSala8
    };

    // Empilha a sala inicial
    empilharSala(&topoSala, ++numeroSalaAtual, nomesDasSalas[indiceSalaAtual], questoesPorSala[indiceSalaAtual], numQuestoesPorSala[indiceSalaAtual]);
    indiceSalaAtual++; // Incrementa para apontar para a próxima sala

    // Enfileira alguns eventos
    enfileirarEvento(&headEvento, &tailEvento, "Você encontrou um mestre que lhe ensinou um novo passo!");
    enfileirarEvento(&headEvento, &tailEvento, "Uma multidão animada o empurra para frente.");
    enfileirarEvento(&headEvento, &tailEvento, "Você parou para apreciar uma apresentação.");
    enfileirarEvento(&headEvento, &tailEvento, "Uma chuva repentina molha o chão, tenha cuidado!");

    // Loop principal do jogo
    while (topoSala != NULL) {
        printf("\nVocê está na sala %d: %s\n", topoSala->numeroSala, topoSala->descricao);

        // Processa eventos
        Evento *eventoAtual = desenfileirarEvento(&headEvento);
        if (eventoAtual != NULL) {
            printf("Evento: %s\n", eventoAtual->descricao);
            // Lógica para lidar com o evento (pode ser expandida conforme necessário)
            free(eventoAtual);
        }

        // Seleciona uma pergunta da sala atual usando o perguntasUsadas da sala
        int indicePergunta = selecionarQuestao(topoSala->questoes, topoSala->numQuestoes, topoSala->perguntasUsadas);

        if (indicePergunta == -1) {
            printf("Você já respondeu todas as perguntas desta sala.\n");
            printf("Avançando automaticamente para a próxima sala.\n");
            // Avança para a próxima sala
            if (indiceSalaAtual >= MAX_SALAS) {
                printf("\nParabéns, %s! Você chegou ao final do labirinto!\n", nomeJogador);
                printf("Parabéns, %s! Você recebeu o título de Aprendiz do Frevo!\n", nomeJogador);
                break;
            }
            empilharSala(&topoSala, ++numeroSalaAtual, nomesDasSalas[indiceSalaAtual], questoesPorSala[indiceSalaAtual], numQuestoesPorSala[indiceSalaAtual]);
            indiceSalaAtual++; // Incrementa para apontar para a próxima sala
            continue; // Volta ao início do loop
        }

        Questao *perguntaAtual = &topoSala->questoes[indicePergunta];
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
                simbolosColetados[totalSimbolosColetados] = simbolosFrevo[indiceSimboloAtual];
                totalSimbolosColetados++;
                indiceSimboloAtual++;
            }

            // Verifica se o jogador chegou à última sala
            if (indiceSalaAtual >= MAX_SALAS) {
                printf("\nParabéns, %s! Você chegou ao final do labirinto!\n", nomeJogador);
                printf("Parabéns, %s! Você recebeu o título de Aprendiz do Frevo!\n", nomeJogador);
                break;
            }

            // Avança para a próxima sala
            empilharSala(&topoSala, ++numeroSalaAtual, nomesDasSalas[indiceSalaAtual], questoesPorSala[indiceSalaAtual], numQuestoesPorSala[indiceSalaAtual]);
            indiceSalaAtual++; // Incrementa para apontar para a próxima sala

        } else {
            printf("Resposta incorreta.\n");
            // Retrocede uma sala
            desempilharSala(&topoSala);
            if (topoSala == NULL) {
                printf("Você saiu do labirinto. Fim de jogo.\n");
                break;
            }
            indiceSalaAtual--; // Decrementa o índice da sala
            numeroSalaAtual--; // Decrementa o número da sala atual
        }
    }

    printf("\nSua pontuação final foi: %d pontos.\n", pontuacao);

    // Exibe o caminho percorrido
    exibirCaminhoPercorrido(topoSala);

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
