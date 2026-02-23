Texture2D EarthColor : register(t0);
sampler EarthColorSampler : register(s0);

Texture2D EarthSpecular : register(t1);
sampler EarthSpecularSampler : register(s1);

Texture2D Clouds : register(t2);
sampler CloudsSampler : register(s2);

Texture2D EarthNight : register(t3);
sampler EarthNightSampler : register(s3);

struct PS_INPUT
{
    float3 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 direction_to_cam : TEXCOORD2;
   
    
};

cbuffer constant : register(b0)
{
    row_major float4x4 m_world;
    row_major float4x4 m_view;
    row_major float4x4 m_proj;
    float4 m_light_direction;
    float4 m_camera_position;
    float m_time;
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
    // ============================================================
    // 1? ALBEDO (ANA RENK) TEXTURE OKUMA
    // ============================================================

    // t0 slotuna C++ tarafýnda baðlanan EarthColor texture’ýný sample eder
    // texture’i alýr ve filtreleyerek float4 (RGBA) üretir
    float4 earth_color =
        EarthColor.Sample(EarthColorSampler, 1.0 - input.texcoord);
    
    float4 earth_night =
        EarthNight.Sample(EarthNightSampler, 1.0 - input.texcoord);

    // ============================================================
    // 2? SPECULAR MAP OKUMA
    // ============================================================

    // t1 slotundaki specular map’ten sadece R kanalýný okur
    // Bu deðer: yüzey bu noktada ne kadar parlak yansýma yapar bilgisidir 
    // (0 ? mat, 1 ? ayna gibi)
    float earth_spec =
        EarthSpecular.Sample(EarthSpecularSampler, 1.0 - input.texcoord).r;

    // ============================================================
    // 3? BULUT (OVERLAY) TEXTURE OKUMA
    // ============================================================

    // t2 slotuna baðlý cloud texture’dan yoðunluk bilgisi alýnýr
    // Genelde gri ton texture olduðu için tek kanal yeterlidir
    float clouds =
        Clouds.Sample(CloudsSampler, 1.0 - input.texcoord + float2(m_time/100.0,0)).r;

    // ============================================================
    // 4? AMBIENT (ORTAM) IÞIK HESABI
    // ============================================================

    // Sahnedeki genel ýþýk seviyesi
    // Iþýk gelmese bile objenin tamamen kararmamasýný saðlar
    float ka = 0.5;

    // Ortam ýþýðýnýn temel rengi
    // Hafif koyu ton: sahnenin “base mood”u
    float3 ia = float3(0.09, 0.082, 0.082);

    // Ortam ýþýðýný yüzey rengiyle iliþkilendirir
    // Texture rengi yoksa ýþýðýn da anlamý olmaz
    ia *= (earth_color.rgb + clouds);

    // Ortam ýþýðýnýn nihai katkýsý
    float3 ambient_light = ka * ia;

    // ============================================================
    // 5? DIFFUSE (YÜZEYE GELEN DÝREKT IÞIK)
    // ============================================================

    // Iþýðýn yüzeye maksimum etki gücü sabit
    float kd = 1.0;

    // Iþýk kaynaðýnýn rengi (beyaz ? renk texture’dan gelir) gündüz için
    // Diffuse ýþýk da yüzey rengiyle çarpýlýr
    float3 id_day = float3(1.0, 1.0, 1.0);
    id_day *= (earth_color.rgb);
    
    // Iþýk kaynaðýnýn rengi (beyaz ? renk texture’dan gelir) gece için
    // Diffuse ýþýk da yüzey rengiyle çarpýlýr
    float3 id_night = float3(1.0, 1.0, 1.0);
    id_night *= (earth_night.rgb + clouds*0.3);

    
    
    // Iþýk yönü ile yüzey normali arasýndaki açý
    // dot = cos(angle)
    // max(0) ? ýþýk yüzeyin arkasýndaysa sýfýrlanýr
    float diffuse_amount =
        dot(m_light_direction.xyz, input.normal);
    
    float diffuse_factor = saturate(diffuse_amount);
    
    float3 id = lerp(id_night, id_day, diffuse_factor);
    
    // Diffuse ýþýðýn yüzeye katkýsý
    float3 diffuse_light =
        kd * id * diffuse_factor;

    // ============================================================
    // 6? SPECULAR (PARLAMA / YANSIYAN IÞIK)
    // ============================================================

    // Specular gücü texture’dan gelir
    // Metal / su / cam gibi yüzeyleri ayýrt eder
    float ks = earth_spec;

    // Parlama ýþýðýnýn rengi
    float3 is = float3(1.0, 1.0, 1.0);

    // Iþýðýn yüzeyden yansýma yönünü hesaplar
    // reflect(): fiziksel yansýma yönü
    float3 reflected_light =
        reflect(m_light_direction.xyz, input.normal);

    // Parlamanýn ne kadar dar ve keskin olacaðýný belirler
    // Büyük deðer ? küçük ama güçlü highlight
    float shininess = 3000.0;

    // Kamera yansýma yönüne ne kadar yakýnsa
    // parlama o kadar güçlü olur
    float specular_amount =
        pow(
            max(0.0, dot(reflected_light, input.direction_to_cam)),
            shininess
        );

    // Specular ýþýðýn yüzeye katkýsý
    float3 specular_light =
        ks * specular_amount * is;

    // ============================================================
    // 7? TÜM IÞIK BÝLEÞENLERÝNÝN TOPLANMASI
    // ============================================================

    // Ortam + diffuse + specular
    float3 final_light =
        ambient_light + diffuse_light + specular_light;

    // ============================================================
    // 8? SON PÝKSEL RENGÝ
    // ============================================================

    // GPU bu rengi render target’a (backbuffer) yazar
    return float4(final_light, 1.0);
}
