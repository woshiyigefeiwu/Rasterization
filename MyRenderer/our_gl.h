#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include <QImage>
#include <QDebug>

#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

struct IShader;

// PI
const double PI = 3.14159265358979323846;

//
extern int Width;
extern int Height;

// 光线方向
extern Vec3f light_dir;

// 视点/摄像机点
extern Vec3f       eye;
//Vec3f       eye;

// 看向的点
extern Vec3f    Center;

// 摄像机的向上方向
extern Vec3f        up;


// 模型，视图 变换矩阵
extern Matrix ModelView;

// 透视投影变换矩阵
extern Matrix Projection;

// 视口变换矩阵
extern Matrix Viewport;

// 获取模型视图变换矩阵（视点/相机点，看向的点，相机的up向量(一般为0,1,0)）
void lookat(Vec3f eye, Vec3f center, Vec3f up);

// 获取透视投影变换矩阵
void projection(float coeff = 0.f); // coeff = -1/c

// 获取视口变换矩阵
void viewport(int x, int y, int w, int h);

// 绘制填充三角形
void triangle(Vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer, QImage &ShowImage);

// 清空显示图像缓存
void ClearBuffer(TGAColor color);

// 设置显示的图像
void SetPixel(QImage &ShowImage, int x, int y, TGAColor color);

// 点绕着 yaw角 旋转（就是绕着y轴，这个角是弧度制）
void rotate_yaw(double x, double y, double z, double yaw_angle, double& x_rotated, double& y_rotated, double& z_rotated);

// 点绕着 pitch角 旋转（就是绕着x轴，这个角是弧度制）
void rotate_pitch(double x, double y, double z, double pitch_angle, double& x_rotated, double& y_rotated, double& z_rotated);

/*---------------------------- 下面是各种着色器的实现 ------------------------*/

struct IShader
{
    /*
        TODO:
            这个各种着色器还是写在 mian.cpp 里面的，
            但是最好是也写在这个文件里面；
            （或者说是得拆出来，便于后面的可编程）
            然后着色器里面用到了modle，所以这里里面还可能得存一个modle的指针
    */

    virtual ~IShader();

    // 顶点着色器（主要用于顶点的坐标变换）
    virtual Vec4f vertex(int iface, int nthvert) = 0;

    // 片段着色器（这个 bar 是重心坐标，通过顶点着色器插值计算出来的信息，进行不同的着色）
    virtual bool fragment(Vec3f bar, TGAColor& color) = 0;

    Model *model = nullptr;
};

/*
    Phong 氏着色
    这里需要特别注意这个法线贴图的概念！
    具体看这里：https://blog.csdn.net/game_jqd/article/details/74858146

    再结合源代码的这一节课：
    https://github.com/ssloy/tinyrenderer/wiki/Lesson-6bis:-tangent-space-normal-mapping
    可以发现，这里用到的法线贴图获取到的法线是位于全局坐标系的（也就是世界空间）
    但是这种方式有缺点，所以我们的法线贴图应该是位于 切线空间（Darboux frame）中的，至于为什么？请看上面的Blog
    所以我们后面从法线贴图获取到的法线，是切线空间中的，同时为了计算光照，我们需要把这个法线从 切线空间 转换到 世界空间中来（TBN 矩阵）
    这个问题就是github项目中 第6课第2部分 要解决的问题。
*/
struct PhongShader : public IShader
{
public:
    PhongShader();
    PhongShader(Model *MewModel)
    {
        model = MewModel;
    }

