#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#pragma pack(push,1)
struct TGA_Header 
{
    char idlength;
    char colormaptype;
    char datatypecode;
    short colormaporigin;
    short colormaplength;
    char colormapdepth;
    short x_origin;
    short y_origin;
    short width;
    short height;
    char  bitsperpixel;
    char  imagedescriptor;
};
#pragma pack(pop)


// ��ʾ��ɫ�Ľṹ��
struct TGAColor
{
    /*
        bgra[4]��һ������ 4 �� unsigned char Ԫ�ص����飬���ڴ洢��ɫ�� BGRA�����̺�͸���ȣ� ������
        bytespp����ʾÿ�����ص�ռ�õ��ֽ�������ʵ���Ǳ�ʾ�洢�ĸ�ʽ���Ҷȣ�RGB �ȣ�
    */
    unsigned char bgra[4];
    unsigned char bytespp;

    TGAColor() : bgra(), bytespp(1)
    {
        for (int i = 0; i < 4; i++) bgra[i] = 0;
    }

    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255) : bgra(), bytespp(4)
    {
        bgra[0] = B;
        bgra[1] = G;
        bgra[2] = R;
        bgra[3] = A;
    }

    TGAColor(unsigned char v) : bgra(), bytespp(1)
    {
        for (int i = 0; i < 4; i++) bgra[i] = 0;
        bgra[0] = v;
    }

    TGAColor(const unsigned char* p, unsigned char bpp) : bgra(), bytespp(bpp)
    {
        for (int i = 0; i < (int)bpp; i++)
        {
            bgra[i] = p[i];
        }
        for (int i = bpp; i < 4; i++)
        {
            bgra[i] = 0;
        }
    }

    unsigned char& operator[](const int i) { return bgra[i]; }

    // ����һ��*������ɫ����ǿ�ȵ���
    TGAColor operator *(float intensity) const
    {
        TGAColor res = *this;
        intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
        for (int i = 0; i < 4; i++) res.bgra[i] = bgra[i] * intensity;
        return res;
    }
};


class TGAImage
{
public:
    // ͼƬ�Ĵ洢ģʽ��GRAYSCALE �ǻҶ�ͼ������������ʾ��ȣ�
    enum Format
    {
        GRAYSCALE = 1, RGB = 3, RGBA = 4
    };

    // ���캯��
    TGAImage();
    TGAImage(int w, int h, int bpp);
    TGAImage(const TGAImage& img);

    ~TGAImage();

    // ����
    TGAImage& operator =(const TGAImage& img);

    // ��ȡͼ������
    bool read_tga_file(const char* filename);

    // ��ͼ������д�뵽 TGA �ļ���
    bool write_tga_file(const char* filename, bool rle = true);
    
    // ˮƽ��תͼ��
    bool flip_horizontally();
    
    // ��ֱ��תͼ��
    bool flip_vertically();
    
    // ��TGAͼ��������Ų���(w,hΪ���ź��ֵ)
    bool scale(int w, int h);
    
    // ���õ�ǰ���ص���ɫ
    bool set(int x, int y, TGAColor& c);
    bool set(int x, int y, const TGAColor& c);
    
    // ��ȡ��ǰ���ص���ɫ
    TGAColor get(int x, int y);

    // ��ȡ
    int get_width();
    int get_height();
    int get_bytespp();
    unsigned char* buffer();

    void clear();

protected:
    // һ��ָ���޷����ַ���unsigned char����ָ�룬���ڴ洢ͼ�����ݵ��ֽ����顣
    unsigned char* data;

    // ͼ��Ŀ�Ⱥ͸߶�
    int width;
    int height;

    // ��ʾÿ�����ص�ռ�õ��ֽ���������ȷ��ͼ�����ݵĸ�ʽ��
    int bytespp;

    // load_rle_data �������ڽ�ѹ���Ͷ�ȡ RLE ѹ����ͼ�����ݡ�
    bool   load_rle_data(std::ifstream& in);

    // unload_rle_data ����������ͼ�����ݽ��� RLE��Run-Length Encoding��ѹ���Ĳ�����
    bool unload_rle_data(std::ofstream& out);
};

#endif //__IMAGE_H__

