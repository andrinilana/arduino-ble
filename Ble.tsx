import React, { useEffect, useState } from 'react';
import {
  View,
  Text,
  PermissionsAndroid,
  Platform,
  StyleSheet,
} from 'react-native';
import {
  BleManager,
  Device,
  Characteristic,
  Subscription,
} from 'react-native-ble-plx';
import { Buffer } from 'buffer';

const SERVICE_UUID = '12345678-1234-1234-1234-123456789abc';
const CHARACTERISTIC_UUID = 'abcd1234-ab12-cd34-ef56-1234567890ab';
const DEVICE_NAME = 'ESP32_BLE';

export default function App() {
  const [receivedData, setReceivedData] = useState<string>('');
  const manager = new BleManager();

  useEffect(() => {
    let monitorSubscription: Subscription | null = null;

    const startBLE = async (): Promise<void> => {
      if (Platform.OS === 'android') {
        await PermissionsAndroid.requestMultiple([
          PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
          PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
          PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
        ]);
      }

      manager.startDeviceScan(null, null, async (error, device: Device | null) => {
        if (error) {
          console.error('Scan error:', error);
          return;
        }

        if (device?.name === DEVICE_NAME) {
          console.log('Found ESP32!');
          manager.stopDeviceScan();

          try {
            const connectedDevice = await device.connect();
            await connectedDevice.discoverAllServicesAndCharacteristics();

            monitorSubscription = connectedDevice.monitorCharacteristicForService(
              SERVICE_UUID,
              CHARACTERISTIC_UUID,
              (err: Error | null, characteristic: Characteristic | null) => {
                if (err) {
                  console.error('Notification error:', err);
                  return;
                }

                if (characteristic?.value) {
                  const decoded = Buffer.from(characteristic.value, 'base64').toString('utf8');
                  console.log('Received:', decoded);
                  setReceivedData(decoded);
                }
              }
            );
          } catch (err) {
            console.error('Connection error:', err);
          }
        }
      });
    };

    startBLE();

    return () => {
      manager.stopDeviceScan();
      monitorSubscription?.remove();
      manager.destroy();
    };
  }, []);

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Real-time BLE Data:</Text>
      <Text style={styles.data}>{receivedData}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: { padding: 20 },
  title: { fontSize: 18 },
  data: { fontSize: 24, color: 'blue' },
});
