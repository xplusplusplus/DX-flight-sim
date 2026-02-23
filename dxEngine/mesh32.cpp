#include "Mesh.h"

// tiny_obj_loader kütüphanesinin çalışması için gereken zorunlu bir tanımlama
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "GraphicsEngine.h"
#include <filesystem>
#include "VertexMesh.h"

// Mesh sınıfının Yapıcı (Constructor) fonksiyonu. Dosya yolunu (full_path) alır.
Mesh::Mesh(const wchar_t* full_path) :Resource(full_path)
{
    // --- 1. ADIM: DEPOLAMA ALANLARINI HAZIRLAMA ---

    // attribs: OBJ dosyasındaki tüm ham verileri tutar (tüm X,Y,Z koordinatları, U,V doku koordinatları vb.)
    tinyobj::attrib_t attribs;

    // shapes: Modelin parçalarını tutar (Örn: Bir araba modeliyse tekerlek bir shape, kapı bir shape'tir)
    std::vector<tinyobj::shape_t> shapes;

    // materials: Modelin materyal (renk/doku) özelliklerini tutar (.mtl dosyasından gelir)
    std::vector<tinyobj::material_t> materials;

    std::string warn; // Uyarı mesajları için
    std::string err;  // Hata mesajları için

    // Dosya yolunu geniş karakterden (wchar_t), standart string'e çeviriyoruz
    auto inputfile = std::filesystem::path(full_path).string();

    // OBJ dosyasının bulunduğu klasörün yolunu buluyoruz. 
    // Neden? Çünkü objenin renklerini tutan .mtl dosyası genelde aynı klasördedir. Onu da bulabilmek için.
    std::string mtldir = inputfile.substr(0, inputfile.find_last_of("\\/"));

    // --- 2. ADIM: DOSYAYI OKUMA ---

    // tinyobj kütüphanesine "Al bu dosyayı, içindeki her şeyi parçala ve yukarıdaki değişkenlere doldur" diyoruz.
    bool res = tinyobj::LoadObj(&attribs, &shapes, &materials, &warn, &err, inputfile.c_str(), mtldir.c_str());

    // Eğer bir hata varsa (dosya bulunamadı, bozuk vs.) programı durdur ve hata fırlat.
    if (!err.empty()) throw std::exception("mesh not created successfully");
    if (!res) throw std::exception("mesh not created successfully");

    // --- 3. ADIM: EKRAN KARTINA GİDECEK LİSTELERİ HAZIRLAMA ---

    // Ekran kartına göndereceğimiz KESİN ve TEMİZlenmiş noktalar listesi
    std::vector<VertexMesh> list_vertices;
    // Hangi noktaların birleşip üçgen oluşturacağını söyleyen sıra numaraları listesi
    std::vector<unsigned int> list_indices;

    // Modelde toplam kaç tane nokta (vertex) olacağını hesaplıyoruz.
    size_t size_vertex_index_list = 0;
    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_vertex_index_list += (shapes[s].mesh.indices.size());
    }

    // Listelerin hafızada (RAM) ne kadar yer kaplayacağını peşinen ayırtıyoruz. 
    // Bu, bilgisayarın sonradan sürekli alan genişletmekle uğraşmasını engeller (Performans artışı).
    list_vertices.reserve(size_vertex_index_list);
    list_indices.reserve(size_vertex_index_list);

    // --- 4. ADIM: MATERYALLERE GÖRE GRUPLAMA (EN ÖNEMLİ KISIM) ---

    // Material slotlarımızı materyal sayısı kadar boyutlandırıyoruz.
    m_material_slots.resize(materials.size());
    size_t index_global_offset = 0; // Toplamda kaçıncı noktada olduğumuzu takip eden sayaç

    // GPU, materyal değiştirmeyi sevmez. O yüzden önce "Materyal 0" kullanan her şeyi, 
    // sonra "Materyal 1" kullanan her şeyi çizeriz. Bu yüzden en dış döngü MATERYALLERDİR.
    for (size_t m = 0; m < materials.size(); m++)
    {
        // Bu materyalin çizilmeye başlanacağı index sırasını kaydediyoruz.
        m_material_slots[m].start_index = index_global_offset;
        m_material_slots[m].material_id = m;

        // Modelin tüm parçalarını (shapes) dönüyoruz
        for (size_t s = 0; s < shapes.size(); s++)
        {
            size_t index_offset = 0;

            // Parçanın içindeki yüzeyleri (üçgenleri / faces) dönüyoruz
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                // EĞER bu yüzey, şu an aradığımız materyale (m) sahip DEĞİLSE, onu atla! (continue)
                if (shapes[s].mesh.material_ids[f] != m) continue;

                // Bir yüzeyin kaç köşesi var? (Genelde 3 olur çünkü oyun motorları üçgen çizer)
                unsigned char num_face_verts = shapes[s].mesh.num_face_vertices[f];

                // Yüzeyin (üçgenin) köşelerini tek tek dönüyoruz (0, 1 ve 2. köşe)
                for (unsigned char v = 0; v < num_face_verts; v++) {

                    // Bu köşenin genel listedeki index (sıra) numarası
                    tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];

                    // --- MATEMATİKSEL SİHİR KISMI ---
                    // Neden 3 ile çarpıyoruz? Çünkü bütün X, Y, Z değerleri tek bir düz listede peş peşe duruyor: [X0,Y0,Z0, X1,Y1,Z1...]
                    // 1. noktanın X'ini bulmak için 1*3 = 3. sıraya, Y'si için 3+1 = 4. sıraya bakarız.
                    tinyobj::real_t vx = attribs.vertices[index.vertex_index * 3 + 0]; // X Pozisyonu
                    tinyobj::real_t vy = attribs.vertices[index.vertex_index * 3 + 1]; // Y Pozisyonu
                    tinyobj::real_t vz = attribs.vertices[index.vertex_index * 3 + 2]; // Z Pozisyonu

                    // Texture koordinatları U ve V olarak 2 tanedir, o yüzden 2 ile çarpıyoruz.
                    tinyobj::real_t tx = attribs.texcoords[index.texcoord_index * 2 + 0]; // U (X Dokusu)
                    tinyobj::real_t ty = attribs.texcoords[index.texcoord_index * 2 + 1]; // V (Y Dokusu)

                    // Işığın çarpma yönünü (Normal) yine X,Y,Z olduğu için 3 ile çarpıyoruz.
                    tinyobj::real_t nx = attribs.normals[index.normal_index * 3 + 0]; // Normal X
                    tinyobj::real_t ny = attribs.normals[index.normal_index * 3 + 1]; // Normal Y
                    tinyobj::real_t nz = attribs.normals[index.normal_index * 3 + 2]; // Normal Z

                    // Yukarıda dosyanın farklı yerlerinden topladığımız bu verileri, BİZİM KENDİ PAKETİMİZE koyuyoruz.
                    VertexMesh vertex(Vector3D(vx, vy, vz), Vector2D(tx, ty), Vector3D(nx, ny, nz));

                    // Paketi temiz listeye ekliyoruz
                    list_vertices.push_back(vertex);

                    // Bu köşeye bir de sıra numarası verip Index listesine ekliyoruz.
                    list_indices.push_back((unsigned int)index_global_offset + v);
                }

                // Bir sonraki yüzeye geçmek için sayaçları arttırıyoruz
                index_offset += num_face_verts;
                index_global_offset += num_face_verts;
            }
        }

        // Bu materyalin (slotun) toplamda kaç tane index (köşe sırası) kullandığını hesaplayıp kaydediyoruz.
        m_material_slots[m].num_indices = index_global_offset - m_material_slots[m].start_index;
    }

    // --- 5. ADIM: EKRAN KARTINA (GPU) TRANSFER ---

    void* shader_byte_code = nullptr;
    size_t size_shader = 0;

    // Shader kodunun bilgisini alıyoruz ki ekran kartı noktaları nasıl dizeceğini bilsin
    GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

    // VERTEX BUFFER OLUŞTUR: list_vertices içindeki verileri RAM'den VRAM'e (Ekran Kartına) kopyala.
    m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&list_vertices[0], sizeof(VertexMesh), (UINT)list_vertices.size(), shader_byte_code, size_shader);

    // INDEX BUFFER OLUŞTUR: list_indices içindeki çizim sırası verilerini ekran kartına kopyala.
    m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&list_indices[0], (UINT)list_indices.size());
}