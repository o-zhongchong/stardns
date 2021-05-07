--CREATE DATABASE
DROP DATABASE IF EXISTS `stardns`;
CREATE DATABASE IF NOT EXISTS `stardns` CHARACTER SET utf8 COLLATE utf8_general_ci;

--CREATE TABLE
DROP TABLE IF EXISTS `domains`;
CREATE TABLE IF NOT EXISTS `domains`
(
`domain_id` INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
`domain_name` NVARCHAR(255) NOT NULL,
`create_owner` NVARCHAR(20) NOT NULL,
`create_time` DATETIME NOT NULL DEFAULT NOW(),
`update_time` DATETIME NOT NULL
);

DROP TABLE IF EXISTS `res_record_a`;
CREATE TABLE IF NOT EXISTS `res_record_a`
(
`resource_id` INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
`domain_id` INT UNSIGNED NOT NULL,
`record_name` NVARCHAR(255) NOT NULL,
`ttl` INT UNSIGNED NOT NULL,
`ip_addr` INT UNSIGNED NOT NULL,
`create_time` DATETIME NOT NULL DEFAULT NOW(),
`update_time` DATETIME NOT NULL
);

--Create Account
drop user 'stardns'@'127.0.0.1';
create user 'stardns'@'127.0.0.1' identified by '';
grant all privileges on stardns.* to 'stardns'@'127.0.0.1';
flush privileges;

--Test Record
insert into domains(domain_name,create_owner,update_time) 
values('test.com','admin',NOW());

insert into res_record_a(domain_id,record_name,ttl,ip_addr,update_time) 
values(1,'www.test.com',600,3232235777,NOW());

