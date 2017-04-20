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
#define TAM 10 //Tamanho da população
#define INF 112345678
#define QTDBILHETES 10
#define QTDTURMAS 9
#define TIMEMAX 8
#define LIVRE -1
typedef pair< int , vector<int> > iv;

map<string, int> professores, turmas, disciplina;
vector<int> rejeitados[MAX], horarios[MAX];
vector<iv> infdisc[MAX];

/*Nosso grid
       SEG  TER  QUA  QUI  SEX  I J
  MAT   0    1    2    3    4   0 X
        5    6    7    8    9   1 x
  VESP 10   11   12   13   14   2 x
       15   16   17   18   19   3 x
  NOT  20   21   22   23   24   4 x
       25   26   27   28   29   5 x
 */

//returna true se o professor não tem problemas em dar aula nesse horário
int profgosta(vector<int> idprof, int turno) {
  for (int i = 0; i < (int) idprof.size(); i++)
    for (int j = 0, id = idprof[i]; j < (int) rejeitados[id].size(); j++)
      if (rejeitados[id][j] == turno) return 0;
  return 1;
}

struct individuo {
  int grade[QTDTURMAS][6][5]; //10 periodos, 5 dias, 6 aulas (2 manhã, 2 tarde, 2 noite)
  set <int> materias; //Matérias já incluidas no horário
  int vivo; //Grade é valida quando está em 1
  int erros;

  individuo () { memset(this->grade, LIVRE, sizeof(this->grade)); vivo = 1; erros = 0; }
  individuo (set<int> m) {
    memset(this->grade, LIVRE, sizeof(this->grade)); vivo = 1; erros = 0;
    for (auto const& i : m) this->addmateria(i);
  }

  //retorna true se há uma restrição nesse horário
  int restrito(vector<int> idprof, int turno) {
    if ((turno / 5 != 5 && turno / 5 != 0) || (turno / 5 == 0 && turno % 5 == 0) || (turno / 5 == 5 && turno % 5 == 4)) return 0;
    int  m, prof1, prof2;
    for (int j = 0; j < QTDTURMAS; j++) {
      if (turno / 5 == 0)  m = this->grade[j][5][turno % 5 - 1]; //primeiro horario manhã, verificamos a materia da noite anterior
      else m = this->grade[j][0][turno % 5 + 1]; //ultimo horario da noite, verificamos a primeira materia da manhã seguinte
      if (m == LIVRE) continue;
      for (auto const& d : infdisc[m])
        if (d.first == j) //é o mesmo periodo, ou seja, é a matéria correta
          for (int k = 0; k < (int) d.second.size(); k++) {
            prof1 = d.second[k];
            for (int i = 0; i < (int) idprof.size(); i++){
              prof2 = idprof[i];
              if (prof1 == prof2) return 1;
            }}}
    return 0;
  }

  //retorna true se o professor está livre no horário (replicação de código)
  int proflivre(vector<int> idprof, int turno) {
    int prof1, prof2, m;
    for (int j = 0; j < QTDTURMAS; j++) {
      m = this->grade[j][turno / 5][turno % 5];
      for (auto const& d : infdisc[m])
        if (d.first == j) //é o mesmo periodo, ou seja, é a matéria correta
          for (int k = 0; k < (int) d.second.size(); k++) {
            prof1 = d.second[k];
            for (int i = 0; i < (int) idprof.size(); i++){
              prof2 = idprof[i];
              if (prof1 == prof2) return 0;
            }}}
    return 1;
  }

  void addmateria(int m) {
    int i, j, periodo, turno, count = 2; //Devemos adicionar duas vezes a matéria na grade
    int possivel[2], p = 0;
    for (int y = 0; y < (int)infdisc[m].size(); y++) {
      periodo = infdisc[m][y].first;
      vector<int> prof = infdisc[m][y].second;
      for (int k = 0; k < (int)horarios[periodo].size(); k++) {
        turno = horarios[periodo][k];
        i = turno / 5; j = turno % 5;
        if (!this->proflivre(prof, turno)) continue;
        if (this->grade[periodo][i][j] == LIVRE && profgosta(prof, turno)) {
          this->grade[periodo][i][j] = m; count--;
        } else if (!this->restrito(prof, turno) && p < 2) possivel[p++] = turno;
      }
      if (count > p) this->vivo = 0; //Não foi possivel colocar a matéria :(
      while (p-- > 0) { this->grade[periodo][possivel[p] / 5] [possivel[p] % 5] = m; this->erros++; }
    }
  }

