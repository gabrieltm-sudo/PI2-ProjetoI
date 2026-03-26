#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

FILE* arquivo;

// struct de sinais
typedef struct{
    uint8_t branch;
    uint8_t jump;
    uint8_t IncPC;
    uint8_t RegDst;
    uint8_t UlaFonte;
    uint8_t MemParaReg;
    uint8_t EscReg;
    uint8_t EscMem;
    uint8_t ulaOp;
}sinaisUC;

sinaisUC sinais;

enum inst{
    tipoI, tipoJ, tipoR
};

typedef struct {
    char mem[17];
    enum inst tipoInst;
    uint16_t instrucao;
    uint8_t opcode;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t funct;
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

// ------------------------------ PROTÓTIPOS -------------------------------

// MENU / CONTROLE DO SISTEMA
void imprimeSimulador(); // (não implementado)
void salvaASM();         // (não implementado)
void salvaDAT();         // (não implementado)
void executaPrograma();  // (não implementado)
void executaInstrucao(); // (não implementado - execução ainda não faz parte)
void voltaInstrucao();   // (não implementado)


// MEMÓRIA
int contaLinhas(char *arq);
void lerMem(char *arq, instrucao **memoria, int linhas);


// PROGRAM COUNTER (PC) / BUSCA
void programCounter(instrucao *memoria, int linhas, int esc, int *bReg);


// DECODIFICAÇÃO
void decodificaInst(instrucao *instrucao);


// UNIDADE DE CONTROLE (UC)
void unidadeControle(instrucao *instrucao, sinaisUC *sinais);


// BANCO DE REGISTRADORES (BREG)
int *inicializaBReg();
void lerRegistradores(int *reg, int rs, int rt, int *valRs, int *valRt);
void escreveRegistrador(int *reg, int rd, int valor, int EscReg);
void imprimeBancoRegistradores(int *reg);


// EXECUÇÃO (AINDA NÃO IMPLEMENTADA)
void executaInstrucaoR(); // (não implementado)
void executaInstrucaoI(); // (não implementado)
void executaInstrucaoJ(); // (não implementado)


// ULA (UNIDADE LÓGICA E ARITMÉTICA)
void ULA();               // (não implementado)
void memDados();          // (não implementado)

// -------------------------------------------------------------------------

int main(){

    int esc;
    int opcao;
    instrucao *memoria = NULL;
    int linhas=0;

    int *bReg = inicializaBReg();

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
                printf("\nDigite o nome do arquivo de memória: ");

                fgets(arq, sizeof(arq), stdin);
                arq[strcspn(arq, "\n")] = '\0';

                linhas = contaLinhas(arq);
                printf("\nNúmero de linhas do arquivo: %d\n", linhas);

                lerMem(arq, &memoria, linhas);
                break;

            case 2:
                //Carregar Mem de Dados
                break;

            case 3:
                //Imprimir Banco de Registradores
                imprimeBancoRegistradores(bReg);
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
                programCounter(memoria, linhas, esc, bReg);
                break;

            case 8:
                //Executa instrução (step)
                esc = 1;
                programCounter(memoria, linhas, esc, bReg);
                break;

            case 9:
                //Voltar instrução (back)
                break;

            case 0:
                //Sair
                free(bReg);
                free(memoria);
                return 0;

            default:
                printf("\nOpção inválida!\n");
                break;
        }
    }

    return 0;
}

