#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 99999999
int **matrix;

// alocando a matriz de forma contínua
// para mais informações, veja:
// http://c-faq.com/aryptr/dynmuldimary.html
int alloc_matrix(int rows, int cols);

// preenchendo a matriz com os números
// pseudo-aleatórios
int fill_matrix(int rows, int cols);

int find_pnum(int initial_addr, int last_addr);

void show_msg(void);

int main(int argc, char *argv[argc + 1]) {
  int pnum_total = 0;

  int matrix_order[2];

  matrix_order[0] = atoi(argv[1]);
  matrix_order[1] = atoi(argv[1]);

  alloc_matrix(matrix_order[0], matrix_order[1]);
  fill_matrix(matrix_order[0], matrix_order[1]);

  pnum_total = find_pnum(matrix_order[0], matrix_order[1]);

  printf("[%8d] - [%8d] - ", pnum_total, matrix_order[0]);

  free(matrix);

  return 0;
}

int alloc_matrix(int rows, int cols) {
  char *error_text = "Ocorreu um erro ao tentar alocar a matriz!";

  // montando um vetor de ponteiros, com o total
  // de colunas
  matrix = malloc(rows * sizeof(int *));
  if (matrix == NULL) {
    fprintf(stderr, "%s\n", error_text);
    exit(-1);
  }

  // vai no primeiro ponteiro ou coluna "0" e adiciona
  // o primeiro endereço da matriz (que também é um vetor)
  matrix[0] = malloc(rows * cols * sizeof(int));
  if (matrix[0] == NULL) {
    fprintf(stderr, "%s\n", error_text);
    exit(-1);
  }

  // para cada elemento do vetor de ponteiros da
  // primeira coluna, adicione o primeiro endereço
  // do restante da linha.
  for (int i = 1; i < rows; i++)
    matrix[i] = matrix[0] + (i * cols);

  return 0;
}

int fill_matrix(int rows, int cols) {
  srand(12345);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      // o valor máximo é 99.999.999
      // precisamos de 27 bits pra representar,
      // um int já é o suficiente
      matrix[i][j] = rand() % MAX_SIZE;
    }
  }

  return 0;
}

int find_pnum(int initial_addr, int last_addr) {
  // total de números primos encontrados
  int pnum_found = 0;

  for (int i = 0; i < (last_addr * initial_addr); i++) {
    int n_div = 0;

    if ((*matrix)[i] % 2 == 0)
      continue;

    for (int j = 1; j <= (*matrix)[i]; j += 2) {

      int result = (*matrix)[i] % j;

      if (result == 0) {
        n_div++;

        if (n_div > 2)
          break;
      }
    }

    if (n_div == 2)
      pnum_found++;
  }

  return pnum_found;
}
