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


// 表示颜色的结构体
struct TGAColor
{
    /*
        bgra[4]：一个包含 4 个 unsigned char 元素的数组，用于存储颜色的 BGRA（蓝绿红透明度） 分量。
        bytespp：表示每个像素点占用的字节数，其实就是表示存储的格式（灰度，RGB 等）
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

    // 重载一下*，对颜色进行强度调节
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
    // 图片的存储模式，GRAYSCALE 是灰度图（可以用来表示深度）
    enum Format
    {
        GRAYSCALE = 1, RGB = 3, RGBA = 4
    };

    // 构造函数
    TGAImage();
    TGAImage(int w, int h, int bpp);
    TGAImage(const TGAImage& img);

    ~TGAImage();

    // 重载
    TGAImage& operator =(const TGAImage& img);

    // 读取图像数据
    bool read_tga_file(const char* filename);

    // 将图像数据写入到 TGA 文件中
    bool write_tga_file(const char* filename, bool rle = true);
    
    // 水平翻转图像
    bool flip_horizontally();
    
    // 垂直翻转图像
    bool flip_vertically();
    
    // 对TGA图像进行缩放操作(w,h为缩放后的值)
    bool scale(int w, int h);
    
    // 设置当前像素的颜色
    bool set(int x, int y, TGAColor& c);
    bool set(int x, int y, const TGAColor& c);
    
    // 获取当前像素的颜色
    TGAColor get(int x, int y);

    // 获取
    int get_width();
    int get_height();
    int get_bytespp();
    unsigned char* buffer();

    void clear();

protected:
    // 一个指向无符号字符（unsigned char）的指针，用于存储图像数据的字节数组。
    unsigned char* data;

    // 图像的宽度和高度
    int width;
    int height;

    // 表示每个像素点占用的字节数，用于确定图像数据的格式。
    int bytespp;

    // load_rle_data 函数用于解压缩和读取 RLE 压缩的图像数据。
    bool   load_rle_data(std::ifstream& in);

    // unload_rle_data 则是用来对图像数据进行 RLE（Run-Length Encoding）压缩的操作。
    bool unload_rle_data(std::ofstream& out);
};

#endif //__IMAGE_H__

