<%--
  Created by IntelliJ IDEA.
  User: stefano
  Date: 08/12/17
  Time: 20.03
  To change this template use File | Settings | File Templates.
--%>
<%@page contentType="text/html" pageEncoding="UTF-8" %>
<%
    session.invalidate();
%>
<!DOCTYPE html>
<html>
<head>
    <jsp:include page="templates/include.jsp"/>
</head>

<body class="white-text center-align" style="background-color: #d2d2ff">

<nav class="blue">
    <div class="nav-wrapper container">
        <a href="index.jsp" class="brand-logo right"><img style="height: 60px; padding: 5px;" src="img/backup.svg" alt="Logo"></a>
    </div>
</nav>

<div id="login-page" class="container valign-wrapper center-align">
    <div class="container">
        <div class="col">
            <div class="card white">
                <div class="card-content black-text">
                    <div class="container">
                        <div class="row" >
                        <img src="img/backup.svg" width="200px" alt="NetBackup by StefanoFontana"/>
                    </div>
                    </div>
                    <form action="Login" method="post">

                            <div class="row margin">
                                <div class="input-field col s12">
                                    <input id="username" type="text" name="username">
                                    <label for="username" class="center-align">Username</label>
                                </div>
                            </div>
                            <div class="row margin">
                                <div class="input-field col s12">
                                    <input id="password" type="password" name="password">
                                    <label for="password">Password</label>
                                </div>
                            </div>
                            <div class="row">
                                <div class="input-field col s12">
                                    <button type="submit" class="btn light-blue waves-effect waves-light col s4 m4">Login</button>
                                </div>
                            </div>
                    </form>
                </div>
            </div>
        </div>
    </div>
</div>
</body>

<jsp:include page="templates/finalize.jsp"/>
</html>
