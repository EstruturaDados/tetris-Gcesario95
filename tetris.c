/*
 * =====================================================================
 *  ByteBros — Tetris Stack
 *  Desafio: Fila circular (FIFO) + Pilha (LIFO) + Trocas estratégicas
 *  Nível   : Mestre
 * =====================================================================
 *  Estruturas principais:
 *    - FilaPecas   : fila circular de capacidade MAX_FILA (5)
 *    - PilhaReserva: pilha linear de capacidade MAX_PILHA (3)
 *  Operações:
 *    1 - Jogar peça         : dequeue da fila + gera nova peça
 *    2 - Reservar peça      : dequeue da fila → push na pilha + gera nova peça
 *    3 - Usar reservada     : pop da pilha (peça descartada do jogo)
 *    4 - Troca simples      : frente da fila ↔ topo da pilha
 *    5 - Troca múltipla     : 3 primeiras da fila ↔ 3 peças da pilha
 */

#include <stdio.h>
#include <stdlib.h>

// ---Constantes ---

#define MAX_FILA      5   // capacidade máxima da fila circular        
#define MAX_PILHA     3   // capacidade máxima da pilha de reserva     
#define TOTAL_INICIAL 5   // peças geradas na inicialização            
#define TOTAL_TROCA   3   // quantidade de peças envolvidas na troca   

// Tipos de peça disponíveis no jogo 
#define TOTAL_TIPOS   4
const char TIPOS_PECA[TOTAL_TIPOS] = {'I', 'O', 'T', 'L'};

// --- Estruturas ----

// Representa uma única peça do jogo
typedef struct {
    char nome;  // tipo da peça: 'I', 'O', 'T' ou 'L' 
    int  id;    // identificador único de criação       
} Peca;

// Fila circular de peças futuras
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

// Garante id único e crescente para cada peça criada 
int proximoId = 0;

// --- Protótipos ---

// --- Fila---
void  inicializarFila(FilaPecas *fila);
int   filaCheia(FilaPecas *fila);
int   filaVazia(FilaPecas *fila);
int   enqueue(FilaPecas *fila, Peca peca);
Peca  dequeue(FilaPecas *fila, int *sucesso);

// --- Pilha ---
void  inicializarPilha(PilhaReserva *pilha);
int   pilhaCheia(PilhaReserva *pilha);
int   pilhaVazia(PilhaReserva *pilha);
int   push(PilhaReserva *pilha, Peca peca);
Peca  pop(PilhaReserva *pilha, int *sucesso);

// --- Trocas ---
void  trocarSimples(FilaPecas *fila, PilhaReserva *pilha);
void  trocarMultipla(FilaPecas *fila, PilhaReserva *pilha);

// --- Geral ---
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
 * Retorna 1 se a fila atingiu MAX_FILA, 0 caso contrário.
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
 * O índice "tras" avança com módulo para reaproveitamento circular.
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
 * O índice "frente" avança com módulo para reaproveitamento circular.
 */
Peca dequeue(FilaPecas *fila, int *sucesso) {
    Peca pecaVazia = {' ', -1}; // sentinela para fila vazia 

    if (filaVazia(fila)) {
        printf("  [!] Fila vazia! Nao ha pecas para jogar.\n");
        *sucesso = 0;
        return pecaVazia;
    }

    Peca pecaRemovida = fila->dados[fila->frente];
    fila->frente      = (fila->frente + 1) % MAX_FILA;
    fila->tamanho--;

    *sucesso = 1;
    return pecaRemovida;
}

// Implementações: Pilha ---

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
    Peca pecaVazia = {' ', -1}; // sentinela para pilha vazia

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

// --- Implementações: Trocas ---

/*
 * trocarSimples
 * Troca a peça da frente da fila com o topo da pilha.
 * Estratégia:
 *   - Acessa diretamente fila->dados[fila->frente] e pilha->dados[pilha->topo]
 *   - Usa variável auxiliar "temp" para a troca (swap clássico)
 *   - Nenhuma peça é removida; apenas os valores são permutados.
 */
void trocarSimples(FilaPecas *fila, PilhaReserva *pilha) {
    // Valida se há peça disponível em ambas as estruturas 
    if (filaVazia(fila)) {
        printf("  [!] Fila vazia! Nao ha peca para trocar.\n");
        return;
    }
    if (pilhaVazia(pilha)) {
        printf("  [!] Pilha vazia! Nao ha peca para trocar.\n");
        return;
    }

    // Swap: frente da fila ↔ topo da pilha
    Peca temp                         = fila->dados[fila->frente];
    fila->dados[fila->frente]         = pilha->dados[pilha->topo];
    pilha->dados[pilha->topo]         = temp;

    printf("\n  > Troca simples realizada: frente da fila <-> topo da pilha.\n");
}

/*
 * trocarMultipla
 * Troca as 3 primeiras peças da fila com as 3 peças da pilha.
 * Estratégia:
 *   - Verifica se fila tem >= 3 peças e pilha tem exatamente 3
 *   - Percorre as 3 posições realizando swap direto no array
 *   - A fila é acessada via índice circular: (frente + i) % MAX_FILA
 *   - A pilha é acessada de topo (0) até base (2): pilha->dados[topo - i]
 *   - Nenhuma peça é removida; apenas os valores são permutados.
 */
