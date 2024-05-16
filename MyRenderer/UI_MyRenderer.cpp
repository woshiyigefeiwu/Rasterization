#include "UI_MyRenderer.h"
#include "ui_UI_MyRenderer.h"
#include "our_gl.h"
#include <QImageReader>
#include <QColorDialog>

// 光线方向
Vec3f light_dir(0,1,1);

// 视点/摄像机点
Vec3f       eye(1, 0.5, 1.5);
//Vec3f       eye(1, 1, 3);

// 看向的点
Vec3f    Center(0,0,0);

// 摄像机的向上方向
Vec3f        up(0,1,0);

int Width  = 800;
int Height = 800;

/*----------------------------------------- 公有成员函数 ---------------------------------------*/
UI_MyRenderer::UI_MyRenderer(QWidget *parent) :QMainWindow(parent), ui(new Ui::UI_MyRenderer)
{
    ui->setupUi(this);

    InitUI();
    InitSignalAndSlot();
}

UI_MyRenderer::~UI_MyRenderer()
{
    delete ui;
}

/*----------------------------------------- protected 成员函数 ---------------------------------------*/

// 这个就是画图函数
void UI_MyRenderer::paintEvent(QPaintEvent *)
{
    // 创建QPainter对象，用于绘制图形
    QPainter painter(this);

    painter.setPen(Qt::red);

    // 绘制QImage
    if(!ShowImage.isNull())
    {
        painter.drawImage(50, 50, ShowImage);
    }
}

/*----------------------------------------- 私有成员函数 ---------------------------------------*/

// 初始化 UI
void UI_MyRenderer::InitUI()
{
    setFixedSize(1500, 1000);

    // 初始化 视图变换矩阵，投影矩阵，视口矩阵
    InitMatrix();
}

// 初始化 视图变换矩阵，投影矩阵，视口矩阵
void UI_MyRenderer::InitMatrix()
{
    lookat(eye, Center, up);
    projection(-1.f / (eye - Center).norm());
    viewport(Width / 8, Height / 8, Width * 3 / 4, Height * 3 / 4);
    light_dir.normalize();
}

void UI_MyRenderer::InitSignalAndSlot()
{
    // 光线的位置
    connect(ui->LightPitchSlider, SIGNAL(valueChanged(int)), this, SLOT(on_LightPitchSlider_ValueChanged(int)));
    connect(ui->LightYawDial, SIGNAL(valueChanged(int)), this, SLOT(on_LightYawDial_ValueChanged(int)));

    // 光线的
    connect(ui->AmbientCheckBox, SIGNAL(stateChanged(int)), this, SLOT(on_AmbientCheckBox_stateChanged(int)));
    connect(ui->DiffuseCheckBox, SIGNAL(stateChanged(int)), this, SLOT(on_DiffuseCheckBox_stateChanged(int)));
    connect(ui->SpecularCheckBox, SIGNAL(stateChanged(int)), this, SLOT(on_SpecularCheckBox_stateChanged(int)));

    // 相机位置
    connect(ui->CameraPitchSlider, SIGNAL(valueChanged(int)), this, SLOT(on_CameraPitchSlider_ValueChanged(int)));
    connect(ui->CameraYawDial, SIGNAL(valueChanged(int)), this, SLOT(on_CameraYawDial_ValueChanged(int)));
    connect(ui->CameraFovSlider, SIGNAL(valueChanged(int)), this, SLOT(on_CameraFovSlider_ValueChanged(int)));
}

// 加载模型
void UI_MyRenderer::LoadModel(QString filePath)
{
    model = new Model(filePath.toStdString().c_str());
    Render();       // 开始渲染
}

// 保存图片
bool UI_MyRenderer::SaveImage(QString filePath)
{
    if(!ShowImage.isNull()) {
        return ShowImage.save(filePath);
    }
    return false;
}

