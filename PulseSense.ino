// #include "MPU6050Sensor.h"
// #include "BuzzerESP.h"

// // הגדרת הפין בו מחובר הבאזזר
// #define BUZZER_PIN 12
// BuzzerESP buzzer(BUZZER_PIN); // קריאה לבנאי עם פרמטר אחד בלבד
// MPU6050Sensor sensor;

// void setup() {
//   Serial.begin(9600);
//   buzzer.begin();
//   if (!sensor.begin()) {
//     Serial.println("Failed to initialize sensor!");
//     while (1);
//   }
//   Serial.println("Sensor initialized.");
//    Serial.println("מצפצף שלוש פעמים...");
//   for (int i = 0; i < 3; i++) {
//     buzzer.beep(500, 1500); // תדר 1500 הרץ, למשך 300 מ״ש
//     delay(200);
//   }
// }

// void loop() {
//   sensor.update();

//   if (sensor.detectFall(12.0)) {
//     Serial.println("Fall detected!");
//   }

//   if (sensor.detectNoMotion(3000)) {
//     Serial.println("No motion for 3 seconds");
//   }
//   else{
//     Serial.println("motion detection");
//   }
//   float motion = sensor.getMotionMagnitude();
//   Serial.print("Motion: ");
//   Serial.println(motion);
//  Serial.println("beeeeeep");
//  buzzer.beep(100, 1500); // תדר 1000 הרץ, 100 מ״ש
//   delay(5000);
// }
// // void loop() {
// //   Serial.println("צפצוף ארוך...");
// //   buzzer.on(1000);  // הפעל את הבאזר בתדר קבוע
// //   delay(2000);      // השאר מופעל למשך 2 שניות
// //   buzzer.off();     // כבה את הבאזר
// //   delay(2000);      // המתן 2 שניות לפני הצפצוף הבא
// // }


// #include <Wire.h>

// #define AM057_ADDRESS 0x48

// // קיבול מחושב לטמפרטורת גוף
// float BODY_CALIBRATION_OFFSET = 71.0;  // מתוקן על בסיס המדידות שלך

// void setup() {
//   Serial.begin(115200);
//   delay(2000);
  
//   Serial.println("🌡️ חיישן טמפרטורת גוף AM-057");
//   Serial.println("==============================");
  
//   Wire.begin(21, 22);
  
//   // בדיקה שהחיישן מגיב
//   Wire.beginTransmission(AM057_ADDRESS);
//   if (Wire.endTransmission() == 0) {
//     Serial.println("✅ חיישן מחובר ומוכן!");
//   } else {
//     Serial.println("❌ שגיאה: חיישן לא מגיב!");
//     while(1) delay(1000);
//   }
  
//   Serial.println("📋 הוראות שימוש:");
//   Serial.println("🤏 הצמדי את החיישן למצח למדידת טמפרטורת גוף");
//   Serial.println("⏱️ חכי 30-60 שניות להתייצבות");
//   Serial.println("🎯 טמפרטורה תקינה: 36.1-37.2°C");
//   Serial.println("");
  
//   // מדידה ראשונית
//   Serial.println("📊 מדידה ראשונית (ללא מגע):");
//   for (int i = 0; i < 3; i++) {
//     float temp = readBodyTemperature();
//     Serial.print("   ");
//     Serial.print(i + 1);
//     Serial.print(". ");
//     Serial.print(temp, 1);
//     Serial.println("°C");
//     delay(2000);
//   }
  
//   Serial.println("\n🎯 מוכן למדידות טמפרטורת גוף!");
//   Serial.println("הצמדי את החיישן למצח...\n");
// }

// void loop() {
//   float bodyTemp = readBodyTemperature();
  
//   if (bodyTemp != -999.0) {
//     // הדפסה עם סמלים
//     Serial.print("🌡️ טמפרטורת גוף: ");
//     Serial.print(bodyTemp, 1);
//     Serial.println("°C");
    
//     // הערכת הטמפרטורה
//     evaluateBodyTemperature(bodyTemp);
    
//     // מעקב אחר שינויים
//     trackTemperatureChanges(bodyTemp);
    
//   } else {
//     Serial.println("❌ שגיאה בקריאה!");
//   }
  
