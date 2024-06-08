#pragma once

#include <QMenu>
#include <qmap.h>
//自定义系统托盘菜单类
class CustomMenu  : public QMenu
{
	Q_OBJECT

public:
	CustomMenu(QWidget *parent = nullptr);
	~CustomMenu();
public:
	void addCustomMenu(const QString& text, const QString& icon, const QString& name);
	QAction* getAction(const QString& text);
private:
	QMap<QString, QAction*> m_menuActionMap;
};
