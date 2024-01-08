# Wireshark—分析HTTP协议



## 一、实验目的 

 1）利用 wireshark 软件分析 HTTP 及其下层协议（TCP 协议）；

 2）了解网络中数据封装的概念；

 3）掌握 HTTP 及 TCP 协议的工作过程。 



## 二、实验内容 

1）启动 wireshark 软件，进行报文截获； 

2）在浏览器访问 www.xjtu.edu.cn 页面（打开网页，浏览并关闭页面）；

 3）停止报文截获，将截获命名为“http—学号”； 

4）分析截获报文。



 ## 三、实验步骤

1）从截获的报文中选择 HTTP 请求报文（即 get 报文）和 HTTP 应答报文，并分析各字段的 值； 

2）综合分析截获的报文，概括 HTTP 协议的工作过程； 

3）从截获报文中选择 TCP 建立连接和释放连接的报文，分析各个字段的值并概括 TCP 协议 的工作过程。 



## 四、实验过程及结果

### 总览：

![image-20231023092320806](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023092320806.png)

**TCP握手：**

![image-20231023101559562](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023101559562.png)

**HTTP:**

![image-20231023101745303](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023101745303.png)

**TCP挥手：**

![image-20231023101713349](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023101713349.png)

**两遍三次握手出现的原因：**在使用Wireshark抓包时，当建立TCP连接时，会出现两遍三次握手的情况。这是因为Wireshark在抓包时，会在本地计算机和目标服务器之间建立一个虚拟的网络接口，称为“Loopback”接口，用于捕获本地计算机与目标服务器之间的通信。因此，当Wireshark抓包时，本地计算机和目标服务器之间的通信实际上是通过Loopback接口进行的，而不是通过物理网络接口。

在进行TCP连接时，会进行三次握手来确保连接的可靠性。在Wireshark抓包时，由于本地计算机和目标服务器之间的通信是通过Loopback接口进行的，因此会出现两遍三次握手的情况。一遍是本地计算机和Loopback接口之间的三次握手，另一遍是Loopback接口和目标服务器之间的三次握手。



**HTTP如何使用TCP：**

​        域名解析 → TCP 三次握手建立连接 → 客户端发起 http 请求 → 服务器响应 http 请 求并发送 Web 页面文件 → 浏览器解析 Web 页面文件并请求其中的资源（如 js、css、图 片等） → 浏览器对页面进行渲染呈现给用户 → TCP 四次挥手释放连接 

HTTP 协议如何使用 TCP 协议：当 HTTP 要传送一条报文时，会以流的形式将报文数据 的内容通过一条打开的 TCP 连接按序传输。TCP 收到数据流之后，会将数据流分成被称作 段的小数据块，并将段封装在 IP 分组中，通过因特网进行传输。



1. TCP连接建立：在HTTP协议中，客户端通过发送一个SYN包来请求建立TCP连接。服务器收到SYN包后，发送一个带有SYN和ACK标志的包作为响应。最后，客户端发送一个带有ACK标志的包来确认连接建立。
2. HTTP请求：一旦TCP连接建立，客户端发送一个HTTP请求给服务器。这个请求通常包含一个请求行、请求头和请求体。请求行包含请求方法（GET、POST等）、URL和HTTP协议版本。请求头包含一些元数据，如Host、User-Agent、Content-Type等。请求体包含一些附加的数据，如表单数据或上传的文件。
3. 服务器响应：服务器收到HTTP请求后，根据请求的内容和服务器的处理逻辑，生成一个HTTP响应。响应通常包含一个响应行、响应头和响应体。响应行包含HTTP协议版本、状态码和状态信息。
4. 数据传输：HTTP协议使用TCP协议来传输数据。TCP协议提供可靠的、面向连接的数据传输。它将HTTP请求和响应分割成多个小的数据包，并通过TCP连接逐个发送。TCP协议还提供流量控制和拥塞控制，以确保数据的可靠传输。
5. 连接关闭：一旦HTTP响应发送完毕，服务器关闭TCP连接。客户端收到响应后，也可以选择关闭TCP连接。如果客户端需要发送更多的HTTP请求，它可以继续使用现有的TCP连接，或者建立一个新的TCP连接。



### TCP三次握手

![image-20231023103105144](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103105144.png)

**第一次：**

![image-20231023103127756](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103127756.png)

Frame，Ethernet and IPv4：

![image-20231023103152563](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103152563.png)

表明在网络层的源地址是客户机的IP:10.172.104.189;目标地址是www.xjtu.edu.cn域名代表的IP：202.117.1.13；



TCP：

![image-20231023103230417](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103230417.png)

第一次握手，源端口是客户机端口52158，目标端口是http的80端口，表示客户机向www.xjtu.edu.cn服务器的（http）80端口发起请求。相对序列号relative sequence number为0.

标志位中只有SYN同步信号处被置为1，表示有效，表明这是一条申请建立TCP连接的请求。



**第二次：**

![image-20231023103257314](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103257314.png)

Frame，Ethernet and IPv4：

![image-20231023103335778](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103335778.png)

消息传递时网络层的源地址IP：202.117.1.13，表明是www.xjtu.edu.cn的主机地址，而目标地址IP：

10.172.104.189，表明客户机地址。



TCP:

![image-20231023103405518](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103405518.png)

第二次握手时，源端口是服务器主机的http80端口，而目标端口则是客户机的52158端口；

标志位处SYN和ACK置为1，表明这是一条服务器发回给客户机的确认包消息。

相对序列号relative sequence number仍是0，而相对确认号则为第一次握手中消息的相对序列号加1，所以是1。

