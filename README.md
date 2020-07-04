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

一个屏蔽操作寄存器以每个元素的粒度影响着一条AVX-512指令。对每个数据元素的任一数值或非数值操作，以及对目的操作数的中间结果的每个元素的更新基于屏蔽操作寄存器的相应比特做谓词断言。

担任一个AVX-512中谓词操作数的一个屏蔽操作遵循以下特性：

• 如果对应的屏蔽操作位不被置1，那么该指令的操作对相应的元素不被执行。这暗示着对一个屏蔽掉的元素的操作不会导致例外或违背。因此，作为屏蔽操作的结果，没有MXCSR异常标志会被更新。

• 如果对应的写屏蔽位不被置1，那么对应的目的元素不用操作结果进行更新。取而代之的是，目的元素值必须被保留（合并屏蔽），或是它必须被清零（清零屏蔽）。

• 对于带有存储器操作数的某些指令，如果某个数据元素所对应的屏蔽位为0，那么其相应的存储器错误会被抑制。

注意，此特征提供了实现控制流谓词作为有效屏蔽的多功能构造，提供了对AVX-512向量寄存器目的的合并行为。由于屏蔽可以选择用于清零而不是合并，以至于所屏蔽的元素可以用0去更新而不是保留它们旧有的值。提供清零行为可以移除隐式的对旧有值的依赖，当这些旧有值不被需要的时候。

大部分带有允许屏蔽的指令都接受这两种屏蔽方式。必须让EVEX.aaa位不等于0的指令（聚集或散播）以及只接受合并屏蔽的写入存储器的指令。

很重要的一点是，逐元素目的的更新规则也应用于目的操作数是存储器位置的场合。向量基于每个元素，附加用作为谓词操作数的屏蔽寄存器进行写入。    
一个屏蔽操作寄存器的值可以是：    
• 作为一条向量指令的结果所产生的值（比如，CMP、FPCLASS，等等）。    
• 从存储器加载的值。
• 从一个通用目的寄存器（GPR）加载的值。
• 用屏蔽到屏蔽的操作进行修改的值。

屏蔽操作寄存器可以被用于作为谓词断言以外的目的。比如，它们可以用于从一个向量操作稀疏集，或用于设置EFLAGS，基于0/0xFFFF_FFFF_FFFF_FFFF/两个屏蔽操作寄存器的按位或操作的其他状态。

<br />

##### 15.6.1.1 屏蔽操作寄存器K0

上述描述的屏蔽操作规则的唯一例外是屏蔽操作寄存器k0不能用作为谓词操作数。屏蔽操作k0不能针对一个向量操作被编码为一个谓词操作数；要选择屏蔽操作k0的编码值将取而代之地选择一个隐式的屏蔽操作值0xFFFFFFFF_FFFFFFFF，从而有效地禁用屏蔽。屏蔽操作寄存器k0可以仍然被用于取屏蔽操作寄存器作为操作数（可以是源也可以是目的）的任一指令。

注意，某些指令隐式地将屏蔽操作用作为额外目的操作数。在这些情况下，设法使用“无屏蔽”特征将转译为所引发的 **#UD** 错误。

<br />

##### 15.6.1.2 屏蔽操作寄存器的使用样例

以下例子描述了带有谓词断言的向量加法操作以及带有谓词断言的将相加后的结果更新到目的操作数。向量寄存器zmm0、zmm1、zmm2，以及k3的初始状态为：

---- MSB........................................LSB ----

zmm0 =    
\[ 0x00000003 0x00000002 0x00000001 0x00000000 \] (bytes 15 through 0)    
\[ 0x00000007 0x00000006 0x00000005 0x00000004 \] (bytes 31 through 16)    
\[ 0x0000000B 0x0000000A 0x00000009 0x00000008 \] (bytes 47 through 32)    
\[ 0x0000000F 0x0000000E 0x0000000D 0x0000000C \] (bytes 63 through 48)    

zmm1 =
\[ 0x0000000F 0x0000000F 0x0000000F 0x0000000F \] (bytes 15 through 0)    
\[ 0x0000000F 0x0000000F 0x0000000F 0x0000000F \] (bytes 31 through 16)    
\[ 0x0000000F 0x0000000F 0x0000000F 0x0000000F \] (bytes 47 through 32)    
\[ 0x0000000F 0x0000000F 0x0000000F 0x0000000F \] (bytes 63 through 48)

zmm2 =
\[ 0xAAAAAAAA 0xAAAAAAAA 0xAAAAAAAA 0xAAAAAAAA \] (bytes 15 through 0)    
\[ 0xBBBBBBBB 0xBBBBBBBB 0xBBBBBBBB 0xBBBBBBBB \] (bytes 31 through 16)    
\[ 0xCCCCCCCC 0xCCCCCCCC 0xCCCCCCCC 0xCCCCCCCC \] (bytes 47 through 32)    
\[ 0xDDDDDDDD 0xDDDDDDDD 0xDDDDDDDD 0xDDDDDDDD \] (bytes 63 through 48)

k3 = 0x8F03 (1000 1111 0000 0011)

一个担任谓词操作数的屏蔽操作寄存器表达为一个用花括号包围的修饰符，在Intel汇编语法中跟在第一个操作数的后面。给定上述状态，我们可以执行以下指令：

```asm
vpaddd zmm2 {k3}, zmm0, zmm1
```

`vpaddd`指令对每个数据元素（这里每个数据元素均为32位整数）基于谓词操作数k3中相应比特值，有条件地执行了32位整数加法。由于如果谓词屏蔽的相应比特不被置1，那么对应元素的操作则不会被执行，从而中间结果如下所示：

