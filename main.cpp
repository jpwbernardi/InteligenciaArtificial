#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

#define MAX 50  //Quantidade máxima de professores, turmas, disciplinas
#define ERR 3  //Quantidade de erros aceitados
#define TOLERANCE 3
#define TAM 30 //Tamanho da população
#define INF 112345678
#define QTDBILHETES 10
#define QTDTURMAS 9
#define TIMEMAX 20
#define LIVRE -1
typedef pair< int , vector<int> > iv;

int fim = 0; // Variavel que controla quantas vezes podemos retentar o processo

map<string, int> professores, turmas, disciplina;
vector<int> rejeitados[MAX], horarios[MAX];
vector<iv> infdisc[MAX];
int creditos[MAX];

/*Nosso grid
  J     0    1    2    3    4
       SEG  TER  QUA  QUI  SEX  I
  MAT   0    1    2    3    4   0
        5    6    7    8    9   1
  VESP 10   11   12   13   14   2
       15   16   17   18   19   3
  NOT  20   21   22   23   24   4
       25   26   27   28   29   5
 */

//returna true se o professor não tem problemas em dar aula nesse horário
int profgosta(vector<int> idprof, int turno) {
  for (int i = 0; i < (int) idprof.size(); i++)
    for (int j = 0, id = idprof[i]; j < (int) rejeitados[id].size(); j++)
      if (rejeitados[id][j] == turno) return 0;
v  return 1;
}

struct individuo {
  int grade[QTDTURMAS][6][5]; //10 periodos, 5 dias, 6 aulas (2 manhã, 2 tarde, 2 noite)
  set <int> materias; //Matérias já incluidas no horário
  int vivo; //Grade é valida quando está em 1
  int erros; //Quantifica os "erros", quantidade de aulas ocorrendo em
             //horários ruins para os professores

  individuo () { memset(this->grade, LIVRE, sizeof(this->grade)); vivo = 1; erros = 0; }
  individuo (set<int> m) {
    memset(this->grade, LIVRE, sizeof(this->grade)); vivo = 1; erros = 0;
    for (auto const& i : m) this->addmateria(i); //Deveria ser mais aleatório
  }

  //retorna verdadeiro se o professor está livre no horário
  int proflivre(vector<int> idprof, int turno) {
    int prof1, prof2, m;
    for (int j = 0; j < QTDTURMAS; j++) {
      m = this->grade[j][turno / 5][turno % 5]; //Pego a matéria que es
      if (m == LIVRE) continue; //Se não há matéria, m == -1
      //Percorremos as informações da matéria m
      for (auto const& d : infdisc[m])
        //Quando achamos a informação referente ao periodo que estamos
        //analizamos, verificamos se algum professor ofertando essa
        //matéria é o mesmo que os recebidos na função. Se for, o
        //horário não está livre
        if (d.first == j)
          for (int k = 0; k < (int) d.second.size(); k++) {
            prof1 = d.second[k];
            for (int i = 0; i < (int) idprof.size(); i++){
              prof2 = idprof[i];
              if (prof1 == prof2) return 0;
            }}}
    return 1;
  }

  //retorna true se há uma restrição nesse horário
  int restrito(vector<int> idprof, int turno) {
    //Se o horário não for o primeiro da manhã, nem o ultimo da noite,
    //ignoramos. Mesma coisa com sexta a noite e segunda de manhã, pois
    //são horários sem restrição
    if ((turno / 5 != 5 && turno / 5 != 0) || (turno / 5 == 0 && turno % 5 == 0) || (turno / 5 == 5 && turno % 5 == 4)) return 0;
    int  m, prof1, prof2;
    for (int j = 0; j < QTDTURMAS; j++) {
      if (turno / 5 == 0)  m = this->grade[j][5][turno % 5 - 1]; //primeiro horario manhã, verificamos a materia da noite anterior
      else m = this->grade[j][0][turno % 5 + 1]; //ultimo horario da noite, verificamos a primeira materia da manhã seguinte
      //Se algum professor de idprof já está em alguma aula no horário
      //m, há restrição legal. O horário não pode ser usado.
      if(!this->proflivre(idprof, m)) return 1;
    }
    return 0;
  }


