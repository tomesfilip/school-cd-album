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
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_newAlbumPageButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_fromDetailToLibButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_goToDetailButton_clicked()
{
    QListWidgetItem* selectedItem = ui->albumList->currentItem();
    if (!selectedItem) return;

    QSqlQuery querySearch;
    QString searchString = "SELECT * FROM albums WHERE album_name = '" + ui->albumList->currentItem()->text() + "'";

    if (!querySearch.exec(searchString)) {
        QMessageBox::information(this, "Query not executed", "Search query was not executed.");
        return;
    } else {
        if (querySearch.next()) {
            ui->albumName->setText(querySearch.value("album_name").toString());
            ui->albumAuthor->setText(querySearch.value("author_name").toString());
            ui->albumYear->setText(querySearch.value("creation_year").toString());
            ui->albumGenre->setText(querySearch.value("genre").toString());
            QString songListString = querySearch.value("song_list").toString();
            QStringList songList = songListString.split(",");

            ui->albumSongList->clear();
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

    ui->stackedWidget->setCurrentIndex(3);
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

    QSqlQuery querySearch;
    QString searchString = "SELECT * FROM albums WHERE album_name = '" + albumName + "'";

    if (!querySearch.exec(searchString)) {
        QMessageBox::information(this, "Query not executed", "Search query was not executed.");
        return;
    } else {
        if (querySearch.next()) {
            QMessageBox::information(this, "Error", "You can't add the album with the same name.");
            return;
        }
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
        QMessageBox::information(this, "Query not executed", "Insertion query was not executed. Error: " + queryAddAlbum.lastError().text());
        return;
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


void MainWindow::on_editAlbumButton_clicked()
{
    QListWidgetItem* selectedItem = ui->albumList->currentItem();
    if (!selectedItem) return;

    QSqlQuery querySearch;
    QString searchString = "SELECT * FROM albums WHERE album_name = '" + ui->albumList->currentItem()->text() + "'";

    if (!querySearch.exec(searchString)) {
        QMessageBox::information(this, "Query not executed", "Search query was not executed.");
        return;
    } else {
        if (querySearch.next()) {
            ui->teAlbumNameEdit->setPlainText(querySearch.value("album_name").toString());
            ui->teAuthorNameEdit->setPlainText(querySearch.value("author_name").toString());
            ui->teYearOfCreationEdit->setText(querySearch.value("creation_year").toString());
            ui->teGenreEdit->setPlainText(querySearch.value("genre").toString());

            QString songListString = querySearch.value("song_list").toString();
            QStringList songList = songListString.split(",");
            ui->teListOfSongsEdit->setPlainText(songListString);
            ui->albumSongList->clear();

            for (const QString& song : songList)
            {
                ui->albumSongList->addItem(song);
            }

            QString imageSrc = querySearch.value("cover_image").toString();
            ui->coverImgLabelEdit->setText(imageSrc);

            QPixmap image(imageSrc);
            QPixmap scaledImage = image.scaled(200, 200, Qt::KeepAspectRatioByExpanding);

            ui->albumImageEdit->setFixedSize(200, 200);
            ui->albumImageEdit->setPixmap(scaledImage);

        }
    }

    ui->stackedWidget->setCurrentIndex(3);
}


void MainWindow::on_addAlbumButtonEdit_clicked()
{
    const QString albumName = ui->teAlbumNameEdit->toPlainText();
    const QString authorName = ui->teAuthorNameEdit->toPlainText();
    const QString yearOfCreation = ui->teYearOfCreationEdit->text();
    const QString genre = ui->teGenreEdit->toPlainText();
    const QString listOfSongs = ui->teListOfSongsEdit->toPlainText();
    QString coverImg = ui->coverImgLabelEdit->text();

    if (albumName.isEmpty() || authorName.isEmpty() || yearOfCreation.isEmpty() || genre.isEmpty() || listOfSongs.isEmpty()) return;

    if (coverImg.isEmpty()) {
        coverImg = QDir::current().absoluteFilePath("data/images/img-cover.jpg");
    }

    QSqlQuery queryAddAlbum;
    queryAddAlbum.prepare("UPDATE albums SET author_name=:authorName, album_name=:albumName, creation_year=:yearOfCreation, genre=:genre, song_list=:listOfSongs, cover_image=:coverImg "
                          "WHERE album_name=:albumName");


    queryAddAlbum.bindValue(":authorName", authorName);
    queryAddAlbum.bindValue(":albumName", albumName);
    queryAddAlbum.bindValue(":yearOfCreation", yearOfCreation);
    queryAddAlbum.bindValue(":genre", genre);
    queryAddAlbum.bindValue(":listOfSongs", listOfSongs);
    queryAddAlbum.bindValue(":coverImg", coverImg);

    if (!queryAddAlbum.exec()) {
        QMessageBox::information(this, "Query not executed", "Update query was not executed.");
        return;
    } else {
        QMessageBox::information(this, "Album added", "The album has been added to the collection.");

        ui->teAlbumName->clear();
        ui->teAuthorName->clear();
        ui->teYearOfCreation->clear();
        ui->teGenre->clear();
        ui->teListOfSongs->clear();
        ui->coverImgLabel->clear();
    }

    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_albumLibPageButtonEdit_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_selectPathButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory( this, "Select folder...", QDir::currentPath());
    if(!dir.isEmpty()){
        ui->selectedPath->setText(dir);
    } else {
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dir + "/Album.db");

    if (!db.open()) {
        QMessageBox::information(this, "Not connected", "Datbase is not connected.");
    }

    QSqlQuery queryCreateTable;
    QString createTableString = "CREATE TABLE IF NOT EXISTS albums (album_id TEXT, album_name TEXT PRIMARY KEY, author_name TEXT, creation_year TEXT, genre TEXT, song_list TEXT, cover_image TEXT)";
    if (!queryCreateTable.exec(createTableString)) {
        QMessageBox::information(this, "Table not created", "Table could not be created. Error: " + queryCreateTable.lastError().text());
    }

    ui->selectedPath->clear();
    renderAlbums(ui);
    ui->stackedWidget->setCurrentIndex(1);
}
