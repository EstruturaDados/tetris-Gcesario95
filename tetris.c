/*
 * =====================================================================
 *                  ByteBros — Tetris Stack
 *      Desafio: Fila circular (FIFO) + Pilha de reserva (LIFO)
 *                  Nível   : Aventureiro
 * =====================================================================
 *  Estruturas principais:
 *    - FilaPecas  : fila circular de capacidade MAX_FILA
 *    - PilhaReserva: pilha linear de capacidade MAX_PILHA
 *    - Cada peça tem: nome (char) e id (int)
 *  Operações:
 *    - Jogar peça      : dequeue da fila + gera nova peça no final
 *    - Reservar peça   : dequeue da fila + push na pilha + gera nova peça
 *    - Usar reservada  : pop da pilha (peça descartada do jogo)
 *    - Exibir estado   : mostra fila e pilha após cada ação
 */

#include <stdio.h>
#include <stdlib.h>

// --- Constantes ---
 
#define MAX_FILA        10  // capacidade máxima da fila circular      
#define TOTAL_INICIAL    5  // peças geradas na inicialização           
#define MAX_PILHA        3  // capacidade máxima da pilha de reserva   
 
// Tipos de peça disponiveis no jogo 
#define TOTAL_TIPOS      4
const char TIPOS_PECA[TOTAL_TIPOS] = {'I', 'O', 'T', 'L'};
 
// --- Estruturas---
 
// Representa uma única peça do jogo
typedef struct {
    char nome;  // tipo da peça: 'I', 'O', 'T' ou 'L' 
    int  id;    // identificador único de criação       
} Peca;
 
/* Fila circular de peças futuras */
typedef struct {
    Peca dados[MAX_FILA]; // array que armazena as peças
    int  frente;          // índice da peça na frente (saída)
    int  tras;            // índice do próximo slot livre
    int  tamanho;         // quantidade de peças na fila
} FilaPecas;
 
// Pilha linear de peças reservadas (LIFO)
typedef struct {
    Peca dados[MAX_PILHA]; // array que armazena as peças reservadas
    int  topo;             // índice do topo (-1 = pilha vazia)
} PilhaReserva;
 
// Variável global de controle de ID
 
// Contador global: garante id único e crescente para cada peça criada
int proximoId = 0;
 
// --- Protótipos ---
 
// Fila
void  inicializarFila(FilaPecas *fila);
int   filaCheia(FilaPecas *fila);
int   filaVazia(FilaPecas *fila);
int   enqueue(FilaPecas *fila, Peca peca);
Peca  dequeue(FilaPecas *fila, int *sucesso);
 
// Pilha
void  inicializarPilha(PilhaReserva *pilha);
int   pilhaCheia(PilhaReserva *pilha);
int   pilhaVazia(PilhaReserva *pilha);
int   push(PilhaReserva *pilha, Peca peca);
Peca  pop(PilhaReserva *pilha, int *sucesso);
 
// Geral 
Peca  gerarPeca(void);
void  exibirEstado(FilaPecas *fila, PilhaReserva *pilha);
void  exibirMenu(void);
 
// --- Implementações: Fila ---
 
/*
 * inicializarFila
 * Prepara a fila para uso: zera índices e tamanho.
 */
void inicializarFila(FilaPecas *fila) {
    fila->frente  = 0;
    fila->tras    = 0;
    fila->tamanho = 0;
}
 
/*
 * filaCheia
 * Retorna 1 se a fila atingiu a capacidade máxima, 0 caso contrário.
 */
int filaCheia(FilaPecas *fila) {
    return fila->tamanho == MAX_FILA;
}
 
/*
 * filaVazia
 * Retorna 1 se não há peças na fila, 0 caso contrário.
 */
int filaVazia(FilaPecas *fila) {
    return fila->tamanho == 0;
}
 
/*
 * enqueue
 * Insere uma peça no final da fila circular.
 * Retorna 1 em caso de sucesso, 0 se a fila estiver cheia.
 * Lógica circular: o índice "tras" avança com módulo (%)
 * para reutilizar o espaço do início do array.
 */
