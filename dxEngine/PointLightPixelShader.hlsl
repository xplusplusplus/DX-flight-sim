Texture2D TextureColor : register(t0); // t0 slotundaki texture
sampler TextureSampler : register(s0); // s0 slotundaki sampler


struct PS_INPUT
{
    float4 position : SV_POSITION; // Ekran uzayý pozisyon
    float2 texcoord : TEXCOORD0; // UV koordinatlarý
    float3 normal : NORMAL0; // Dünya uzayý normal
    float3 world_pos : TEXCOORD1; // Dünya uzayý pozisyon
};

cbuffer constant : register(b0) // CPU ? GPU sabit veriler
{
    row_major float4x4 m_world; // Model ? Dünya
    row_major float4x4 m_view; // Dünya ? Kamera
    row_major float4x4 m_proj; // Kamera ? Clip
    float4 m_light_direction; // (kullanýlmýyor)
    float4 m_camera_position; // Kamera pozisyonu
    float4 m_light_position; // Noktasal ýþýk pozisyonu
    float m_light_radius; // Iþýk yarýçapý
    float m_time; // Zaman
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
    // Texture’dan yüzey rengi al
    float4 tex_color = TextureColor.Sample(TextureSampler,float2(input.texcoord.x,1 - input.texcoord.y));

    // ================= AMBIENT =================
    float ka = 1.5; // Ortam ýþýk katsayýsý
    
    float3 ia = float3(0.09, 0.082, 0.082); // Ortam rengi
    ia *= (tex_color.rgb); // Texture ile çarp

    float3 ambient_light = ka * ia; // Ambient katký

    // ================= DIFFUSE =================
    float kd = 0.7; // Diffuse katsayýsý

    // Iþýk ? yüzey yönü
    float3 light_dir =
        normalize(m_light_position.xyz - input.world_pos.xyz);
    
    float3 id = float3(1, 1, 1); // Iþýk rengi (beyaz)

    // Lambert: cos(theta) = dot(N, L)
    float diffuse_light_amount =
        max(0, dot(light_dir.xyz, input.normal));

    // ================= SPECULAR =================
    float ks = 0.0; // Specular katsayýsý
    float3 is = float3(1.0, 1.0, 1.0); // Specular rengi

    // Iþýk – yüzey mesafesi
    float distance_light_object =
        length(m_light_position.xyz - input.world_pos.xyz);
    
    // Iþýk yarýçapý sonrasý fade
    float fade_area =
        max(0, distance_light_object - m_light_radius);
    
    // Attenuation katsayýlarý
    float constant_func = 1.0;
    float linear_func = 2.0;
    float quadratic_func = 2.0;

    // Diffuse rengi texture ile çarp
    id *= (tex_color.rgb);
    
    // Mesafeye baðlý zayýflama
    float attenuation =
        constant_func +
        linear_func * fade_area +
        quadratic_func * fade_area * fade_area;

    // Zayýflatýlmýþ diffuse ýþýk
    float3 diffuse_light =
        (kd * id * diffuse_light_amount) / attenuation;
    
    // Yansýyan ýþýk vektörü
    float3 reflected_light =
        reflect(light_dir.xyz, input.normal);

    float shininess = 20.0; // Parlama keskinliði

    // Yüzey ? kamera yönü 
    float3 direction_to_cam =
        normalize(input.world_pos - input.world_pos.xyz);
    
    // Phong specular hesabý
    float specular_amount =
        pow(max(0.0, dot(reflected_light, direction_to_cam)),
            shininess);

    // Zayýflatýlmýþ specular ýþýk
    float3 specular_light =
        (ks * specular_amount * is) / attenuation;

    // ================= FINAL =================
    // Tüm ýþýklarý topla
    float3 final_light =
        ambient_light + diffuse_light + specular_light;

    return float4(final_light, 1.0); // Son piksel rengi
}
