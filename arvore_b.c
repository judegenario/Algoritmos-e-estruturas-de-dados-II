#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore_b.h"

/*funcao usada na main que chama as outras funcoes de
 busca*/
void buscarRegistro(FILE *arq_dados, ARVORE * arv, tipoChave chave){
	PAGINA p;
	int rrn_resultado;
	int pos_resultado;
	/*faz a busca na arvore*/
	char res = buscaArvore(arv, arv->raiz, chave, &rrn_resultado, &pos_resultado, &p);

	/*se encontrou*/
	if(res){
		/*le o aluno do arquivo de dados e o imprime*/
		tipoAluno aluno;
		fseek(arq_dados, p.pares[pos_resultado].rrn*sizeof(tipoAluno), 0);
		fread(&aluno, sizeof(tipoAluno), 1, arq_dados);
		imprimeRegistro(aluno);
	}else
		printf("Esse registro não foi encontrado\n");
	
}

/*funcao para buscar uma dada chave na pagina passada. a busca eh feita atraves de uma busca binaria eh retornado um inteiro que representa a posicao onde a chave esta, se a chave for encontrada, ou a posicao onde ela deveria estar*/
int buscaPagina(PAGINA *p, tipoChave chave) {
	int inicio, meio, fim;
	inicio = 0;
  	fim = p->contador - 1;
  	meio = (inicio+fim)/2;

	while (inicio <= fim) {
		if (p->pares[meio].chave < chave)
			inicio = meio + 1;
		else if (p->pares[meio].chave == chave)
			return meio;
		else
			fim = meio - 1;

		meio = (inicio + fim)/2;
	}

	return inicio;
}

/*funcao para buscar  recursivamente uma dada chave
 em uma arvore. retorna 1 se encontrou e 0 caso 
 contrario. os 3 ultimos parametros, passados por 
 referencia sao atualizados com a pagina onde o 
 registro esta, o rrn dessa pagina e a sua posicao 
 dentro do vetor de pares, ou entao os mesmos dados 
 sobre onde o registro deveria estar se ele nao for 
 encontrado*/
char buscaArvore(ARVORE *arv, int rrn_atual, tipoChave chave, int *rrn_encontrado, int *pos_encontrada, PAGINA * p){
	if (rrn_atual == -1)
		return 0; /*chave de busca não encontrada*/
	else{
		int pos;
		/*le a pagina onde sera feita a busca*/
		fseek(arv->arq, rrn_atual*sizeof(PAGINA), 0);
		fread(p, sizeof(PAGINA), 1, arv->arq);
		/*faz a busca na pagina lida e atualiza os valores*/
		pos = buscaPagina(p, chave);
		*rrn_encontrado = rrn_atual; 
		*pos_encontrada = pos;

		if (p->pares[pos].chave == chave) 
			return 1; /*a chave foi encontrada*/
		else
			/*se a chave procurada for diferente da
			atual procurar em seu  filho apropriado*/
			return buscaArvore(arv, p->filhos[pos], chave, rrn_encontrado, pos_encontrada, p);
	} 
} 

/*funcao para inserir um registro o escrevendo no 
arquivo de dados e chamando a funcao que escreve no 
arquivo de indice*/
void inserirRegistro(FILE *arq_dados, ARVORE *arv, int *qtd, tipoAluno reg){
	PAGINA p;
	int rrn_resultado = -1;
	int pos_resultado;
	/*faz uma busca na arvore*/
	char res = buscaArvore(arv, arv->raiz, reg.numUSP, &rrn_resultado, &pos_resultado, &p);

	/*se o registro ainda nao existe*/
	if(!res){
		/*cria um par (chave+rrn) para o registro*/
		registro novo_par;
		novo_par.chave = reg.numUSP;
		novo_par.rrn = *qtd;
		/*escreve o novo registro no arquivo de dados
		e adiciona seu indice*/
		fseek(arq_dados, 0, SEEK_END);
		fwrite(&reg, sizeof(tipoAluno), 1, arq_dados);
		(*qtd)++;
		inserirArvore(arv, novo_par, &p, rrn_resultado, pos_resultado, -1, -1, 0);
	}else
		printf("Um registro com esse número já existe\n");
}

