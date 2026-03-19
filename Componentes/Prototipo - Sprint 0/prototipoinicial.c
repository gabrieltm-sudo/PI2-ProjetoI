// Atualmente lê o que está escrito em memoria1.mem e imprime na tela.
// Lê os bits como char.
// Deve implementar a decodificação para os campos opcode, rs, rt e rd (de acordo com o tipo de instrução pego).

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE* arquivo;

typedef struct{
    char mem[17];
} code;

int contaLinhas(char *arq);
void lerArquivo(char *arq, code **memoria, int linhas);

int main(){
    char arq[20];
    printf("Digite o nome do arquivo de memória: ");
    fgets(arq, sizeof(arq), stdin);
    arq[strcspn(arq, "\n")] = '\0';

    int linhas = contaLinhas(arq);
    printf("Número de linhas do arquivo: %d\n", linhas);

    code *memoria = NULL;
    lerArquivo(arq, &memoria, linhas);

    free(memoria);
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
    *memoria = (code *)malloc(linhas * sizeof(code));
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
