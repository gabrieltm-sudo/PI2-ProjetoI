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
    int decodificado;
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
void salvaASM(instrucao *memoria, int linhas);
void salvaDAT(int *memDados, int linhasMemDados);
void voltaInstrucao();
void run(instrucao *memoria, int linhas, int *bReg, sinaisUC *sinais, int *pc, int *memDados);
void step(instrucao *memoria, int linhas, int *bReg, sinaisUC *sinais, int *pc, int *memDados);

// MEMÓRIA
int contaLinhas(char *arq);
void lerMem(char *arq, instrucao **memoria, int linhas);


// PROGRAM COUNTER (PC) / BUSCA
void programCounter(int *pc);


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
void executaInstrucao(instrucao *instrucao, sinaisUC *sinais, int *bReg, int *memDados);
int8_t extensorBit(int8_t imm);


// ULA (UNIDADE LÓGICA E ARITMÉTICA)
int ULA(int op1, int op2, int ulaOp, int *zero);

// MEMÓRIA DE DADOS
int *inicializaMemDados();
int contaMemDados(char *arqMem);
void lerMemDados(char *arqMem, int linhasMemDados, int **memDados);
void escreveMemDados(int *memDados, int endereco, int valor);
int retornaMemoria(int *memDados, int enderecoULA);
void imprimeMemDados(int *memDados, int linhasMemDados);

// -------------------------------------------------------------------------

int main(){
    int pc = 0;
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
                imprimeBancoRegistradores(bReg);
                imprimeMemDados(memDados, linhasMemDados);
                break;

            case 5:
                // Salvar .asm
                salvaASM(memoria, linhas);
                break;

            case 6:
                // Salvar .dat
                salvaDAT(memDados,linhasMemDados);
                break;

            case 7:
                //Executar programa (run)
                run(memoria, linhas, bReg, &sinais, &pc, memDados);
                break;

            case 8:
                //Executa instrução (step)
                step(memoria, linhas, bReg, &sinais, &pc, memDados);
                break;

            case 9:
                //Voltar instrução (back)
                break;

            case 0:
                //Sair
                free(bReg);
                free(memoria);
                free(memDados);
                return 0;

            default:
                printf("\nOpção inválida!\n");
                break;
        }
    }

    return 0;
}

