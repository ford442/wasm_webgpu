#include "lib_webgpu.h"
#include <vector>
#include <iostream>
#include <assert.h>

WGpuAdapter adapter;
// WGpuCanvasContext canvasContext;
WGpuDevice device;
WGpuQueue queue;
// WGpuRenderPipeline renderPipeline;
WGpuBindGroupLayoutEntry bindGroupLayoutEntry;
WGpuBindGroupLayout bindGroupLayout=0;
WGpuComputePipeline computePipeline;

int bufferSize = 64 * sizeof(float);

std::vector<float>input(bufferSize/sizeof(float));

const char *computeShader =
"@group(0) @binding(0) var<storage,read> inputBuffer: array<f32,64>;"
"@group(0) @binding(1) var<storage,read_write> outputBuffer: array<f32,64>;"
// The function to evaluate for each element of the processed buffer
"fn f(x: f32) -> f32 {"
"return 2.0 * x + 1.0;"
"}"
"@compute @workgroup_size(32)"
"fn computeStuff(@builtin(global_invocation_id) id: vec3<u32>) {"
    // Apply the function f to the buffer element at index id.x:
"outputBuffer[id.x] = f(inputBuffer[id.x]);"
"}";

void raf(WGpuDevice device){
queue=wgpu_device_get_queue(device);
std::cout << "beginning compute commands" << std::endl;

 //   WGpuRenderPassColorAttachment colorAttachment = WGPU_RENDER_PASS_COLOR_ATTACHMENT_DEFAULT_INITIALIZER;
 //   colorAttachment.view = wgpu_texture_create_view(wgpu_canvas_context_get_current_texture(canvasContext), 0);
//   WGpuRenderPassDescriptor passDesc = {};
WGpuComputePassDescriptor computePassDescriptor={};
// computePassDescriptor.timestampWriteCount = 0;
computePassDescriptor.timestampWrites = NULL;
//   passDesc.numColorAttachments = 1;
//   passDesc.colorAttachments = &colorAttachment;
 //  WGpuRenderPassEncoder pass = wgpu_command_encoder_begin_render_pass(encoder, &passDesc);
WGpuBufferDescriptor bufferDescriptor={};
bufferDescriptor.mappedAtCreation=false;
bufferDescriptor.size=bufferSize;
bufferDescriptor.usage=WGPU_BUFFER_USAGE_STORAGE|WGPU_BUFFER_USAGE_COPY_DST;
WGpuBuffer inputBuffer;// =device.createBuffer(bufferDescriptor);
bufferDescriptor.usage=WGPU_BUFFER_USAGE_STORAGE|WGPU_BUFFER_USAGE_COPY_SRC;
WGpuBuffer outputBuffer;// =device.createBuffer(bufferDescriptor);
bufferDescriptor.usage=WGPU_BUFFER_USAGE_COPY_DST|WGPU_BUFFER_USAGE_MAP_READ;
WGpuBuffer mapBuffer;// =device.createBuffer(bufferDescriptor);
// queue.writeBuffer(inputBuffer,0,input.data(),input.size()*sizeof(float));
for(int i=0;i<input.size();++i){
input[i]=0.1f*i;
}
std::cout << "skipping input buffer" << std::endl;
// wgpu_queue_write_buffer(queue,inputBuffer,0,input.data(),input.size()*sizeof(float));
WGpuShaderModuleDescriptor shaderModuleDescriptor={computeShader,0,NULL};
std::cout << "wgpu_device_create_shader_module" << std::endl;
WGpuShaderModule cs=wgpu_device_create_shader_module(device,&shaderModuleDescriptor);
const char * Entry="computeStuff";
std::cout << "wgpu_device_create_compute_pipeline" << std::endl;
computePipeline=wgpu_device_create_compute_pipeline(device,cs,Entry,bindGroupLayout,NULL,0);
	std::cout << "creating encoder" << std::endl;
WGpuCommandEncoder encoder=wgpu_device_create_command_encoder(device,0);
std::cout << "wgpu_command_encoder_begin_compute_pass" << std::endl;
WGpuComputePassEncoder computePass=wgpu_command_encoder_begin_compute_pass(encoder,&computePassDescriptor);

std::cout << "wgpu_compute_pass_encoder_set_pipeline" << std::endl;
wgpu_compute_pass_encoder_set_pipeline(computePass,computePipeline);
//    wgpu_render_pass_encoder_draw(pass, 3, 1, 0, 0);
//    wgpu_render_pass_encoder_end(pass);
uint32_t invocationCount = bufferSize / sizeof(float);
uint32_t workgroupSize = 32;
	// This ceils invocationCount / workgroupSize
uint32_t workgroupCount = (invocationCount + workgroupSize - 1) / workgroupSize;
	
std::cout << "mapBuffer:\n" << std::endl;
std::cout << mapBuffer << std::endl;
std::cout << "dispatch workgropups" << std::endl;
wgpu_compute_pass_encoder_dispatch_workgroups(encoder,workgroupCount,1,1);
// pass.dispatchWorkgroups(workgroupCount, 1, 1);
// pass.end();
// encoder.copyBufferToBuffer(outputBuffer,0,mapBuffer,0,bufferSize);

std::cout << "at wgpu_command_encoder_finish" << std::endl;
WGpuCommandBuffer commandBuffer=wgpu_command_encoder_finish(encoder);
	
// std::cout << "at wgpu_queue_submit_one_and_destroy" << std::endl;
// wgpu_queue_submit_one_and_destroy(queue,commandBuffer);
	
// bool done=false;
// auto handle=mapBuffer.mapAsync(WGPU_MAP_MODE_READ,0,bufferSize{
// const float* output=(const float*)mapBuffer.getConstMappedRange(0,bufferSize);
for(int i=0;i<input.size();++i){

// mapBuffer.unmap();
}
// done=true;
// });
return; // Render just one frame, static content
}

