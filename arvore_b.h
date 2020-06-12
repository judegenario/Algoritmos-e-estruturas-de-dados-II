#define TAM 20
#define ORDEM 341 /*ordem calculada para que a pagina tivesse tamanho 4096*/

typedef int tipoChave;

/*struct para descrever o registro do aluno*/
typedef struct {
	tipoChave numUSP;
	char nome[TAM];
	char sobrenome[TAM];
	char curso[TAM];
	float nota;
} tipoAluno;

/*struct para descrever o par (chave+rrn) que vai ser usado na arvore*/
typedef struct {
	tipoChave chave;
	int rrn;
} registro;

/*struct para descrever a pagina/no da arvore, bem parecida com a descrita em aula exceto pela adição do campo pai que representa o rrn da pagina pai e eh usado em casos de overflow do no*/
typedef struct pagina {
	int contador; /*conta o numero de chave*/
 	registro pares[ORDEM-1]; /*pares de chaves com seus rrns no arquivo de registro */
	int filhos[ORDEM]; /*rrns das paginas filhas*/
	long pai; /*rrn da pagina pai, -1 caso nao exista, long para que o tamanho da pagina fosse 4096*/
} PAGINA; 

/*struct para  descrever dados da arvore, usada para facilitar*/
typedef struct arvore {
	FILE *arq; /*arquivo de indice que possui a arvore*/
	int raiz; /*rrn do no raiz*/
} ARVORE;

/*funcoes utilizadas para a insercao e busca na arvore que serao melhor descritas no arvore_b.c*/
void povoamentoInicial(ARVORE*, FILE *, int *);
int buscaPagina(PAGINA *, tipoChave);
char buscaArvore(ARVORE *, int, tipoChave, int*, int*, PAGINA*);
void imprimeRegistro(tipoAluno);
void inserirRegistro(FILE *, ARVORE*, int*, tipoAluno);
void buscarRegistro(FILE *, ARVORE*, tipoChave);
void inserirArvore(ARVORE *, registro, PAGINA*, long, int, int, int, int);
void imprimePagina(PAGINA *);
void inserirPagina(ARVORE *, registro, PAGINA *, int, int, int, int);

