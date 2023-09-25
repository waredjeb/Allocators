#include <vector>

struct ParticleSoA {
  ParticleSoA() = default;

  struct Columns {
    double* x;
    double* y;
    double* z;
    int* id;
  } columns;
};
