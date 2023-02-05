#include "mainwindow.h"
#include "ui_mainwindow.h"


#define YEAR_RX "\\d{4}"

void renderAlbums(Ui::MainWindow *ui) {
    QSqlQuery getAllAlbums("SELECT * FROM albums");
     ui->albumList->clear();
    while (getAllAlbums.next()) {
        const QString albumName = getAllAlbums.value("album_name").toString();
        const QString coverImg = getAllAlbums.value("cover_image").toString();

        QListWidgetItem *albumItem = new QListWidgetItem(QIcon(coverImg), QString(albumName));

        ui->albumList->addItem(albumItem);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->albumList->setViewMode(QListWidget::IconMode);
    ui->albumList->setIconSize(QSize(150, 150));
    ui->albumList->setResizeMode(QListWidget::Adjust);

    QRegularExpression yearRx(YEAR_RX);
    QRegularExpressionValidator *yearValidator = new QRegularExpressionValidator(yearRx, this);
    ui->teSearchYear->setValidator(yearValidator);
    ui->teYearOfCreation->setValidator(yearValidator);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QDir::current().absoluteFilePath("data/Album.db"));

    if (!db.open()) {
        QMessageBox::information(this, "Not connected", "Datbase is not connected.");
    }

    renderAlbums(ui);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_selectImageButton_clicked()
{
    QString imgPath = QFileDialog::getOpenFileName(this, tr("Select an image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)"));

    if (!imgPath.isEmpty()) {
        ui->coverImgLabel->setText(imgPath);
    }
}


void MainWindow::on_searchButton_clicked()
{
    const QString authorSearch = ui->teSearchAuthor->toPlainText();
    const QString genreSearch = ui->teSearchGenre->toPlainText();
    const QString yearSearch = ui->teSearchYear->text();

    QSqlQuery querySearch;
    QString searchString = "SELECT * FROM albums WHERE ";

    if (!authorSearch.isEmpty()) {
        searchString += "author_name = '" + authorSearch + "' AND ";
    }

    if (!genreSearch.isEmpty()) {
        searchString += "genre = '" + genreSearch + "' AND ";
    }

    if (!yearSearch.isEmpty()) {
        searchString += "creation_year = '" + yearSearch + "' AND ";
    }

    searchString.chop(5);

    if (!querySearch.exec(searchString)) {
        QMessageBox::information(this, "Query not executed", "Search query was not executed.");
    } else {
        ui->albumList->clear();
        while (querySearch.next()) {
            QString albumName = querySearch.value("album_name").toString();
            QString coverImg = querySearch.value("cover_image").toString();

            QListWidgetItem *albumItem = new QListWidgetItem(QIcon(coverImg), QString(albumName));

            ui->albumList->addItem(albumItem);
        }
    }
}


void MainWindow::on_albumLibPageButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_newAlbumPageButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_fromDetailToLibButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_goToDetailButton_clicked()
{
    QListWidgetItem* selectedItem = ui->albumList->currentItem();
    if (!selectedItem) return;

    QSqlQuery querySearch;
    QString searchString = "SELECT * FROM albums WHERE album_name = '" + ui->albumList->currentItem()->text() + "'";

    if (!querySearch.exec(searchString)) {
        QMessageBox::information(this, "Query not executed", "Search query was not executed.");
    } else {
        if (querySearch.next()) {
            ui->albumName->setText(querySearch.value("album_name").toString());
            ui->albumAuthor->setText(querySearch.value("author_name").toString());
            ui->albumYear->setText(querySearch.value("creation_year").toString());
            ui->albumGenre->setText(querySearch.value("genre").toString());
            QString songListString = querySearch.value("song_list").toString();
            QStringList songList = songListString.split(",");

            for (const QString& song : songList)
            {
                ui->albumSongList->addItem(song);
            }

            QPixmap image(querySearch.value("cover_image").toString());
            QPixmap scaledImage = image.scaled(200, 200, Qt::KeepAspectRatioByExpanding);

            ui->albumImage->setFixedSize(200, 200);
            ui->albumImage->setPixmap(scaledImage);

        }
    }

    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_deleteAlbumButton_clicked()
{
    QListWidgetItem* selectedItem = ui->albumList->currentItem();
    if (!selectedItem) return;

    int result = QMessageBox::warning(this, "Confirm Deletion", "Are you sure you want to delete the album: " + selectedItem->text() + " ?", QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
         QSqlQuery queryDeleteAlbum;
         if(!queryDeleteAlbum.exec("DELETE FROM albums WHERE album_name = '" + selectedItem->text() + "'")) {
             QMessageBox::information(this, "Query not executed", "The album deletion failed.");
         }        
         renderAlbums(ui);
    } else return;
}


void MainWindow::on_resetSearchButton_clicked()
{
    ui->teSearchAuthor->clear();
    ui->teSearchGenre->clear();
    ui->teSearchYear->clear();

    renderAlbums(ui);
}


void MainWindow::on_addAlbumButton_clicked()
{
    const QString albumId = QUuid::createUuid().toString();
    const QString albumName = ui->teAlbumName->toPlainText();
    const QString authorName = ui->teAuthorName->toPlainText();
    const QString yearOfCreation = ui->teYearOfCreation->text();
    const QString genre = ui->teGenre->toPlainText();
    const QString listOfSongs = ui->teListOfSongs->toPlainText();
    QString coverImg = ui->coverImgLabel->text();

    if (albumName.isEmpty() || authorName.isEmpty() || yearOfCreation.isEmpty() || genre.isEmpty() || listOfSongs.isEmpty()) return;

    if (coverImg.isEmpty()) {
        coverImg = QDir::current().absoluteFilePath("data/images/img-cover.jpg");
    }

    QSqlQuery queryAddAlbum;
    queryAddAlbum.prepare("INSERT INTO albums (album_id, author_name, album_name, creation_year, genre, song_list, cover_image) "
                          "VALUES (:albumId, :authorName, :albumName, :yearOfCreation, :genre, :listOfSongs, :coverImg)");


    queryAddAlbum.bindValue(":albumId", albumId);
    queryAddAlbum.bindValue(":authorName", authorName);
    queryAddAlbum.bindValue(":albumName", albumName);
    queryAddAlbum.bindValue(":yearOfCreation", yearOfCreation);
    queryAddAlbum.bindValue(":genre", genre);
    queryAddAlbum.bindValue(":listOfSongs", listOfSongs);
    queryAddAlbum.bindValue(":coverImg", coverImg);

    if (!queryAddAlbum.exec()) {
        QMessageBox::information(this, "Query not executed", "Insertion query was not executed.");
    } else {
        QMessageBox::information(this, "Album added", "The album has been added to the collection.");

        ui->teAlbumName->clear();
        ui->teAuthorName->clear();
        ui->teYearOfCreation->clear();
        ui->teGenre->clear();
        ui->teListOfSongs->clear();
        ui->coverImgLabel->clear();
    }
}

