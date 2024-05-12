#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tgaimage.h"

TGAImage::TGAImage() : data(NULL), width(0), height(0), bytespp(0) {}

TGAImage::TGAImage(int w, int h, int bpp) : data(NULL), width(w), height(h), bytespp(bpp)
{
    unsigned long nbytes = width * height * bytespp;
    data = new unsigned char[nbytes];
    memset(data, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage& img) : data(NULL), width(img.width), height(img.height), bytespp(img.bytespp) 
{
    unsigned long nbytes = width * height * bytespp;
    data = new unsigned char[nbytes];
    memcpy(data, img.data, nbytes);
}

TGAImage::~TGAImage() 
{
    if (data) delete [] data;
}

TGAImage& TGAImage::operator =(const TGAImage& img)
{
    if (this != &img) {
        if (data) delete[] data;
        width = img.width;
        height = img.height;
        bytespp = img.bytespp;
        unsigned long nbytes = width * height * bytespp;
        data = new unsigned char[nbytes];
        memcpy(data, img.data, nbytes);
    }
    return *this;
}

// 读取图像数据
bool TGAImage::read_tga_file(const char* filename) 
{
    // 清空一下
    if (data) delete[] data;
    data = NULL;

    // Input file stream class
    std::ifstream in;
    in.open(filename, std::ios::binary);    // 以二进制的形式打开文件
    if (!in.is_open()) 
    {
        std::cerr << "can't open file " << filename << "\n";
        in.close();
        return false;
    }

    // 读取二进制文件里面的数据部分，并存到 TGA_Header 结构体中
    TGA_Header header;
    in.read((char*)&header, sizeof(header));
    if (!in.good()) 
    {
        in.close();
        std::cerr << "an error occured while reading the header\n";
        return false;
    }

    // 初始化一些参数
    width = header.width;
    height = header.height;
    bytespp = header.bitsperpixel >> 3;
    if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA)) 
    {
        in.close();
        std::cerr << "bad bpp (or width/height) value\n";
        return false;
    }
    
    // 图像数据
    unsigned long nbytes = bytespp * width * height;
    data = new unsigned char[nbytes];

    // 数据类型码为 2 3，表示图像数据是连续存储的，直接读到缓存就行
    if (3 == header.datatypecode || 2 == header.datatypecode) 
    {
        in.read((char*)data, nbytes);
        if (!in.good()) 
        {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    }
    // 如果数据类型码为 10 或 11，表示图像数据是经过了 RLE 压缩的，需要调用 load_rle_data 函数进行解压缩并读取数据。
    else if (10 == header.datatypecode || 11 == header.datatypecode) 
    {
        if (!load_rle_data(in)) 
        {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    }
    else 
    {
        in.close();
        std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
        return false;
    }

    // 根据 imagedescriptor 属性来判断是否需要将图像 水平翻转 或者 垂直翻转
    if (!(header.imagedescriptor & 0x20))
    {
        flip_vertically();
    }
    if (header.imagedescriptor & 0x10)
    {
        flip_horizontally();
    }
    
    std::cerr << width << "x" << height << "/" << bytespp * 8 << "\n";
    in.close();
    return true;
}

// load_rle_data 函数用于解压缩和读取 RLE 压缩的图像数据。
bool TGAImage::load_rle_data(std::ifstream& in) 
{
    unsigned long pixelcount = width * height;
    unsigned long currentpixel = 0;
    unsigned long currentbyte = 0;
    TGAColor colorbuffer;

    do 
    {
        // 每次读取一个 chunk 的数据
        unsigned char chunkheader = 0;
        chunkheader = in.get();
        if (!in.good()) 
        {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
        
        // 如果 chunkheader 的值小于 128，表示该 chunk 中包含连续的像素，需要读取 chunkheader+1 个像素的数据。
        if (chunkheader < 128) 
        {
            chunkheader ++;
            for (int i = 0; i < chunkheader; ++i)
            {
                in.read((char*)colorbuffer.bgra, bytespp);
                if (!in.good())
                {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }
                
                for (int t = 0; t < bytespp; t++)
                    data[currentbyte ++] = colorbuffer.bgra[t];

                currentpixel++;
                if (currentpixel > pixelcount) 
                {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
        // 如果 chunkheader 的值大于等于 128，表示该 chunk 中包含重复的像素，需要读取该像素的数据 chunkheader-127 次。
        // 读取的像素数据会存储在 data 数组中。
        else 
        {
            chunkheader -= 127;
            in.read((char*)colorbuffer.bgra, bytespp);
            if (!in.good())
            {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }
            
            for (int i = 0; i < chunkheader; i++) 
            {
                for (int t = 0; t < bytespp; t++)
                    data[currentbyte++] = colorbuffer.bgra[t];

                currentpixel++;
                if (currentpixel > pixelcount) 
                {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    } while (currentpixel < pixelcount);
    return true;
}

// 将图像数据写入到 TGA 文件中
bool TGAImage::write_tga_file(const char* filename, bool rle) 
{
    unsigned char developer_area_ref[4] = { 0, 0, 0, 0 };
    unsigned char extension_area_ref[4] = { 0, 0, 0, 0 };
    unsigned char footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };
    
    std::ofstream out;
    out.open(filename, std::ios::binary);
    if (!out.is_open()) 
    {
        std::cerr << "can't open file " << filename << "\n";
        out.close();
        return false;
    }

    TGA_Header header;
    memset((void*)&header, 0, sizeof(header));
    header.bitsperpixel = bytespp << 3;
    header.width = width;
    header.height = height;
    header.datatypecode = (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.imagedescriptor = 0x20; // top-left origin
    
    // 将 header 数据读到 out 里面
    out.write((char*)&header, sizeof(header));
    if (!out.good()) 
    {
        out.close();
        std::cerr << "can't dump the tga file\n";
        return false;
    }
    
    if (!rle) 
    {
        out.write((char*)data, width * height * bytespp);
        if (!out.good()) 
        {
            std::cerr << "can't unload raw data\n";
            out.close();
            return false;
        }
    }
    else 
    {
        if (!unload_rle_data(out)) 
        {
            out.close();
            std::cerr << "can't unload rle data\n";
            return false;
        }
    }

    /*
        这一段代码用于将开发者区域引用（developer_area_ref）、扩展区域引用（extension_area_ref）和文件尾部（footer）写入到 TGA 文件中。
        TGA 文件格式允许在文件中包含一个开发者区域和一个扩展区域，用于存储额外的自定义数据或扩展功能。在这段代码中，首先使用 out.write 函数将开发者区域引用的内容写入到文件中，大小为 sizeof(developer_area_ref)。然后再将扩展区域引用和文件尾部的内容按照相同的方式写入到文件中。
        在每次写入操作后，通过 out.good() 检查写入是否成功。如果写入失败，则会输出错误信息，并关闭文件流，然后返回 false 表示写入失败。
        总的来说，这段代码是将开发者区域引用、扩展区域引用和文件尾部的内容写入到 TGA 文件中的操作。这些区域在 TGA 文件中并非必需，根据需要可以选择是否使用它们。
    */

    out.write((char*)developer_area_ref, sizeof(developer_area_ref));
    if (!out.good()) 
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char*)extension_area_ref, sizeof(extension_area_ref));
    if (!out.good()) 
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char*)footer, sizeof(footer));
    if (!out.good()) 
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }

    out.close();
    return true;
}

// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
/*
    unload_rle_data 则是用来对图像数据进行 RLE（Run-Length Encoding）压缩的操作。
    RLE 是一种简单的数据压缩算法，用于连续重复的数据块的压缩。
    在这个函数中，对图像数据进行遍历，找出连续重复的像素块，并用一定规则对其进行压缩，然后将压缩后的数据写入到文件中。
*/ 
bool TGAImage::unload_rle_data(std::ofstream& out)
{
    const unsigned char max_chunk_length = 128;
    unsigned long npixels = width * height;
    unsigned long curpix = 0;
    while (curpix < npixels) 
    {
        unsigned long chunkstart = curpix * bytespp;
        unsigned long curbyte = curpix * bytespp;
        unsigned char run_length = 1;
        bool raw = true;
        while (curpix + run_length < npixels && run_length < max_chunk_length) 
        {
            bool succ_eq = true;
            for (int t = 0; succ_eq && t < bytespp; t++) 
            {
                succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);
            }
            curbyte += bytespp;
            if (1 == run_length) 
            {
                raw = !succ_eq;
            }
            if (raw && succ_eq) 
            {
                run_length--;
                break;
            }
            if (!raw && !succ_eq) 
            {
                break;
            }
            run_length++;
        }
        curpix += run_length;
        
        out.put(raw ? run_length - 1 : run_length + 127);
        if (!out.good()) 
        {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
        out.write((char*)(data + chunkstart), (raw ? run_length * bytespp : bytespp));
        if (!out.good()) 
        {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
    }
    return true;
}

// 将图像水平翻转
bool TGAImage::flip_horizontally()
{
    if (!data) return false;
    int half = width >> 1;
    for (int i = 0; i < half; i++)
    {
        for (int j = 0; j < height; j++)
        {
            TGAColor c1 = get(i, j);
            TGAColor c2 = get(width - 1 - i, j);
            set(i, j, c2);
            set(width - 1 - i, j, c1);
        }
    }
    return true;
}

// 将图像垂直翻转
bool TGAImage::flip_vertically()
{
    if (!data) return false;
    unsigned long bytes_per_line = width * bytespp;
    // 临时
    unsigned char* line = new unsigned char[bytes_per_line];
    int half = height >> 1;
    for (int j = 0; j < half; j++)
    {
        unsigned long l1 = j * bytes_per_line;
        unsigned long l2 = (height - 1 - j) * bytes_per_line;

        /*
            void *memmove(void *dest, const void *source, std::size_t size);
            将指针source所指向内存区域的前size个字节复制到指针dest所指向的内存区域。
        */
        memmove((void*)line, (void*)(data + l1), bytes_per_line);
        memmove((void*)(data + l1), (void*)(data + l2), bytes_per_line);
        memmove((void*)(data + l2), (void*)line, bytes_per_line);
    }
    delete[] line;
    return true;
}

// 获取当前像素的颜色
TGAColor TGAImage::get(int x, int y)
{
    if (!data || x < 0 || y < 0 || x >= width || y >= height) 
    {
        return TGAColor();
    }
    /*
        (x + y * width) 计算出像素在一维数组中的偏移量。
        (x + y * width) * bytespp 根据每个像素占用的字节数，计算出像素在字节数组中的真实偏移量。
    */
    return TGAColor(data + (x + y * width) * bytespp, bytespp);
}

// 设置当前像素的颜色
bool TGAImage::set(int x, int y, TGAColor& c)
{
    if (!data || x < 0 || y < 0 || x >= width || y >= height)
    {
        return false;
    }
    memcpy(data + (x + y * width) * bytespp, c.bgra, bytespp);
    return true;
}

bool TGAImage::set(int x, int y, const TGAColor& c) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {
        return false;
    }
    memcpy(data + (x + y * width) * bytespp, c.bgra, bytespp);
    return true;
}

int TGAImage::get_bytespp() 
{
    return bytespp;
}

int TGAImage::get_width() 
{
    return width;
}

int TGAImage::get_height() 
{
    return height;
}

unsigned char *TGAImage::buffer() 
{
    return data;
}

void TGAImage::clear()
{
    memset((void*)data, 0, width * height * bytespp);
}

/*
    对TGA图像进行缩放操作
    函数使用两个嵌套的循环来遍历原始图像的每个像素。在内部循环中，通过处理errx和erry来计算新图像中每个像素在原图中的位置。
    这种算法称为双线性插值，用于在缩放过程中平滑地处理像素的颜色。
*/ 
bool TGAImage::scale(int w, int h)
{
    if (w <= 0 || h <= 0 || !data) return false;
    unsigned char* tdata = new unsigned char[w * h * bytespp];  // 结果
    int nscanline = 0;
    int oscanline = 0;
    int erry = 0;
    unsigned long nlinebytes = w * bytespp;
    unsigned long olinebytes = width * bytespp;
    for (int j = 0; j < height; j++) 
    {
        int errx = width - w;
        int nx = -bytespp;
        int ox = -bytespp;
        for (int i = 0; i < width; i++) 
        {
            ox += bytespp;
            errx += w;
            while (errx >= (int)width) 
            {
                errx -= width;
                nx += bytespp;
                memcpy(tdata + nscanline + nx, data + oscanline + ox, bytespp);
            }
        }

        erry += h;
        oscanline += olinebytes;
        while (erry >= (int)height) 
        {
            if (erry >= (int)height << 1) // it means we jump over a scanline
                memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);
            erry -= height;
            nscanline += nlinebytes;
        }
    }

    delete[] data;
    data = tdata;
    width = w;
    height = h;
    return true;
}

