#include <cmath>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include "our_gl.h"

using namespace std;

//// ���߷���
//Vec3f light_dir(0,1,1);

//// �ӵ�/�������
//Vec3f       eye(1, 0.5, 1.5);
////Vec3f       eye(0, 0, 3);

//// ����ĵ�
//Vec3f    Center(0,0,0);

//// ����������Ϸ���
//Vec3f        up(0,1,0);

// ���� 4 x 4 �ľ���
Matrix ModelView;
Matrix Viewport;
Matrix Projection;

IShader::~IShader() {}

// ������ yaw�� ��ת����������y�ᣬ������ǻ����ƣ�
void rotate_yaw(double x, double y, double z, double yaw_angle, double& x_rotated, double& y_rotated, double& z_rotated)
{
    double cos_yaw = cos(yaw_angle);
    double sin_yaw = sin(yaw_angle);

    x_rotated = x * cos_yaw - y * sin_yaw;
    y_rotated = x * sin_yaw + y * cos_yaw;
    z_rotated = z;
}

// ������ pitch�� ��ת����������x�ᣬ������ǻ����ƣ�
void rotate_pitch(double x, double y, double z, double pitch_angle, double& x_rotated, double& y_rotated, double& z_rotated)
{
    double cos_pitch = cos(pitch_angle);
    double sin_pitch = sin(pitch_angle);

    x_rotated = x * cos_pitch + z * sin_pitch;
    y_rotated = y;
    z_rotated = -x * sin_pitch + z * cos_pitch;
}

// ģ����ͼ�任����
void lookat(Vec3f eye, Vec3f center, Vec3f up)
{
    Vec3f z = (eye - center).normalize();
    Vec3f x = cross(up, z).normalize();
    Vec3f y = cross(z, x).normalize();
    ModelView = Matrix::identity();
    Matrix translaition = Matrix::identity();
    Matrix rotation = Matrix::identity();
    for (int i = 0; i < 3; i++) 
    {
        translaition[i][3] = -center[i];
    }
    for (int i = 0; i < 3; i++) 
    {
        rotation[0][i] = x[i];
        rotation[1][i] = y[i];
        rotation[2][i] = z[i];
    }
    ModelView = rotation * translaition;
}

// ͶӰ����
void projection(float coeff)
{
    Projection = Matrix::identity();
    Projection[3][2] = coeff;
}

// �ӿڱ任����
void viewport(int x, int y, int w, int h) 
{
    Viewport = Matrix::identity();
    Viewport[0][3] = x+w/2.f;
    Viewport[1][3] = y+h/2.f;
    Viewport[2][3] = 255.f/2.f;
    Viewport[0][0] = w/2.f;
    Viewport[1][1] = h/2.f;
    Viewport[2][2] = 255.f/2.f;
}

// ������������
Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P)
{
    // ���ڴ洢������ABC��������������
    Vec3f s[2];
    for (int i = 2; i--; ) 
    {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }
    
    // ���������������ABC�ķ�����ƽ��
    Vec3f u = cross(s[0], s[1]);    
    if (std::abs(u[2]) > 1e-2)
    {
        // �ֱ�����˵�P�������������A��B��C��Ȩ�ء�Ȩ�صļ����ǻ������������������
        return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    }
    
    return Vec3f(-1, 1, 1);
}

// ������ʾ��ͼ��
void SetPixel(QImage &ShowImage, int x, int y, TGAColor color)
{
    /*
        ps��������Ҫ�ر��ע�⣡

            ������������ɫ��ʱ���ǰ��� RGB ȥ���õĶ԰ɣ�
            ����������Ҫ�ر�ע�⣬��Ϊ�����õ��� TGAColor����� TGAColor ����� BGR �����У�
            ���������ڴ���ʱ����д�� QColor(color[0], color[1], color[2]) ��
            ��ôд�ᵼ����ɫ�Ĳ��ԣ�
    */
    ShowImage.setPixelColor(x, Height - 1 - y, QColor(color[2], color[1], color[0]));
}

// ---------------------------------------------------------------------- ��Ⱦ��ʽ ----------------------------------------------------------------------

