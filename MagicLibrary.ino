#include <Adafruit_NeoPixel.h>
#include <Stepper_28BYJ_48.h>

#define sensorP1 5
#define sensorP2 4
#define sensorP3 3
#define sensorP4 2

#define candle A1
#define candleVol 255 //キャンドルの明るさ(電圧値0~255)

#define sphereL 40
#define sphereR 41

#define clockL1 8
#define clockL2 9
#define clockL3 10
#define clockL4 11
Stepper_28BYJ_48 myStepper0(clockL1,clockL2,clockL3,clockL4);

#define NeoPixPin 13
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(4, NeoPixPin, NEO_GRB + NEO_KHZ800);

bool stateP1 = false;
bool stateP2 = false;
bool stateP3 = false;
bool stateP4 = false;
int currentPage = 0;
int openPageNum = 0;
int prePage = 0;
int randomClockSpeed = 0;
int actionsArray[4] = {0};
int colorArray[3][3] = {{0,255,255},{255,0,255},{255,0,0}};
int randomColorNum = 0;

void setup() {
  Serial.begin(9600);

  pinMode(sensorP1,INPUT);
  pinMode(sensorP2,INPUT);
  pinMode(sensorP3,INPUT);
  pinMode(sensorP4,INPUT);
  
  pinMode(sphereL,OUTPUT);
  pinMode(sphereR,OUTPUT);

  pinMode(candle,OUTPUT); 

  pixels.begin();
  pixels.clear();
  pixels.show();
}

void loop() {
  //本のセンサー状態の読み取り
  ReadOpenPage();
  
  //センサー起動時にアクションを変更
  if(prePage != currentPage && prePage == 0){
    ShuffleActions(currentPage);
    randomColorNum = random(3);    
  }
  
  //アクションの切り替え
  SwitchAction();

  DebugActionArray();

  prePage = currentPage;
}

void SwitchAction(){
  switch(currentPage){
    case 0: //本を閉じたとき
      Serial.println("Closed");
      analogWrite(candle,0);  //キャンドル消灯
      SetSphereSignal(0,0);
      TurnOffPixels();
      break;

    case 1: //1ページ目
      SelectActions();    
      Serial.print("Page1");
      break;

    case 2: //2ページ目
      SelectActions();
      Serial.print("Page2");
      break;

    case 3: //3ページ目
      SelectActions();
      Serial.print("Page3");
      break;

    case 4: //4ページ目
      SelectActions();
      Serial.print("Page4");        
      break;
    
    default:
    break;
  }
}

void SelectActions(){
  switch(actionsArray[currentPage-1]){
    case 4: //時計順回転
      if(prePage != currentPage){
        randomSeed(analogRead(0)); // 乱数の初期化
        randomClockSpeed = random(2); //時計の回転方向の乱数を設定
      }
      DoStepClock(randomClockSpeed == 1 ? 1:-1);
      SetSphereSignal(0,0); 
      TurnOffPixels();
      Serial.println("Action 1");      
      break;
    case 2: //魔法陣
      TurnOnPixels();
      SetSphereSignal(0,0);
      Serial.println("Action 2"); 
      break;
    case 3: //スフィアへの信号出力
      SetSphereSignal(1,1);
      TurnOffPixels();
      Serial.println("Action 3"); 
      break;
    case 1: //キャンドル．
      SetSphereSignal(0,0);
      analogWrite(candle,candleVol); //キャンドル点灯
      TurnOffPixels();
      Serial.println("Action candle"); 
      break;
    default:  
    break;
  }
}

void ReadOpenPage(){
  
  stateP1 = digitalRead(sensorP1);
  stateP2 = digitalRead(sensorP2);
  stateP3 = digitalRead(sensorP3);
  stateP4 = digitalRead(sensorP4);

  openPageNum = 0;
  if(stateP1) openPageNum++;
  if(stateP2) openPageNum++;
  if(stateP3) openPageNum++;
  if(stateP4) openPageNum++;

  if(openPageNum == 1){
    if(stateP1) currentPage = 1;
    else if(stateP2) currentPage = 2;
    else if(stateP3) currentPage = 3;
    else if(stateP4) currentPage = 4;
  } 
  else if(openPageNum == 0){
    currentPage = 0;
  } 
}

void ShuffleActions(int firstOpenedPage) {
  
  for (int i = 0; i < 4; i ++) {  //配列初期化
    actionsArray[i] = 0;  
  }
  actionsArray[firstOpenedPage-1] = 4;  //最初に開いたページにキャンドル点灯をセット

  randomSeed(analogRead(0)); // 乱数の初期化
  int idx; // ランダムな位置
  for (int i = 0; i < 3; i++) { //キャンドル以外のアクションをランダムにセット
    do {
      idx = random(4); // 0〜3の乱数を生成
    } while (actionsArray[idx] != 0); // すでに数字が入っている場所はスキップ
    actionsArray[idx] = 1 + i; // アクションナンバーをセット
  }
}


//天球作品への信号処理
void SetSphereSignal(int ValueL , int ValueR){
  digitalWrite(sphereL,ValueL);
  digitalWrite(sphereR,ValueR);   
}

//時計作品への信号処理
void DoStepClock(int speed){  
  myStepper0.step(speed);

  digitalWrite(8,  LOW);
  digitalWrite(9,  LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}

//魔法陣作品への信号処理
void TurnOnPixels(){
  for(int i=0; i<4; i++) {
    pixels.setBrightness(255);
    pixels.setPixelColor(i, pixels.Color(colorArray[randomColorNum][0],colorArray[randomColorNum][1],colorArray[randomColorNum][2])); //大255,0,0　中255,0,255　　小0,255,255
  }
  pixels.show();
}

void TurnOffPixels(){
  pixels.clear();
  pixels.show();
}

void DebugActionArray(){
  Serial.print("  ");
  Serial.print(actionsArray[0]);
  Serial.print(",");
  Serial.print(actionsArray[1]);
  Serial.print(",");
  Serial.print(actionsArray[2]);
  Serial.print(",");
  Serial.print(actionsArray[3]);
  Serial.print("  ");  
}