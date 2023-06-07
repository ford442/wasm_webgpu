#include "lib_webgpu.h"
#include <vector>
#include <iostream>
#include <assert.h>

WGpuAdapter adapter=0;
WGpuDevice device=0;
WGpuBindGroupLayout bindGroupLayout=0;
WGpuComputePipeline computePipeline=0;
WGpuQueue queue=0;
WGpuComputePassDescriptor computePassDescriptor={};
WGpuBufferDescriptor bufferDescriptor={};
WGpuBufferBindingLayout bufferBindingLayout1={};
WGpuBufferBindingLayout bufferBindingLayout2={};
WGpuBuffer inputBuffer=0;
WGpuShaderModuleDescriptor shaderModuleDescriptor={};
WGpuBuffer outputBuffer=0;
WGpuBuffer mapBuffer=0;
WGpuBuffer uniBuffer=0;
WGpuShaderModule cs=0;
WGpuCommandBufferDescriptor commandBufferDescriptor={};
WGpuCommandBuffer commandBuffer=0;
WGpuCommandEncoder encoder=0;
WGpuComputePassEncoder computePass=0;
WGpuBindGroupLayoutEntry bindGroupLayoutEntries[2]={};
WGpuBindGroupEntry bindGroupEntry[2]={};
WGpuBindGroup bindGroup=0;
WGpuPipelineLayout pipelineLayout=0;
WGpuCommandEncoderDescriptor commandEncoderDescriptor={};
int bufferSize = 64 * sizeof(float);

const char *computeShader =
"@group(0) @binding(0) var<storage,read> inputBuffer: array<f32,64>;"
"@group(0) @binding(1) var<storage,read_write> outputBuffer: array<f32,64>;"
// The function to evaluate for each element of the processed buffer
"fn f(x: f32) -> f32 {"
"return 2.0 * x + 0.42;"
"}"
"@compute @workgroup_size(32)"
"fn computeStuff(@builtin(global_invocation_id) id: vec3<u32>) {"
    // Apply the function f to the buffer element at index id.x:
"outputBuffer[id.x] = f(inputBuffer[id.x]);"
"}";

