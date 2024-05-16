#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

// 存储模型数据，以及一些对于模型的操作
class Model
{
public:
    // 读取模型，并初始化数据
    Model(const char *filename);
    ~Model();
    
    // 获取模型点的个数
    int nverts();
    
    // 获取模型面的数量
    int nfaces();

    // 获取对应面上，对应点的法向量，并且归一化
    Vec3f normal(int iface, int nthvert);
    
    // 从法线贴图中获取给定UV坐标处的法线向量。
    Vec3f normal(Vec2f uv);

    // 获取顶点
    Vec3f vert(int i);
    Vec3f vert(int iface, int nthvert);
    
    // 获取 uv 坐标
    Vec2f uv(int iface, int nthvert);

    // 获取面的信息
    std::vector<int> face(int idx);

    // 获取漫反射贴图上对应uv坐标的颜色值
    TGAColor diffuse(Vec2f uv);

    // 获取镜面反射
    TGAColor specular2(Vec2f uvf);

    // 获取镜面反射贴图上对应uv坐标的r分量的颜色，并且归一化
    float specular(Vec2f uv);


public:
    // 模型顶点集合 v
    std::vector<Vec3f> verts_;
    
    // 模型的面的集合 f
    std::vector<std::vector<Vec3i> > faces_; // attention, this Vec3i means vertex/uv/normal

    // 模型法向量集合 vn
    std::vector<Vec3f> norms_;
    
    // 模型uv坐标集合 vt
    std::vector<Vec2f> uv_;
    
    // 漫反射贴图
    TGAImage diffusemap_;
    // 法向量贴图
    TGAImage normalmap_;
    // 镜面反射贴图
    TGAImage specularmap_;

    // 读取纹理贴图
    void load_texture(std::string filename, const char* suffix, TGAImage& img);
};
#endif //__MODEL_H__

