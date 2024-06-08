#pragma once

#include <QSystemTrayIcon>
#include<qwidget.h>
//系统托盘类
class SysTray  : public QSystemTrayIcon
{
	Q_OBJECT

public:
	SysTray(QWidget *parent);
	~SysTray();
public slots:
	//系统托盘图标被点击时的操作，参数是被点击的原因
	void onIconActived(QSystemTrayIcon::ActivationReason reason);
	
private:
	void initSystemTray();//初始化系统托盘
	void addSysTrayMenu();//添加系统托盘菜单
private:
	QWidget* m_parent;
};
