#pragma once

#include <QWidget>
#include "ui_SkinWindow.h"
#include "basicwindow.h"
#include "qpalette.h"
class SkinWindow : public BasicWindow
{
	Q_OBJECT

public:
	SkinWindow(QWidget *parent = nullptr);
	~SkinWindow();
public:
	void initControl();
public slots:
	void onShowClose();//重写BasicWindow窗口的关闭函数
private:
	Ui::SkinWindowClass ui;
};
