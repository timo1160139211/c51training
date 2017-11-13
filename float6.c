//////////////////////////////////
//      显示任意 float(6)       //
//                              //
//               @morpheus      //
//////////////////////////////////

//@@ debugging  >>   101 中 0 无法显示!
//@@ location   >>  递归求值算法 
//@@ solution   >>  整数 判断  (当前位模10)!=0 || (剩余数 >1)   [即 高位上还有值].  小数同理.  

#include <reg51.h>
#include <absacc.h>
//#include <math.h>

#define uc unsigned char

#define PA8255 XBYTE[0x7cff]
#define PB8255 XBYTE[0x7dff]
#define COM8255 XBYTE[0x7fff]

uc data buf[6];
uc code table[16]={
    0x3f,0x06,0x5b,0x4f,0x66,
    0x6d,0x7d,0x07,0x7f,0x6f,
    0x77,0x7c,0x39,0x5e,0x79,0x71
};

//延时函数   /s
void delay(unsigned char x){       
    data i;
    for(i=0;i<x*12000;i++);
}

//显示函数 
void display(){
    uc segcode,bitcode,i;
    bitcode=0xdf;
    for(i=0;i<6;i++){
        segcode=buf[i];
        PA8255=table[segcode];
        PB8255=bitcode;
        delay(0.005); //5ms
        PB8255=0xff;
        bitcode=bitcode>>1;
        bitcode=bitcode|0x80;
    }
}

//返回 整数的位数 
unsigned int subf_zheng(float f) compact reentrant{
    long longf = (long) f;
    if((longf%10)>0){
        return 1 + subf_zheng(f/10);
    }else{
        return 0;
    }
}

//返回 小数的位数
unsigned int subf_xiao(float f) compact reentrant{
    long k=(long)f;
    if((f-k)*10>=1){
        return 1 + subf_xiao((f-k)*10);
    }else{
        return 0;
    }
}

/**          
 *  思路 => Step1. 获取 (1)浮点数        : f 
 *                      (2)整数位的个数  : zhengshu
 *                      (3)小数位的个数  : xiaoshu 
 * 
 *          Step2. 放大 , 放大 10^xiaoshu 倍 , 为下一步拆数做准备.
 * 
 *          Step3. 赋值 : 取出每一位上的数 , 赋值到对应显示管上. 
 *          
 *          Step4. 显示 
 */
void main(){

		//任意  float(6)
        float f = 3.14159;

        unsigned int j=0;        //循环控制 
		unsigned int zhengshu=0; //整数位数 
		unsigned int xiaoshu=0;  //小数位数 
        unsigned long bigIntf=0; //保存 float 转换来的 长整型 
		unsigned long exp=0;     //放大倍数控制 
		
        COM8255 = 0x80;//控制字 

		//初始化。 默认全显示 0 
        buf[0]=0;buf[1]=0;buf[2]=0;buf[3]=0;buf[4]=0;buf[5]=0;

       //获取整数位  的数量   123.4  -> 3    0.123 -> 1   最少一个整数位 
       zhengshu=subf_zheng(f);
       if(zhengshu<1){zhengshu=1;}//如果整数少于一个，那么置位 1 个 
       if(zhengshu>6){zhengshu=6;}//如果整数多于6个，那么置位 6 个  ，(but  优先显示小数) 12345.12345  -> 5.12345 


       //获取小数位  的数量   0.123456  -> 6    0.0 ->     最多 5 个小数位,最少  ( 0.12345 ) (精确到后5位)   
       xiaoshu=subf_xiao(f);  
       //\\if(xiaoshu<1){xiaoshu=1;}//如果小数少于一个，那么置位 1 个   \\小数可以 0个 
       if(xiaoshu>5){xiaoshu=5;}//如果小数多于5个，那么置位 5 个 

       PA8255=table[zhengshu];  //1  debug----------------------------------------------------------
       PB8255=0x00;
       delay(3);
       
       
       PA8255=0x77;            //A   debug----------------------------------------------------------
       PB8255=0x00;
       delay(3);
       
       PA8255=table[xiaoshu];  //5    debug----------------------------------------------------------
       PB8255=0x00;
       delay(3);

       //计算出放大多少倍.   
       for(j=0,exp=1;(j<xiaoshu&&j<5);j++){exp*=10;}
       bigIntf=f*exp;//转换成长整形 ( )  0.1234567 -> 12345  ()  最多放大 10^5 倍 
       
       
       /**     小数:      3.1 4 1 5 9
        *       
        *    显示数码管:   0 1 2 3 4 5  
        *    每次循环%10:  9 5 1 4 1 3 
        *          两个正好相反. 
	    */   
       for(j=0;(j<6&& j<xiaoshu+zhengshu);j++){
           buf[5-j]=bigIntf%10;                  //将模10出来的 9 5 1 4 1 3 放到显示数码管的  5 4 3 2 1 上 
           bigIntf/=10;                          //依次 整除10. 
           if((j+1)==zhengshu){                  //个位后加 点'.'   cause:(init j =0; !=1;)
             buf[j]=buf[j]|0x80;                 // 0000 0000 |0x80 = 1000 0000   cause:最高位是点 
           }
       }

       //display
        while(1){
           for(j=0;j<3000;j++){
               display();
           }
        }
}






