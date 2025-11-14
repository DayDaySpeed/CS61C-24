# 61C Project 1: snek

# 61C Project 2: CS61Classify

=======

# CS61CPU

Look ma, I made a CPU! Here's what I did:



| 编号 | 名称（xN） | ABI 名称                         | 用途说明                   |
| ---- | ---------- | -------------------------------- | -------------------------- |
| x0   | zero       | zero                             | 常数 0，读取为 0，写入无效 |
| x1   | ra         | return address                   | 函数返回地址               |
| x2   | sp         | stack pointer                    | 栈顶指针                   |
| x3   | gp         | global pointer                   | 全局变量指针               |
| x4   | tp         | thread pointer                   | 线程局部存储指针           |
| x5   | t0         | temporary 0                      | 临时寄存器（调用者保存）   |
| x6   | t1         | temporary 1                      | 临时寄存器（调用者保存）   |
| x7   | t2         | temporary 2                      | 临时寄存器（调用者保存）   |
| x8   | s0         | saved register 0 / frame pointer | 被调用者保存 / 栈帧指针    |
| x9   | s1         | saved register 1                 | 被调用者保存               |
| x10  | a0         | argument 0 / return value        | 函数参数 / 返回值          |
| x11  | a1         | argument 1                       | 函数参数 / 返回值          |
| x12  | a2         | argument 2                       | 函数参数                   |
| x13  | a3         | argument 3                       | 函数参数                   |
| x14  | a4         | argument 4                       | 函数参数                   |
| x15  | a5         | argument 5                       | 函数参数                   |
| x16  | a6         | argument 6                       | 函数参数                   |
| x17  | a7         | argument 7                       | 函数参数                   |
| x18  | s2         | saved register 2                 | 被调用者保存               |
| x19  | s3         | saved register 3                 | 被调用者保存               |
| x20  | s4         | saved register 4                 | 被调用者保存               |
| x21  | s5         | saved register 5                 | 被调用者保存               |
| x22  | s6         | saved register 6                 | 被调用者保存               |
| x23  | s7         | saved register 7                 | 被调用者保存               |
| x24  | s8         | saved register 8                 | 被调用者保存               |
| x25  | s9         | saved register 9                 | 被调用者保存               |
| x26  | s10        | saved register 10                | 被调用者保存               |
| x27  | s11        | saved register 11                | 被调用者保存               |
| x28  | t3         | temporary 3                      | 临时寄存器（调用者保存）   |
| x29  | t4         | temporary 4                      | 临时寄存器（调用者保存）   |
| x30  | t5         | temporary 5                      | 临时寄存器（调用者保存）   |
| x31  | t6         | temporary 6                      | 临时寄存器（调用者保存）   |

**zero（x0）**：永远为 0

**ra（x1）**：返回地址

**sp（x2）**：栈指针

**gp / tp（x3–x4）**：全局 / 线程指针

**t0–t6（x5–x7, x28–x31）**：临时寄存器（调用者保存）

**s0–s11（x8–x9, x18–x27）**：保存寄存器（被调用者保存）

**a0–a7（x10–x17）**：函数参数 / 返回值





| 类型       | 字段布局（从左到右）                                         | 位位置说明                                                   |
| ---------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| **R-type** | `funct7` `rs2` `rs1` `funct3` `rd` `opcode`                  | `[31:25]` `[24:20]` `[19:15]` `[14:12]` `[11:7]` `[6:0]`     |
| **I-type** | `imm[11:0]` `rs1` `funct3` `rd` `opcode`                     | `[31:20]` `[19:15]` `[14:12]` `[11:7]` `[6:0]`               |
| **S-type** | `imm[11:5]` `rs2` `rs1` `funct3` `imm[4:0]` `opcode`         | `[31:25]` `[24:20]` `[19:15]` `[14:12]` `[11:7]` `[6:0]`     |
| **B-type** | `imm[12]` `imm[10:5]` `rs2` `rs1` `funct3` `imm[4:1]` `imm[11]` `opcode` | `[31]` `[30:25]` `[24:20]` `[19:15]` `[14:12]` `[11:8]` `[7]` `[6:0]` |
| **U-type** | `imm[31:12]` `rd` `opcode`                                   | `[31:12]` `[11:7]` `[6:0]`                                   |
| **J-type** | `imm[20]` `imm[10:1]` `imm[11]` `imm[19:12]` `rd` `opcode`   | `[31]` `[30:21]` `[20]` `[19:12]` `[11:7]` `[6:0]`           |





