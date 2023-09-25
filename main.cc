#include "CachingAllocator.h"
#include "ParticleSoA.h"
#include "ParticleSoAVec.h"
#include <chrono>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


class Timer {
public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}

    void reset() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    double elapsed() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time_;
        return elapsed_time.count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_time_;
};

void press_enter_to_continue(void) {
   printf("Press enter");
   getchar();
   return;
}

int main() {
   printf("Hello! Run top -p %d to follow along!\n", getpid());

    const int N = 1000;
    const int NIter = 100000;
    CachingAllocator allocator;
    Timer timer;

    for (int it = 0; it < NIter; ++it) {
        ParticleSoA<N> particles;

        particles.columns.x = static_cast<double*>(allocator.allocate(sizeof(double) * N));
        particles.columns.y = static_cast<double*>(allocator.allocate(sizeof(double) * N));
        particles.columns.z = static_cast<double*>(allocator.allocate(sizeof(double) * N));
        particles.columns.id = static_cast<int*>(allocator.allocate(sizeof(int) * N));

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
            if (particles.columns.z[i] != particles.columns.x[i] + particles.columns.y[i]) {
                std::cerr << "Result is incorrect!" << std::endl;
                return 1; // Return an error code
            }
        }

        allocator.deallocate(particles.columns.x);
        allocator.deallocate(particles.columns.y);
        allocator.deallocate(particles.columns.z);
        allocator.deallocate(particles.columns.id);
    }  

   std::cout << "Elapsed time: " << timer.elapsed() << " ms " << std::endl;
   press_enter_to_continue();
    
   Timer timer2;

   for(int it = 0; it < NIter ; ++it){
	    ParticleSoAVec particlesVec;
        particlesVec.columns.z.resize(N); 
        for (int i = 0; i < N; ++i) {
            particlesVec.columns.x.push_back(static_cast<double>(i)); 
            particlesVec.columns.y.push_back(static_cast<double>(i)); 
        }

        // Add vectors x and y to z
        for (int i = 0; i < N; ++i) {
            particlesVec.columns.z[i] = particlesVec.columns.x[i] + particlesVec.columns.y[i];
        }
        // Check the result for correctness
        for (int i = 0; i < N; ++i) {
            if (particlesVec.columns.z[i] != particlesVec.columns.x[i] + particlesVec.columns.y[i]) {
                std::cerr << "Result is incorrect!" << std::endl;
                return 1; // Return an error code
            }
        }
   }

   std::cout << "Elapsed time: " << timer2.elapsed() << " ms " << std::endl;

    return 0;
}
