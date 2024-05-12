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

// ���߷���
extern Vec3f light_dir;

// �ӵ�/�������
extern Vec3f       eye;
//Vec3f       eye;

// ����ĵ�
extern Vec3f    Center;

// ����������Ϸ���
extern Vec3f        up;


// ģ�ͣ���ͼ �任����
extern Matrix ModelView;

// ͸��ͶӰ�任����
extern Matrix Projection;

// �ӿڱ任����
extern Matrix Viewport;

// ��ȡģ����ͼ�任�����ӵ�/����㣬����ĵ㣬�����up����(һ��Ϊ0,1,0)��
void lookat(Vec3f eye, Vec3f center, Vec3f up);

// ��ȡ͸��ͶӰ�任����
void projection(float coeff = 0.f); // coeff = -1/c

// ��ȡ�ӿڱ任����
void viewport(int x, int y, int w, int h);

// �������������
void triangle(Vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer, QImage &ShowImage);

// �����ʾͼ�񻺴�
void ClearBuffer(TGAColor color);

// ������ʾ��ͼ��
void SetPixel(QImage &ShowImage, int x, int y, TGAColor color);

// ������ yaw�� ��ת����������y�ᣬ������ǻ����ƣ�
void rotate_yaw(double x, double y, double z, double yaw_angle, double& x_rotated, double& y_rotated, double& z_rotated);

// ������ pitch�� ��ת����������x�ᣬ������ǻ����ƣ�
void rotate_pitch(double x, double y, double z, double pitch_angle, double& x_rotated, double& y_rotated, double& z_rotated);

/*---------------------------- �����Ǹ�����ɫ����ʵ�� ------------------------*/

struct IShader
{
    /*
        TODO:
            ���������ɫ������д�� mian.cpp ����ģ�
            ���������Ҳд������ļ����棻
            ������˵�ǵò���������ں���Ŀɱ�̣�
            Ȼ����ɫ�������õ���modle�������������滹���ܵô�һ��modle��ָ��
    */

    virtual ~IShader();

    // ������ɫ������Ҫ���ڶ��������任��
    virtual Vec4f vertex(int iface, int nthvert) = 0;

    // Ƭ����ɫ������� bar ���������꣬ͨ��������ɫ����ֵ�����������Ϣ�����в�ͬ����ɫ��
    virtual bool fragment(Vec3f bar, TGAColor& color) = 0;

    Model *model = nullptr;
};

/*
    Phong ����ɫ
    ������Ҫ�ر�ע�����������ͼ�ĸ��
    ���忴���https://blog.csdn.net/game_jqd/article/details/74858146

    �ٽ��Դ�������һ�ڿΣ�
    https://github.com/ssloy/tinyrenderer/wiki/Lesson-6bis:-tangent-space-normal-mapping
    ���Է��֣������õ��ķ�����ͼ��ȡ���ķ�����λ��ȫ������ϵ�ģ�Ҳ��������ռ䣩
    �������ַ�ʽ��ȱ�㣬�������ǵķ�����ͼӦ����λ�� ���߿ռ䣨Darboux frame���еģ�����Ϊʲô���뿴�����Blog
    �������Ǻ���ӷ�����ͼ��ȡ���ķ��ߣ������߿ռ��еģ�ͬʱΪ�˼�����գ�������Ҫ��������ߴ� ���߿ռ� ת���� ����ռ�������TBN ����
    ����������github��Ŀ�� ��6�ε�2���� Ҫ��������⡣
*/
struct PhongShader : public IShader
{
public:
    PhongShader();
    PhongShader(Model *MewModel)
    {
        model = MewModel;
    }

    // ����һ����ɫ
    void setupColor(QColor specularColor, QColor diffuseColor, QColor ambientColor)
    {
        float spec_bgr[3] = {0.0, 0.0, 0.0};        // [0.0 , 1.0] �о�����
        float diff_bgr[3] = {0.0, 0.0, 0.0};        // [0.0 , 0.5] �о�����
        float ambient_bgr[3] = {0.0, 0.0, 0.0};     // [0.0 , 50] �о�����

        // ������Ҫע�� rgb �� bgr
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
            std::cout << "PhongShader.vertex : The model = NULL ��\n";
            return Vec4f();
        }

        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
        return Viewport * Projection * ModelView * gl_Vertex; // transform it to screen coordinates
    }

    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        Vec2f uv = varying_uv * bar;

        // ���������ǵù�һ��
        Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();

        // ���������ǵù�һ��
        Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();

        // ����Ĺ��շ���
        Vec3f r = (n * (n * l * 2.f) - l).normalize();

        // ���淴�䣨����ȡ�ݴη�Ӧ����������һ�¸߹⣬��ֹ���أ�
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));

        // ������ = ������ �� ������ ֮��ļнǵ����ң�Ҳ���ǵ����
        float diff = std::max(0.f, n * l);

        // ��ȡ�����������ɫ
        TGAColor c = model->diffuse(uv);
        color = c;

        // ������
        float ambient[3] = {5.0, 5.0, 5.0};

        // ��� 5 �ǻ������գ�diff ��ϵ����1��spec ��ϵ���� 0.6
        for (int i = 0; i < 3; i++) color[i] = std::min<float>(ambient[i] + ambient_pro[i] + c[i] * (diff + diff_pro[i] + (.6 + spec_pro[i]) * spec), 255);

        /* ----------  ����������һ��pongģ�� -------*/

        return false;
    }

