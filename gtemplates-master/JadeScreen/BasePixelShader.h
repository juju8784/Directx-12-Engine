#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer ConstantBuffer
// {
//
//   float4x4 w;                        // Offset:    0 Size:    64 [unused]
//   float4x4 v;                        // Offset:   64 Size:    64 [unused]
//   float4x4 p;                        // Offset:  128 Size:    64 [unused]
//   float4 lightPos[2];                // Offset:  192 Size:    32
//   float4 lightColor[2];              // Offset:  224 Size:    32
//   float4 lightDirection;             // Offset:  256 Size:    16
//   float4 worldCamera;                // Offset:  272 Size:    16
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// sample                            sampler      NA          NA             s0      1 
// tex2D                             texture  float4          2d             t0      1 
// ConstantBuffer                    cbuffer      NA          NA            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// WORLD                    0   xyzw        1     NONE   float   xyzw
// NORMAL                   0   xyz         2     NONE   float   xyz 
// TEXCOORD                 0   xy          3     NONE   float   xy  
// DISTANCE                 0   xyz         4     NONE   float       
// DISTANCE                 1   xyz         5     NONE   float       
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
ps_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[18], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_input_ps linear v1.xyzw
dcl_input_ps linear v2.xyz
dcl_input_ps linear v3.xy
dcl_output o0.xyzw
dcl_temps 6
sample_indexable(texture2d)(float,float,float,float) r0.xyzw, v3.xyxx, t0.xyzw, s0
lt r1.x, r0.w, l(0.500000)
discard_nz r1.x
add r1.xyzw, -v1.xyzw, cb0[17].xyzw
dp4 r1.w, r1.xyzw, r1.xyzw
rsq r1.w, r1.w
mul r1.xyz, r1.wwww, r1.xyzx
ne r1.w, cb0[12].w, l(-1.000000)
add r2.xyzw, -v1.xyzw, cb0[12].xyzw
dp4 r2.w, r2.xyzw, r2.xyzw
sqrt r3.x, r2.w
mul r3.x, r3.x, l(0.100000)
min r3.x, r3.x, l(1.000000)
add r3.x, -r3.x, l(1.000000)
rsq r2.w, r2.w
mul r2.xyz, r2.wwww, r2.xyzx
dp3_sat r2.w, v2.xyzx, r2.xyzx
mul r3.yzw, r2.wwww, cb0[14].xxyz
dp3 r4.x, -r2.xyzx, v2.xyzx
add r4.x, r4.x, r4.x
mad r2.xyz, v2.xyzx, -r4.xxxx, -r2.xyzx
dp3 r4.x, r2.xyzx, r2.xyzx
rsq r4.x, r4.x
mul r2.xyz, r2.xyzx, r4.xxxx
dp3_sat r2.x, r1.xyzx, r2.xyzx
log r2.x, r2.x
mul r2.x, r2.x, l(128.000000)
exp r2.x, r2.x
mul r2.xyz, r2.xxxx, cb0[14].xyzx
mul r2.xyz, r2.wwww, r2.xyzx
mul r2.xyz, r3.xxxx, r2.xyzx
mul r2.xyz, r2.xyzx, l(0.800000, 0.800000, 0.800000, 0.000000)
mad r2.xyz, r3.yzwy, r3.xxxx, r2.xyzx
and r2.xyz, r1.wwww, r2.xyzx
ne r1.w, cb0[16].w, l(-1.000000)
if_nz r1.w
  dp3 r1.w, v2.xyzx, v2.xyzx
  rsq r1.w, r1.w
  mul r3.xyz, r1.wwww, v2.xyzx
  add r4.xyzw, -v1.xyzw, cb0[13].xyzw
  dp4 r1.w, r4.xyzw, r4.xyzw
  sqrt r2.w, r1.w
  mul r2.w, r2.w, l(0.033333)
  min r2.w, r2.w, l(1.000000)
  add r2.w, -r2.w, l(1.000000)
  rsq r1.w, r1.w
  mul r4.xyz, r1.wwww, r4.xyzx
  dp3 r1.w, cb0[16].xyzx, cb0[16].xyzx
  rsq r1.w, r1.w
  mul r5.xyz, r1.wwww, cb0[16].xyzx
  dp3_sat r1.w, -r4.xyzx, r5.xyzx
  add r3.w, -r1.w, l(0.950000)
  mul_sat r3.w, r3.w, l(6.666668)
  add r3.w, -r3.w, l(1.000000)
  lt r1.w, l(0.800000), r1.w
  and r1.w, r1.w, l(0x3f800000)
  dp3_sat r4.w, r4.xyzx, r3.xyzx
  mul r5.x, r1.w, r4.w
  mul r5.x, r2.w, r5.x
  mad r5.xyz, r5.xxxx, r3.wwww, r2.xyzx
  dp3 r5.w, -r4.xyzx, r3.xyzx
  add r5.w, r5.w, r5.w
  mad r3.xyz, r3.xyzx, -r5.wwww, -r4.xyzx
  dp3 r4.x, r3.xyzx, r3.xyzx
  rsq r4.x, r4.x
  mul r3.xyz, r3.xyzx, r4.xxxx
  dp3_sat r1.x, r1.xyzx, r3.xyzx
  log r1.x, r1.x
  mul r1.x, r1.x, l(128.000000)
  exp r1.x, r1.x
  mul r1.x, r1.x, r4.w
  mul r1.x, r1.w, r1.x
  mul r1.x, r2.w, r1.x
  mul r1.x, r3.w, r1.x
  mad r2.xyz, r1.xxxx, l(0.800000, 0.000000, 0.000000, 0.000000), r5.xyzx
