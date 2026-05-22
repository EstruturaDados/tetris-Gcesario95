/*
 * =====================================================================
 *  ByteBros — Tetris Stack
 *  Desafio: Simulação de fila de peças futuras (FIFO / Fila Circular)
 *  Nível   : Novato
 * ===================================================================
 *  Estrutura principal:
 *    - Fila circular de capacidade MAX_FILA
 *    - Cada peça tem: nome (char) e id (int)
 *    - Operações: enqueue, dequeue, exibir
 */

#include<stdio.h>
#include<stdlib.h>

// --- Constantes ---
#define MAX_FILA 10       // Capacidade máxima da fila de peças
#define TOTAL_INICIAL 5   // Número inicial de peças na fila

// Tipo de peças disponivel no jogo
#define TOTAL_TIPOS 4
const char TIPOS_PECA[TOTAL_TIPOS] = {'I', 'O', 'T', 'L'};

// Representa uma unica peça do jogo
typedef struct {
    char nome; // Tipo de peça (I, O, T, L)
    int id;    // Identificador unico de criação
} Peca;

// Fila circular de peça
typedef struct {
    Peca dados[MAX_FILA]; // Array para armazenar as peças
    int inicio;          // Índice do início da fila
    int fim;             // Índice do fim da fila
    int total;           // Número atual de peças na fila
} FilaPecas;

// --- Variavel global para controle de ID das peças ---
// Contador global para atribuir IDs únicos às peças
int proximoId = 0;

// --- Prototipos de funções ---
void inicializarFila(FilaPecas *f);
int filaCheia(FilaPecas *f);
int filaVazia(FilaPecas *f);
Peca gerarPeca(void);
int enqueue(FilaPecas *f, Peca p);
Peca dequeue(FilaPecas *f, int *p);
void exibirFila(FilaPecas *f);
void exibirMenu(void);

// --- Implementaçoes
void inicializarFila(FilaPecas *f) {
    f->inicio = 0;
    f->fim = 0;
    f->total = 0;
}

// Fila Cheia
int filaCheia(FilaPecas *f) {
    return f->total == MAX_FILA;
}

// Fila Vazia
int filaVazia(FilaPecas *f) {
    return f->total == 0;
}

/*
 * gerarPeca
 * Cria uma nova peça com tipo aleatório e id sequencial único.
 * O id é incrementado a cada chamada, garantindo unicidade.
 */

Peca gerarPeca(void) {
    Peca novaPeca;
    novaPeca.nome = TIPOS_PECA[rand() % TOTAL_TIPOS]; // Seleciona um tipo aleatório
    novaPeca.id = proximoId++; // Atribui o próximo ID e incrementa o contador
    return novaPeca;
}

/*
 * enqueue
 * Insere uma peça no final da fila circular.
 * Verifica se a fila está cheia antes de inserir.
 * Lógica circular: ao chegar no fim do array, o índice "fim"
 * volta ao início usando o operador módulo (%).
 */

int enqueue(FilaPecas *f, Peca p) {
    if (filaCheia(f)) {
        printf("Fila cheia! Não é possível adicionar mais peças.\n");
        return 0; // Falha ao enfileirar
    }
    f->dados[f->fim] = p; // Adiciona a peça no fim da fila
    f->fim = (f->fim + 1) % MAX_FILA; // Move o índice do fim circularmente
    f->total++; // Incrementa o total de peças na fila
    return 1; // Sucesso ao enfileirar
}

/*
 * dequeue
 * Remove e retorna a peça da frente da fila circular (FIFO).
 * Lógica circular: avança "inicio" com módulo para reutilizar espaço.
 */

 Peca dequeue (FilaPecas *f, int *p) {
    Peca pecaVazia = {' ', -1}; /* valor sentinela para fila vazia */
 
    if (filaVazia(f)) {
        printf("  [!] Fila vazia! Nao ha pecas para jogar.\n");
        *p = 0;
        return pecaVazia;
    }
 
    /* Recupera a peça da frente e avança o índice circularmente */
    Peca pecaRemovida = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % MAX_FILA;
    f->total--;
 
    *p = 1;
    return pecaRemovida;
}

/*
 * exibirFila
 * Percorre a fila e imprime cada peça no formato [TIPO id].
 * Usa índice circular para acessar os elementos na ordem correta.
 */
void exibirFila(FilaPecas *f) {
    printf("\n Fila de Peças (%d/%d): \n", f->total, MAX_FILA);
    if (filaVazia(f)) {
        printf("Fila vazia! Nao ha pecas para exibir.\n");

    } else {
        // Percorre a partir do inicio ate cobrir todos os elemntos
        for (int i = 0; i < f->total; i++) {
            int idx = (f->inicio + i) % MAX_FILA;
            printf("[%c %d] ", f->dados[idx].nome, f->dados[idx].id);
        }
    }

    printf("\n");
}

/**
 * exibirMenu
 * Mostra as opções disponíveis para o usuário interagir com a fila de peças.
 */
void exibirMenu(void) {
    printf("\n--- Menu TETRIS STACK ---\n");
    printf("1. Jogar peça (dequeue)\n");
    printf("2. Inserir peça (enqueue)\n");
    printf("3. Exibir fila de peças\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
}

// --- Função principal ---
int main(void) {
    FilaPecas fila;
    inicializarFila(&fila); // Inicializa a fila de peças

    // Preenche a fila com peças iniciais
    printf("\n ----------------------------------------------\n");
    printf(" BYTEBROS - TETRIS STACK - Desafio Novato\n");
    printf("Sistema de Fila de Peças (FIFO / Fila Circular)\n");
    printf(" ----------------------------------------------\n");
    printf("\n Inicializando fila com %d peças... \n", TOTAL_INICIAL);
    
    for (int i = 0; i < TOTAL_INICIAL; i++) {
        Peca novaPeca = gerarPeca();
        enqueue(&fila, novaPeca);
    }

    // Exibe o estado inicial da fila
    printf("\n  Confira a seguir seu estado: \n");
    exibirFila(&fila);

    // Loop principal do jogo
    int opcao;
    do {
        exibirMenu();
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: { // Jogar peça (dequeue)
                int sucesso;
                Peca pecaJogada = dequeue(&fila, &sucesso);
                if (sucesso) {
                    printf("Voce jogou a peca: [%c %d]\n", pecaJogada.nome, pecaJogada.id);
                }
                break;
            }
            case 2: { // Inserir peça (enqueue)
                Peca novaPeca = gerarPeca();
                if (enqueue(&fila, novaPeca)) {
                    printf("Nova peca inserida: [%c %d]\n", novaPeca.nome, novaPeca.id);
                }
                break;
            }
            case 3: // Exibir fila de peças
                exibirFila(&fila);
                break;
            case 0:
                printf("Saindo do jogo. Obrigado por jogar!\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;

}