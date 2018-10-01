#include "KernelManager.h"

void KernelManager::add(std::string name, Kernel kernel) {
	if (mKernels.count(name))
		delete mKernels[name].weights;
	mKernels[name] = kernel;
}

Kernel KernelManager::get(std::string name) const {
	if (mKernels.count(name))
		return mKernels.at(name);
	return Kernel{ 0,NULL };
}

void KernelManager::clear() {
	for (auto kernel : mKernels)
		delete kernel.second.weights;
	mKernels.clear();
}