public:
    mat<2, 3, float> varying_uv;  // same as above
    mat<4, 4, float> uniform_M = Projection * ModelView;
    mat<4, 4, float> uniform_MIT = uniform_M.invert_transpose();

    // ��ɫ�ĵ��� (b g r)(�����ok�ģ�����һ�·�Χ)
    float ambient_pro[3] = {0.0, 0.0, 0.0};     // [0.0 , 50] �о�����
    float spec_pro[3] = {0.0, 0.0, 0.0};        // [0.0 , 1.0] �о�����
    float diff_pro[3] = {0.0, 0.0, 0.0};        // [0.0 , 0.5] �о�����

    const float spec_color_max = 1.0;
    const float ambient_color_max = 30.0;
    const float diff_color_max = 0.3;
};

// �������ɫ��
struct GouraudShader : public IShader
{
    GouraudShader();
    GouraudShader(Model *MewModel)
    {
        model = MewModel;
    }

    // ������ɫ���Ὣ����д�� varying_intensity
    // ƬԪ��ɫ���� varying_intensity �ж�ȡ����
    Vec3f varying_intensity;

    /*
        ���ڴ洢������ÿ��������������ꡣ�����������ڶ�����ɫ���н�ģ�͵��������괫�ݸ�ƬԪ��ɫ����
        ������ɫ�������ÿ�����������źͶ�����ż������Ӧ���������꣬������洢�� varying_uv �����еĶ�Ӧλ�á�
        varying_uv ��������ʾ������ţ���0��1��2����������ʾ���������ά�ȣ���u��v����
    */
    mat<2, 3, float> varying_uv;

    // ��������������(����ţ��������)
    virtual Vec4f vertex(int iface, int nthvert)
    {
        if(model == nullptr)
        {
            std::cout << "GouraudShader.vertex : The model = NULL ��\n";
            return Vec4f();
        }

        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));

        // �任�������굽��Ļ���꣨�ӿھ��� * ͶӰ���� * ģ����ͼ�任���� * v��
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;

        // �������ǿ�ȣ����㷨���� * ���շ���
        Vec3f normal = (model->normal(iface, nthvert)).normalize();
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert) * light_dir); // get diffuse lighting intensity

        return gl_Vertex;
    }

    // ���ݴ�����������꣬��ɫ���Լ�varying_intensity�������ǰ���ص���ɫ
    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        if(model == nullptr)
        {
            std::cout << "GouraudShader.fragment : The model = NULL ��\n";
            return true;
        }

        // ͨ����ֵ���õ���ǰ���ֵ��� uv ���꣨��ֵ����Ҫ�����ܹ�ʹ���������в�ͬ�ĵ��в�ͬ����ɫ����ƽ���Ĺ��ȣ�
        Vec2f uv = varying_uv * bar;

        // ��ȡ��ֵ��� uv ����ĵ�������ɫ
        TGAColor c = model->diffuse(uv);

        // ��ֵһ�¹�ǿ
        float intensity = varying_intensity * bar;

        color = c * intensity;
        //color = TGAColor(255, 255, 255) * intensity;

        return false;
    }
};

// ��һ����ֵ�ڵĹ���ǿ�ȸ��滻Ϊһ��
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
            std::cout << "ToonShader.vertex : The model = NULL ��\n";
            return Vec4f();
        }

        // ����������ת�����������
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        gl_Vertex = Projection * ModelView * gl_Vertex;

        //varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));

        // ���㵱ǰ��������ǿ��
        varying_ity[nthvert] = model->normal(iface, nthvert) * light_dir;

        gl_Vertex = Viewport * gl_Vertex;
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        if(model == nullptr)
        {
            std::cout << "ToonShader.fragment : The model = NULL ��\n";
            return true;
        }

        // �õ����������Թ�ǿ���в�ֵ
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

// ���Է��������в�ֵ����������Դ�������αߵĲ��
// ƽ����ɫ����ֱ���÷�������
struct FlatShader : public IShader
{
    FlatShader();
    FlatShader(Model *MewModel)
    {
        model = MewModel;
    }

    // ���������Ϣ
    mat<3, 3, float> varying_tri;