\[ \*\*\*\*\*\*\*\*\*\* \*\*\*\*\*\*\*\*\*\* 0x00000010 0x0000000F \] (bytes 15 through 0)    
\[ \*\*\*\*\*\*\*\*\*\* \*\*\*\*\*\*\*\*\*\* \*\*\*\*\*\*\*\*\*\* \*\*\*\*\*\*\*\*\*\* \] (bytes 31 through 16)    
\[ 0x0000001A 0x00000019 0x00000018 0x00000017 \] (bytes 47 through 32)    
\[ 0x0000001E \*\*\*\*\*\*\*\*\*\* \*\*\*\*\*\*\*\*\*\* \*\*\*\*\*\*\*\*\*\* ] (bytes 63 through 48)

这里的`**********`表示没有执行任何操作。

此中间结果然后被写入到目的向量寄存器zmm2，使用屏蔽操作寄存器k3作为写屏蔽，产生以下最终结果：

zmm2 =    
\[ 0xAAAAAAAA 0xAAAAAAAA 0x00000010 0x0000000F \] (bytes 15 through 0)    
\[ 0xBBBBBBBB 0xBBBBBBBB 0xBBBBBBBB 0xBBBBBBBB \] (bytes 31 through 16)    
\[ 0x0000001A 0x00000019 0x00000018 0x00000017 \] (bytes 47 through 32)     
\[ 0x0000001E 0xDDDDDDDD 0xDDDDDDDD 0xDDDDDDDD \] (bytes 63 through 48)

注意，对于一条数据元素宽度为64位的指令（比如`vaddpd`），屏蔽操作寄存器k3中只有8位LSB（最低有效位）才会被用于标识谓词操作，对应源/目的向量中8个元素的每一个。

<br />

#### 15.6.2 屏蔽操作指令

AVX-512基础指令提供了一组屏蔽操作指令以允许程序员对一个给定的屏蔽操作寄存器进行设置、拷贝或操作其内容。共有三种屏蔽操作指令：

• 屏蔽读/写指令：这些指令在一个通用目的寄存器或存储器与一个屏蔽操作寄存器之间，或是在两个屏蔽操作寄存器之间搬移数据。比如：

```asm
kmovw k1, ebx    ; 将EBX的低16位搬移到k1中。
```

• 标志指令：这个类别由基于屏蔽操作寄存器的内容来修改EFLAGS的指令构成。比如：

```asm
kortestw k1, k2    ; 对寄存器k1和k2进行按位或，然后根据操作结果更新EFLAGS。
```

• 屏蔽逻辑指令：这些指令在屏蔽操作寄存器之间执行标准的按位逻辑操作。比如：

```asm
kandw k1, k2, k3    ; 将k2和k3的低16位进行按位与，然后将结果送给k1。
```

<br />

#### 15.6.3 广播

EVEX编码提供了一个位域用于为某些加载操作指令编码数据广播，也就是从存储器加载数据并执行某些计算或数据搬移操作的指令。来自存储器的一个源元素可以跨有效源操作数的所有元素进行广播（重复）（对于一个32位数据元素高达16次，对于一个64位数据元素高达8次）。当我们想对一条向量指令的所有操作重新使用相同的标量操作数时这很有用。广播只能用于带有32位或64位元素大小的指令中。字节和字指令不支持嵌入的广播。数据广播的功能性以Intel汇编语法被表达为：一个由花括号包围的装饰符，前面跟着最后的寄存器/存储器操作数。    
比如：    

```asm
vmulps zmm1, zmm2, [rax] {1to16}
```

`{1to16}`原语从存储器加载了一个 **float**（单精度浮点）元素，然后将它复制16份以形成一个由16个32位单精度浮点元素所构成的向量，然后用第一个源操作数向量中的相应元素乘以这16个 **float** 元素，最后将这16个结果存放到目的操作数中。

具有存储语义的AVX-512指令以及纯粹的加载指令不支持广播原语。

```asm
vmovaps [rax], {k3}, zmm19
```

相比之下，上述例子中k3屏蔽操作寄存器用作为谓词操作数。只有对应于k3中非零比特的数据元素，其存储操作才会被执行。

<br />

#### 15.6.4 静态舍入模式以及抑制所有异常

在先前的SIMD指令扩展中（一直到AVX和AVX2），舍入控制通常在MXCSR中指定，还有少量指令提供逐指令的舍入通过在imm8操作数内的编码域进行覆盖（override）。AVX-512提供了一种更灵活的编码属性，针对具有舍入语义的浮点指令来覆盖基于MXCSR的舍入控制。这种嵌入在EVEX前缀中的舍入属性被称为**静态**（逐指令）**舍入模式**（***Static Round Mode***），或**舍入模式覆盖**（***Rounding Mode Override***）。此属性允许程序员静态应用一个特定的算术舍入模式，而不用考虑MXCSR中的RM位。它只对寄存器到寄存器风格的、具有舍入语义的EVEX编码浮点指令可用。这三种舍入控制接口之间的不同点在表15-4中概括。

表15-4：三种舍入控制接口的特征

  舍入接口 | 静态舍入覆盖 | imm8嵌入舍入覆盖 | MXCSR舍入控制
  ---- | ---- | ---- | ----
  语义要求 | FP舍入 | FP舍入 | FP舍入 
  前缀要求 | EVEX.B = 1 | N/A | N/A
  舍入控制 | EVEX.L'L | IMM8[1:0]或MXCSR.RC（依赖于IMM8[2]） | MXCSR.RC
  抑制所有异常（SAE） | 隐式的 | 不 | 不
  SIMD FP异常 #XF | 所有都抑制 | 可能引发#I，#P（除非SPE置1） | MXCSR屏蔽控制
  MXCSR标志更新 | 不 | 是（除了PE，如果SPE置1的话） | 是
  优先权 | 在MXCSR.RC之上 | 在EVEX.L'L之上 | 默认
  适用范围 | 512位，寄存器-寄存器，标量寄存器-寄存器 | ROUNDPx，ROUNDSx，VCVTPS2PH，VRNDSCALExx | 所有SIMD操作数，向量长度