void trocarMultipla(FilaPecas *fila, PilhaReserva *pilha) {
    // Valida se ambas as estruturas têm pelo menos TOTAL_TROCA peças
    if (fila->tamanho < TOTAL_TROCA) {
        printf("  [!] A fila precisa ter pelo menos %d pecas para a troca multipla.\n",
               TOTAL_TROCA);
        return;
    }
    if (pilha->topo + 1 < TOTAL_TROCA) {
        printf("  [!] A pilha precisa ter pelo menos %d pecas para a troca multipla.\n",
               TOTAL_TROCA);
        return;
    }

    /*
     * Percorre as 3 posições e realiza swap entre:
     *   - fila  : posições [frente+0], [frente+1], [frente+2] (circular)
     *   - pilha : posições [topo], [topo-1], [topo-2] (topo → base)
     */
    for (int i = 0; i < TOTAL_TROCA; i++) {
        int indiceFila  = (fila->frente + i) % MAX_FILA;
        int indicePilha = pilha->topo - i;

        Peca temp                    = fila->dados[indiceFila];
        fila->dados[indiceFila]      = pilha->dados[indicePilha];
        pilha->dados[indicePilha]    = temp;
    }

    printf("\n  > Troca multipla realizada: 3 primeiros da fila <-> 3 da pilha.\n");
}

// --- Implementações: Geral ---

/*
 * gerarPeca
 * Cria uma nova peça com tipo aleatório e id sequencial único.
 * Usa rand() da stdlib.h — não requer time.h.
 */
Peca gerarPeca(void) {
    Peca novaPeca;
    novaPeca.nome = TIPOS_PECA[rand() % TOTAL_TIPOS]; // tipo aleatório
    novaPeca.id   = proximoId++;                       // id sequencial
    return novaPeca;
}

/*
 * exibirEstado
 * Exibe o estado completo: fila (frente→trás) e pilha (topo→base).
 */
void exibirEstado(FilaPecas *fila, PilhaReserva *pilha) {
    printf("\n  Estado atual:\n");

    //--- Fila ---
    printf("  Fila de pecas    : ");
    if (filaVazia(fila)) {
        printf("[ fila vazia ]");
    } else {
        for (int i = 0; i < fila->tamanho; i++) {
            int indice = (fila->frente + i) % MAX_FILA;
            printf("[%c %d] ", fila->dados[indice].nome, fila->dados[indice].id);
        }
    }

    //--- Pilha ---
    printf("\n  Pilha de reserva : ");
    if (pilhaVazia(pilha)) {
        printf("[ pilha vazia ]");
    } else {
        printf("(Topo -> Base): ");
        for (int i = pilha->topo; i >= 0; i--) {
            printf("[%c %d] ", pilha->dados[i].nome, pilha->dados[i].id);
        }
    }

    printf("\n");
}

/*
 * exibirMenu
 * Imprime as seis opções de ação do nível mestre.
 */
void exibirMenu(void) {
    printf("\n  ┌──────┬──────────────────────────────────────────────┐\n");
    printf("  │      │         TETRIS STACK — NIVEL MESTRE          │\n");
    printf("  ├──────┼──────────────────────────────────────────────┤\n");
    printf("  │  1   │ Jogar peca da frente da fila                 │\n");
    printf("  │  2   │ Enviar peca da fila para a pilha de reserva  │\n");
    printf("  │  3   │ Usar peca da pilha de reserva                │\n");
    printf("  │  4   │ Trocar frente da fila com topo da pilha      │\n");
    printf("  │  5   │ Trocar os 3 primeiros da fila com a pilha    │\n");
    printf("  │  0   │ Sair                                         │\n");
    printf("  └──────┴──────────────────────────────────────────────┘\n");
    printf("  Opcao: ");
}

// --- Programa principal ---

int main(void) {
    FilaPecas    fila;
    PilhaReserva pilha;

    inicializarFila(&fila);
    inicializarPilha(&pilha);

    //--- Cabeçalho ---
    printf("\n ----------------------------------------------------\n");
    printf("  ---   ByteBros — TETRIS STACK                      ---\n");
    printf("  ---   Fila Circular + Pilha de Reserva + Trocas    ---\n");
    printf("  ---   Nivel: Mestre                                  ---\n");
    printf("  ----------------------------------------------------\n");

    // Preenche a fila com peças iniciais
    printf("\n  Inicializando fila com %d pecas...\n", TOTAL_INICIAL);
    for (int i = 0; i < TOTAL_INICIAL; i++) {
        enqueue(&fila, gerarPeca());
    }

    // Exibe o estado inicial 
    exibirEstado(&fila, &pilha);

    // --- Loop principal----
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

                    // Gera nova peça para manter a fila sempre cheia
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

                    // Gera nova peça para manter a fila sempre cheia
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

            // Opção 4: Troca simples (frente da fila ↔ topo da pilha) 
            case 4: {
                trocarSimples(&fila, &pilha);
                exibirEstado(&fila, &pilha);
                break;
            }

            // Opção 5: Troca múltipla (3 da fila ↔ 3 da pilha) 
            case 5: {
                trocarMultipla(&fila, &pilha);
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