这是服务器对客户机申请的应答请求。



**第三次：**

![image-20231023103435514](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103435514.png)

Frame，Ethernet and IPv4：

![image-20231023103455248](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103455248.png)

本条消息传输时的网络层源地址IP又是客户机：10.172.104.189；而目标地址为IP：202.117.1.13，表明是www.xjtu.edu.cn的主机地址。表明此消息是客户机向服务器发送的。



TCP：

![image-20231023103520813](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023103520813.png)

第三次握手时源端口又是客户机52158；而目标端口是服务器的80端口；

标志位只有ACK处置为1；表明这是客户机向服务器的确认消息。

相对确认号relative ACK number为第二次握手中的相对序列号加1，为1；



经历三次握手后，客户机和服务器的TCP连接正式建立。



### HTTP

**http请求：**

![image-20231023102028979](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023102028979.png)

Frame，Ethernet and IPv4：

![image-20231023102422273](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023102422273.png)

传输时的网络层源地址IP又是客户机：10.172.104.189；而目标地址为IP：202.117.1.13，表明是www.xjtu.edu.cn的主机地址。表明此消息是客户机向服务器发送的。

TCP：

![image-20231023102647804](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023102647804.png)

此时的TCP部分说明了源端口端口号52158，和目标端口号80，表明消息是客户端发出到服务器的请求；

此消息的长度为616，确认号的标志位置为1；

这表明客户机向服务器发送HTTP请求时，TCP协议向服务器发送了确认消息，确认服务器是否收到了客户机发送的http建立请求。

HTTP:

![image-20231023105543638](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023105543638.png)

**http请求报答的格式：**

```
GET(sapce)URL(space)HTTPversion\r\n
头部字段名：content\r\n
.
.
.
\r\n
```

URL：

![image-20231023110543337](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023110543337.png)

HTTP version：

![image-20231023110613924](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023110613924.png)

头部字段+\r\n：Host,Connection,User-Agent,Accept,Referer,Accept-Encoding,Accept-Language,Cookie

![image-20231023110713912](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023110713912.png)



**http应答：**

![image-20231023121317113](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023121317113.png)

Frame，Ethernet and IPv4：

![image-20231023121249976](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023121249976.png)

TCP：

![image-20231023121212775](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023121212775.png)

HTTP：

![image-20231023111020066](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023111020066.png)

**http应答报文格式：**

```
HTTPversion(space)状态码(space)状态信息\r\n
头部字段名：content\r\n
.
.
.
\r\n
```

状态行：

![image-20231023120756988](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023120756988.png)

200表示客户端请求成功；

头部段：

![image-20231023120932017](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023120932017.png)







### TCP四次挥手

![image-20231023121425057](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023121425057.png)

1. FIN/ACK包：TCP连接的一方发送一个FIN（finish）标志的TCP包，表示它已经完成了数据传输，并且希望关闭连接。
2. ACK包：接收到FIN包的一方发送一个ACK（acknowledge）标志的TCP包，表示它已经收到了FIN包，并且同意关闭连接。
3. FIN/ACK包：接收到ACK包的一方发送一个带有FIN/ACK标志的TCP包，表示它也同意关闭连接。
4. ACK包：发送FIN/ACK包的一方接收到ACK包后，确认对方已同意关闭连接，然后发送一个ACK标志的TCP包，表示连接已经关闭。

**第一次挥手：**

![image-20231023121744416](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023121744416.png)

Frame，Ethernet and IPv4：

![image-20231023121847830](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023121847830.png)



TCP：
![image-20231023121923419](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023121923419.png)

源端口号是服务器端的80；而目的端口号为客户机端口号52128；

标志位的ACK和FIN置为1，说明这是一条服务器端向客户机发送的断开连接的请求。

相对确认号为617；



**第二次挥手：**

![image-20231023122455802](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023122455802.png)

Frame，Ethernet and IPv4：

![image-20231023122625904](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023122625904.png)

TCP：

![image-20231023122648299](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023122648299.png)

源端口号为客户机的52128；而目的端口号为服务器端的80端口；

标志位的ACK置为1；

第一次挥手服务器向客户机发出断开连接的请求后，第二次挥手时客户机向服务器端发送确认消息，表明服务器到客户机的连接已经断开；



**第三次挥手：**

![image-20231023122517265](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023122517265.png)

Frame，Ethernet and IPv4：

![image-20231023122710813](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023122710813.png)

TCP：

![image-20231023122730214](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023122730214.png)

源端口号为客户机的52128端口；而目的端口号为服务器端的80端口；

标志位的ACK和FIN置为1；

说明第三次挥手即是客户机向服务器端发出了断开连接的请求；



**第四次挥手：**

![image-20231023122536341](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023122536341.png)

Frame，Ethernet and IPv4：

![image-20231023122749780](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023122749780.png)

TCP：

![image-20231023122809099](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023122809099.png)

源端口号是服务器端的80；而目的端口号为客户机端口号52128端口；

第三次挥手是客户机向服务器端发出了断开连接的请求；服务器端收到这个请求后第四次挥手即是服务器端向客户机发出了ACK确认消息，断开了客户机到服务器的连接；



**四次挥手之后，从服务器到客户机和从客户机到服务器端的两边连接都断开了。**





## 五、心得体会

​	通过本次实验，我掌握了使用 wireshark 进行抓包的方法，并通过抓包学习了 HTTP 请 求与响应报文的格式，以及 TCP 协议建立连接时的三次握手过程和释放连接时的四次挥手过程，HTTP如何使用TCP的过程。