/*funcao auxiliar usada no debug que imprime os dados
 de uma pagina*/
void imprimePagina(PAGINA *p){
	int i;
	printf("contador=%d\npares:\n", p->contador);
	for(i=0;i<p->contador;i++)
		printf("%d %d\n", p->pares[i].chave, p->pares[i].rrn);
	printf("filhos:");
	for(i=0;i<p->contador+1;i++)
		printf("%d ", p->filhos[i]);
	printf("pai:%ld\n", p->pai);

	printf("\n");
}

/*imprime o registro passado*/
void imprimeRegistro(tipoAluno reg){
	printf("N USP: %d\n", reg.numUSP);
	printf("Nome: %s\n", reg.nome);
	printf("Sobrenome: %s\n", reg.sobrenome);
	printf("Curso: %s\n", reg.curso);
	printf("Nota: %.2f\n", reg.nota);
}

/*funcao para anular um  vetor usada na criacao de 
 uma nova pagina*/
void anularFilhos(int *vet){
	int i;
	for(i=0;i<ORDEM;i++)
		vet[i] = -1;
}

/*funcao para inserir um registro na arvore, verifica
 se havera overflow e faz o tratamento caso precise*/
void inserirArvore(ARVORE *arv, registro novo_par, PAGINA * p, long rrn, int pos, int filho_dir, int filho_esq, int propagacao){
	/*nao houve overflow*/
	if (p->contador < ORDEM-1){
		inserirPagina(arv,novo_par, p, rrn, pos, filho_dir, filho_esq);
	}else{ /*houve overflow*/
		int i, j;
		PAGINA nova_pagina;
		int rrn_nova_pagina;
		/*calcula qual sera a chave que sera promovida
		  que esta em reg_meio*/
		int meio = ORDEM/2;
		registro reg_meio;
		if(pos == meio)
			reg_meio = novo_par;
		else if(pos>meio){
			reg_meio = p->pares[meio];
			p->contador--;
		}else
			reg_meio = p->pares[meio-1];
		
		/*atraves do tamanho do arquivo calcula o rrn 
		da nova pagina*/
		fseek(arv->arq, 0, SEEK_END);
		rrn_nova_pagina = ftell(arv->arq)/sizeof(PAGINA) + propagacao; /*como a atualizacao 
		do arquivo eh feita apos a chamada recursiva,
		o parametro propagacao eh usado para saber o 
		rrn no caso de propagacao de overflow*/

		/*atualizar dados da nova pagina*/
		nova_pagina.pai = p->pai;
		nova_pagina.contador = (ORDEM-1)/2;
		anularFilhos(nova_pagina.filhos);

		/*copiar os registros da metade maior para 
		a nova pagina*/
		for(i=meio, j=0;i<ORDEM-1;i++){
			/*como o novo registro nao esta inserido
			 ainda eh preciso verificar se ele esta
			 na metade maior*/
			if((pos == i && i != meio)){
				nova_pagina.pares[j] = novo_par;
				nova_pagina.filhos[j] = filho_dir;
				nova_pagina.filhos[++j] = filho_esq;
			}
			else if((i==meio && pos<=meio) || i != meio){
				nova_pagina.pares[j] = p->pares[i];
				nova_pagina.filhos[j] = p->filhos[i];
				nova_pagina.filhos[++j] = p->filhos[i+1];
				p->contador--;
			}
		}

		/*caso o registro novo seja incluido no final*/
		if(pos == ORDEM-1){
			nova_pagina.pares[j] = novo_par;
			nova_pagina.filhos[j] = filho_dir;
			nova_pagina.filhos[j+1] = filho_esq;
		}

		/*caso o registro novo seja incluido na metade menor, inserir ele na pagina atual*/
		if(pos < meio){
			memcpy(&(p->pares[pos+1]), &(p->pares[pos]), (p->contador-pos)*sizeof(registro));
			memcpy(&(p->filhos[pos+2]), &(p->filhos[pos+1]), (p->contador-pos)*sizeof(long int));
			p->pares[pos] = novo_par;
			p->filhos[pos] = filho_dir;
			p->filhos[pos+1] = filho_esq;
		}

		/*se o overflow for da raiz tem que criar uma
		 nova raiz*/
		if(p->pai == -1){
			/*atualiza dados da nova raiz*/
			PAGINA nova_raiz;
			int rrn_nova_raiz;
			nova_raiz.contador = 1;
			nova_raiz.pai = -1;
			nova_raiz.pares[0] = reg_meio;
			anularFilhos(nova_raiz.filhos);
			nova_raiz.filhos[0] = rrn;
			nova_raiz.filhos[1] = rrn_nova_pagina;
			rrn_nova_raiz = rrn_nova_pagina+1;

			/*atualiza pais das pagina ja criadas e
			 atualiza a arvore*/
			nova_pagina.pai  = rrn_nova_raiz;
			p->pai  = rrn_nova_raiz;
			arv->raiz = rrn_nova_raiz;
			fseek(arv->arq, 0, 0);
			fwrite(&rrn_nova_raiz, sizeof(int), 1, arv->arq);

			/*escreve a nova raiz no arquivo*/
			fseek(arv->arq, rrn_nova_raiz*sizeof(PAGINA), 0);
			fwrite(&nova_raiz, sizeof(PAGINA), 1, arv->arq);
		}else{ /*tem pai e sera feita a promocao*/
			PAGINA pai;
			int nova_pos;

			/*le o no pai do arquivo*/
			fseek(arv->arq, p->pai*sizeof(PAGINA), 0);
			fread(&pai, sizeof(PAGINA), 1, arv->arq);
			/*calcula a posicao na nova pagina*/
			nova_pos = buscaPagina(&pai, reg_meio.chave);
			/*chamada recursiva para inserir o registro na pagina pai*/
			inserirArvore(arv, reg_meio, &pai, p->pai, nova_pos, rrn, rrn_nova_pagina, propagacao+1);
		}

		/*no fim atualiza as paginas que foram criadas/atualizadas*/
		fseek(arv->arq, rrn*sizeof(PAGINA), 0);
		fwrite(p, sizeof(PAGINA), 1, arv->arq);
		fseek(arv->arq, rrn_nova_pagina*sizeof(PAGINA), 0);
		fwrite(&nova_pagina, sizeof(PAGINA), 1, arv->arq);
	}

}