    virtual ~FlatShader() {}

    virtual Vec4f vertex(int iface, int nthvert)
    {
        if(model == nullptr)
        {
            std::cout << "FlatShader.vertex : The model = NULL ��\n";
            return Vec4f();
        }

        // ת��Ϊ�������
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
            std::cout << "FlatShader.fragment : The model = NULL ��\n";
            return true;
        }

        Vec3f n = cross(varying_tri.col(1) - varying_tri.col(0), varying_tri.col(2) - varying_tri.col(0)).normalize();
        float intensity = n * light_dir;
        color = TGAColor(255, 255, 255) * intensity;
        return false;
    }
};

/*
    ��� Shader ��Ŀ���� ��6�ε�2���֣����߿ռ��еķ�����ͼ��

    ������Ҫ�ر�ע�����������ͼ�ĸ��
    ���忴���https://blog.csdn.net/game_jqd/article/details/74858146

    �ٽ��Դ�������һ�ڿΣ�
    https://github.com/ssloy/tinyrenderer/wiki/Lesson-6bis:-tangent-space-normal-mapping
    ���Է��֣������õ��ķ�����ͼ��ȡ���ķ�����λ��ȫ������ϵ�ģ�Ҳ��������ռ䣩
    �������ַ�ʽ��ȱ�㣬�������ǵķ�����ͼӦ����λ�� ���߿ռ䣨Darboux frame���еģ�����Ϊʲô���뿴�����Blog
    �������Ǻ���ӷ�����ͼ��ȡ���ķ��ߣ������߿ռ��еģ�ͬʱΪ�˼�����գ�������Ҫ��������ߴ� ���߿ռ� ת���� ����ռ�������TBN ����
    ����������github��Ŀ�� ��6�ε�2���� Ҫ��������⡣

    TBN ������� ����ռ� �� ���߿ռ� ֮�以��ת���������
        - ����ռ� = TBN������� x ���߿ռ�
        - ���߿ռ� = TBN���� x ����ռ�
*/
struct TSNormalMap_Shader : public IShader
{
    TSNormalMap_Shader();
    TSNormalMap_Shader(Model *MewModel)
    {
        model = MewModel;
    }

    // �����ε� uv ���꣬vertex shader ����ģ���Ϊ fragment shader ����
    mat<2, 3, float> varying_uv;

    // �����ζ������꣨ע�������������꣡��
    mat<4, 3, float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS

    // ÿ������ķ��ߣ�����FS��ֵ
    mat<3, 3, float> varying_nrm;

    // �����ζ����һ��������꣨��ʵ���ǳ��������������һά��
    mat<3, 3, float> ndc_tri;

    virtual Vec4f vertex(int iface, int nthvert)
    {
        if(model == nullptr)
        {
            std::cout << "TSNormalMap_Shader.vertex : The model = NULL ��\n";
            return Vec4f();
        }

        // ��ȡ�����uv����
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));

        // ��ȡ����ķ�����
        varying_nrm.set_col(nthvert, proj<3>((Projection * ModelView).invert_transpose() * embed<4>(model->normal(iface, nthvert), 0.f)));

        // ��ȡ���������
        Vec4f gl_Vertex = Viewport * Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, gl_Vertex);

        // ��һ������
        ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));

        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        if(model == nullptr)
        {
            std::cout << "TSNormalMap_Shader.fragment : The model = NULL ��\n";
            return true;
        }

        // ���������� ��ֵһ��
        Vec3f bn = (varying_nrm * bar).normalize();
        Vec2f uv = varying_uv * bar;

        /*
            ��
                mat<3, 3, float> A;
            ��һ�д���һֱ��
                B.set_col(2, bn);
            ��������һ�� TBN ���󣨵��棩
            ��ֻ�����󷨲�ͬ��������ͨ���������ʽ��ģ����ǿ������ͷǳ������...��
            ������ ����ռ� �� ���߿ռ� ��ת����
        */
        mat<3, 3, float> A;
        A[0] = ndc_tri.col(1) - ndc_tri.col(0);
        A[1] = ndc_tri.col(2) - ndc_tri.col(0);
        A[2] = bn;

        mat<3, 3, float> AI = A.invert();

        Vec3f i = AI * Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
        Vec3f j = AI * Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);

        // ��� B ���� TBN �������
        mat<3, 3, float> B;
        B.set_col(0, i.normalize());
        B.set_col(1, j.normalize());
        B.set_col(2, bn);

        // ����ռ� = TBN������� x ���߿ռ�
        Vec3f n = (B * model->normal(uv)).normalize();

        float diff = std::max(0.f, n * light_dir);
        color = model->diffuse(uv) * diff;
        //for (int i = 0; i < 3; i++) color[i] = std::min<float>(color[i] + 50, 255);

        return false;
    }
};


#endif //__OUR_GL_H__