/*
    ������������Σ������õ����ǰ�Χ����...��
    ע������� pts �õ���������꣬����Ҫ�������ķ�����������������

    ����Ⱦ
*/
void triangle(Vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer, QImage &ShowImage)
{
    // �������εİ�Χ��
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j] / pts[i][3]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
        }
    }

    // ��ǰ��������P����ɫcolor
    Vec2i P;
    TGAColor color;

    // �����߽���е�ÿһ������
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) 
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) 
        {
            // ��һ���������꣨ע�����������Ҫ�������һ��������
            Vec3f c = barycentric(proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), proj<2>(pts[2] / pts[2][3]), proj<2>(P));
            
            // ��ֵ����P��zbuffer
            // pts[i]Ϊ�����ε���������
            // pts[i][2]Ϊ�����ε�z��Ϣ(0~255)
            // pts[i][3]Ϊ�����ε�ͶӰϵ��(1-z/c)
            float z_P = (pts[0][2] / pts[0][3]) * c.x + (pts[1][2] / pts[1][3]) * c.y + (pts[2][2] / pts[2][3]) * c.z;
            int frag_depth = std::max(0, std::min(255, int(z_P + .5)));

            // P����һ���ķ���С��0����zbufferС������zbuffer������Ⱦ
            if (c.x < 0 || c.y < 0 || c.z<0 || zbuffer.get(P.x, P.y)[0] > frag_depth) continue;

            // ����ƬԪ��ɫ�����㵱ǰ������ɫ
            bool discard = shader.fragment(c, color);
            if (!discard) 
            {
                // ���� zbuffer
                zbuffer.set(P.x, P.y, TGAColor(frag_depth));

                // Ϊ����������ɫ
                image.set(P.x, P.y, color);

                // ������ʾͼƬ��������ɫ
                SetPixel(ShowImage, P.x, P.y, color);
            }
        }
    }
}

// ����Ⱦ
void DrawPoint(Vec4f* pts, TGAImage& zbuffer, QImage &ShowImage)
{
    // �����и��ǳ���Ҫע��ĵ㣺�������Ǽǵ�Ҫ���Ե���ά����Ϊ����һ��������꣨����ά��ʵ��ͶӰϵ���������˲ű������ĵ㣬�������ǻῴ�����汻������

    TGAColor color(0,0,0);

    // ����һ��ֱ�Ӱѵ����ó���ɫ����Ϊ��̫���ˣ�����̫С�ˣ���ô����������������...
//    for(int i=0;i<3;i++)
//    {
//        SetPixel(ShowImage, pts[i][0] / pts[i][3], pts[i][1] / pts[i][3], color);
//    }


    // �����������Χ�У�������Χ���еĵ㣨�������ǿ��Դ����ϡ��һ�㣩������������Ч���ȽϺã�
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j] / pts[i][3]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
        }
    }

    // ��ǰ��������P����ɫcolor
    Vec2i P;

    // �����߽���е�ÿһ������
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x += 5)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y += 5)
        {
            // ��һ���������꣨ע�����������Ҫ�������һ��������
            Vec3f c = barycentric(proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), proj<2>(pts[2] / pts[2][3]), proj<2>(P));

            // ��ֵ����P��zbuffer
            // pts[i]Ϊ�����ε���������
            // pts[i][2]Ϊ�����ε�z��Ϣ(0~255)
            // pts[i][3]Ϊ�����ε�ͶӰϵ��(1-z/c)
            float z_P = (pts[0][2] / pts[0][3]) * c.x + (pts[1][2] / pts[1][3]) * c.y + (pts[2][2] / pts[2][3]) * c.z;
            int frag_depth = std::max(0, std::min(255, int(z_P + .5)));

            // P����һ���ķ���С��0����zbufferС������zbuffer������Ⱦ
            if (c.x < 0 || c.y < 0 || c.z<0 || zbuffer.get(P.x, P.y)[0] > frag_depth) continue;

            SetPixel(ShowImage, P.x, P.y, color);
        }
    }
}

// ����
void DrawLine(pair<Vec2f,Vec2f> line, QImage &ShowImage)
{
    TGAColor color(0,0,0);

    int x0 = line.first[0];
    int x1 = line.second[0];
    int y0 = line.first[1];
    int y1 = line.second[1];

    bool steep = false;
    if (abs(x0 - x1) < abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int k = dy > 0 ? 1 : -1;
    if (dy < 0)dy = -dy;
    float e = -dx;
    int x = x0, y = y0;
    while (x != x1)
    {
        if (steep) SetPixel(ShowImage, y, x, color);
        else SetPixel(ShowImage, x, y, color);

        e += (2 * dy);
        if (e > 0)
        {
            y += k;
            e -= (2 * dx);
        }
        ++x;
    }
}

// ����Ⱦ
void DrawLine(Vec4f* pts, QImage &ShowImage)
{
    Vec2f po[3];
    for(int i=0;i<3;i++)
    {
        po[i] = {pts[i][0] / pts[i][3], pts[i][1] / pts[i][3]};
    }
    pair<Vec2f,Vec2f> line[3] =
    {
        {po[0], po[1]},
        {po[1], po[2]},
        {po[2], po[0]}
    };

    for(int i=0;i<3;i++)
    {
        DrawLine(line[i], ShowImage);
    }
}