//   Serial.println("────────────────────");
//   delay(3000); // מדידה כל 3 שניות
// }

// float readBodyTemperature() {
//   // קריאת נתונים גולמיים
//   Wire.beginTransmission(AM057_ADDRESS);
//   Wire.write(0x00);
//   if (Wire.endTransmission(false) != 0) return -999.0;
  
//   if (Wire.requestFrom(AM057_ADDRESS, 2) != 2) return -999.0;
  
//   uint8_t msb = Wire.read();
//   uint8_t lsb = Wire.read();
//   uint16_t raw = (msb << 8) | lsb;
  
//   // המרה וקיבול
//   int16_t signed_raw = (int16_t)raw;
//   float rawTemp = signed_raw / 256.0;
//   float bodyTemp = rawTemp + BODY_CALIBRATION_OFFSET;
  
//   // הדפסת פרטים לאבחון (אופציונלי)
//   /*
//   Serial.print("🔍 גולמי: ");
//   Serial.print(rawTemp, 2);
//   Serial.print("°C → מכויל: ");
//   Serial.print(bodyTemp, 2);
//   Serial.print("°C | ");
//   */
  
//   return bodyTemp;
// }

// void evaluateBodyTemperature(float temp) {
//   if (temp >= 36.1 && temp <= 37.2) {
//     Serial.println("✅ טמפרטורת גוף תקינה");
//   } else if (temp >= 37.3 && temp <= 38.0) {
//     Serial.println("🟡 חום קל - מומלץ מעקב");
//   } else if (temp > 38.0 && temp <= 39.0) {
//     Serial.println("🔥 חום - יש להתייעץ עם רופא");
//   } else if (temp > 39.0 && temp <= 42.0) {
//     Serial.println("🚨 חום גבוה! פנייה מיידית לרופא");
//   } else if (temp < 36.1 && temp >= 35.0) {
//     Serial.println("❄️ טמפרטורה נמוכה מהרגיל");
//   } else if (temp < 35.0 && temp >= 25.0) {
//     Serial.println("🤷 לא במגע - הצמידי את החיישן למצח");
//   } else if (temp < 25.0) {
//     Serial.println("🧊 טמפרטורת חדר - החיישן לא במגע עם הגוף");
//   } else if (temp > 42.0) {
//     Serial.println("⚠️ ערך לא סביר - בדקי מיקום החיישן");
//   }
// }

// void trackTemperatureChanges(float currentTemp) {
//   static float lastTemp = 0;
//   static float maxTemp = 0;
//   static float minTemp = 50;
//   static int readingCount = 0;
  
//   readingCount++;
  
//   // עדכון מינימום ומקסימום
//   if (currentTemp > maxTemp) maxTemp = currentTemp;
//   if (currentTemp < minTemp) minTemp = currentTemp;
  
//   // זיהוי שינויים משמעותיים
//   if (lastTemp != 0) {
//     float change = currentTemp - lastTemp;
    
//     if (change > 0.5) {
//       Serial.print("📈 עלייה: +");
//       Serial.print(change, 1);
//       Serial.println("°C");
//     } else if (change < -0.5) {
//       Serial.print("📉 ירידה: ");
//       Serial.print(change, 1);
//       Serial.println("°C");
//     }
//   }
  
//   // דיווח סטטיסטיקות כל 20 מדידות
//   if (readingCount % 20 == 0) {
//     Serial.println("📊 סיכום 20 מדידות אחרונות:");
//     Serial.print("   טווח: ");
//     Serial.print(minTemp, 1);
//     Serial.print("-");
//     Serial.print(maxTemp, 1);
//     Serial.println("°C");
    
//     float range = maxTemp - minTemp;
//     if (range < 0.5) {
//       Serial.println("   ✅ יציבות מצוינת");
//     } else if (range < 1.0) {
//       Serial.println("   ✅ יציבות טובה");
//     } else {
//       Serial.println("   ⚠️ תנודות גדולות - בדקי מיקום");
//     }
    
//     // איפוס לחישוב הבא
//     maxTemp = currentTemp;
//     minTemp = currentTemp;
//   }
  
//   lastTemp = currentTemp;
// }

