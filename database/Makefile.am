
noinst_DATA = schema

all : mysql.sql postgres.sql

mysql.sql : tasker.dia
	tedia2sql -i $< -o mysql -t innodb

postgres.sql : tasker.dia
	tedia2sql -i $< -o postgres -t postgres
