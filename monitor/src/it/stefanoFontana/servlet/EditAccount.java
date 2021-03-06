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
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

@WebServlet(name = "EditAccount")
public class EditAccount extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        try {
            String username = request.getParameter("username");
            InitialContext ctx = new InitialContext();
            DataSource dataSource = (DataSource) ctx.lookup("jdbc/mysql");
            Connection conn = dataSource.getConnection();

            if (request.getParameter("action").equals("ChangePassword")) {
                String oldPassword = SHAEncrypt.get_SHA_512_SecurePassword(request.getParameter("password"));
                String newPassword = SHAEncrypt.get_SHA_512_SecurePassword(request.getParameter("newpassword"));
                String repPassword = SHAEncrypt.get_SHA_512_SecurePassword(request.getParameter("reppassword"));

                if (!repPassword.equals(newPassword)) {
                    PrintWriter out = response.getWriter();
                    response.setContentType("text/html");
                    out.println("<html><body>Le password non corrispondono <br/> <a href=\"monitor.jsp\">Ritorna alla console</a></body></html>");
                    return;
                }

                String checkUser_query = "SELECT username, password FROM nbmonitor.users WHERE username=? AND password=?;";

                PreparedStatement checkUser = conn.prepareStatement(checkUser_query);
                checkUser.setString(1, username);
                checkUser.setString(2, oldPassword);
                ResultSet ret = checkUser.executeQuery();

                if (!ret.next()) {
                    PrintWriter out = response.getWriter();
                    response.setContentType("text/html");
                    out.println("<html><body>Password errata. <br/> <a href=\"monitor.jsp\">Ritorna alla console</a></body></html>");
                    return;
                }

                PreparedStatement update = conn.prepareStatement("UPDATE nbmonitor.users SET password=? WHERE username=?");
                update.setString(1, newPassword);
                update.setString(2, username);
                update.executeUpdate();

                response.sendRedirect("monitor.jsp");


            } else if (request.getParameter("action").equals("ChangeAvatar")) {

                String encoded = request.getParameter("encoding");

                PreparedStatement update = conn.prepareStatement("UPDATE nbmonitor.users SET avatar=? WHERE username=?");
                update.setString(1, encoded);
                update.setString(2, username);
                update.executeUpdate();
                response.sendRedirect("monitor.jsp");

            } else if(request.getParameter("action").equals("NewAccount")){
                PreparedStatement isadmin = conn.prepareStatement("SELECT id FROM nbmonitor.users WHERE username=? and connection_key=?");
                isadmin.setString(1, "admin");
                isadmin.setString(2, (String) ((HttpSession)request.getSession()).getAttribute("sessionKey"));
                ResultSet r = isadmin.executeQuery();
                if(r.next()){
                    String usr = request.getParameter("newuser");
                    PreparedStatement exist = conn.prepareStatement("SELECT id FROM nbmonitor.users WHERE username=?");
                    exist.setString(1, usr);
                    ResultSet a = exist.executeQuery();
                    if(!a.next()){
                        RandomString aa = new RandomString(8);
                        String pass = aa.nextString();
                        PreparedStatement insert = conn.prepareStatement("INSERT INTO nbmonitor.users(username, password) VALUES (?, ?)");
                        insert.setString(1, usr);
                        insert.setString(2, SHAEncrypt.get_SHA_512_SecurePassword(pass));
                        insert.executeUpdate();
                        PrintWriter out = response.getWriter();
                        out.println(usr);
                        out.println(pass);
                    }
                }

            } else {
                String err = RegisterException.registerException("Nessuna azione specificata", new Exception());
                response.sendRedirect("err/whoops.jsp?err=" + err);
            }
        } catch (NullPointerException e) {
            String err = RegisterException.registerException("Nessun parametro comunicato", e);
            response.sendRedirect("err/whoops.jsp?err=" + err);
        } catch (SQLException e) {
            String err = RegisterException.registerException("Errore di database", e);
            response.sendRedirect("err/whoops.jsp?err=" + err);
        } catch (NamingException e) {
            response.sendRedirect("err/whoops.jsp?err=!CFG");
        }

    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        String err = RegisterException.registerException("Protocollo insicuro", new Exception());
        response.sendRedirect("err/whoops.jsp?err=" + err);
    }
}
