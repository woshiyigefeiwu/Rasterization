#include "UI_MyRenderer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UI_MyRenderer w;
    w.show();

    return a.exec();
}
