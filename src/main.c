#include <stdio.h>
#include "../include/blockchain.h"
#include "../include/mtwister/mtwister.h"

struct Metadados
{
  int quantidade;
  int tamanho_bloco;
};
typedef struct Metadados Metadados;

void criarHash(BlocoNaoMinerado *bNM, unsigned char *hash);
void printBloco(BlocoNaoMinerado *bNM);
// void addBlocoMinerado(BlocoMinerado *bM, NoLista **raiz);
void inicializarMetadados(const char *caminho, Metadados *cabecalho);
void arquivaBlocosMinerados(int n, BlocoMinerado *bM, const char *caminho);
void attMetadados(const char *caminho, int n, Metadados *cabecalho);
BlocoMinerado *procuraUltimoBloco(const char *caminho, int n);
void preencheCarteiras(MTRand *r, unsigned int *Carteira);
void transacoes(MTRand *r, unsigned int *Carteira, int nTrans);
arvore* insereNo(arvore **raiz, int chave);
arvore* criaNo(int chave);
void printArvore(arvore *raiz);
void montaArvore(unsigned int *Carteira, arvore **raiz, int tam);
void printMaior(arvore *raiz);

int main(int argc, char *argv[])
{

  MTRand r = seedRand(SEED);
  preencheCarteiras(&r, &Carteira);

  const char *arquivo = "./blockchain.bin";
  Metadados *cabecalho = (Metadados *)malloc(sizeof(Metadados));
  inicializarMetadados(arquivo, cabecalho);

  BlocoMinerado blocos[2];
  BlocoNaoMinerado *bNM = NULL;
  BlocoMinerado *bMAtual = NULL;
  BlocoMinerado *bMAnterior = NULL;
  arvore *raiz = NULL;
  unsigned int Carteira[MAX_ENDERECOS];
  int a = -1;
  int n = 0;

  if (cabecalho->quantidade == 0)
  {
    // Bloco Genesis
    bNM = alocaBlocoNaoMinerado();
    initBloco(bNM, NULL, &r, &Carteira);

    bMAtual = minerarBloco(bNM);
    bMAnterior = bMAtual;
    n += 1;
    blocos[n - 1] = *bMAtual;
  }
  else if (cabecalho->quantidade > 0)
  {
    bMAnterior = procuraUltimoBloco(arquivo, cabecalho->quantidade);
  }
    // NoLista *raiz = NULL;
    // addBlocoMinerado(genMinerado, &raiz);

  while(a != 0)
  {
    printf("Digite o numero referente a operação que deseja realizar\n");
    printf("1-Pesquisar hash de um bloco\n");
    scanf("%d"&a);
    printf("2-Pesquisar Carteira\n");
    scanf("%d"&a);
    printf("3-Buscar carteira com mais BitCoins\n");
    scanf("%d"&a);
    printf("4-Listar Carteiras e suas quantias de BitCoin(s)\n");
    scanf("%d"&a);
    printf("5-Minerar Bloco\n");
    scanf("%d"&a);
    printf("0-Sair\n");
    scanf("%d"&a);

    switch(a)
    {
      case 1:
        //Fazer a busca do hash de um bloco
        break;
      
      case 2:
        printf("Digite o endereco da Carteira que deseja pesquisar\n");
        int end;
        scanf("%d"&end);
        printf("A Carteira %d tem %u BitCoin(s)\n",end, Carteira[end]);
        break;
      
      case 3:
        montaArvore(&Carteira, &raiz, MAX_ENDERECOS);
        printMaior(raiz);
        break;
      
      case 4:
        montaArvore(&Carteira, &raiz, MAX_ENDERECOS);
        printf("Endereco   -   Bitcoin(s)\n");
        printArvore(arvore *raiz);
        break;
      
      case 5:
        int minerar = 1;
        while (minerar)
        {
          // Aloca um novo
          bNM = alocaBlocoNaoMinerado();
          // Inicializa
          initBloco(bNM, bMAnterior, &r, &Carteira);
          // Minera
          bMAtual = minerarBloco(bNM);
          n += 1;
          blocos[n - 1] = *bMAtual;
          if (n == 2)
          {
            arquivaBlocosMinerados(n, blocos, arquivo);
            attMetadados(arquivo, n, cabecalho);
            n = 0;
          }
          // Adiciona o bloco minerado na blockchain
          // addBlocoMinerado(bMAtual, &raiz);
          bMAnterior = bMAtual;
        }
        break;
    }
  }
  return 0;
}

