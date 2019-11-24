# Intel AVX-512 Brief Introduction
Intel AVX-512简介

<br />

## 15.1 概述

Intel AVX-512家族由一组指令集扩展集构成，包括了AVX-512基础、AVX-512指数与倒数指令、AVX-512冲突、AVX-512预取，以及额外的512位SIMD指令扩展。intel AVX-512指令是对AVX与AVX2的自然扩展。Intel AVX-512引入了下列架构上的提升：

• 支持512位宽度的向量与SIMD寄存器组。512位寄存器状态受操作系统管理，通过使用XSAVE/XRSTOR指令，这对指令在45nm的Intel 64处理器中就已引入。

• 在64位模式下，支持16个新增的512位SIMD寄存器（总共具有32个额SIMD寄存器，从ZMM0到ZMM31）。这新增的16个寄存器状态同样受操作系统使用XSAVE/XRSTOR/XSAVEOPT指令进行管理。

• 支持8个屏蔽操作（**opmask**）寄存器（从k0到k7），用于带条件的执行以及高效的目的操作数的合并。操作屏蔽寄存器的状态同样受操作系统使用XSAVE/XRSTOR/XSAVEOPT指令进行管理。

• 添加了一个新的编码前缀（称作为 **EVEX**）以支持将向量长度编码增加到512位。EVEX前缀基于VEX前缀作为基础进行构建，提供了紧凑的、高效的可用于VEX编码的功能性，外加下列增强的向量特性：

    • 操作屏蔽。
    • 内嵌的广播。
    • 内嵌的指令前缀舍入控制。
    • 压缩的地址位移
    
<br />

#### 15.1.1 512位宽度的SIMD寄存器支持

Intel AVX-512指令支持512位宽度的SIMD寄存器（ZMM0-ZMM31）。ZMM寄存器的低256位与相应的YMM寄存器对应，而低128位则与相应的XMM寄存器对应。

<br />

#### 15.1.2 32个SIMD寄存器支持

Intel AVX-512指令也支持64位模式下32个SIMD寄存器（XMM0-XMM31，YMM0-YMM31，以及ZMM0-ZMM31）。在32位模式下，可用的向量寄存器的个数依旧是8。

<br />

#### 15.1.3 八个屏蔽操作寄存器支持

AVX-512指令支持8个屏蔽操作寄存器（k0-k7）。每个屏蔽操作寄存器的宽度在架构上定义为MAX_KL的大小（最大64位）。八个屏蔽操作寄存器的其中七个（k0-k7）可以被用于跟EVEX编码的AVX-512基础指令相结合以提供带条件的执行以及目的操作数中数据元素的高效合并。屏蔽操作寄存器k0的编码一般是在所有数据元素都需要的情况下（即无条件处理）进行使用。此外，屏蔽操作寄存器也被用于向量标志/元素级的向量源以引入新奇的SIMD功能性，比如可以参考**VCOMPRESSPS**。

