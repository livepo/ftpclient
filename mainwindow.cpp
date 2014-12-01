#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFtp>
#include <QFile>
#include <QTreeWidgetItem>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ftp = new QFtp(this);
//    ftp->connectToHost("ftp.qt.nokia.com");
//    ftp->login();
//    ftp->cd("qt");
//    ftp->get("INSTALL");
//    ftp->close();
//    connect(ftp, SIGNAL(commandStarted(int)), this, SLOT(ftpCommandStarted(int)));
//    connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(ftpCommandFinished(int,bool)));

    ui->progressBar->setValue(0);
    connect(ui->fileList, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(processItem(QTreeWidgetItem*,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ftpCommandStarted(int)
{
    int id = ftp->currentCommand();
    switch(id)
    {
    case QFtp::ConnectToHost:
        ui->label->setText(tr("正在连接到服务器..."));
        break;
    case QFtp::Login:
        ui->label->setText(tr("正在登录..."));
        break;
    case QFtp::Close:
        ui->label->setText(tr("正在关闭连接..."));

    }
}

void MainWindow::ftpCommandFinished(int, bool error)
{
    if (ftp->currentCommand()== QFtp::ConnectToHost){
        if (error)
            ui->label->setText(tr("连接服务器出现错误：%1").arg(ftp->errorString()));
        else
            ui->label->setText(tr("连接到服务器成功"));
    }else if (ftp->currentCommand()== QFtp::Login){
        if (error)
            ui->label->setText(tr("登录出现错误：%1").arg(ftp->errorString()));
        else {
            ui->label->setText(tr("登陆成功"));
            ftp->list();
        }    
    }else if (ftp->currentCommand()==QFtp::Get){
        if (error)
            ui->label->setText(tr("下载出现错误：%1").arg(ftp->errorString()));
        else {
            ui->label->setText(tr("已经完成下载"));
            file->close();
        }

        ui->downloadButton->setEnabled(true);
    }else if (ftp->currentCommand() == QFtp::List){
        if (isDirectory.isEmpty())
        {
            ui->fileList->addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("<empty>")));
            ui->fileList->setEnabled(false);
            ui->label->setText(tr("该目录为空"));
        }
    }else if (ftp->currentCommand() == QFtp::Close){
        ui->label->setText(tr("已经关闭连接"));
    }
}


void MainWindow::on_connectButton_clicked()
{
    ui->fileList->clear();
    currentPath.clear();
    isDirectory.clear();
    ftp = new QFtp(this);
    connect(ftp, SIGNAL(commandStarted(int)), this, SLOT(ftpCommandStarted(int)));
    connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(ftpCommandFinished(int,bool)));
    connect(ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addToList(QUrlInfo)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SLOT(updateDataTransferProgress(qint64,qint64)));
    QString ftpServer = ui->ftpServerLineEdit->text();
    QString userName = ui->userNameLineEdit->text();
    QString passWord = ui->passWordLineEdit->text();
    ftp->connectToHost(ftpServer, 21);
    ftp->login(userName, passWord);
}

void MainWindow::addToList(const QUrlInfo &urlInfo)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, urlInfo.name());
    item->setText(1, QString::number(urlInfo.size()));
    item->setText(2, urlInfo.owner());
    item->setText(3, urlInfo.group());
    item->setText(4, urlInfo.lastModified().toString("MMM dd yyyy"));
    QPixmap pixmap(urlInfo.isDir()?"../myFTP/dir.png":"../myFTP/file.png");
    item->setIcon(0, pixmap);
    isDirectory[urlInfo.name()] = urlInfo.isDir();
    ui->fileList->addTopLevelItem(item);
    if (!ui->fileList->currentItem()){
        ui->fileList->setCurrentItem(ui->fileList->topLevelItem(0));
        ui->fileList->setEnabled(true);
    }
}


void MainWindow::processItem(QTreeWidgetItem *Item, int)
{
    QString name = Item->text(0);
    if (isDirectory.value(name)){
        ui->fileList->clear();
        isDirectory.clear();
        currentPath += "/";
        currentPath += name;
        ftp->cd(name);
        ftp->list();
        ui->cdToParentButton->setEnabled(true);
    }
}



void MainWindow::on_cdToParentButton_clicked()
{
    ui->fileList->clear();
    isDirectory.clear();
    currentPath = currentPath.left(currentPath.lastIndexOf("/"));
    if (currentPath.isEmpty()){
        ui->cdToParentButton->setEnabled(false);
        ftp->cd("/");
    }else {
        ftp->cd(currentPath);
    }
    ftp->list();
}

void MainWindow::on_downloadButton_clicked()
{
    QString fileName = ui->fileList->currentItem()->text(0);
    file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly)){
        delete file;
        return;
    }
    ui->downloadButton->setEnabled(false);
    ftp->get(ui->fileList->currentItem()->text(0), file);
}

void MainWindow::updateDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(readBytes);
}
