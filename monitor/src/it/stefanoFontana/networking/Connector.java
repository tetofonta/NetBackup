package it.stefanoFontana.networking;

import it.stefanoFontana.networking.Structs.BackupThread;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Properties;

public class Connector {
    DatagramSocket clientSocket;
    InetAddress ip;

    byte[] receiveData = new byte[1024];

    public static final String AskPacket = "moGET";
    public static final String AskNo = "moNMR";
    public static final String Toggle = "moTOG";
    public static final String config = "web.properties";
    private Properties settings = new Properties();

    private int port;

    private Connector() throws Exception {

        FileInputStream in = new FileInputStream("web.properties");
        settings.load(in);

        clientSocket = new DatagramSocket();
        ip = InetAddress.getByName(settings.getProperty("server", "localhost"));
        port = Integer.parseInt(settings.getProperty("port", "5577"));
    }

    private static Connector instance = null;

    public static Connector getInstance() throws Exception {
        if (instance == null) instance = new Connector();
        return instance;
    }

    public void command(String cmd){
        cmd = "mo" + cmd;
        try {
            DatagramPacket sendPacket = new DatagramPacket(cmd.getBytes(), cmd.getBytes().length, ip, port); //TODO: specifica in properties
            clientSocket.send(sendPacket);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public int getCapableOf(){
        try {
            DatagramPacket sendPacket = new DatagramPacket(AskNo.getBytes(), AskNo.getBytes().length, ip, port); //TODO: specifica in properties
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
