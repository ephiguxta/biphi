#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 99999999

int **matrix;

// controlando o acesso das threads a variável global
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int total_prime_n = 0;

// alocando a matriz de forma contínua
// para mais informações, veja:
// http://c-faq.com/aryptr/dynmuldimary.html
int alloc_matrix(int rows, int cols);

int show_matrix(int rows, int cols);

// preenchendo a matriz com os números
// pseudo-aleatórios
int fill_matrix(int rows, int cols);

struct pair_addr {
  int initial_addr;
  int last_addr;
};
// essa função será responsável por achar
// os números primos, a estrutura acima serve
// de base para separar os endereços das submatrizes
// para as threads
void *find_pnum(void *data);

struct parse_data {
  int addr;
  char remainder;
  char n_times;
};
// pega o número inteiro que pode dividir a matriz corretamente,
// também o número restante, caso a matriz não for de ordem par
// esse dado restante será usado para a última thread operar sobre.
struct parse_data slice_matrix(int rows, int cols, int threads);

void data_share(int rows, int cols, int threads, struct pair_addr *index_addr);

void show_msg(void);

int main(int argc, char *argv[argc + 1]) {
  int matrix_order[2];
  int threads = 0;

  matrix_order[0] = atoi(argv[1]);
  matrix_order[1] = atoi(argv[1]);

  threads = atoi(argv[2]);

  // cada thread vai receber um par de endereços
  // e um variável com seu id
  struct pair_addr *index_addr;
  pthread_t *threads_id;

  alloc_matrix(matrix_order[0], matrix_order[1]);
  fill_matrix(matrix_order[0], matrix_order[1]);

  threads_id = (pthread_t *)malloc(threads * sizeof(pthread_t));
  if (threads_id == NULL) {
    fprintf(stderr, "Erro ao alocar o threads_id\n");
    exit(-1);
  }

  index_addr = (struct pair_addr *)malloc(threads * sizeof(struct pair_addr));
  if (index_addr == NULL) {
    fprintf(stderr, "Erro ao alocar o index_addr\n");
    exit(-1);
  }

  // separando os endereços e os indíces restantes, caso não for uma matriz
  // quadrada, os elementos restante ficam para a última thread
  data_share(matrix_order[0], matrix_order[1], threads, index_addr);

  for (int j = 0; j < threads; j++)
    pthread_create(&threads_id[j], NULL, find_pnum, &index_addr[j]);

  for (int k = 0; k < threads; k++)
    pthread_join(threads_id[k], NULL);

  printf("[%8d] - [%8d] - ", total_prime_n, matrix_order[0]);

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

void *find_pnum(void *data) {
  int limit = sqrt(MAX_SIZE);
  char is_prime = 0;

  struct pair_addr *index_addr = (struct pair_addr *)data;

  for (int i = index_addr->initial_addr; i <= index_addr->last_addr; i++) {

    if ((*matrix)[i] % 2 == 0)
      continue;

    is_prime = 1;

    for (int j = 3; j <= limit; j += 2) {

      int result = (*matrix)[i] % j;

      if (result == 0) {
        is_prime = 0;
        break;
      }
    }

    if (is_prime) {
      pthread_mutex_lock(&mutex);
      total_prime_n++;
      pthread_mutex_unlock(&mutex);
    }
  }

  return NULL;
}

void data_share(int rows, int cols, int threads, struct pair_addr *slice_addr) {
  struct parse_data slice_data = slice_matrix(rows, cols, threads);

  int initial_addr = 1;
  int final_addr = 0;

  for (char i = 1; i <= slice_data.n_times; i++) {

    // tratando a última "fatia" da matriz
    if (slice_data.remainder && i == slice_data.n_times) {

      final_addr += slice_data.addr;
      final_addr += slice_data.remainder;

    } else {
      final_addr += slice_data.addr;
    }

    slice_addr[i - 1].initial_addr = initial_addr - 1;
    slice_addr[i - 1].last_addr = final_addr - 1;

    // evitando que duas threas operem no mesmo endereço
    // da matriz
    initial_addr = final_addr + 1;
  }
}

/*
struct parse_data {
  int addr;
  char remainder;
  char n_times;
};
*/
struct parse_data slice_matrix(int rows, int cols, int threads) {
  // quantas vezes vamos dividir a matriz, elementos restantes
  // e o multiplicador inteiro
  struct parse_data slice_data;

  slice_data.remainder = 0;

  int t_elements = rows * cols;

  // caso não haja a possibilidade de uma divisão
  // inteira, os números "á margem" são contados
  if (t_elements % threads) {
    // esse valor será inserido na última thread
    slice_data.remainder = t_elements - (floor(t_elements / threads) * threads);
  }

  // removendo os números a márgem e transformando
  // em uma matriz de ordem par
  t_elements -= slice_data.remainder;

  // multiplicador inteiro
  slice_data.addr = t_elements / threads;
  // números de sub-matrizes
  slice_data.n_times = t_elements / slice_data.addr;

  return slice_data;
}
