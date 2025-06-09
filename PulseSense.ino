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


//דופק מזהה מגע - אני לא יודעת מה בקשר לדופק עצמו
// #include <Wire.h>

// #define MAX30105_ADDRESS 0x57

// // הגדרות שנוכל לשנות
// uint8_t ledPower = 0x1F;  // עוצמת LEDs (0x00-0x3F)
// uint8_t sampleRate = 0x27; // מהירות דגימה

// void setup() {
//   Serial.begin(115200);
//   delay(2000);
  
//   Serial.println("🔧 MAX30102 - Debug וכיוון");
//   Serial.println("============================");
  
//   Wire.begin(21, 22);
  
//   initSensor();
  
//   Serial.println("📋 פקודות:");
//   Serial.println("+ = הגבר עוצמת LEDs");
//   Serial.println("- = הקטן עוצמת LEDs"); 
//   Serial.println("r = איפוס הגדרות");
//   Serial.println("c = מצב כיוון");
//   Serial.println();
// }

// void loop() {
//   // בדיקת פקודות מהמשתמש
//   if (Serial.available()) {
//     char cmd = Serial.read();
//     handleCommand(cmd);
//     return;
//   }
  
//   // קריאת נתונים
//   uint32_t red, ir;
//   if (readSensorData(&red, &ir)) {
    
//     // הדפסה עם גרף פשוט
//     Serial.print("🔴 IR: ");
//     Serial.print(ir);
//     Serial.print(" | ❤️ Red: ");
//     Serial.print(red);
    
//     // גרף פשוט
//     Serial.print(" | ");
//     printBar(ir, 100000);
    
//     // זיהוי מגע דינמי
//     static uint32_t baseline = 1000;
//     static int readingCount = 0;
    
//     // עדכון baseline (רק כשאין מגע)
//     if (ir < baseline * 1.5) {
//       baseline = (baseline * 9 + ir) / 10; // ממוצע נע
//     }
    
//     uint32_t threshold = baseline * 2; // סף דינמי
    
//     if (ir > threshold && ir > 5000) {
//       Serial.print(" 🤚 מגע!");
      
//       // ספירת דופק פשוט
//       static uint32_t lastIR = 0;
//       static unsigned long lastBeat = 0;
//       static int beatCount = 0;
      
//       if (ir > lastIR + (ir * 0.1)) { // עלייה של 10%
//         unsigned long now = millis();
//         if (now - lastBeat > 400 && now - lastBeat < 2000) { // 30-150 BPM
//           beatCount++;
//           if (beatCount >= 3) { // לפחות 3 דפיקות לחישוב
//             int bpm = 60000 / (now - lastBeat);
//             Serial.print(" 💓 ");
//             Serial.print(bpm);
//             Serial.print(" BPM");
//           }
//           lastBeat = now;
//         }
//       }
//       lastIR = ir;
      
//     } else {
//       Serial.print(" ✋ אין מגע");
//     }
    
//     // הצגת baseline וסף
//     Serial.print(" (סף: ");
//     Serial.print(threshold);
//     Serial.print(", baseline: ");
//     Serial.print(baseline);
//     Serial.println(")");
    
//     readingCount++;
    
//     // כל 50 מדידות - הצגת סטטיסטיקות
//     if (readingCount % 50 == 0) {
//       Serial.println("📊 טיפים:");
//       if (ir < 5000) {
//         Serial.println("   💡 ערכים נמוכים - נסי '+' להגברת LEDs");
//       }
//       if (ir > 200000) {
//         Serial.println("   💡 ערכים גבוהים - נסי '-' להקטנת LEDs");
//       }
//       Serial.println();
//     }
    
//   } else {
//     Serial.println("❌ שגיאה בקריאה");
//   }
  
//   delay(100);
// }

// void handleCommand(char cmd) {
//   switch(cmd) {
//     case '+':
//       if (ledPower < 0x3F) {
//         ledPower += 0x05;
//         updateLEDPower();
//         Serial.print("💡 עוצמת LEDs: 0x");
//         Serial.println(ledPower, HEX);
//       }
//       break;
      
//     case '-':
//       if (ledPower > 0x05) {
//         ledPower -= 0x05;
//         updateLEDPower();
//         Serial.print("💡 עוצמת LEDs: 0x");
//         Serial.println(ledPower, HEX);
//       }
//       break;
      
//     case 'r':
//       Serial.println("🔄 איפוס הגדרות...");
//       ledPower = 0x1F;
//       initSensor();
//       break;
      
//     case 'c':
//       calibrationMode();
//       break;
//   }
// }

// void calibrationMode() {
//   Serial.println("🎯 מצב כיוון:");
//   Serial.println("1. הרם את האצבע מהחיישן");
//   Serial.println("2. לחץ Enter כשמוכן");
  
//   while (!Serial.available()) delay(100);
//   Serial.readString();
  
