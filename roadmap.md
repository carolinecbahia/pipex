# 🗺️ Roadmap e Guia de Implementação do Pipex (42)

Este documento serve como um mapa de referência e um checklist passo a passo para a implementação do projeto Pipex, replicando a funcionalidade `infile cmd1 | cmd2 outfile`.

## I. Visão Geral Conceitual

O Pipex se baseia na comunicação Interprocessos (IPC) usando pipes.

| Componente | Função no Pipex | Chamadas de Sistema Chave |
| :--- | :--- | :--- |
| **Processo Pai** | Gerente. Cria o pipe e os processos filhos. Aguarda a conclusão. | `pipe()`, `fork()`, `waitpid()`, `close()` |
| **Filho 1 (`cmd1`)** | Trabalhador de Escrita. Lê do `infile` e **escreve no pipe**. | `dup2()`, `execve()` |
| **Filho 2 (`cmd2`)** | Trabalhador de Leitura. **Lê do pipe** e escreve no `outfile`. | `dup2()`, `execve()` |

## II. Fluxograma Lógico (Estrutura de Controle)

A tabela abaixo descreve a sequência de eventos e a responsabilidade de cada processo.

### 1. Início e Setup (Processo Pai)

| Símbolo | Ação | Notas Conceituais |
| :---: | :--- | :--- |
| **Início** | Validação de argumentos e *parsing* | Extrair `infile`, `outfile` e comandos. |
| **Processo** | `pipe(fd[2])` | **Cria o canal**: FDs de Leitura (`fd[0]`) e Escrita (`fd[1]`). |

### 2. Caminho do Primeiro Comando (`cmd1`)

*(Executado no Processo **Filho 1**)*

| Símbolo | Ação | Foco do Processo |
| :---: | :--- | :--- |
| **Decisão** | `pid1 = fork()` | Cria o processo Filho 1. |
| **Redirecionamento** | `open("infile", ...)` | Abre o arquivo de entrada. |
| **Redirecionamento** | `dup2(infile_fd, 0 - stdin)` | **ENTRADA:** `cmd1` lê de `infile`. |
| **Redirecionamento** | `dup2(fd[1], 1 - stdout)` | **SAÍDA (PIPE):** `cmd1` escreve no canal. |
| **Limpeza** | `close(fd[0])` e `close(fd[1])` | **Crucial:** Fecha as cópias não utilizadas do pipe. |
| **Execução** | `execve(cmd1, ...)` | Substitui o código. |
| **Fim** | *(Processo termina)* | A saída está agora no pipe, pronta para `cmd2`. |

### 3. Caminho do Segundo Comando (`cmd2`)

*(Executado no Processo **Filho 2**)*

| Símbolo | Ação | Foco do Processo |
| :---: | :--- | :--- |
| **Decisão** | `pid2 = fork()` | Cria o processo Filho 2 (no Pai). |
| **Redirecionamento** | `open("outfile", ...)` | Abre/Cria o arquivo de saída. |
| **Redirecionamento** | `dup2(fd[0], 0 - stdin)` | **ENTRADA (PIPE):** `cmd2` lê do canal. |
| **Redirecionamento** | `dup2(outfile_fd, 1 - stdout)` | **SAÍDA:** `cmd2` escreve no `outfile`. |
| **Limpeza** | `close(fd[0])` e `close(fd[1])` | **Crucial:** Fecha as cópias não utilizadas do pipe. |
| **Execução** | `execve(cmd2, ...)` | Substitui o código. |
| **Fim** | *(Processo termina)* | A execução está concluída. |

### 4. Conclusão e Espera (Processo Pai)

| Símbolo | Ação | Notas Conceituais |
| :---: | :--- | :--- |
| **Limpeza Final** | `close(fd[0])` e `close(fd[1])` | **Fecha as cópias ORIGINAIS** do pipe que o Pai possui. |
| **Espera** | `waitpid(pid1, ...)` | Aguarda o término do Filho 1. |
| **Espera** | `waitpid(pid2, ...)` | Aguarda o término do Filho 2. |
| **Fim** | Retorna o código de saída | Retorna o status do último comando (`cmd2`). |

## III. 📝 Checklist de Implementação

Use este checklist para garantir que todos os passos, especialmente as chamadas de *file descriptors*, foram implementados corretamente.

### 🎯 Fase 1: Inicialização e Preparação

| Status | Tarefa | Detalhe Crucial |
| :---: | :--- | :--- |
| ☐ | Validação e *Parsing* | Confirme o número de argumentos. |
| ☐ | Criação do Pipe | Chame `pipe(fd[2])` **apenas uma vez** no processo Pai. |
| ☐ | Busca do `PATH` | Funções de busca de comando (`find_path`) prontas. |

### 👶 Fase 2: Execução do Primeiro Comando (`cmd1`)

| Status | Tarefa | Detalhe Crucial |
| :---: | :--- | :--- |
| ☐ | Criação do Processo | `pid1 = fork()`. |
| ☐ | **Redirecionar Entrada** | `dup2(infile_fd, STDIN_FILENO)`. |
| ☐ | **Redirecionar Saída (Pipe)** | `dup2(fd[1], STDOUT_FILENO)`. |
| ☐ | **FECHAMENTO (Filho 1)** | **`close(fd[0])` e `close(fd[1])`**. |
| ☐ | Execução | `execve()` para `cmd1`. |

### 🧑 Fase 3: Execução do Segundo Comando (`cmd2`)

| Status | Tarefa | Detalhe Crucial |
| :---: | :--- | :--- |
| ☐ | Criação do Processo | `pid2 = fork()`. |
| ☐ | **Redirecionar Entrada (Pipe)** | `dup2(fd[0], STDIN_FILENO)`. |
| ☐ | **Redirecionar Saída** | `dup2(outfile_fd, STDOUT_FILENO)`. |
| ☐ | **FECHAMENTO (Filho 2)** | **`close(fd[0])` e `close(fd[1])`**. |
| ☐ | Execução | `execve()` para `cmd2`. |

### 🏁 Fase 4: Conclusão e Limpeza (Processo Pai)

| Status | Tarefa | Detalhe Crucial |
| :---: | :--- | :--- |
| ☐ | **FECHAMENTO (Pai)** | **`close(fd[0])` e `close(fd[1])`** (Originals). |
| ☐ | Aguardar Filhos | `waitpid(pid1, ...)` e `waitpid(pid2, ...)`. |
| ☐ | Fim do Programa | Retorna o código de saída do `cmd2`. |
