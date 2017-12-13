package it.stefanoFontana.servlet;

import it.stefanoFontana.utils.RegisterException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import javax.websocket.Session;
import java.io.IOException;

@WebServlet(name = "Logout")
public class Logout extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        HttpSession sex = request.getSession();
        sex.setAttribute("username", "");
        sex.setAttribute("sessionKey", "");
        sex.invalidate();
        response.sendRedirect("index.jsp");
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        HttpSession sex = request.getSession();
        sex.setAttribute("username", "");
        sex.setAttribute("sessionKey", "");
        sex.invalidate();
        response.sendRedirect("index.jsp");
    }
}
