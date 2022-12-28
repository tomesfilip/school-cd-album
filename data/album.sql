-- Active: 1668107191875@@localhost@3306

DROP TABLE IF EXISTS albums;

CREATE TABLE
    albums (
        album_id TEXT PRIMARY KEY,
        author_name TEXT NOT NULL,
        album_name TEXT NOT NULL,
        creation_year INTEGER NOT NULL,
        genre TEXT NOT NULL,
        song_list TEXT NOT NULL,
        cover_image TEXT NOT NULL
    );

INSERT INTO
    albums (
        album_id,
        author_name,
        album_name,
        creation_year,
        genre,
        song_list,
        cover_image
    )
VALUES (
        "1",
        'autor1',
        'album1',
        2005,
        'pop',
        'song1, song2',
        '/Users/tomes/Documents/school/mp/data/images/img-cover.jpg'
    );

INSERT INTO
    albums (
        album_id,
        author_name,
        album_name,
        creation_year,
        genre,
        song_list,
        cover_image
    )
VALUES (
        "2",
        'autor2',
        'album2',
        2006,
        'rock',
        'song1, song2, song3',
        '/Users/tomes/Documents/school/mp/data/images/img-cover.jpg'
    );

INSERT INTO
    albums (
        album_id,
        author_name,
        album_name,
        creation_year,
        genre,
        song_list,
        cover_image
    )
VALUES (
        "3",
        'autor3',
        'album3',
        2015,
        'metal',
        'song1, song2, song3, song4, song5',
        '/Users/tomes/Documents/school/mp/data/images/img-cover.jpg'
    );