// // פונקציה לכיוון ידני מהיר
// void manualAdjustment() {
//   Serial.println("🔧 כיוון ידני:");
//   Serial.println("הצמידי את החיישן למצח והקש את הטמפרטורה הרצויה:");
  
//   while (!Serial.available()) delay(100);
//   float desiredTemp = Serial.parseFloat();
//   Serial.readString(); // נקה באפר
  
//   if (desiredTemp >= 35.0 && desiredTemp <= 39.0) {
//     // קריאה נוכחית
//     Wire.beginTransmission(AM057_ADDRESS);
//     Wire.write(0x00);
//     Wire.endTransmission(false);
//     Wire.requestFrom(AM057_ADDRESS, 2);
    
//     if (Wire.available() >= 2) {
//       uint8_t msb = Wire.read();
//       uint8_t lsb = Wire.read();
//       uint16_t raw = (msb << 8) | lsb;
//       int16_t signed_raw = (int16_t)raw;
//       float rawTemp = signed_raw / 256.0;
      
//       // חישוב קיבול חדש
//       BODY_CALIBRATION_OFFSET = desiredTemp - rawTemp;
      
//       Serial.print("✅ קיבול עודכן ל-");
//       Serial.print(BODY_CALIBRATION_OFFSET, 1);
//       Serial.println("°C");
      
//       Serial.print("🎯 טמפרטורה עכשיו: ");
//       Serial.print(desiredTemp, 1);
//       Serial.println("°C");
//     }
//   } else {
//     Serial.println("❌ טמפרטורה חייבת להיות בין 35-39°C");
//   }
// }
/// טמפרטורה עובד


// #include <Wire.h>

// #define AM057_ADDRESS 0x48

// // קיבול ידני - התאם את הערכים האלה!
// float CALIBRATION_OFFSET = 55.0;  // מוסיף לתוצאה (משנה לפי הצורך)
// float CALIBRATION_SCALE = 1.0;    // מכפיל (אם צריך)

// void setup() {
//   Serial.begin(115200);
//   Serial.println("🌡️ חיישן AM-057 עם קיבול ידני");
//   Serial.println("==================================");
  
//   Wire.begin(21, 22);
//   Wire.setClock(100000);
//   delay(1000);
  
//   // בדיקה שהחיישן מגיב
//   Wire.beginTransmission(AM057_ADDRESS);
//   if (Wire.endTransmission() == 0) {
//     Serial.println("✅ חיישן AM-057 זוהה!");
//   } else {
//     Serial.println("❌ חיישן לא מגיב!");
//     while(1);
//   }
  
//   Serial.println("🔧 הוראות קיבול:");
//   Serial.println("1. הניח את החיישן בטמפרטורת חדר (~23°C)");
//   Serial.println("2. רשום מה הטמפרטורה האמיתית בחדר");
//   Serial.println("3. עדכן את CALIBRATION_OFFSET בקוד");
//   Serial.println("4. לטמפרטורת גוף - הניח אצבע על החיישן");
//   Serial.println();
  
//   calibrationMode();
  
//   Serial.println("מתחיל מדידות רגילות...\n");
// }

// void loop() {
//   float rawTemp = readRawTemperature();
  
//   if (rawTemp != -999.0) {
//     // קיבול ידני
//     float calibratedTemp = (rawTemp * CALIBRATION_SCALE) + CALIBRATION_OFFSET;
    
//     Serial.print("🌡️ גולמי: ");
//     Serial.print(rawTemp, 2);
//     Serial.print("°C → מכויל: ");
//     Serial.print(calibratedTemp, 2);
//     Serial.println("°C");
    
//     // הערכת הטמפרטורה
//     evaluateTemperature(calibratedTemp);
    
//     // רמז לקיבול
//     if (abs(calibratedTemp - 23.0) > 5.0) {
//       Serial.print("💡 עדכן CALIBRATION_OFFSET ל-");
//       Serial.print(CALIBRATION_OFFSET + (23.0 - calibratedTemp), 1);
//       Serial.println(" לטמפרטורת חדר 23°C");
//     }
    
//   } else {
//     Serial.println("❌ שגיאה בקריאת טמפרטורה!");
//   }
  
//   Serial.println("----------------------------");
//   delay(2000);
// }