// 开始渲染
void UI_MyRenderer::Render()
{
    if(model == nullptr) return;

    qDebug() << "model点数：" << model->nverts() << "；model面数：" << model->nfaces() << '\n';

    if(renderMode == FACE) FaceRender();
    else if(renderMode == VERTEX) VertexRender();
    else if(renderMode == LINE) LineRender();
}

// 清空画面
void UI_MyRenderer::clearImage()
{
    //在这里初始化一下 Image
    QImage TempImage(Width, Height, QImage::Format_RGB888);
    TempImage.fill(QColor(255.f, 255.f, 255.f));
    ShowImage = TempImage;

    update();
}

// 面渲染
void UI_MyRenderer::FaceRender()
{
    //在这里初始化一下 Image
    QImage TempImage(Width, Height, QImage::Format_RGB888);
    TempImage.fill(QColor(255.f, 255.f, 255.f));
    ShowImage = TempImage;

    // 可以开始渲染了

    // 初始化image和zbuffer
    TGAImage image(Width, Height, TGAImage::RGB);
    TGAImage zbuffer(Width, Height, TGAImage::GRAYSCALE);

    // 实例化 Phong 着色
    PhongShader shader(model);
    shader.setupColor(specularColor, diffuseColor, ambientColor);   // 设置一下选择的颜色

    // 遍历模型的面
    for (int i = 0; i < model->nfaces(); i++)
    {
        // 三角形的屏幕坐标
        Vec4f screen_coords[3];
        for (int j = 0; j < 3; j++)
        {
            /*
                通过顶点着色器读取模型顶点
                变换顶点坐标到屏幕坐标（视角矩阵 * 投影矩阵 * 模型视图变换矩阵 * v）
                （注意这里给的是齐次坐标哦，要用的话还需要除以最后一个分量的）
                计算光照强度
            */
            screen_coords[j] = shader.vertex(i, j);
        }

        // 遍历完3个顶点，一个三角形光栅化完成
        // 绘制三角形，triangle内部通过片元着色器对三角形着色
        triangle(screen_coords, shader, image, zbuffer, ShowImage);
    }

    update();

//    image.flip_vertically();
//    zbuffer.flip_vertically();
//    image.write_tga_file("tempimage_8.tga");
//    zbuffer.write_tga_file("zbuffer.tga");
}

// 线渲染
void UI_MyRenderer::LineRender()
{
    //在这里初始化一下 Image
    QImage TempImage(Width, Height, QImage::Format_RGB888);
    TempImage.fill(QColor(255.f, 255.f, 255.f));
    ShowImage = TempImage;
    TGAImage zbuffer(Width, Height, TGAImage::GRAYSCALE);

    // 实例化 Phong 着色
    PhongShader shader(model);
    shader.setupColor(specularColor, diffuseColor, ambientColor);   // 设置一下选择的颜色

    // 遍历模型的面
    for (int i = 0; i < model->nfaces(); i++)
    {
        // 三角形的屏幕坐标
        Vec4f screen_coords[3];
        for (int j = 0; j < 3; j++)
        {
            /*
                通过顶点着色器读取模型顶点
                变换顶点坐标到屏幕坐标（视角矩阵 * 投影矩阵 * 模型视图变换矩阵 * v）
                （注意这里给的是齐次坐标哦，要用的话还需要除以最后一个分量的）
                计算光照强度
            */
            screen_coords[j] = shader.vertex(i, j);
        }


        DrawLine(screen_coords, ShowImage);
    }

    update();
}

