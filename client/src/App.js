import React, { useState, useRef } from 'react';
import { Bluetooth, Heart, Activity, Wifi, WifiOff, Thermometer } from 'lucide-react';

const SimplePulseSenseApp = () => {
  const [connected, setConnected] = useState(false);
  const [data, setData] = useState({
    motion: 0,
    pulse: 0,
    status: 'אין מגע',
    temperature: null,
    temp_status: 'לא זמין'
  });
  const [alerts, setAlerts] = useState([]);
  const [connecting, setConnecting] = useState(false);

  const deviceRef = useRef(null);
  const dataCharacteristicRef = useRef(null);
  const commandCharacteristicRef = useRef(null);

  // בדיקה אם הדפדפן תומך ב-Bluetooth
  const isBluetoothSupported = () => {
    return navigator.bluetooth && typeof navigator.bluetooth.requestDevice === 'function';
  };

  // התחברות מפושטת ל-BLE
  const connect = async () => {
    if (!isBluetoothSupported()) {
      alert('הדפדפן לא תומך ב-Bluetooth. אנא השתמש ב-Chrome');
      return;
    }

    setConnecting(true);

    try {
      console.log('מחפש מכשירים BLE...');

      // בקש מכשיר BLE
      const device = await navigator.bluetooth.requestDevice({
        filters: [{
          namePrefix: 'PulseSense'
        }],
        optionalServices: ['0000ffe0-0000-1000-8000-00805f9b34fb']
      });

      console.log('מתחבר ל:', device.name);
      deviceRef.current = device;

      // התחבר
      const server = await device.gatt.connect();
      console.log('מחובר ל-GATT');

      // קבל service
      const service = await server.getPrimaryService('0000ffe0-0000-1000-8000-00805f9b34fb');
      console.log('קיבלתי service');

      // קבל characteristic לקבלת נתונים
      const dataCharacteristic = await service.getCharacteristic('0000ffe1-0000-1000-8000-00805f9b34fb');
      console.log('קיבלתי characteristic לנתונים');
      dataCharacteristicRef.current = dataCharacteristic;

      // קבל characteristic לשליחת פקודות
      const commandCharacteristic = await service.getCharacteristic('0000ffe2-0000-1000-8000-00805f9b34fb');
      console.log('קיבלתי characteristic לפקודות');
      commandCharacteristicRef.current = commandCharacteristic;

      // התחל להאזין לנתונים
      await dataCharacteristic.startNotifications();
      dataCharacteristic.addEventListener('characteristicvaluechanged', handleDataReceived);

      // האזן לניתוק
      device.addEventListener('gattserverdisconnected', handleDisconnect);

      setConnected(true);
      setConnecting(false);
      console.log('חיבור BLE הושלם בהצלחה!');

    } catch (error) {
      console.error('שגיאה בחיבור:', error);
      setConnecting(false);

      if (error.name === 'NotFoundError') {
        alert('לא נמצא מכשיר. וודא שה-ESP32 פועל ושמו מתחיל ב-"PulseSense"');
      } else if (error.name === 'SecurityError') {
        alert('שגיאת אבטחה. וודא שאתה ב-https או localhost');
      } else {
        alert('שגיאה בחיבור: ' + error.message);
      }
    }
  };

  // קבלת נתונים
  const handleDataReceived = (event) => {
    const value = new TextDecoder().decode(event.target.value);
    console.log('📨 קיבלתי:', value);

    try {
      const jsonData = JSON.parse(value);

      if (jsonData.type === 'alert') {
        // התראה
        const newAlert = {
          id: Date.now(),
          message: jsonData.message,
          time: new Date().toLocaleTimeString('he-IL')
        };
        setAlerts(prev => [newAlert, ...prev.slice(0, 4)]);
        console.log('🚨 התראה:', jsonData.message);
      } else {
        // נתונים רגילים עם טמפרטורה
        setData({
          motion: jsonData.motion || 0,
          pulse: jsonData.pulse || 0,
          status: jsonData.status || 'לא זמין',
          temperature: jsonData.temperature || null,
          temp_status: jsonData.temp_status || 'לא זמין',
          time: jsonData.time || Date.now()
        });
      }
    } catch (e) {
      console.log('📄 נתון לא-JSON:', value);
      // נסה לפרש כנתון פשוט
      if (value.includes('motion') || value.includes('pulse')) {
        console.log('💡 זה נראה כמו נתוני חיישן');
      }
    }
  };

  // ניתוק
  const handleDisconnect = () => {
    console.log('🔌 המכשיר התנתק');
    setConnected(false);
    setData({ motion: 0, pulse: 0, status: 'מנותק' });
  };

  const disconnect = () => {
    if (deviceRef.current && deviceRef.current.gatt.connected) {
      deviceRef.current.gatt.disconnect();
    }
  };

  // שליחת פקודה
  const sendCommand = async (command) => {
    if (!commandCharacteristicRef.current) {
      console.log('❌ אין חיבור לשליחת פקודה');
      return;
    }

    try {
      const encoder = new TextEncoder();
      await commandCharacteristicRef.current.writeValue(encoder.encode(command));
      console.log('📤 שלחתי:', command);
    } catch (error) {
      console.error('❌ שגיאה בשליחת פקודה:', error);
    }
  };

  // בדיקת תמיכה בדפדפן
  if (!isBluetoothSupported()) {
    return (
        <div className="min-h-screen bg-red-50 flex items-center justify-center p-6">
          <div className="bg-white rounded-xl shadow-lg p-8 text-center max-w-md">
            <div className="text-6xl mb-4">❌</div>
            <h1 className="text-2xl font-bold text-red-600 mb-4">דפדפן לא נתמך</h1>
            <p className="text-gray-700 mb-4">
              אנא השתמש בדפדפן Chrome או Edge כדי להתחבר למכשיר
            </p>
            <p className="text-sm text-gray-500">
              Safari ו-Firefox אינם תומכים ב-Web Bluetooth
            </p>
          </div>
        </div>
    );
  }

  return (
      <div className="min-h-screen bg-gradient-to-br from-blue-50 to-purple-100 p-6">
        <div className="max-w-2xl mx-auto space-y-6">

          {/* כותרת וחיבור */}
          <div className="bg-white rounded-xl shadow-lg p-6">
            <div className="text-center mb-6">
              <h1 className="text-3xl font-bold text-gray-800 flex items-center justify-center gap-3">
                <Heart className="text-red-500" />
                PulseSense
              </h1>
            </div>

            <div className="flex items-center justify-between">
              <div className="flex items-center gap-2">
                {connected ? (
                    <>
                      <Wifi className="text-green-500" size={20} />
                      <span className="text-green-600 font-medium">מחובר</span>
                    </>
                ) : (
                    <>
                      <WifiOff className="text-red-500" size={20} />
                      <span className="text-red-600 font-medium">מנותק</span>
                    </>
                )}
              </div>

              {!connected ? (
                  <button
                      onClick={connect}
                      disabled={connecting}
                      className="bg-blue-500 hover:bg-blue-600 disabled:bg-blue-300 text-white px-6 py-3 rounded-lg flex items-center gap-2 transition-colors"
                  >
                    <Bluetooth size={20} />
                    {connecting ? 'מתחבר...' : 'התחבר'}
                  </button>
              ) : (
                  <button
                      onClick={disconnect}
                      className="bg-red-500 hover:bg-red-600 text-white px-4 py-2 rounded-lg transition-colors"
                  >
                    נתק
                  </button>
              )}
            </div>
          </div>

          {/* נתוני חיישנים */}
          {connected && (
              <div className="grid md:grid-cols-3 gap-6">

                {/* תנועה */}
                <div className="bg-white rounded-xl shadow-lg p-6">
                  <div className="flex items-center gap-3 mb-4">
                    <Activity className="text-blue-500" size={28} />
                    <h2 className="text-xl font-bold text-gray-800">תנועה</h2>
                  </div>

                  <div className="mb-3">
                <span className="text-3xl font-bold text-blue-600">
                  {typeof data.motion === 'number' ? data.motion.toFixed(1) : '0.0'}
                </span>
                    <span className="text-gray-500 text-sm mr-2">יחידות</span>
                  </div>

                  <div className="w-full bg-gray-200 rounded-full h-3">
                    <div
                        className="bg-blue-500 h-3 rounded-full transition-all duration-300"
                        style={{
                          width: `${Math.min(Math.max((data.motion || 0) * 8, 0), 100)}%`
                        }}
                    ></div>
                  </div>
                </div>

                {/* דופק */}
                <div className="bg-white rounded-xl shadow-lg p-6">
                  <div className="flex items-center gap-3 mb-4">
                    <Heart className="text-red-500" size={28} />
                    <h2 className="text-xl font-bold text-gray-800">דופק</h2>
                  </div>

                  <div className="mb-3">
                <span className="text-3xl font-bold text-red-600">
                  {data.pulse > 0 ? data.pulse : '--'}
                </span>
                    <span className="text-gray-500 text-sm mr-2">BPM</span>
                  </div>

                  <div className="text-gray-600 text-sm">
                    <strong>מצב:</strong> {data.status}
                  </div>

                  {data.pulse > 0 && (
                      <div className="mt-2">
                        <div className="flex items-center gap-2">
                          <div className="w-2 h-2 bg-red-500 rounded-full animate-pulse"></div>
                          <span className="text-sm text-green-600">דופק פעיל</span>
                        </div>
                      </div>
                  )}
                </div>

                {/* טמפרטורה */}
                <div className="bg-white rounded-xl shadow-lg p-6">
                  <div className="flex items-center gap-3 mb-4">
                    <Thermometer className="text-orange-500" size={28} />
                    <h2 className="text-xl font-bold text-gray-800">טמפרטורה</h2>
                  </div>

                  <div className="mb-3">
                <span className="text-3xl font-bold text-orange-600">
                  {data.temperature ? data.temperature.toFixed(1) : '--'}
                </span>
                    <span className="text-gray-500 text-sm mr-2">°C</span>
                  </div>

                  <div className="text-gray-600 text-sm">
                    <strong>סוג:</strong> {data.temp_status}
                  </div>

                  {data.temperature && data.temperature > 37.2 && (
                      <div className="mt-2">
                        <div className="flex items-center gap-2">
                          <div className="w-2 h-2 bg-red-500 rounded-full animate-pulse"></div>
                          <span className="text-sm text-red-600">טמפרטורה גבוהה</span>
                        </div>
                      </div>
                  )}
                </div>
              </div>
          )}

          {/* התראות */}
          {alerts.length > 0 && (
              <div className="bg-white rounded-xl shadow-lg p-6">
                <h3 className="text-lg font-bold text-gray-800 mb-4 flex items-center gap-2">
                  🚨 התראות
                </h3>
                <div className="space-y-3">
                  {alerts.map((alert) => (
                      <div
                          key={alert.id}
                          className="p-4 bg-red-50 border-r-4 border-red-500 rounded-lg"
                      >
                        <div className="flex justify-between items-start">
                          <span className="text-red-800 font-medium">{alert.message}</span>
                          <span className="text-red-600 text-xs">{alert.time}</span>
                        </div>
                      </div>
                  ))}
                </div>
                <button
                    onClick={() => setAlerts([])}
                    className="mt-3 text-sm text-gray-500 hover:text-gray-700"
                >
                  נקה התראות
                </button>
              </div>
          )}

          {/* פקודות ובדיקות */}
          {connected && (
              <div className="bg-white rounded-xl shadow-lg p-6">
                <h3 className="text-lg font-bold text-gray-800 mb-4">פקודות מהירות</h3>
                <div className="flex gap-3 flex-wrap">
                  <button
                      onClick={() => sendCommand('ping')}
                      className="bg-green-500 hover:bg-green-600 text-white px-4 py-2 rounded-lg transition-colors text-sm"
                  >
                    🏓 בדיקת חיבור
                  </button>
                  <button
                      onClick={() => sendCommand('1')}
                      className="bg-blue-500 hover:bg-blue-600 text-white px-4 py-2 rounded-lg transition-colors text-sm"
                  >
                    🏃 תנועה
                  </button>
                  <button
                      onClick={() => sendCommand('2')}
                      className="bg-red-500 hover:bg-red-600 text-white px-4 py-2 rounded-lg transition-colors text-sm"
                  >
                    💓 דופק
                  </button>
                  <button
                      onClick={() => sendCommand('3')}
                      className="bg-orange-500 hover:bg-orange-600 text-white px-4 py-2 rounded-lg transition-colors text-sm"
                  >
                    🌡️ טמפרטורה
                  </button>
                  <button
                      onClick={() => sendCommand('4')}
                      className="bg-purple-500 hover:bg-purple-600 text-white px-4 py-2 rounded-lg transition-colors text-sm"
                  >
                    🔄 משולב
                  </button>
                  <button
                      onClick={() => {
                        console.clear();
                        console.log('🧹 Console נוקה');
                      }}
                      className="bg-gray-500 hover:bg-gray-600 text-white px-4 py-2 rounded-lg transition-colors text-sm"
                  >
                    🧹 נקה Console
                  </button>
                </div>
              </div>
          )}

          {/* הוראות */}
          <div className="bg-yellow-50 border border-yellow-200 rounded-xl p-6">
            <h3 className="text-lg font-bold text-yellow-800 mb-3">💡 איך מתחברים:</h3>
            <ol className="text-yellow-700 space-y-2 list-decimal list-inside">
              <li>וודא שה-ESP32 פועל ורואה "🔵 Bluetooth מוכן!"</li>
              <li>לחץ "התחבר" ובחר מכשיר שמכיל "PulseSense" בשם</li>
              <li>אם לא רואה - נסה לאפס את ESP32 ולנסות שוב</li>
              <li>בדוק ב-Console (F12) אם יש שגיאות</li>
            </ol>
          </div>
        </div>
      </div>
  );
};

export default SimplePulseSenseApp;