// float readRawTemperature() {
//   // קריאת רגיסטר טמפרטורה
//   Wire.beginTransmission(AM057_ADDRESS);
//   Wire.write(0x00);
//   if (Wire.endTransmission(false) != 0) {
//     return -999.0;
//   }
  
//   if (Wire.requestFrom(AM057_ADDRESS, 2) != 2) {
//     return -999.0;
//   }
  
//   uint8_t msb = Wire.read();
//   uint8_t lsb = Wire.read();
//   uint16_t raw = (msb << 8) | lsb;
  
//   // הדפסת נתונים גולמיים
//   Serial.print("📊 גולמי: 0x");
//   if (raw < 0x1000) Serial.print("0");
//   if (raw < 0x100) Serial.print("0");
//   if (raw < 0x10) Serial.print("0");
//   Serial.print(raw, HEX);
//   Serial.print(" → ");
  
//   // המרה לפי הפורמט שנראה הכי טוב (LM75-like)
//   int16_t signed_raw = (int16_t)raw;
//   float temperature = signed_raw / 256.0;
  
//   return temperature;
// }

// void evaluateTemperature(float temp) {
//   if (temp >= 20.0 && temp <= 26.0) {
//     Serial.println("🏠 טמפרטורת חדר נוחה");
//   } else if (temp > 26.0 && temp <= 30.0) {
//     Serial.println("🔥 חם בחדר");
//   } else if (temp > 30.0 && temp <= 42.0) {
//     Serial.println("🤏 טמפרטורת גוף - מגע ישיר?");
//   } else if (temp < 20.0 && temp >= 10.0) {
//     Serial.println("❄️ קר בחדר");
//   } else if (temp < 10.0) {
//     Serial.println("🧊 קר מאוד!");
//   } else if (temp > 45.0) {
//     Serial.println("🔥 חם מדי - אולי טעות בקיבול");
//   }
// }

// void calibrationMode() {
//   Serial.println("🎯 מצב קיבול - 10 מדידות:");
  
//   float sum = 0;
//   int validReadings = 0;
  
//   for (int i = 0; i < 10; i++) {
//     float temp = readRawTemperature();
//     if (temp != -999.0) {
//       sum += temp;
//       validReadings++;
//       Serial.print("   ");
//       Serial.print(i + 1);
//       Serial.print(". ");
//       Serial.print(temp, 2);
//       Serial.println("°C");
//     }
//     delay(500);
//   }
  
//   if (validReadings > 0) {
//     float average = sum / validReadings;
//     Serial.print("\n📊 ממוצע: ");
//     Serial.print(average, 2);
//     Serial.println("°C");
    
//     Serial.print("💡 אם טמפרטורת החדר היא 23°C, ");
//     Serial.print("עדכן CALIBRATION_OFFSET ל-");
//     Serial.println(23.0 - average, 1);
    
//     Serial.print("💡 אם טמפרטורת החדר היא 25°C, ");
//     Serial.print("עדכן CALIBRATION_OFFSET ל-");
//     Serial.println(25.0 - average, 1);
//   }
  
//   Serial.println();
// }

// // פונקציה לקיבול מתקדם עם שתי נקודות
// void advancedCalibration() {
//   Serial.println("🎯 קיבול מתקדם עם שתי נקודות:");
//   Serial.println("1. הניח את החיישן בטמפרטורת חדר");
//   Serial.println("2. לחץ כל מקש ו-Enter");
  
//   while (!Serial.available()) delay(100);
//   Serial.readString(); // נקה את הבאפר
  
//   float temp1 = readRawTemperature();
//   Serial.print("נקודה 1 (חדר): ");
//   Serial.print(temp1, 2);
//   Serial.println("°C");
  
//   Serial.println("הקש את הטמפרטורה האמיתית של החדר ו-Enter:");
//   while (!Serial.available()) delay(100);
//   float realTemp1 = Serial.parseFloat();
  
//   Serial.println("2. הניח אצבע על החיישן למשך 30 שניות");
//   Serial.println("לחץ כל מקש ו-Enter כשמוכן");
  
//   while (!Serial.available()) delay(100);
//   Serial.readString();
  
