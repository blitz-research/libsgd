#include "gaussianfilter.h"

namespace sgd {

Vector<Vec4f> generateKernel(uint32_t size, float sigma) {

	Vector<Vec4f> kernel(size);
	float sum = 0;
	for (int i = 0; i < size; ++i) {
		float x = (float)i - (float)size / 2 + .5f;
		kernel[i] = Vec4f(std::exp(-(x * x) / (2 * sigma * sigma)) / (std::sqrt(2 * pi) * sigma));
		sum += kernel[i].w;
	}
	for (int i = 0; i < size; ++i) {
		kernel[i] /= sum;
	}
	return kernel;
}

}