void inicializarMetadados(const char *caminho, Metadados *cabecalho)
{
  FILE *arq = fopen(caminho, "rb");
  if (arq == NULL)
  {
    arq = fopen(caminho, "wb");
    cabecalho->quantidade = 0;
    cabecalho->tamanho_bloco = (int)sizeof(BlocoMinerado);
    if (fwrite(cabecalho, sizeof(Metadados), 1, arq) != 1)
    {
      fclose(arq);
      printf("Erro ao escrever no arquivo\n");
      exit(1);
    }
    printf("Ok!\n");
    fclose(arq);
    return;
  }
  // Verifica se existem metadados
  rewind(arq);
  if (fread(cabecalho, sizeof(Metadados), 1, arq) != 1)
  {
    fclose(arq);
    printf("Erro na leitura do arquivo\n");
    exit(1);
  }
  // Se existem o que está escrito neles?
  if (cabecalho->quantidade >= 0 && cabecalho->tamanho_bloco == sizeof(BlocoMinerado))
  {
    fclose(arq);
    printf("%d, %d\n", cabecalho->quantidade, cabecalho->tamanho_bloco);
    return;
  }
  fclose(arq);
}

BlocoMinerado *procuraUltimoBloco(const char *caminho, int n)
{
  // abre o arquivo para leitura
  FILE *arq = fopen(caminho, "rb");
  if (arq == NULL)
  {
    printf("Falhaa ao abrir o arquivo");
    exit(1);
  }

  long int offset = 0;
  // calcula a posição do ponteiro leitor
  if (n == 0)
  {
    offset = sizeof(Metadados);
  }
  else
  {
    offset = (sizeof(BlocoMinerado) * (n - 1)) + sizeof(Metadados);
  }
  // posiciona o ponteiro
  rewind(arq);
  if (fseek(arq, offset, SEEK_SET) != 0)
  {
    fclose(arq);
    exit(1);
  }
  BlocoMinerado *aux = (BlocoMinerado *)malloc(sizeof(BlocoMinerado));
  if (fread(aux, sizeof(BlocoMinerado), 1, arq) != 1)
  {
    printf("Falha na leitura do arquivo");
    fclose(arq);
    exit(1);
  }
  fclose(arq);

  return aux;
}

void arquivaBlocosMinerados(int n, BlocoMinerado *bM, const char *caminho)
{

  FILE *arq = fopen(caminho, "rb");
  if (arq == NULL)
  {
    printf("Erro ao abrir o arquivo");
    exit(1);
  }
  rewind(arq);
  Metadados cabecalho;

  if (fread(&cabecalho, sizeof(Metadados), 1, arq) != 1)
  {
    fclose(arq);
    printf("Erro na leitura do arquivo");
    exit(1);
  }
  fclose(arq);
  arq = fopen(caminho, "ab");
  if (fwrite(bM, sizeof(BlocoMinerado), n, arq) != n)
  {
    fclose(arq);
    printf("Erro na gravação no arquivo\n");
    exit(1);
  }
  fclose(arq);
}

