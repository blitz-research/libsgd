#include "shadermodule.h"

namespace sgd {

wgpu::ShaderModule createShaderModule(const wgpu::Device& device, CString source, const char* label) {

	wgpu::ShaderModuleWGSLDescriptor wgslDesc;
	wgslDesc.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
	wgslDesc.code = source.c_str();

	wgpu::ShaderModuleDescriptor desc;
	desc.nextInChain = (wgpu::ChainedStruct*)&wgslDesc;
	desc.label = label;

	auto shaderModule = device.CreateShaderModule(&desc);
	SGD_ASSERT(shaderModule);

	return shaderModule;
}

}
