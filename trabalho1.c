#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 2

typedef struct Imovel
{
    int codigo;
    char endereco[30];
    float preco;
    char bairro[30];
} Imovel;

struct Node  {
	struct Imovel data;
	struct Node* next;
	struct Node* prev;
};

struct Node* headImoveisEntregues;
struct Node* headImoveisDisponiveis;
struct Node* currentImoveisEntregues;
struct Node* currentImoveisDisponiveis;

typedef struct ConjuntoListas
{
  struct Node* disponiveis;
  struct Node* entregues ;
} ConjuntoListas;


struct Node* GetNewNode(struct Imovel imv) {
	struct Node* newNode
		= (struct Node*)malloc(sizeof(struct Node));
	newNode->data = imv;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
};

void InsertAtHead(struct Node *head,struct Imovel imv) {
	struct Node* newNode = GetNewNode(imv);
	if(head == NULL) {
		head = newNode;
		return;
	}
	head->prev = newNode;
	newNode->next = head;
	head = newNode;
};

void InsertAtTail(struct Node *head,struct Imovel imv) {
	struct Node* temp = head;
	struct Node* newNode = GetNewNode(imv);
	if(head == NULL) {
		head = newNode;
		return;
	}
	while(temp->next != NULL) temp = temp->next; // Go To last Node
	temp->next = newNode;
	newNode->prev = temp;
};

void irParaPrimeiro(struct Node* current, struct Node* head) {
    struct Node* novoCurrent = head;
    current = novoCurrent;
}

void irParaUltimo(struct Node* current, struct Node* head) {
  struct Node* temp = head;
  while(temp->next != NULL) temp = temp->next;
  struct Node* tail = temp;
  current = tail;
}

void avancarNposicoes(struct Node* current, struct Node* head, int n){
    irParaPrimeiro(current,head);
    for(int i = 1; i < n; i++){
        if (&head->next == NULL){
            break;
        } else {
            current = current->next;
        }
    }
    printf(" +++++ %s +++++++ \n ", current->data.endereco);
}

void delete(struct Node* head, struct Imovel delVal)
{
  struct Node* temp = head;
  struct Node* previous = (struct Node*) malloc(sizeof(struct Node));

  //Case when there is only 1 node in the list

  if(temp->next == NULL)
  {
    head = NULL;
    free(temp);
    printf("Value %d, deleted \n",delVal.codigo);
    return;

  }
  //if the head node itself needs to be deleted
  if(temp!=NULL && temp->data.codigo==delVal.codigo)
  {
    //Case 1 head becomes 30
    head = temp->next; //changing head to next in the list
    head->prev = NULL; //assing new head's previous value to NULL

    //case 1: 22 deleted and freed
    free(temp);
    printf("Value %d, deleted \n",delVal.codigo);

    return;
  }

  //run until we find the value to be deleted in the list
  while (temp != NULL && (*temp).data.codigo != delVal.codigo)
  {
    //store previous link node as we need to change its next val
    previous = temp;
    temp = temp->next;
  }

  //if value is not present then
  //temp will have traversed to last node NULL
  if(temp==NULL)
  {
    printf("Value not found\n");
    return;
  }

  // Case 2: (24)->next = 16 (as 20->next = 16)
  // Case 3: (16)->next = NULL (as 12->next = NULL)
  previous->next = temp->next;

  //If the last node is to be deleted
  if(temp->next == NULL)
  {
    // Case 3: Just need to free the memory
    printf("Value %d, deleted \n",delVal.codigo);
    free(temp);
    return;
  }
}

void *t_inquilino(void *arg)
{

    int *tempo = (rand() % (5 - 2 + 1)) + 2;
    pthread_mutex_lock(&inquilino);
    alugar(NULL);
    pthread_mutex_unlock(&inquilino);
    sleep(*tempo);
    pthread_mutex_lock(&inquilino);
    devolver(NULL);
    pthread_mutex_unlock(&inquilino);

    pthread_exit(NULL);
    // lista
    
};

void *t_corretor(void *arg)
{
  // // lista
  ConjuntoListas *listas;
  listas = (ConjuntoListas *)arg;

  struct Node *currentImoveisDisponiveis;
  currentImoveisDisponiveis = listas->disponiveis;

  struct Node *currentImoveisEntregues;
  currentImoveisEntregues = listas->entregues;

  pthread_mutex_lock(&corretor);

  for (int i = 0; i < 3; i++)
  {
      int opcao = (rand() % (3 - 2 + 1)) + 2;
      Imovel *ultimoImovelDisponivel = getUltimoElementoLista(currentImoveisDisponiveis);
      switch (opcao)
      {
      // case 1:
      //     adicionarImovel(currentImoveisDisponiveis, ImovelAleatorio);
      //     exibirElementosLista(currentImoveisDisponiveis);
      //     break;
      case 2: // tem que ver pq nao tem Imovel em entregues
          moverImovel(currentImoveisDisponiveis, currentImoveisEntregues, ultimoImovelDisponivel);
          printf("Imovel %d foi movido\n", ultimoImovelDisponivel->codigo);
          break;
      case 3:
          removerImovel(currentImoveisDisponiveis, ultimoImovelDisponivel);
          printf("Imovel %d foi removido\n", ultimoImovelDisponivel->codigo);
          break;
      default:
          break;
      }
  }

  sleep(10); //TODO fazer ser random
  pthread_mutex_unlock(&corretor);

  pthread_exit(NULL);
}

void Print(struct Node* head) {
	struct Node* temp = head;
	printf("Forward: ");
	while(temp != NULL) {
		printf("Imovel[codigo: %d , endereco: %s];",temp->data.codigo, temp->data.endereco);
		temp = temp->next;
	}
	printf("\n");
}

pthread_mutex_t inquilino;
pthread_mutex_t corretor;

int main(int argc, char *argv[])
{
    pthread_t t[NUM_THREADS];
    struct Node *imoveisDisponiveis = (struct Node *)malloc(sizeof(struct Node));
    struct Node *imoveisEntregues = (struct Node *)malloc(sizeof(struct Node));

    // imoveis
    Imovel casaSobrado = {123, "Rua Dois", 1500, "Trindade"};
    Imovel laje = {456, "Rua Tres", 45200, "Centro"};
    Imovel predio = {789, "Rua Trajano", 1000, "Itacorubi"};
    Imovel casa = {555, "Av. Beira Mar", 1000000, "Centro"};
    Imovel kitnet = {47, "R. EduVieira", 1700, "Pantanal"};

    headImoveisDisponiveis = GetNewNode(casaSobrado);
    printf("inseriu head");
    InsertAtTail(headImoveisDisponiveis,laje );
    printf("inseriu laje");
    InsertAtTail(headImoveisDisponiveis,predio);
    printf("inseriu predio ");
    delete(headImoveisDisponiveis, predio);
    printf("removeu predio ");
    Print(headImoveisDisponiveis);

    ConjuntoListas *conjuntoListas = (ConjuntoListas *)malloc(sizeof(ConjuntoListas));
    conjuntoListas->disponiveis = headImoveisDisponiveis;
    conjuntoListas->entregues = headImoveisEntregues;

    // chamada das threads
    pthread_create(&t[0], NULL, t_corretor, (void *)conjuntoListas);
    pthread_create(&t[1], NULL, t_inquilino, (void *)conjuntoListas);

    // garante o termino de todas threads
    pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    return 0;
};