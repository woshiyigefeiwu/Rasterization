#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

// �洢ģ�����ݣ��Լ�һЩ����ģ�͵Ĳ���
class Model
{
public:
    // ��ȡģ�ͣ�����ʼ������
    Model(const char *filename);
    ~Model();
    
    // ��ȡģ�͵�ĸ���
    int nverts();
    
    // ��ȡģ���������
    int nfaces();

    // ��ȡ��Ӧ���ϣ���Ӧ��ķ����������ҹ�һ��
    Vec3f normal(int iface, int nthvert);
    
    // �ӷ�����ͼ�л�ȡ����UV���괦�ķ���������
    Vec3f normal(Vec2f uv);

    // ��ȡ����
    Vec3f vert(int i);
    Vec3f vert(int iface, int nthvert);
    
    // ��ȡ uv ����
    Vec2f uv(int iface, int nthvert);

    // ��ȡ�����Ϣ
    std::vector<int> face(int idx);

    // ��ȡ��������ͼ�϶�Ӧuv�������ɫֵ
    TGAColor diffuse(Vec2f uv);

    // ��ȡ���淴��
    TGAColor specular2(Vec2f uvf);

    // ��ȡ���淴����ͼ�϶�Ӧuv�����r��������ɫ�����ҹ�һ��
    float specular(Vec2f uv);


public:
    // ģ�Ͷ��㼯�� v
    std::vector<Vec3f> verts_;
    
    // ģ�͵���ļ��� f
    std::vector<std::vector<Vec3i> > faces_; // attention, this Vec3i means vertex/uv/normal

    // ģ�ͷ��������� vn
    std::vector<Vec3f> norms_;
    
    // ģ��uv���꼯�� vt
    std::vector<Vec2f> uv_;
    
    // ��������ͼ
    TGAImage diffusemap_;
    // ��������ͼ
    TGAImage normalmap_;
    // ���淴����ͼ
    TGAImage specularmap_;

    // ��ȡ������ͼ
    void load_texture(std::string filename, const char* suffix, TGAImage& img);
};
#endif //__MODEL_H__

