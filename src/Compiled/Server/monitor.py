import time

import datetime

import py_backend

import remi.gui as gui
from remi import start, App
from threading import Timer

class element:
    """
    """
    def __init__(self, slot, client, percentage, files, speed, onclick):
        """

        Args:
            slot:
            client:
            percentage:
            files:
            speed:
            onclick:
        """
        self.slot = gui.Label(text=slot, style={'position': 'relative', 'top': '-150px'})
        self.client = gui.Label(text=client, style={'position': 'relative', 'top': '-150px'})
        self.perc = gui.Label(text=str(percentage) + "%", style={'font-size': '30pt', 'margin': '12px', 'position': 'relative', 'top': '-150px'})
        self.percentage = percentage
        if percentage < 0:
            self.perc.set_text("--%")
            self.percentage = 0
        self.files = gui.Label(files, style={'position': 'relative', 'top': '-150px', 'font-size': '16pt'})
        self.speed = gui.Label(speed, style={'position': 'relative', 'top': '-150px', 'font-size': '16pt'})

    def appendTo(self, lay):
        """

        Args:
            lay:
        """
        a = gui.Svg(150, 150, style={'margin': '0px'})
        self.pb = gui.SvgRectangle(0, 0, int(150 * (self.percentage/100)), 150, style={'fill': 'rgba(0, 0, 255, 0.5)', 'stroke': 'transparent'})
        a.append(self.pb)
        lay.append(a)
        lay.append(self.slot)
        lay.append(self.client)
        lay.append(self.perc)
        lay.append(self.files)
        lay.append(self.speed)

    def update(self, client, percentage, speed, files):
        """

        Args:
            client:
            percentage:
            speed:
            files:
        """
        self.client.set_text(client)
        self.perc.set_text(str(percentage) + "%")
        self.files.set_text(files)
        self.percentage = percentage
        self.pb.set_size(int(150 * (self.percentage/100)), 150)
        self.speed.set_text(speed)

class MyApp(App):
    """
    """
    def __init__(self, *args):
        """

        Args:
            args:
        """
        self.iface = py_backend.interface("127.0.0.1", 5577)
        super(MyApp, self).__init__(*args)

    elements = []

    def cmd_stp(self, widget, id):
        """

        Args:
            widget:
            id:
        """
        self.iface.stop(id)

    def cmd_toggl(self, widget, id):
        """

        Args:
            widget:
            id:
        """
        self.iface.toggl(id)

    def bak_click_listener(self, widget, id):
        """

        Args:
            widget:
            id:
        """
        dialog = gui.GenericDialog("Dettagli", message='', width='500px')

        dialog.append(gui.Label("Current Socket: " + str(self.q[id].socket)))
        dialog.append(gui.Label("TCP Port: " + str(self.q[id].port)))
        dialog.append(gui.Label("Client: " + str(self.q[id].connected[3]) + "." + str(self.q[id].connected[2]) + "." + str(self.q[id].connected[1]) + "." +str(self.q[id].connected[0])))
        dialog.append(gui.Label("Transferred " + str(self.q[id].transferred) + " Bytes from file " + str(self.q[id].transferredFiles) + " of " + str(self.q[id].totalFiles)))
        dialog.append(gui.Label("Started " + str(datetime.datetime.fromtimestamp(self.q[id].started).strftime('%Y-%m-%d %H:%M:%S'))))

        foo = gui.Widget(layout_orientation=gui.Widget.LAYOUT_HORIZONTAL, style={'align': 'centre'})
        stop = gui.Button(text="STOP")
        stop.set_on_click_listener(self.cmd_stp, id)
        foo.append(stop)
        pause = gui.Button(text="PAUSA/RIPRENDI")
        pause.set_on_click_listener(self.cmd_toggl, id)
        foo.append(pause)

        dialog.append(foo)

        dialog.show(self)

    def on_menu_click(self, widget, view, list):
        """

        Args:
            widget:
            view:
            list:
        """
        for i in list:
            i.style['display'] = 'none'
        view.style['display'] = 'block'


    def main(self):
        """

        Returns:

        """

        views = []

        page = gui.Widget(margin='0px', style={'display': 'block', 'top': '0px', 'position': 'absolute'})

# INTESTAZIONE

        menu_wrapper = gui.Widget(width='100%', layout_orientation=gui.Widget.LAYOUT_HORIZONTAL, margin='0px', style={'position': 'fixed', 'background-color': '#0000FF', 'z-index': '100'})

        menu_wrapper.append(gui.Image("/res/logo.png",  width=100, height=100, margin='15px'))

        # men = gui.Menu(height='75px', layout_orientation=gui.Widget.LAYOUT_HORIZONTAL)
        # backups_view = gui.MenuItem(text="Trasferimenti in corso")
        # config_view = gui.MenuItem(text="Configurazione")
        # men.append(backups_view)
        # men.append(config_view)
        # menu_wrapper.append(men)

        page.append(menu_wrapper)

# /INTESTAZIONE/

# CORPO
        corpo = gui.Widget(style={'position': 'absolute', 'top': '130px', 'width': '95vw'})
# BACKUPS
        bak_container = gui.Widget(layout_orientation=gui.Widget.LAYOUT_HORIZONTAL, margin='3%', style={'display': 'block', 'overflow': 'hidden'})
        for i in range(0, self.iface.get_backups):
            cnt = gui.Widget(width=150, height=150, margin='10px', style={'display': 'block', 'box-shadow': '2px 2px 2px #888888', 'justify-content': 'centre'})
            q = element(str(i), "CLOSED", -1, "0 / 0", "0B/s", 3)
            self.elements.append(q)
            q.appendTo(cnt)
            cnt.set_on_click_listener(self.bak_click_listener, i)
            bak_container.append(cnt)
        self.update()
        views.append(bak_container)
        corpo.append(bak_container)
# /BACKUPS/

        # backups_view.set_on_click_listener(self.on_menu_click, bak_container, views)

        page.append(corpo)
# /CORPO/

        self.on_menu_click(None, bak_container, views)
        return page

    def update(self):
        """

        """
        self.q = self.iface.getBackupList()
        q = self.q

        for i in range(0, self.iface.get_backups):
            try:
                rapporto = int(int(q[i].transferred/q[i].dimension * 100) * (q[i].transferredFiles/q[i].totalFiles ))
            except ZeroDivisionError:
                rapporto = 0

            host = ""
            for n in range(0, 4):
                host += str(q[i].connected[3-n])
                if n != 3: host += "."
            deltaTrans = q[i].transferred - q[i].lastTrans
            if deltaTrans < 0:
                deltaTrans = -deltaTrans
            if rapporto >= 100: deltaTrans = 0
            q[i].lastTrans = q[i].transferred
            self.elements[i].update(host, rapporto, str(q[i].transferredFiles) + " / " + str(q[i].totalFiles), str(2*deltaTrans/1000000)[:6] + " MB/s")

        Timer(0.5, self.update).start()


if __name__ == "__main__":

    start(MyApp,
          address='0.0.0.0',
          port=5566,
          multiple_instance=True,
          enable_file_cache=True,
          update_interval=0.1,
          start_browser=False,
          https=True,
          certfile='./server.pem',
          ignoreSIGINT = False,
          username="Stefano",
          password="Sste2mila")
