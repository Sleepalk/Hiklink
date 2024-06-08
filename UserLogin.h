#pragma once

#include <QWidget>
#include "ui_UserLogin.h"
#include "basicwindow.h"
class UserLogin : public BasicWindow
{//登录窗口的基类为BasicWindow
	Q_OBJECT

public:
	UserLogin(QWidget *parent = nullptr);
	~UserLogin();
private slots:
	void onLoginBtnClicked();//登录窗口点击
private:
	void initControl();//初始化控件
	bool connectMySql();//查看连接数据库是否成功
	bool veryfyAccountCode(bool &isAccountLogin,QString &strAccount);//验证账号和密码
private:
	Ui::UserLoginClass ui;
};
