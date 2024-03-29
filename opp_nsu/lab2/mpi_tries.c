#include <omp.h>
#include <sys/time.h>
#include <time.h>

#include <cmath>
#include <iostream>

constexpr float EPS = 0.00001f;
constexpr float TAU = 0.01f;

void mul(const float* partA, const float* x, float* res, int matrixSize) {
  for (int i = 0; i < matrixSize; ++i) {
    res[i] = 0.0f;
    for (int j = 0; j < matrixSize; ++j) {
      res[i] += partA[i * matrixSize + j] * x[j];
    }
  }
}

float vectorAbs(const float* x, int size) {
  float len = 0.0f;
  for (int i = 0; i < size; ++i) len += x[i] * x[i];
  return len;
}

void vectorSub(const float* x, const float* y, float* res, float k, int size) {
  for (int i = 0; i < size; ++i) res[i] = x[i] - k * y[i];
}

void nextX(float* x, const float* b, float* Ax, int matrixSize, float* buffer) {
  vectorSub(Ax, b, buffer, 1, matrixSize);

  vectorSub(x, buffer, x, TAU, matrixSize);
}

bool ending(float* x, float* partA, float* b, float* Ax, int matrixSize,
            float* buffer) {
  mul(partA, x, Ax, matrixSize);

  vectorSub(Ax, b, buffer, 1, matrixSize);

  float div = (vectorAbs(buffer, matrixSize) / vectorAbs(b, matrixSize));

  std::cout << div << std::endl;

  return div < EPS * EPS;
}

void init_1(float*& x, float*& partA, float*& b, float*& Ax, int matrixSize) {
  partA = new float[matrixSize * matrixSize];
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      partA[i * matrixSize + j] = 1.0f;
    }
    partA[i * matrixSize + i] = 2.0f;
  }

  b = new float[matrixSize];
  for (int i = 0; i < matrixSize; ++i) {
    b[i] = (float)matrixSize + 1;
  }
  x = new float[matrixSize];
  for (int i = 0; i < matrixSize; ++i) {
    x[i] = 0.0f;
  }

  Ax = new float[matrixSize];
}

void printRes(float* x, int size) {
  for (int i = 0; i < size; ++i) std::cout << x[i] << " ";
}

int main(int argc, char* argv[]) {
  // omp_set_num_threads(THREADS_NUM);

  int size = 2;
  float* partA;
  float* b;
  float* x;

  float* Ax;

  float* buffer = new float[size];

  init_1(x, partA, b, Ax, size);

  // struct timeval start_time, end_time;
  // gettimeofday(&start_time, 0);
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  while (!ending(x, partA, b, Ax, size, buffer)) {
    nextX(x, b, Ax, size, buffer);
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  // gettimeofday(&end_time, 0);

  printRes(x, size);

  // std::cout << "\nElapsed time is "
  //     << (int)((end_time.tv_sec - start_time.tv_sec) * 1000000 + end_time.tv
  //     _usec - start_time.tv_usec)
  //     << '\n';

  std::cout << "\nElapsed time is "
            << (float)(end.tv_sec - start.tv_sec) +
                   (1e-9 * (end.tv_nsec - start.tv_nsec))
            << std::endl;

  delete[] partA;
  delete[] b;
  delete[] x;
  delete[] Ax;
  delete[] buffer;
  return 0;
}