  //Metodo que adiciona determinada matéria a grade
  void addmateria(int m) {
    int i, j, periodo, turno, count, p;
    //Vetor "possivel" serve para guardar horários "possíves" mas que os
    //professores não vão gostar
    int possivel[MAX];
    //Insere a matéria no conjunto de matérias do indivíduo
    this->materias.insert(m);
    //Essa mesma matéria pode estar sendo oferecida em mais de uma
    //turma(periodo), portanto adicionamos a matéria na grade de todas as turmas
    //que ela está sendo ofertada
    for (int y = 0; y < (int) infdisc[m].size(); y++) {
      p = 0; count = creditos[m]; //Devemos adicionar duas vezes a matéria na
      //grade e p é o inteiro que controla a posição
                        //do vetor possivel
      periodo = infdisc[m][y].first;
      //Mais de um professor pode dar essa aula e, para conveniencia dos
      //alunos, essa aula é ofertada no mesmo horário para todos os
      //professores que vão ministra-la. Por isso, todos eles devem
      //estar livres e "gostar" desse horário
      vector<int> prof = infdisc[m][y].second;
      //Procuramos cada horário disponível para aulas do período dessa
      //matéria para decidir qual é o melhor
      for (int k = 0; k < (int) horarios[periodo].size() && count; k++) {
        turno = horarios[periodo][k];
        i = turno / 5; j = turno % 5;
        //Se o professor já está dando aula nesse horário em outra
        //turma, esse horário não pode ser escolhido
        if (!this->proflivre(prof, turno)) continue;
        //Se o horário está livre (não tem outra matéria sendo ofertada
        //nesse horário) && é um horário bom para o professor, adicionamos na grade
        if (this->grade[periodo][i][j] == LIVRE && profgosta(prof, turno)) {
          this->grade[periodo][i][j] = m; count--;
          //Se o professor não gosta do horário, mas não existe
          //restrição legal, guardamos esse horário, caso seja
          //necessário
        } else if (!this->restrito(prof, turno) && p < count) possivel[p++] = turno;
      }
      //Se a quantidade de horários que falta colocar é menor do que
      //temos disponível, não é possivel colocar a matéria na grade. O
      //individuo está morto.
      if (count > p) this->vivo = 0;
      //Caso seja necessário, usamos os horários que reservamos (que os
      //professores não gostam) e marcamos um erro
      else while (p-- > 0 && count) { this->grade[periodo][possivel[p] / 5] [possivel[p] % 5] = m; this->erros++; count--; }
    }
  }

  //Método de cruzamento entre dois individuos
  individuo cruza(individuo b) {
    set <int> m;
    individuo p;
    //O melhor individuo é escolhido para perpetuar seus genes para o
    //novo individuo. Ou seja, a grade dos horários do filho é,
    //inicialmente, igual a melhor grade dentre as dos pais
    if (this->erros < b.erros) p = *this;
    else p = b;
    for (auto const& i : b.materias) m.insert(i);
    for (auto const& i : this->materias) m.insert(i);
    for (auto const& i : p.materias) m.erase(i);
    //As matérias que o menos apto possuia, porém o melhor não possuia,
    //são adicionadas na grade do novo indivíduo
    for (auto const& i : m) p.addmateria(i);
    //Esse novo individuo é retornado
    return p;
  }

  //Método de mutação de um indivíduo. Esse inteiro que recebemos é a
  //probabilidade da mutação e deve estar no intervalo [0, 100]. Na
  //nossa variação, p sempre é 100%, mas para fins de portabilidade,
  //deixamos o parametro na função
  void muta(int p) {
    if (p < rand() % 100) return;
    //t = quantidade de matérias que p não tem
    int t = disciplina.size() - this->materias.size();
    if (t == 0) return;
    //Realiza-se um sorteio, a dentre as t opções e a matéria sorteada
    //será adicionada
    t = rand() % t;
    for (auto const& i: disciplina) {
      if (this->materias.find(i.second) == this->materias.end()) {
        if (t--) continue;
        this->addmateria(i.second); break;
      }
    }
  }
};

