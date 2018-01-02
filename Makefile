cli = echoclient.o
srv = echoserver.o
client:$(cli)
server:$(srv)
echoserver.o:protocol.h 
echoclient.o:protocol.h
client:
	g++ -o client $(cli)
server:
	g++ -o server $(srv)
.PHONY:clean
clean:
	-rm -f $(cli) $(srv) client server
	



