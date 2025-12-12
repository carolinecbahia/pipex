# pipex

🗺️ Fluxograma Esquemático do Pipex

O fluxograma é dividido em três colunas principais, representando os três processos envolvidos: o Processo Pai (que orquestra), o Filho 1 (cmd1) e o Filho 2 (cmd2).
Início (Processo Pai)
Símbolo	Ação	Notas Conceituais
Início	Validação de argumentos e parsing	Confirma se todos os arquivos e comandos estão presentes.
Processo	🪚 pipe(fd[2])	Cria o canal de comunicação. O pai agora tem os FDs fd[0] (leitura) e fd[1] (escrita).
Caminho do Primeiro Comando (cmd1)

(O processo pai se prepara para lançar o primeiro comando)
Símbolo	Ação	Foco do Processo
Decisão	👶 fork()	O processo se divide! Se pid == 0, o código entra no Filho 1.
Processo (Filho 1)	open("infile", ...)	Abre o arquivo de entrada.
Processo (Filho 1)	dup2(infile_fd, 0 - stdin)	Redireciona a ENTRADA: Faz com que cmd1 leia do infile (FD 0 -> infile).
Processo (Filho 1)	dup2(fd[1], 1 - stdout)	Redireciona a SAÍDA (O Pipe!): Faz com que cmd1 escreva no pipe (FD 1 -> fd[1]).
Processo (Filho 1)	close(fd[0]) e close(fd[1])	Limpeza: Fecha as cópias dos FDs do pipe que o Filho 1 não usará mais.
Processo (Filho 1)	🚀 execve(cmd1, ...)	Executa o Comando: Substitui o código do processo Filho 1 pelo cmd1.
Fim	(Filho 1 termina)	O processo Filho 1 morre. Sua saída está no pipe.
Caminho do Segundo Comando (cmd2)

(O processo pai, após o fork do Filho 1, lança o segundo comando)
Símbolo	Ação	Foco do Processo
Decisão	🧑 fork() (Novo fork no Pai)	O processo se divide novamente! Se pid == 0, o código entra no Filho 2.
Processo (Filho 2)	open("outfile", ...)	Abre o arquivo de saída.
Processo (Filho 2)	dup2(fd[0], 0 - stdin)	Redireciona a ENTRADA (O Pipe!): Faz com que cmd2 leia do pipe (FD 0 -> fd[0]).
Processo (Filho 2)	dup2(outfile_fd, 1 - stdout)	Redireciona a SAÍDA: Faz com que cmd2 escreva no outfile (FD 1 -> outfile).
Processo (Filho 2)	close(fd[0]) e close(fd[1])	Limpeza: Fecha as cópias dos FDs do pipe que o Filho 2 não usará mais.
Processo (Filho 2)	🚀 execve(cmd2, ...)	Executa o Comando: Substitui o código do processo Filho 2 pelo cmd2.
Fim	(Filho 2 termina)	O processo Filho 2 morre.
Conclusão (Processo Pai)
Símbolo	Ação	Notas Conceituais
Processo (Pai)	close(fd[0]) e close(fd[1])	Limpeza Final: Fecha as cópias originais dos FDs do pipe que o Pai tinha.
Processo (Pai)	⏸️ waitpid(...) (para Filho 1)	O pai aguarda o Filho 1.
Processo (Pai)	⏸️ waitpid(...) (para Filho 2)	O pai aguarda o Filho 2.
Fim	Retorna o código de saída (do cmd2)	O programa Pipex finaliza.

Este fluxo mostra que o Processo Pai é o gerente que:

    Cria a infraestrutura (pipe()).

    Lança os trabalhadores (fork()).

    Aguarda o término de todos (waitpid()).

Os Processos Filhos são os trabalhadores que:

    Ajustam sua entrada e saída (dup2()).

    Executam a tarefa (execve()).

✅ Checklist de Implementação do Pipex

🎯 Fase 1: Inicialização e Preparação
Status	Tarefa	Detalhe Crucial
☐	Validação e Parsing	Confirme o número de argumentos. Extraia infile, outfile e os comandos.
☐	Criação do Pipe	Chame pipe(fd[2]) uma única vez no processo principal (Pai).
☐	Busca do PATH	Garanta que as funções de busca do comando (find_path) estejam prontas para execve().

👶 Fase 2: Execução do Primeiro Comando (cmd1)

(Execute este bloco de código após o primeiro fork() no processo Filho 1)
Status	Tarefa	Detalhe Crucial
☐	Criação do Processo	Chame pid1 = fork(). Continue se pid1 == 0.
☐	Abrir Entrada	Abra infile. Lidar com erros de abertura.
☐	Redirecionar Entrada	Chame dup2(infile_fd, STDIN_FILENO) (FD 0).
☐	Redirecionar Saída (Pipe)	Chame dup2(fd[1], STDOUT_FILENO) (FD 1).
☐	Fechamento (Filho 1)	FECHAR fd[0] e fd[1] (Os FDs do pipe).
☐	Execução	Chame execve() (ou equivalente) para cmd1.
☐	Tratamento de Erros	Lidar com o que acontece se execve() falhar (deve sair e retornar um código de erro).

🧑 Fase 3: Execução do Segundo Comando (cmd2)

(Execute este bloco de código no processo Pai, após o fork do Filho 1)
Status	Tarefa	Detalhe Crucial
☐	Criação do Processo	Chame pid2 = fork(). Continue se pid2 == 0.
☐	Abrir Saída	Abra/Crie outfile (com as flags corretas, como O_TRUNC e O_CREAT).
☐	Redirecionar Entrada (Pipe)	Chame dup2(fd[0], STDIN_FILENO) (FD 0).
☐	Redirecionar Saída	Chame dup2(outfile_fd, STDOUT_FILENO) (FD 1).
☐	Fechamento (Filho 2)	FECHAR fd[0] e fd[1] (Os FDs do pipe).
☐	Execução	Chame execve() (ou equivalente) para cmd2.
☐	Tratamento de Erros	Lidar com o que acontece se execve() falhar.

🏁 Fase 4: Conclusão e Limpeza

(Execute este bloco de código no processo Pai)
Status	Tarefa	Detalhe Crucial
☐	Fechamento (Pai)	FECHAR os FDs originais do pipe: fd[0] e fd[1].
☐	Aguardar Filho 1	Chame waitpid(pid1, ...) (ou wait para todos).
☐	Aguardar Filho 2	Chame waitpid(pid2, ...) (guarde o código de saída do último comando).
☐	Fim do Programa	Retorne o código de saída (do cmd2) para o shell.