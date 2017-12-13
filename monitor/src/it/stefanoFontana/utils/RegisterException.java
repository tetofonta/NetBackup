package it.stefanoFontana.utils;

import javax.naming.InitialContext;
import javax.naming.NamingException;
import javax.sql.DataSource;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class RegisterException {

    private static RandomString rnd = new RandomString(100);

    public static String registerException(String message, Exception e){
        try {
            InitialContext ctx = new InitialContext();
            DataSource dataSource = (DataSource) ctx.lookup("jdbc/mysql");
            Connection conn = dataSource.getConnection();
            String key;

            do{
                key = rnd.nextString();
                PreparedStatement check = conn.prepareStatement("SELECT keyMsg FROM nbmonitor.errors WHERE keyMsg=?");
                check.setString(1, key);
                ResultSet q = check.executeQuery();
                if(!q.next()) break;
            } while(true);

            PreparedStatement ins = conn.prepareStatement("INSERT INTO nbmonitor.errors(keyMsg, message, stack) VALUES (?, ?, ?)");
            ins.setString(1, key);
            ins.setString(2, message);
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            PrintStream ps = new PrintStream(baos);
            e.printStackTrace(ps);
            ps.close();
            ins.setString(3, baos.toString());
            ins.executeUpdate();
            conn.close();
            return key;
        } catch (SQLException ex){
            return "!SQL";
        } catch (NamingException ex){
            return "!CFG";
        }
    }

}