//   // מדידת baseline
//   Serial.println("📊 מודד baseline (ללא מגע)...");
//   uint32_t baselineSum = 0;
//   int validReadings = 0;
  
//   for (int i = 0; i < 20; i++) {
//     uint32_t red, ir;
//     if (readSensorData(&red, &ir)) {
//       baselineSum += ir;
//       validReadings++;
//       Serial.print(".");
//     }
//     delay(100);
//   }
  
//   if (validReadings > 0) {
//     uint32_t baseline = baselineSum / validReadings;
//     Serial.println();
//     Serial.print("📊 Baseline: ");
//     Serial.println(baseline);
    
//     Serial.println("3. עכשיו הנח אצבע על החיישן ולחץ Enter");
//     while (!Serial.available()) delay(100);
//     Serial.readString();
    
//     // מדידת אצבע
//     Serial.println("📊 מודד עם אצבע...");
//     uint32_t fingerSum = 0;
//     validReadings = 0;
    
//     for (int i = 0; i < 20; i++) {
//       uint32_t red, ir;
//       if (readSensorData(&red, &ir)) {
//         fingerSum += ir;
//         validReadings++;
//         Serial.print(".");
//       }
//       delay(100);
//     }
    
//     if (validReadings > 0) {
//       uint32_t finger = fingerSum / validReadings;
//       Serial.println();
//       Serial.print("📊 עם אצבע: ");
//       Serial.println(finger);
      
//       float ratio = (float)finger / baseline;
//       Serial.print("📊 יחס: ");
//       Serial.println(ratio, 2);
      
//       if (ratio > 2.0) {
//         Serial.println("✅ הכיוון טוב!");
//       } else if (ratio > 1.5) {
//         Serial.println("⚠️ נסי להגביר LEDs (+)");
//       } else {
//         Serial.println("❌ צריך להגביר LEDs משמעותית");
//       }
//     }
//   }
  
//   Serial.println("🏁 סיום כיוון\n");
// }

// bool readSensorData(uint32_t *red, uint32_t *ir) {
//   Wire.beginTransmission(MAX30105_ADDRESS);
//   Wire.write(0x07);
  
//   if (Wire.endTransmission(false) != 0) return false;
  
//   if (Wire.requestFrom(MAX30105_ADDRESS, 6) < 6) return false;
  
//   *red = 0;
//   *red |= (uint32_t)Wire.read() << 16;
//   *red |= (uint32_t)Wire.read() << 8;
//   *red |= Wire.read();
//   *red &= 0x3FFFF;
  
//   *ir = 0;
//   *ir |= (uint32_t)Wire.read() << 16;
//   *ir |= (uint32_t)Wire.read() << 8;
//   *ir |= Wire.read();
//   *ir &= 0x3FFFF;
  
//   return true;
// }

// void initSensor() {
//   writeRegister(0x09, 0x40); // Reset
//   delay(100);
//   writeRegister(0x09, 0x03); // HR + SpO2
//   writeRegister(0x0A, sampleRate);
//   updateLEDPower();
  
//   Serial.print("✅ חיישן אותחל | עוצמת LEDs: 0x");
//   Serial.println(ledPower, HEX);
// }

// void updateLEDPower() {
//   writeRegister(0x0C, ledPower); // Red LED
//   writeRegister(0x0D, ledPower); // IR LED
// }

// void writeRegister(byte reg, byte value) {
//   Wire.beginTransmission(MAX30105_ADDRESS);
//   Wire.write(reg);
//   Wire.write(value);
//   Wire.endTransmission();
// }

// void printBar(uint32_t value, uint32_t maxVal) {
//   int bars = (value * 20) / maxVal;
//   if (bars > 20) bars = 20;
  
//   for (int i = 0; i < bars; i++) {
//     Serial.print("█");
//   }
//   for (int i = bars; i < 20; i++) {
//     Serial.print("░");
//   }
// }



#include <Wire.h>

#define MAX30105_ADDRESS 0x57

// משתנים פשוטים
uint8_t ledPower = 0x1F;
uint32_t lastIR = 0, lastRed = 0;
bool dataChanging = false;
int unchangedCount = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("💓 MAX30102 - גישה פשוטה");
  Serial.println("==========================");
  
  Wire.begin(21, 22);
  
  // איפוס ואתחול פשוט
  Serial.println("🔧 מאתחל...");
  
  writeRegister(0x09, 0x40); // Reset
  delay(200);
  writeRegister(0x09, 0x03); // HR + SpO2 mode
  writeRegister(0x0A, 0x27); // Settings
  writeRegister(0x0C, ledPower); // Red LED
  writeRegister(0x0D, ledPower); // IR LED
  
  Serial.println("✅ מוכן!");
  Serial.println("📊 צפי בנתונים:");
  Serial.println("   ערכים נמוכים (מתחת ל-30000) = אין מגע");
  Serial.println("   ערכים גבוהים (מעל 50000) = יש מגע");
  Serial.println();
  Serial.println("📋 פקודות: + הגבר | - הקטן | r איפוס");
  Serial.println();
}

