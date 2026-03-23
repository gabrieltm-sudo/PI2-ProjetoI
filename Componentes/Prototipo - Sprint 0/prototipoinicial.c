// Decodificação em andamento (Gabriel);


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

FILE* arquivo;

enum inst{
    tipoI, tipoJ, tipoR
};

typedef struct {
    char mem[17];
    enum inst tipoInst;
    uint16_t instrucao;
    uint16_t opcode;
    uint16_t rs;
    uint16_t rt;
    uint16_t rd;
    uint16_t funct;
    int16_t imm;
    uint16_t addr;
} instrucao;

/*Instruções:
Tipo R:
opcode: 4 bits (0000);
rs: 3 bits;
rt: 3 bits;
rd: 3 bits;
funct: 3 bits

Tipo I:
opcode: 4 bits;
rs (base/registrador fonte): 3 bits;
rt (destino/registrador fonte): 3 bits;
imediato: 6 bits (estendido);

Tipo J:
opcode: 4 bits;
End: 8 bits;
*/


int contaLinhas(char *arq);
void lerArquivo(char *arq, instrucao **memoria, int linhas);
void programCounter(instrucao *memoria, int linhas, int esc);
void decodificaInst(instrucao *instrucao);

int main(){

    int esc;

    char arq[20];
    printf("Digite o nome do arquivo de memória: ");
    fgets(arq, sizeof(arq), stdin);
    arq[strcspn(arq, "\n")] = '\0';

    int linhas = contaLinhas(arq);
    printf("Número de linhas do arquivo: %d\n", linhas);

    instrucao *memoria = NULL;
    lerArquivo(arq, &memoria, linhas);

    printf("\n\n0 - Rodar automaticamente\n1 - Rodar passo a passo\n");
    scanf("%d", &esc);
    getchar();

    programCounter(memoria, linhas, esc);
    return 0;
}

int contaLinhas(char *arq){      // Analisar se é necessário - Atualmente é redundante.
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

void lerArquivo(char *arq, instrucao **memoria, int linhas){
    *memoria = (instrucao *)malloc(256 * sizeof(instrucao));
    arquivo = fopen(arq, "r");
    int i=0;
    char mem[17];
    
    if(arquivo==NULL){
        printf("Permissão negada!");
        return;
    }
    while(i < linhas && fscanf(arquivo, "%16s", mem) != EOF){
        
        strcpy((*memoria)[i].mem, mem);
        printf("%dª memória: %s\n", i+1, (*memoria)[i].mem);
        
        (*memoria)[i].instrucao = strtoul(mem, NULL, 2);    // Transforma a string em uint16_t
        i++;
    }
    
    fclose(arquivo);
}

void programCounter(instrucao *memoria, int linhas, int esc){

    int pc = 0;

    while(pc < linhas){
        printf("\nPC = %d | Memória = %s\n", pc, memoria[pc].mem);
        decodificaInst(&memoria[pc]);

        if(esc == 1){
            printf("Pressione enter para o próximo passo\n\n");
            getchar();
        }
        pc++;
    }
}

void decodificaInst(instrucao *instrucao){

    (*instrucao).opcode = (*instrucao).instrucao >> 12;

    printf("opcode: %d\n", (*instrucao).opcode);



}