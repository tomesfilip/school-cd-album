#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>

#include <QListWidgetItem>
#include <QIcon>
#include <QFileDialog>
#include <QPixmap>
#include <QDir>

#include <QUuid>

#include <iostream>
#include <string>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_selectImageButton_clicked();

    void on_searchButton_clicked();

    void on_albumLibPageButton_clicked();

    void on_newAlbumPageButton_clicked();

    void on_fromDetailToLibButton_clicked();

    void on_goToDetailButton_clicked();

    void on_deleteAlbumButton_clicked();

    void on_resetSearchButton_clicked();

    void on_addAlbumButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