AVX-512中的静态舍入覆盖也暗示了“抑制所有异常”（SAE）属性。SAE的效果就好比所有的MXCSR屏蔽位被置1了，并且没有MXCSR标志被更新。通过EVEX而不用SAE来使用静态舍入模式是不被支持的。

静态舍入模式与SAE控制可以在指令的编码中被允许，通过将寄存器到寄存器的向量指令中EVEX.b位置1。在这种情况下，向量长度被假定为MAXVL（AVX-512打包的向量指令情况下为512位）或对于标量指令则为128位。表15-5概括了AVX-512指令中可能的静态舍入模式分配。

注意，有些指令已经通过立即数位静态地指定了舍入模式。在这种情况下，立即数比特位要优先于嵌入的舍入模式（同样，嵌入的舍入模式要优先于MXCSR.RM所指示的）。

表15-5：静态舍入模式

  功能 | 描述
  ---- | ----
  {rn-sae} | 舍入到最近的（偶数）+ SAE
  {rd-sae} | 向下舍入（向 -inf 舍入）+ SAE
  {ru-sae} | 向上舍入（向 +inf 舍入）+ SAE
  {rz-sae} | 向零舍入（截断）+ SAE

这里，所谓的舍入到最近（向偶数舍入），即（Round to nearest, even）是IEEE754标准中的舍入模式。IEEE754不支持我们传统数学中的四舍五入模式，取而代之的则是使用最近舍入（取偶）法。这种舍入方法几乎跟传统数学上的四舍五入一样，除了对于x.5的舍入有所不同。如果某个浮点数的尾数是 .5 ，那么该浮点数要向其最近的偶数方向进行舍入。比如：Round(0.5) = 0; Round(1.5) = 2; Round(2.5) = 2; Round(−23.5) = −24; Round(−24.5) = -24。

为何IEEE754要采取Round-to-nearest-even这种舍入模式呢？由于对于像0.5，它到0和1的距离是一样近，偏向谁都不合适，四舍五入模式取1；而1.5则取2……如果在求和计算中使用四舍五入这么一直算下去，误差有可能越来越大。机会均等才公平，也就是向上和向下各占一半才合理，在大量计算中，从统计角度来看，高一位分别是偶数和奇数的概率正好是50%:50%。

以下列出其用法的一个样例：

```asm
vaddps zmm7 {k6}, zmm2, zmm4, {rd-sae}
```

上述指令将会执行向量zmm2和zmm4的单精度浮点向量加法，采用舍入到-inf的舍入模式，使用k6作为条件写屏蔽，将结果存放到zmm7。

注意，上述指令执行时MXCSR.RM位被忽略，并且不受该指令执行结果的影响。

对于指令实例，静态舍入模式不被允许的例子如下所示：

```asm
; 舍入模式已经在指令立即数中指定了
vrndscaleps zmm7 {k6}, zmm2, 0x00

; 带有存储器操作数的指令
vmulps zmm7 {k6}, zmm2, [rax], {rd-sae}

; 向量长度不同于MAXVL（512位）的指令
vaddps ymm7 {k6}, ymm2, ymm4, {rd-sae}
```

<br />

#### 15.6.5 压缩的Disp\*N编码

EVEX编码支持一种新的**位移**（***displacement***）表示，可允许一般用于展开（比如循环展开）代码的存储器寻址，这里一个8位的位移可以寻址超过一个8比特值动态范围的一个范围。这种压缩的位移编码被引用为 disp8\*N，这里N是一个常量，由每条指令的存储器操作特性所隐含。

压缩的位移基于（发生在一次循环内的存储器操作数的）有效位移是每次迭代的存储器访问的粒度的倍数。由于存储器寻址中的基寄存器已经提供了字节粒度的分辨率，因而传统的disp8操作数的低位变得冗余，并且可以从存储器操作特性上来暗示。

存储器操作特性依赖于以下两点：    
• 目的操作数被更新为一个完整的向量，或一单个元素，或多个元素的元组。    
• 存储器源操作数（或向量源操作数，如果目的操作数为存储器）被取（或被看）作为一个完整向量，或是一单个元素，或是多个元素的元组。

比如：    

```asm
vaddps zmm7, zmm2, disp8[membase + index*8]
```

目的操作数zmm7被更新为一个完整的512位向量，并且64字节的数据从存储器作为一个完整的向量被取出；下一次展开的迭代可能从以64字节为粒度的存储器中取出，对于每次迭代。地址的最低6位可以被压缩，因此 N = 2<sup>6</sup> = 64。“disp8”对有效地址计算所起的作用为：64 * disp8。

```asm
vbroadcastf32x4 zmm7, disp8[membase + index*8]
```

在上述的VBROADCASTF32x4指令中，存储器被取作为4个32位实例的四元组。因而通常可以被压缩的最低地址位为4，对应于四元组宽度：2<sup>4</sup> = 16个字节（4x32位）。从而N = 2<sup>4</sup>。

对于仅更新目的操作数中一个元素，或是源操作数元素被分别独立获取的EVEX编码的指令，可被压缩的最低地址位个数一般是数据元素的字节宽度，因而N = 2<sup>(width)</sup>。

这里需要注意的是，上述汇编代码中的**disp8**在真正用的时候是一个立即数。我们可以看以下几个实际的使用例子：