// 点渲染
void UI_MyRenderer::VertexRender()
{
    //在这里初始化一下 Image
    QImage TempImage(Width, Height, QImage::Format_RGB888);
    TempImage.fill(QColor(255.f, 255.f, 255.f));
    ShowImage = TempImage;
    TGAImage zbuffer(Width, Height, TGAImage::GRAYSCALE);

    // 实例化 Phong 着色
    PhongShader shader(model);
    shader.setupColor(specularColor, diffuseColor, ambientColor);   // 设置一下选择的颜色

    // 遍历模型的面
    for (int i = 0; i < model->nfaces(); i++)
    {
        // 三角形的屏幕坐标
        Vec4f screen_coords[3];
        for (int j = 0; j < 3; j++)
        {
            /*
                通过顶点着色器读取模型顶点
                变换顶点坐标到屏幕坐标（视角矩阵 * 投影矩阵 * 模型视图变换矩阵 * v）
                （注意这里给的是齐次坐标哦，要用的话还需要除以最后一个分量的）
                计算光照强度
            */
            screen_coords[j] = shader.vertex(i, j);
        }

        DrawPoint(screen_coords, zbuffer, ShowImage);
    }

    update();
}

/*----------------------------------------- 槽函数 ---------------------------------------*/

//
void UI_MyRenderer::on_CameraFovSlider_ValueChanged(int val)
{
    InitMatrix();
    Render();
}

void UI_MyRenderer::on_CameraYawDial_ValueChanged(int val)
{
    qDebug() << "rotate_yaw 之前 ： " << eye[0] << " , "  << eye[1] << " , " << eye[2] << "\n";

    double rate = 1.0 * val / 360 * (2 * PI);
    double res[3];
    rotate_yaw(eye[0], eye[1], eye[2], rate, res[0], res[1], res[2]);
    for(int i=0;i<3;i++) eye[i] = res[i];

    qDebug() << "val = " << val << " , rate = " << rate;
    qDebug() << "rotate_yaw 之后 ： " << res[0] << " , "  << res[1] << " , " << res[2] << "\n";

    InitMatrix();
    Render();
}

void UI_MyRenderer::on_CameraPitchSlider_ValueChanged(int val)
{
    qDebug() << "rotate_pitch 之前 ： " << eye[0] << " , "  << eye[1] << " , " << eye[2] << "\n";

    double rate = 1.0 * val / 360 * (2 * PI);
    double res[3];
    rotate_pitch(eye[0], eye[1], eye[2], rate, res[0], res[1], res[2]);
    for(int i=0;i<3;i++) eye[i] = res[i];

    qDebug() << "val = " << val << " , rate = " << rate;
    qDebug() << "rotate_pitch 之后 ： " << res[0] << " , "  << res[1] << " , " << res[2] << "\n";

    InitMatrix();
    Render();
}

//
void UI_MyRenderer::on_SpecularCheckBox_stateChanged(int state)
{
    if (state == Qt::Checked)
    {
        specularFlag = true;
//        qDebug() << "state == Qt::Checked" << qRed(specularColor.rgb()) << " " << qGreen(specularColor.rgb()) << " " << qBlue(specularColor.rgb());
    }
    else if (state == Qt::Unchecked)
    {
        specularFlag = false;

        if(qRed(specularColor.rgb()) == 0 && qGreen(specularColor.rgb()) == 0 && qBlue(specularColor.rgb()) == 0)
        {
            return;
        }

        // 优化一下，需要重新设置的时候再渲染
        specularColor.setRgb(0, 0, 0);
        Render();
    }
}

void UI_MyRenderer::on_DiffuseCheckBox_stateChanged(int state)
{
    if (state == Qt::Checked)
    {
        diffuseFlag = true;
    }
    else if (state == Qt::Unchecked)
    {
        diffuseFlag = false;

        if(qRed(diffuseColor.rgb()) == 0 && qGreen(diffuseColor.rgb()) == 0 && qBlue(diffuseColor.rgb()) == 0)
        {
            return;
        }

        // 优化一下，需要重新设置的时候再渲染
        diffuseColor.setRgb(0,0,0);
        Render();
    }
}

void UI_MyRenderer::on_AmbientCheckBox_stateChanged(int state)
{
    if (state == Qt::Checked)
    {
        ambientFlag = true;
    }
    else if (state == Qt::Unchecked)
    {
        ambientFlag = false;
        if(qRed(ambientColor.rgb()) == 0 && qGreen(ambientColor.rgb()) == 0 && qBlue(ambientColor.rgb()) == 0)
        {
            return;
        }

        // 优化一下，需要重新设置的时候再渲染
        ambientColor.setRgb(0,0,0);
        Render();
    }
}


