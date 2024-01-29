#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// khai báo chân IO
#define RELAY 27
#define VIN   12
#define IIN   14
#define NTC   13

float v,is,temp;

float i0 = 2.5;   // khi dòng 0A, điện áp ACS712 ra 2.5V

// nhiệt độ từ 5-100 độ C
float res[20] = { 25.58, 20.00, 15.76, 12.51, 10.00, 8.048, 6.518, 5.312, 4.354, 3.588,
                2.974, 2.476, 2.072, 1.743, 1.473, 1.250, 1.065, 0.911, 0.7824, 0.6744};
int adcTemp[20];
int adcT;
int sac = 0;

void setup()
{
  pinMode(RELAY, OUTPUT);
//  digitalWrite(RELAY, HIGH);
//  delay(100);
  digitalWrite(RELAY, LOW); // Ngắt sạc
  
  Serial.begin(9600);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hi!");
  delay(1000);
  
  for(int i=0; i<20; i++){
    adcTemp[i] = res[i]/(res[i]+10)*1.515*4095;
    Serial.print("adc");
    Serial.print(i);
    Serial.print(" =");
    Serial.println(adcTemp[i]);
    
  }
}


void loop(){
  // đo adc
  v = analogRead(VIN);
  is = analogRead(IIN);
  adcT = analogRead(NTC);
  Serial.println(v);
  Serial.println(is);
  Serial.println(adcT);
  // Tính toán áp, dòng, nhiệt độ
  v = v/4095*3.3*2;
  is = is/4095*3.3*1.5;
  Serial.println(is);
  if(is>i0)
    is=(is-i0)/i0*20;
    else  is=(i0-is)/i0*20;
  for(int i=0; i<20; i++){
    if(adcT> adcTemp[i+1]){
      temp = 5*(i+1) + 5*(adcT*1.0 - adcTemp[i])/(adcTemp[i+1] - adcTemp[i]);
      break;
    }
  }
  Serial.println(v);
  Serial.println(is);
  Serial.println(temp);
  
  // màn hình hiển thị
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(v);
  lcd.print("V");
  lcd.setCursor(9,0);
  lcd.print(is);
  lcd.print("A");
  lcd.setCursor(9,1);
  lcd.print(temp);
  lcd.write(223);
  lcd.print("C");
  
// Điều khiển
// cho sạc
  if( sac == 0 && v < 4.2 && temp<40.0 && is<=10.0){
    digitalWrite(RELAY, HIGH);
    sac = 1;
  }
  // sạc đầy 4.2V
  if(v >= 4.2){
    digitalWrite(RELAY, LOW);
    lcd.setCursor(0,1);
    lcd.print("Full!");
    while(1);
  }
  // quá nhiệt 50 độ
  if(temp>50.0){
    digitalWrite(RELAY, LOW);
    lcd.setCursor(0,1);
    lcd.print("E temp!");
    while(1);
  }
  // quá dòng 10A
  if(is > 10.0){
    digitalWrite(RELAY, LOW);
    lcd.setCursor(0,1);
    lcd.print("E I!");
    while(1);
  }   
  delay(1000);
}
