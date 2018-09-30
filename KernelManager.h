#ifndef __EKH_SCRATCH_GRAPHICS_1_KERNEL_MANAGER__
#define __EKH_SCRATCH_GRAPHICS_1_KERNEL_MANAGER__

#include <map>
#include <string>

struct Kernel {
	int samples;
	float* weights;
};

class KernelManager {
private:
	std::map<std::string, Kernel> mKernels;
public:
	void add(std::string name, Kernel kernel);
	Kernel get(std::string name) const;
	void clear();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_KERNEL_MANAGER__