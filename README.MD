# Projeto Integrador II - Projeto I

Simulador de uma arquitetura MiniMIPS de 16 bits, implementado em C, com execução interativa por menu e suporte a operações de carga/armazenamento, instruções aritméticas/lógicas e controle de fluxo.

## Autores

- Alex Maron Vaz
- Adrian do Couto Martins
- Gabriel Torres Machado

### Perfis GitHub dos autores

- Alex Maron Vaz: [AlexMaronVaz](https://github.com/AlexMaronVaz)
- Adrian do Couto Martins: [AdrianCouto](https://github.com/AdrianCouto)
- Gabriel Torres Machado: [gabrieltm-sudo](https://github.com/gabrieltm-sudo)

## Visão geral

Este projeto implementa um simulador didático com os seguintes componentes:

- Memória de instruções com 256 posições (16 bits por instrução).
- Memória de dados com 256 posições inteiras.
- Banco de registradores com 8 registradores.
- Unidade de Controle e ULA.
- Program Counter (PC) com suporte a salto condicional e incondicional.
- Execução em modo contínuo (run), passo a passo (step) e retrocesso de 1 instrução (back).
- Exportação da memória de instruções em formato ASM e da memória de dados em formato DAT.
- Coleta de estatísticas por tipo e por instrução.

## Estrutura do repositório

```text
.
├── mainminimips.c                    # Código-fonte principal do simulador
├── makefile                          # Regras de compilação e execução
├── memoria1.mem                      # Exemplo de memória de instruções
└── dados.dat                         # Exemplo de memória de dados

```

## Arquitetura MiniMIPS usada no projeto

### Formatos de instrução

- Tipo R: opcode (4) + rs (3) + rt (3) + rd (3) + funct (3)
- Tipo I: opcode (4) + rs (3) + rt (3) + imediato (6)
- Tipo J: opcode (4) + endereço (8)

### Instruções suportadas

- Tipo R:
  - add (opcode 0, funct 0)
  - sub (opcode 0, funct 2)
  - and (opcode 0, funct 4)
  - or  (opcode 0, funct 5)
- Tipo I:
  - addi (opcode 4)
  - beq  (opcode 8)
  - lw   (opcode 11)
  - sw   (opcode 15)
- Tipo J:
  - j    (opcode 2)

### Convenções importantes

- O imediato de instruções tipo I possui 6 bits e é estendido com sinal.
- O simulador considera fim do programa ao encontrar instrução 0000000000000000 (HALT/NOP no contexto do projeto).
- A memória é limitada a 256 endereços para instruções e dados.

## Como compilar e executar

Pré-requisito: GCC e Make instalados em ambiente Linux.

### Usando Makefile

```bash
make help
make compile
make run
```

Ou para compilar e executar em sequência:

```bash
make compilerun
```

Para remover o executável:

```bash
make rm
```

### Compilação manual

```bash
gcc -Wall -Wextra -g mainminimips.c -o exec
./exec
```

## Fluxo recomendado de uso no menu

Ao iniciar o programa, o menu principal oferece opções de carregamento, visualização e execução. Um fluxo típico é:

1. Carregar memória de instruções (.mem)
2. Carregar memória de dados (.dat)
3. Executar em step para depuração ou run para execução completa
4. Consultar estatísticas e estado do simulador
5. Exportar .asm e/ou .dat se necessário

## Opções do menu principal

- 1: Carregar Memória de Instruções (.mem)
- 2: Carregar Memória de Dados (.dat)
- 3: Imprimir memórias (instruções e dados)
- 4: Imprimir Banco de Registradores
- 5: Imprimir simulador (registradores, memórias e estatísticas)
- 6: Salvar .asm
- 7: Salvar .dat
- 8: Run (executa até o fim)
- 9: Step (executa uma instrução)
- 10: Back (volta uma instrução no histórico)
- 0: Sair

## Formato dos arquivos de entrada

### Arquivo .mem (memória de instruções)

- Uma instrução binária de 16 bits por linha.
- Exemplo:

```text
0100000010111011
0100000001000011
0000010001011000
1111000010000000
0010000000000001
```

### Arquivo .dat (memória de dados)

- Um inteiro por linha (o simulador lê até 256 posições).
- Exemplo:

```text
12
2
-5
8
10
```

## Estatísticas coletadas

Durante a execução, o simulador contabiliza:

- Total de instruções executadas
- Quantidade por tipo (R, I, J)
- Quantidade por instrução específica (add, sub, and, or, addi, beq, lw, sw, j)

## Arquivos de exemplo

- memoria1.mem: programa de teste em binário.
- dados.dat: conjunto de dados para memória de dados.

## Referências

- C. Ortega-Sanchez, "MiniMIPS: An 8-Bit MIPS in an FPGA for Educational Purposes," in 2011 International Conference on Reconfigurable Computing and FPGAs, 2011, doi: 10.1109/ReConFig.2011.62.

## Licença

Projeto acadêmico para fins educacionais na disciplina de Projeto Integrador II.