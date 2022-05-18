#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 99999999
int **matrix;

// alocando a matriz de forma contínua
// para mais informações, veja:
// http://c-faq.com/aryptr/dynmuldimary.html
int alloc_matrix(int rows, int cols);

int show_matrix(int rows, int cols);

// preenchendo a matriz com os números
// pseudo-aleatórios
int fill_matrix(int rows, int cols);

void show_msg(void);

int main(void) {

  while (1) {
    int option = 0;
    int matrix_order[2];

    show_msg();

    printf("Informe uma opção: ");
    scanf("%d", &option);

    switch (option) {
    case 1:
      printf("Informe a ordem da matrix (NxM): \n");

      printf("Linhas: ");
      scanf("%d", &matrix_order[0]);

      printf("Colunas: ");
      scanf("%d", &matrix_order[1]);

      if (alloc_matrix(matrix_order[0], matrix_order[1]) == 0) {
        printf("\nMatriz %dx%d alocada com sucesso!\n", matrix_order[0],
               matrix_order[1]);
      }

      break;

    default:
      puts("Opção inexistente!");
      exit(-1);
    }
  }

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

int show_matrix(int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printf("[%8d] ", matrix[i][j]);
      if (j + 1 == cols)
        puts("");
    }
  }

  // por se tratar de um vetor também, podemos
  // acessar essa matriz com apenas um laço de
  // repetição
  /*
  for(int i = 0; i < (rows * cols); i++) {
    printf("[%8d] ", (*matrix)[i]);
    if((i + 1) % cols == 0)
      puts("");
  }
  */

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

void show_msg(void) {
  char *msg_1 = "1 - Alocar matriz";

  printf("%s\n", msg_1);
}