    // 处理一下颜色
    void setupColor(QColor specularColor, QColor diffuseColor, QColor ambientColor)
    {
        float spec_bgr[3] = {0.0, 0.0, 0.0};        // [0.0 , 1.0] 感觉可以
        float diff_bgr[3] = {0.0, 0.0, 0.0};        // [0.0 , 0.5] 感觉可以
        float ambient_bgr[3] = {0.0, 0.0, 0.0};     // [0.0 , 50] 感觉可以

        // 这里需要注意 rgb 和 bgr
        if(specularColor.isValid())
        {
            QRgb rgb = specularColor.rgb();
            if(qBlue(rgb) == 255 && qGreen(rgb) == 255 && qRed(rgb) == 255)
            {
                rgb = qRgb(0, 0, 0);
            }
            spec_bgr[0] = qBlue(rgb) / 255 * spec_color_max;
            spec_bgr[1] = qGreen(rgb) / 255 * spec_color_max;
            spec_bgr[2] = qRed(rgb) / 255 * spec_color_max;
        }

        if(diffuseColor.isValid())
        {
            QRgb rgb = diffuseColor.rgb();
            if(qBlue(rgb) == 255 && qGreen(rgb) == 255 && qRed(rgb) == 255)
            {
                rgb = qRgb(0, 0, 0);
            }
            diff_bgr[0] = qBlue(rgb) / 255 * diff_color_max;
            diff_bgr[1] = qGreen(rgb) / 255 * diff_color_max;
            diff_bgr[2] = qRed(rgb) / 255 * diff_color_max;
        }

        if(ambientColor.isValid())
        {
            QRgb rgb = ambientColor.rgb();
            if(qBlue(rgb) == 255 && qGreen(rgb) == 255 && qRed(rgb) == 255)
            {
                rgb = qRgb(0, 0, 0);
            }
            ambient_bgr[0] = qBlue(rgb) / 255 * ambient_color_max;
            ambient_bgr[1] = qGreen(rgb) / 255 * ambient_color_max;
            ambient_bgr[2] = qRed(rgb) / 255 * ambient_color_max;
        }

        for(int i=0;i<3;i++)
        {
            spec_pro[i] = spec_bgr[i];
            diff_pro[i] = diff_bgr[i];
            ambient_pro[i] = ambient_bgr[i];
        }
    }

    virtual Vec4f vertex(int iface, int nthvert)
    {
        if(model == nullptr)
        {
            std::cout << "PhongShader.vertex : The model = NULL ！\n";
            return Vec4f();
        }

        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
        return Viewport * Projection * ModelView * gl_Vertex; // transform it to screen coordinates
    }

    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        Vec2f uv = varying_uv * bar;

        // 法向量，记得归一化
        Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();

        // 光向量，记得归一化
        Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();

        // 反射的光照方向
        Vec3f r = (n * (n * l * 2.f) - l).normalize();

        // 镜面反射（这里取幂次方应该是限制了一下高光，防止过曝）
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));

        // 漫反射 = 法向量 和 光向量 之间的夹角的余弦（也就是点积）
        float diff = std::max(0.f, n * l);

        // 获取纹理坐标的颜色
        TGAColor c = model->diffuse(uv);
        color = c;

        // 环境光
        float ambient[3] = {5.0, 5.0, 5.0};

        // 这个 5 是环境光照，diff 的系数是1，spec 的系数是 0.6
        for (int i = 0; i < 3; i++) color[i] = std::min<float>(ambient[i] + ambient_pro[i] + c[i] * (diff + diff_pro[i] + (.6 + spec_pro[i]) * spec), 255);

        /* ----------  这里重新试一下pong模型 -------*/

        return false;
    }

public:
    mat<2, 3, float> varying_uv;  // same as above
    mat<4, 4, float> uniform_M = Projection * ModelView;
    mat<4, 4, float> uniform_MIT = uniform_M.invert_transpose();

    // 颜色的调整 (b g r)(这个是ok的，限制一下范围)
    float ambient_pro[3] = {0.0, 0.0, 0.0};     // [0.0 , 50] 感觉可以
    float spec_pro[3] = {0.0, 0.0, 0.0};        // [0.0 , 1.0] 感觉可以
    float diff_pro[3] = {0.0, 0.0, 0.0};        // [0.0 , 0.5] 感觉可以

    const float spec_color_max = 1.0;
    const float ambient_color_max = 30.0;
    const float diff_color_max = 0.3;
};

// 高洛德着色器
struct GouraudShader : public IShader
{
    GouraudShader();
    GouraudShader(Model *MewModel)
    {
        model = MewModel;
    }

    // 顶点着色器会将数据写入 varying_intensity
    // 片元着色器从 varying_intensity 中读取数据
    Vec3f varying_intensity;

