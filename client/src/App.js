import React, { useState, useRef, useEffect } from 'react';
import { Bluetooth, Heart, Activity, Wifi, WifiOff, Thermometer, Zap, Loader } from 'lucide-react';

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
  const [pulseAnimation, setPulseAnimation] = useState(false);

  const deviceRef = useRef(null);
  const dataCharacteristicRef = useRef(null);
  const commandCharacteristicRef = useRef(null);

  // Styles
  const styles = {
    container: {
      minHeight: '100vh',
      background: 'linear-gradient(135deg, #a855f7 0%, #ec4899 25%, #6366f1 100%)',
      position: 'relative',
      overflow: 'hidden',
      direction: 'rtl',
      fontFamily: 'system-ui, -apple-system, sans-serif'
    },
    backgroundPattern: {
      position: 'absolute',
      top: 0,
      left: 0,
      right: 0,
      bottom: 0,
      backgroundImage: 'radial-gradient(circle at 25px 25px, rgba(255,255,255,0.1) 2px, transparent 0)',
      backgroundSize: '50px 50px',
      opacity: 0.5
    },
    content: {
      position: 'relative',
      zIndex: 10,
      maxWidth: '1200px',
      margin: '0 auto',
      padding: '24px',
      display: 'flex',
      flexDirection: 'column',
      gap: '32px'
    },
    header: {
      textAlign: 'center',
      padding: '32px 0'
    },
    title: {
      fontSize: '3rem',
      fontWeight: 'bold',
      color: 'white',
      marginBottom: '16px',
      display: 'flex',
      alignItems: 'center',
      justifyContent: 'center',
      gap: '16px'
    },
    subtitle: {
      color: 'rgba(255, 255, 255, 0.8)',
      fontSize: '1.125rem',
      margin: 0
    },
    panel: {
      background: 'rgba(255, 255, 255, 0.2)',
      backdropFilter: 'blur(16px)',
      borderRadius: '24px',
      boxShadow: '0 25px 50px -12px rgba(0, 0, 0, 0.25)',
      padding: '32px',
      border: '1px solid rgba(255, 255, 255, 0.3)'
    },
    connectionPanel: {
      display: 'flex',
      alignItems: 'center',
      justifyContent: 'space-between',
      flexWrap: 'wrap',
      gap: '20px'
    },
    connectionInfo: {
      display: 'flex',
      alignItems: 'center',
      gap: '16px'
    },
    statusIcon: {
      padding: '12px',
      borderRadius: '50%',
      boxShadow: '0 10px 25px -5px rgba(0, 0, 0, 0.1)',
      transition: 'all 0.5s ease'
    },
    statusIconConnected: {
      background: '#10b981'
    },
    statusIconDisconnected: {
      background: '#ef4444'
    },
    statusText: {
      color: 'white'
    },
    statusTitle: {
      fontSize: '1.5rem',
      fontWeight: 'bold',
      margin: '0 0 4px 0'
    },
    statusSubtitle: {
      color: 'rgba(255, 255, 255, 0.7)',
      margin: 0
    },
    button: {
      background: 'linear-gradient(to right, #3b82f6, #2563eb)',
      color: 'white',
      padding: '16px 32px',
      borderRadius: '16px',
      border: 'none',
      display: 'flex',
      alignItems: 'center',
      gap: '12px',
      transition: 'all 0.3s ease',
      cursor: 'pointer',
      fontSize: '1rem',
      fontWeight: '500'
    },
    buttonHover: {
      transform: 'scale(1.05)',
      boxShadow: '0 20px 40px -10px rgba(59, 130, 246, 0.4)'
    },
    disconnectButton: {
      background: 'linear-gradient(to right, #ef4444, #dc2626)',
      padding: '12px 24px'
    },
    sensorsGrid: {
      display: 'grid',
      gridTemplateColumns: 'repeat(auto-fit, minmax(300px, 1fr))',
      gap: '32px'
    },
    sensorCard: {
      background: 'rgba(255, 255, 255, 0.8)',
      backdropFilter: 'blur(4px)',
      borderRadius: '16px',
      boxShadow: '0 25px 50px -12px rgba(0, 0, 0, 0.25)',
      padding: '24px',
      transition: 'all 0.5s ease',
      border: '1px solid rgba(255, 255, 255, 0.2)'
    },
    sensorHeader: {
      display: 'flex',
      alignItems: 'center',
      gap: '16px',
      marginBottom: '16px'
    },
    sensorIcon: {
      padding: '12px',
      borderRadius: '50%',
      color: 'white',
      boxShadow: '0 10px 25px -5px rgba(0, 0, 0, 0.1)'
    },
    motionIcon: {
      background: 'linear-gradient(135deg, #3b82f6, #2563eb)'
    },
    pulseIcon: {
      background: 'linear-gradient(135deg, #ef4444, #dc2626)'
    },
    tempIcon: {
      background: 'linear-gradient(135deg, #f97316, #ea580c)'
    },
    sensorTitle: {
      fontSize: '1.25rem',
      fontWeight: 'bold',
      color: '#1f2937',
      margin: 0
    },
    sensorValue: {
      marginBottom: '16px',
      display: 'flex',
      alignItems: 'baseline',
      gap: '8px'
    },
    valueNumber: {
      fontSize: '2.5rem',
      fontWeight: 'bold',
      color: '#1f2937'
    },
    valueUnit: {
      color: '#6b7280',
      fontSize: '1.125rem'
    },
    progressBar: {
      width: '100%',
      background: '#e5e7eb',
      borderRadius: '9999px',
      height: '8px',
      overflow: 'hidden'
    },
    progressFill: {
      height: '100%',
      background: 'linear-gradient(to right, #3b82f6, #2563eb)',
      borderRadius: '9999px',
      transition: 'all 1s ease-out'
    },
    commandsGrid: {
      display: 'grid',
      gridTemplateColumns: 'repeat(auto-fit, minmax(120px, 1fr))',
      gap: '16px'
    },
    commandButton: {
      background: 'rgba(255, 255, 255, 0.1)',
      border: '1px solid rgba(255, 255, 255, 0.2)',
      color: 'white',
      padding: '16px',
      borderRadius: '16px',
      transition: 'all 0.3s ease',
      cursor: 'pointer',
      textAlign: 'center'
    },
    commandIcon: {
      fontSize: '1.5rem',
      marginBottom: '8px'
    },
    commandLabel: {
      fontSize: '0.875rem',
      fontWeight: '500'
    }
  };

  // אנימציה לדופק
  useEffect(() => {
    if (data.pulse > 0) {
      setPulseAnimation(true);
      const interval = setInterval(() => {
        setPulseAnimation(prev => !prev);
      }, 60000 / Math.max(data.pulse, 60));
      return () => clearInterval(interval);
    }
  }, [data.pulse]);

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

      const device = await navigator.bluetooth.requestDevice({
        filters: [{
          namePrefix: 'PulseSense'
        }],
        optionalServices: ['0000ffe0-0000-1000-8000-00805f9b34fb']
      });

      console.log('מתחבר ל:', device.name);
      deviceRef.current = device;

      const server = await device.gatt.connect();
      console.log('מחובר ל-GATT');

      const service = await server.getPrimaryService('0000ffe0-0000-1000-8000-00805f9b34fb');
      console.log('קיבלתי service');

      const dataCharacteristic = await service.getCharacteristic('0000ffe1-0000-1000-8000-00805f9b34fb');
      console.log('קיבלתי characteristic לנתונים');
      dataCharacteristicRef.current = dataCharacteristic;

      const commandCharacteristic = await service.getCharacteristic('0000ffe2-0000-1000-8000-00805f9b34fb');
      console.log('קיבלתי characteristic לפקודות');
      commandCharacteristicRef.current = commandCharacteristic;

      await dataCharacteristic.startNotifications();
      dataCharacteristic.addEventListener('characteristicvaluechanged', handleDataReceived);

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
        const newAlert = {
          id: Date.now(),
          message: jsonData.message,
          time: new Date().toLocaleTimeString('he-IL')
        };
        setAlerts(prev => [newAlert, ...prev.slice(0, 4)]);
        console.log('🚨 התראה:', jsonData.message);
      } else {
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
        <div style={styles.container}>
          <div style={{...styles.content, justifyContent: 'center', alignItems: 'center'}}>
            <div style={{...styles.panel, textAlign: 'center', maxWidth: '400px'}}>
              <div style={{fontSize: '4rem', marginBottom: '24px'}}>❌</div>
              <h1 style={{fontSize: '1.875rem', fontWeight: 'bold', color: '#dc2626', margin: '0 0 16px 0'}}>
                דפדפן לא נתמך
              </h1>
              <p style={{color: '#374151', margin: '0 0 16px 0', lineHeight: '1.6'}}>
                אנא השתמש בדפדפן Chrome או Edge כדי להתחבר למכשיר
              </p>
              <p style={{fontSize: '0.875rem', color: '#6b7280', margin: 0}}>
                Safari ו-Firefox אינם תומכים ב-Web Bluetooth
              </p>
            </div>
          </div>
        </div>
    );
  }

  return (
      <div style={styles.container}>
        <div style={styles.backgroundPattern}></div>

        <div style={styles.content}>
          {/* כותרת ראשית */}
          <div style={styles.header}>
            <h1 style={styles.title}>
              <Heart
                  color="#f87171"
                  size={48}
                  style={pulseAnimation ? {animation: 'pulse 0.8s ease-in-out infinite'} : {}}
              />
              PulseSense
            </h1>
            <p style={styles.subtitle}>מערכת ניטור חיוני מתקדמת</p>
          </div>

          {/* פאנל חיבור */}
          <div style={{...styles.panel, ...styles.connectionPanel}}>
            <div style={styles.connectionInfo}>
              <div style={{
                ...styles.statusIcon,
                ...(connected ? styles.statusIconConnected : styles.statusIconDisconnected)
              }}>
                {connected ? <Wifi color="white" size={24} /> : <WifiOff color="white" size={24} />}
              </div>
              <div style={styles.statusText}>
                <h2 style={styles.statusTitle}>
                  {connected ? 'מחובר בהצלחה' : 'מנותק'}
                </h2>
                <p style={styles.statusSubtitle}>
                  {connected ? 'קבלת נתונים בזמן אמת' : 'לחץ להתחברות למכשיר'}
                </p>
              </div>
            </div>

            {!connected ? (
                <button
                    onClick={connect}
                    disabled={connecting}
                    style={{
                      ...styles.button,
                      ...(connecting ? {opacity: 0.8} : {})
                    }}
                    onMouseEnter={(e) => !connecting && Object.assign(e.target.style, styles.buttonHover)}
                    onMouseLeave={(e) => !connecting && Object.assign(e.target.style, styles.button)}
                >
                  {connecting ? (
                      <>
                        <Loader style={{animation: 'spin 1s linear infinite'}} size={24} />
                        מתחבר...
                      </>
                  ) : (
                      <>
                        <Bluetooth size={24} />
                        התחבר למכשיר
                      </>
                  )}
                </button>
            ) : (
                <button
                    onClick={disconnect}
                    style={{...styles.button, ...styles.disconnectButton}}
                >
                  נתק
                </button>
            )}
          </div>

          {/* נתוני חיישנים */}
          {connected && (
              <div style={styles.sensorsGrid}>
                {/* תנועה */}
                <div style={styles.sensorCard}>
                  <div style={styles.sensorHeader}>
                    <div style={{...styles.sensorIcon, ...styles.motionIcon}}>
                      <Activity size={28} />
                    </div>
                    <h3 style={styles.sensorTitle}>חיישן תנועה</h3>
                  </div>

                  <div style={styles.sensorValue}>
                <span style={styles.valueNumber}>
                  {typeof data.motion === 'number' ? data.motion.toFixed(1) : '0.0'}
                </span>
                    <span style={styles.valueUnit}>יחידות</span>
                  </div>

                  <div style={styles.progressBar}>
                    <div
                        style={{
                          ...styles.progressFill,
                          width: `${Math.min(Math.max((data.motion || 0) * 8, 0), 100)}%`
                        }}
                    />
                  </div>

                  {data.motion > 10 && (
                      <div style={{marginTop: '12px', display: 'flex', alignItems: 'center', gap: '8px', color: '#3b82f6'}}>
                        <Zap size={16} />
                        <span style={{fontSize: '0.875rem', fontWeight: '500'}}>תנועה אקטיבית</span>
                      </div>
                  )}
                </div>

                {/* דופק */}
                <div style={styles.sensorCard}>
                  <div style={styles.sensorHeader}>
                    <div style={{...styles.sensorIcon, ...styles.pulseIcon}}>
                      <Heart
                          size={28}
                          style={pulseAnimation ? {animation: 'pulse 0.8s ease-in-out infinite'} : {}}
                      />
                    </div>
                    <h3 style={styles.sensorTitle}>דופק</h3>
                  </div>

                  <div style={styles.sensorValue}>
                <span style={styles.valueNumber}>
                  {data.pulse > 0 ? data.pulse : '--'}
                </span>
                    <span style={styles.valueUnit}>BPM</span>
                  </div>
                  <p style={{color: '#4b5563', fontSize: '0.875rem', margin: '4px 0'}}>
                    מצב: {data.status}
                  </p>

                  {data.pulse > 0 && (
                      <div style={{display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginTop: '12px'}}>
                        <div style={{display: 'flex', alignItems: 'center', gap: '8px', color: '#10b981'}}>
                          <div style={{
                            width: '8px',
                            height: '8px',
                            background: '#10b981',
                            borderRadius: '50%',
                            animation: 'pulse 2s infinite'
                          }}></div>
                          <span style={{fontSize: '0.875rem', fontWeight: '500'}}>דופק פעיל</span>
                        </div>
                        <div style={{fontSize: '0.75rem', color: '#6b7280'}}>
                          {data.pulse < 60 ? 'איטי' : data.pulse > 100 ? 'מהיר' : 'נורמלי'}
                        </div>
                      </div>
                  )}
                </div>

                {/* טמפרטורה */}
                <div style={styles.sensorCard}>
                  <div style={styles.sensorHeader}>
                    <div style={{...styles.sensorIcon, ...styles.tempIcon}}>
                      <Thermometer size={28} />
                    </div>
                    <h3 style={styles.sensorTitle}>טמפרטורה</h3>
                  </div>

                  <div style={styles.sensorValue}>
                <span style={styles.valueNumber}>
                  {data.temperature ? data.temperature.toFixed(1) : '--'}
                </span>
                    <span style={styles.valueUnit}>°C</span>
                  </div>
                  <p style={{color: '#4b5563', fontSize: '0.875rem', margin: '4px 0'}}>
                    סוג: {data.temp_status}
                  </p>

                  {data.temperature && (
                      <div style={{display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginTop: '12px'}}>
                        <div style={{fontSize: '0.75rem', color: '#6b7280'}}>
                          {data.temperature < 36 ? '🧊 קר' :
                              data.temperature > 37.2 ? '🔥 חם' : '✅ נורמלי'}
                        </div>
                        {data.temperature > 37.2 && (
                            <div style={{display: 'flex', alignItems: 'center', gap: '4px', color: '#ef4444', fontSize: '0.75rem'}}>
                              <div style={{
                                width: '8px',
                                height: '8px',
                                background: '#ef4444',
                                borderRadius: '50%',
                                animation: 'pulse 1s infinite'
                              }}></div>
                              גבוה
                            </div>
                        )}
                      </div>
                  )}
                </div>
              </div>
          )}

          {/* התראות */}
          {alerts.length > 0 && (
              <div style={styles.panel}>
                <h3 style={{fontSize: '1.5rem', fontWeight: 'bold', color: 'white', margin: '0 0 24px 0'}}>
                  🚨 התראות אחרונות
                </h3>
                <div style={{display: 'flex', flexDirection: 'column', gap: '16px'}}>
                  {alerts.map((alert) => (
                      <div
                          key={alert.id}
                          style={{
                            padding: '16px',
                            background: 'rgba(239, 68, 68, 0.2)',
                            border: '1px solid rgba(239, 68, 68, 0.3)',
                            borderRadius: '16px',
                            backdropFilter: 'blur(4px)',
                            display: 'flex',
                            justifyContent: 'space-between',
                            alignItems: 'flex-start'
                          }}
                      >
                  <span style={{color: 'rgba(255, 255, 255, 0.9)', fontWeight: '500', flex: 1}}>
                    {alert.message}
                  </span>
                        <span style={{color: 'rgba(255, 255, 255, 0.7)', fontSize: '0.875rem', whiteSpace: 'nowrap', marginRight: '12px'}}>
                    {alert.time}
                  </span>
                      </div>
                  ))}
                </div>
                <button
                    onClick={() => setAlerts([])}
                    style={{
                      marginTop: '16px',
                      color: 'rgba(255, 255, 255, 0.6)',
                      background: 'none',
                      border: 'none',
                      fontSize: '0.875rem',
                      cursor: 'pointer'
                    }}
                >
                  נקה את כל ההתראות
                </button>
              </div>
          )}

          {/* פקודות ובדיקות */}
          {connected && (
              <div style={styles.panel}>
                <h3 style={{fontSize: '1.5rem', fontWeight: 'bold', color: 'white', margin: '0 0 24px 0'}}>
                  פקודות מהירות
                </h3>
                <div style={styles.commandsGrid}>
                  {[
                    { cmd: 'ping', label: 'בדיקה', icon: '🏓' },
                    { cmd: '1', label: 'תנועה', icon: '🏃' },
                    { cmd: '2', label: 'דופק', icon: '💓' },
                    { cmd: '3', label: 'טמפרטורה', icon: '🌡️' },
                    { cmd: '4', label: 'משולב', icon: '🔄' },
                    { cmd: 'clear', label: 'נקה', icon: '🧹' }
                  ].map(({ cmd, label, icon }) => (
                      <button
                          key={cmd}
                          onClick={() => cmd === 'clear' ? console.clear() : sendCommand(cmd)}
                          style={styles.commandButton}
                          onMouseEnter={(e) => {
                            e.target.style.transform = 'scale(1.05)';
                            e.target.style.background = 'rgba(255, 255, 255, 0.2)';
                          }}
                          onMouseLeave={(e) => {
                            e.target.style.transform = 'scale(1)';
                            e.target.style.background = 'rgba(255, 255, 255, 0.1)';
                          }}
                      >
                        <div style={styles.commandIcon}>{icon}</div>
                        <div style={styles.commandLabel}>{label}</div>
                      </button>
                  ))}
                </div>
              </div>
          )}

          {/* הוראות */}
          <div style={{
            ...styles.panel,
            background: 'rgba(251, 191, 36, 0.2)',
            border: '1px solid rgba(251, 191, 36, 0.3)'
          }}>
            <h3 style={{fontSize: '1.5rem', fontWeight: 'bold', color: 'white', margin: '0 0 16px 0'}}>
              💡 הוראות שימוש
            </h3>
            <div style={{display: 'grid', gap: '24px', color: 'rgba(255, 255, 255, 0.9)'}}>
              <div>
                <h4 style={{fontWeight: '600', margin: '0 0 8px 0'}}>שלבי החיבור:</h4>
                <ol style={{margin: 0, paddingRight: '20px', lineHeight: '1.6'}}>
                  <li style={{fontSize: '0.875rem', marginBottom: '4px'}}>
                    וודא שה-ESP32 פועל ורואה "🔵 Bluetooth מוכן!"
                  </li>
                  <li style={{fontSize: '0.875rem', marginBottom: '4px'}}>
                    לחץ "התחבר למכשיר" ובחר מכשיר עם "PulseSense"
                  </li>
                  <li style={{fontSize: '0.875rem', marginBottom: '4px'}}>
                    המתן לחיבור מוצלח
                  </li>
                </ol>
              </div>
              <div>
                <h4 style={{fontWeight: '600', margin: '0 0 8px 0'}}>פתרון בעיות:</h4>
                <ul style={{margin: 0, paddingRight: '20px', lineHeight: '1.6'}}>
                  <li style={{fontSize: '0.875rem', marginBottom: '4px'}}>
                    • אם לא רואה מכשיר - אפס את ESP32
                  </li>
                  <li style={{fontSize: '0.875rem', marginBottom: '4px'}}>
                    • בדוק ב-Console (F12) אם יש שגיאות
                  </li>
                  <li style={{fontSize: '0.875rem', marginBottom: '4px'}}>
                    • השתמש ב-Chrome או Edge בלבד
                  </li>
                </ul>
              </div>
            </div>
          </div>
        </div>

        <style>{`
        @keyframes pulse {
          0%, 100% { opacity: 1; }
          50% { opacity: 0.5; }
        }
        @keyframes spin {
          from { transform: rotate(0deg); }
          to { transform: rotate(360deg); }
        }
      `}</style>
      </div>
  );
};

export default SimplePulseSenseApp;