//   float temp2 = readRawTemperature();
//   Serial.print("נקודה 2 (אצבע): ");
//   Serial.print(temp2, 2);
//   Serial.println("°C");
  
//   Serial.println("הקש את הטמפרטורה האמיתית של האצבע (בדרך כלל 36-37°C):");
//   while (!Serial.available()) delay(100);
//   float realTemp2 = Serial.parseFloat();
  
//   // חישוב הקיבול
//   float scale = (realTemp2 - realTemp1) / (temp2 - temp1);
//   float offset = realTemp1 - (temp1 * scale);
  
//   Serial.println("\n🎯 תוצאות קיבול:");
//   Serial.print("CALIBRATION_SCALE = ");
//   Serial.println(scale, 4);
//   Serial.print("CALIBRATION_OFFSET = ");
//   Serial.println(offset, 2);
  
//   Serial.println("עדכן את הערכים בקוד ותעלה מחדש!");
// }

// #include <Wire.h>

// #define MAX30105_ADDRESS 0x57
// #define REG_PART_ID 0xFF

// void setup() {
//   Serial.begin(115200);
//   delay(2000);
//   Serial.println("MAX30102 Debug - Step by Step");
  
//   Serial.println("Step 1: Starting I2C...");
//   Wire.begin();
//   Serial.println("Step 1: I2C started ✓");
  
//   Serial.println("Step 2: Testing connection...");
//   Wire.beginTransmission(MAX30105_ADDRESS);
//   byte error = Wire.endTransmission();
  
//   if (error == 0) {
//     Serial.println("Step 2: Connection OK ✓");
//   } else {
//     Serial.println("Step 2: Connection FAILED ✗");
//     Serial.print("Error code: ");
//     Serial.println(error);
//   }
  
//   Serial.println("Step 3: Reading Part ID...");
//   byte partID = readPartID();
//   Serial.print("Step 3: Part ID = 0x");
//   Serial.println(partID, HEX);
  
//   if (partID == 0x15) {
//     Serial.println("Step 3: Part ID correct ✓");
//   } else {
//     Serial.println("Step 3: Part ID wrong ✗");
//   }
  
//   Serial.println("Step 4: Setup complete, starting loop...");
// }

// void loop() {
//   Serial.println("Loop running...");
//   delay(2000);
// }

// byte readPartID() {
//   Wire.beginTransmission(MAX30105_ADDRESS);
//   Wire.write(REG_PART_ID);
//   byte error = Wire.endTransmission(false);
  
//   if (error != 0) {
//     Serial.print("Write error: ");
//     Serial.println(error);
//     return 0;
//   }
  
//   Wire.requestFrom(MAX30105_ADDRESS, 1);
//   if (Wire.available()) {
//     return Wire.read();
//   } else {
//     Serial.println("No data available");
//     return 0;
//   }
// }

//חיפוש מכשירים בI2C
// #include <Wire.h>

// void setup() {
//   Serial.begin(115200);
//   delay(3000);
//   Serial.println("=== Basic I2C Test - Back to Basics ===");
  
//   Wire.begin();
//   Serial.println("I2C initialized");
  
//   scanI2C();
// }

// void loop() {
//   delay(5000);
//   Serial.println("Scanning again...");
//   scanI2C();
// }

// void scanI2C() {
//   byte error, address;
//   int nDevices = 0;
  
//   Serial.println("Scanning I2C addresses...");
  
//   for(address = 1; address < 127; address++) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();
    
//     if (error == 0) {
//       Serial.print("Device found at 0x");
//       if (address < 16) Serial.print("0");
//       Serial.print(address, HEX);
      
//       if (address == 0x57) {
//         Serial.print(" <- MAX30102!");
//       }
//       Serial.println();
//       nDevices++;
//     }
//   }
  
//   if (nDevices == 0) {
//     Serial.println("❌ NO I2C devices found!");
//     Serial.println("This means:");
//     Serial.println("1. Wiring problem");
//     Serial.println("2. Power problem"); 
//     Serial.println("3. Sensor damaged");
//   } else {
//     Serial.print("✅ Found ");
//     Serial.print(nDevices);
//     Serial.println(" device(s)");
//   }
//   Serial.println("------------------------");
// }