#include "CCMainWindow.h"
#include <qproxystyle.h>
#include <qpainter.h>
#include "SkinWindow.h"
#include <qtimer.h>
#include "SysTray.h"
#include "RootContatitem.h"
#include "ContactItem.h"
#include "NotifyManager.h"
#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include "qapplication.h"
#include "WindowManager.h"
#include<qsqlquery.h>
QString gstrLoginHeadPath;
extern QString gLoginEmployeeID;//外部声明,UserLogin.cpp

CCMainWindow::CCMainWindow(QString account, bool isAccountLogin, QWidget *parent)
    : BasicWindow(parent)
    ,m_isAccountLogin(isAccountLogin)
    ,m_account(account)
{
    ui.setupUi(this);
    this->setWindowIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));
    //设置窗口风格
    this->setWindowFlags(windowFlags() | Qt::Tool);
    //加载样式表
    this->loadStyleSheet("CCMainWindow");
    setHeadPixmap(getHeadPicturePath());
    initControl();
    initTimer();
}

//定时器，进行qq等级升级
void CCMainWindow::initTimer()
{
    QTimer* timer = new QTimer(this);
    timer->setInterval(5000);
    connect(timer, &QTimer::timeout, [this] {
        static int level = 0;
        if (level == 99)
            level = 0;
        level++;
        this->setLevelPixmap(level);
        });
    timer->start();
}

//初始化树，联系人
void CCMainWindow::initContactTree()
{
    //展开与收缩时的信号,树项被点击，(点击项，点击的列)
    connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), 
        this, SLOT(onItemClicked(QTreeWidgetItem*, int)));//点击
    connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)),
        this, SLOT(onItemExpanded(QTreeWidgetItem*)));//展开
    connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
        this, SLOT(onItemCollapsed(QTreeWidgetItem*)));//收缩
    connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
        this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));//双击

    //根节点
    QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
    pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);//设置子孩子项永远显示
    pRootGroupItem->setData(0, Qt::UserRole, 0);//设置数据,第0列,自定义角色


    RootContatitem* pItemName = new RootContatitem(true, ui.treeWidget);

    //获取公司部门ID(公司群号)
    QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
        .arg(QString("公司群")));
    queryCompDepID.exec();//执行查询
    queryCompDepID.first();//指向结果集第一条
    int CompDepID = queryCompDepID.value(0).toInt();
    //获取qq登陆者所在的部门ID
    QSqlQuery querySelfDepID(QString("SELECT departmentID FROM tab_employees WHERE employeeID = %1")
        .arg(gLoginEmployeeID));
    querySelfDepID.exec();//执行查询
    querySelfDepID.first();//指向结果集第一条
    int SelfDepID = querySelfDepID.value(0).toInt();//自己所在部门ID

    //初始化公司群及登录者所在的群
    addCompanyDeps(pRootGroupItem, CompDepID);
    addCompanyDeps(pRootGroupItem, SelfDepID);
    QString strGroupName = QString("奇牛科技");
    pItemName->setText(strGroupName);

    //插入分组结点
    ui.treeWidget->addTopLevelItem(pRootGroupItem);//顶节点
    ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);

}

//添加公司部门
void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID)
{
    QTreeWidgetItem* pChlid = new QTreeWidgetItem;
    QPixmap pix;
    pix.load(":/Resources/MainWindow/head_mask.png");
    pChlid->setData(0, Qt::UserRole, 1);
    pChlid->setData(0, Qt::UserRole + 1, DepID);
    //获取公司，部门头像
    QPixmap groupPix;
    QSqlQuery queryPicture(QString("SELECT picture FROM tab_department WHERE departmentID = %1")
        .arg(DepID));
    queryPicture.exec();
    queryPicture.first();
    groupPix.load(queryPicture.value(0).toString());


    //获取部门名称
    QString strDepName;
    QSqlQuery queryDepName(QString("SELECT department_name FROM tab_department WHERE departmentID =%1")
        .arg(DepID));
    queryDepName.exec();
    queryDepName.first();
    strDepName = queryDepName.value(0).toString();

    ContactItem* pContactItem = new ContactItem(ui.treeWidget);
    pContactItem->setHeadPixmap(getRoundImage(groupPix,pix, pContactItem->getHeadLabelSize()));

    
    pContactItem->setUserName(strDepName);
    pRootGroupItem->addChild(pChlid);
    ui.treeWidget->setItemWidget(pChlid, 0, pContactItem);

    //m_groupMap.insert(pChlid, department_name);
}


