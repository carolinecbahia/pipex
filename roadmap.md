# 🗺️ PIPEX ROADMAP E GUIA DE IMPLEMENTAÇÃO

Este guia detalhado serve como o mapa oficial para o desenvolvimento do projeto **Pipex**, replicando o mecanismo UNIX de *pipes* e redirecionamento de I/O, conforme a sintaxe: `infile cmd1 | cmd2 outfile`.

## I. Visão Geral Conceitual

O Pipex utiliza a Comunicação Interprocessos (IPC) via `pipe()` para ligar a saída padrão de um comando à entrada padrão do próximo. 

| Componente | Função no Pipex | Chamadas de Sistema Chave |
| :--- | :--- | :--- |
| **Processo Pai** | Gerente e Orquestrador. | [cite_start]`pipe()`, `fork()`, `waitpid()`, `close()` [cite: 89] |
| **Filho 1 (`cmd1`)** | Lê do `infile` e **escreve** no pipe. | [cite_start]`dup2()`, `execve()` [cite: 89] |
| **Filho 2 (`cmd2`)** | **Lê** do pipe e escreve no `outfile`. | [cite_start]`dup2()`, `execve()` [cite: 89] |

---

## II. Fluxograma Lógico (Estrutura de Controle)

O fluxo de controle do programa é dividido em quatro fases, garantindo o correto encadeamento dos processos.

### 1. 🎯 Fase de Setup (Processo Pai)

| Símbolo | Ação | Notas Conceituais |
| :---: | :--- | :--- |
| **Início** | Validação de argumentos e *parsing* | [cite_start]Confirma 4 argumentos: `file1 cmd1 cmd2 file2`[cite: 92]. |
| **Processo** | `pipe(fd[2])` | **Cria o canal**: FDs de Leitura (`fd[0]`) e Escrita (`fd[1]`). |

### 2. 👶 Caminho do Primeiro Comando (`cmd1`)

*(Executado no Processo **Filho 1** após o primeiro `fork()`)*

| Símbolo | Ação | Foco do Processo |
| :---: | :--- | :--- |
| **Decisão** | `pid1 = fork()` | Cria o processo Filho 1. |
| **Redirecionamento** | `dup2(infile_fd, 0 - stdin)` | **ENTRADA:** `cmd1` lê de `infile`. |
| **Redirecionamento** | `dup2(fd[1], 1 - stdout)` | **SAÍDA (PIPE):** `cmd1` escreve no canal. |
| **Limpeza** | `close(fd[0])` e `close(fd[1])` | **Crucial:** Fecha as cópias não utilizadas do pipe para evitar bloqueios. |
| **Execução** | `execve(cmd1, ...)` | Substitui o código. |

### 3. 🧑 Caminho do Segundo Comando (`cmd2`)

*(Executado no Processo **Filho 2** após o segundo `fork()`)*

| Símbolo | Ação | Foco do Processo |
| :---: | :--- | :--- |
| **Decisão** | `pid2 = fork()` | Cria o processo Filho 2. |
| **Redirecionamento** | `dup2(fd[0], 0 - stdin)` | **ENTRADA (PIPE):** `cmd2` lê do canal. |
| **Redirecionamento** | `dup2(outfile_fd, 1 - stdout)` | **SAÍDA:** `cmd2` escreve no `outfile`. |
| **Limpeza** | `close(fd[0])` e `close(fd[1])` | **Crucial:** Fecha as cópias não utilizadas do pipe. |
| **Execução** | `execve(cmd2, ...)` | Substitui o código. |

### 4. 🏁 Conclusão e Espera (Processo Pai)

| Símbolo | Ação | Notas Conceituais |
| :---: | :--- | :--- |
| **Limpeza Final** | `close(fd[0])` e `close(fd[1])` | **Fecha as cópias ORIGINAIS** do pipe do Pai. |
| **Espera** | `waitpid(pid1, ...)` e `waitpid(pid2, ...)` | Aguarda o término de **ambos** os processos filhos. |
| **Fim** | Retorna o código de saída | Retorna o status do último comando (`cmd2`). |

---

## III. 📝 Checklist de Implementação (Ações Críticas)

Utilize este checklist para garantir a integridade do I/O e a correta manipulação dos FDs.

| Fase | Status | Tarefa | Detalhe Crucial |
| :---: | :---: | :--- | :--- |
| **Setup** | ☐ | Criação do Pipe | `pipe(fd[2])` **apenas uma vez** no Pai. |
| **Setup** | ☐ | Busca do `PATH` | Achar o caminho completo do executável (`find_path`). |
| **CMD 1** | ☐ | Redirecionar Entrada | `dup2(infile_fd, STDIN_FILENO)`. |
| **CMD 1** | ☐ | Redirecionar Saída | `dup2(fd[1], STDOUT_FILENO)`. |
| **CMD 1** | ☐ | **FECHAMENTO CRÍTICO** | `close(fd[0])` e `close(fd[1])` (no Filho 1). |
| **CMD 2** | ☐ | Redirecionar Entrada | `dup2(fd[0], STDIN_FILENO)`. |
| **CMD 2** | ☐ | Redirecionar Saída | `dup2(outfile_fd, STDOUT_FILENO)`. |
| **CMD 2** | ☐ | **FECHAMENTO CRÍTICO** | `close(fd[0])` e `close(fd[1])` (no Filho 2). |
| **Fim** | ☐ | Fechamento Final | `close(fd[0])` e `close(fd[1])` (no Processo Pai). |
| **Fim** | ☐ | Espera | `waitpid()` para **todos** os processos filhos. |

---

## IV. 🛠️ Apêndice: Requisitos e Restrições do Projeto

### [cite_start]1. Funções Externas Autorizadas (Mandatory Part) [cite: 89]

| Categoria | Funções Autorizadas |
| :---: | :--- |
| **I/O e Files** | `open`, `close`, `read`, `write`, `unlink` |
| **Memória** | `malloc`, `free` |
| **Processos & Pipes** | `fork`, `pipe`, `execve`, `wait`, `waitpid`, `exit` |
| **Utilitários** | `perror`, `strerror`, `access`, `dup`, `dup2` |
| **Output** | `ft_printf` ou qualquer equivalente **codificado por você** |
| **Biblioteca** | `Libft` (autorizada) |

### 2. Restrições e Requisitos de Implementação

* **Linguagem:** O projeto deve ser escrito em C.
* **Norminette:** Deve estar em conformidade com a Norm; erros na parte *bonus* resultam em 0.
* **Comportamento:** Deve comportar-se **exatamente** como o comando *shell*: `< file1 cmd1 | cmd2 > file2`.
* **Terminação:** O programa deve nunca terminar inesperadamente (ex: *segfault*, *double free*).
* **Vazamento de Memória (*Memory Leaks*):** Toda a memória alocada deve ser liberada.
* **Makefile:** Deve conter as regras `$(NAME)`, `all`, `clean`, `fclean` e `re`.

### 3. Requisitos para o Bônus (Opcional)

A parte bônus **só será avaliada** se a parte obrigatória estiver **PERFEITA** e funcionando sem mau funcionamento.

O bônus permite implementar:

* **Múltiplos Pipes:** Suporte a `$ ./pipex file1 cmd1 cmd2 cmd3 ... cmdn file2`.
* **Here Doc (`<<`):** Suporte a `$ ./pipex here_doc LIMITER cmd cmd1 file` (equivalente a `cmd << LIMITER | cmd1 >> file`).
