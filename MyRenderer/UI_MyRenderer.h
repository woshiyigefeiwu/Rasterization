#ifndef UI_MYRENDERER_H
#define UI_MYRENDERER_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QPainter>
#include <QPaintEvent>
#include "model.h"

namespace Ui {
class UI_MyRenderer;
}

// 选项：多线程，平面裁切
enum Option{MUTITHREAD, FACECULLING};

// 渲染模式(面，线，点)
enum RenderMode{FACE,LINE,VERTEX};

class UI_MyRenderer : public QMainWindow
{
    Q_OBJECT

/*--------------------------- 公有成员函数 -----------------------------*/
public:
    explicit UI_MyRenderer(QWidget *parent = nullptr);
    ~UI_MyRenderer();

    // 设置选项
    void SetOption(Option option, bool val);

    // 设置渲染模式
    void SetRenderMode(RenderMode mode) {renderMode = mode;}

/*--------------------------- protected 成员函数 -----------------------------*/
protected:
    // 画图函数
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;


/*--------------------------- 私有成员函数 -----------------------------*/
private:
    // 初始化 UI
    void InitUI();

    // 初始化矩阵
    void InitMatrix();

    // 初始化信号绑定
    void InitSignalAndSlot();

    // 加载模型
    void LoadModel(QString filePath);

    // 保存图片
    bool SaveImage(QString filePath);

    // 开始渲染
    void Render();

    // 清空
    void clearImage();

    // 面渲染
    void FaceRender();

    // 线渲染
    void LineRender();

    // 点渲染
    void VertexRender();


/*--------------------------- 槽函数 -----------------------------*/
private slots:

    void on_SpecularColorButton_clicked();

    void on_DiffuseColorButton_clicked();

    void on_AmbientColorButton_clicked();

    void on_action_OpenFile_triggered();

    void on_action_SaveImage_triggered();

    void on_action_MultiThread_triggered();

    void on_action_FaceCulling_triggered();

    void on_action_About_triggered();

    // 光线
    void on_LightPitchSlider_ValueChanged(int val);

    void on_LightYawDial_ValueChanged(int val);

    void on_SpecularCheckBox_stateChanged(int state);

    void on_DiffuseCheckBox_stateChanged(int state);

    void on_AmbientCheckBox_stateChanged(int state);

    // 摄像机
    void on_CameraPitchSlider_ValueChanged(int val);

    void on_CameraYawDial_ValueChanged(int val);

    void on_CameraFovSlider_ValueChanged(int val);

    // 模型
    void on_LineCheckBox_stateChanged(int state);       // 线渲染

    void on_VertexCheckBox_stateChanged(int state);       // 点渲染


/*--------------------------- 公有成员变量 -----------------------------*/
public:
    RenderMode renderMode = FACE;

/*--------------------------- 私有成员变量 -----------------------------*/
private:
    Ui::UI_MyRenderer *ui;

    // 模型
    Model *model = nullptr;

    // 最终显示的图像
    QImage ShowImage;

    // BlingPhong 模型的三种颜色(颜色选择器)
    QColor specularColor;       // 高光
    QColor diffuseColor;        // 漫反射光
    QColor ambientColor;        // 环境光

    // 选中框的状态
    bool specularFlag = false;
    bool diffuseFlag = false;
    bool ambientFlag = false; 
};

#endif // UI_MYRENDERER_H
