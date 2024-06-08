
#include "SysTray.h"
#include "CustomMenu.h"
SysTray::SysTray(QWidget *parent)
	: m_parent(parent)
	,QSystemTrayIcon(parent)
{
	initSystemTray();
	show();
}

SysTray::~SysTray()
{}

void SysTray::onIconActived(QSystemTrayIcon::ActivationReason reason) {
	// Trigger 就是点击触发
	if (reason == QSystemTrayIcon::Trigger) {
		m_parent->show();
		m_parent->activateWindow();
	}
	// 如果触发的是 Context 图标
	else if (reason == QSystemTrayIcon::Context) {
		addSysTrayMenu();
	}
}

void SysTray::initSystemTray()
{
	this->setToolTip(QStringLiteral("QQ-奇牛科技"));
	this->setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(onIconActived(QSystemTrayIcon::ActivationReason)));
}

void SysTray::addSysTrayMenu()
{
	CustomMenu* customMenu = new CustomMenu(m_parent);
	customMenu->addCustomMenu("onShow", ":/Resources/MainWindow/app/logo.ico",
		QStringLiteral("显示"));
	customMenu->addCustomMenu("onQuit", ":/Resources/MainWindow/app/page_close_btn_hover.png", 
		QStringLiteral("退出"));


	//显示，退出按钮被触发,Trigger时是被触发的意思
	connect(customMenu->getAction("onShow"), SIGNAL(triggered(bool)), m_parent, SLOT(onShowNormal(bool)));
	connect(customMenu->getAction("onQuit"), SIGNAL(triggered(bool)), m_parent, SLOT(onShowQuit(bool)));

	customMenu->exec(QCursor::pos());//菜单进入事件循环,让菜单跑起来
	delete customMenu;
	customMenu = nullptr;
}
