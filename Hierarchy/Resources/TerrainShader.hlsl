//////////////////////////////////////////////////////////////////////
// HLSL File:
// This example is compiled using the fxc shader compiler.
// It is possible directly compile HLSL in VS2013
//////////////////////////////////////////////////////////////////////

// This first constant buffer is special.
// The framework looks for particular variables and sets them automatically.
// See the CommonApp comments for the names it looks for.
cbuffer CommonApp
{
	float4x4 g_WVP;
	float4 g_lightDirections[MAX_NUM_LIGHTS];
	float3 g_lightColours[MAX_NUM_LIGHTS];
	int g_numLights;
	float4x4 g_InvXposeW;
	float4x4 g_W;
};


// When you define your own cbuffer you can use a matching structure in your app but you must be careful to match data alignment.
// Alternatively, you may use shader reflection to find offsets into buffers based on variable names.
// The compiler may optimise away the entire cbuffer if it is not used but it shouldn't remove indivdual variables within it.
// Any 'global' variables that are outside an explicit cbuffer go
// into a special cbuffer called "$Globals". This is more difficult to work with
// because you must use reflection to find them.
// Also, the compiler may optimise individual globals away if they are not used.
cbuffer MyApp
{
	float	g_frameCount;
	float3	g_waveOrigin;
}


// VSInput structure defines the vertex format expected by the input assembler when this shader is bound.
// You can find a matching structure in the C++ code.
struct VSInput
{
	float4 pos:POSITION;
	float4 colour:COLOUR0;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD;
};

// PSInput structure is defining the output of the vertex shader and the input of the pixel shader.
// The variables are interpolated smoothly across triangles by the rasteriser.
struct PSInput
{
	float4 pos:SV_Position;
	float4 colour:COLOUR0;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD;
	float4 mat:COLOUR1;
};

// PSOutput structure is defining the output of the pixel shader, just a colour value.
struct PSOutput
{
	float4 colour:SV_Target;
};

// Define several Texture 'slots'
Texture2D g_materialMap;
Texture2D g_texture0;
Texture2D g_texture1;
Texture2D g_texture2;


// Define a state setting 'slot' for the sampler e.g. wrap/clamp modes, filtering etc.
SamplerState g_sampler;

// The vertex shader entry point. This function takes a single vertex and transforms it for the rasteriser.
void VSMain(const VSInput input, out PSInput output)
{
	output.pos = mul(input.pos, g_WVP);

	output.colour = input.colour;

	//sets the normal
	output.normal = input.normal;

	//maps the texture to the correct 512 position
	output.tex.x = (input.pos.x + 512) / 1024;
	output.tex.y = 1 - (input.pos.z + 512) / 1024;

	//samples and applies the material
	output.mat = g_materialMap.SampleLevel(g_sampler, output.tex, 0);

}

// The pixel shader entry point. This function writes out the fragment/pixel colour.
void PSMain(const PSInput input, out PSOutput output)
{
	float4 light = (0, 0, 0, 0);

	for (int i = 0; i < g_numLights; ++i)
	{
		//keeps the value between 0 and 1
		clamp(light, 0, 1);
		//calculates the light directions by dot producting it with the normal
		light += dot(normalize(input.normal), normalize(g_lightDirections[i]));
		light.w = 1;
	}

	//gets the material
	float4 material = input.mat;
	//resets the colour to 0
	output.colour = (0, 0, 0, 0);

	//gets the samples from the material and applies in the correct position
	output.colour += g_texture0.Sample(g_sampler, input.tex * 20) * material.x;
	output.colour += g_texture1.Sample(g_sampler, input.tex * 20) * material.y;
	output.colour += g_texture2.Sample(g_sampler, input.tex * 20) * material.z;

	//sets the alpha to be full
	output.colour.w = 1;

	//multiplies the colour by the light intensity
	output.colour *= light;

}