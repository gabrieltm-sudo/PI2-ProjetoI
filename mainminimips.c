#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

FILE *arquivo, *arquivoMemDados;

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
    int8_t imm;
    uint8_t addr;
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
void voltaInstrucao();   // (não implementado)


// MEMÓRIA
int contaLinhas(char *arq);
void lerMem(char *arq, instrucao **memoria, int linhas);


// PROGRAM COUNTER (PC) / BUSCA
void programCounter(instrucao *memoria, int linhas, int esc, int *bReg, sinaisUC *sinais, int *pc);


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
void executaInstrucao(instrucao *instrucao, sinaisUC *sinais, int *bReg); // falta a memória de dados
int8_t extensorBit(int8_t imm);
void executaInstrucaoR(); // (não implementado)
void executaInstrucaoI(); // (não implementado)
void executaInstrucaoJ(); // (não implementado)


// ULA (UNIDADE LÓGICA E ARITMÉTICA)
int ULA(int op1, int op2, int ulaOp, int *zero);

// MEMÓRIA DE DADOS
int *inicializaMemDados();
int contaMemDados(char *arqMem);
void lerMemDados(char *arqMem, int linhasMemDados, int **memDados);
void escreveMemDados(int *memDados, int endereco, int valor);
void imprimeMemDados(int *memDados, int inicio, int fim);

// -------------------------------------------------------------------------

