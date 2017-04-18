#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

#define MAX 50

typedef pair< int , vector<int> > iv;

map<string, int> professores, turmas, disciplina;
vector<int> rejeitados[MAX], horarios[MAX];
vector<iv> infdisc[MAX];


struct individuo {
  int grade[10][5][6]; //10 periodos, 5 dias, 6 aulas (2 manhã, 2 tarde, 2 noite)
  set <int> materias; //Matérias já incluidas no horário
  int vivo; //Grade é valida quando está em 1

  individuo () { memset(this->grade, 0, sizeof(this->grade)); vivo = 1; }
  individuo (set<int> m) {
    //Completar
  }

  individuo cruza(individuo b) {
    set <int> m;
    for (auto const& i : b.materias) m.insert(i);
    for (auto const& i : this->materias) m.insert(i);
    //return individuo(materias);
    individuo p;
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
      for (int j = 0; j < 5; j++)
        for (int k = 0; k < 6; k++)
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
  scanf("%d", &N);
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


int main(void) {
  in();
  debug();
  srand(time(NULL)); //Seed do rand
  individuo o, p;
  for (int i = 0; i < 5; printf("\n"), i++)
    for (int j = 0; j < 6; j++)
      printf("[%d]", p.grade[0][i][j]);
  return 0;
}