  individuo cruza(individuo b) {
    set <int> m;
    individuo p;
    if (this->erros < b.erros) p = *this;
    else p = b;
    for (auto const& i : b.materias) m.insert(i);
    for (auto const& i : this->materias) m.insert(i);
    for (auto const& i : p.materias) m.erase(i);
    for (auto const& i : m) p.addmateria(i);
    return p;
  }

  void muta(int p) { // 0 <= P <= 100
    if (p < rand() % 100) return;
    int t = disciplina.size() - this->materias.size();
    t = rand() % t;
    for (auto const& i: disciplina)
      if (this->materias.find(i.second) == this->materias.end()) {
        if (t) { t--; continue; }
        this->materias.insert(i.second); break;
      }
    individuo q(this->materias);
    for (int i = 0; i < 10; i++)
      for (int j = 0; j < 6; j++)
        for (int k = 0; k < 5; k++)
          this->grade[i][j][k] = q.grade[i][j][k];
  }
};


void in() {
  char nome[20], materia[20], sem[20];
  int N, M, cont = 0, tmp, prof, tur, mat, novo;
  //Lê os professores
  scanf("%d", &N);
  while (N--) {
    scanf(" %s %d", nome, &M);
    professores[string(nome)] = cont; prof = cont++;
    while (M--) { scanf("%d", &tmp); rejeitados[prof].push_back(tmp); }
  }

  //Lê as turmas/semestres
  scanf("%d", &N); cont = 0;
  while (N--) {
    scanf(" %s %d %d", nome, &tmp, &M);
    turmas[string(nome)] = cont; tur = cont++;
    while (M--) { scanf("%d", &tmp); horarios[tur].push_back(tmp);  }
  }

  //Lê as disciplinas
  scanf("%d", &N);
  while (N--) {
    scanf(" %s %d %s %s", materia, &tmp, sem, nome);
    if (disciplina.find(string(materia)) == disciplina.end())
      disciplina[string(materia)] = cont++;
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

individuo selecao(individuo pop[TAM]) {
  int bilhetes = 0, count = 0, sorteado;
  for (int i = 0; i < TAM; i++)
    if (pop[i].vivo) bilhetes += QTDBILHETES / (pop[i].erros + 1);
    else { pop[i] = individuo(pop[i].materias);} //Se o inidivuo estava morto, ressuscita ;)
  sorteado = rand() % bilhetes + 1;
  for (int i = 0; i < TAM; i++) {
    count += QTDBILHETES / (pop[i].erros + 1);
    if (count >= sorteado) return pop[i];
  }
  return pop[0]; //Nunca vai acontecer, é só para o compilador não reclamar
}

individuo genetico(int err) {
  individuo pop[TAM], x, y, z;
  clock_t inicio = clock();
  while ((double) (clock() - inicio) / CLOCKS_PER_SEC < TIMEMAX) {
    individuo novos[TAM];
    for (int i = 0; i < TAM; i++) {
      x = selecao(pop);
      y = selecao(pop);
      z = x.cruza(y);
      z.muta(MAX - z.materias.size());
      novos[i] = z;
      //if (z.erros <= err && z.materias.size() == disciplina.size()) return z;
    }
    for (int i = 0; i < TAM; i++) pop[i] = novos[i];
  }
  int menor = INF, pos = 0;
  for (int i = 0; i < TAM; i++)
    if (pop[i].erros < menor) { menor = pop[i].erros; pos = i; }
  return pop[pos];
}

int main(void) {
  in();
  debug();
  srand(time(NULL)); //Seed do rand
  individuo p = genetico(ERR);
  return 0;
}