    /*
        用于存储三角形每个顶点的纹理坐标。它的作用是在顶点着色器中将模型的纹理坐标传递给片元着色器。
        顶点着色器会根据每个顶点的面序号和顶点序号计算出对应的纹理坐标，并将其存储在 varying_uv 矩阵中的对应位置。
        varying_uv 的列数表示顶点序号（即0、1、2），行数表示纹理坐标的维度（即u、v）。
    */
    mat<2, 3, float> varying_uv;

    // 接受两个变量，(面序号，顶点序号)
    virtual Vec4f vertex(int iface, int nthvert)
    {
        if(model == nullptr)
        {
            std::cout << "GouraudShader.vertex : The model = NULL ！\n";
            return Vec4f();
        }

        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));

        // 变换顶点坐标到屏幕坐标（视口矩阵 * 投影矩阵 * 模型视图变换矩阵 * v）
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;

        // 计算光照强度（顶点法向量 * 光照方向）
        Vec3f normal = (model->normal(iface, nthvert)).normalize();
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert) * light_dir); // get diffuse lighting intensity

        return gl_Vertex;
    }

    // 根据传入的重心坐标，颜色，以及varying_intensity计算出当前像素的颜色
    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        if(model == nullptr)
        {
            std::cout << "GouraudShader.fragment : The model = NULL ！\n";
            return true;
        }

        // 通过插值，得到当前点插值后的 uv 坐标（插值很重要，它能够使得三角形中不同的点有不同的颜色，能平滑的过度）
        Vec2f uv = varying_uv * bar;

        // 获取插值后的 uv 坐标的的纹理颜色
        TGAColor c = model->diffuse(uv);

        // 插值一下光强
        float intensity = varying_intensity * bar;

        color = c * intensity;
        //color = TGAColor(255, 255, 255) * intensity;

        return false;
    }
};

// 将一定阈值内的光照强度给替换为一种
struct ToonShader : public IShader
{
    ToonShader();
    ToonShader(Model *MewModel)
    {
        model = MewModel;
    }

    //mat<3, 3, float> varying_tri;
    Vec3f varying_ity;

    virtual ~ToonShader() {}

    virtual Vec4f vertex(int iface, int nthvert)
    {
        if(model == nullptr)
        {
            std::cout << "ToonShader.vertex : The model = NULL ！\n";
            return Vec4f();
        }

        // 将顶点坐标转换成齐次坐标
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        gl_Vertex = Projection * ModelView * gl_Vertex;

        //varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));

        // 计算当前这个点光照强度
        varying_ity[nthvert] = model->normal(iface, nthvert) * light_dir;

        gl_Vertex = Viewport * gl_Vertex;
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        if(model == nullptr)
        {
            std::cout << "ToonShader.fragment : The model = NULL ！\n";
            return true;
        }

        // 用点的重心坐标对光强进行插值
        float intensity = varying_ity * bar;
        if (intensity > .85) intensity = 1;
        else if (intensity > .60) intensity = .80;
        else if (intensity > .45) intensity = .60;
        else if (intensity > .30) intensity = .45;
        else if (intensity > .15) intensity = .30;
        color = TGAColor(255, 155, 0) * intensity;
        return false;
    }
};

// 不对法向量进行插值，法向量来源于三角形边的叉积
// 平面着色器（直接用法向量）
struct FlatShader : public IShader
{
    FlatShader();
    FlatShader(Model *MewModel)
    {
        model = MewModel;
    }

    // 三个点的信息
    mat<3, 3, float> varying_tri;

    virtual ~FlatShader() {}

    virtual Vec4f vertex(int iface, int nthvert)
    {
        if(model == nullptr)
        {
            std::cout << "FlatShader.vertex : The model = NULL ！\n";
            return Vec4f();
        }

        // 转换为其次坐标
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        gl_Vertex = Projection * ModelView * gl_Vertex;

        varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        gl_Vertex = Viewport * gl_Vertex;
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        if(model == nullptr)
        {
            std::cout << "FlatShader.fragment : The model = NULL ！\n";
            return true;
        }

        Vec3f n = cross(varying_tri.col(1) - varying_tri.col(0), varying_tri.col(2) - varying_tri.col(0)).normalize();
        float intensity = n * light_dir;
        color = TGAColor(255, 255, 255) * intensity;
        return false;
    }
};