int contaLinhas(char *arq){
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
    *memoria = calloc(256, sizeof(instrucao));
    if(memoria == NULL){
    printf("\nMemoria nao carregada!\n");
    return;
}
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

int8_t extensorBit(int8_t imm){
    imm = imm<<2;
    //printf("\n%d", imm);

    imm = imm>>2;
    //printf("\n%d", imm);

    return imm;
}

// RUN 
void run(instrucao *memoria, int linhas, int *bReg, sinaisUC *sinais, int *pc, int *memDados){

    if(*pc >= linhas){
        printf("\nFim das instruções!\n");
        return;
    }
    while(*pc < linhas){
        
        printf("\nPC = %d | Memória = %s\n", *pc, memoria[*pc].mem);
       
        // Decodifica
        decodificaInst(&memoria[*pc]);

        memoria[*pc].decodificado = 1;

        // Controle
        unidadeControle(&memoria[*pc], sinais);

        // Executa
        executaInstrucao(&memoria[*pc], sinais, bReg, memDados);

        //PC
        programCounter(pc);
    }
}

void step(instrucao *memoria, int linhas, int *bReg, sinaisUC *sinais, int *pc, int *memDados){

    if(*pc >= linhas){
        printf("\nFim das instruções!\n");
        return;
    }

    printf("\nPC = %d | Memória = %s\n", *pc, memoria[*pc].mem);

    // Decodifica
    decodificaInst(&memoria[*pc]);

    memoria[*pc].decodificado = 1;

    // Controle
    unidadeControle(&memoria[*pc], sinais);

    // Executa
    executaInstrucao(&memoria[*pc], sinais, bReg, memDados);

    //PC
    programCounter(pc);
}

// PC
void programCounter(int *pc){

    (*pc)++;
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
        printf("\n[ Tipo R ] \n");
        printf("opcode: %d\n", (*instrucao).opcode);
        printf("rs: %d\n", (*instrucao).rs);
        printf("rt: %d\n", (*instrucao).rt);
        printf("rd: %d\n", (*instrucao).rd);
        printf("funct: %d\n", (*instrucao).funct);
        break;

    case 2:
        (*instrucao).tipoInst = tipoJ;
        (*instrucao).addr = ((*instrucao).instrucao) &0xFF; // pega os 8 bits do adress
        printf("\n[ Tipo J ]\n");
        printf("opcode: %d\n", (*instrucao).opcode);
        printf("address: %d\n", (*instrucao).addr);
        break;

    default:
        (*instrucao).tipoInst = tipoI;
        (*instrucao).rs = ((*instrucao).instrucao >> 9) &0x7; // pega os 3 bits do rs
        (*instrucao).rt = ((*instrucao).instrucao >> 6) &0x7; // pega os 3 bits do rt
        (*instrucao).imm = ((*instrucao).instrucao) &0x3F; // pega os 6 bits do imediato (deve passar por um extensor antes da ULA)
        (*instrucao).imm = extensorBit((*instrucao).imm);
        printf("\n[ Tipo I ] \n");
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

            break;

        case 2: // opcode = 0010 - J
            (*sinais).RegDst = 0;
            (*sinais).EscReg = 0;
            (*sinais).UlaFonte = 0;
            (*sinais).ulaOp = 0;
            (*sinais).EscMem = 0;
            (*sinais).MemParaReg = 0;
            (*sinais).jump = 1;
            (*sinais).branch = 0;

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
    printf("\n");
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

    for(i = 0; i < linhasMemDados; i++) {
        fscanf(arquivoMemDados, "%d", &(*memDados)[i]);
    }

    imprimeMemDados(*memDados, linhasMemDados);

    fclose(arquivoMemDados);
}

void escreveMemDados(int *memDados, int endereco, int valor) {
    if (endereco >= 0 && endereco < 256) {
        memDados[endereco] = valor;
    } else {
        printf("\nErro ao escrever na memória.\n");
    }
}

int retornaMemoria(int *memDados, int enderecoULA) {
    return memDados[enderecoULA];
}

void imprimeMemDados(int *memDados, int linhasMemDados) {
    printf("\nMemória de Dados:\n\n");

    int i=0;
    for(i=0;i<linhasMemDados;i++){
        printf("Posição de Memória %d: %d\n",i,(memDados)[i]);
    }
}

void executaInstrucao(instrucao* instrucao, sinaisUC *sinais, int *bReg, int *memDados){
    int  operador1, operador2, UlaResultado=0, regDst, dadoFinal=0, zero = 0;; // passar para uint8_t aqui e nas funções


    lerRegistradores(bReg, (*instrucao).rs, (*instrucao).rt, &operador1, &operador2);

    if((*sinais).UlaFonte==1){
        operador2=(*instrucao).imm; 
    }

    UlaResultado = ULA(operador1, operador2, (*sinais).ulaOp, &zero);

    if((*sinais).EscMem==1){
        escreveMemDados(memDados, UlaResultado, (*instrucao).rt);
    }

    if((*sinais).MemParaReg==1){
        dadoFinal = UlaResultado;
    }else{
        dadoFinal = retornaMemoria(memDados, UlaResultado);
    }
    
    if((*sinais).RegDst==1){
        regDst = (*instrucao).rd;
    }else{
        regDst = (*instrucao).rt;
    }

    if((*sinais).EscReg==1){
        escreveRegistrador(bReg, regDst, dadoFinal, (*sinais).EscReg);
        printf("\nRegistrador a ser escrito: $%d com o valor %d\n", regDst, dadoFinal);
    }

    if((*sinais).branch==1 && zero == 1){ 
        printf("\nPulo condicional detectado\n");
    }

}

void salvaASM(instrucao *memoria, int linhas) {
    int pc = 0;

    arquivo = fopen("main.asm", "w");

    if (arquivo == NULL) {
        printf("\nErro ao criar arquivo\n");
        return;
    }

    fprintf(arquivo, ".data\n\n");

    fprintf(arquivo, ".text\n");
    fprintf(arquivo, ".globl main\n");
    fprintf(arquivo, "\nmain:\n");

    while(pc < linhas){
        if((memoria)[pc].decodificado==0){
            decodificaInst(&memoria[pc]);
        }

        switch(memoria[pc].opcode){
            case 0: // opcode = 0000

                if(memoria[pc].funct==0){
                    fprintf(arquivo,"    add $%d, $%d, $%d\n", (memoria)[pc].rd, (memoria)[pc].rs, (memoria)[pc].rt);
                }
                else if((memoria)[pc].funct==2){
                    fprintf(arquivo,"    sub $%d, $%d, $%d\n", (memoria)[pc].rd, (memoria)[pc].rs, (memoria)[pc].rt);
                }
                else if((memoria)[pc].funct==4){
                    fprintf(arquivo,"    and $%d, $%d, $%d\n", (memoria)[pc].rd, (memoria)[pc].rs, (memoria)[pc].rt);
                }
                else if((memoria)[pc].funct==5){
                    fprintf(arquivo,"    or $%d, $%d, $%d\n", (memoria)[pc].rd, (memoria)[pc].rs, (memoria)[pc].rt);
                }
                break;

            case 2: // opcode = 0010 - J
                fprintf(arquivo,"    j %d\n", (memoria)[pc].addr);

                break;

            case 4: // opcode = 0100 - Addi
                fprintf(arquivo,"    addi $%d, $%d, %d\n", (memoria)[pc].rs, (memoria)[pc].rt, (memoria)[pc].imm);

                break;

            case 8: // opcode = 1000 - BEQ
                fprintf(arquivo,"    beq $%d, $%d, %d\n", (memoria)[pc].rs, (memoria)[pc].rt, (memoria)[pc].imm);

                break;

            case 11: // opcode = 1011 - lw
                fprintf(arquivo,"    lw $%d, $%d, %d\n", (memoria)[pc].rs, (memoria)[pc].rt, (memoria)[pc].imm);

                break;

            case 15: // opcode = 1111 - sw
                fprintf(arquivo,"    sw $%d, $%d, %d\n", (memoria)[pc].rs, (memoria)[pc].rt, (memoria)[pc].imm);

                break;
        }

        pc++;
    }

    fprintf(arquivo, "\n    li $v0, 10\n");
    fprintf(arquivo, "    syscall\n");

    fclose(arquivo);

    printf("\nArquivo 'main.asm' salvo!\n");
}

void salvaDAT(int *memDados, int linhasMemDados){
    arquivo = fopen("dados.dat","w");

    if (arquivo == NULL) {
        printf("\nErro ao criar arquivo\n");
        return;
    }

    for(int i=0;i<linhasMemDados;i++){
        fprintf(arquivo,"%d\n",memDados[i]);
    }

    fclose(arquivo);

    printf("\nArquivo 'dados.dat' salvo!\n");
}