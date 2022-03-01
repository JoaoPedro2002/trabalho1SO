#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 10

int numeroImovel = 9;

pthread_mutex_t inquilino;
pthread_mutex_t corretor;


// estrutura imoveis
typedef struct Imovel{
    int codigo;
    char endereco[30];
    float preco;
    char bairro[30];
} Imovel;

// estrutura nodo, carrega o imovel e a referencia para o proximo nodo
struct Node {
	struct Imovel data;
	struct Node* next;
};

struct Node* headImoveisEntregues;
struct Node* headImoveisDisponiveis;
struct Node* currentImoveisEntregues; // ponteiro entregues
struct Node* currentImoveisDisponiveis; // ponteiro disponiveis

int tamanhoImoveisDisponiveis = 0; // tamanho disponiveis
int tamanhoImoveisEntregues = 0; // tamanho entregues

// criar novo nodo
struct Node* novoNodo(struct Imovel imv) { 
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->data = imv;
	newNode->next = NULL;
	return newNode;
};

// Adiciona na cauda
void insertDisponiveis(struct Imovel imv) {
	struct Node* temp = headImoveisDisponiveis;
	struct Node* newNode = novoNodo(imv);
	if(headImoveisDisponiveis == NULL) { // caso a lista seja vazia
		headImoveisDisponiveis = newNode;
		return;
	}
	while(temp->next != NULL) temp = temp->next; // vai para o ultimo elemento
	temp->next = newNode;
  tamanhoImoveisDisponiveis++;
};

void insertEntregues(struct Imovel imv) {
	struct Node* temp = headImoveisEntregues;
	struct Node* newNode = novoNodo(imv);
	if(headImoveisEntregues == NULL) {
		headImoveisEntregues = newNode;
		return;
	}
	while(temp->next != NULL) temp = temp->next;
	temp->next = newNode;
  tamanhoImoveisEntregues++;
};

void irParaPrimeiroDisponiveis() { currentImoveisDisponiveis = headImoveisDisponiveis; }

void irParaPrimeiroEntregues() { currentImoveisEntregues = headImoveisEntregues; }

void avancarNPosicoesDisponiveis(int n){
  irParaPrimeiroDisponiveis();
  for(int i = 0; i < n; i++){
    if (currentImoveisDisponiveis->next == NULL){
      break;
    } else {
      currentImoveisDisponiveis = currentImoveisDisponiveis->next;
    }
  }
}

void avancarNPosicoesEntregues(int n) {
  irParaPrimeiroEntregues();
  for(int i = 0; i < n; i++){
    if (currentImoveisEntregues->next == NULL){
      break;
    } else {
      currentImoveisEntregues = currentImoveisEntregues->next;
    }
  }
}

void deleteDisponiveis(int delCodigo)
{
  struct Node* temp = headImoveisDisponiveis;
  
  //Caso tenha apenas um nodo na lista
  if(temp->next == NULL)
  {
    if (temp->data.codigo != delCodigo) { printf("Valor nao encontrado"); return; }
    headImoveisDisponiveis = NULL;
    free(temp);
    tamanhoImoveisDisponiveis--;
    return;

  }

  //Se o head for deletado
  if(temp!=NULL && temp->data.codigo==delCodigo)
  {
    headImoveisDisponiveis = temp->next;
    free(temp);
    tamanhoImoveisDisponiveis--;
    return;
  }

  struct Node* previous;

  // Procura o imovel pelo codigo
  while (temp != NULL && temp->data.codigo != delCodigo) {
    previous = temp;
    temp = temp->next;
  }

  //Valor nao encontrado
  if(temp==NULL)
  {
    printf("Value not found\n");
    return;
  }

  // Se o ultimo nodo for excluido
  if(temp->next == NULL)
  {
    previous->next = NULL;
    free(temp);
    tamanhoImoveisDisponiveis--;
    return;
  } 

  previous->next = temp->next;
  free(temp);
  tamanhoImoveisDisponiveis--;
}

void deleteEntregues(int delCodigo) {
  struct Node* temp = headImoveisEntregues;

  //Quando tem apenas um nodo na lista
  if(temp->next == NULL)
  {
    if (temp->data.codigo != delCodigo) { printf("Valor nao enontrado"); return; }
    headImoveisEntregues = NULL;
    free(temp);
  
    tamanhoImoveisEntregues--;
    return;

  }
  //Se a head seja o valor a ser removido
  if(temp!=NULL && temp->data.codigo==delCodigo)
  {
    headImoveisEntregues = temp->next; 
    //headImoveisEntregues->prev = NULL; 
    
    free(temp);
    tamanhoImoveisEntregues--;
    return;
  }

  struct Node* previous;
  // Tenta encontrar um imovel pelo codigo
  while (temp != NULL && temp->data.codigo != delCodigo) {
    previous = temp;
    temp = temp->next;
  }

  //Valor nao encontrado
  if(temp==NULL)
  {
    printf("Valor nao encontrado\n");
    return;
  }

  //Se o nodo a ser deletado é o ultimo
  if(temp->next == NULL)
  {
    previous->next = NULL;
    free(temp);
    tamanhoImoveisEntregues--;
    return;
  } 

  //Caso o nodo esteja entre outros dois nodos
  previous->next = temp->next;
  free(temp);
  tamanhoImoveisEntregues--;
}