/*
    这个 Shader 项目里面 第6课第2部分（切线空间中的法线贴图）

    这里需要特别注意这个法线贴图的概念！
    具体看这里：https://blog.csdn.net/game_jqd/article/details/74858146

    再结合源代码的这一节课：
    https://github.com/ssloy/tinyrenderer/wiki/Lesson-6bis:-tangent-space-normal-mapping
    可以发现，这里用到的法线贴图获取到的法线是位于全局坐标系的（也就是世界空间）
    但是这种方式有缺点，所以我们的法线贴图应该是位于 切线空间（Darboux frame）中的，至于为什么？请看上面的Blog
    所以我们后面从法线贴图获取到的法线，是切线空间中的，同时为了计算光照，我们需要把这个法线从 切线空间 转换到 世界空间中来（TBN 矩阵）
    这个问题就是github项目中 第6课第2部分 要解决的问题。

    TBN 矩阵就是 世界空间 到 切线空间 之间互相转变的桥梁：
        - 世界空间 = TBN矩阵的逆 x 切线空间
        - 切线空间 = TBN矩阵 x 世界空间
*/
struct TSNormalMap_Shader : public IShader
{
    TSNormalMap_Shader();
    TSNormalMap_Shader(Model *MewModel)
    {
        model = MewModel;
    }

    // 三角形的 uv 坐标，vertex shader 输出的，作为 fragment shader 输入
    mat<2, 3, float> varying_uv;

    // 三角形顶点坐标（注意这个是其次坐标！）
    mat<4, 3, float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS

    // 每个顶点的法线，将被FS插值
    mat<3, 3, float> varying_nrm;

    // 三角形顶点归一化后的坐标（其实就是除以齐次坐标的最后一维）
    mat<3, 3, float> ndc_tri;

    virtual Vec4f vertex(int iface, int nthvert)
    {
        if(model == nullptr)
        {
            std::cout << "TSNormalMap_Shader.vertex : The model = NULL ！\n";
            return Vec4f();
        }

        // 获取顶点的uv坐标
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));

        // 获取顶点的法向量
        varying_nrm.set_col(nthvert, proj<3>((Projection * ModelView).invert_transpose() * embed<4>(model->normal(iface, nthvert), 0.f)));

        // 获取顶点的坐标
        Vec4f gl_Vertex = Viewport * Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, gl_Vertex);

        // 归一化坐标
        ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));

        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        if(model == nullptr)
        {
            std::cout << "TSNormalMap_Shader.fragment : The model = NULL ！\n";
            return true;
        }

        // 用重心坐标 插值一下
        Vec3f bn = (varying_nrm * bar).normalize();
        Vec2f uv = varying_uv * bar;

        /*
            从
                mat<3, 3, float> A;
            这一行代码一直到
                B.set_col(2, bn);
            都是在求一个 TBN 矩阵（的逆）
            （只不过求法不同，这里是通过求基的形式算的，但是看起来就非常的奇怪...）
            （用于 世界空间 和 切线空间 的转换）
        */
        mat<3, 3, float> A;
        A[0] = ndc_tri.col(1) - ndc_tri.col(0);
        A[1] = ndc_tri.col(2) - ndc_tri.col(0);
        A[2] = bn;

        mat<3, 3, float> AI = A.invert();

        Vec3f i = AI * Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
        Vec3f j = AI * Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);

        // 这个 B 就是 TBN 矩阵的逆
        mat<3, 3, float> B;
        B.set_col(0, i.normalize());
        B.set_col(1, j.normalize());
        B.set_col(2, bn);

        // 世界空间 = TBN矩阵的逆 x 切线空间
        Vec3f n = (B * model->normal(uv)).normalize();

        float diff = std::max(0.f, n * light_dir);
        color = model->diffuse(uv) * diff;
        //for (int i = 0; i < 3; i++) color[i] = std::min<float>(color[i] + 50, 255);

        return false;
    }
};


#endif //__OUR_GL_H__