## 🔧 你需要生成的控制信号（按项目规范）

| 信号名      | 位宽 | 说明                                     |
| ----------- | ---- | ---------------------------------------- |
| `RegWEn`    | 1    | 是否写回寄存器 rd                        |
| `ASel`      | 1    | ALU 输入 A 选择：0=rs1，1=PC             |
| `BSel`      | 1    | ALU 输入 B 选择：0=rs2，1=立即数         |
| `ALUSel`    | 4    | ALU 操作类型（加、减、与、或、乘等）     |
| `PCSel`     | 2    | PC 更新方式：0=PC+4，1=PC+imm，2=rs1+imm |
| `BrUn`      | 1    | 分支比较是否无符号                       |
| `MemRW`     | 1    | 是否写内存（store）                      |
| `LoadType`  | 3    | load 类型（byte/half/word）              |
| `StoreType` | 2    | store 类型（byte/half/word）             |
| `WBSel`     | 2    | 写回数据选择：0=ALU，1=DMEM，2=PC+4      |





## 🧠 R-type 指令（opcode = `0110011` = 0x33）

| 指令  | opcode | funct3 | funct7 |
| ----- | ------ | ------ | ------ |
| add   | 0x33   | 0x0    | 0x00   |
| sub   | 0x33   | 0x0    | 0x20   |
| mul   | 0x33   | 0x0    | 0x01   |
| sll   | 0x33   | 0x1    | 0x00   |
| mulh  | 0x33   | 0x1    | 0x01   |
| slt   | 0x33   | 0x2    | 0x00   |
| xor   | 0x33   | 0x4    | 0x00   |
| srl   | 0x33   | 0x5    | 0x00   |
| sra   | 0x33   | 0x5    | 0x20   |
| mulhu | 0x33   | 0x3    | 0x01   |
| or    | 0x33   | 0x6    | 0x00   |
| and   | 0x33   | 0x7    | 0x00   |

## 🧠 I-type 运算指令（opcode = `0010011` = 0x13）

| 指令 | opcode | funct3 | funct7 |
| ---- | ------ | ------ | ------ |
| addi | 0x13   | 0x0    | —      |
| slti | 0x13   | 0x2    | —      |
| xori | 0x13   | 0x4    | —      |
| ori  | 0x13   | 0x6    | —      |
| andi | 0x13   | 0x7    | —      |
| slli | 0x13   | 0x1    | 0x00   |
| srli | 0x13   | 0x5    | 0x00   |
| srai | 0x13   | 0x5    | 0x20   |

## 🧠 Load 指令（opcode = `0000011` = 0x03）

| 指令 | opcode | funct3 | funct7 |
| ---- | ------ | ------ | ------ |
| lb   | 0x03   | 0x0    | —      |
| lh   | 0x03   | 0x1    | —      |
| lw   | 0x03   | 0x2    | —      |

## 🧠 Store 指令（opcode = `0100011` = 0x23）

| 指令 | opcode | funct3 | funct7 |
| ---- | ------ | ------ | ------ |
| sb   | 0x23   | 0x0    | —      |
| sh   | 0x23   | 0x1    | —      |
| sw   | 0x23   | 0x2    | —      |

## 🧠 Branch 指令（opcode = `1100011` = 0x63）

