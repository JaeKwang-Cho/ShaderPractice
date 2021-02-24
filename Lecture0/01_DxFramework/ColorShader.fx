//**************************************************************//
//  Effect File exported by RenderMonkey 1.6
//
//  - Although many improvements were made to RenderMonkey FX  
//    file export, there are still situations that may cause   
//    compilation problems once the file is exported, such as  
//    occasional naming conflicts for methods, since FX format 
//    does not support any notions of name spaces. You need to 
//    try to create workspaces in such a way as to minimize    
//    potential naming conflicts on export.                    
//    
//  - Note that to minimize resulting name collisions in the FX 
//    file, RenderMonkey will mangle names for passes, shaders  
//    and function names as necessary to reduce name conflicts. 
//**************************************************************//

//--------------------------------------------------------------//
// ColorShader
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string ColorShader_Pass_0_Model : ModelData = "C:\\Program Files (x86)\\AMD\\RenderMonkey 1.82\\Examples\\sphere.x";

// Global variables
// 1.world matrix 2.view matrix 3.projection matrix
// 
float4x4 gWorldMatrix : World; 
float4x4 gViewMatrix : View; 
float4x4 gProjectionMatrix : Projection; 
// substitute matrix values into variables by tool

struct VS_INPUT
// Get vertex's position for shader to transform.
{
  float4 mPosition : POSITION; 
  // sub vertext's postion value by sementic 'POSITION'
  // float4 = (x, y, z, w)
};

struct VS_OUTPUT
// Return vertex for rasterizer to transfomr into pixel
{
   float4 mPosition : POSITION;
   // use same sementic 'Position'
};

// main function
VS_OUTPUT ColorShader_Pass_0_Vertex_Shader_vs_main(VS_INPUT Input)
{
   VS_OUTPUT Output;
  
   // us 'mul' function to matrix product.
   Output.mPosition = mul(Input.mPosition,gWorldMatrix);
   Output.mPosition = mul(Output.mPosition, gViewMatrix);
   Output.mPosition = mul(Output.mPosition, gProjectionMatrix);
   
   return Output;
}







float4 ColorShader_Pass_0_Pixel_Shader_ps_main() : COLOR
// return float4 used as COLOR in backbuffer.
{
   return float4(1.0f, 0.5f,0.0f, 0.3f);
}


//--------------------------------------------------------------//
// Technique Section for ColorShader
//--------------------------------------------------------------//
technique ColorShader
{
   pass Pass_0
   {
      VertexShader = compile vs_2_0 ColorShader_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 ColorShader_Pass_0_Pixel_Shader_ps_main();
   }

}

