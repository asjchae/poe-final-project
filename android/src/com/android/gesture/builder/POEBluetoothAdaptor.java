package com.android.gesture.builder;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;


public class POEBluetoothAdaptor {
	
	BluetoothAdapter bluetooth = null;
	String mStatus; // it shows the status of application
	String connection_status; // it shows the bluetooth connection status. 
	String bluetooth_name; // it saves the name of bluetooth device that we connects
	String Socket_connection_status;
	String Connection_status;
	String Sending_status;
	int Sending_with_deci = 0;
	String Sending_with_deci_str ;
	BluetoothDevice mmdevice = null; // bluetooth device that we actually uses for data transfer
	BluetoothSocket mSocket = null; // bluetooth socket for data transfer
	private InputStream mInputStream = null; // variable for getting data from bluetooth shield
	private OutputStream mOutputStream = null; // variable for sending data from bluetooth shield
	
	public POEBluetoothAdaptor() {
		// get the bluetooth defaultAdapter;
		bluetooth = BluetoothAdapter.getDefaultAdapter();
		
		if(bluetooth != null)
		{
			if (bluetooth.isEnabled()) {
			    String mydeviceaddress = bluetooth.getAddress();
			    String mydevicename = bluetooth.getName();
			    mStatus = mydevicename + " : " + mydeviceaddress;
			}
			else
			{
			    mStatus = "Bluetooth is not Enabled.";
			}
		}
	}
	
	public void connect() {

		if (bluetooth.isEnabled()){
			Set<BluetoothDevice> pairedDevices = bluetooth.getBondedDevices();
	
			// Check through the devices that are connected to my own device.
			// If the name of device is "SeeedBTSlave" which is the one that we use
			// mmdevice is now "SeeedBTSlave".
			for(BluetoothDevice bt : pairedDevices)
			{
				if (bt.getName().equals("SeeedBTSlave"))
				{
					bluetooth_name = bt.getName();
					mmdevice = bt;
					break;
				}
			}
			connection_status = bluetooth_name;
		}
		else
		{
			connection_status = "Bluetooth is not Enabled.";
		}
	}
	
	public void send(int[] toArduino) throws IOException 
	{
		
		UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); //Standard SerialPortService ID
		mSocket = mmdevice.createRfcommSocketToServiceRecord(uuid);
		try {
            // Connect the device through the socket. This will block
            // until it succeeds or throws an exception
            mSocket.connect();
        	Socket_connection_status = "mSockect connected";

        } catch (IOException connectException) {
        	Socket_connection_status = "mSockect did not connect";
        }
		
		try
		{
			// try again to make sure it works
        	mSocket.connect();
        	Socket_connection_status = "mSockect connected";
        } catch (Exception e){
        	Socket_connection_status = "mSockect did not connect";
        	Log.d("bluetooth", "...Error data send: " + e.getMessage() + "...");   
        }
        mOutputStream = mSocket.getOutputStream();
        mInputStream = mSocket.getInputStream();
        
        // There's every missing first number. So we just make it 0.
        toArduino[0]=0;
        
        // For 15 vertical length
        for (int i = 0; i < 15; i++) {
        	
        	// We consider the original 12 length list as binary, and change it to decimal, and send it.
        	// This work has benefits with the memory of data. It can change 12 length of data to 4.
        	Sending_with_deci = 0;
        	for (int j = 0; j< 12; j++){
	        	if (toArduino[12*i+j] == 1) {
	        		Sending_with_deci = Sending_with_deci*2+1;
	        	}
	        	else {
	        		Sending_with_deci = Sending_with_deci*2;
	        	}
	        	
        	}
        	
        	Sending_with_deci_str = adding_x(Sending_with_deci);
            try {
            	mOutputStream.write(Sending_with_deci_str.getBytes());
            }catch  (IOException e){}
            Sending_with_deci = 0;
        }
        Sending_status = "Data Sent!";
    }
	
	// To make all one 12 length of data lenConnection_status change it to decimal and add extra 'x's to make it length 4.
	
	// If we send [00000001101
	//             000001000000
	//             111111111111
	//             ......     ]
	// It change all binary number as a decimal, so the list would be changed as
	// [13, 64, 4095, ...]
	// To make all the value's length as 4, we add 'x's, which makes list as
	// [13xx, 64xx, 4095, ...]
	// so all the value's length changes to 4 and would be sent to the Arduino.
	
	public String adding_x(int Sending_with_deci){
		String Sending_with_deci_str1 = null;
		if (Sending_with_deci<10){
			Sending_with_deci_str1 = Integer.toString(Sending_with_deci)+"xxx";
		}
		else if (Sending_with_deci<100){
			Sending_with_deci_str1 = Integer.toString(Sending_with_deci)+"xx";
		}
		else if (Sending_with_deci<1000){
			Sending_with_deci_str1 = Integer.toString(Sending_with_deci)+"x";
		}
		else {
			Sending_with_deci_str1 = Integer.toString(Sending_with_deci);
		}
		return Sending_with_deci_str1;
	}
}