```asm
    vpxord  zmm0, zmm0, zmm0
    vpaddd  zmm1, zmm0, 64[rcx + 8 * 8]
    vpaddd  zmm2, zmm0, 128[rcx]

    ; 由于这里直接取64个字节的数据，
    ; 因此地址被压缩，
    ; 实际计算时采用64 * disp8。
    vpaddd  zmm3, zmm0, (64 * 255)[rcx + 8 * rax]
```

<br />

## 15.7 存储器对齐

对于EVEX编码的SIMD指令的存储器对齐要求类似于VEX编码的SIMD指令。对于EVEX编码的SIMD指令，应用以下三个类别的存储器对齐：

• 显式对齐的、具有EVEX前缀编码的512位向量长度的SIMD加载和存储指令访问64字节的存储器（比如，`VMOVAPD`、`VMOVAPS`、`VMOVDQA`，等等）。这些指令总是需要存储器地址以64字节的边界对齐。

• 显式的非对齐SIMD加载和存储指令访问小于等于64字节的数据（比如，`VMOVUPD`、`VMOVUPS`、`VMOVDQU`、`VMOVQ`、`VMOVD`，等等）。这些指令不需要存储器地址与自然的向量长度字节边界对齐。

• 使用EVEX编码的大部分算术和数据处理指令支持存储器访问语义。当这些指令访问存储器时，没有对齐限制。

当非对齐访问跨Cache行或向量长度自然对齐的边界时，软件软件可以看到性能处罚，因而针对常用的数据集尽量做到合理的对齐应该是继续值得鼓励的。

在Intel 64与IA-32架构中的原子存储器操作仅需要确保存储器操作数大小和对齐场景的一个子集。这些需要保障的原子操作在3A中的第7.1.1节中描述。

<br />

# 优化手册第17章——对Intel AVX-512指令的软件优化

Intel高级向量扩展512（Intel® AVX-512）是以下所要描述的512位指令集扩展，由起始于Skylake服务器微架构的最近微架构、以及基于Knights Landing的Intel Xeon Phi处理器予以支持。

- Intel AVX-512基础（F）    
    —— 512位向量宽度    
    —— 32个512位长的向量寄存器    
    —— 数据扩展与数据压缩指令    
    —— 三目逻辑指令    
    —— 8个全新的64位长的掩码寄存器    
    —— 两个源跨道（cross-lane）置换（permute）指令    
    —— 散播（scatter）指令    
    —— 嵌入的广播/舍入    
    —— 超越函数的支持
    
- Intel AVX-512冲突探测指令（CD）
- Intel AVX-512指数与倒数指令（ER）
- Intel AVX-512预取指令（PF）
- Intel AVX-512字节与字（位宽的）指令（BW）
- Intel AVX-512双字与四字（位宽的）指令（DQ）——新的QWORD以及计算与转换指令
- Intel AVX-512向量长度扩展（VL）

本章中所列出的性能报告都基于禁用Intel Turbo-Boost技术、且核心与非核心频率设置为1.8GHz的Skylake服务器系统上的数据Cache单元（DCU）驻留数据测量。

<br />

## 17.1 基本的Intel AVX-512 VS AVX2编码

在大部分情况下，Intel AVX-512的主要性能驱动将会是512位的寄存器宽度。本章描述了在基本的Intel AVX2与Intel AVX-512编码之间的相似性和不同点，并解释了如何将Intel AVX2代码轻易地转换为Intel AVX-512。第一小节描述了内建代码的转换而第二小节展示了汇编代码。以下部分突出了要做这些转换时所需要考虑和对待的高级方面。

在以下小节中的例子实现了一个笛卡尔坐标系统的旋转。笛卡尔坐标系统中的一个点被描述为(x, y)对。以下图展示了将一个点(x, y)做笛卡尔旋转**θ**角度得到(x', y')的过程。

