/*  使用库Adafruit_ST7735
 * -针对合宙ESP32-C3以及合宙EVB-Air101-Expansion-LCD拓展块所修改编写的贪吃蛇小游戏- 
 *                                                   -2022.3.6-
 *                                                  BY Tracker
 * 
 * 
 */



/*                        硬件定义初始化块                              */

#include <stdlib.h>          
#include <Adafruit_GFX.h>    //核心图形库
#include <Adafruit_ST7735.h> //为ST7735硬件专用库
#include "Adafruit_ST77xx.h" //提供屏幕旋转功能
#include <SPI.h>


boolean UP, DOWN, LEFT, RIGHT, CENTER_FLAG; //全局标志位FLAG_VALUE----测试用

//屏幕引脚定义
#define TFT_CS          7
#define TFT_RST         10 //或者设置为-1，并连接到Arduino的RESET引脚
#define TFT_DC          6
#define TFT_MOSI        3 //数据输出
#define TFT_SCLK        2 //时钟输出
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,TFT_DC,TFT_MOSI,TFT_SCLK,TFT_RST);
//定义圆周率
#define pi 3.1415926


/*                      应用层定义初始化块                        */

//以下对应面积块的定义为 Width x Height
//屏幕大小：160 x 80  游戏地图：120 x 80  积分栏（菜单）：40 x 80

//定义地图大小
#define SCREEN_LIMIT 0  
#define SCREEN_WIDTH 120
#define SCREEN_HEIGHT 80
//定义蛇每节宽高
#define SNAKE_WIDTH 1 
#define SNAKE_HEIGHT 1
//定义方向/按键
#define DIR_U 1   //UP
#define DIR_D 2   //DOWN
#define DIR_L 3   //LEFT
#define DIR_R 4   //RIGHT
#define DIR_C 5   //CENTER

//蛇的相关参数
typedef struct Snake{
  int x;
  int y;
  struct Snake *last;
  struct Snake *next;  
}snake;
snake *head = NULL;
snake *tail = NULL; 
snake *egg = NULL;

int init_nodes = 20; //定义初始关节数量
int start_x = 80;  //定义头节点坐标
int start_y = 40;
int vx = 1; //定义每次移动的格数
int vy = 0;
int last_dir = 0; //记录上次方向


boolean Game_over = false; //结束标志


void setup(void) {
  Serial.begin(115200);
  Serial.print(F("Hello! ST77xx TFT Test"));

  //如果使用0.96“160x80 TFT，请使用此初始值设定项（取消注释）：
  tft.initR(INITR_MINI160x80); // Init ST7735S迷你显示器

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

/*                    硬件及应用初始化函数                       */
  tft.setRotation(3);  //屏幕方向设定
  tft.fillScreen(ST77XX_BLACK);
  
  Init_Title();
  GAME_Menu();

  KEY_GPIOinit();
  Ball_init();



}

void loop() {

  Ball_Move();

}

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextSize(1);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);  //设置字体包裹，是否打开
  tft.fillScreen(ST77XX_BLACK);  //填充背景颜色（必须填充，否则之前的图像，将不会消失）
  tft.setCursor(0, 30); //设置游标初始位置,x轴为0，Y轴为30
  tft.setTextColor(ST77XX_RED);  //设置字体为红色
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);//设置光标的起始位置
  tft.fillScreen(ST77XX_BLACK);//填充屏幕，为黑色（必须填充，否则之前的图像，将不会消失）
  tft.setTextColor(ST77XX_WHITE);//设置字体颜色，为白色
  tft.setTextSize(0);  //设置字体大小为，0
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(pi, 6); //显示，p这个变量，第6位以后，不再显示
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}

//以下个人编写


/*!
    @brief  显示打印文本的函数
    @param  x      文字左上角的x坐标
    @param  y      文字左上角的y坐标
    @param  size   文字字号  
    @param  text   文本信息，需要以字符串传入，该参数为指针类型
    @param  color  字体颜色
*/
void Drawtext(char x, char y, char size, char *text, uint16_t color) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(false);
  tft.print(text);
}

void Tracktech_test(){
  tft.setTextWrap(false);  //设置字体包裹，是否打开
  tft.fillScreen(ST77XX_BLACK);  //填充背景颜色（必须填充，否则之前的图像，将不会消失）
  tft.setCursor(0, 30); //设置游标初始位置,x轴为0，Y轴为30
  tft.setTextColor(ST77XX_WHITE);  //设置字体为白色
  tft.setTextSize(2);
  tft.println("Track-Tech!");
}

