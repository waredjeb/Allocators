#include <iostream>
#include <memory>

struct ParticleSoAVec {
    struct Columns {
        std::vector<double> x;
        std::vector<double> y;
        std::vector<double> z;
        std::vector<int> id;
    } columns;

    ParticleSoAVec(){};

    // No explicit destructor needed; std::unique_ptr handles deallocation

    // Define custom constructor and assignment operator if needed
    ParticleSoAVec(const ParticleSoAVec&) = delete;
    ParticleSoAVec& operator=(const ParticleSoAVec&) = delete;
};

