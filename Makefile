cli = echoclient.o
srv = echoserver.o queue.o
client:$(cli)
server:$(srv)
echoserver.o:protocol.h queue.h
echoclient.o:protocol.h
client:
	gcc -o client $(cli)
server:
	gcc -o server $(srv)
.PHONY:clean
clean:
	-rm -f $(cli) $(srv) client server
	