void ObtainedWebGpuDevice(WGpuDevice result,void *userData){
device=result;

//   canvasContext = wgpu_canvas_get_webgpu_context("canvas");
 //  WGpuCanvasConfiguration config = WGPU_CANVAS_CONFIGURATION_DEFAULT_INITIALIZER;
//   config.device = device;
//   config.format = navigator_gpu_get_preferred_canvas_format();
//   wgpu_canvas_context_configure(canvasContext, &config);
raf(device);
}


// shaderModuleDescriptor.code=computeShader;

//   WGpuShaderModule vs = wgpu_device_create_shader_module(device, &shaderModuleDesc);
//   shaderModuleDesc.code = fragmentShader;
/*
WGpuProgrammableStageDescriptor stageDesc{};
stageDesc.module=cs;
stageDesc.entryPoint="computeStuff";
stageDesc.constantCount=0;
stageDesc.constants=NULL;
 */
//   WGpuRenderPipelineDescriptor renderPipelineDesc = WGPU_RENDER_PIPELINE_DESCRIPTOR_DEFAULT_INITIALIZER;
 //  skipped somehow
// WGpuComputePipelineDescriptor computePipelineDesc={};
// computePipelineDesc.compute=stageDesc;
 //  renderPipelineDesc.vertex.module = vs;
//   renderPipelineDesc.vertex.entryPoint = "main";
//   renderPipelineDesc.fragment.module = fs;
//   renderPipelineDesc.fragment.entryPoint = "main";
 //  WGpuColorTargetState colorTarget = WGPU_COLOR_TARGET_STATE_DEFAULT_INITIALIZER;
 //  colorTarget.format = config.format;
 //  renderPipelineDesc.fragment.numTargets = 1;
//   renderPipelineDesc.fragment.targets = &colorTarget;
 
// wgpu_device_create_compute_pipeline(
// WGpuDevice device,
// WGpuShaderModule computeModule, 
// const char *entryPoint NOTNULL,
// WGpuPipelineLayout layout,
// const WGpuPipelineConstant *constants,
// int numConstants);


//   emscripten_set_main_loop(raf,0);

void ObtainedWebGpuAdapter(WGpuAdapter result,void *userData){
adapter=result;
WGpuDeviceDescriptor deviceDesc={};
wgpu_adapter_request_device_async(adapter,&deviceDesc,ObtainedWebGpuDevice,0);
// wgpu_adapter_request_device_async_simple(adapter,ObtainedWebGpuDevice);
// wgpu_adapter_request_device_sync(adapter,&deviceDesc);
// wgpu_adapter_request_device_sync_simple(adapter);
	// ObtainedWebGpuDevice();
}

int main(void){
WGpuRequestAdapterOptions options={};
options.powerPreference=WGPU_POWER_PREFERENCE_HIGH_PERFORMANCE;
// navigator_gpu_request_adapter_sync_simple();
navigator_gpu_request_adapter_async(&options,ObtainedWebGpuAdapter,0);
// navigator_gpu_request_adapter_async_simple(ObtainedWebGpuAdapter);

}