//
void UI_MyRenderer::on_AmbientColorButton_clicked()
{
    ambientColor = QColorDialog::getColor(Qt::white, this);
    if(ambientFlag == false)
    {
        ambientColor.setRgb(0,0,0);
    }
    Render();
}

void UI_MyRenderer::on_DiffuseColorButton_clicked()
{
    diffuseColor = QColorDialog::getColor(Qt::white, this);
    if(diffuseFlag == false)
    {
        diffuseColor.setRgb(0,0,0);
    }
    Render();
}

void UI_MyRenderer::on_SpecularColorButton_clicked()
{
    specularColor = QColorDialog::getColor(Qt::white, this);
    if(specularFlag == false)
    {
        specularColor.setRgb(0,0,0);
    }
    Render();
}

// 光线 Yaw 角变化
void UI_MyRenderer::on_LightYawDial_ValueChanged(int val)
{
    double rate = 1.0 * val / 360 * (2 * PI);
    double res[3];
    rotate_yaw(light_dir[0], light_dir[1], light_dir[2], rate, res[0], res[1], res[2]);
    for(int i=0;i<3;i++) light_dir[i] = res[i];

    InitMatrix();
    Render();
//    qDebug() << "on_LightYawDial_ValueChanged : " << val;
}

// 光线 pitch 角变化
void UI_MyRenderer::on_LightPitchSlider_ValueChanged(int val)
{
    double rate = 1.0 * val / 360 * (2 * PI);
    double res[3];
    rotate_pitch(light_dir[0], light_dir[1], light_dir[2], rate, res[0], res[1], res[2]);
    for(int i=0;i<3;i++) light_dir[i] = res[i];

    InitMatrix();
    Render();
//    qDebug() << "on_LightPitchSlider_ValueChanged : " << val;
}

// 点击打开文件
void UI_MyRenderer::on_action_OpenFile_triggered()
{
    qDebug() << "This is on_action_OpenFile_triggered()！\n";

    QString filePath = QFileDialog::getOpenFileName(this, "Open Model File", "", "OBJ(*.obj)");
    if(!filePath.isEmpty())
    {
        qDebug() << filePath << '\n';
        LoadModel(filePath);
    }
}

// 点击保存图片
void UI_MyRenderer::on_action_SaveImage_triggered()
{
    qDebug() << "This is on_action_SaveImage_triggered()！\n";

    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG(*.png)");
    if(!fileName.isEmpty() && SaveImage(fileName)) {
        qDebug() << "保存图片成功！\n";
    }
    else {
        qDebug() << "保存图片失败！\n";
    }
}

// 点击多线程
void UI_MyRenderer::on_action_MultiThread_triggered()
{
    qDebug() << "This is on_action_MultiThread_triggered()！\n";
}

// 点击平面裁切
void UI_MyRenderer::on_action_FaceCulling_triggered()
{
    qDebug() << "This is on_action_FaceCulling_triggered()！\n";
}

// 点击关于
void UI_MyRenderer::on_action_About_triggered()
{
    qDebug() << "This is on_action_About_triggered()！\n";
}

void UI_MyRenderer::on_LineCheckBox_stateChanged(int state)       // 线渲染
{
    clearImage();
    if (state == Qt::Unchecked)
    {
        SetRenderMode(FACE);
    }
    else if(state == Qt::Checked)
    {
        SetRenderMode(LINE);
    }

    Render();
}
void UI_MyRenderer::on_VertexCheckBox_stateChanged(int state)       // 点渲染
{
    clearImage();
    if (state == Qt::Unchecked)
    {
        SetRenderMode(FACE);
    }
    else if(state == Qt::Checked)
    {
        SetRenderMode(VERTEX);
    }

    Render();
}
