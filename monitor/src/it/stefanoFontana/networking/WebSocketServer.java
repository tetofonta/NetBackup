package it.stefanoFontana.networking;

import com.sun.corba.se.spi.extension.ZeroPortPolicy;
import it.stefanoFontana.networking.Structs.BackupThread;

import javax.naming.InitialContext;
import javax.naming.NamingException;
import javax.sql.DataSource;
import javax.websocket.*;
import javax.websocket.server.ServerEndpoint;
import java.io.IOException;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

@ServerEndpoint(value = "/connect")
public class WebSocketServer {

    Connector conn;

    @OnOpen
    public void onOpen(Session session) {
        try {
            conn = Connector.getInstance();
        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @OnMessage
    public String onMessage(String message, Session session) {

        String[] splitted = message.split("/");
        if (splitted.length < 3) return "WRONG CREDENTIALS";
        String userkey = splitted[0];
        String username = splitted[1];
        String command = splitted[2];
        try {
            InitialContext ctx = new InitialContext();
            DataSource dataSource = (DataSource) ctx.lookup("jdbc/mysql");
            Connection connect = dataSource.getConnection();

            PreparedStatement checkUser = connect.prepareStatement("SELECT id FROM nbmonitor.users WHERE connection_key=? AND username=?");
            checkUser.setString(1, userkey);
            checkUser.setString(2, username);
            ResultSet retrn = checkUser.executeQuery();

            if (retrn.next()) {
                if(command.contains("TOG"))
                    conn.command(command);
                else if(command.contains("STP"))
                    conn.command(command);
                else if (command.equals("GET")) {

                    BackupThread[] bak;
                    try {
                        bak = conn.askStatus();
                    } catch (IOException e) {
                        return "Error";
                    }

                    StringBuilder ret = new StringBuilder();
                    ret.append("DESC;");


                    for (BackupThread b :
                            bak) {
                        if (b.socket != -10 || (b.filesTransferred == b.numberOfFiles && b.numberOfFiles > 0)) {
                            ret.append(b);
                            ret.append(";");
                        }
                    }

                    return ret.toString();
                }

                return "Unknow Command";
            }
        } catch (SQLException e) {
            e.printStackTrace();
        } catch (NamingException e) {
            e.printStackTrace();
        }
        return "Wrong Credentials";
    }

    @OnClose
    public void onClose(Session session) {

    }

    @OnError
    public void onError(Throwable exception, Session session) {

    }
}