int enqueue(FilaPecas *fila, Peca peca) {
    if (filaCheia(fila)) {
        printf("  [!] Fila cheia! Nao e possivel inserir mais pecas.\n");
        return 0;
    }
 
    fila->dados[fila->tras] = peca;
    fila->tras = (fila->tras + 1) % MAX_FILA;
    fila->tamanho++;
    return 1;
}
 
/*
 * dequeue
 * Remove e retorna a peça da frente da fila circular (FIFO).
 * Define *sucesso = 1 se válido, 0 se fila vazia.
 * Lógica circular: o índice "frente" avança com módulo (%).
 */
Peca dequeue(FilaPecas *fila, int *sucesso) {
    Peca pecaVazia = {' ', -1}; /* sentinela para fila vazia */
 
    if (filaVazia(fila)) {
        printf("  [!] Fila vazia! Nao ha pecas para jogar.\n");
        *sucesso = 0;
        return pecaVazia;
    }
 
    Peca pecaRemovida    = fila->dados[fila->frente];
    fila->frente         = (fila->frente + 1) % MAX_FILA;
    fila->tamanho--;
 
    *sucesso = 1;
    return pecaRemovida;
}
 
// --- Implementações: Pilha ---
 
/*
 * inicializarPilha
 * Prepara a pilha para uso: topo = -1 indica pilha vazia.
 */
void inicializarPilha(PilhaReserva *pilha) {
    pilha->topo = -1;
}
 
/*
 * pilhaCheia
 * Retorna 1 se a pilha atingiu MAX_PILHA, 0 caso contrário.
 */
int pilhaCheia(PilhaReserva *pilha) {
    return pilha->topo == MAX_PILHA - 1;
}
 
/*
 * pilhaVazia
 * Retorna 1 se não há peças na pilha, 0 caso contrário.
 */
int pilhaVazia(PilhaReserva *pilha) {
    return pilha->topo == -1;
}
 
/*
 * push
 * Empilha uma peça no topo da pilha de reserva (LIFO).
 * Retorna 1 em caso de sucesso, 0 se a pilha estiver cheia.
 */
int push(PilhaReserva *pilha, Peca peca) {
    if (pilhaCheia(pilha)) {
        printf("  [!] Pilha cheia! Nao e possivel reservar mais pecas.\n");
        return 0;
    }
 
    pilha->topo++;
    pilha->dados[pilha->topo] = peca;
    return 1;
}
 
/*
 * pop
 * Remove e retorna a peça do topo da pilha (LIFO).
 * Define *sucesso = 1 se válido, 0 se pilha vazia.
 */
Peca pop(PilhaReserva *pilha, int *sucesso) {
    Peca pecaVazia = {' ', -1}; /* sentinela para pilha vazia */
 
    if (pilhaVazia(pilha)) {
        printf("  [!] Pilha vazia! Nao ha pecas reservadas.\n");
        *sucesso = 0;
        return pecaVazia;
    }
 
    Peca pecaRemovida = pilha->dados[pilha->topo];
    pilha->topo--;
 
    *sucesso = 1;
    return pecaRemovida;
}
 
// --- Implementações: Geral ---
 
/*
 * gerarPeca
 * Cria uma nova peça com tipo aleatório e id sequencial único.
 * Usa rand() da stdlib.h — não requer time.h.
 */
Peca gerarPeca(void) {
    Peca novaPeca;
    novaPeca.nome = TIPOS_PECA[rand() % TOTAL_TIPOS]; /* tipo aleatório */
    novaPeca.id   = proximoId++;                       /* id sequencial  */
    return novaPeca;
}
 
/*
 * exibirEstado
 * Exibe o estado completo: fila (frente→trás) e pilha (topo→base).
 */
void exibirEstado(FilaPecas *fila, PilhaReserva *pilha) {
    // Fila 
    printf("\n  Estado atual:\n");
    printf("  Fila de pecas      : ");
 
    if (filaVazia(fila)) {
        printf("[ fila vazia ]");
    } else {
        for (int i = 0; i < fila->tamanho; i++) {
            int indice = (fila->frente + i) % MAX_FILA;
            printf("[%c %d] ", fila->dados[indice].nome, fila->dados[indice].id);
        }
    }
 
    // --- Pilha ---
    printf("\n  Pilha de reserva   : ");
 
    if (pilhaVazia(pilha)) {
        printf("[ pilha vazia ]");
    } else {
        // Exibe do topo até a base: topo → base
        printf("(Topo -> Base): ");
        for (int i = pilha->topo; i >= 0; i--) {
            printf("[%c %d] ", pilha->dados[i].nome, pilha->dados[i].id);
        }
    }
 
    printf("\n");
}
 
