
all: server.key server.csr

server.key:
	openssl genrsa -des3 -out server.key 1024

server.csr: server.key
	openssl req -new -key server.key -out server.csr

cp server.key server.key.org
openssl rsa -in server.key.org -out server.key