![图17.2 笛卡尔旋转](https://github.com/zenny-chen/Intel-AVX512-Brief-Introduction/blob/master/CartesianRotation.PNG)

<br />

### 17.1.1 内建指令代码

以下对Intel AVX2与Intel AVX-512的比较展示了如何将一单个Intel AVX2代码序列转为到Intel AVX-512。本例描述了Intel AVX的指令格式、64字节的ZMM寄存器、带有64字节对齐的动态与静态存储器分配，以及在一个ZMM寄存器中表示16个浮点元素的C数据类型。当做此转换时，遵循这些准则。

- 将静态与动态分配的缓存以64字节对齐
- 对常量使用两倍的补充缓存大小
- 将`__mm256_`内建名前缀变为`__mm512_`
- 将变量数据类型名从`__m256`变为`__m512`
- 将迭代次数除以2

例17-1：用内建函数实现笛卡尔坐标系统旋转

Intel AVX2内建代码

```cpp
#include <immintrin.h>
int main(void)
{
    int len = 3200;
    
    //Dynamic memory allocation with 32-byte alignment
    float* pInVector = (float *)_mm_malloc(len*sizeof(float),32);
    float* pOutVector = (float *)_mm_malloc(len*sizeof(float),32);
    
    //init data
    for (int i=0; i<len; i++)
        pInVector[i] = 1;

    float cos_teta = 0.8660254037;
    float sin_teta = 0.5;
    
    //Static memory allocation of 8 floats with 32-byte alignments
    alignas(32) float cos_sin_teta_vec[8] = {cos_teta, sin_teta, cos_teta, sin_teta, 
                                             cos_teta, sin_teta, cos_teta, sin_teta};
    alignas(32) float sin_cos_teta_vec[8] = {sin_teta, cos_teta, sin_teta, cos_teta, 
                                             sin_teta, cos_teta, sin_teta, cos_teta};

    //__m256 data type represents a Ymm register with 8 float elements
    __m256 Ymm_cos_sin = _mm256_load_ps(cos_sin_teta_vec);
    
    //Intel® AVX2 256-bit packed single load
    __m256 Ymm_sin_cos = _mm256_load_ps(sin_cos_teta_vec);
    __m256 Ymm0, Ymm1, Ymm2, Ymm3;

    //processing 16 elements in an unrolled twice loop
    for(int i=0; i<len; i+=16)
    {
        Ymm0 = _mm256_load_ps(pInVector+i);
        Ymm1 = _mm256_moveldup_ps(Ymm0);
        Ymm2 = _mm256_movehdup_ps(Ymm0);
        Ymm2 = _mm256_mul_ps(Ymm2,Ymm_sin_cos);
        Ymm3 = _mm256_fmaddsub_ps(Ymm1,Ymm_cos_sin,Ymm2);
        _mm256_store_ps(pOutVector + i,Ymm3);
        Ymm0 = _mm256_load_ps(pInVector+i+8);
        Ymm1 = _mm256_moveldup_ps(Ymm0);
        Ymm2 = _mm256_movehdup_ps(Ymm0);
        Ymm2 = _mm256_mul_ps(Ymm2, Ymm_sin_cos);
        Ymm3 = _mm256_fmaddsub_ps(Ymm1,Ymm_cos_sin,Ymm2);
        _mm256_store_ps(pOutVector+i+8,Ymm3);
    }

    _mm_free(pInVector);
    _mm_free(pOutVector);
    return 0;
}
```

Intel AVX-512内建代码

```cpp
#include <immintrin.h>
int main(void)
{
    int len = 3200;

    // Dynamic memory allocation with 64-byte alignment
    float* pInVector = (float *)_mm_malloc(len*sizeof(float),64);
    float* pOutVector = (float *)_mm_malloc(len*sizeof(float),64);

    //init data
    for (int i=0; i<len; i++)
        pInVector[i] = 1;

    float cos_teta = 0.8660254037;
    float sin_teta = 0.5;

    // Static memory allocation of 16 floats with 64-byte alignments
    alignas(64) float cos_sin_teta_vec[16] = {cos_teta, sin_teta, cos_teta, sin_teta, cos_teta, sin_teta, 
                                              cos_teta, sin_teta, cos_teta, sin_teta, cos_teta, sin_teta,
                                              cos_teta, sin_teta, cos_teta, sin_teta};
    alignas(64) float sin_cos_teta_vec[16] = {sin_teta, cos_teta, sin_teta, cos_teta, sin_teta, cos_teta,
                                              sin_teta, cos_teta, sin_teta, cos_teta, sin_teta, cos_teta,
                                              sin_teta, cos_teta, sin_teta, cos_teta};

    //__m512 data type represents a Zmm register with 16 float elements
    __m512 Zmm_cos_sin = _mm512_load_ps(cos_sin_teta_vec);
    
    //Intel® AVX-512 512-bit packed single load
    __m512 Zmm_sin_cos = _mm512_load_ps(sin_cos_teta_vec);
    __m512 Zmm0, Zmm1, Zmm2, Zmm3;

    //processing 32 elements in an unrolled twice loop
    for(int i=0; i<len; i+=32)
    {
        Zmm0 = _mm512_load_ps(pInVector+i);
        Zmm1 = _mm512_moveldup_ps(Zmm0);
        Zmm2 = _mm512_movehdup_ps(Zmm0);
        Zmm2 = _mm512_mul_ps(Zmm2,Zmm_sin_cos);
        Zmm3 = _mm512_fmaddsub_ps(Zmm1,Zmm_cos_sin,Zmm2);
        _mm512_store_ps(pOutVector + i,Zmm3);
        Zmm0 = _mm512_load_ps(pInVector+i+16);
        Zmm1 = _mm512_moveldup_ps(Zmm0);
        Zmm2 = _mm512_movehdup_ps(Zmm0);
        Zmm2 = _mm512_mul_ps(Zmm2, Zmm_sin_cos);
        Zmm3 = _mm512_fmaddsub_ps(Zmm1,Zmm_cos_sin,Zmm2);
        _mm512_store_ps(pOutVector+i+16,Zmm3);
    }
    
    _mm_free(pInVector);
    _mm_free(pOutVector);
    return 0;
}
```

<br />

### 17.1.2 汇编代码

与内建代码的移植准则类似，以下列出汇编代码移植准则：

- 将静态与动态分配的缓存以64字节对齐
- 对常量使用两倍的补充缓存大小，如果需要的话
- 对指令名添加一个“**v**”前缀
- 将寄存器名由`ymm`变为`zmm`
- 将迭代次数除以2（或将宽度长度翻倍）

Intel AVX2汇编代码

```cpp
#include <immintrin.h>
int main(void)
{
    int len = 3200;

    //Dynamic memory allocation with 32-byte alignment
    float* pInVector = (float *)_mm_malloc(len*sizeof(float),32);
    float* pOutVector = (float *)_mm_malloc(len*sizeof(float),32);

    //init data
    for (int i=0; i<len; i++)
        pInVector[i] = 1;

    float cos_teta = 0.8660254037;
    float sin_teta = 0.5;

    //Static memory allocation of 8 floats with 32byte alignments
    alignas(32) float cos_sin_teta_vec[8] = {cos_teta, sin_teta, cos_teta, sin_teta, 
                                             cos_teta, sin_teta, cos_teta, sin_teta};
    alignas(32) float sin_cos_teta_vec[8] = {sin_teta, cos_teta, sin_teta, cos_teta, 
                                             sin_teta, cos_teta, sin_teta, cos_teta};

    __asm
    {
        mov rax,pInVector
        mov r8,pOutVector

        // Load into a ymm register of 32 bytes
        vmovups ymm3,ymmword ptr[cos_sin_teta_vec]
        vmovups ymm4, ymmword ptr[sin_cos_teta_vec]
        
        mov edx, len
        shl edx, 2
        xor ecx, ecx

loop1:
        vmovsldup ymm0, [rax+rcx]
        vmovshdup ymm1, [rax+rcx]
        vmulps ymm1, ymm1, ymm4
        vfmaddsub213ps ymm0, ymm3, ymm1

        // 32-byte store from a ymm register
        vmovaps [r8+rcx], ymm0
        vmovsldup ymm0, [rax+rcx+32]
        vmovshdup ymm1, [rax+rcx+32]
        vmulps ymm1, ymm1, ymm4
        vfmaddsub213ps ymm0, ymm3, ymm1

        // offset 32 bytes from previous store
        vmovaps [r8+rcx+32], ymm0

        // Processed 64bytes in this loop (the code is unrolled twice)
        add ecx, 64
        cmp ecx, edx
        jl loop1
    }

    _mm_free(pInVector);
    _mm_free(pOutVector);
    return 0;
}
```

Intel AVX-512汇编代码

```cpp
#include <immintrin.h>
int main(void)
{
    int len = 3200;

    //Dynamic memory allocation with 64byte alignment
    float* pInVector = (float *)_mm_malloc(len*sizeof(float),64);
    float* pOutVector = (float *)_mm_malloc(len*sizeof(float),64);

    //init data
    for (int i=0; i<len; i++)
        pInVector[i] = 1;

    float cos_teta = 0.8660254037;
    float sin_teta = 0.5;

    //Static memory allocation of 16 floats with 64byte alignments
    alignas(64) float cos_sin_teta_vec[16] = {cos_teta, sin_teta, cos_teta, sin_teta, cos_teta, sin_teta, cos_teta,
                                              sin_teta, cos_teta, sin_teta, cos_teta, sin_teta, cos_teta,
                                              sin_teta, cos_teta, sin_teta};
    alignas(64) float sin_cos_teta_vec[16] = {sin_teta, cos_teta, sin_teta, cos_teta, sin_teta, cos_teta,
                                              sin_teta, cos_teta, sin_teta, cos_teta, sin_teta, cos_teta,
                                              sin_teta, cos_teta, sin_teta, cos_teta};

    __asm
    {
        mov rax,pInVector
        mov r8,pOutVector

        // Load into a zmm register of 64 bytes
        vmovups zmm3, zmmword ptr[cos_sin_teta_vec]
        vmovups zmm4, zmmword ptr[sin_cos_teta_vec]
        mov edx, len
        shl edx, 2
        xor ecx, ecx

loop1:
        vmovsldup zmm0, [rax+rcx]
        vmovshdup zmm1, [rax+rcx]
        vmulps zmm1, zmm1, zmm4
        vfmaddsub213ps zmm0, zmm3, zmm1

        // 64 byte store from a zmm register
        vmovaps [r8+rcx], zmm0
        vmovsldup zmm0, [rax+rcx+64]
        vmovshdup zmm1, [rax+rcx+64]
        vmulps zmm1, zmm1, zmm4
        vfmaddsub213ps zmm0, zmm3, zmm1

        // offset 64 bytes from previous store
        vmovaps [r8+rcx+64], zmm0

        // Processed 128-bytes in this loop (the code is unrolled twice)
        add ecx, 128
        cmp ecx, edx
        jl loop1
    }
    
    _mm_free(pInVector);
    _mm_free(pOutVector);
    return 0;
}
```

<br />

## 17.2 掩码

使用扩展VEX编码模式（EVEX）的Intel AVX-512指令编码了一个谓词操作数，以带条件地控制逐元素的计算操作，并将结果更新到目的操作数。谓词操作数称之为掩码操作（**opmask**）寄存器。掩码操作是一组八个架构寄存器，每个64位。这8个架构寄存器中，只有k1到k7可以被寻址为谓词操作数；k0可被用作为常规的源或目的操作数，但不能被编码为一个谓词操作数。

一个谓词操作数对于某些带有存储器源操作数的指令可以被用于允许存储器错误抑制。

作为一个谓词操作数，掩码操作寄存器包含了一个比特用于掌管一个向量寄存器的每个数据元素的操作/更新。掩码在Skylake微架构上对所有数据大小的指令全都支持：字节（`int8`）、字（`int16`）、单精度浮点（`float32`）、整数双字（`int32`）、双精度浮点（`float64`）、整数四字（`int64`）。从而，一个向量寄存器保持着8、16、32或64个元素；根据这个特性，一个向量掩码寄存器的长度为64位。Skylake微架构上的掩码也对所有向量长度的值允许：128位、256位以及512位。每条指令仅访问它所需要的最低有效掩码位的个数，基于其数据类型和向量长度。比如，Intel AVX-512指令对具有512位向量长度的64位数据元素进行操作，那么仅使用掩码操作寄存器的8（即512/64）个最低有效位。

一个掩码寄存器以逐元素的粒度影响了一条AVX-512指令。所以，任一对每个元素的、以及对目的操作数的中间结果的逐元素更新的数值或非数值操作，会以掩码寄存器的相应位上进行断言。

AVX-512中作为谓词操作数的掩码操作具有以下特性：

- 指令的操作仅针对一个元素执行，如果对应的掩码操作被置1的话。这暗示了对一个被掩码关闭（即掩码值为0）的元素的一个操作，不会引发异常或侵犯。因而，对一个被掩码关闭的操作的结果不会更新**MXCSR**异常标志。
- 一个目的元素不会以操作的结果而更新，如果对应的写掩码位不被置1的话。取而代之的是，目的元素的值可以被保留（融合掩码），或是被清零（清零掩码）。
- 对于带有一个存储器操作符的某些指令，对带有掩码位为0的元素，存储器故障会被抑制。

注意，这个特征提供了一个非常强大的对控制流谓词实现的构造，由于掩码提供了对Intel AVX-512向量寄存器目的操作数的融合行为。作为一种替代方式，掩码操作可以用于清零，而非融合，以至于掩码位为0的元素会用0去更新，而不是保留旧的值。清零行为移除了对旧值的隐式依赖，当该值不再需要时。

大部分允许掩码操作的指令都接受两种形式的掩码方式。必须具有EVEX.aaa比特的指令与0（收集与扩散）以及写入到存储器有所不同，它只接受融合掩码。

当目的操作数是一个存储器位置时，也会应用逐元素的目的操作数更新规则。向量被写在以每个元素为基准，基于被用作为一个谓词操作数的掩码寄存器。

一个掩码操作寄存器的值可以是：

- 作为一条向量指令的结果而产生（**CMP**、**FPCLASS**、等等）。
- 从存储器加载。
- 从GPR寄存器加载。
- 被掩码到掩码的操作所修改。

<br />

### 17.2.1 掩码例子

受到掩码屏蔽的指令对打包的数据元素做带条件的操作，依赖于与每个数据元素相关联的掩码位每个数据元素的掩码位就是掩码寄存器中对应的比特位。

当执行一条掩码指令时，如果对应掩码值为0，那么对于该元素的返回值为0。目的寄存器中相应的值依赖于清零标志：    
- 如果标准被置1，那么存储器位置用零来填充。
- 如果标志不被置1，那么存储器位置的值会被保留。

以下这些图展示了一个例子，使用融合掩码，将一个寄存器掩码搬移到另一个寄存器。

```asm
vmovaps zmm1 {k1}, zmm0
```

在这条指令执行前的目的寄存器的值如下所示：

<br />

![opt-17.2.1-1.JPG](https://github.com/zenny-chen/Intel-AVX512-Brief-Introduction/blob/master/opt-17.2.1-1.JPG)

<br />

操作如下所示：

<br />

![opt-17.2.1-2.JPG](https://github.com/zenny-chen/Intel-AVX512-Brief-Introduction/blob/master/opt-17.2.1-2.JPG)

<br />

带有清零掩码的执行结果如下（注意指令中的 `{z}`）：

```asm
vmovaps zmm1 {k1}{z}, zmm0
```

<br />

![opt-17.2.1-3.JPG](https://github.com/zenny-chen/Intel-AVX512-Brief-Introduction/blob/master/opt-17.2.1-3.JPG)

<br />

注意，融合掩码操作具有对目的操作数的依赖性，而清零掩码则没有这种依赖。

以下代码例子展示了相比起Intel AVX2，Intel AVX-512的掩码是如何用来完成算法的：

```c
const int N = miBufferWidth;
const double* restrict a = A;
const double* restrict b = B;
double* restrict c = Cref;

for (int i = 0; i < N; i++)
{
    double res = b[i];
    if(a[i] > 1.0){
        res = res * a[i];
    }
    c[i] = res;
}
```

例17-3：使用内建函数的掩码操作

Intel AVX2指令代码：

```c
for (int i = 0; i < N; i+=32)
{
    __m256d aa, bb, mask;
    #pragma unroll(8)
    for (int j = 0; j < 8; j++)
    {
        aa = _mm256_loadu_pd(a+i+j*4);
        bb = _mm256_loadu_pd(b+i+j*4);
        mask = _mm256_cmp_pd(_mm256_set1_pd(1.0), aa, 1);
        aa = _mm256_and_pd(aa, mask); // zero the false values
        aa = _mm256_mul_pd(aa, bb);
        bb = _mm256_blendv_pd(bb, aa, mask);
        _mm256_storeu_pd(c+4*j, bb);
    }
    c += 32;
}

// 基准
```

Intel AVX-512指令代码：

```c
for (int i = 0; i < N; i+=32)
{
    __m512d aa, bb;
    __mmask8 mask;
    #pragma unroll(4)
    for (int j = 0; j < 4; j++)
    {
        aa = _mm512_loadu_pd(a+i+j*8);
        bb = _mm512_loadu_pd(b+i+j*8);
        mask = _mm512_cmp_pd_mask(_mm512_set1_pd(1.0), aa, 1);
        bb = _mm512_mask_mul_pd(bb, mask, aa, bb);
        _mm512_storeu_pd(c+8*j, bb);
    }
    c += 32;
}

// 加速2.9倍
```

<br />

例17-4：使用汇编的掩码操作：

Intel AVX2汇编代码：

```asm
loop:
vmovupd ymm1, ymmword ptr [rax+rcx*8]
inc r9d
vmovupd ymm6, ymmword ptr [rax+rcx*8+0x20]
vmovupd ymm2, ymmword ptr [r11+rcx*8]
vmovupd ymm7, ymmword ptr [r11+rcx*8+0x20]
vmovupd ymm11, ymmword ptr [rax+rcx*8+0x40]
vmovupd ymm12, ymmword ptr [r11+rcx*8+0x40]
vcmppd ymm4, ymm0, ymm1, 0x1
vcmppd ymm9, ymm0, ymm6, 0x1
vcmppd ymm14, ymm0, ymm11, 0x1
vandpd ymm16, ymm1, ymm4
vandpd ymm17, ymm6, ymm9
vmulpd ymm3, ymm16, ymm2
vmulpd ymm8, ymm17, ymm7
vmovupd ymm1, ymmword ptr [rax+rcx*8+0x60]
vmovupd ymm6, ymmword ptr [rax+rcx*8+0x80]
vblendvpd ymm5, ymm2, ymm3, ymm4
vblendvpd ymm10, ymm7, ymm8, ymm9
vmovupd ymm2, ymmword ptr [r11+rcx*8+0x60]
vmovupd ymm7, ymmword ptr [r11+rcx*8+0x80]
vmovupd ymmword ptr [rsi+r10*1], ymm5
vmovupd ymmword ptr [rsi+r10*1+0x20], ymm10
vcmppd ymm4, ymm0, ymm1, 0x1
vcmppd ymm9, ymm0, ymm6, 0x1
vandpd ymm18, ymm11, ymm14
vandpd ymm19, ymm1, ymm4
vandpd ymm20, ymm6, ymm9
vmulpd ymm13, ymm18, ymm12
vmulpd ymm3, ymm19, ymm2
vmulpd ymm8, ymm20, ymm7
vmovupd ymm11, ymmword ptr [rax+rcx*8+0xa0]
vmovupd ymm1, ymmword ptr [rax+rcx*8+0xc0]
vmovupd ymm6, ymmword ptr [rax+rcx*8+0xe0]
vblendvpd ymm15, ymm12, ymm13, ymm14
vblendvpd ymm5, ymm2, ymm3, ymm4
vblendvpd ymm10, ymm7, ymm8, ymm9
vmovupd ymm12, ymmword ptr [r11+rcx*8+0xa0]
vmovupd ymm2, ymmword ptr [r11+rcx*8+0xc0]
vmovupd ymm7, ymmword ptr [r11+rcx*8+0xe0]
vmovupd ymmword ptr [rsi+r10*1+0x40], ymm15
vmovupd ymmword ptr [rsi+r10*1+0x60], ymm5
vmovupd ymmword ptr [rsi+r10*1+0x80], ymm10
vcmppd ymm14, ymm0, ymm11, 0x1
vcmppd ymm4, ymm0, ymm1, 0x1
vcmppd ymm9, ymm0, ymm6, 0x1
vandpd ymm21, ymm11, ymm14
add rcx, 0x20
vandpd ymm22, ymm1, ymm4
vandpd ymm23, ymm6, ymm9
vmulpd ymm13, ymm21, ymm12
vmulpd ymm3, ymm22, ymm2
vmulpd ymm8, ymm23, ymm7
vblendvpd ymm15, ymm12, ymm13, ymm14
vblendvpd ymm5, ymm2, ymm3, ymm4
vblendvpd ymm10, ymm7, ymm8, ymm9
vmovupd ymmword ptr [rsi+r10*1+0xa0], ymm15
vmovupd ymmword ptr [rsi+r10*1+0xc0], ymm5
vmovupd ymmword ptr [rsi+r10*1+0xe0], ymm10
add rsi, 0x100
cmp r9d, r8d
jb loop

; // 基准
```

Intel AVX-512指令代码：

```asm
loop:
vmovups zmm0, zmmword ptr [rax+rcx*8]
inc r9d
vmovups zmm2, zmmword ptr [rax+rcx*8+0x40]
vmovups zmm4, zmmword ptr [rax+rcx*8+0x80]
vmovups zmm6, zmmword ptr [rax+rcx*8+0xc0]
vmovups zmm1, zmmword ptr [r11+rcx*8]
vmovups zmm3, zmmword ptr [r11+rcx*8+0x40]
vmovups zmm5, zmmword ptr [r11+rcx*8+0x80]
vmovups zmm7, zmmword ptr [r11+rcx*8+0xc0]
vcmppd k1, zmm8, zmm0, 0x1
vcmppd k2, zmm8, zmm2, 0x1
vcmppd k3, zmm8, zmm4, 0x1
vcmppd k4, zmm8, zmm6, 0x1
vmulpd zmm1{k1}, zmm0, zmm1
vmulpd zmm3{k2}, zmm2, zmm3
vmulpd zmm5{k3}, zmm4, zmm5
vmulpd zmm7{k4}, zmm6, zmm7
vmovups zmmword ptr [rsi+r10*1], zmm1
vmovups zmmword ptr [rsi+r10*1+0x40], zmm3
vmovups zmmword ptr [rsi+r10*1+0x80], zmm5
vmovups zmmword ptr [rsi+r10*1+0xc0], zmm7
add rcx, 0x20
add rsi, 0x100
cmp r9d, r8d
jb loop

; // 提速2.9倍
```

<br />

### 17.2.2 掩码开销

使用掩码可能会导致更低的性能，比起相应的非掩码代码。这可能会由于以下场景而导致：

- 对每个加载会有一个额外的混合（blend）操作。
- 当使用融合掩码时，会产生对目的操作数的依赖。当使用清零掩码时，此依赖不会存在。
- 更多具有限制性的转运规则。

以下例子展示了使用融合掩码是如何创建了对目的寄存器的依赖的。

表17-1：掩码例子

没有掩码：

```asm
mov rbx, iter
loop:
vmulps zmm0, zmm9, zmm8
vmulps zmm1, zmm9, zmm8
dec rbx
jnle loop

; // 基准
```

融合掩码：

```asm
mov rbx, iter
loop:
vmulps zmm0{k1}, zmm9, zmm8
vmulps zmm1{k1}, zmm9, zmm8
dec rbx
jnle loop

; // 降速4倍
```

清零掩码：

```asm
mov rbx, iter
loop:
vmulps zmm0{k1}{z}, zmm9, zmm8
vmulps zmm1{k1}{z}, zmm9, zmm8
dec rbx
jnle loop

; // 与基准相同
```

不带掩码，处理器在一个双FMA的服务器上执行了每个周期执行了2个乘法。    
带有融合掩码，处理器每4个周期执行了2个乘法，在迭代N处依赖于迭代N-1的乘法输出。    
清零融合对目的寄存器并不具有依赖性，从而可以每个周期执行2个乘法。

***建议：*** *掩码具有开销，因此只有在必要时再使用它。尽可能使用清零掩码，而不是融合掩码。*

<br />

### 17.2.3 掩码 vs. 混合（Blending）

