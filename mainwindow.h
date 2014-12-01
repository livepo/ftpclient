#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

class QFtp;
class QFile;
class QUrlInfo;
class QTreeWidgetItem;


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QFtp *ftp;
    QHash<QString, bool> isDirectory;//用来存储一个路径是否为目录的信息
    QString currentPath;//用来存储现在的路径
    QFile *file;//用来表示下载的文件

private slots:
    void ftpCommandStarted(int);
    void ftpCommandFinished(int, bool);
    void updateDataTransferProgress(qint64, qint64);
    void addToList(const QUrlInfo &urlInfo);
    void processItem(QTreeWidgetItem*, int);
    void on_connectButton_clicked();
    void on_cdToParentButton_clicked();
    void on_downloadButton_clicked();
};

#endif // MAINWINDOW_H
