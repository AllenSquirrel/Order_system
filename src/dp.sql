create database if not exists order_sys;

use order_sys;

create table if not exists tb_dish(id int primary key auto_increment,name varchar(32) unique not null,price int not null,ctime datetime);

create table if not exists tb_order(id int primary key auto_increment,dishes varchar(255) comment '[1,2]',status int comment '0 未完成  1 已完成',mtime datetime);


insert tb_dish values(null,"红烧肉",2880,now()),(null,"鱼香茄子",1990,now());

insert tb_order values(null,"[1,2]",0,now()),(null,"[2]",1,now());