/*
 * exibirMenu
 * Imprime as quatro opções de ação do nível intermediário.
 */
void exibirMenu(void) {
    printf("\n =================================== \n");
    printf("    TETRIS STACK — MENU      \n");
    printf(" =================================== \n");
    printf("  │  1. Jogar peca \n");
    printf("  │  2. Reservar peca \n");
    printf("  │  3. Usar peca reservada \n");
    printf("  │  0. Sair \n");
    printf("  ==================================\n");
    printf("  Opcao: ");
}
 
// --- Programa principal ---
 
int main(void) {
    FilaPecas    fila;
    PilhaReserva pilha;
 
    inicializarFila(&fila);
    inicializarPilha(&pilha);
 
    /* ── Cabeçalho ── */
    printf("\n ====================================\n");
    printf("      ByteBros — TETRIS STACK  \n");
    printf("   Fila (FIFO) + Pilha de Reserva  \n");
    printf(" ====================================\n");
 
    // Preenche a fila com peças iniciais
    printf("\n  Inicializando fila com %d pecas...\n", TOTAL_INICIAL);
    for (int i = 0; i < TOTAL_INICIAL; i++) {
        enqueue(&fila, gerarPeca());
    }
 
    // Exibe estado inicial
    exibirEstado(&fila, &pilha);
 
    // --- Loop principal ---
    int opcao;
    do {
        exibirMenu();
        scanf("%d", &opcao);
 
        switch (opcao) {
 
            // Opção 1: Jogar peça
            case 1: {
                int sucesso;
                Peca pecaJogada = dequeue(&fila, &sucesso);
 
                if (sucesso) {
                    printf("\n  > Peca jogada: [%c %d]\n",
                           pecaJogada.nome, pecaJogada.id);
 
                    // Gera nova peça e mantém a fila sempre cheia
                    Peca novaPeca = gerarPeca();
                    enqueue(&fila, novaPeca);
                    printf("  > Nova peca gerada: [%c %d]\n",
                           novaPeca.nome, novaPeca.id);
                }
 
                exibirEstado(&fila, &pilha);
                break;
            }
 
            // Opção 2: Reservar peça (fila → pilha)
            case 2: {
                // Só reserva se houver espaço na pilha
                if (pilhaCheia(&pilha)) {
                    printf("\n  [!] Pilha cheia! Nao e possivel reservar mais pecas.\n");
                    exibirEstado(&fila, &pilha);
                    break;
                }
 
                int sucesso;
                Peca pecaReservada = dequeue(&fila, &sucesso);
 
                if (sucesso) {
                    push(&pilha, pecaReservada);
                    printf("\n  > Peca reservada: [%c %d]\n",
                           pecaReservada.nome, pecaReservada.id);
 
                    // Gera nova peça e mantém a fila sempre cheia
                    Peca novaPeca = gerarPeca();
                    enqueue(&fila, novaPeca);
                    printf("  > Nova peca gerada: [%c %d]\n",
                           novaPeca.nome, novaPeca.id);
                }
 
                exibirEstado(&fila, &pilha);
                break;
            }
 
            // Opção 3: Usar peça reservada (pop da pilha)
            case 3: {
                int sucesso;
                Peca pecaUsada = pop(&pilha, &sucesso);
 
                if (sucesso) {
                    printf("\n  > Peca reservada usada: [%c %d]\n",
                           pecaUsada.nome, pecaUsada.id);
                }
 
                exibirEstado(&fila, &pilha);
                break;
            }
 
            // Opção 0: Sair
            case 0:
                printf("\n  Encerrando o jogo. Ate logo!\n\n");
                break;
 
            // Entrada inválida
            default:
                printf("\n  [!] Opcao invalida. Tente novamente.\n");
                exibirEstado(&fila, &pilha);
                break;
        }
 
    } while (opcao != 0);
 
    return 0;
}
 