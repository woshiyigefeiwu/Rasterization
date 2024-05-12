#include <iostream>
#include <fstream>
#include <sstream>
#include <QDebug>
#include "model.h"

// 读取模型，并初始化数据
Model::Model(const char* filename) : verts_(), faces_(), norms_(), uv_(), diffusemap_(), normalmap_(), specularmap_()
{
    qDebug() << filename;

    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;

    std::string line;
    while (!in.eof()) 
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;

        // 顶点
        if (!line.compare(0, 2, "v ")) 
        {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            verts_.push_back(v);
        }
        // 法向量
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f n;
            for (int i = 0; i < 3; i++) iss >> n[i];
            norms_.push_back(n);
        }
        // uv 坐标
        else if (!line.compare(0, 3, "vt ")) 
        {
            iss >> trash >> trash;
            Vec2f uv;
            for (int i = 0; i < 2; i++) iss >> uv[i];
            uv_.push_back(uv);
        }
        // 面的信息
        else if (!line.compare(0, 2, "f ")) 
        {
            std::vector<Vec3i> f;
            Vec3i tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) 
            {
                for (int i = 0; i < 3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    load_texture(filename, "_diffuse.tga", diffusemap_);
    load_texture(filename, "_nm.tga", normalmap_);
    load_texture(filename, "_spec.tga", specularmap_);
}

Model::~Model() {}

int Model::nverts() 
{
    return (int)verts_.size();
}

int Model::nfaces()
{
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
    std::vector<int> face;
    for (int i = 0; i < (int)faces_[idx].size(); i++) face.push_back(faces_[idx][i][0]);
    return face;
}

Vec3f Model::vert(int i) 
{
    return verts_[i];
}

Vec3f Model::vert(int iface, int nthvert) 
{
    return verts_[faces_[iface][nthvert][0]];
}

// 读取纹理贴图
void Model::load_texture(std::string filename, const char* suffix, TGAImage& img)
{
    std::string texfile(filename);
    // 在texfile中查找最后一个"."的位置，并将其索引存储在dot变量中。
    size_t dot = texfile.find_last_of(".");
    if (dot != std::string::npos)
    {
        texfile = texfile.substr(0, dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

Vec3f Model::normal(Vec2f uvf)
{
    Vec2i uv(uvf[0] * normalmap_.get_width(), uvf[1] * normalmap_.get_height());
    TGAColor c = normalmap_.get(uv[0], uv[1]);
    
    /*
        将颜色c中的RGB值（每个值范围为0到255）映射到[-1, 1]的范围内，生成一个法线向量res。
        具体地，通过遍历RGB三个分量，将其归一化到[0, 1]范围后乘以2，并减去1，以得到范围为[-1, 1]的值，最终构成法线向量。
    */
    Vec3f res;
    for (int i = 0; i < 3; i++)
        res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;

    return res;
}

Vec2f Model::uv(int iface, int nthvert)
{
    return uv_[faces_[iface][nthvert][1]];
}

TGAColor Model::diffuse(Vec2f uvf)
{
    Vec2i uv(uvf[0] * diffusemap_.get_width(), uvf[1] * diffusemap_.get_height());
    return diffusemap_.get(uv[0], uv[1]);
}

float Model::specular(Vec2f uvf)
{
    Vec2i uv(uvf[0] * specularmap_.get_width(), uvf[1] * specularmap_.get_height());
    // 从高光贴图中获取该位置的颜色值，并取其中的红色分量作为高光强度值。然后将红色分量除以1.f进行归一化处理，并将结果返回。
    return specularmap_.get(uv[0], uv[1])[0] / 1.f;
}

TGAColor Model::specular2(Vec2f uvf)
{
    Vec2i uv(uvf[0] * specularmap_.get_width(), uvf[1] * specularmap_.get_height());
    // 从高光贴图中获取该位置的颜色值，并取其中的红色分量作为高光强度值。然后将红色分量除以1.f进行归一化处理，并将结果返回。
    return specularmap_.get(uv[0], uv[1]);
}

Vec3f Model::normal(int iface, int nthvert)
{
    int idx = faces_[iface][nthvert][2];
    return norms_[idx].normalize();
}

