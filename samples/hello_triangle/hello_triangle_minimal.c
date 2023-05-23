#include "lib_webgpu.h"

WGpuAdapter adapter;
WGpuCanvasContext canvasContext;
WGpuDevice device;
WGpuQueue queue;
WGpuRenderPipeline renderPipeline;

EM_BOOL raf(double time, void *userData)
{
  WGpuCommandEncoder encoder = wgpu_device_create_command_encoder(device, 0);

  WGpuRenderPassColorAttachment colorAttachment = WGPU_RENDER_PASS_COLOR_ATTACHMENT_DEFAULT_INITIALIZER;
  colorAttachment.view = wgpu_texture_create_view(wgpu_canvas_context_get_current_texture(canvasContext), 0);

  WGpuRenderPassDescriptor passDesc = {};
  passDesc.numColorAttachments = 1;
  passDesc.colorAttachments = &colorAttachment;

  WGpuRenderPassEncoder pass = wgpu_command_encoder_begin_render_pass(encoder, &passDesc);
  wgpu_render_pass_encoder_set_pipeline(pass, renderPipeline);
  wgpu_render_pass_encoder_draw(pass, 3, 1, 0, 0);
  wgpu_render_pass_encoder_end(pass);

  WGpuCommandBuffer commandBuffer = wgpu_command_encoder_finish(encoder);

  wgpu_queue_submit_one_and_destroy(queue, commandBuffer);

  return EM_FALSE; // Render just one frame, static content
}

