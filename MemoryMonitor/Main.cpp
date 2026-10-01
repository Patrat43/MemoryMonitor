#include <array>
#include "MemoryMonitor.h"


int main() {
	float* p = _new float(3.14f);
	float* john = _new float[10];
	delete p;
	delete[] john;
	std::unique_ptr<int> x = std::make_unique<int>(66);
	std::shared_ptr<float> y = std::make_shared<float>(3.14);
}