dnsforwarder
============

### 一个简单的 DNS 转发代理

**主要功能：**

1. 指定不同的域名使用不同的服务器（支持非标准端口）、不同的协议（UDP、TCP）进行查询；
2. DNS 缓存及相关的控制（支持自定义 TTL）；
3. 屏蔽包含指定的 IP 的 DNS 数据包；
4. Hosts 功能（支持通配符、CName 指定、网络 Hosts）；
5. 屏蔽指定的域名查询请求（广告屏蔽？）；
6. 跨平台（Windows、Linux）；

### 安装和部署
 参考 `default.config`。

### 配置逻辑
**host 匹配顺序：**

1. `DisabledList`, 行数据 < 512 字节。
2. Hosts {`AppendHosts`（静态） -> hosts 文件（动态）}
    <br>
    类型：支持 IPv4、IPv6、`@@`（跳过）、\<name> (`GoodIPList`) 和 CName
3. DNS 缓存。
4. (ServerGroup = `GroupFile` + `UDPGroup` + `TCPGroup` + `TLSGroup`) {
       <br>
        行数据 < 384 字节；
       <br>
        全被转为小写；
       <br>
        `#`和`;`后面为注释；
       <br>
        相同 domain 时后面的规则生效（**L**ast **I**n **F**irst **M**atch）；
       <br>
        `GroupFile` 路径语法： Win: `ExpandEnvironmentStrings`, *nix: `wordexp`。
       <br>
    }
5. 无匹配时选择一组上级 DNS 转发查询。

**DisabledList, GroupFile 的 domain 匹配**：
1. 无通配符: 全部 || 每一个`.`后面的部分；
2. 有通配符: Win: `PathMatchSpec(,)`; *nix: `fnmatch(,,)`。

### Log 类型缩写

| Alias | Description   |
| ----- | ------------- |
| B     | Bad           |
| C     | Cache         |
| H     | Hosts records |
| INFO  | Information   |
| R     | Rejected      |
| S     | Tls           |
| T     | TCP           |
| U     | UDP           |

---

### A simple DNS forwarder

**Main Fetures:**

1. Forwarding queries to customized domains (and their subdomains) to specified servers over a specified protocol (UDP or TCP). non-standard ports are supported;
2. DNS cache and its controls (including modifying TTL for different domains);
3. Ignoring DNS responses from upstream servers containing particular IPs;
4. Loading hosts from file (including the support for wildcards, CName redirections and remote hosts files);
5. Refusing queries to specified domains (for ads blocking?);
6. Cross-platform (Windows, Linux);

### Installation & Deployment
Read `default.en.config`.

### Configuring Logic
**host matching order：**

1. `DisabledList`, line length < 512 chars
2. Hosts {`AppendHosts` (StaticHosts) -> hosts-files (DynamicHosts)}
   <br>
   Types：supports IPv4, IPv6, `@@` (skipping), \<name> (`GoodIPList`) and CName
3. DNS Cache
4. (ServerGroup = `GroupFile` + `UDPGroup` + `TCPGroup` + `TLSGroup`) {
       <br>
       line length < 384 chars;
       <br>
       converted to lower case before testing;
       <br>
       `#` or `;` leads comments;
       <br>
       for the same domain rules, the last wins (**L**ast **I**n **F**irst **M**atch)；
       <br>
       `GroupFile` path syntax:  Win: `ExpandEnvironmentStrings`, *nix: `wordexp`.
       <br>
   }
5. If none matches, forward it to 1 group of upstream DNS.

**Domain Matching for DisabledList, GroupFile:**
1. No Wild Card: Full || Right part of each `.`;
2. Wild Card: Win: `PathMatchSpec(,)`; *nix: `fnmatch(,,)`。

### Log Flags

| Alias | Description   |
| ----- | ------------- |
| B     | Bad           |
| C     | Cache         |
| H     | Hosts records |
| INFO  | Information   |
| R     | Rejected      |
| S     | Tls           |
| T     | TCP           |
| U     | UDP           |

---

### License :
GPL v3

### Dependencies :

  For Linux:

    pthread;
    libcurl (optional);

  For Windows:

    None.

### Macros needed to be declared while compiling :

  For Linux:

    None.

  For Windows x86 (at least Windows XP)

    _WIN32

  For Windows x86-64 (at least Windows Vista):

    _WIN32
    _WIN64

### Build

  [CI.yml](https://github.com/lifenjoiner/dnsforwarder/blob/mydev/.github/workflows/CI.yml)