void attMetadados(const char *caminho, int n, Metadados *cabecalho)
{
  FILE *arq = fopen(caminho, "r+b");
  if (arq == NULL)
  {
    printf("Erro na abertura do arquivo\n");
    exit(1);
  }
  rewind(arq);
  if (fread(cabecalho, sizeof(Metadados), 1, arq) != 1)
  {
    fclose(arq);
    printf("Erro na leitrua do arquivo\n");
    exit(1);
  }
  rewind(arq);
  cabecalho->quantidade += n;
  if (fwrite(cabecalho, sizeof(Metadados), 1, arq) != 1)
  {
    fclose(arq);
    printf("Erro ao escrever no arquivo\n");
    exit(1);
  }
  fclose(arq);
}

void criarHash(BlocoNaoMinerado *bNM, unsigned char *hash)
{
  SHA256((unsigned char *)bNM, sizeof(BlocoNaoMinerado), hash);
}

void printBloco(BlocoNaoMinerado *bNM)
{
  printf("Numero: %ud\n", bNM->numero);
  printf("Nonce: %ud\n", bNM->nonce);
  for (int i = 0; i < 183; i += 3)
  {
    printf("Transacao: Origem: %d, Destino: %d, BitCoins: %d\n", bNM->data[i], bNM->data[i + 1], bNM->data[i + 2]);
  }
}

BlocoMinerado *minerarBloco(BlocoNaoMinerado *bNM)
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  // Para não demorar muito na mineração
  // 0 para nao minerado e 1 para minerado
  char minerado = 0;
  do
  {
    criarHash(bNM, hash);
    int *chave = (int *)hash;
    if (*chave == 0)
      minerado = 1;
    bNM->nonce += 1;
  } while (minerado == 0);
  bNM->nonce -= 1;
  printBloco(bNM);
  BlocoMinerado *aux = (BlocoMinerado *)malloc(sizeof(BlocoMinerado));
  aux->bloco = *bNM;
  for (int k = 0; k < SHA256_DIGEST_LENGTH; k++)
  {
    aux->hash[k] = hash[k];
    printf("%x", hash[k]);
  }
  printf("\n");
  return aux;
}

unsigned int getRandInt(MTRand *r, unsigned int min, unsigned int max)
{
  return (unsigned int)(genRandLong(r) % (max - min + 1)) + min;
}

BlocoNaoMinerado *alocaBlocoNaoMinerado()
{
  BlocoNaoMinerado *aux = (BlocoNaoMinerado *)malloc(sizeof(BlocoNaoMinerado));
  return aux;
}

void initBloco(BlocoNaoMinerado *bNM, BlocoMinerado *bAnt, MTRand *r, unsigned int *Carteira)
{

  int i = 0;
  if (bAnt == NULL)
  {
    bNM->numero = 1;
    bNM->nonce = 0;
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
      bNM->hashAnterior[i] = 0;
  }
  else
  {
    // Definindo o número do bloco
    bNM->numero = bAnt->bloco.numero + 1;

    // Definindo nonce um número aleatório entre [0, 10000]
    bNM->nonce = 0;

    // Definindo a assinatura do bloco anterior
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
      bNM->hashAnterior[i] = bAnt->hash[i];
  }

  // Inicializando o campo data com 0
  i = 0;
  for (i = 0; i < 184; i++)
    bNM->data[i] = 0;

  // Gerando o número de transações
  unsigned int nTrans = getRandInt(r, 0, MAX_TRANSACOES);

  transacoes(r, Carteira, nTrans);
}

void preencheCarteiras(MTRand *r, unsigned int *Carteira) 
{
  int i = 0;
  while (i < MAX_ENDERECOS)
  {
    Carteira[i] = getRandInt(r, 0, MAX_BITCOINS_TRANSACAO);
    i++;
  }
}

void transacoes(MTRand *r, unsigned int *Carteira, int nTrans)
{
  for(int i=0; i < nTrans; i++)
  {
    short int origem = getRandInt(r, 0, 255);
    short int destino = getRandInt(r, 0, 255);

    if(origem == destino)
      {
        nTrans++;
        continue;
      }

    short int btc = getRandInt(r, 0, MAX_BITCOINS_TRANSACAO);
    
    if(btc > Carteira[origem])
      Carteira[origem] = 0;
    else
      Carteira[origem] -= btc;

    Carteira[destino] += btc;
  }
}

