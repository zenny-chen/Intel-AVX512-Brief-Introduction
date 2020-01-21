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


