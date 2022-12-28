# Semestral work - Multiplatform programming (Qt Creator widget app)

## About

The app is able to create a CD albums from user's input and store them in the sqlite database.
These albums are displayed in the list where the name and the image cover of the album is visible.
When the user selects the album and then clicks to the 'Go to detail of selected album' button, the detail view of that album is displayed.
The user is also able to select and delete any album from the main list view.

## How to create database with teplate data

1. open terminal and navigate to the _data_ folder
2. use command `sqlite3 Album.db --init album.sql`

## Typical problems (path related)

### Fallback cover image (/data/images/img-cover.jpg) isn't displayed

To fix for db initialization: adjust the absolute path in the album.sql

### Relative paths in the app

To fix: change the working directory in the Projects > Run > Working directory > it should end with _/CDAlbum_