/*!
    @brief  用于按键GPIO初始化和宏定义
*/
void KEY_GPIOinit(){


//LCD拓展版按键定义
  #define UPKEY     8
  #define DWKEY     13
  #define LKEY      5
  #define RKEY      9
  #define CENTER    4
//按键对应GPIO初始化-上拉输入
  pinMode(UPKEY,INPUT_PULLUP);   //UPKEY  
  pinMode(DWKEY,INPUT_PULLUP);   //DWKEY
  pinMode(LKEY,INPUT_PULLUP);    //LKEY
  pinMode(RKEY,INPUT_PULLUP);    //RKEY
  pinMode(CENTER,INPUT_PULLUP);  //CENTER
//LED测试
  pinMode(12,OUTPUT);  //CENTER

}

/*!
    @brief  该函数用于按键扫描，上下左右中分别返回12345，若无按键则返回0
*/
int KEYScan(){

  if(digitalRead(UPKEY) == 0) {
    testdrawtext("UPKEY", ST77XX_WHITE);
    return 1;
    }
  else if(digitalRead(DWKEY) == 0) {    
    testdrawtext("DWKEY", ST77XX_WHITE);
    return 2;
    }
  else if(digitalRead(LKEY) == 0) {   
    testdrawtext("LKEY", ST77XX_WHITE);
    return 3;
    }
  else if(digitalRead(RKEY) == 0) {    
    testdrawtext("RKEY", ST77XX_WHITE);
    return 4;
    }
  else if(digitalRead(CENTER) == 0) {
    testdrawtext("CENTER", ST77XX_WHITE);
    return 5;
    }
  else{
    Space_Clean(2);//全清
    return 0;
    }

}

/*!
    @brief  启动时的加载界面
*/
void Init_Title(){
//主体名称显示   
  Drawtext(20, 25, 2,"Track-tech", ST77XX_WHITE);
  Drawtext(35, 42, 2,"Gameboy", ST77XX_WHITE);
  delay(3000);
  tft.fillScreen(ST77XX_BLACK);  //填充背景颜色（必须填充，否则之前的图像，将不会消失）
  Drawtext(20, 35, 1,"Designed by baichuan", ST77XX_WHITE);
  delay(3000);
  tft.fillScreen(ST77XX_BLACK);  //填充背景颜色（必须填充，否则之前的图像，将不会消失）
}

/*!
    @brief  游戏界面的积分栏
*/
void GAME_Menu(){

//绘制主体框架-游戏区-积分区（菜单区）,显示文字
  // tft.fillScreen(ST77XX_BLACK);  //填充背景颜色（必须填充，否则之前的图像，将不会消失）
  tft.drawRect(0, 0, 160, 80, ST77XX_WHITE);
  tft.drawLine(120, 0, 120, 80,ST77XX_WHITE);
  Drawtext(128, 3, 1, "Menu", ST77XX_WHITE);
  

}

/*!
    @brief  用于生成随机数
    @param  min      对生成随机数的最小值进行规定
    @param  max      对生成随机数的最大值进行规定
*/
int randomNuber(int min, int max){

  // char save[10];    //存储用

//初始化随机数发生器
  randomSeed(5);

// 生成随机数并返回

  return random(min, max);
//  while(1){

// //测试用
//     sprintf( save, "%d", random(0, 1000));   //导入数组
//     tft.fillScreen(ST77XX_BLACK);
//     Drawtext(10, 40, 1, save , ST77XX_WHITE);
//     delay(1000);

//   }

}


/*!
    @brief  区域清零函数
    @param  Num   清零区块选择
*/
void Space_Clean(int Num){
  // 游戏地图：120 x 80
  // 屏幕大小：160 x 80

  if (Num == 1)         //清除地图
  {
    tft.fillRect(1, 1, 118, 78, ST77XX_BLACK);  
  }
  else if (Num == 2)    //全清屏幕，除边框
  {
    tft.fillRect(1, 1, 158, 78, ST77XX_BLACK);  
  }
  
}



/*                测试用小球函数                */

void Ball_init(){

  head = (snake *)malloc(sizeof(snake));

  head->x = 80;
  head->y = 40;


}

void Ball_Move(){

  int dir = KEYScan();
  if(dir != 0 ){  
    switch(dir){
      case DIR_U: vy = -1;  vx = 0; break; //↑
      case DIR_D: vy =  1;  vx = 0; break; //↓
      case DIR_L: vx = -1;  vy = 0; break; //←
      case DIR_R: vx =  1;  vy = 0; break; //→
      default:break;
    }
    last_dir = dir;
  }

head->x+=vx;
head->y+=vy;

//中间量归零
vx = vy =0;

tft.drawPixel(head->x, head->y, ST77XX_WHITE); //绘制本次位置点

tft.drawPixel(head->x, head->y, ST77XX_WHITE); //删除上次位置点


}