int contaLinhas(char *arq){      // Analisar se é necessário - Atualmente é redundante.
    arquivo = fopen(arq, "r");
    char ch;
    int count=0;

    if(arquivo==NULL){
        printf("\nAcesso negado!\n");
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

// Leitura da memória
void lerMem(char *arq, instrucao **memoria, int linhas){
    *memoria = (instrucao *)malloc(256 * sizeof(instrucao));
    arquivo = fopen(arq, "r");
    int i=0;
    char mem[17];
    
    if(arquivo==NULL){
        printf("\nPermissão negada!");
        return;
    }

    printf("\nMemória de Instruções\n\n");

    while(i < linhas && fscanf(arquivo, "%16s", mem) != EOF){
        
        strcpy((*memoria)[i].mem, mem);
        printf("%dª memória: %s\n", i+1, (*memoria)[i].mem);
        
        (*memoria)[i].instrucao = strtoul(mem, NULL, 2);    // Transforma a string em uint16_t
        i++;
    }
    
    fclose(arquivo);
}

// PC
void programCounter(instrucao *memoria, int linhas, int esc, int *bReg){

    int pc = 0;

    while(pc < linhas){ // Clock

        printf("\nPC = %d | Memória = %s\n", pc, memoria[pc].mem);

        decodificaInst(&memoria[pc]);
        unidadeControle(&memoria[pc], &sinais);

        if(esc == 1){
            printf("Pressione enter para o próximo passo\n\n");

            getchar();
        }

        pc++;
    }
}

// Decodificação
void decodificaInst(instrucao *instrucao){

    printf("Instrução: [ %d ]\n", (*instrucao).instrucao);
    (*instrucao).opcode = (*instrucao).instrucao >> 12; // Pega os 4 bits do opcode

    printf("opcode: %d\n", (*instrucao).opcode);

    switch((*instrucao).opcode){
    case 0:
        (*instrucao).tipoInst = tipoR;
        (*instrucao).rs = ((*instrucao).instrucao >> 9) & 0x7; // pega os 3 bits do rs (desloca 6 bits para a direita e pega os 3 mais significativos que ficaram)
        (*instrucao).rt = ((*instrucao).instrucao >> 6) & 0x7; // pega os 3 bits do rt
        (*instrucao).rd = ((*instrucao).instrucao >> 3) & 0x7; // pega os 3 bits do rd
        (*instrucao).funct = ((*instrucao).instrucao) & 0x7;
        printf("[ Tipo R ] \n");
        printf("rs: %d\n", (*instrucao).rs);
        printf("rt: %d\n", (*instrucao).rt);
        printf("rd: %d\n", (*instrucao).rd);
        printf("funct: %d\n", (*instrucao).funct);
        break;

    case 2:
        (*instrucao).tipoInst = tipoJ;
        (*instrucao).addr = ((*instrucao).instrucao) &0xFF; // pega os 8 bits do adress
        printf("[ Tipo J ]\n");
        printf("adress: %d\n", (*instrucao).addr);
        break;

    default:
        (*instrucao).tipoInst = tipoI;
        (*instrucao).rs = ((*instrucao).instrucao >> 9) &0x7; // pega os 3 bits do rs
        (*instrucao).rt = ((*instrucao).instrucao >> 6) &0x7; // pega os 3 bits do rt
        (*instrucao).imm = ((*instrucao).instrucao) &0x3F; // pega os 6 bits do imediato (deve passar por um extensor antes da ULA)
        printf("[ Tipo I ] \n");
        printf("rs: %d\n", (*instrucao).rs);
        printf("rt: %d\n", (*instrucao).rt);
        printf("imediato: %d\n", (*instrucao).imm);
    }
}

//UC
void unidadeControle(instrucao *instrucao, sinaisUC *sinais){
    // Testar
    switch((*instrucao).opcode){
        case 0: // opcode = 0000
            (*sinais).RegDst = 1;
            (*sinais).EscReg = 1;
            (*sinais).UlaFonte = 0;
            (*sinais).ulaOp = (*instrucao).funct;
            (*sinais).EscMem = 0;
            (*sinais).MemParaReg = 1;
            (*sinais).jump = 0;
            (*sinais).branch = 0;

            if((*sinais).ulaOp==0){
                printf("Add $%d, $%d, $%d\n", (*instrucao).rd, (*instrucao).rs, (*instrucao).rt);
                // chama ULA e o que mais for necessário.
            }
            else if((*sinais).ulaOp==2){
                printf("Sub $%d, $%d, $%d", (*instrucao).rd, (*instrucao).rs, (*instrucao).rt);
                // chama ULA e o que mais for necessário.
            }
            break;

        case 2:
            (*sinais).jump = 1;
            printf("\nlógica jump\n");
            break;

        case 4: // opcode = 0100 - Addi
            (*sinais).RegDst = 0;
            (*sinais).EscReg = 1;
            (*sinais).UlaFonte = 1;
            (*sinais).ulaOp = 1;
            (*sinais).EscMem = 0;
            (*sinais).MemParaReg = 1;
            (*sinais).jump = 0;
            (*sinais).branch = 0;

            printf("\nAddi $%d, $%d, %d\n", (*instrucao).rs, (*instrucao).rt, (*instrucao).imm);
            break;

        case 8: // opcode = 1000 - BEQ
            (*sinais).RegDst = 0;
            (*sinais).EscReg = 0;
            (*sinais).UlaFonte = 0;
            (*sinais).ulaOp = 6; // 110 (?) - verificar
            (*sinais).EscMem = 0;
            (*sinais).MemParaReg = 0;
            (*sinais).jump = 0;
            (*sinais).branch = 1; // verificar também

            printf("\nbeq $%d, $%d, %d\n", (*instrucao).rs, (*instrucao).rt, (*instrucao).imm);

            break;

        case 11: // opcode = 1011 - lw
            (*sinais).RegDst = 0;
            (*sinais).EscReg = 1;
            (*sinais).UlaFonte = 1;
            (*sinais).ulaOp = 3;
            (*sinais).EscMem = 0;
            (*sinais).MemParaReg = 1;
            (*sinais).jump = 0;
            (*sinais).branch = 0;
            
            printf("\nlw $%d, $%d, %d\n", (*instrucao).rs, (*instrucao).rs, (*instrucao).imm);

            break;

        case 15: // opcode = 1111 - sw
            (*sinais).RegDst = 0;
            (*sinais).EscReg = 0;
            (*sinais).UlaFonte = 1;
            (*sinais).ulaOp = 5; // (?)
            (*sinais).EscMem = 1;
            (*sinais).MemParaReg = 0;
            (*sinais).jump = 0;
            (*sinais).branch = 0;

            printf("\nsw $%d, $%d, %d\n", (*instrucao).rs, (*instrucao).rt, (*instrucao).imm);

            break;
    }

}

int *inicializaBReg(){
    int *reg=NULL;

    reg = (int*)malloc(8*sizeof(int));

    for(int i=0;i<8;i++){
        reg[i]=0;
    }

    return reg;
}

void lerRegistradores(int *reg, int rs, int rt, int *valRs, int *valRt){
    *valRs = reg[rs];
    *valRt = reg[rt];
}

void escreveRegistrador(int *reg, int rd, int valor, int EscReg){
    if(EscReg){
        reg[rd] = valor;
    }
}

void imprimeBancoRegistradores(int *reg){
    printf("\nBanco de Registradores:\n\n");

    for(int i=0;i<8;i++){
        printf("Registrador %d: %d\n",i, reg[i]);
    }
}
