#include <cmath>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include "our_gl.h"

using namespace std;

//// 光线方向
//Vec3f light_dir(0,1,1);

//// 视点/摄像机点
//Vec3f       eye(1, 0.5, 1.5);
////Vec3f       eye(0, 0, 3);

//// 看向的点
//Vec3f    Center(0,0,0);

//// 摄像机的向上方向
//Vec3f        up(0,1,0);

// 都是 4 x 4 的矩阵
Matrix ModelView;
Matrix Viewport;
Matrix Projection;

IShader::~IShader() {}

// 点绕着 yaw角 旋转（就是绕着y轴，这个角是弧度制）
void rotate_yaw(double x, double y, double z, double yaw_angle, double& x_rotated, double& y_rotated, double& z_rotated)
{
    double cos_yaw = cos(yaw_angle);
    double sin_yaw = sin(yaw_angle);

    x_rotated = x * cos_yaw - y * sin_yaw;
    y_rotated = x * sin_yaw + y * cos_yaw;
    z_rotated = z;
}

// 点绕着 pitch角 旋转（就是绕着x轴，这个角是弧度制）
void rotate_pitch(double x, double y, double z, double pitch_angle, double& x_rotated, double& y_rotated, double& z_rotated)
{
    double cos_pitch = cos(pitch_angle);
    double sin_pitch = sin(pitch_angle);

    x_rotated = x * cos_pitch + z * sin_pitch;
    y_rotated = y;
    z_rotated = -x * sin_pitch + z * cos_pitch;
}

// 模型视图变换矩阵
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

// 投影矩阵
void projection(float coeff)
{
    Projection = Matrix::identity();
    Projection[3][2] = coeff;
}

// 视口变换矩阵
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

// 计算重心坐标
Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P)
{
    // 用于存储三角形ABC的两条边向量。
    Vec3f s[2];
    for (int i = 2; i--; ) 
    {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }
    
    // 这个向量与三角形ABC的法向量平行
    Vec3f u = cross(s[0], s[1]);    
    if (std::abs(u[2]) > 1e-2)
    {
        // 分别计算了点P相对于三个顶点A、B、C的权重。权重的计算是基于三角形面积的性质
        return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    }
    
    return Vec3f(-1, 1, 1);
}

// 设置显示的图像
void SetPixel(QImage &ShowImage, int x, int y, TGAColor color)
{
    /*
        ps：这里需要特别的注意！

            我们在设置颜色的时候，是按照 RGB 去设置的对吧；
            但是这里需要特别注意，因为我们用的是 TGAColor，这个 TGAColor 里面的 BGR 的排列；
            所以我们在传的时候不能写成 QColor(color[0], color[1], color[2]) ！
            这么写会导致颜色的不对！
    */
    ShowImage.setPixelColor(x, Height - 1 - y, QColor(color[2], color[1], color[0]));
}

// ---------------------------------------------------------------------- 渲染方式 ----------------------------------------------------------------------

/*
    绘制填充三角形（这里用的又是包围盒了...）
    注意这里的 pts 用的是齐次坐标，所以要除以最后的分量才是真正的坐标

    面渲染
*/
void triangle(Vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer, QImage &ShowImage)
{
    // 求三角形的包围盒
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

    // 当前像素坐标P，颜色color
    Vec2i P;
    TGAColor color;

    // 遍历边界框中的每一个像素
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) 
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) 
        {
            // 求一下重心坐标（注意齐次坐标需要除以最后一个分量）
            Vec3f c = barycentric(proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), proj<2>(pts[2] / pts[2][3]), proj<2>(P));
            
            // 插值计算P的zbuffer
            // pts[i]为三角形的三个顶点
            // pts[i][2]为三角形的z信息(0~255)
            // pts[i][3]为三角形的投影系数(1-z/c)
            float z_P = (pts[0][2] / pts[0][3]) * c.x + (pts[1][2] / pts[1][3]) * c.y + (pts[2][2] / pts[2][3]) * c.z;
            int frag_depth = std::max(0, std::min(255, int(z_P + .5)));

            // P的任一质心分量小于0或者zbuffer小于已有zbuffer，不渲染
            if (c.x < 0 || c.y < 0 || c.z<0 || zbuffer.get(P.x, P.y)[0] > frag_depth) continue;

            // 调用片元着色器计算当前像素颜色
            bool discard = shader.fragment(c, color);
            if (!discard) 
            {
                // 更新 zbuffer
                zbuffer.set(P.x, P.y, TGAColor(frag_depth));

                // 为像素设置颜色
                image.set(P.x, P.y, color);

                // 设置显示图片的像素颜色
                SetPixel(ShowImage, P.x, P.y, color);
            }
        }
    }
}

// 点渲染
void DrawPoint(Vec4f* pts, TGAImage& zbuffer, QImage &ShowImage)
{
    // 这里有个非常需要注意的点：就是我们记得要除以第四维，因为这是一个其次坐标（第四维其实是投影系数），除了才变成正真的点，否则我们会看到画面被拉伸了

    TGAColor color(0,0,0);

    // 做法一：直接把点设置成颜色，因为点太少了，而且太小了，这么做根本看不出轮廓...
//    for(int i=0;i<3;i++)
//    {
//        SetPixel(ShowImage, pts[i][0] / pts[i][3], pts[i][1] / pts[i][3], color);
//    }


    // 做法二：求包围盒，遍历包围盒中的点（这里我们可以处理的稀疏一点），这样看起来效果比较好！
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

    // 当前像素坐标P，颜色color
    Vec2i P;

    // 遍历边界框中的每一个像素
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x += 5)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y += 5)
        {
            // 求一下重心坐标（注意齐次坐标需要除以最后一个分量）
            Vec3f c = barycentric(proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), proj<2>(pts[2] / pts[2][3]), proj<2>(P));

            // 插值计算P的zbuffer
            // pts[i]为三角形的三个顶点
            // pts[i][2]为三角形的z信息(0~255)
            // pts[i][3]为三角形的投影系数(1-z/c)
            float z_P = (pts[0][2] / pts[0][3]) * c.x + (pts[1][2] / pts[1][3]) * c.y + (pts[2][2] / pts[2][3]) * c.z;
            int frag_depth = std::max(0, std::min(255, int(z_P + .5)));

            // P的任一质心分量小于0或者zbuffer小于已有zbuffer，不渲染
            if (c.x < 0 || c.y < 0 || c.z<0 || zbuffer.get(P.x, P.y)[0] > frag_depth) continue;

            SetPixel(ShowImage, P.x, P.y, color);
        }
    }
}

// 划线
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

// 线渲染
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

