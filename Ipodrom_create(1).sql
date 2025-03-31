-- Created by Vertabelo (http://vertabelo.com)
-- Last modification date: 2025-03-31 07:36:05.77

-- tables
-- Table: Horses
CREATE TABLE Horses (
    id integer NOT NULL CONSTRAINT id PRIMARY KEY AUTOINCREMENT,
    name varchar(100) NOT NULL,
    age integer NOT NULL,
    experience integer NOT NULL,
    owner varchar(100) NOT NULL,
    price decimal(10,2) NOT NULL
);

-- Table: Jockeys
CREATE TABLE Jockeys (
    id integer NOT NULL CONSTRAINT Jockeys_pk PRIMARY KEY AUTOINCREMENT,
    last_name varchar(100) NOT NULL,
    experience integer NOT NULL,
    birth_year integer NOT NULL,
    address varchar(255) NOT NULL
);

-- Table: PrizeFund
CREATE TABLE PrizeFund (
    id integer NOT NULL CONSTRAINT PrizeFund_pk PRIMARY KEY,
    horse_id integer NOT NULL,
    amount double precision NOT NULL,
    Horses_id integer NOT NULL,
    CONSTRAINT PrizeFund_Horses FOREIGN KEY (Horses_id)
    REFERENCES Horses (id)
);

-- Table: Races
CREATE TABLE Races (
    id integer NOT NULL CONSTRAINT Races_pk PRIMARY KEY AUTOINCREMENT,
    date date NOT NULL,
    race_number integer NOT NULL,
    horse_id integer NOT NULL,
    jockey_id integer NOT NULL,
    place integer NOT NULL,
    jockeys_id integer NOT NULL,
    horses_id integer NOT NULL,
    CONSTRAINT Races_Jockeys FOREIGN KEY (jockeys_id)
    REFERENCES Jockeys (id),
    CONSTRAINT Races_Horses FOREIGN KEY (horses_id)
    REFERENCES Horses (id)
);

-- Table: Users
CREATE TABLE Users (
    id integer NOT NULL CONSTRAINT Users_pk PRIMARY KEY,
    username character(64) NOT NULL,
    password varchar(256) NOT NULL,
    role varchar(64) NOT NULL
);

-- End of file.