//初始化控件
void CCMainWindow::initControl() {
    //树获取焦点时不绘制边框
    ui.treeWidget->setStyle(new CustomProxyStyle(this));
    setLevelPixmap(0);
    //setHeadPixmap(":/Resources/MainWindow/girl.png");
    setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

    QHBoxLayout* appupLayout = new QHBoxLayout;
    appupLayout->setContentsMargins(0, 0, 0, 0);
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png", "app_7"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
    appupLayout->addStretch();
    appupLayout->addSpacing(0);
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));

    ui.appWidget->setLayout(appupLayout);

    ui.bottomLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
    ui.bottomLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
    ui.bottomLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
    ui.bottomLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
    ui.bottomLayout->addStretch();

    initContactTree();//调用初始化树

    //个性签名
    ui.lineEdit->installEventFilter(this);//注册事件过滤器

    //好友搜索
    ui.searchLineEdit->installEventFilter(this);

    //窗口关闭和最小化的信号与槽连接
    connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));
    connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));
    // 更新搜索风格
    connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]()
        {
            updateSeachStyle();
        });
    //系统托盘
    SysTray* systray = new SysTray(this);
    //systray->setIcon()
}

QString CCMainWindow::getHeadPicturePath()
{//获取头像路径
    QString strPicturePath;
    if (!m_isAccountLogin)//如果是QQ号登录
    {
        QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees WHERE employeeID = %1")
            .arg(gLoginEmployeeID));
        queryPicture.exec();
        queryPicture.next();
        strPicturePath = queryPicture.value(0).toString();
    }
    else//账号登录
    {
        QSqlQuery queryEmployeeID(QString("SELECT employeeID FROM tab_accounts WHERE account = '%1'")
            .arg(m_account));
        queryEmployeeID.exec();
        queryEmployeeID.next();
        int employeeID = queryEmployeeID.value(0).toInt();
        QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees WHERE employeeID = %1")
            .arg(employeeID));
        queryPicture.exec();
        queryPicture.next();
        strPicturePath = queryPicture.value(0).toString();
    }
    gstrLoginHeadPath = strPicturePath;
    return strPicturePath;
}

void CCMainWindow::resizeEvent(QResizeEvent* event)
{
    this->setUsername(QString("奇牛科技-越分享越拥有"));
    BasicWindow::resizeEvent(event);
}

//设置名称
void CCMainWindow::setUsername(const QString& username)
{
    ui.nameLabel->adjustSize();//自动调整尺寸
    //文本过长则进行靠右省略...
    //fontMetrics()返回QFontMetrics类对象(字体体积)
    QString name = ui.nameLabel->fontMetrics().elidedText(username,Qt::ElideRight,ui.nameLabel->width());
    
    //ui.nameLabel->setToolTip(QString(username));
    ui.nameLabel->setText(name);
    
}

//设置图片等级
void CCMainWindow::setLevelPixmap(int level)
{
    QPixmap levelPixmap(ui.levelBtn->size());//创建一张大小为levelBtn的空图像
    levelPixmap.fill(Qt::transparent);//填充透明
    QPainter painter(&levelPixmap);
    painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));
    int unitNum = level % 10;//个位数
    int tenNum = level / 10;//十位数
    //十位,tenNum*6是因为levelvalue.png图片每个数字的间隙是6
    //drawPixmap(绘制点x，绘制点y，图片，图片左上角x，图片左上角y，拷贝的宽度，拷贝的高度)
    painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), tenNum * 6, 0, 6, 7);
    //个位
    painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), unitNum * 6, 0, 6, 7);

    ui.levelBtn->setIcon(levelPixmap);
    ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainWindow::setHeadPixmap(const QString& headPath)
{//设置头像
    QPixmap pix;
    pix.load(":/Resources/MainWindow/head_mask.png");
    ui.headLabel->setPixmap(getRoundImage(QPixmap(headPath),pix,ui.headLabel->size()));
}

//设置在线状态图标
void CCMainWindow::setStatusMenuIcon(const QString& statusPath) {
    QPixmap statusBtnPixmap(ui.stausBtn->size());
    statusBtnPixmap.fill(Qt::transparent);//设置填充透明

    QPainter painter(&statusBtnPixmap);
    painter.drawPixmap(4, 4, QPixmap(statusPath));

    ui.stausBtn->setIcon(statusBtnPixmap);
    ui.stausBtn->setIconSize(ui.stausBtn->size());
}

