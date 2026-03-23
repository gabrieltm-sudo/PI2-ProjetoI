// Decodificação em andamento (Gabriel);
// Editar parâmetros dos protótipos conforme criação da função

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

//************ Editar parâmetros conforme criação da função **************
void imprimeBancoRegistradores();
void imprimeSimulador();
void salvaASM();
void salvaDAT();
void executaPrograma();
void executaInstrucao();
void voltaInstrucao();

// Componentes
int buscaInstrucao();
void decodificaInstrucao();
void executaInstrucaoR();
void executaInstrucaoI();
void executaInstrucaoJ();
void ULA();
void atualizaPC();

int main(){

    int esc;
    int opcao;

    while (1) {
        printf("\nMenu:\n\n");
        printf("1. Carregar Memória de Instruções (.mem)\n");
        printf("2. Carregar Memória de Dados (.dat)\n");
        printf("3. Imprimir Banco de Registradores\n");
        printf("4. Imprimir todo o Simulador\n");
        printf("5. Salvar .asm\n");
        printf("6. Salvar .dat\n");
        printf("7. Executa programa (run)\n");
        printf("8. Executa uma instrução (step)\n");
        printf("9. Volta uma instrução (back)\n");
        printf("0. Sair\n\n");
        printf("Digite uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                //Carregar Mem de Instr.
                char arq[20];
                printf("Digite o nome do arquivo de memória: ");
                fgets(arq, sizeof(arq), stdin);
                arq[strcspn(arq, "\n")] = '\0';

                int linhas = contaLinhas(arq);
                printf("Número de linhas do arquivo: %d\n", linhas);

                instrucao *memoria = NULL;
                lerArquivo(arq, &memoria, linhas);
                break;
            case 2:
                //Carregar Mem de Dados
                break;
            case 3:
                //Imprimir Banco de Registradores
                break;

            case 4:
                //Imprimir simulador
                break;
            case 5:
                // Salvar .asm
                break;
            case 6:
                // Salvar .dat
                break;
            case 7:
                //Executar programa (run)
                esc = 0;
                programCounter(memoria, linhas, esc);
                break;
            case 8:
                //Executa instrução (step)
                esc = 1;
                programCounter(memoria, linhas, esc);
                break;
            case 9:
                //Voltar instrução (back)
                break;
            case 0:
                //Sair
                return 0;
            default:
                printf("Opção inválida!\n");
        }
    }
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
