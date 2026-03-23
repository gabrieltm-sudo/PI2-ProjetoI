// Atualmente lê o que está escrito em memoria1.mem e imprime na tela.
// Lê os bits como char.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE* arquivo;

typedef struct mem{
    char mem[17];
} code;


int contaLinhas(char *arq);
void lerArquivo(char *arq, code **memoria, int linhas);
void programCounter(code *memoria, int linhas, int esc);

int main(){

    int esc;

    char arq[20];
    printf("Digite o nome do arquivo de memória: ");
    fgets(arq, sizeof(arq), stdin);
    arq[strcspn(arq, "\n")] = '\0';

    int linhas = contaLinhas(arq);
    printf("Número de linhas do arquivo: %d\n", linhas);

    code *memoria = NULL;
    lerArquivo(arq, &memoria, linhas);

    printf("\n\n0 - Rodar automaticamente\n1 - Rodar passo a passo\n");
    scanf("%d", &esc);
    getchar();

    programCounter(memoria, linhas, esc);
    return 0;
}

int contaLinhas(char *arq){
    arquivo = fopen(arq, "r");
    char ch;
    int count=0;
    if(arquivo==NULL){
        printf("Acesso negado!\n");
        return 0;
    }
    while((ch=fgetc(arquivo))!=EOF){
        if(ch=='\n'){
            count++;
        }
    }
    fclose(arquivo);
    return count;
}

void lerArquivo(char *arq, code **memoria, int linhas){
    *memoria = (code *)malloc(256 * sizeof(code));
    arquivo = fopen(arq, "r");
    int i=0;
    char mem[17];
    if(arquivo==NULL){
        printf("Permissão negada!");
        return;
    }
    else{
        while(i < linhas && fscanf(arquivo, "%16s", mem) != EOF){
            strcpy((*memoria)[i].mem, mem);
            printf("%dª memória: %s\n", i+1, (*memoria)[i].mem);
            i++;
        }
    }
    fclose(arquivo);
}

void programCounter(code *memoria, int linhas, int esc){

    int pc = 0;

    while(pc < linhas){
        printf("\nPC = %d | Memória = %s\n", pc, memoria[pc].mem);


        if(esc == 1){
            printf("Pressione enter para o próximo passo\n\n");
            getchar();
        }
        pc++;
    }
}