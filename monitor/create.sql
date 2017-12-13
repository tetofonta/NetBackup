CREATE DATABASE IF NOT EXISTS nbmonitor CHARACTER SET 'utf8';

create table users
(
  id int auto_increment
    primary key,
  username varchar(30) not null,
  password varchar(128) null,
  connection_key varchar(128) null,
  lastlogin mediumtext null,
  constraint users_id_uindex
  unique (id)
)
;

CREATE TABLE errors
(
  `key` VARCHAR(100) PRIMARY KEY NOT NULL,
  message VARCHAR(300),
  stack TEXT
);
CREATE UNIQUE INDEX errors_key_uindex ON errors (`key`);
