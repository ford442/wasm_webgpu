#include "lib_webgpu.h"

WGpuAdapter adapter;
// WGpuCanvasContext canvasContext;
WGpuDevice device;
WGpuQueue queue;
// WGpuRenderPipeline renderPipeline;
WGpuComputePipeline computePipeline;

EM_BOOL raf(double time, void *userData){
WGpuCommandEncoder encoder=wgpu_device_create_command_encoder(device,0);
 //   WGpuRenderPassColorAttachment colorAttachment = WGPU_RENDER_PASS_COLOR_ATTACHMENT_DEFAULT_INITIALIZER;
 //   colorAttachment.view = wgpu_texture_create_view(wgpu_canvas_context_get_current_texture(canvasContext), 0);
//   WGpuRenderPassDescriptor passDesc = {};
WGpuComputePassDescriptor computePassDescriptor={};
 	computePassDescriptor.timestampWriteCount = 0;
	computePassDescriptor.timestampWrites = NULL;
//   passDesc.numColorAttachments = 1;
//   passDesc.colorAttachments = &colorAttachment;
 //  WGpuRenderPassEncoder pass = wgpu_command_encoder_begin_render_pass(encoder, &passDesc);
WGpuComputePassEncoder pass=wgpu_command_encoder_begin_compute_pass(encoder,&computePassDescriptor);
wgpu_compute_pass_encoder_set_pipeline(pass,computePipeline);
//    wgpu_render_pass_encoder_draw(pass, 3, 1, 0, 0);
//    wgpu_render_pass_encoder_end(pass);
uint32_t invocationCount = bufferSize / sizeof(float);
uint32_t workgroupSize = 32;
	// This ceils invocationCount / workgroupSize
uint32_t workgroupCount = (invocationCount + workgroupSize - 1) / workgroupSize;
pass.dispatchWorkgroups(workgroupCount, 1, 1);
pass.end();
encoder.copyBufferToBuffer(outputBuffer,0,mapBuffer,0,bufferSize);
WGpuCommandBuffer commandBuffer=wgpu_command_encoder_finish(encoder);
wgpu_queue_submit_one_and_destroy(queue,commandBuffer);
return EM_FALSE; // Render just one frame, static content
}

void ObtainedWebGpuDevice(WGpuDevice result,void *userData){
device=result;
queue=wgpu_device_get_queue(device);
//   canvasContext = wgpu_canvas_get_webgpu_context("canvas");
 //  WGpuCanvasConfiguration config = WGPU_CANVAS_CONFIGURATION_DEFAULT_INITIALIZER;
//   config.device = device;
//   config.format = navigator_gpu_get_preferred_canvas_format();
//   wgpu_canvas_context_configure(canvasContext, &config);
std::vector<float>input(bufferSize/sizeof(float));
for(int i=0;i<input.size();++i){
input[i]=0.1f*i;
}

queue.writeBuffer(inputBuffer,0,input.data(),input.size()*sizeof(float));

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
 
WgpuBuffer inputBuffer=NULL;
WgpuBuffer outputBuffer=NULL;
WgpuBuffer mapBuffer=NULL;

uint_t bufferSize = 64 * sizeof(float);

WGpuBufferDescriptor bufferDescriptor={};
bufferDescriptor.mappedAtCreation=false;
bufferDescriptor.size=bufferSize;
bufferDescriptor.usage=WGPU_BUFFER_USAGE_STORAGE|WGPU_BUFFER_USAGE_COPY_DST;
inputBuffer=device.createBuffer(bufferDescriptor);
bufferDescriptor.usage=WGPU_BUFFER_USAGE_STORAGE |WGPU_BUFFER_USAGE_COPY_SRC;
outputBuffer=device.createBuffer(bufferDescriptor);
bufferDescriptor.usage=WGPU_BUFFER_USAGE_COPY_DST|WGPU_BUFFER_USAGE_MAP_READ;
mapBuffer=device.createBuffer(bufferDescriptor);

WGpuShaderModuleDescriptor shaderModuleDesc={};
shaderModuleDesc.code=computeShader;
//   WGpuShaderModule vs = wgpu_device_create_shader_module(device, &shaderModuleDesc);
//   shaderModuleDesc.code = fragmentShader;
WGpuShaderModule cs=wgpu_device_create_shader_module(device,&shaderModuleDesc);
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

WGpuBindGroupLayoutEntry bindGroupLayoutEntry;
WGpuBindGroupLayout bindGroupLayout=0;

const char * Entry="computeStuff";
computePipeline=wgpu_device_create_compute_pipeline(device,cs,Entry,bindGroupLayout,NULL,0);
//   emscripten_request_animation_frame_loop(raf,0);
}

void ObtainedWebGpuAdapter(WGpuAdapter result,void *userData){
adapter=result;
WGpuDeviceDescriptor deviceDesc={};
wgpu_adapter_request_device_async(adapter,&deviceDesc,ObtainedWebGpuDevice,0);
}

int main(void){
WGpuRequestAdapterOptions options={};
// options.powerPreference=WGPU_POWER_PREFERENCE_LOW_POWER;
navigator_gpu_request_adapter_async(&options,ObtainedWebGpuAdapter,0);
}
