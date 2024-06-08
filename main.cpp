#include "CCMainWindow.h"
#include <QtWidgets/QApplication>
#include "UserLogin.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);//窗口关闭之后不退出
    UserLogin *userlogin = new UserLogin;
    userlogin->show();
    return a.exec();
}
