#include <vector>

template <int N>
class ParticleSoA {

	public:
		ParticleSoA() = default;
	
		struct Columns{
 			double* x;
			double* y;
			double* z;
			int* id;
		} columns;
			
};
