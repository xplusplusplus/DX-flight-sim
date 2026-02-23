struct VS_INPUT
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; // Ekran koordinatý (GPU için zorunlu)
    float2 texcoord : TEXCOORD0; // Doku koordinatý
    float3 normal : NORMAL0; // Yüzey normali
    float3 world_pos : TEXCOORD1; // Iþýk hesabý için Dünya üzerindeki konumu
};

cbuffer constant : register(b0)
{
    row_major float4x4 m_world; // Model -> Dünya matrisi
    row_major float4x4 m_view; // Dünya -> Kamera matrisi
    row_major float4x4 m_proj; // Kamera -> Ekran (Perspektif) matrisi
    float4 m_light_direction; // Yönlü ýþýk (Güneþ vb.)
    float4 m_camera_position; // Parlama hesabý için kamera konumu
    float4 m_light_position; // Noktasal ýþýk konumu
    float m_light_radius; // Iþýðýn menzili
    float m_time; // Animasyon için zaman
};




VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    // 1. Model koordinatýný Dünya koordinatýna çevir (World Space)
    output.position = mul(input.position, m_world);
    
    // 2. Iþýk hesaplarý için ham dünya pozisyonunu sakla
    output.world_pos = output.position.xyz;

    // 3. Kameraya göre konumlandýr (View Space)
    output.position = mul(output.position, m_view);

    // 4. Derinlik ve Perspektif ekleyerek 2D ekrana ez (Clip Space)
    output.position = mul(output.position, m_proj);
    
    // 

    // Verileri Pixel Shader'a aktar
    output.texcoord = input.texcoord;
    output.normal = normalize(input.normal); // Normali birim vektör yap

    return output;
}