| 指令 | opcode | funct3 | funct7 |
| ---- | ------ | ------ | ------ |
| beq  | 0x63   | 0x0    | —      |
| bne  | 0x63   | 0x1    | —      |
| blt  | 0x63   | 0x4    | —      |
| bge  | 0x63   | 0x5    | —      |
| bltu | 0x63   | 0x6    | —      |
| bgeu | 0x63   | 0x7    | —      |

## 🧠 U-type 指令

| 指令  | opcode | funct3 | funct7 |
| ----- | ------ | ------ | ------ |
| lui   | 0x37   | —      | —      |
| auipc | 0x17   | —      | —      |

## 🧠 J-type 指令

| 指令 | opcode | funct3 | funct7 |
| ---- | ------ | ------ | ------ |
| jal  | 0x6F   | —      | —      |

## 🧠 jalr 指令（I-type 跳转）

| 指令 | opcode | funct3 | funct7 |
| ---- | ------ | ------ | ------ |
| jalr | 0x67   | 0x0    | —      |

## 🧠 CSR 指令（opcode = `1110011` = 0x73）

| 指令  | opcode | funct3 | funct7 |
| ----- | ------ | ------ | ------ |
| csrw  | 0x73   | 0x1    | —      |
| csrwi | 0x73   | 0x5    | —      |



# R,I,S,B,U,J指令总揽

## 🧮 R-type 指令（寄存器运算）

| Instruction        | Opcode | Funct3 | Funct7 | Operation                                   |
| ------------------ | ------ | ------ | ------ | ------------------------------------------- |
| add rd, rs1, rs2   | 0x33   | 0x0    | 0x00   | R[rd] ← R[rs1] + R[rs2]                     |
| sub rd, rs1, rs2   | 0x33   | 0x0    | 0x20   | R[rd] ← R[rs1] - R[rs2]                     |
| mul rd, rs1, rs2   | 0x33   | 0x0    | 0x01   | R[rd] ← (R[rs1] * R[rs2])[31:0]             |
| sll rd, rs1, rs2   | 0x33   | 0x1    | 0x00   | R[rd] ← R[rs1] << R[rs2]                    |
| mulh rd, rs1, rs2  | 0x33   | 0x1    | 0x01   | R[rd] ← (R[rs1] * R[rs2])[63:32]            |
| mulhu rd, rs1, rs2 | 0x33   | 0x3    | 0x01   | R[rd] ← (R[rs1] * R[rs2])[63:32] (unsigned) |
| slt rd, rs1, rs2   | 0x33   | 0x2    | 0x00   | R[rd] ← (R[rs1] < R[rs2]) ? 1 : 0           |
| xor rd, rs1, rs2   | 0x33   | 0x4    | 0x00   | R[rd] ← R[rs1] ^ R[rs2]                     |
| srl rd, rs1, rs2   | 0x33   | 0x5    | 0x00   | R[rd] ← R[rs1] >> R[rs2] (unsigned)         |
| sra rd, rs1, rs2   | 0x33   | 0x5    | 0x20   | R[rd] ← R[rs1] >> R[rs2] (signed)           |
| or rd, rs1, rs2    | 0x33   | 0x6    | 0x00   | R[rd] ← R[rs1]                              |
| and rd, rs1, rs2   | 0x33   | 0x7    | 0x00   | R[rd] ← R[rs1] & R[rs2]                     |

## 📥 I-type 指令（立即数运算 / load / jalr / CSR）

