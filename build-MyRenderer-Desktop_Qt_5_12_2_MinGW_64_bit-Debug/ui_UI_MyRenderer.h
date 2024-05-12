/********************************************************************************
** Form generated from reading UI file 'UI_MyRenderer.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UI_MYRENDERER_H
#define UI_UI_MYRENDERER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UI_MyRenderer
{
public:
    QAction *action_OpenFile;
    QAction *action_SaveImage;
    QAction *action_MultiThread;
    QAction *action_FaceCulling;
    QAction *action_About;
    QWidget *centralWidget;
    QTabWidget *Setting;
    QWidget *LightSetting;
    QSlider *LightPitchSlider;
    QDial *LightYawDial;
    QLabel *PitchLabel;
    QLabel *YawLabel;
    QPushButton *SpecularColorButton;
    QPushButton *DiffuseColorButton;
    QPushButton *AmbientColorButton;
    QCheckBox *SpecularCheckBox;
    QCheckBox *DiffuseCheckBox;
    QCheckBox *AmbientCheckBox;
    QWidget *CameraSetting;
    QLabel *PitchLabel_2;
    QLabel *YawLabel_2;
    QSlider *CameraPitchSlider;
    QDial *CameraYawDial;
    QSlider *CameraFovSlider;
    QLabel *FovLabel;
    QWidget *ModelSetting;
    QMenuBar *menuBar;
    QMenu *FileMenu;
    QMenu *SettingMenu;
    QMenu *HelpMenu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *UI_MyRenderer)
    {
        if (UI_MyRenderer->objectName().isEmpty())
            UI_MyRenderer->setObjectName(QString::fromUtf8("UI_MyRenderer"));
        UI_MyRenderer->setEnabled(true);
        UI_MyRenderer->resize(1296, 690);
        action_OpenFile = new QAction(UI_MyRenderer);
        action_OpenFile->setObjectName(QString::fromUtf8("action_OpenFile"));
        action_OpenFile->setCheckable(false);
        action_SaveImage = new QAction(UI_MyRenderer);
        action_SaveImage->setObjectName(QString::fromUtf8("action_SaveImage"));
        action_MultiThread = new QAction(UI_MyRenderer);
        action_MultiThread->setObjectName(QString::fromUtf8("action_MultiThread"));
        action_MultiThread->setCheckable(true);
        action_FaceCulling = new QAction(UI_MyRenderer);
        action_FaceCulling->setObjectName(QString::fromUtf8("action_FaceCulling"));
        action_FaceCulling->setCheckable(true);
        action_About = new QAction(UI_MyRenderer);
        action_About->setObjectName(QString::fromUtf8("action_About"));
        centralWidget = new QWidget(UI_MyRenderer);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setEnabled(true);
        Setting = new QTabWidget(centralWidget);
        Setting->setObjectName(QString::fromUtf8("Setting"));
        Setting->setEnabled(true);
        Setting->setGeometry(QRect(870, 20, 391, 571));
        Setting->setTabPosition(QTabWidget::North);
        Setting->setTabShape(QTabWidget::Rounded);
        LightSetting = new QWidget();
        LightSetting->setObjectName(QString::fromUtf8("LightSetting"));
        LightPitchSlider = new QSlider(LightSetting);
        LightPitchSlider->setObjectName(QString::fromUtf8("LightPitchSlider"));
        LightPitchSlider->setEnabled(true);
        LightPitchSlider->setGeometry(QRect(70, 70, 18, 160));
        LightPitchSlider->setMaximum(359);
        LightPitchSlider->setOrientation(Qt::Vertical);
        LightYawDial = new QDial(LightSetting);
        LightYawDial->setObjectName(QString::fromUtf8("LightYawDial"));
        LightYawDial->setGeometry(QRect(230, 90, 101, 151));
        LightYawDial->setMaximum(359);
        PitchLabel = new QLabel(LightSetting);
        PitchLabel->setObjectName(QString::fromUtf8("PitchLabel"));
        PitchLabel->setGeometry(QRect(60, 30, 41, 31));
        YawLabel = new QLabel(LightSetting);
        YawLabel->setObjectName(QString::fromUtf8("YawLabel"));
        YawLabel->setGeometry(QRect(260, 70, 41, 31));
        SpecularColorButton = new QPushButton(LightSetting);
        SpecularColorButton->setObjectName(QString::fromUtf8("SpecularColorButton"));
        SpecularColorButton->setGeometry(QRect(220, 280, 161, 24));
        DiffuseColorButton = new QPushButton(LightSetting);
        DiffuseColorButton->setObjectName(QString::fromUtf8("DiffuseColorButton"));
        DiffuseColorButton->setGeometry(QRect(220, 330, 161, 24));
        AmbientColorButton = new QPushButton(LightSetting);
        AmbientColorButton->setObjectName(QString::fromUtf8("AmbientColorButton"));
        AmbientColorButton->setGeometry(QRect(220, 380, 161, 24));
        SpecularCheckBox = new QCheckBox(LightSetting);
        SpecularCheckBox->setObjectName(QString::fromUtf8("SpecularCheckBox"));
        SpecularCheckBox->setGeometry(QRect(50, 280, 141, 21));
        DiffuseCheckBox = new QCheckBox(LightSetting);
        DiffuseCheckBox->setObjectName(QString::fromUtf8("DiffuseCheckBox"));
        DiffuseCheckBox->setGeometry(QRect(50, 330, 141, 21));
        AmbientCheckBox = new QCheckBox(LightSetting);
        AmbientCheckBox->setObjectName(QString::fromUtf8("AmbientCheckBox"));
        AmbientCheckBox->setGeometry(QRect(50, 380, 141, 21));
        Setting->addTab(LightSetting, QString());
        CameraSetting = new QWidget();
        CameraSetting->setObjectName(QString::fromUtf8("CameraSetting"));
        PitchLabel_2 = new QLabel(CameraSetting);
        PitchLabel_2->setObjectName(QString::fromUtf8("PitchLabel_2"));
        PitchLabel_2->setGeometry(QRect(70, 40, 41, 31));
        YawLabel_2 = new QLabel(CameraSetting);
        YawLabel_2->setObjectName(QString::fromUtf8("YawLabel_2"));
        YawLabel_2->setGeometry(QRect(270, 80, 41, 31));
        CameraPitchSlider = new QSlider(CameraSetting);
        CameraPitchSlider->setObjectName(QString::fromUtf8("CameraPitchSlider"));
        CameraPitchSlider->setEnabled(true);
        CameraPitchSlider->setGeometry(QRect(80, 80, 18, 160));
        CameraPitchSlider->setMaximum(359);
        CameraPitchSlider->setOrientation(Qt::Vertical);
        CameraYawDial = new QDial(CameraSetting);
        CameraYawDial->setObjectName(QString::fromUtf8("CameraYawDial"));
        CameraYawDial->setGeometry(QRect(240, 100, 101, 151));
        CameraYawDial->setMaximum(359);
        CameraFovSlider = new QSlider(CameraSetting);
        CameraFovSlider->setObjectName(QString::fromUtf8("CameraFovSlider"));
        CameraFovSlider->setGeometry(QRect(100, 350, 160, 18));
        CameraFovSlider->setMaximum(359);
        CameraFovSlider->setOrientation(Qt::Horizontal);
        FovLabel = new QLabel(CameraSetting);
        FovLabel->setObjectName(QString::fromUtf8("FovLabel"));
        FovLabel->setGeometry(QRect(50, 350, 31, 16));
        Setting->addTab(CameraSetting, QString());
        ModelSetting = new QWidget();
        ModelSetting->setObjectName(QString::fromUtf8("ModelSetting"));
        Setting->addTab(ModelSetting, QString());
        UI_MyRenderer->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(UI_MyRenderer);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setEnabled(true);
        menuBar->setGeometry(QRect(0, 0, 1296, 25));
        FileMenu = new QMenu(menuBar);
        FileMenu->setObjectName(QString::fromUtf8("FileMenu"));
        FileMenu->setEnabled(true);
        SettingMenu = new QMenu(menuBar);
        SettingMenu->setObjectName(QString::fromUtf8("SettingMenu"));
        HelpMenu = new QMenu(menuBar);
        HelpMenu->setObjectName(QString::fromUtf8("HelpMenu"));
        UI_MyRenderer->setMenuBar(menuBar);
        mainToolBar = new QToolBar(UI_MyRenderer);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        UI_MyRenderer->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(UI_MyRenderer);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        UI_MyRenderer->setStatusBar(statusBar);

        menuBar->addAction(FileMenu->menuAction());
        menuBar->addAction(SettingMenu->menuAction());
        menuBar->addAction(HelpMenu->menuAction());
        FileMenu->addAction(action_OpenFile);
        FileMenu->addAction(action_SaveImage);
        SettingMenu->addAction(action_MultiThread);
        SettingMenu->addAction(action_FaceCulling);
        HelpMenu->addAction(action_About);

        retranslateUi(UI_MyRenderer);

        Setting->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(UI_MyRenderer);
    } // setupUi

    void retranslateUi(QMainWindow *UI_MyRenderer)
    {
        UI_MyRenderer->setWindowTitle(QApplication::translate("UI_MyRenderer", "UI_MyRenderer", nullptr));
        action_OpenFile->setText(QApplication::translate("UI_MyRenderer", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
        action_SaveImage->setText(QApplication::translate("UI_MyRenderer", "\344\277\235\345\255\230\345\233\276\347\211\207", nullptr));
        action_MultiThread->setText(QApplication::translate("UI_MyRenderer", "\345\244\232\347\272\277\347\250\213", nullptr));
        action_FaceCulling->setText(QApplication::translate("UI_MyRenderer", "\345\271\263\351\235\242\350\243\201\345\210\207", nullptr));
        action_About->setText(QApplication::translate("UI_MyRenderer", "\345\205\263\344\272\216", nullptr));
        PitchLabel->setText(QApplication::translate("UI_MyRenderer", "pitch", nullptr));
        YawLabel->setText(QApplication::translate("UI_MyRenderer", "yaw", nullptr));
        SpecularColorButton->setText(QApplication::translate("UI_MyRenderer", "Specular Color", nullptr));
        DiffuseColorButton->setText(QApplication::translate("UI_MyRenderer", "Diffuse Color", nullptr));
        AmbientColorButton->setText(QApplication::translate("UI_MyRenderer", "Ambient Color", nullptr));
        SpecularCheckBox->setText(QApplication::translate("UI_MyRenderer", "Specular Color", nullptr));
        DiffuseCheckBox->setText(QApplication::translate("UI_MyRenderer", "Diffuse Color", nullptr));
        AmbientCheckBox->setText(QApplication::translate("UI_MyRenderer", "Ambient Color", nullptr));
        Setting->setTabText(Setting->indexOf(LightSetting), QApplication::translate("UI_MyRenderer", "\345\205\211\347\272\277", nullptr));
        PitchLabel_2->setText(QApplication::translate("UI_MyRenderer", "pitch", nullptr));
        YawLabel_2->setText(QApplication::translate("UI_MyRenderer", "yaw", nullptr));
        FovLabel->setText(QApplication::translate("UI_MyRenderer", "fov", nullptr));
        Setting->setTabText(Setting->indexOf(CameraSetting), QApplication::translate("UI_MyRenderer", "\347\233\270\346\234\272", nullptr));
        Setting->setTabText(Setting->indexOf(ModelSetting), QApplication::translate("UI_MyRenderer", "\346\250\241\345\236\213", nullptr));
        FileMenu->setTitle(QApplication::translate("UI_MyRenderer", "\346\226\207\344\273\266", nullptr));
        SettingMenu->setTitle(QApplication::translate("UI_MyRenderer", "\350\256\276\347\275\256", nullptr));
        HelpMenu->setTitle(QApplication::translate("UI_MyRenderer", "\345\270\256\345\212\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UI_MyRenderer: public Ui_UI_MyRenderer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UI_MYRENDERER_H