//A função in() realiza a leitura da entrada conforme a descrição da
//entrada. Além disso, executa alguns prints que serão usados por um
//programa auxiliar para deixar a entrada mais amigável para o usuário,
//conforme descrito no pdf
void in() {
  char nome[20], materia[20], sem[20];
  int N, M, cont = 0, tmp, prof, tur, mat, novo, c;
  //Lê os professores
  scanf("%d", &N);
  printf("%d\n", N);
  while (N--) {
    scanf(" %s %d", nome, &M);
    professores[string(nome)] = cont; prof = cont++;
    printf("%s: %d", nome, cont - 1);
    while (M--) { scanf("%d", &tmp); rejeitados[prof].push_back(tmp); }
  }

  //Lê as turmas/semestres
  scanf("%d", &N); cont = 0;
  printf("%d\n", N);
  while (N--) {
    scanf(" %s %d %d", nome, &tmp, &M);
    turmas[string(nome)] = cont; tur = cont++;
    printf("%s: %d", nome, cont - 1);
    while (M--) { scanf("%d", &tmp); horarios[tur].push_back(tmp);  }
  }

  //Lê as disciplinas
  scanf("%d", &N);
  while (N--) {
    scanf(" %s %d %s %s", materia, &c, sem, nome);
    if (disciplina.find(string(materia)) == disciplina.end()) {
      printf("%s %d\n", materia, cont);
      creditos[cont] = c;
      disciplina[string(materia)] = cont++;
    }
    mat = disciplina[string(materia)];
    prof = professores[string(nome)];
    tur = turmas[string(sem)];
    novo = 1;
    for (int i = 0; i < (int) infdisc[mat].size(); i++) {
      if (infdisc[mat][i].first != tur) continue;
      novo = 0; infdisc[mat][i].second.push_back(prof);
    }
    if (novo == 0) continue;
    vector <int> p; p.push_back(prof);
    infdisc[mat].push_back(iv(tur, p));
  }
}

//A função debug() faz o print de todos as estruturas criadas para
//execução do algoritmo
void debug() {
  for (auto const& i : professores)
    printf("%s: %d\n", i.first.c_str(), i.second);
  for (int i = 0; i < MAX; printf("\n"), i++)
    for (int j = 0; j < (int) rejeitados[i].size(); j++)
      printf("[%d]", rejeitados[i][j]);

  for (auto const& i: turmas)
    printf("%s: %d\n", i.first.c_str(), i.second);
  for (int i = 0; i < MAX; printf("\n"), i++)
    for (int j = 0; j < (int) horarios[i].size(); j++)
      printf("[%d]", horarios[i][j]);

  for (auto const& i: disciplina)
    printf("%s: %d\n", i.first.c_str(), i.second);
  for (int i = 0; i < MAX; printf("\n"), i++)
    for (int j = 0; j < (int) infdisc[i].size(); j++) {
      printf("<%d, {", infdisc[i][j].first);
      for (int k = 0; k < (int) infdisc[i][j].second.size(); k++)
        printf("[%d]", infdisc[i][j].second[k]);
      printf("} >  ");
    }
}