void loop() {
  // בדיקת פקודות
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == '+' && ledPower < 0x3F) {
      ledPower += 0x08;
      updateLEDs();
      Serial.print("💡 עוצמה: 0x");
      Serial.println(ledPower, HEX);
    }
    else if (cmd == '-' && ledPower > 0x08) {
      ledPower -= 0x08;
      updateLEDs();
      Serial.print("💡 עוצמה: 0x");
      Serial.println(ledPower, HEX);
    }
    else if (cmd == 'r') {
      Serial.println("🔄 איפוס...");
      resetSensor();
    }
    return;
  }
  
  // קריאת נתונים פשוטה
  uint32_t red, ir;
  if (readData(&red, &ir)) {
    
    // בדיקה אם הנתונים משתנים
    if (ir == lastIR && red == lastRed) {
      unchangedCount++;
    } else {
      unchangedCount = 0;
      dataChanging = true;
    }
    
    // הצגת נתונים
    Serial.print("IR: ");
    Serial.print(ir);
    Serial.print(" | Red: ");
    Serial.print(red);
    
    // בדיקה אם הנתונים תקועים
    if (unchangedCount > 10) {
      Serial.print(" | ⚠️ נתונים תקועים!");
      if (unchangedCount > 20) {
        Serial.println(" - עושה איפוס...");
        resetSensor();
        return;
      }
    } else {
      Serial.print(" | ✅ משתנה");
    }
    
    // זיהוי מגע פשוט
    Serial.print(" | ");
    if (ir > 50000) {
      Serial.print("🤚 מגע חזק");
    } else if (ir > 30000) {
      Serial.print("👆 מגע קל");
    } else if (ir > 15000) {
      Serial.print("🤏 מגע חלש");
    } else {
      Serial.print("✋ אין מגע");
    }
    
    // זיהוי דופק פשוט מאוד
    static uint32_t lastValue = 0;
    static unsigned long lastTime = 0;
    
    if (ir > 40000 && ir > lastValue + 5000) { // עלייה משמעותי
      unsigned long now = millis();
      if (now - lastTime > 400 && now - lastTime < 1500) {
        int bpm = 60000 / (now - lastTime);
        if (bpm > 50 && bpm < 150) {
          Serial.print(" | 💓 ");
          Serial.print(bpm);
          Serial.print(" BPM");
        }
      }
      lastTime = now;
    }
    lastValue = ir;
    
    Serial.println();
    
    // שמירת ערכים קודמים
    lastIR = ir;
    lastRed = red;
    
  } else {
    Serial.println("❌ שגיאה בקריאה");
    unchangedCount++;
    
    if (unchangedCount > 30) {
      Serial.println("🔄 איפוס בגלל שגיאות...");
      resetSensor();
    }
  }
  
  delay(200); // 5 מדידות לשנייה
}

bool readData(uint32_t *red, uint32_t *ir) {
  // קריאה ישירה וברורה
  Wire.beginTransmission(MAX30105_ADDRESS);
  Wire.write(0x07); // FIFO Data
  
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  
  int received = Wire.requestFrom(MAX30105_ADDRESS, 6);
  if (received < 6) {
    return false;
  }
  
  // קריאת Red (3 bytes ראשונים)
  *red = 0;
  *red |= (uint32_t)Wire.read() << 16;
  *red |= (uint32_t)Wire.read() << 8;
  *red |= Wire.read();
  *red &= 0x3FFFF; // 18 bit mask
  
  // קריאת IR (3 bytes אחרונים)
  *ir = 0;
  *ir |= (uint32_t)Wire.read() << 16;
  *ir |= (uint32_t)Wire.read() << 8;
  *ir |= Wire.read();
  *ir &= 0x3FFFF; // 18 bit mask
  
  return true;
}

void resetSensor() {
  Serial.println("   🔄 מאפס החיישן...");
  
  // איפוס I2C
  Wire.end();
  delay(100);
  Wire.begin(21, 22);
  delay(100);
  
  // איפוס החיישן
  writeRegister(0x09, 0x40); // Reset bit
  delay(200);
  
  // הגדרות מחדש
  writeRegister(0x09, 0x03); // Mode
  writeRegister(0x0A, 0x27); // Settings
  updateLEDs();
  
  // איפוס משתנים
  lastIR = 0;
  lastRed = 0;
  unchangedCount = 0;
  
  Serial.println("   ✅ איפוס הושלם");
}

void updateLEDs() {
  writeRegister(0x0C, ledPower); // Red LED
  writeRegister(0x0D, ledPower); // IR LED
}

void writeRegister(byte reg, byte value) {
  Wire.beginTransmission(MAX30105_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}