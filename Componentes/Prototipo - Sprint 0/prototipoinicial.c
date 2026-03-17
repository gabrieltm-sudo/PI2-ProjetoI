// Atualmente lê o que está escrito em memoria1.mem e imprime na tela.
// Lê os bits como char.
// Deve implementar a decodificação para os campos opcode, rs, rt e rd (de acordo com o tipo de instrução pego).

#include <stdio.h>
#include <string.h>

FILE* arquivo;
void lerArquivo(char *arq);

typedef struct{
    char mem[17];
} code;

code memoria[12];

int main(){
    char arq[20];

    printf("Digite o nome do arquivo de memória: ");
    fgets(arq, sizeof(arq), stdin);
    arq[strcspn(arq, "\n")] = '\0';

    lerArquivo(arq);

    return 0;
}

void lerArquivo(char *arq){
    arquivo = fopen(arq, "r");
    int i=0;
    char mem[16];
    if(arquivo==NULL){
        printf("Permissão negada!");
        return;
    } else{
        while((fscanf(arquivo, "%s\n", mem) != EOF)){
            strcpy(memoria[i].mem, mem);
            printf("%dª memória: %s\n", i+1, memoria[i].mem);
            i++;
        }
    }
    fclose(arquivo);
}