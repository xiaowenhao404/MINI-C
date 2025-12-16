# C编译器：
## 检查时间

***词法 + 语法：*** 11月30日-12月7日

***完整代码：*** 12月28日-1月4日
***
## Get Start:
Linux:

1. install flex &nbsp; bison &nbsp; nasm
```
sudo apt-get install flex bison nasm
```
2. run compiler
```
make file=<filename>    // filename doesnot include .c
```

分析结果在当前目录下

词法分析： `Lexical`

语法分析: `Grammatical`

中间代码：`InnerCode`

汇编代码：`assembly.asm`

可执行文件：`<file name>`

_____

### Available keywords and operations
*void* *main* *return*

*const* *static*

*if* *else* *for* *while* *do* *break* *continue* *default*

(&nbsp;&nbsp;&nbsp;)&nbsp;&nbsp;&nbsp;[&nbsp;&nbsp;&nbsp;]&nbsp;&nbsp;&nbsp;{&nbsp;&nbsp;&nbsp;}

<&nbsp;&nbsp;&nbsp;>&nbsp;&nbsp;&nbsp;<=&nbsp;&nbsp;&nbsp;>=&nbsp;&nbsp;&nbsp;&&&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;!=

+&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;&nbsp;*&nbsp;&nbsp;&nbsp;/&nbsp;&nbsp;&nbsp;%&nbsp;&nbsp;&nbsp;^ ~~(power op)~~

=&nbsp;&nbsp;&nbsp;+=&nbsp;&nbsp;&nbsp;-=&nbsp;&nbsp;&nbsp;*=&nbsp;&nbsp;&nbsp;/=&nbsp;&nbsp;&nbsp;%=&nbsp;&nbsp;&nbsp;++&nbsp;&nbsp;&nbsp;--

*int* *float* *double* *char*

*pointer* *typedef*

*input_int* *output_int*