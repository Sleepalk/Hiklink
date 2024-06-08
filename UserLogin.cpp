#include "UserLogin.h"
#include "CCMainWindow.h"
#include "basicwindow.h"
#include "SysTray.h"
#include<qmessagebox.h>
#include<qsql.h>
#include<qsqldatabase.h>
#include<qsqlquery.h>

QString gLoginEmployeeID;//登陆者qq号，全局变量，在其他类中会使用
UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	this->setWindowIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));
	this->setAttribute(Qt::WA_DeleteOnClose);//窗口关闭立即释放
	this->initTitleBar();//初始化标题栏,函数定义在BasicWindow中
	this->setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");//设置标题栏标题
	this->loadStyleSheet("UserLogin");//加载样式表
	this->initControl();//初始化控件

	//系统托盘
	SysTray* systray = new SysTray(this);
}

void UserLogin::initControl() {
	QLabel* headlabel = new QLabel(this);
	headlabel->setFixedSize(68, 68);
	QPixmap pix(":/Resources/MainWindow/head_mask.png");//先定义一个空的圆形头像
	headlabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pix, headlabel->size()));
	headlabel->move(width() / 2 - 34, ui.titleWiget->height() - 34);
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	//调用连接数据库
	if (!connectMySql()) {
		QMessageBox::information(NULL, QString("提示"), QString("数据库连接失败"));
		this->close();
	}
}

//连接数据库
bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//连接SQLite数据库
	db.setDatabaseName("D:/VS-QtItems/QtQQ/qtqq.db");//设置数据库文件路径，确保项目能连接到对应的数据库
	bool isOpened = db.open();
	if (isOpened) {
		return true;
	}
	else
	{
		return false;
	}
	
}

//验证账号
bool UserLogin::veryfyAccountCode(bool& isAccountLogin, QString& strAccount)
{
	QString strAccountInput = ui.editUserAccount->text();//获取登录窗口中账号的信息，与数据库比对
	QString strCodeInput = ui.editPassword->text();//获取登录窗口中密码的信息，与数据库比对

	//输入员工号(QQ号登录),10001,10002,10003.....
	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1")
									.arg(strAccountInput);//组装查询语句
	QSqlQuery queryEmployeeID(strSqlCode);
	queryEmployeeID.exec();//执行

	//qq号登录,10001,10002,10003.....
	if (queryEmployeeID.first()) {//指向结果集第一条
		QString strCode = queryEmployeeID.value(0).toString();//数据库中qq号的密码
		if (strCode == strCodeInput) {
			gLoginEmployeeID = strAccountInput;
			isAccountLogin = false;
			strAccount = strAccountInput;
			return true;
		}
		else
		{
			isAccountLogin = true;
			return false;
		}
	}
	//账号登录renyi,rener,yanyi.......
	strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = '%1'")
							.arg(strAccountInput);//数据库查询字符串时，要加上单引号''
	QSqlQuery queryAccount(strSqlCode);
	queryAccount.exec();//执行
	if (queryAccount.first()) {
		QString strCode = queryAccount.value(0).toString();
		if (strCode == strCodeInput) {
			gLoginEmployeeID = queryAccount.value(1).toString();//employeeID
			isAccountLogin = true;
			strAccount = strAccountInput;
			return true;
		}
		else
			return false;
	}
	return false;
}

void UserLogin::onLoginBtnClicked() {
	bool isAccountLogin;
	QString strAccount;//账号或qq号
	if (!veryfyAccountCode(isAccountLogin, strAccount)) {
		QMessageBox::warning(NULL, QString("提示"), QString("账号或密码输入错误！"));
		ui.editPassword->setText("");
		//ui.editUserAccount->setText("");
		return;//直接返回
	}

	//更新登录状态
	QString strSqlStatus = QString("UPDATE tab_employees SET online = 2 WHERE employeeID = %1")
		.arg(gLoginEmployeeID);
	QSqlQuery sqlStatus(strSqlStatus);
	sqlStatus.exec();
	this->close();
	CCMainWindow *w = new CCMainWindow(strAccount,isAccountLogin);
	w->show();
}

UserLogin::~UserLogin()
{}
