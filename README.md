# Inteligência Artificial
Trabalho desenvolvido para disciplina de Inteligência Artificial.

#### Alunos
  * Acácia dos Campos da Terra
  * João Pedro Winckler bernardi

#### Professor
  * José Carlos Bins Filho

# Repositório

Contém o código de um algoritmo genético modificado para gerar horários
de um curso, dada uma determinada entrada. Informações mais específicas estão descritas na documentação de nome 'Documentação.pdf'.

# Compilação

 * Compilar o código fonte com a flag -std=c++11 
  ou
 * usar o makefile disponibilizado, através do comando 'make'

# Execução
  ```
  > /run.sh <arquivoDeEntrada>
  ```

# Entrada
  A entrada consiste de uma sequência de dados relacionados à professores, disciplinas, preferências de horários. Os dados exigidos são:

  * Número N de professores, seguido de N nomes de professores, estando cada professor associado a um número M de horários, seguido de M horários os quais o professor prefere não ministrar aulas
  * Número N de semestres, seguido de N códigos de semestres, estando cada semestre associado a um o número de sala K e a um número M de quantidade de horários, seguido de M horários disponíveis
  * Número N de disciplinas, seguido de N linhas contendo o código da disciplina, o número de períodos a serem ocupados por aquela disciplina, o código do semestre e o nome do professor que irá ministrar a aula
 

# Saída
  A saída legível contendo o horário gerado poderá ser encontrada em uma planilha, nomeada 'horarios.csv', a qual é gerada automaticamente após a execução do programa.