int main(){
    int pc = 0;
    int esc;
    int opcao;
    instrucao *memoria = NULL;
    sinaisUC sinais;
    int linhas=0, linhasMemDados=0;

    int *bReg = inicializaBReg();
    int *memDados = inicializaMemDados();

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
                printf("\nDigite o nome do arquivo da memória de instruções (.mem): ");

                fgets(arq, sizeof(arq), stdin);
                arq[strcspn(arq, "\n")] = '\0';

                linhas = contaLinhas(arq);
                printf("\n%d Instruções.\n", linhas);

                lerMem(arq, &memoria, linhas);
                break;

            case 2:
                //Carregar Mem de Dados
                char arqMem[20];
                printf("\nDigite o nome do arquivo da memória de dados (.dat): ");

                fgets(arqMem, sizeof(arqMem), stdin);
                arqMem[strcspn(arqMem, "\n")] = '\0';

                linhasMemDados = contaMemDados(arqMem);
                printf("\n%d Dados encontrados.\n", linhasMemDados);

                lerMemDados(arqMem, linhasMemDados, &memDados);

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
                programCounter(memoria, linhas, esc, bReg, &sinais, &pc);
                break;

            case 8:
                //Executa instrução (step)
                esc = 1;
                programCounter(memoria, linhas, esc, bReg, &sinais, &pc);
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

int contaMemDados(char *arqMem){
    arquivoMemDados = fopen(arqMem, "r");
    char ch;
    int count=0;

    if(arquivoMemDados==NULL){
        printf("\nAcesso negado!\n");
        return 0;
    }

    while((ch=fgetc(arquivoMemDados))!=EOF){
        if(ch=='\n'){
            count++;
        }
    }

    fclose(arquivoMemDados);
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
void programCounter(instrucao *memoria, int linhas, int esc, int *bReg, sinaisUC *sinais, int *pc){
    char sair = '1';

    while((*pc < linhas) && (sair != '0')){

        printf("\nPC = %d | Memória = %s\n", *pc, memoria[*pc].mem);

        // Decodifica
        decodificaInst(&memoria[*pc]);

        // Controle
        unidadeControle(&memoria[*pc], sinais);

        // Executa
        executaInstrucao(&memoria[*pc], sinais, bReg);

        *pc = *pc + 1;

        // step
        if(esc == 1){
            printf("\nPressione ENTER para próximo passo ou 0 para sair: ");
            sair = getchar();
        }
    }
}

// Decodificação
void decodificaInst(instrucao *instrucao){

    (*instrucao).opcode = (*instrucao).instrucao >> 12; // Pega os 4 bits do opcode


    switch((*instrucao).opcode){
    case 0:
        (*instrucao).tipoInst = tipoR;
        (*instrucao).rs = ((*instrucao).instrucao >> 9) & 0x7; // pega os 3 bits do rs (desloca 6 bits para a direita e pega os 3 mais significativos que ficaram)
        (*instrucao).rt = ((*instrucao).instrucao >> 6) & 0x7; // pega os 3 bits do rt
        (*instrucao).rd = ((*instrucao).instrucao >> 3) & 0x7; // pega os 3 bits do rd
        (*instrucao).funct = ((*instrucao).instrucao) & 0x7;
        printf("[ Tipo R ] \n");
        printf("opcode: %d\n", (*instrucao).opcode);
        printf("rs: %d\n", (*instrucao).rs);
        printf("rt: %d\n", (*instrucao).rt);
        printf("rd: %d\n", (*instrucao).rd);
        printf("funct: %d\n", (*instrucao).funct);
        break;

    case 2:
        (*instrucao).tipoInst = tipoJ;
        (*instrucao).addr = ((*instrucao).instrucao) &0xFF; // pega os 8 bits do adress
        printf("[ Tipo J ]\n");
        printf("opcode: %d\n", (*instrucao).opcode);
        printf("address: %d\n", (*instrucao).addr);
        break;

    default:
        (*instrucao).tipoInst = tipoI;
        (*instrucao).rs = ((*instrucao).instrucao >> 9) &0x7; // pega os 3 bits do rs
        (*instrucao).rt = ((*instrucao).instrucao >> 6) &0x7; // pega os 3 bits do rt
        (*instrucao).imm = ((*instrucao).instrucao) &0x3F; // pega os 6 bits do imediato (deve passar por um extensor antes da ULA)
        printf("[ Tipo I ] \n");
        printf("opcode: %d\n", (*instrucao).opcode);
        printf("rs: %d\n", (*instrucao).rs);
        printf("rt: %d\n", (*instrucao).rt);
        printf("imediato: %d\n", (*instrucao).imm);
    }
}

//UC
void unidadeControle(instrucao *instrucao, sinaisUC *sinais){

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
            }
            else if((*sinais).ulaOp==2){
                printf("Sub $%d, $%d, $%d\n", (*instrucao).rd, (*instrucao).rs, (*instrucao).rt);
            }
            else if((*sinais).ulaOp==4){
                printf("\nAND $%d, $%d, $%d\n", (*instrucao).rd, (*instrucao).rs, (*instrucao).rt);

            }
            else if((*sinais).ulaOp==5){
                printf("\nOR $%d, $%d, %d\n", (*instrucao).rd, (*instrucao).rs, (*instrucao).rt);
            }
            break;

        case 2:
            (*sinais).jump = 1;
            printf("\nJ %d\n", (*instrucao).addr);
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
            (*sinais).ulaOp = 6; // 110
            (*sinais).EscMem = 0;
            (*sinais).MemParaReg = 0;
            (*sinais).jump = 0;
            (*sinais).branch = 1;

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
            
            printf("\nlw $%d, $%d, %d\n", (*instrucao).rs, (*instrucao).rt, (*instrucao).imm);

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
    return calloc(8, sizeof(int));
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

int *inicializaMemDados(){
    return calloc(256, sizeof(int));
}

void lerMemDados(char *arqMem, int linhasMemDados, int **memDados) {
    int i=0;

    if (*memDados == NULL) {
        printf("\nErro ao alocar memória\n");
        return;
    }

    arquivoMemDados = fopen(arqMem, "r");
    if (arquivoMemDados == NULL) {
        printf("\nErro ao abrir o arquivo %s\n", arqMem);
        return;
    }

    int valor;

    for(i = 0; i < linhasMemDados; i++) {
        fscanf(arquivoMemDados, "%d", &(*memDados)[i]);
    }

    printf("\nMemória de Dados:\n\n");

    for(i=0;i<linhasMemDados;i++){
        printf("Posição de Memória %d: %d\n",i,(*memDados)[i]);
    }

    fclose(arquivoMemDados);
}

void escreveMemDados(int *memDados, int endereco, int valor) {
    if (endereco >= 0 && endereco < 256) {
        memDados[endereco] = valor;
    } else {
        printf("\nErro ao escrever na memória.\n");
    }
}

void imprimeMemDados(int *memDados, int inicio, int fim) {
    for (int i=inicio; i<=fim; i++) {
        printf("Endereço %d: %d\n", i, memDados[i]);
    }
}

int ULA(int op1, int op2, int ulaOp, int *zero){
    int resultado = 0;

    switch(ulaOp){
        case 0: // ADD
            resultado = op1 + op2;
            break;

        case 2: // SUB
            resultado = op1 - op2;
            break;

        case 4: // AND
            resultado = op1 & op2;
            break;

        case 5: // OR
            resultado = op1 | op2;
            break;

        case 6: //BEQ
            resultado = op1 - op2;
            break;

        case 1: // ADDI
            resultado = op1 + op2;
            break;

        case 3: // LW/SW
            resultado = op1 + op2;
            break;

        default:
            printf("\nOperação da ULA inválida!\n");
    }

    // flag zero
    if(resultado == 0){
        *zero = 1;
    } else {
        *zero = 0;
    }

    return resultado;
}

// *Adicionar memória de dados
void executaInstrucao(instrucao* instrucao, sinaisUC *sinais, int *bReg ){  
    int  operador1, operador2, UlaResultado=0, regDst, dadoFinal; // passar para uint8_t aqui e nas funções

    lerRegistradores(bReg, (*instrucao).rs, (*instrucao).rt, &operador1, &operador2);

    if((*sinais).UlaFonte==1){
        // operador2=extensorBit((*instrucao).imm); 
        printf("\nextensor em andamento\n"); // passar por extensor 
    }

    // UlaResultado = ULA(operador1, operador2, (*sinais).ulaOp);
    int zero = 0;

    UlaResultado = ULA(operador1, operador2, (*sinais).ulaOp, &zero);

    printf("\nULA: %d op %d = %d\n", operador1, operador2, UlaResultado);

    if((*sinais).EscMem==1){
        printf("\nlógica para escrever dado da memória\n");
        printf("em andamento\n");
    }// else{
        // deve ler sempre (?)
    //}

    if((*sinais).MemParaReg==0){
        // dadoFinal = lerMemDados();
        printf("\nPega valor do banco de dados para banco de registradores\n");
        printf("em andamento\n");
    }else{
        dadoFinal = UlaResultado;
    }
    
    if((*sinais).RegDst==0){
        regDst = (*instrucao).rd;
    }else{
        regDst = (*instrucao).rt;
    }

    escreveRegistrador(bReg, (*instrucao).rt, dadoFinal, (*sinais).EscReg);
    printf("\nRegistrador a ser escrito: $%d\n", regDst);

    if((*sinais).branch==1){  // && UlaResposta == 0?
        printf("\nlógica branch if equal\n");
    }

}

int8_t extensorBit(int8_t imm){
    printf("\nlógica para extender bits\n");
    printf("Em andamento\n");

    return imm;
}
