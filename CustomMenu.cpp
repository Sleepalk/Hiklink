#include "CustomMenu.h"
#include "CommonUtils.h"
//自定义系统托盘菜单类
CustomMenu::CustomMenu(QWidget *parent)
	: QMenu(parent)
{
	this->setAttribute(Qt::WA_TranslucentBackground);//背景透明
	CommonUtils::loadStyleSheet(this, "Menu");//加载样式
}

CustomMenu::~CustomMenu()
{
}

void CustomMenu::addCustomMenu(const QString & text, const QString & icon, const QString & name)
{//添加自定义菜单
	QAction* pAction = addAction(QIcon(icon), name);
	m_menuActionMap.insert(text, pAction);
}

QAction* CustomMenu::getAction(const QString& text)
{
	return m_menuActionMap[text];
}