/*funcao para inserir um registro em uma pagina 
sabendo que nao dara overflow*/
void inserirPagina(ARVORE *arv, registro novo_par, PAGINA * p, int rrn, int pos, int filho_dir, int filho_esq){
	/*passa os registros pra frente*/
	if(pos != p->contador){
		memcpy(&(p->pares[pos+1]), &(p->pares[pos]), (p->contador-pos)*sizeof(registro));
		memcpy(&(p->filhos[pos+2]), &(p->filhos[pos+1]), (p->contador-pos)*sizeof(long int));
	}

	/*insere o novo registro*/
	p->pares[pos] = novo_par;
	p->filhos[pos] = filho_dir;
	p->filhos[pos+1] = filho_esq;
	p->contador++;

	/*atualiza a pagina no arquivo*/
	fseek(arv->arq, rrn*sizeof(PAGINA), 0);
	fwrite(p, sizeof(PAGINA), 1, arv->arq);
}

/*funcao para o povoamento inicial da arvore. o numero usp eh gerado sequencialmente, a nota aleatoriamente e os campos textuais sao formados pelo nome do campo concatenado ao numero usp para facilitar a verificacao na busca. sao inseridos nesse povoamento um numero 10 vezes maior que a ordem da arvore, no caso sao gerados 3410 registros*/
void povoamentoInicial(ARVORE* arv, FILE *arq, int *qtd){
	int i;
	tipoAluno reg;
	for(i=1;i<=ORDEM*10;i++){
		reg.numUSP = i;
		reg.nota = rand() % 10;
		char num[TAM];
		char nome[TAM] = "nome";
		char sobrenome[TAM] = "sobrenome";
		char curso[TAM] = "curso";
		sprintf(num, "%d", i);
		strcat(nome, num);
		strcat(sobrenome, num);
		strcat(curso, num);
		strcpy(reg.nome, nome);
		strcpy(reg.sobrenome, sobrenome);
		strcpy(reg.curso, curso);

		inserirRegistro(arq, arv, qtd, reg);
	}

}