QWidget* CCMainWindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
    QPushButton* btn = new QPushButton(this);
    btn->setFixedSize(20, 20);

    QPixmap pixmap(btn->size());
    pixmap.fill(Qt::transparent);//设置透明

    QPainter painter(&pixmap);
    QPixmap appPixmap(appPath);
    painter.drawPixmap((btn->width() - appPixmap.width())/2, 
                                (btn->height() - appPixmap.height()) / 2, appPixmap );
    btn->setIcon(pixmap);
    btn->setIconSize(btn->size());
    btn->setObjectName(appName);
    btn->setProperty("hasborder", true);//设置属性

    //设置信号与槽
    connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);
    return btn;
}

void CCMainWindow::onAppIconClicked() {
    //如果信号发送者的对象名是app_skin
    if (sender()->objectName() == "app_skin") {
        SkinWindow* skinWindow = new SkinWindow;
        skinWindow->show();
    }
}

//项被点击对应的槽函数
void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column) {
    bool bIsChild = item->data(0, Qt::UserRole).toBool();
    if (!bIsChild) {
        item->setExpanded(!item->isExpanded());
    }
    
}

//项被展开对应的槽函数
void CCMainWindow::onItemExpanded(QTreeWidgetItem* item) {
    bool bIsChild = item->data(0, Qt::UserRole).toBool();
    if (!bIsChild) {
        // 先把 QWidget指针，转换成 RootContactItem类型的指针
        // dynamic_cast 将基类对象指针（或引用）转换到继承类指针
        RootContatitem* prootItem = dynamic_cast<RootContatitem*>(ui.treeWidget->itemWidget(item, 0));
        // 判断一下，进行角度转换
        if (prootItem)
        {
            prootItem->setExpanded(true);
        }
    }
}


//项被收缩对应的槽函数
void CCMainWindow::onItemCollapsed(QTreeWidgetItem* item) {
    bool bIsChild = item->data(0, Qt::UserRole).toBool();
    if (!bIsChild) {
        // 先把 QWidget指针，转换成 RootContactItem类型的指针
        // dynamic_cast 将基类对象指针（或引用）转换到继承类指针
        RootContatitem* prootItem = dynamic_cast<RootContatitem*>(ui.treeWidget->itemWidget(item, 0));
        // 判断一下，进行角度转换
        if (prootItem)
        {
            prootItem->setExpanded(false);
        }
    }
}

//项被双击对应的槽函数
void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();
    if (bIsChild) {
        WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());

        /*QString strGroup = m_groupMap.value(item);
        if (strGroup == QString("公司群")) {
            QString itemData = item->data(0, Qt::UserRole + 1).toString();
            WindowManager::getInstance()->addNewTalkWindow(itemData,COMPANY);
        }
        else if (strGroup == QString("人事部")) {
            QString itemData = item->data(0, Qt::UserRole + 1).toString();
            WindowManager::getInstance()->addNewTalkWindow(itemData,PERSONELGROUP);
        }
        else if (strGroup == QString("市场部")) {
            QString itemData = item->data(0, Qt::UserRole + 1).toString();
            WindowManager::getInstance()->addNewTalkWindow(itemData,MARKETGROUP);
        }
        else if (strGroup == QString("研发部")) {
            QString itemData = item->data(0, Qt::UserRole + 1).toString();
            WindowManager::getInstance()->addNewTalkWindow(itemData, DEVELOPMENTGROUP);
        }*/
    }
}

bool CCMainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (ui.searchLineEdit == obj) {
        //键盘焦点事件
        if (event->type() == QEvent::FocusIn) {
            // 给 搜索栏部件，设置样式
            // 背景为白色
            // hover 鼠标悬浮，pressed 按下效果
            ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255);border-bottom:1px solid rgba(%1,%2,%3,100)} \
																				QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)} \
																				QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)} \
																				QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}")
                .arg(m_colorBackGround.red())
                .arg(m_colorBackGround.green())
                .arg(m_colorBackGround.blue()));
        }
        else if (event->type() == QEvent::FocusOut)
        {
            // 鼠标移开，焦点不在搜索框时，要还原
            updateSeachStyle();
        }
    }
    return false;
}

void CCMainWindow::mousePressEvent(QMouseEvent* event)
{
    if (qApp->widgetAt(event->pos()) != ui.searchLineEdit && ui.searchLineEdit->hasFocus()) {
        ui.searchLineEdit->clearFocus();
    }
    else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus()) {
        ui.lineEdit->clearFocus();
    }
    BasicWindow::mousePressEvent(event);
}

void CCMainWindow::updateSeachStyle() {
    // 进行还原
    ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50);border-bottom:1px solid rgba(%1,%2,%3,30)}\
																		QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
        .arg(m_colorBackGround.red())
        .arg(m_colorBackGround.green())
        .arg(m_colorBackGround.blue()));
}

CCMainWindow::~CCMainWindow()
{
}