arvore* insereNo(arvore **raiz,unsigned int btc, int endereco) 
{

  if((*raiz) == NULL) 
      return *raiz = criaNo(btc); 

  if(btc < (*raiz)->btc)
    (*raiz)->esq = insereNo( &((*raiz)->esq), btc, endereco);

  else
    (*raiz)->dir = insereNo( &((*raiz)->dir), btc, endereco);    
}

arvore* criaNo(unsigned int btc, int endereco) {

    arvore* no;
    
    no = (arvore *) malloc(sizeof(arvore));
    no->btc = btc;
    no->endereco = endereco;
    no->esq = NULL;
    no->dir = NULL;
    return no;
}

void printArvore(arvore *raiz) {
  if (raiz==NULL) return;
  printArvore(raiz->esq);
  printf("%d   -   %u\n", raiz->endereco, raiz->btc);
  printArvore(raiz->dir);
}

void montaArvore(unsigned int *Carteira, arvore **raiz, int tam)
{
  for(int i=0; i < tam; i++)
    insereNo(raiz,Carteira[i],i);
}

void printMaior(arvore *raiz) {
  if (raiz==NULL) return;
  if (raiz->dir == NULL)
    printf("A Carteira com maior valor eh a %d com um total de %u BitCoin(s)\n", raiz->endereco, raiz->btc);
  printArvore(raiz->dir);
}

/*do
  {
    // Endereço de origem
    unsigned int origem = getRandInt(r, 0, MAX_ENDERECOS);
    // Endereço de destino
    unsigned int destino = getRandInt(r, 0, MAX_ENDERECOS);
    //   // Número de bitcoins na transação
    unsigned int nBitcoins = getRandInt(r, 0, MAX_BITCOINS_TRANSACAO);

    bNM->data[i] = (unsigned char)origem;
    bNM->data[i + 1] = (unsigned char)destino;
    bNM->data[i + 2] = (unsigned char)nBitcoins;
    i += 3;
  } while (i <= nTrans);*/

/*void addBlocoMinerado(BlocoMinerado *bM, NoLista **raiz)
{
  NoLista *aux = (NoLista *)malloc(sizeof(NoLista));
  aux->bM = bM;
  aux->prox = NULL;
  if (*raiz == NULL)
  {
    *raiz = aux;
    return;
  }
  aux->prox = (*raiz)->prox;
  (*raiz)->prox = aux;
}

void transacoes(MTRand *r, EnderecoAVL **end, BitCoinAVL **btc)
{
  // for (int i = getRandInt(r, 0, 61); i > 0; i--)
  // {
  //   unsigned int origem, destino;
  //   origem = getRandInt(r, 0, MAX_ENDERECOS);
  //   destino = getRandInt(r, 0, MAX_ENDERECOS);
  //   if (origem != destino)
  //   { // Verificação para evitar que seja realizada uma transação com origem e destino iguais
  //     // Endereço de origem
  //     unsigned int origemCarteira = origem;
  //     // Endereço de destino
  //     unsigned int destinoCarteira = destino;
  //     // Número de bitcoins na transação
  //     unsigned int nBitcoins = getRandInt(r, 0, MAX_BITCOINS_TRANSACAO);
  //   }
  //   else // caso sejam iguais o i é incrementado para que uma nova transacao valida ocorra
  //     i++;
  // }
}*/
/*

unsigned char *buscaHash(NoLista *raiz, unsigned int chave)
{
  int naoAchou = 1;
  int numAtual = raiz->bM->bloco.numero;
  do
  {
    if (raiz == NULL)
    {
      naoAchou = 1;
      break;
    }
    if (numAtual == chave)
      naoAchou = 0;
    raiz = raiz->prox;
  } while (naoAchou);
  return naoAchou ? NULL : raiz->bM->hash;
}
*/