| Instruction         | Opcode | Funct3 | Funct7/Imm | Operation                                   |
| ------------------- | ------ | ------ | ---------- | ------------------------------------------- |
| lb rd, offset(rs1)  | 0x03   | 0x0    | offset     | R[rd] ← SignExt(Mem(R[rs1] + offset, byte)) |
| lh rd, offset(rs1)  | 0x03   | 0x1    | offset     | R[rd] ← SignExt(Mem(R[rs1] + offset, half)) |
| lw rd, offset(rs1)  | 0x03   | 0x2    | offset     | R[rd] ← Mem(R[rs1] + offset, word)          |
| addi rd, rs1, imm   | 0x13   | 0x0    | imm        | R[rd] ← R[rs1] + imm                        |
| slli rd, rs1, imm   | 0x13   | 0x1    | 0x00       | R[rd] ← R[rs1] << imm                       |
| slti rd, rs1, imm   | 0x13   | 0x2    | imm        | R[rd] ← (R[rs1] < imm) ? 1 : 0              |
| xori rd, rs1, imm   | 0x13   | 0x4    | imm        | R[rd] ← R[rs1] ^ imm                        |
| srli rd, rs1, imm   | 0x13   | 0x5    | 0x00       | R[rd] ← R[rs1] >> imm                       |
| srai rd, rs1, imm   | 0x13   | 0x5    | 0x20       | R[rd] ← R[rs1] >> imm                       |
| ori rd, rs1, imm    | 0x13   | 0x6    | imm        | R[rd] ← R[rs1]                              |
| andi rd, rs1, imm   | 0x13   | 0x7    | imm        | R[rd] ← R[rs1] & imm                        |
| jalr rd, rs1, imm   | 0x67   | 0x0    | imm        | R[rd] ← PC + 4; PC ← R[rs1] + imm           |
| csrw rd, csr, rs1   | 0x73   | 0x1    | csr        | CSR[csr] ← R[rs1]                           |
| csrwi rd, csr, uimm | 0x73   | 0x5    | uimm       | CSR[csr] ← uimm                             |

## 📤 S-type 指令（store）

| Instruction         | Opcode | Funct3 | Immediate | Operation                           |
| ------------------- | ------ | ------ | --------- | ----------------------------------- |
| sb rs2, offset(rs1) | 0x23   | 0x0    | offset    | Mem(R[rs1] + offset) ← R[rs2][7:0]  |
| sh rs2, offset(rs1) | 0x23   | 0x1    | offset    | Mem(R[rs1] + offset) ← R[rs2][15:0] |
| sw rs2, offset(rs1) | 0x23   | 0x2    | offset    | Mem(R[rs1] + offset) ← R[rs2]       |

## 🔀 B-type 指令（分支跳转）

| Instruction           | Opcode | Funct3 | Immediate | Operation                                       |
| --------------------- | ------ | ------ | --------- | ----------------------------------------------- |
| beq rs1, rs2, offset  | 0x63   | 0x0    | offset    | if(R[rs1] == R[rs2]) PC ← PC + offset           |
| bne rs1, rs2, offset  | 0x63   | 0x1    | offset    | if(R[rs1] != R[rs2]) PC ← PC + offset           |
| blt rs1, rs2, offset  | 0x63   | 0x4    | offset    | if(R[rs1] < R[rs2]) PC ← PC + offset            |
| bge rs1, rs2, offset  | 0x63   | 0x5    | offset    | if(R[rs1] ≥ R[rs2]) PC ← PC + offset            |
| bltu rs1, rs2, offset | 0x63   | 0x6    | offset    | if(R[rs1] < R[rs2]) PC ← PC + offset (unsigned) |
| bgeu rs1, rs2, offset | 0x63   | 0x7    | offset    | if(R[rs1] ≥ R[rs2]) PC ← PC + offset (unsigned) |

## 🏗️ U-type 指令（高位立即数）

| Instruction      | Opcode | Immediate | Operation                 |
| ---------------- | ------ | --------- | ------------------------- |
| auipc rd, offset | 0x17   | offset    | R[rd] ← PC + offset << 12 |
| lui rd, offset   | 0x37   | offset    | R[rd] ← offset << 12      |

## 🚀 J-type 指令（跳转）

| Instruction | Opcode | Immediate | Operation                     |
| ----------- | ------ | --------- | ----------------------------- |
| jal rd, imm | 0x6F   | imm       | R[rd] ← PC + 4; PC ← PC + imm |

![image-20251114135025546](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20251114135025546.png)

## 拉取 starter 仓库（允许合并不相关历史）
git pull project03 main --allow-unrelated-histories
