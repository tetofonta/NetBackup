package it.stefanoFontana.networking.Structs;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class BackupThread {
    public int id;
    public int socket; ///< socket del processo di backup
    public int port; ///< porta di backup
    public int pid; ///< pid del processo forkato
    public short family;
    public byte[] ip = new byte[14];
    public long dimension; ///< dimensione di trasmissione
    public long transferred; ///< dimensione trasmessa all'istante della lettura
    public int numberOfFiles; ///< numero di file totali
    public int filesTransferred; ///< numero dfi file trasferiti all'istante di lettura
    public int startedInTime; ///< data di avvio
    public int status; ///< stato (0) running (1) pause

    public static BackupThread createFromBytes(byte[] bytes, int id) throws IOException {
        final BackupThread data = new BackupThread();
        final ByteBuffer buf = ByteBuffer.wrap(bytes);

        data.id = id;

        // If needed...
        buf.order(ByteOrder.LITTLE_ENDIAN);

        data.socket = buf.getInt();
        data.port = buf.getInt();
        data.pid = buf.getInt();
        data.family = buf.getShort();
        buf.get(data.ip);
        data.dimension = buf.getLong();
        data.transferred = buf.getLong();
        data.numberOfFiles = buf.getInt();
        data.filesTransferred = buf.getInt();
        data.startedInTime = buf.getInt();
        data.status = buf.getInt();

        return data;
    }

    @Override
    public String toString() {
        return "" + socket + "|" + port + "|" + pid + "|" + dimension + "|" + transferred + "|" + numberOfFiles + "|" + filesTransferred + "|" + status + "|" + id;
    }
}
