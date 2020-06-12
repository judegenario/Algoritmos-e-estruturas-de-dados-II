/*
	Trabalho 1 - Implementacao de Arvore B

	Juliana Campos Degenario Ribeiro - 11275112
	Leila Gomes Ferreira - 11218581

	Nesse trabalho foram implementadas a insercao
	e busca em uma arvore-b
	A implementacao escolhida por nos tem uma 
	abordagem similar a que foi passada em aula
	tendo sido  adicionado apenas um campo pai
	a estrutura da pagina para ser utilizada em
	casos de overflow.
	
*/
#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"

void criaArquivo(FILE *arq, char *nome);
void finalizaExecucao(FILE *arq_dados, FILE *arq_ind);

int main(void) {
  	FILE *arq_dados;
	int comando= 0;
	int qtd_reg = 0;
	char foi_criado = 0;
	ARVORE arv;
	arv.raiz = 1;

	do {
	    arq_dados= fopen("alunos.dad", "r+");
	    if (arq_dados== NULL){
			criaArquivo(arq_dados, "alunos.dad");
			foi_criado = 1;	
		}	
	} while(arq_dados== NULL);

	do {
	    arv.arq = fopen("indice.dad", "r+");
	    if (arv.arq== NULL) criaArquivo(arv.arq, "indice.dad");
	} while(arv.arq== NULL);

	/*caso o arquivo tenha sido criado agora, portanto a arvore nao foi criado ainda, criar a pagina raiz e, em seguida fazer o povoamento inicial*/
	if(foi_criado){
		/*inicializar raiz*/
		PAGINA raiz;
		raiz.contador = 0;
		raiz.pai = -1;
		int i;
		for(i=0;i<ORDEM;i++)
			raiz.filhos[i] = -1;
		/*escrever raiz no arquivo de indice*/
		fwrite(&arv.raiz, sizeof(int), 1, arv.arq);
		fseek(arv.arq, sizeof(PAGINA), 0);
		fwrite(&raiz, sizeof(PAGINA), 1, arv.arq);
		
		printf("Fazendo povoamento inicial..\n");
		povoamentoInicial(&arv, arq_dados, &qtd_reg);

	}else{
		/*caso o arquivo não tenha sido criado, le do arquivo o rrn da raiz*/
		fread(&arv.raiz, sizeof(int), 1, arv.arq);
	}

	/* qtd_reg recebe a qtd de registros baseado no tamanho do arquivo de dados*/
	fseek(arq_dados, 0, SEEK_END);
	qtd_reg = ftell(arq_dados)/sizeof(tipoAluno);
	fseek(arq_dados, 0, SEEK_SET);
	
	do {
		printf("Operacoes:\n1- Gravar\n2- Pesquisar\n3- Finalizar\n\n");

		scanf("%d", &comando);
		switch(comando){
			case 1: {
				tipoAluno reg;
				printf("Digite o n USP do aluno:");
				scanf("%d", &reg.numUSP);
				printf("Digite o nome do aluno:");
				scanf(" %[^\n]", reg.nome);
				printf("Digite o sobrenome do aluno:");
				scanf(" %[^\n]", reg.sobrenome);
				printf("Digite o curso do aluno:");
				scanf(" %[^\n]", reg.curso);
				printf("Digite a nota do aluno:");
				scanf("%f", &reg.nota);
				inserirRegistro(arq_dados, &arv, &qtd_reg, reg);
				break;
			}
			case 2: {
				tipoChave n_usp;
				printf("Digite o N USP desejado:");
				scanf("%d", &n_usp);	
				buscarRegistro(arq_dados, &arv,n_usp);
				break;
			}
		}

	} while(comando != 3);


	finalizaExecucao(arq_dados, arv.arq);

	return 0;
}

void criaArquivo(FILE *arq, char *nome) {
    if (arq== NULL) {
        arq= fopen(nome, "w");
        fclose(arq);
        arq= NULL;
    }
}

void finalizaExecucao(FILE *arq_dados, FILE *arq_ind){
	fclose(arq_dados);
	fclose(arq_ind);
}