void ObtainedWebGpuDevice(WGpuDevice result, void *userData)
{
  device = result;
  queue = wgpu_device_get_queue(device);

  canvasContext = wgpu_canvas_get_webgpu_context("canvas");

  WGpuCanvasConfiguration config = WGPU_CANVAS_CONFIGURATION_DEFAULT_INITIALIZER;
  config.device = device;
  config.format = navigator_gpu_get_preferred_canvas_format();
  wgpu_canvas_context_configure(canvasContext, &config);

  const char *vertexShader =
    "@vertex\n"
    "fn main(@builtin(vertex_index) vertexIndex : u32) -> @builtin(position) vec4<f32> {\n"
      "var pos = array<vec2<f32>, 3>(\n"
        "vec2<f32>(0.0, 0.5),\n"
        "vec2<f32>(-0.5, -0.5),\n"
        "vec2<f32>(0.5, -0.5)\n"
      ");\n"

      "return vec4<f32>(pos[vertexIndex], 0.0, 1.0);\n"
    "}\n";

  const char *fragmentShader =
    "struct FragmentOutput {"
  "  @location(0) fragColor: vec4<f32>,"
"}"

"@group(0) @binding(0) "
"var<uniform> time_1: f32;"
"@group(0) @binding(1) "
"var<uniform> mouse: vec2<f32>;"
"@group(0) @binding(2) "
"var<uniform> resolution: vec2<f32>;"
"var<private> fragColor: vec4<f32>;"
"var<private> gl_FragCoord: vec4<f32>;"

"fn opSmoothUnion(d1_: f32, d2_: f32, k: f32) -> f32 {"
 "  var d1_1: f32;"
  "  var d2_1: f32;"
  "  var k_1: f32;"
  "  var h: f32;"

  "  d1_1 = d1_;"
  "  d2_1 = d2_;"
  "  k_1 = k;"
  "  let _e12 = d2_1;"
  "  let _e13 = d1_1;"
  "  let _e16 = k_1;"
  "  _ = (0.5 + ((0.5 * (_e12 - _e13)) / _e16));"
  "  let _e23 = d2_1;"
  "  let _e24 = d1_1;"
  "  let _e27 = k_1;"
  "  h = clamp((0.5 + ((0.5 * (_e23 - _e24)) / _e27)), 0.0, 1.0);"
  "  _ = d2_1;"
  "  _ = d1_1;"
  "  _ = h;"
  "  let _e37 = d2_1;"
   " let _e38 = d1_1;"
   " let _e39 = h;"
   " let _e41 = k_1;"
   " let _e42 = h;"
   " let _e45 = h;"
   " return (mix(_e37, _e38, _e39) - ((_e41 * _e42) * (1.0 - _e45)));"
"}"

"fn sdSphere(p: vec3<f32>, s: f32) -> f32 {"
  "  var p_1: vec3<f32>;"
  "  var s_1: f32;"

  "  p_1 = p;"
  "  s_1 = s;"
  "  _ = p_1;"
  "  let _e9 = p_1;"
   " let _e11 = s_1;"
   " return (length(_e9) - _e11);"
"}"

"fn map(p_2: vec3<f32>) -> f32 {"
 "   var p_3: vec3<f32>;"
"    var d: f32;"
 "   var i: i32;"
  "  var fi: f32;"
  "  var time: f32;"

  "  p_3 = p_2;"
  "  d = 2.0;"
  "  i = 0;"
  "  loop {"
  "      let _e10 = i;"
   "     if !((_e10 < 16)) {"
   "         break;"
    "    }"
    "   {"
      "      let _e17 = i;"
        "    fi = f32(_e17);"
         "   let _e20 = time_1;"
         "   let _e21 = fi;"
         "   _ = ((_e21 * 412.531005859375) + 0.5130000114440918);"
         "   let _e26 = fi;"
        "    time = ((_e20 * (fract(((_e26 * 412.531005859375) + 0.5130000114440918)) - 0.5)) * 2.0);"
        "    let _e38 = p_3;"
        "    let _e39 = time;"
        "    let _e40 = fi;"
        "    _ = (vec3<f32>(_e39) + (_e40 * vec3<f32>(52.51259994506836, 64.62744140625, 632.25)));"
        "    let _e48 = time;"
        "    let _e49 = fi;"
        "    _ = (_e38 + (sin((vec3<f32>(_e48) + (_e49 * vec3<f32>(52.51259994506836, 64.62744140625, 632.25)))) * vec3<f32>(2.0, 2.0, 0.800000011920929)));"
        "    let _e66 = fi;"
"_ = ((_e66 * 412.531005859375) + 0.5123999714851379);"
       "     let _e71 = fi;"
       "     _ = fract(((_e71 * 412.531005859375) + 0.5123999714851379));"
       "     let _e79 = fi;"
       "     _ = ((_e79 * 412.531005859375) + 0.5123999714851379);"
       "     let _e84 = fi;"
       "     _ = mix(0.5, 1.0, fract(((_e84 * 412.531005859375) + 0.5123999714851379)));"
       "     let _e91 = p_3;"
       "     let _e92 = time;"
       "     let _e93 = fi;"
       "     _ = (vec3<f32>(_e92) + (_e93 * vec3<f32>(52.51259994506836, 64.62744140625, 632.25)));"
       "     let _e101 = time;"
       "     let _e102 = fi;"
      "      let _e119 = fi;"
       "     _ = ((_e119 * 412.531005859375) + 0.5123999714851379);"
      "      let _e124 = fi;"
      "      _ = fract(((_e124 * 412.531005859375) + 0.5123999714851379));"
      "      let _e132 = fi;"
      "      _ = ((_e132 * 412.531005859375) + 0.5123999714851379);"
      "      let _e137 = fi;"
      "      let _e144 = sdSphere((_e91 + (sin((vec3<f32>(_e101) + (_e102 * vec3<f32>(52.51259994506836, 64.62744140625, 632.25)))) * vec3<f32>(2.0, 2.0, 0.800000011920929))), mix(0.5, 1.0, fract(((_e137 * 412.531005859375) + 0.5123999714851379))));"
      "      _ = d;"
     "       let _e147 = p_3;"
    "        let _e148 = time;"
    "        let _e149 = fi;"
    "        _ = (vec3<f32>(_e148) + (_e149 * vec3<f32>(52.51259994506836, 64.62744140625, 632.25)));"
    "        let _e157 = time;"
    "        let _e158 = fi;"
    "        _ = (_e147 + (sin((vec3<f32>(_e157) + (_e158 * vec3<f32>(52.51259994506836, 64.62744140625, 632.25)))) * vec3<f32>(2.0, 2.0, 0.800000011920929)));"
    "        let _e175 = fi;"
    "        _ = ((_e175 * 412.531005859375) + 0.5123999714851379);"
    "        let _e180 = fi;"
   "         _ = fract(((_e180 * 412.531005859375) + 0.5123999714851379));"
    "        let _e188 = fi;"
   "         _ = ((_e188 * 412.531005859375) + 0.5123999714851379);"
   "         let _e193 = fi;"
   "         _ = mix(0.5, 1.0, fract(((_e193 * 412.531005859375) + 0.5123999714851379)));"
   "         let _e200 = p_3;"
   "         let _e201 = time;"
   "         let _e202 = fi;"
  "          _ = (vec3<f32>(_e201) + (_e202 * vec3<f32>(52.51259994506836, 64.62744140625, 632.25)));"
  "          let _e210 = time;"
  "          let _e211 = fi;"
  "          let _e228 = fi;"
  "          _ = ((_e228 * 412.531005859375) + 0.5123999714851379);"
  "          let _e233 = fi;"
  "          _ = fract(((_e233 * 412.531005859375) + 0.5123999714851379));"
  "          let _e241 = fi;"
  "          _ = ((_e241 * 412.531005859375) + 0.5123999714851379);"
  "          let _e246 = fi;"
  "          let _e253 = sdSphere((_e200 + (sin((vec3<f32>(_e210) + (_e211 * vec3<f32>(52.51259994506836, 64.62744140625, 632.25)))) * vec3<f32>(2.0, 2.0, 0.800000011920929))), mix(0.5, 1.0, fract(((_e246 * 412.531005859375) + 0.5123999714851379))));"
  "          let _e254 = d;"
  "          let _e256 = opSmoothUnion(_e253, _e254, 0.4000000059604645);"
  "          d = _e256;"
  "      }"
 "       continuing {"
 "           let _e14 = i;"
 "           i = (_e14 + 1);"
 "       }"
 "   }"
 "   let _e257 = d;"
 "   return _e257;"
"}"

"fn calcNormal(p_4: vec3<f32>) -> vec3<f32> {"
"    var p_5: vec3<f32>;"
"    var h_1: f32;"
"    var k_2: vec2<f32>;"

"    p_5 = p_4;"
"    h_1 = 9.999999747378752e-6;"
"    k_2 = vec2<f32>(f32(1), f32(-(1)));"
"    let _e15 = k_2;"
"    let _e17 = p_5;"
"    let _e18 = k_2;"
"    let _e20 = h_1;"
"    _ = (_e17 + (_e18.xyy * _e20));"
"    let _e23 = p_5;"
"    let _e24 = k_2;"
"    let _e26 = h_1;"
"    let _e29 = map((_e23 + (_e24.xyy * _e26)));"
"    let _e31 = k_2;"
"    let _e33 = p_5;"
"    let _e34 = k_2;"
"    let _e36 = h_1;"
"    _ = (_e33 + (_e34.yyx * _e36));"
"    let _e39 = p_5;"
"    let _e40 = k_2;"
"    let _e42 = h_1;"
"    let _e45 = map((_e39 + (_e40.yyx * _e42)));"
"    let _e48 = k_2;"
"    let _e50 = p_5;"
"    let _e51 = k_2;"
"    let _e53 = h_1;"
"    _ = (_e50 + (_e51.yxy * _e53));"
 "   let _e56 = p_5;"
 "   let _e57 = k_2;"
 "   let _e59 = h_1;"
 "   let _e62 = map((_e56 + (_e57.yxy * _e59)));"
 "   let _e65 = k_2;"
 "   let _e67 = p_5;"
 "   let _e68 = k_2;"
 "   let _e70 = h_1;"
 "   _ = (_e67 + (_e68.xxx * _e70));"
 "   let _e73 = p_5;"
 "   let _e74 = k_2;"
 "   let _e76 = h_1;"
 "   let _e79 = map((_e73 + (_e74.xxx * _e76)));"
 "   _ = ((((_e15.xyy * _e29) + (_e31.yyx * _e45)) + (_e48.yxy * _e62)) + (_e65.xxx * _e79));"
 "   let _e82 = k_2;"
 "   let _e84 = p_5;"
 "   let _e85 = k_2;"
 "   let _e87 = h_1;"
 "   _ = (_e84 + (_e85.xyy * _e87));"
 "   let _e90 = p_5;"
"    let _e91 = k_2;"
"    let _e93 = h_1;"
  "  let _e96 = map((_e90 + (_e91.xyy * _e93)));"
  "  let _e98 = k_2;"
  "  let _e100 = p_5;"
  "  let _e101 = k_2;"
  "  let _e103 = h_1;"
  "  _ = (_e100 + (_e101.yyx * _e103));"
  "  let _e106 = p_5;"
  "  let _e107 = k_2;"
  "  let _e109 = h_1;"
  "  let _e112 = map((_e106 + (_e107.yyx * _e109)));"
  "  let _e115 = k_2;"
  "  let _e117 = p_5;"
  "  let _e118 = k_2;"
  "  let _e120 = h_1;"
  "  _ = (_e117 + (_e118.yxy * _e120));"
  "  let _e123 = p_5;"
  "  let _e124 = k_2;"
  "  let _e126 = h_1;"
  "  let _e129 = map((_e123 + (_e124.yxy * _e126)));"
  "  let _e132 = k_2;"
  "  let _e134 = p_5;"
  "  let _e135 = k_2;"
  "  let _e137 = h_1;"
  "  _ = (_e134 + (_e135.xxx * _e137));"
  "  let _e140 = p_5;"
  "  let _e141 = k_2;"
  "  let _e143 = h_1;"
  "  let _e146 = map((_e140 + (_e141.xxx * _e143)));"
 "   return normalize(((((_e82.xyy * _e96) + (_e98.yyx * _e112)) + (_e115.yxy * _e129)) + (_e132.xxx * _e146)));"
"}"

"fn main_1() {"
  "  var uv: vec2<f32>;"
  "  var rayOri: vec3<f32>;"
  "  var rayDir: vec3<f32>;"
  "  var depth: f32;"
  "  var p_6: vec3<f32>;"
  "  var i_1: i32;"
  "  var dist: f32;"
  "  var n: vec3<f32>;"
  "  var b: f32;"
  "  var col: vec3<f32>;"

  "  let _e5 = gl_FragCoord;"
  "  let _e7 = resolution;"
  "  uv = (_e5.xy / _e7.xy);"
  "  let _e11 = uv;"
   " let _e15 = resolution;"
   " let _e17 = resolution;"
  "  let _e24 = (((_e11 - vec2<f32>(0.5)) * vec2<f32>((_e15.x / _e17.y), 1.0)) * 6.0);"
  "  rayOri = vec3<f32>(_e24.x, _e24.y, 3.0);"
  "  rayDir = vec3<f32>(0.0, 0.0, -(1.0));"
  "  depth = 0.0;"
  "  i_1 = 0;"
   " loop {"
   "     let _e41 = i_1;"
   "     if !((_e41 < 64)) {"
   "         break;"
   "     }"
   "     {"
     "       let _e48 = rayOri;"
     "       let _e49 = rayDir;"
     "       let _e50 = depth;"
      "      p_6 = (_e48 + (_e49 * _e50));"
      "      _ = p_6;"
     "       let _e54 = p_6;"
      "      let _e55 = map(_e54);"
      "      dist = _e55;"
      "      let _e57 = depth;"
      "      let _e58 = dist;"
       "     depth = (_e57 + _e58);"
       "     let _e60 = dist;"
       "     if (_e60 < 9.999999974752427e-7) {"
       "         {"
        "            break;"
       "         }"
      "      }"
      "  }"
     "   continuing {"
     "       let _e45 = i_1;"
     "       i_1 = (_e45 + 1);"
    "    }"
   " }"
 "   _ = depth;"
  "  let _e66 = depth;"
  "  depth = min(6.0, _e66);"
  "  _ = p_6;"
 "   let _e69 = p_6;"
 "   let _e70 = calcNormal(_e69);"
  "  n = _e70;"
 "   _ = n;"
 "   _ = vec3<f32>(0.5770000219345093);"
  "  let _e76 = n;"
 "   _ = dot(_e76, vec3<f32>(0.5770000219345093));"
"    _ = n;"
"    _ = vec3<f32>(0.5770000219345093);"
 "   let _e84 = n;"
 "   b = max(0.0, dot(_e84, vec3<f32>(0.5770000219345093)));"
 "   let _e92 = b;"
 "   let _e93 = time_1;"
 "   let _e97 = uv;"
 "   _ = ((vec3<f32>((_e92 + (_e93 * 3.0))) + (_e97.xyx * 2.0)) + vec3<f32>(f32(0), f32(2), f32(4)));"
 "   let _e111 = b;"
 "   let _e112 = time_1;"
 "   let _e116 = uv;"
 "   let _e135 = b;"
 "   col = ((vec3<f32>(0.5) + (0.5 * cos(((vec3<f32>((_e111 + (_e112 * 3.0))) + (_e116.xyx * 2.0)) + vec3<f32>(f32(0), f32(2), f32(4)))))) * (0.8500000238418579 + (_e135 * 0.3499999940395355)));"
 "   let _e141 = col;"
 "   let _e142 = depth;"
 "   _ = (-(_e142) * 0.15000000596046448);"
 "   let _e146 = depth;"
 "   col = (_e141 * exp((-(_e146) * 0.15000000596046448)));"
"    let _e152 = col;"
"    let _e154 = depth;"
"   fragColor = vec4<f32>(_e152.x, _e152.y, _e152.z, (1.0 - ((_e154 - 0.5) / 2.0)));"
"    return;"
"}"

"@fragment "
"fn main(@builtin(position) param: vec4<f32>) -> FragmentOutput {"
"    gl_FragCoord = param;"
"    main_1();"
"    let _e11 = fragColor;"
"    return FragmentOutput(_e11);"
"}"
"\n";

  WGpuShaderModuleDescriptor shaderModuleDesc = {};
  shaderModuleDesc.code = vertexShader;
  WGpuShaderModule vs = wgpu_device_create_shader_module(device, &shaderModuleDesc);

  shaderModuleDesc.code = fragmentShader;
  WGpuShaderModule fs = wgpu_device_create_shader_module(device, &shaderModuleDesc);

  WGpuRenderPipelineDescriptor renderPipelineDesc = WGPU_RENDER_PIPELINE_DESCRIPTOR_DEFAULT_INITIALIZER;
  renderPipelineDesc.vertex.module = vs;
  renderPipelineDesc.vertex.entryPoint = "main";
  renderPipelineDesc.fragment.module = fs;
  renderPipelineDesc.fragment.entryPoint = "main";

  WGpuColorTargetState colorTarget = WGPU_COLOR_TARGET_STATE_DEFAULT_INITIALIZER;
  colorTarget.format = config.format;
  renderPipelineDesc.fragment.numTargets = 1;
  renderPipelineDesc.fragment.targets = &colorTarget;

  renderPipeline = wgpu_device_create_render_pipeline(device, &renderPipelineDesc);

  emscripten_request_animation_frame_loop(raf, 0);
}

void ObtainedWebGpuAdapter(WGpuAdapter result, void *userData)
{
  adapter = result;

  WGpuDeviceDescriptor deviceDesc = {};
  wgpu_adapter_request_device_async(adapter, &deviceDesc, ObtainedWebGpuDevice, 0);
}

int main()
{
  WGpuRequestAdapterOptions options = {};
  options.powerPreference = WGPU_POWER_PREFERENCE_LOW_POWER;
  navigator_gpu_request_adapter_async(&options, ObtainedWebGpuAdapter, 0);
}