![avx512-register-scheme](https://github.com/zenny-chen/Intel-AVX512-Brief-Introduction/blob/master/avx512-register-scheme.png)

<br />

#### 15.1.4 指令语法增强

EVEX编码的架构以以下方式增强了向量指令编码模式：

• 512位向量长度，多达32个ZMM寄存器，使用增强的VEX（EVEX）支持向量编程环境。

EVEX前缀提供了比起VEX前缀更多的可编码的位域。除了在64位模式下对32个ZMM寄存器编码之外，使用EVEX前缀编码的指令可以直接对屏蔽操作寄存器操作数（一共8个）的其中7个进行编码，以提供向量指令编程的带条件处理。增强的向量编程环境可以显式地用包含以下元素的指令语法进行表达：

• 一个屏蔽操作操作数：屏蔽操作寄存器使用“k1”到“k7”的记号进行表达。一个EVEX编码的指令支持使用屏蔽操作寄存器 k1 的带条件的向量操作，这可以表示为在目的操作数之后使用记号 **{k1}** 来表示。对这条特征的使用对于大部分指令都是可选的。有两种类型的屏蔽方式（合并与清零），通过使用 **EVEX.z** 位进行区分（在指令签名中用 **{z}** 表示）。

• 内嵌的广播对于某些指令可以在源操作数上支持，该源操作数可以被编码为一个存储器向量。一个存储器向量的数据元素可以带条件的去获取或写入。

• 对于仅操作在SIMD寄存器中带有舍入语义的浮点数据上的指令语法，EVEX编码可以显式地在EVEX位域中提供舍入控制，或以标量长度，或以512位向量长度。

在512位指令中，对源操作数的所有元素的向量加法可以用与AVX指令相同的语法来表达：

```asm
VADDPS    zmm1,  zmm2,  zmm3
```

此外，AVX-512基础的EVEX编码模式可以将带条件的向量加法表示为：

```asm
VADDPS    zmm1  {k1} {z},  zmm2,  zmm3
```

这里：

• 对目的的带条件处理与更新用一个屏蔽操作寄存器来表达。

• 对目的元素所选择的屏蔽操作的清零行为用 **{z}** 修饰符来表达（而合并行为则是默认的，无需指定修饰符）。

注意，某些SIMD指令支持三操作数语法，但只处理512位SIMD指令集扩展中少于或等于128位的数据部分，因为目的寄存器的MAXVL-1:128被处理器清零。相同的规则也应用在256位数据的指令操作中，其目的寄存器的MAXVL-1:256位被清零。

<br />

## 15.5 访问XMM、YMM与ZMM寄存器

YMM寄存器的低128位与相应的XMM寄存器重叠。遗留的SSE指令（即，操作在XMM状态上但不使用VEX前缀的SIMD指令，也被称为非VEX编码的SIMD指令）将不访问YMM寄存器的高位（(MAXV - 1):128）。带有VEX前缀并且向量长度为128位的AVX与FMA指令会清零YMM寄存器的高128位。

YMM寄存器的高位（255:128）可以被许多带有VEX.256前缀的指令进行读写。    
XSAVE与XRSTOR可以被用于保存和恢复YMM寄存器的高位。    
一个ZMM寄存器的低256位与相应的YMM寄存器重叠。遗留的SSE指令（即，操作在XMM状态上但不使用VEX前缀的SIMD指令，也被称为非VEX编码的SIMD指令）不会访问ZMM寄存器的高位（(MAXVL - 1):128），这里MAXVL为最大向量长度（当前为512位）。带有一个VEX前缀且向量长度为128位的AVX与FMA指令会清零ZMM寄存器的高384位，而VEX前缀和256位的则清零ZMM寄存器的高256位。    
ZMM寄存器的高位（511:256）可以用带有EVEX.512前缀的指令进行读写。

<br />

## 15.6 使用EVEX编码的增强向量编程环境

EVEX编码的AVX-512指令支持增强的向量编程环境。增强的向量编程环境使用EVEX位域编码和一组八个屏蔽操作寄存器相结合的方式提供以下特性：

• 对一条EVEX编码的指令做有条件的向量处理。屏蔽操作寄存器k1到k7可被用来有条件地控制每个数据元素的计算操作，以及对一条AVX-512基础指令的目的有条件地做每个元素的更新。屏蔽操作寄存器的每个比特控制一个向量元素的操作（一个向量元素可以是8位、16位、32位或64位）。

• 除了通过EVEX位域编码来提供对向量指令的谓词（**predication**）控制之外，屏蔽操作寄存器也可以被类似地用在通用目的寄存器上，作为针对非屏蔽相关的指令使用modR/M编码的源/目的操作数。在这种情况下，k0到k7的屏蔽操作寄存器可以被选用。

• 在64位模式中，有32个向量寄存器可以使用EVEX前缀进行编码。

• 对某些指令，可针对能编码为存储器向量的操作数支持广播。一条存储器向量的数据元素可以有条件地获取或写入，而向量大小依赖于数据变换功能。

• 灵活的寄存器到寄存器的舍入控制给EVEX编码的512位与标量指令增添风味。四种舍入模式直接受EVEX前缀内的直接编码支持，重载MXCSR设置。

• 一个元素广播到目的向量寄存器的剩余部分。

• 压缩的8位位移编码模式对一般要求disp32语法的指令增加指令编码密度。

<br />

#### 15.6.1 屏蔽操作寄存器来谓词断言向量数据处理

使用EVEX的AVX-512指令编码一个谓词（**predicate**）操作数，以有条件地控制每个元素的计算操作以及对目的操作数的结果更新。谓词操作数则称为屏蔽操作寄存器。屏蔽操作是一组八个大小为MAX_KL（64位）的架构寄存器。注意，这八个架构寄存器之中，只有k1到k7可以被用来作为谓词操作数。k0可以被用作为一个普通的源或目的，但不能用于编码为一个谓词操作数。另外还要注意，一个谓词操作可以被用于允许存储器错误抑制，对于某些带有存储器操作数的指令（源或目的）。

作为一个谓词操作数，屏蔽操作寄存器包含一个比特来控制/更新一个向量寄存器的每个数据元素。通常来说，屏蔽操作寄存器可以支持所有元素大小的指令：字节（int8），字（int16），单精度浮点（float32），整型双字（int32），双精度浮点（float64），整型四字（int64）。因而，原则上一个ZMM寄存器可以持有8，16，32或64个元素。一个屏蔽操作寄存器的长度，即MAX_KL，足以处理多达64个元素，每个元素一比特，一共64位。对于大部分的AVX-512指令都支持屏蔽操作。对于一个给定的向量长度，每条指令只访问屏蔽位的***最低有效位部分***，基于其数据类型所需要的数量。比如，AVX-512基础指令对64位数据元素进行操作且具有512位的向量长度，那么就使用屏蔽操作寄存器的最低8位有效位。
