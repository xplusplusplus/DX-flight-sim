// Vertex Shader
struct VSInput
{
    float3 pos : POSITION; // sadece pozisyon
};

struct VSOutput
{
    float4 pos : SV_POSITION; // sadece clip-space pozisyon
};

VSOutput vsmain(VSInput input)
{
    VSOutput output;
    output.pos = float4(input.pos, 1.0f);
    return output;
}

// Pixel Shader
float4 psmain(VSOutput input) : SV_TARGET
{
    // sabit bir renk döndürüyoruz
    return float4(1.0f, 1.0f, 1.0f, 1.0f); // beyaz
}