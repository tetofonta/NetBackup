package it.stefanoFontana.networking;

import it.stefanoFontana.networking.Structs.BackupThread;

import java.io.IOException;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Connector {
    DatagramSocket clientSocket;
    InetAddress ip;

    byte[] receiveData = new byte[1024];

    public static final String AskPacket = "moGET";
    public static final String AskNo = "moNMR";
    public static final String Toggle = "moTOG";

    private Connector() throws UnknownHostException, SocketException {
        clientSocket = new DatagramSocket();
        ip = InetAddress.getByName("localhost");

    }

    private static Connector instance = null;

    public static Connector getInstance() throws UnknownHostException, SocketException {
        if (instance == null) instance = new Connector();
        return instance;
    }

    public void command(String cmd){
        try {
            DatagramPacket sendPacket = new DatagramPacket(cmd.getBytes(), cmd.getBytes().length, ip, 5577); //TODO: specifica in properties
            clientSocket.send(sendPacket);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public int getCapableOf(){
        try {
            DatagramPacket sendPacket = new DatagramPacket(AskNo.getBytes(), AskNo.getBytes().length, ip, 5577); //TODO: specifica in properties
            clientSocket.send(sendPacket);
            DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
            clientSocket.receive(receivePacket);
            final BackupThread data = new BackupThread();
            final ByteBuffer buf = ByteBuffer.wrap(receivePacket.getData());
            buf.order(ByteOrder.LITTLE_ENDIAN);
            return buf.getInt();
        } catch (IOException e){
            return 0;
        }
    }

    public BackupThread[] askStatus() throws IOException {
        //TODO: timeout

        int len = getCapableOf();
        BackupThread[] ret = new BackupThread[len]; //TODO: specifica in properties

        DatagramPacket sendPacket = new DatagramPacket(AskPacket.getBytes(), AskPacket.getBytes().length, ip, 5577); //TODO: specifica in properties
        clientSocket.send(sendPacket);

        for (int i = 0; i < len; i++) {
            DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
            clientSocket.receive(receivePacket);
            ret[i] = BackupThread.createFromBytes(receivePacket.getData(), i);
        }

        return ret;
    }


}