Imovel alugar() {
  int aleatorio = rand() % tamanhoImoveisDisponiveis;
  avancarNPosicoesDisponiveis(aleatorio); // vai para a posicao definida por um numero aleatorio de 0 ate o tamanho da lista
  Imovel imvAlugado = currentImoveisDisponiveis->data;
  deleteDisponiveis(imvAlugado.codigo); // remove o imovel da lista de imoveis disponiveis
  printf("Inquilino aluga imovel %d\n", imvAlugado.codigo);
  return imvAlugado; // retorna imovel
}

void devolver(Imovel imvAlugado) {
  insertEntregues(imvAlugado); // adiciona imovel na lista de imoveis entregues
  printf("Inquilino devolve imovel %d\n", imvAlugado.codigo);
}

void* t_inquilino(void *args)
{
    srand(time(NULL));
    int tempo = (rand() % (3 - 1 + 1));
    pthread_mutex_lock(&inquilino);
    if (tamanhoImoveisDisponiveis > 0) {
      Imovel imvAlugado = alugar(); // alugar imovel
      pthread_mutex_unlock(&inquilino);
      sleep(tempo); // espera um tempo entre 1 e 3
      pthread_mutex_lock(&inquilino);
      devolver(imvAlugado); // devolve
    }
    pthread_mutex_unlock(&inquilino);
    pthread_exit(NULL);
};

void adicionarImovel() {
  srand(time(NULL));
  float preco = rand() % (10000 + 1 - 1000) + 1000; // preco aleatorio entre 10000 e 1000
  Imovel imovel = {numeroImovel, "endereco", preco, "bairro" }; // cria imovel
  insertDisponiveis(imovel); // adiciona imovel
  printf("Corretor adiciona imovel %d\n", numeroImovel);
  numeroImovel++;
}

void removerImovel() {
  int aleatorio = rand() % tamanhoImoveisDisponiveis;
  avancarNPosicoesDisponiveis(aleatorio); // vai para a posicao definida por um numero aleatorio de 0 ate o tamanho da lista 
  Imovel imvRemovido = currentImoveisDisponiveis->data; // pega o imovel dessa posição
  deleteDisponiveis(imvRemovido.codigo); // remove da lista de imoveis disponiveis
  printf("Corretor remove imovel %d\n", imvRemovido.codigo);
}

void moverImovel() {
  int aleatorio = rand() % tamanhoImoveisEntregues;
  avancarNPosicoesEntregues(aleatorio); // vai para a posicao definida por um numero aleatorio de 0 ate o tamanho da lista 
  Imovel imvMovido = currentImoveisEntregues->data; // pega o imovel da lista de entregues
  deleteEntregues(imvMovido.codigo); // remove o imovel da lista de entregues
  insertDisponiveis(imvMovido); // insere o imovel na lista de disponiveis
  printf("Corretor move imovel %d \n", imvMovido.codigo);
}

void* t_corretor(void *args) {
  
  srand(time(NULL));

  for (int i=0; i < 3; i++) {
    pthread_mutex_lock(&corretor);
    int opcao = (rand() % (3 - 1 + 1)) + 1;
    printf("Corretor selecionou a opcao: %s\n", opcao == 1 ? "adicionar" : opcao == 2 ? "mover" : "remover");
    switch (opcao) { // opcao aleatorio 1: adicionar, 2: mover, 3: remover
    case 1:
        adicionarImovel(); // adicionar
        break;
    case 2:
      if (tamanhoImoveisEntregues > 0) { 
        moverImovel(); // mover
      }
      break;
    case 3:
      if (tamanhoImoveisDisponiveis > 0) {
        removerImovel(); // remover
      }
      break;
    default:
      break;
    }
    pthread_mutex_unlock(&corretor);
    sleep((rand() % (5 - 2 + 1))); // espera uma quantidade aleatoria de tempo
  }
  

  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t t[NUM_THREADS];
    // imoveis
    Imovel disponivel1 = {1, "Rua 1", 1500, "Trindade"};
    Imovel disponivel2 = {2, "Rua 2", 45200, "Centro"};
    Imovel disponivel3 = {3, "Rua 3", 1000, "Itacorubi"};
    Imovel disponivel4 = {4, "Rua 6", 1540, "Trindade"};
    Imovel disponivel5 = {5, "Rua 6", 4200, "Centro"};
    Imovel disponivel6 = {6, "Rua 4", 1990, "Itacorubi"};
    
    Imovel entregues1 = {7, "Rua 4", 10000, "Centro"};
    Imovel entregues2 = {8, "Rua 5", 1700, "Pantanal"};

    insertDisponiveis(disponivel1);
    insertDisponiveis(disponivel2);
    insertDisponiveis(disponivel3);
    insertDisponiveis(disponivel4);
    insertDisponiveis(disponivel5);
    insertDisponiveis(disponivel6);
    insertEntregues(entregues1);
    insertEntregues(entregues2);

    srand(time(NULL));

    // // chamada das threads
    for(int i=0; i < NUM_THREADS; i+=2) {
      pthread_create(&t[i], NULL, t_corretor, NULL);
      pthread_create(&t[i+1], NULL, t_inquilino, NULL);
    }
    
    // // garante o termino de todas threads
    for (int i=0; i < NUM_THREADS; i++) {
      pthread_join(t[i], NULL);
    }

    
    return 0;
};