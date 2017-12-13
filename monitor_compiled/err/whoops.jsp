<%@ page import="javax.naming.InitialContext" %>
<%@ page import="javax.naming.NamingException" %>
<%@ page import="javax.sql.DataSource" %>
<%@ page import="java.sql.Connection" %>
<%@ page import="java.sql.PreparedStatement" %>
<%@ page import="java.sql.ResultSet" %>
<%@ page import="java.sql.SQLException" %>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Errore</title>
    <link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
    <link type="text/css" rel="stylesheet" href="../css/materialize.min.css" media="screen,projection"/>
    <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
</head>
<body style="background-color: #f4f4f4">

<nav class="blue">
    <div class="nav-wrapper container">
        <a href="../index.jsp" class="brand-logo right"><img style="height: 60px; padding: 5px;" src="../img/backup.svg" alt="Logo"></a>
    </div>
</nav>

<div class="container">
    <div class="valign-wrapper">
    <div class="col center-align">
        <div class="row">
            <img src="../img/seo.gif">
        </div>
        <div class="row">
            <h2>Whoops, that's embarrassing!</h2>
        </div>
        <div class="row">
            <%
                String key = request.getParameter("err");
                if (key == null)
                    out.println("<p>Messaggio: Errore indefinito. </p>");
                else if (key.equals("!SQL"))
                    out.println("<p>Messaggio: Errore di connessione al database. Contatta un'amministratore di rete. </p>");
                else if (key.equals("!CFG"))
                    out.println("<p>Messaggio: Errore di configurazione. Contatta un'amministratore di rete. </p>");
                else {
                    try {
                        InitialContext ctx = new InitialContext();
                        DataSource dataSource = (DataSource) ctx.lookup("jdbc/mysql");
                        Connection conn = dataSource.getConnection();

                        PreparedStatement getError = conn.prepareStatement("SELECT message FROM nbmonitor.errors WHERE keyMsg=?");
                        getError.setString(1, key);
                        ResultSet ret = getError.executeQuery();

                        ret.beforeFirst();
                        ret.next();

                        out.println("<p>Codice errore: " + key + " </p>");
                        out.println("<p>Messaggio: " + ret.getString("message") + " </p>");

                    } catch (SQLException ex) {
                    } catch (NamingException ex) {
                    }

                }
            %>
        </div>
    </div>
    </div>
</div>
</body>

<script type="text/javascript" src="../js/jquery.min.js"></script>
<script type="text/javascript" src="../js/materialize.min.js"></script>

</html>