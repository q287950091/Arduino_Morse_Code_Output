#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp LCDO; //備註:此處不必填入記憶體位置
const char *Trans[][6] = {    //註冊摩斯密碼對應字串
  { ".", "_", 0 },            //A
  { "_", ".", ".", ".", 0 },  //B
  { "_", ".", "_", ".", 0 },  //C
  { "_", ".", ".", 0 },       //D
  { ".", 0 },                 //E
  { ".", ".", "_", ".", 0 },  //F
  { "_", "_", ".", 0 },       //G
  { ".", ".", ".", ".", 0 },  //H
  { ".", ".", 0 },            //I
  { ".", "_", "_", "_", 0 },  //J
  { "_", ".", "_", 0 },       //K
  { ".", "_", ".", ".", 0 },  //L
  { "_", "_", 0 },            //M
  { "_", ".", 0 },            //N
  { "_", "_", "_", 0 },       //O
  { ".", "_", "_", ".", 0 },  //P
  { "_", "_", ".", "_", 0 },  //Q
  { ".", "_", ".", 0 },       //R
  { ".", ".", ".", 0 },       //S
  { "_", 0 },                 //T
  { ".", ".", "_", 0 },       //U
  { ".", ".", ".", "_", 0 },  //V
  { ".", "_", "_", 0 },       //W
  { "_", ".", ".", "_", 0 },  //X
  { "_", ".", "_", "_", 0 },  //Y
  { "_", "_", ".", ".", 0 }   //Z
};
const char *Number[][7] = {        //註冊摩斯密碼對應字串
  { "_", "_", "_", "_", "_", 0 },  //0
  { ".", "_", "_", "_", "_", 0 },  //1
  { ".", ".", "_", "_", "_", 0 },  //2
  { ".", ".", ".", "_", "_", 0 },  //3
  { ".", ".", ".", ".", "_", 0 },  //4
  { ".", ".", ".", ".", ".", 0 },  //5
  { "_", ".", ".", ".", ".", 0 },  //6
  { "_", "_", ".", ".", ".", 0 },  //7
  { "_", "_", "_", ".", ".", 0 },  //8
  { "_", "_", "_", "_", ".", 0 }   //9
};
const char *Trans_Eng[] = { //輸入字串翻譯
  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
  "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
   "W", "X", "Y", "Z" };
int Input, WordCounter = 0, NumCheck = 0,
           SignalOutput = 13,  //蜂鳴器輸出腳位
           PauseTime = 100;    //點延遲時間
bool PlayBack = 0;
void setup() {
  Serial.begin(9600);    //啟動端口監聽
  pinMode(SignalOutput, OUTPUT); //蜂鳴器輸出
  LCDO.begin(16, 2);     //初始化LCD
  LCDO.setBacklight(1); 
}

void loop() {
  if (Serial.available() > 0) { //檢測串口狀態
    if (WordCounter >= 32) {    //LCD字符位置檢測
      WordCounter = 0;          //LCD字符位置重置
      LCDO.begin(16, 2);        //初始化LCD
      LCDO.setBacklight(1);
    }
    Input = Serial.read();      //獲取串口輸入
    if (Input == 46 || Input == 47 || Input == 95 || Input == 92) {  //排除特殊字符
      if (Input == 46) {        // . 字符
        convertor(0, 2);
        LCDO.print(".");
      }
      if (Input == 95) {        // _ 字符
        convertor(1, 2);
        LCDO.print("_");
      }
      if (Input != 47 && Input != 92) {  
        delay(PauseTime);
      } else {                  // "\" 和 "/" 字符表換行或LCD的空格
        delay(PauseTime * 4);
        Serial.print("\n");
        LCDO.print(" ");
      }
      goto end;  //跳脫迴圈
    }
    if (Input > 90) {
      Input = Input - 32;     //大小寫轉換
    } else if (Input < 65) {
      Input = Input - 48;
      if (Input >= 0) {
        NumCheck = 1;
        convertor(Input, 0);  //將ASCII數字轉換為數字
      } else {                //輸入錯誤回傳空格
        Serial.print(" \n");
        LCDO.print(" ");
        delay(PauseTime * 3);
      }
      goto end;  //跳脫迴圈
    }
    Input = Input - 65;  //將英文字符轉換為對應摩斯密碼
    convertor(Input, 1);
end:
    delay(PauseTime * 3);
    if (WordCounter < 32) WordCounter++;  //紀錄LCD字數
  }
  Input = 0, NumCheck = 0;  //重置輸入
  delay(1);                 //等待執行指令
}

void convertor(int IValue, int Modeset) {   //輸入轉摩斯密碼
  switch (Modeset) {     //轉換模式設定 0表數字 1表英文 2表例外 並且各自以不同頻率的聲音輸出
    case 0:
      for (int i = 0; Number[IValue][i] != 0; i++) {
        if (Number[IValue][i] == "_") {
          tone(SignalOutput, 440, PauseTime * 3);
          delay(PauseTime * 3);
          Serial.print("_");
        } else if (Number[IValue][i] == ".") {
          tone(SignalOutput, 440, PauseTime);
          delay(PauseTime);
          Serial.print(".");
        }
        delay(PauseTime);
      }
      if (PlayBack != 0) {
        Serial.println(IValue);
      } else {
        Serial.print("\n");
      }
      delay(PauseTime * 3);
      break;
    case 1:
      for (int i = 0; Trans[IValue][i] != 0; i++) {
        if (Trans[IValue][i] == "_") {
          tone(SignalOutput, 495, PauseTime * 3);
          delay(PauseTime * 3);
          Serial.print("_");
        } else if (Trans[IValue][i] == ".") {
          tone(SignalOutput, 495, PauseTime);
          delay(PauseTime);
          Serial.print(".");
        }
        delay(PauseTime);
      }
      if (PlayBack != 0) {
        Serial.println(Trans_Eng[IValue]);
      } else {
        Serial.print("\n");
      }
      delay(PauseTime * 3);
      break;
    case 2:
      if (IValue == 1) {
        tone(SignalOutput, 262, PauseTime * 3);
        delay(PauseTime * 3);
        Serial.print("_");
      } else if (IValue == 0) {
        tone(SignalOutput, 262, PauseTime);
        delay(PauseTime);
        Serial.print(".");
      }
      delay(PauseTime);
      break;
  }
  if (WordCounter <= 15 && Modeset != 2) {  //設置LCD顯示位置
    if (NumCheck != 1) {
      LCDO.setCursor(WordCounter, 0);
      LCDO.print(Trans_Eng[IValue]);
    } else {
      LCDO.print(IValue);
    }
  } else if (WordCounter >= 16 && WordCounter <= 32 && Modeset != 2) {
    LCDO.setCursor((WordCounter - 16), 1);
    if (NumCheck != 1) {
      LCDO.print(Trans_Eng[IValue]);
    } else {
      LCDO.print(IValue);
    }
  }
}