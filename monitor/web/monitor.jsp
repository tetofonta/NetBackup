<%--
  Created by IntelliJ IDEA.
  User: stefano
  Date: 09/12/17
  Time: 16.45
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<%@ page import="javax.naming.InitialContext" %>
<%@ page import="javax.sql.DataSource" %>
<%@ page import="java.sql.Connection" %>
<%@ page import="java.sql.PreparedStatement" %>
<%@ page import="java.sql.ResultSet" %>
<%
    String username = "errored";
    String key = "";
    String avatar = null;
    try {
        username = (String) session.getAttribute("username");
        key = (String) session.getAttribute("sessionKey");
        InitialContext ctx = new InitialContext();
        DataSource dataSource = (DataSource) ctx.lookup("jdbc/mysql");
        Connection conn = dataSource.getConnection();

        PreparedStatement q = conn.prepareStatement("SELECT username, avatar FROM nbmonitor.users WHERE username=? AND connection_key=?");
        q.setString(1, username);
        q.setString(2, key);
        ResultSet res = q.executeQuery();
        if (!res.next()) {
            response.sendRedirect("err/whoops.jsp");
        }

        res.beforeFirst();
        res.next();

        avatar = res.getString("avatar");

        conn.close();

    } catch (Exception e) {
        response.sendRedirect("err/whoops.jsp");
    }
%>

<html>
<head>
    <title>Monitor - <%= username%>
    </title>
    <jsp:include page="templates/include.jsp"/>
</head>
<body>

<nav class="blue">
    <div class="nav-wrapper container">
        <a href="Logout" class="brand-logo right"><img style="height: 60px; padding: 5px;" src="img/backup.svg"
                                                       alt="Logo"></a>
        <ul id="nav-mobile" class="left hide-on-med-and-down">
            <li>
                <a class="modal-trigger" href="#account">
                    <p>
                    <div class="chip">
                        <img src="data:image/png;base64, <%=avatar%>" alt="Contact Person">
                        <%=username%>
                    </div>
                    </p>
                </a>
            </li>
            <li><a href="#" onclick="
            document.getElementById('percentages').style.display = 'block';
            document.getElementById('fileServer').style.display = 'none';
">Stato Globale</a></li>
            <%--<li><a href="#" onclick="--%>
            <%--document.getElementById('percentages').style.display = 'none';--%>
            <%--document.getElementById('fileServer').style.display = 'block';--%>
            <%--">File Server</a></li>--%>
            <%
                if (username.equals("admin")) {
            %>
            <li>
                <a class="modal-trigger" href="#accounts">Gestione Accounts</a>
            </li>
            <%
                }
            %>
        </ul>
    </div>
</nav>
<div class="container" id="percentages">

    <h3>Backup in corso</h3>
    <div id="progresses">
        Nessun Backup in corso
    </div>

    <h3>Backup completati</h3>
    <div id="completed">
        Nessun backup completato
    </div>
</div>

<div class="container" id="fileServer" style="display: none">

</div>

<!-- Show user nfos-->
<div id="account" class="modal">
    <div class="modal-content ">
        <div class="row">Username: <%=username%>
        </div>
        <div class="row"><a class="btn waves-light modal-trigger" href="#chPassword">Cambia Password</a></div>
        <div class="row valign-wrapper">
            <img class="responsive-img circle" src="data:image/png;base64, <%=avatar%>"/>
            <a class="btn waves-light modal-trigger" href="#chAvatar">Cambia Avatar</a>
        </div>
    </div>
    <div class="modal-footer">
        <a href="#!" class="modal-action modal-close waves-effect waves-green btn-flat">Annulla</a>
    </div>
</div>

<div id="chPassword" class="modal">
    <div class="modal-content">
        <form action="EditAccount" method="post">
            <input type="hidden" name="action" value="ChangePassword">
            <input type="hidden" name="username" value="<%=username%>">
            <div class="row">
                <div class="input-field col s12">
                    <input id="password" type="password" name="password">
                    <label for="password">Vecchia Password</label>
                </div>
            </div>
            <div class="row">
                <div class="input-field col s12">
                    <input id="newpassword" type="password" name="newpassword">
                    <label for="newpassword">Nuova Password</label>
                </div>
            </div>
            <div class="row">
                <div class="input-field col s12">
                    <input id="reppassword" type="password" name="reppassword">
                    <label for="reppassword">Ripeti Nuova Password</label>
                </div>
            </div>
            <div class="modal-footer">
                <button type="submit" class="btn waves-light">Cambia Password</button>
            </div>
        </form>
    </div>
</div>

<div id="chAvatar" class="modal">
    <div class="modal-content">
        <div class="row"><input onchange="getencode()" type="file" name="file" id="file"></div>
        <form action="EditAccount" method="post">
            <input type="hidden" name="action" value="ChangeAvatar">
            <input type="hidden" name="encoding" id="encoding">
            <input type="hidden" name="username" value="<%=username%>">
            <div class="modal-footer">
                <button type="submit" class="btn waves-light">Cambia Avatar</button>
            </div>
        </form>
    </div>
