#pragma once
#include "../include/openssl/crypto.h"
#include "../include/openssl/sha.h"
// Semente para o algoritmo mtwister
#define SEED 1234567
// Constantes
#define MAX_ENDERECOS = 256
#define MAX_BITCOINS_TRANSACAO = 50
#define MAX_TRANSACOES = 61
// Bloco que ainda não foi minerado
struct BlocoNaoMinerado
{
  unsigned int numero;                              // 4
  unsigned int nonce;                               // 4
  unsigned char data[184];                          // nao alterar. Deve ser inicializado com zeros.
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH]; // 32
};
typedef struct BlocoNaoMinerado BlocoNaoMinerado;

// Bloco que já foi minerado
struct BlocoMinerado
{
  BlocoNaoMinerado bloco;
  unsigned char hash[SHA256_DIGEST_LENGTH]; // 32 bytes
};
typedef struct BlocoMinerado BlocoMinerado;

//Arvore de Carteiras
struct arvore {

    unsigned int btc;
    int endereco;
    struct arvoreBinaria *esq, *dir;
};
typedef struct arvore arvore;

/* Gera um número aleatório inteiro e positivo com tamanho mínimo = min e máximo = max
 * Entrada:
 *  MTRand *r -> registro para o algoritmo MTwister
 *  unsigned int min -> menor valor possível do resultado (mínimo de 0)
 *  unsigned int max -> maior valor possível do resultado (0 se não houver)
 * Saída:
 *  unsigned int (um número inteiro positivo aleatório entre [min, max])
 */
unsigned int getRandInt(MTRand *r, unsigned int min, unsigned int max);

/* Inicializa o bloco não minerado
 * Entrada:
 *  BlocoNaoMinerado *bNM -> ponteiro para o bloco há ser inicializado
 *  BlocoMinerado *bAnt -> ponteiro para o bloco anterior
 *  MTRand *r -> registro para o algoritmo MTwister
 * Saída:
 *  void (Nenhuma)
 */
void initBloco(BlocoNaoMinerado *bNM, BlocoMinerado *bAnt, MTRand *r);

/* Minera um bloco de modo que a assinatura do bloco comece com 4 zeros
 * Entrada:
 *  BlocoNaoMinerado *bNM -> ponteiro para o bloco há ser minerado
 * Saída:
 *  BlocoMinerado * (Ponteiro para o bloco já minerado).
 */
BlocoMinerado *minerarBloco(BlocoNaoMinerado *bNM);

/* Busca um bloco minerado o qual o número do bloco seja igual a chave de busca
 * Entrada:
 *  BlocoMinerado *bM -> ponteiro para o bloco atual que será comparado
 *  unsigned int chave -> chave de busca, ou seja, bloco que queremos encontrar
 * Saída:
 *  BlocoMinerado * (Ponteiro para o bloco minerado onde o número do bloco é igual a chave)
 */
BlocoMinerado *buscaBloco(BlocoMinerado *bM, unsigned int chave);

/* Retorna a assinatura do n-ésimo bloco onde n = chave
 * Entrada:
 *  unsigned int chave -> número do bloco que queremos a assinatura
 * Saída:
 *  unsigned char * (Ponteiro para a assinatura do bloco)
 */
unsigned char *getHash(unsigned int chave);

/* Retorna o número de bitcoins de um dado endereço
 * Entrada:
 *  unsigned int end -> endereço o qual queremos saber a quantidade de bitcoins que possui
 *  int *vEnderecos -> vetor de endereços
 * Saída:
 *  unsigned int (Número de bitcoins que o endereço fornecido possui)
 */
unsigned int getBitcoins(unsigned int end, int *vEnderecos);

/* Retorna o endereço com o maior número de bitcoins
 * Entrada:
 *  int *vEnderecos -> vetor de endereços
 * Saída:
 *  int (Endereço com o maior número de bitcoins)
 */
int getMaxBitcoins(int *vEnderecos);

/* Lista os endereços em ordem crescente com base na quantidade de bitcoins
 * Entrada:
 *  int *vEnderecos -> vetor de endereços
 * Saída:
 *  void (nenhuma)
 */
void listarEnderecos(int *vEnderecos);