endif 
eq r1.x, cb0[16].w, l(-1.000000)
eq r1.y, cb0[12].w, l(-1.000000)
and r1.x, r1.y, r1.x
if_nz r1.x
  mov o0.xyzw, r0.xyzw
  ret 
endif 
mul o0.xyz, r0.xyzx, r2.xyzx
mov o0.w, r0.w
ret 
// Approximately 86 instruction slots used
#endif

const BYTE BasePixelShader[] =
{
     68,  88,  66,  67, 116,  83, 
     55,  76, 242,  20, 122,  29, 
    174, 247, 171,  59, 207,  77, 
    241, 194,   1,   0,   0,   0, 
    116,  14,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     72,   3,   0,   0,  20,   4, 
      0,   0,  72,   4,   0,   0, 
    216,  13,   0,   0,  82,  68, 
     69,  70,  12,   3,   0,   0, 
      1,   0,   0,   0, 184,   0, 
      0,   0,   3,   0,   0,   0, 
     60,   0,   0,   0,   0,   5, 
    255, 255,   0,   1,   0,   0, 
    228,   2,   0,   0,  82,  68, 
     49,  49,  60,   0,   0,   0, 
     24,   0,   0,   0,  32,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    156,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 163,   0,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0, 169,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    115,  97, 109, 112, 108, 101, 
      0, 116, 101, 120,  50,  68, 
      0,  67, 111, 110, 115, 116, 
     97, 110, 116,  66, 117, 102, 
    102, 101, 114,   0, 169,   0, 
      0,   0,   7,   0,   0,   0, 
    208,   0,   0,   0,  32,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 232,   1, 
      0,   0,   0,   0,   0,   0, 
     64,   0,   0,   0,   0,   0, 
      0,   0, 244,   1,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  24,   2,   0,   0, 
     64,   0,   0,   0,  64,   0, 
      0,   0,   0,   0,   0,   0, 
    244,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     26,   2,   0,   0, 128,   0, 
      0,   0,  64,   0,   0,   0, 
      0,   0,   0,   0, 244,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  28,   2, 
      0,   0, 192,   0,   0,   0, 
     32,   0,   0,   0,   2,   0, 
      0,   0,  44,   2,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  80,   2,   0,   0, 
    224,   0,   0,   0,  32,   0, 
      0,   0,   2,   0,   0,   0, 
     92,   2,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    128,   2,   0,   0,   0,   1, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0, 144,   2, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 180,   2, 
      0,   0,  16,   1,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0, 192,   2,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 119,   0, 102, 108, 
    111,  97, 116,  52, 120,  52, 
      0, 171,   3,   0,   3,   0, 
      4,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 234,   1, 
      0,   0, 118,   0, 112,   0, 
    108, 105, 103, 104, 116,  80, 
    111, 115,   0, 102, 108, 111, 
     97, 116,  52,   0,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     37,   2,   0,   0, 108, 105, 
    103, 104, 116,  67, 111, 108, 
    111, 114,   0, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     37,   2,   0,   0, 108, 105, 
    103, 104, 116,  68, 105, 114, 
    101,  99, 116, 105, 111, 110, 
      0, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  37,   2, 
      0,   0, 119, 111, 114, 108, 
    100,  67,  97, 109, 101, 114, 
     97,   0,   1,   0,   3,   0, 
      1,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  37,   2, 
      0,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  49,  48,  46,  49,   0, 
     73,  83,  71,  78, 196,   0, 
      0,   0,   6,   0,   0,   0, 
      8,   0,   0,   0, 152,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 164,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,  15,   0,   0, 170,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      7,   7,   0,   0, 177,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   3,   0,   0, 186,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
      7,   0,   0,   0, 186,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
      7,   0,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  87,  79, 
     82,  76,  68,   0,  78,  79, 
     82,  77,  65,  76,   0,  84, 
     69,  88,  67,  79,  79,  82, 
     68,   0,  68,  73,  83,  84, 
     65,  78,  67,  69,   0, 171, 
     79,  83,  71,  78,  44,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  84,  65,  82,  71,  69, 
     84,   0, 171, 171,  83,  72, 
     69,  88, 136,   9,   0,   0, 
     80,   0,   0,   0,  98,   2, 
      0,   0, 106,   8,   0,   1, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   0,   0,   0,   0, 
     18,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      0,   0,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,  98,  16,   0,   3, 
    242,  16,  16,   0,   1,   0, 
      0,   0,  98,  16,   0,   3, 
    114,  16,  16,   0,   2,   0, 
      0,   0,  98,  16,   0,   3, 
     50,  16,  16,   0,   3,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0, 104,   0,   0,   2, 
      6,   0,   0,   0,  69,   0, 
      0, 139, 194,   0,   0, 128, 
     67,  85,  21,   0, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   3,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     49,   0,   0,   7,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  63,  13,   0, 
      4,   3,  10,   0,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   9, 242,   0,  16,   0, 
      1,   0,   0,   0,  70,  30, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
     17,   0,   0,   0,  17,   0, 
      0,   7, 130,   0,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  68,   0,   0,   5, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      1,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  57,   0,   0,   8, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58, 128,  32,   0, 
      0,   0,   0,   0,  12,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128, 191,   0,   0, 
      0,   9, 242,   0,  16,   0, 
      2,   0,   0,   0,  70,  30, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
     12,   0,   0,   0,  17,   0, 
      0,   7, 130,   0,  16,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  75,   0,   0,   5, 
     18,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   3,   0,   0,   0, 
      1,  64,   0,   0, 205, 204, 
    204,  61,  51,   0,   0,   7, 
     18,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0,   0,   8,  18,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  68,   0,   0,   5, 
    130,   0,  16,   0,   2,   0, 
      0,   0,  58,   0,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      2,   0,   0,   0, 246,  15, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  16,  32,   0,   7, 
    130,   0,  16,   0,   2,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,   8, 226,   0, 
     16,   0,   3,   0,   0,   0, 
    246,  15,  16,   0,   2,   0, 
      0,   0,   6, 137,  32,   0, 
      0,   0,   0,   0,  14,   0, 
      0,   0,  16,   0,   0,   8, 
     18,   0,  16,   0,   4,   0, 
      0,   0,  70,   2,  16, 128, 
     65,   0,   0,   0,   2,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,  11, 
    114,   0,  16,   0,   2,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,   6,   0, 
     16, 128,  65,   0,   0,   0, 
      4,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0,  16,   0, 
      0,   7,  18,   0,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  68,   0,   0,   5, 
     18,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
      6,   0,  16,   0,   4,   0, 
      0,   0,  16,  32,   0,   7, 
     18,   0,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     47,   0,   0,   5,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   2,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  67, 
     25,   0,   0,   5,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   2,   0, 
      0,   0,  56,   0,   0,   8, 
    114,   0,  16,   0,   2,   0, 
      0,   0,   6,   0,  16,   0, 
      2,   0,   0,   0,  70, 130, 
     32,   0,   0,   0,   0,   0, 
     14,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      2,   0,   0,   0, 246,  15, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   2,   0, 
      0,   0,   6,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,  10, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
    205, 204,  76,  63, 205, 204, 
     76,  63, 205, 204,  76,  63, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 114,   0,  16,   0, 
      2,   0,   0,   0, 150,   7, 
     16,   0,   3,   0,   0,   0, 
      6,   0,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,   1,   0, 
      0,   7, 114,   0,  16,   0, 
      2,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  57,   0,   0,   8, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58, 128,  32,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128, 191,  31,   0, 
      4,   3,  58,   0,  16,   0, 
      1,   0,   0,   0,  16,   0, 
      0,   7, 130,   0,  16,   0, 
      1,   0,   0,   0,  70,  18, 
     16,   0,   2,   0,   0,   0, 
     70,  18,  16,   0,   2,   0, 
      0,   0,  68,   0,   0,   5, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      3,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70,  18,  16,   0,   2,   0, 
      0,   0,   0,   0,   0,   9, 
    242,   0,  16,   0,   4,   0, 
      0,   0,  70,  30,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,  13,   0, 
      0,   0,  17,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     75,   0,   0,   5, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    130,   0,  16,   0,   2,   0, 
      0,   0,  58,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0, 137, 136,   8,  61, 
     51,   0,   0,   7, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     58,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
      0,   8, 130,   0,  16,   0, 
      2,   0,   0,   0,  58,   0, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     68,   0,   0,   5, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   4,   0, 
      0,   0, 246,  15,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   4,   0,   0,   0, 
     16,   0,   0,   9, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 130,  32,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
     70, 130,  32,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
     68,   0,   0,   5, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   8, 
    114,   0,  16,   0,   5,   0, 
      0,   0, 246,  15,  16,   0, 
      1,   0,   0,   0,  70, 130, 
     32,   0,   0,   0,   0,   0, 
     16,   0,   0,   0,  16,  32, 
      0,   8, 130,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   5,   0,   0,   0, 
      0,   0,   0,   8, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,  51,  51, 
    115,  63,  56,  32,   0,   7, 
    130,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,  88,  85, 213,  64, 
      0,   0,   0,   8, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  49,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
    205, 204,  76,  63,  58,   0, 
     16,   0,   1,   0,   0,   0, 
      1,   0,   0,   7, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  16,  32, 
      0,   7, 130,   0,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   4,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  50,   0, 
      0,   9, 114,   0,  16,   0, 
      5,   0,   0,   0,   6,   0, 
     16,   0,   5,   0,   0,   0, 
    246,  15,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  16,   0, 
      0,   8, 130,   0,  16,   0, 
      5,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
      0,   0,   0,   7, 130,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16,   0,   5,   0, 
      0,   0,  58,   0,  16,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11, 114,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
    246,  15,  16, 128,  65,   0, 
      0,   0,   5,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   4,   0,   0,   0, 
     16,   0,   0,   7,  18,   0, 
     16,   0,   4,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  68,   0, 
      0,   5,  18,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,   6,   0,  16,   0, 
      4,   0,   0,   0,  16,  32, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  47,   0,   0,   5, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  67,  25,   0,   0,   5, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,  12, 
    114,   0,  16,   0,   2,   0, 
      0,   0,   6,   0,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0, 205, 204,  76,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   5,   0, 
      0,   0,  21,   0,   0,   1, 
     24,   0,   0,   8,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     58, 128,  32,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128, 191,  24,   0,   0,   8, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  58, 128,  32,   0, 
      0,   0,   0,   0,  12,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128, 191,   1,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  31,   0,   4,   3, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  62,   0, 
      0,   1,  21,   0,   0,   1, 
     56,   0,   0,   7, 114,  32, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   5, 130,  32,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 148,   0,   0,   0, 
     86,   0,   0,   0,   6,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,  73,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};