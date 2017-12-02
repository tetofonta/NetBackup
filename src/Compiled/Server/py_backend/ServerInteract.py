import socket
from ctypes import *

class backupThread:
    """
    """

    def __init__(self, socket=-10, port=0, connected=0, dimension=0, transferred=0, totalFiles=0, transferredFiles=0, started=0, q=0, status=1):
        """

        Args:
            socket:
            port:
            connected:
            dimension:
            transferred:
            totalFiles:
            transferredFiles:
            started:
            q:
            status:
        """
        self.socket = socket
        self.port = port
        self.connected = [(connected >> 24) & 0x000000FF, (connected >> 16) & 0x000000FF, (connected >> 8) & 0x000000FF, (connected) & 0x000000FF]
        self.dimension = dimension
        self.transferred = transferred
        self.totalFiles = totalFiles
        self.transferredFiles = transferredFiles
        self.started = started
        self.lastTrans = q
        self.stat = status

class interface:
    """
    """

    __GET_NUMBER = b'moNMR'
    __GET_LIST = b'moGET'
    __STOP = b'moSTP'
    __TOG = b'moTOG'

    def __init__(self, ip, port):
        """

        Args:
            ip:
            port:
        """
        self._ip = ip
        self._port = port
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        self._sock.sendto(self.__GET_NUMBER, (self._ip, self._port))
        self._backups, foo = self._sock.recvfrom(4, 0)
        self._backups = int.from_bytes(self._backups[0:], byteorder='little')

    oldBaks = []
    def getBackupList(self):
        """

        Returns:

        """
        backups = []
        self._sock.sendto(self.__GET_LIST, (self._ip, self._port))
        for i in range(0, self._backups):
            q = 0
            try:
                q = self.oldBaks[i].lastTrans
            except:
                pass

            recived, foo = self._sock.recvfrom(56, 0)
            backups.append(
                backupThread(
                    int.from_bytes(recived[0:4], byteorder='little', signed=True),
                    int.from_bytes(recived[4:8], byteorder='little'),
                    int.from_bytes(recived[12:16], byteorder='little'),
                    int.from_bytes(recived[24:32], byteorder='little'),
                    int.from_bytes(recived[32:40], byteorder='little'),
                    int.from_bytes(recived[40:44], byteorder='little'),
                    int.from_bytes(recived[44:48], byteorder='little'),
                    int.from_bytes(recived[48:56], byteorder='little'),
                    q,
                    int.from_bytes(recived[56:60], byteorder='little')
                    ))
        self.oldBaks = backups
        return backups

    @property
    def get_backups(self):
        """

        Returns:

        """
        return self._backups

    def stop(self, id):
        """

        Args:
            id:
        """
        self._sock.sendto(self.__STOP + str(id).encode('ascii'), (self._ip, self._port))

    def toggl(self, id):
        """

        Args:
            id:
        """
        self._sock.sendto(self.__TOG + str(id).encode('ascii'), (self._ip, self._port))


