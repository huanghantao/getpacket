# 使用方法

## 编译

终端1：

```shell
cd getpacket
make
insmod getpacket_k.ko
gcc getpacket_u.c
./a.out
```

## 测试

终端2：

### 测试1

```shell
ping 127.0.0.1
```

将会在终端1显示到五元组信息：

```
protocol: ICMP
source ip: 127.0.0.1
dest ip: 127.0.0.1
source port: dest port: 
protocol: ICMP
source ip: 127.0.0.1
dest ip: 127.0.0.1
source port: dest port:
```

### 测试2

```shell
ping www.baidu.com
```

将会在终端1显示到五元组信息：

```shell
protocol: UDP
source ip: 127.0.0.1
dest ip: 127.0.1.1
source port: 53821
dest port: 53

protocol: UDP
source ip: ***.***.196.131
dest ip: ***.***.196.2
source port: 63313
dest port: 53

protocol: UDP
source ip: 127.0.1.1
dest ip: 127.0.0.1
source port: 53
dest port: 53821

protocol: ICMP
source ip: ***.***.196.131
dest ip: ***.***.210.27
source port: dest port: 
protocol: ICMP
source ip: ***.***.196.131
dest ip: ***.***.210.27
source port: dest port: 
protocol: ICMP
source ip: ***.***.196.131
dest ip: ***.***.210.27
source port: dest port:
```

### 测试3

```shell
wget www.baidu.com
```

将会在终端1显示到五元组信息：

```shell
protocol: UDP
source ip: 127.0.0.1
dest ip: 127.0.1.1
source port: 35833
dest port: 53

protocol: UDP
source ip: ***.***.196.131
dest ip: ***.***.196.2
source port: 34156
dest port: 53

protocol: UDP
source ip: 127.0.0.1
dest ip: 127.0.1.1
source port: 35833
dest port: 53

protocol: UDP
source ip: ***.***.196.131
dest ip: ***.***.196.2
source port: 19639
dest port: 53

protocol: UDP
source ip: 127.0.1.1
dest ip: 127.0.0.1
source port: 53
dest port: 35833

protocol: UDP
source ip: 127.0.1.1
dest ip: 127.0.0.1
source port: 53
dest port: 35833

protocol: TCP
source ip: ***.***.196.131
dest ip: ***.***.88.100
source port: 50268
dest port: 8080

protocol: TCP
source ip: ***.***.196.131
dest ip: ***.***.***.100
source port: 50268
dest port: 8080

protocol: TCP
source ip: ***.***.196.131
dest ip: ***.***.88.100
source port: 50268
dest port: 8080

protocol: TCP
source ip: ***.***.196.131
dest ip: ***.***.88.100
source port: 50268
dest port: 8080

protocol: TCP
source ip: ***.***.196.131
dest ip: ***.***.88.100
source port: 50268
dest port: 8080

protocol: TCP
source ip: ***.***.196.131
dest ip: ***.***.88.100
source port: 50268
dest port: 8080
```