//A função selecao(individuo) realiza a escolha do melhor individuo
//possível para o cruzamento. Quanto melhor o individuo(menor sua
//quantidade de erros), maior é sua chance de ser escolhido. É baseada
//no método de escalonamento de processos por loteria. Mais detalhes no pdf.
individuo selecao(individuo pop[TAM]) {
  if (fim == TOLERANCE) return pop[0];
  int bilhetes = 0, count = 0, sorteado;
  for (int i = 0; i < TAM; i++) {
    //tomamos o máximo, pois se a quantidade de erros for maior que a
    //quantidade de bilhetes máximo que cada individuo pode ter, esse
    //individuo nunca seria escolhido
    if (pop[i].vivo) bilhetes += max(QTDBILHETES / (pop[i].erros + 1), 1);
    //Se o inidivuo estava morto, tentamos ressuscita-lo
    else pop[i] = individuo(pop[i].materias);
  }
  //Se não havia individuo a ser escolhido, como tentamos ressuscitar
  //todos durante a distribuição dos bilhetes, tentamos novamente a
  //seleção
  if (bilhetes == 0) { fim++; return selecao(pop); }
  //Fazemos o sorteio do bilhete. O indivíduo com o bilhete sorteado é o escolhido.
  fim = 0; sorteado = rand() % bilhetes + 1;
  for (int i = 0; i < TAM; i++) {
    if (pop[i].vivo) count += QTDBILHETES / (pop[i].erros + 1);
    if (count >= sorteado) return pop[i];
  }
  //Nunca vai acontecer, pois sempre alguem vai ser sorteado, mas é
  //necessário para o compilador não reclamar
  return pop[0];
}


//Realiza o print do indivíduo escolhido
void print(individuo p) {
  if (!p.vivo) { printf("Não foi possível gerar um indivíduo\n"); return; }
  for (int i = 0; i < QTDTURMAS; i++) {
    printf("%d\n", i);
    for (int j = 0; j < 6; j++,printf("\n"))
      for (int k = 0; k < 5; k++)
        printf("%3d", p.grade[i][j][k]);
  }
}

//Função do Algoritmo Genético, conforme descrita no pdf
//Note que uma vez que um indivíduo tenha se tornado ideal(sem erros e
//com todas as matérias na grade), pelo menos um individuo ideal sempre
//existirá na população
individuo genetico() {
  individuo pop[TAM], x, y, z;
  clock_t inicio = clock();
  //Enquanto o programa não tiver excedido o tempo determinado
  //(TIMEMAX), a população continua se reproduzindo
  while ((double) (clock() - inicio) / CLOCKS_PER_SEC < TIMEMAX) {
    individuo novos[TAM];
    for (int i = 0; i < TAM; i++) {
      x = selecao(pop);
      //"fim" é a variavel que conta quantas vezes foi tentado ser feita
      //a seleção, se for diferente de 0, não foi possivel selecionar
      //ninguem e, portanto, a população está morta
      if (fim != 0) { printf("Não foi possível determinar um horário.\n"); return pop[0]; }
      y = selecao(pop);
      if (fim != 0) { printf("Não foi possível determinar um horário.\n"); return pop[0]; }
      z = x.cruza(y);
      //Todo indivíduo novo sofre mutação, assim, obrigamos todas as matérias aparecerem.
      z.muta(100);
      novos[i] = z;
      //Esse if serve apenas para o programa terminar mais rápido, pois
      //o primeiro individuo "aceitavel" produzido é retornado. Caso
      //seja do interesse do usuário, comentar essa linha aumenta as
      //chances de um melhor individuo.
      if (z.vivo && z.erros <= ERR && z.materias.size() == disciplina.size()) return z;
    }
    for (int i = 0; i < TAM; i++) pop[i] = novos[i];
  }
  //Realiza a seleção do melhor individuo
  int menor = INF, pos = 0;
  for (int i = 0; i < TAM; i++)
    if (pop[i].vivo && pop[i].erros <= menor && z.materias.size() == disciplina.size()) {
      menor = pop[i].erros; pos = i;
    }
  return pop[pos];
}


int main(void) {
  in(); //Realiza a leitura da entrada
  //debug(); Função para verificar se a entrada foi lida corretamente
  srand(time(NULL)); //Seed do rand
  individuo p = genetico(); //Execução o algoritmo
  print(p); //Individuo escolhido
  return 0;
}