void raf(WGpuDevice device){
queue=wgpu_device_get_queue(device);
std::cout << "beginning compute commands" << std::endl;
std::vector<float>input(bufferSize/sizeof(float));
// computePassDescriptor.timestampWrites = NULL;
computePassDescriptor.numTimestampWrites = uint32_t(0);
bufferDescriptor.mappedAtCreation=false;
bufferDescriptor.size=bufferSize;
bufferDescriptor.usage=WGPU_BUFFER_USAGE_MAP_READ|WGPU_BUFFER_USAGE_COPY_DST;
mapBuffer=wgpu_device_create_buffer(device,&bufferDescriptor);
bufferDescriptor.usage=WGPU_BUFFER_USAGE_STORAGE|WGPU_BUFFER_USAGE_COPY_SRC;
bufferDescriptor.mappedAtCreation=false;
bufferDescriptor.usage=WGPU_BUFFER_USAGE_STORAGE|WGPU_BUFFER_USAGE_COPY_DST;
inputBuffer=wgpu_device_create_buffer(device,&bufferDescriptor);
outputBuffer=wgpu_device_create_buffer(device,&bufferDescriptor);
bufferDescriptor.usage=WGPU_BUFFER_USAGE_UNIFORM|WGPU_BUFFER_USAGE_COPY_DST;
for(int i=0;i<input.size();++i){
input[i]=21.0021f;
}
std::cout << "not skipping input buffer" << std::endl;
wgpu_queue_write_buffer(queue,inputBuffer,0,input.data(),input.size()*sizeof(float));
shaderModuleDescriptor={computeShader,0,NULL};
std::cout << "wgpu_device_create_shader_module" << std::endl;
cs=wgpu_device_create_shader_module(device,&shaderModuleDescriptor);
std::cout << "create bindgroup layout" << std::endl;
bufferBindingLayout1.type=3;
bufferBindingLayout2.type=2;
bindGroupLayoutEntries[0].binding=0;
bindGroupLayoutEntries[0].visibility=WGPU_SHADER_STAGE_COMPUTE;
bindGroupLayoutEntries[0].type=1;
bindGroupLayoutEntries[0].layout.buffer=bufferBindingLayout1;
bindGroupLayoutEntries[1].binding=1;
bindGroupLayoutEntries[1].visibility=WGPU_SHADER_STAGE_COMPUTE;
bindGroupLayoutEntries[1].type=1;
bindGroupLayoutEntries[1].layout.buffer=bufferBindingLayout2;
bindGroupLayout=wgpu_device_create_bind_group_layout(device,bindGroupLayoutEntries,2);
const char * Entry="computeStuff";
std::cout << "wgpu_device_create_compute_pipeline" << std::endl;
pipelineLayout=wgpu_device_create_pipeline_layout(device,&bindGroupLayout,1);
computePipeline=wgpu_device_create_compute_pipeline(device,cs,Entry,pipelineLayout,NULL,0);
std::cout << "create bindgroup" << std::endl;
bindGroupEntry[0].binding=0;
bindGroupEntry[0].resource=inputBuffer;
bindGroupEntry[1].binding=1;
bindGroupEntry[1].resource=outputBuffer;
bindGroup=wgpu_device_create_bind_group(device,bindGroupLayout,bindGroupEntry,2);
std::cout << "creating encoder" << std::endl;
encoder=wgpu_device_create_command_encoder(device,0);
std::cout << "wgpu_command_encoder_begin_compute_pass" << std::endl;
computePass=wgpu_command_encoder_begin_compute_pass(encoder,&computePassDescriptor);
std::cout << "wgpu_encoder_set_bind_group" << std::endl;
wgpu_encoder_set_bind_group(computePass,0,bindGroup,0,0);
std::cout << "wgpu_compute_pass_encoder_set_pipeline" << std::endl;
wgpu_compute_pass_encoder_set_pipeline(computePass,computePipeline);
uint32_t invocationCount = bufferSize / sizeof(float);
uint32_t workgroupSize = 32;
	// This ceils invocationCount / workgroupSize
uint32_t workgroupCount = (invocationCount + workgroupSize - 1) / workgroupSize;
std::cout << "inputBuffer: " << inputBuffer << std::endl;
std::cout << "input: " << input[0] << std::endl;
std::cout << "at wgpu_command_encoder_copy_buffer_to_buffer" << std::endl;
wgpu_command_encoder_copy_buffer_to_buffer(encoder,outputBuffer,0,mapBuffer,0,bufferSize);
std::cout << "dispatch workgroups:" << workgroupCount << ",1,1" << std::endl;
// wgpu_compute_pass_encoder_dispatch_workgroups(computePass,workgroupCount,uint32_t(1),uint32_t(1));
wgpu_compute_pass_encoder_dispatch_workgroups(computePass,uint32_t(32));
	// copy output buff
std::cout << "at commandBuffer=wgpu_encoder_finish(encoder);" << std::endl;
commandBuffer=wgpu_encoder_finish(encoder);

WGpuOnSubmittedWorkDoneCallback onComputeDone=[](WGpuQueue queue,void *userData){
WGpuBufferMapCallback mapCallback=[](WGpuBuffer buffer,void *userData, WGPU_MAP_MODE_FLAGS mode, double_int53_t offset, double_int53_t size){
std::cout << "at mapCallback!" << std::endl;
std::cout << buffer << std::endl;
std::cout << "test" << std::endl;
std::cout << "wgpu_buffer_read_mapped_range" << std::endl;
auto getOutput = wgpu_buffer_get_mapped_range(buffer,uint32_t(0),size);
std::cout << "buffer read_mapped_range" << std::endl;
std::cout << getOutput << std::endl;
wgpu_encoder_end(computePass);
};	
std::cout << "at wgpu WGpuOnSubmittedWorkDoneCallback!" << std::endl;
std::cout << "mapBuffer:" << mapBuffer << std::endl;
std::cout << "outputBuffer:" << outputBuffer << std::endl;
std::cout << "mapBuffer:" << mapBuffer << std::endl;
std::cout << "wgpu_encoder_end" << std::endl;
std::cout << "wgpu_buffer_map_async" << std::endl;
WGPU_MAP_MODE_FLAGS mode1=0x1; // WGPU_MAP_MODE_READ
void *userDataA;
wgpu_buffer_map_async(mapBuffer,mapCallback,&userDataA,mode1,uint32_t(0),bufferSize);
};
wgpu_queue_set_on_submitted_work_done_callback(queue,onComputeDone,0);
std::cout << "at wgpu_queue_submit_one_and_destroy" << std::endl;
// wgpu_queue_submit_one_and_destroy(queue,commandBuffer);

return;
}

void ObtainedWebGpuDevice(WGpuDevice result,void *userData){
device=result;
raf(device);
}
/*
//   WGpuShaderModule vs = wgpu_device_create_shader_module(device, &shaderModuleDesc);
WGpuProgrammableStageDescriptor stageDesc{};
stageDesc.module=cs;
stageDesc.entryPoint="computeStuff";
stageDesc.constantCount=0;
stageDesc.constants=NULL;
WGpuRenderPipelineDescriptor renderPipelineDesc = WGPU_RENDER_PIPELINE_DESCRIPTOR_DEFAULT_INITIALIZER;
//  skipped somehow
WGpuComputePipelineDescriptor computePipelineDesc={};
computePipelineDesc.compute=stageDesc;
wgpu_device_create_compute_pipeline(WGpuDevice device,WGpuShaderModule computeModule, const char *entryPoint NOTNULL,WGpuPipelineLayout layout,const WGpuPipelineConstant *constants,int numConstants);
 emscripten_set_main_loop(raf,0);
 */

void ObtainedWebGpuAdapter(WGpuAdapter result,void *userData){
adapter=result;
WGpuDeviceDescriptor deviceDescriptor={};
wgpu_adapter_request_device_async(adapter,&deviceDescriptor,ObtainedWebGpuDevice,0);
}

int main(void){
WGpuRequestAdapterOptions options={};
options.powerPreference=WGPU_POWER_PREFERENCE_HIGH_PERFORMANCE;
navigator_gpu_request_adapter_async(&options,ObtainedWebGpuAdapter,0);
}
