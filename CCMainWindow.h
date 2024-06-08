#pragma once

#include <QtWidgets/QWidget>
#include "ui_CCMainWindow.h"
#include "basicwindow.h"
#include <qtimer.h>
#include "CommonUtils.h"
class CCMainWindow : public BasicWindow
{
    Q_OBJECT

public:
    CCMainWindow(QString account,bool isAccountLogin,QWidget *parent = nullptr);
    ~CCMainWindow();
public:
    void initTimer();//定时器，进行qq等级升级
    void initControl();
    QString getHeadPicturePath();
    void setUsername(const QString& username);//设置用户名
    void setLevelPixmap(int level);//设置qq等级图片
    void setHeadPixmap(const QString& headPath);//设置头像
    void setStatusMenuIcon(const QString& statusPath);//设置在线状态图标

    //添加应用部件
    QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);

    

    void initContactTree();//初始化树，联系人
private:
    void resizeEvent(QResizeEvent* event);//重写尺寸事件
    void updateSeachStyle();//更新搜索样式
    bool eventFilter(QObject* obj, QEvent* event);
    void mousePressEvent(QMouseEvent* event);

    void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);
private slots:

    void onItemClicked(QTreeWidgetItem* item, int column);//项被点击对应的槽函数

    void onItemExpanded(QTreeWidgetItem* item);//项被展开对应的槽函数

    void onItemCollapsed(QTreeWidgetItem* item);//项被收缩对应的槽函数

    void onItemDoubleClicked(QTreeWidgetItem* item, int column);//项被双击对应的槽函数

    void onAppIconClicked();//图标点击时的槽函数
private:
    Ui::CCMainWindowClass ui;
    bool m_isAccountLogin;
    QString m_account;//账号或qq号
    //QMap<QTreeWidgetItem*, QString> m_groupMap;//所有分组的分组项
};
