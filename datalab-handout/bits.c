/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  return ~(~(~x & y) & ~(x & ~y));  
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  int x = 1;
  x = x << 31;
  return x;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  // x = -1 :   x + 1 = 0,   异或后  排除x+1 = 0,这种情况
  //当x=-1时, flag_2为 0  返回0
  //当x不等于-1时    flag_2为1,返回值是flag_1的值
  //flag_1    只有Tmin ^ Tmax 取反后等于0     
  //-1是一个特例   取反后为0
  int y = x + 1;  //Tmin
  int flag_1 = !(~(x ^ y)); 
  int flag_2= !!(y ^ 0);
  return flag_1 & flag_2; 
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int y = 0xAA; 
  //+优先级大于 << 
  y  = y + (y << 8);
  y = y + (y << 16);
  //只留下x的偶数位值
  x = x & y;
  return !(y ^ x) ;
}

/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int y = x & 0x3f; //截取x的后6位
  int flag_1 = !(x >> 6); //右移六位后,取反为1   x的高位全是0  符合条件
  int flag_2 = !((x >> 3) ^ 0x6);  //满足第一个条件后,右移3位   异或110   如果为0  就是属于'0' 到'7'
  int flag_3 = !(y ^ 0x38);  //满足第一个条件后   是'8'
  int flag_4 = !(y ^ 0x39);   //满足第一个条件后   是'9'
  return flag_1 & (flag_2 | flag_3 | flag_4);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  //返回值为:  x = 0, return (x & y) ^ (~x & z)   
  //           x != 0, 把x变为全1  返回相同
  //关键点在于 x == 0  和x!=0    这种情况有很多种
  //           将上面的问题转换成   0  对应  全1
  //                               !0  对应  全0
  //                               这样  就变成了两种情况  全0  全1
  //最终处理之后返回值为    (~x & y) ^ (x & z)
  x = !x;   //取非 把x变为 0 1两种    把他们扩展到32位
  x = ~x + 1;  //01快速扩展到全0全1 
 // x = (x << 1) + x;
 // x = (x << 2) + x;
 // x = (x << 4) + x;
 // x = (x << 8) + x;
 // x = (x << 16) + x;
  return ((~x) & y) ^ (x & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  //基本思路:  基本返回值0,找出返回1的条件
  // flag_3      x == y     1
  // flag_4      y-x  且xy同号  减法不会溢出    大于0  1    小于等于0  1
  //flag _5      x小于0  y大于0     1
  //flag_6       x大于0  y小于0     1
  int nagete_x = ~x + 1;
  int sum = y + nagete_x; 
  int flag_3 = !(x ^ y);
  int flag_5 = (x >> 31) & !(y >> 31);   
  int flag_6 = !(x >> 31) & (y >> 31);   
  int flag_4 = !(sum >> 31) & !flag_5 & !flag_6;   //xy同号  减法不会溢出  减法有效
 // printf("flag_1 = %d %x \n",flag_1, flag_1);
 // printf("flag_2 = %d %x \n",flag_2, flag_2);
 // printf("flag_3 = %d %x \n",flag_3, flag_3);
 // printf("flag_4 = %d %x \n",flag_4, flag_4);
 // printf("flag_5 = %d %x \n",flag_5, flag_5);
 // printf("flag_6 = %d %x \n",flag_6, flag_6);
  return flag_3 | flag_4 | flag_5;
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  //基本思路:   取负值,按位取反+1  只有0和Tmin的负值 是自己
  // x和-x右移31位  结果就是各自的符号位扩展31位
  // 相加后  有两种情况:(1): 0和Tmin   相加后结果为0
  //                    (2): 其他相加后    结果为全1
  //  flag_1:   ~sum   0和Tmin结果为全1  其他为全0
  //  flag_2:   ~0和~Tmin    ~Tmin 01111  ~0全1
  //  flag_3:  flag_1 & flag_2 Tmin为 01111 0为1111 其他为全0   
  //          右移31位后  0为 全1值为-1   其他为全0值为0   取负值后返回   
  int neg_x = ~x;
  int y = neg_x + 1;
  int hight_x = x >> 31;
  int hight_y = y >> 31;
  int flag_1 = ~(hight_x + hight_y);
  int flag_2 = neg_x;
  int flag_3 = (~(flag_1 & flag_2) >> 31) + 1;
 // printf("%d %x\n", flag_1, flag_1);
 // printf("%d %x\n", flag_1, flag_2);
 // printf("%d %x\n", flag_3, flag_3);
  return flag_3;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  //思路  用上面做的函数   conditional  做两个if功能    一次返回分半查找的剩余一半函数   
  //                                                    一次计算分半如果选择前一半,bit数要加上一半的长度,选择后面长度加0不做处理
  int x_32, x_16, x_8, x_4, x_2;  //分别表示x的不同比特
  int bit_8 = 0xff << 8;
  int bit_32 = (bit_8 + 0xff) << 16;   
  int bit_16 = bit_8;
  int flag_32,flag_16,flag_8,flag_4,flag_2;  //表示 ~x_32  重使用一次 减少 ops数量
  int sum = 0;
  int flag_nage = !!(x >> 31);     //  x是负数   值为1
  int extend_f_n = ~flag_nage + 1;      //扩展flag_nage

  x = (extend_f_n & (~x)) ^ ((~extend_f_n) & x);  // flag_nage 是1时  x赋值~x   是0是  x不变
  
  bit_8 = 0xf0; 

  x_32 = x & bit_32;
  x_32 = !x_32;
  x_32 = ~x_32 + 1;
  flag_32 = ~x_32;
  x_16 = (flag_32 & (x >> 16)) ^ ((x_32 & (x & (~bit_32))));
  sum = sum + (flag_32 & 16);

  x = x_16;   //改变x
  x_16 = x & bit_16;
  x_16 = !x_16;
  x_16 = ~x_16 + 1;
  flag_16 = ~x_16;
  x_8 = (flag_16 & (x >> 8)) ^ ((x_16 & (x & 0xff)));
  sum = sum + (flag_16 & 8);

  x = x_8;   //改变x
  x_8 = x & bit_8;
  x_8 = !x_8;
  x_8 = ~x_8 + 1;
  flag_8 = ~x_8;
  x_4 = (flag_8 & (x >> 4)) ^ ((x_8 & (x & (0xf))));   //~bit_8  改为0xf  省一个op
  sum = sum + (flag_8 & 4);

  x = x_4;   //改变x
  x_4 = x & 12;
  x_4 = !x_4;
  x_4 = ~x_4 + 1;
  flag_4 = ~x_4;
  x_2 = (flag_4 & (x >> 2)) ^ ((x_4 & (x & 3)));
  sum = sum + (flag_4 & 2);

  x = x_2;
  x_2 = x_2 & 2;
  x_2 = !x_2;
  x_2 = ~x_2 + 1;
  flag_2 = ~x_2;
  x = (flag_2 & (x >> 1)) ^ ((x_2 & (x & 1)));
  sum = sum + (flag_2 & 1);
  sum = sum + (x & 1);  //如果最后两位是01  x二分后是1  就要加1

  sum = sum + 1;  //加符号位
   
  return sum;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  //思路:  (1) 0返回0
  //       (2) exp全1 为无穷 直接返回
  //       (3) exp全0 f不是全0  直接加左移一位   最后补上符号位
  //                  f是全0   不做处理 返回原值 
  //       (4) exp 不全1全0   直接加1 相当于乘2
  unsigned bit_exp;
  unsigned bit_f;  //小数部分
  unsigned flag_s;  //符号位
  if(uf == 0){
    return 0;
  }

  bit_f = uf & (0x007fffff);
  bit_exp = uf & (0x7f800000);   //取出指数部分
  flag_s = uf >> 31;
  if(bit_exp == 0x7f800000){
    return uf;
  }
  bit_exp = bit_exp >> 23;       //exp的值
  if(bit_exp != 0){
    bit_exp = bit_exp + 1;
    bit_exp = bit_exp << 23;
    uf = uf & ~(0x7f800000);
    uf = uf ^ bit_exp;
  }
  if(bit_exp == 0 && bit_f != 0){
      uf = uf << 1;
      if(flag_s == 1){
        uf = uf | (flag_s << 31);  //补上符号位
      }
  }

  return uf;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  //基本思路 确定指数的范围   exp < 127   指数小于0时 小数,返回0
  //                          exp > 158    指数大于31  越界
  //                          exp 全0      返回0
  //                          exp 全1      返回0x80000000u
  //                          exp 其他情况下   对f部分补上隐藏位1  再右移exp-127位,
  //                              最后根据s返回正负
  unsigned bit_exp;
  unsigned bit_f;  //小数部分
  unsigned flag_s;  //符号位
  unsigned E;      //指数
  int F2Int;
  bit_f = uf & (0x007fffff);   //取出小数部分
  bit_f = bit_f | (0x00800000);  //补上前面隐藏1
  bit_exp = uf & (0x7f800000);   //取出指数部分
  bit_exp = bit_exp >> 23;
  flag_s = uf >> 31;
  //printf("s %u %x  exp  %u %x   f  %u  %x\n", flag_s, flag_s, bit_exp, bit_exp, bit_f, bit_f);
  if(bit_exp == 0xff || bit_exp > 158){  //无穷   或者NaN   指数越界  
    return 0x80000000u;
  }
  if(bit_exp < 127 || bit_exp  == 0){  //指数是负数   非规格数
    return 0;
  }   
  E = bit_exp - 127;
  bit_f = bit_f >> (23 - E);  //右移
  //printf("E %u %x  bit_f   %u %x \n", E, E, bit_f, bit_f);
  F2Int = bit_f;   
  if(flag_s == 1){
    F2Int = -F2Int;
  }
  return F2Int;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  //基本思路:
  //        时间超时  10s内跑不完所有测试数据  可能是电脑问题,或者是算法问题
  //        后面换一个机器测试一下
  unsigned bit_exp;
  unsigned bit_float;  //float值的bit位
  unsigned INF = 0x7f800000;
  if(x < -127){  //值太小
    return 0;
  }   
  if(x > 128){  //太大  返回+INF
    return INF;
  }
  //printf("x    %d\n", x);
  bit_exp = x + 127;
  bit_exp = bit_exp << 23;
  //printf("bit_float  %u  %x\n", bit_float, bit_float);
  return bit_exp;
}
