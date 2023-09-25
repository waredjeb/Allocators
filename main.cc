#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <random>
#include "CachingAllocator.h"
#include "ParticleSoA.h"
#include "ParticleSoAVec.h"

class Timer {
 public:
  Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}

  void reset() { start_time_ = std::chrono::high_resolution_clock::now(); }

  double elapsed() const {
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time =
        end_time - start_time_;
    return elapsed_time.count();
  }

 private:
  std::chrono::high_resolution_clock::time_point start_time_;
};

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());

  // Define the range for random numbers
  std::uniform_int_distribution<int> distribution(500, 1000);

  // Generate a random integer between 10 and 10000

  const int NIter = 10000;
  CachingAllocator allocator(false);
  Timer timer;

  for (int it = 0; it < NIter; ++it) {
    const int N = distribution(gen);
    ParticleSoA particles;
    particles.columns.x =
        static_cast<double*>(allocator.allocate(sizeof(double) * N));
    particles.columns.y =
        static_cast<double*>(allocator.allocate(sizeof(double) * N));
    particles.columns.z =
        static_cast<double*>(allocator.allocate(sizeof(double) * N));
    // Fill vectors x and y
    for (int i = 0; i < N; ++i) {
      particles.columns.x[i] = static_cast<double>(i);
      particles.columns.y[i] = static_cast<double>(i * 2);
    }

    // Add vectors x and y to z
    for (int i = 0; i < N; ++i) {
      particles.columns.z[i] = particles.columns.x[i] + particles.columns.y[i];
    }

    // Check the result for correctness
    for (int i = 0; i < N; ++i) {
      if (particles.columns.z[i] !=
          particles.columns.x[i] + particles.columns.y[i]) {
        std::cerr << "Result is incorrect!" << std::endl;
        return 1;  // Return an error code
      }
    }

    allocator.deallocate(particles.columns.z);
    allocator.deallocate(particles.columns.y);
    allocator.deallocate(particles.columns.x);
  }

  std::cout << "Elapsed time: " << timer.elapsed() << " ms " << std::endl;

  Timer timer2;

  for (int it = 0; it < NIter; ++it) {
    int NPart = distribution(gen);
    const int N = NPart;
    ParticleSoAVec particlesVec;
    particlesVec.columns.z.resize(N);
    for (int i = 0; i < N; ++i) {
      particlesVec.columns.x.push_back(static_cast<double>(i));
      particlesVec.columns.y.push_back(static_cast<double>(i));
    }

    // Add vectors x and y to z
    for (int i = 0; i < N; ++i) {
      particlesVec.columns.z[i] =
          particlesVec.columns.x[i] + particlesVec.columns.y[i];
    }
    // Check the result for correctness
    for (int i = 0; i < N; ++i) {
      if (particlesVec.columns.z[i] !=
          particlesVec.columns.x[i] + particlesVec.columns.y[i]) {
        std::cerr << "Result is incorrect!" << std::endl;
        return 1;  // Return an error code
      }
    }
  }

  std::cout << "Elapsed time: " << timer2.elapsed() << " ms " << std::endl;

  return 0;
}