</div>

<div id="accounts" class="modal">
    <div class="modal-content">
        <h5>Nuovo account</h5>
        <form action="EditAccount" method="post" target="_blank">
            <input type="hidden" value="NewAccount" name="action">
            <div class="row">
                <div class="input-field col s12">
                    <input id="username" type="password" name="newuser">
                    <label for="username">Nome utente</label>
                </div>
            </div>
            <div class="modal-footer">
                <button type="submit" class="btn waves-light">Crea</button>
            </div>
        </form>
    </div>
</div>

</body>

<script type="text/javascript" src="js/jquery.min.js"></script>
<script type="text/javascript" src="js/materialize.min.js"></script>
<script>

    function getencode() {

        var file = document.getElementById('file').files[0];
        var reader = new FileReader();

        reader.onload = function (readerEvt) {
            var binaryString = readerEvt.target.result;
            document.getElementById("encoding").value = btoa(binaryString);
        };

        reader.readAsBinaryString(file);
    }

    var inited = false;
    var conn = new WebSocket(window.location.href.replace("monitor.jsp", "connect").replace("http://", "ws://").replace("https://", "wss://"));

    conn.onopen = function (event) {
        inited = true;
    };

    conn.onmessage = function (event) {
        var splitted = event.data.split(";");
        if (splitted[0] === "DESC") {
            if (splitted.length !== 2) {
                var contenitore = "";
                var completi = "";

                var array;
                for (i = 1; i < splitted.length - 1; i++) {

                    array = splitted[i].split("|");
                    var transferred = array[4];
                    var totaldimension = array[3];
                    var files = array[6];
                    var totalfiles = array[5]

                    var file_percentages = transferred / totaldimension;
                    var overallPercentage = (files / totalfiles + (file_percentages * (1 / totalfiles))) * 100;

                    if (files === totalfiles) overallPercentage = 100;

                    overallPercentage = Math.ceil(overallPercentage);
                    file_percentages = Math.ceil(file_percentages * 100);

                    var prog = "<div class=\"row\">\n" +
                        "<div class=\"card white darken-1 hoverable\">\n" +
                        "<div class=\"card-content\">\n" +
                        "<div class=\"row\">\n" +
                        "<div class=\"col s2\">\n" +
                        "pid: " + array[2] + " <br/>\n" +
                        "port: " + array[1] + "\n" +
                        "<h5>" + array[8] + "</h5>\n" +
                        "</div>\n" +
                        "<div class=\"col s1\">\n" +
                        "<div>\n" +
                        "<i class=\"material-icons\">" + (array[7] === "1" ? "pause" : "play_arrow") + "</i>\n" +
                        "</div>\n" +
                        "</div>\n" +
                        "File Corrente: <br/>\n" +
                        "<div class=\"col s9\">\n" +
                        "<div class=\"row\">\n" +
                        "<div class=\"progress s12\">\n" +
                        "<div class=\"determinate\" style=\"width: " + file_percentages + "%\"></div>\n" +
                        "</div>\n" +
                        "</div>\n" +
                        "Totale: <br/>\n" +
                        "<div class=\"row\">\n" +
                        "<div class=\"progress s12\">\n" +
                        "<div class=\"determinate\" style=\"width: " + overallPercentage + "%\"></div>\n" +
                        "</div>\n" +
                        "</div>\n" +
                        "</div>\n" +
                        "</div>\n" +
                        "</div>\n" +
                        "<div class='card-action'>" +
                        "<a onclick='toggle("+ array[8] +")'><i class='material-icons'> " + (array[7] === "1" ? "play_arrow" : "pause") + "</i></a>" +
                        "<a onclick='stahp("+ array[8] +")'><i class='material-icons'>stop</i></a>" +
                        "</div>\n" +
                        "</div>\n" +
                        "</div>\n";

                    if (array[0] !== "-10") contenitore += prog;
                    else completi += prog;
                }

                document.getElementById("progresses").innerHTML = contenitore;
                document.getElementById("completed").innerHTML = completi;

            } else {
                document.getElementById("progresses").innerHTML = "Nessun Backup in corso";
            }
        }
    };

    conn.onerror = function () {
        $('#progresses').innerHTML = "WEBSOCKET_ERROR"
    };

    function toggle(id) {
        try {
            conn.send("<%=key%>/<%=username%>/" + "TOG" + id);
        } catch (e) {
            alert("Impossibile inviare");
        }
    }

    function stahp(id){
        try {
            conn.send("<%=key%>/<%=username%>/" + "STP" + id);
        } catch (e) {
            alert("Impossibile inviare");
        }
    }

    function update() {
        try {
            conn.send("<%=key%>/<%=username%>/" + "GET");
        } catch (e) {
//            try {
//                conn.close()
//            } catch (e) {
//            }
//            conn = new WebSocket(window.location.href.replace("monitor.jsp", "connect").replace("http://", "ws://").replace("https://", "wss://"));
        }
        setTimeout(update, 1000);
    }

    $(document).ready(function () {
        $('.modal').modal();

    });

    update();
</script>
</html>
