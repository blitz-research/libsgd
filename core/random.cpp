#include "random.h"

#include <ctime>

namespace sgd {

namespace {

struct ranctx {
	uint32_t a = 3;
	uint32_t b = 11;
	uint32_t c = 23;
	uint32_t d = 37;
};

ranctx r;

#define rot(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
uint32_t rndval(ranctx* x) {
	uint32_t e = x->a - rot(x->b, 27);
	x->a = x->b ^ rot(x->c, 17);
	x->b = x->c + x->d;
	x->c = x->d + e;
	x->d = e + x->a;
	return x->d;
}

void rndinit(ranctx* x, uint32_t seed) {
	uint32_t i;
	x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
	for (i = 0; i < 20; ++i) { (void)rndval(x); }
}

} // namespace

void seedRnd(uint32_t seed) {
	rndinit(&r, seed);
}

void seedRnd() {
	uint32_t tm = std::time(nullptr);
	seedRnd(tm);
}

float rnd() {
	return float(rndval(&r) & 0xffffff) / float(0x1000000);
}

float rnd(float max) {
	return rnd() * max;
}

float rnd(float min, float max) {
	return rnd(max - min) + min;
}

}
