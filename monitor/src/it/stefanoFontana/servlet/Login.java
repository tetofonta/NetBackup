package it.stefanoFontana.servlet;

import it.stefanoFontana.utils.RandomString;
import it.stefanoFontana.utils.RegisterException;
import it.stefanoFontana.utils.SHAEncrypt;

import javax.naming.InitialContext;
import javax.naming.NamingException;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import javax.sql.DataSource;
import java.io.*;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

@WebServlet(name = "Login")
public class Login extends HttpServlet {

    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        try {
            String user = request.getParameter("username");
            String password = SHAEncrypt.get_SHA_512_SecurePassword(request.getParameter("password"));

            String checkUser_query = "SELECT username, password FROM nbmonitor.users WHERE username=? AND password=?;";
            InitialContext ctx = new InitialContext();
            DataSource dataSource = (DataSource) ctx.lookup("jdbc/mysql");
            Connection conn = dataSource.getConnection();

            PreparedStatement checkUser = conn.prepareStatement(checkUser_query);
            checkUser.setString(1, user);
            checkUser.setString(2, password);
            ResultSet ret = checkUser.executeQuery();

            if(ret.next()){
                String sessionKey = new RandomString(128).nextString();
                HttpSession session = request.getSession();
                session.setAttribute("sessionKey", sessionKey);
                session.setAttribute("username", user);
                String query = "UPDATE nbmonitor.users SET connection_key=?, lastlogin=? WHERE username=? AND password=?";
                checkUser = conn.prepareStatement(query);
                checkUser.setString(1, sessionKey);
                checkUser.setInt(2, (int) (System.currentTimeMillis() / 1000L));
                checkUser.setString(3, user);
                checkUser.setString(4, password);
                checkUser.executeUpdate();
                response.sendRedirect("monitor.jsp");
                conn.close();
                return;
            }

            String err = RegisterException.registerException("Credenziali errate per l'accesso di " + user, new Exception());
            response.sendRedirect("err/whoops.jsp?err=" + err);
            conn.close();

        } catch (SQLException e) {
            String err = RegisterException.registerException("Errore di database", e);
            response.sendRedirect("err/whoops.jsp?err=" + err);
        } catch (NamingException e) {
            response.sendRedirect("err/whoops.jsp?err=!CFG");
        } catch (NullPointerException e){
            String err = RegisterException.registerException("Nessun parametro inviato", e);
            response.sendRedirect("err/whoops.jsp?err=" + err);
        }
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        String err = RegisterException.registerException("Protocollo insicuro", new Exception());
        response.sendRedirect("err/whoops.jsp?err=" + err);
    }

}
