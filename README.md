# chat_monitor


we did test with 

-  3 layers network (threads)

![](https://raw.githubusercontent.com/jero98772/chat_monitor/main/pictures/1.jpeg)

using middle with make run-middle , when we execute that it run a instance of server and client with theads because excute 1 and time and it get yield

-  "4" layers network (procces or program)

![](https://raw.githubusercontent.com/jero98772/chat_monitor/main/pictures/3.png)

create for 4 options for make and the main.cpp 


middle client is conected to the main server and client is conected for each middle server in a diferent port that the main server and other servers

1. server
2. middle-client
3. middle-server
4. client

- add to server a protocol for conect with other servers 

![](https://raw.githubusercontent.com/jero98772/chat_monitor/main/pictures/2.png)

the problem is why i can't run another instance of a server in other port


### Error given in both case

	Error al hacer bind del socket del servidor.
	make: *** No rule to make target '12348'.  Stop.


but when we test test/client.cpp and test/server.cpp  it can handle multiples clients and servers without problem