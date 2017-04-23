#include <stdio.h>
#include <string.h>

#define MAX 50
#define MAX1 10

char turmas[MAX][MAX1];
char materias[MAX][MAX1];
char turnos[MAX][MAX1] = { "Manhã", "Tarde", "Noite" };

int main(void) {
  int id, i, j, k, N;
  char nome[MAX1];
  scanf("%d", &N);
  for (i = 0; i < N; i++) {
    scanf(" %s %d", nome, &id);
    strcpy(turmas[id], nome);
  }
  while (1) {
    scanf(" %s", nome);
    if (!strcmp(nome, "-")) break;
    scanf("%d", &id);
    strcpy(materias[id], nome);
  }
  for (i = 0; i < N; i++) {
    scanf("%d", &id);
    printf("%s;\n", turmas[id]);
    for (j = 0; j < 6; printf("\n"), j++) {
      printf("%s;", j % 2 == 0 ? turnos[j / 2] : "");
      for (k = 0; k < 5; k++) {
        scanf("%d", &id);
        printf("%s;", id == -1 ? "" : materias[id]);
      }
    }
    printf("\n");
  }
  return 0;
}
