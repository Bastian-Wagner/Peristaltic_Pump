# This Python file uses the following encoding: utf-8
import sys
import os
import serial
import glob
import time

from PyQt5 import QtCore, QtWidgets
import breeze_resources

from form import Ui_main


class main(QtWidgets.QMainWindow):

    def __init__(self):
        super(main, self).__init__()
        self.ui = Ui_main()
        self.ui.setupUi(self)
        #Get serial ports
        self.get_serial_ports()
        #Connect all GUI components
        self.connect_all_gui_components()
        #Gray out buttons
        self.ui.pushButton_disconnect.setEnabled(False)
        self.ui.pushButton_start.setEnabled(False)
        self.ui.pushButton_stop.setEnabled(False)
        self.ui.doubleSpinBox_flowrate.setEnabled(False)
        self.ui.doubleSpinBox_volume.setEnabled(False)
        self.ui.spinBox_calibration.setEnabled(False)


    def connect_all_gui_components(self):
        self.ui.comboBox_ports.currentIndexChanged.connect(self.set_port)

        self.ui.pushButton_connect.clicked.connect(self.connect)
        self.ui.pushButton_disconnect.clicked.connect(self.disconnect)
        self.ui.pushButton_start.clicked.connect(self.start)
        self.ui.pushButton_stop.clicked.connect(self.stop)


    # Set the port that is selected from the dropdown menu
    def set_port(self):
        self.port = self.ui.comboBox_ports.currentText()

    def get_serial_ports(self):
        """ Lists serial port names
        :raises EnvironmentError
        On unsupported or unknown platform
        :returns:
        A list of the serial ports available on the system
        """
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
                ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')

        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        self.ui.comboBox_ports.addItems(result)


    # Connect to the Arduino board
    def connect(self):
        try:
            port_declared = self.port in vars()
            try:
                self.serial = serial.Serial()
                self.serial.port = self.port
                self.serial.baudrate = 230400
                self.serial.parity = serial.PARITY_NONE
                self.serial.stopbits = serial.STOPBITS_ONE
                self.serial.bytesize = serial.EIGHTBITS
                self.serial.timeout = 1
                self.serial.open()
            #self.serial.flushInput()

            # This is a thread that always runs and listens to commands from the Arduino
            #self.global_listener_thread = Thread(self.listening)
            #self.global_listener_thread.finished.connect(lambda:self.self.thread_finished(self.global_listener_thread))
            #self.global_listener_thread.start()

                self.ui.pushButton_disconnect.setEnabled(True)
                self.ui.pushButton_start.setEnabled(True)
                self.ui.doubleSpinBox_flowrate.setEnabled(True)
                self.ui.doubleSpinBox_volume.setEnabled(True)
                self.ui.doubleSpinBox_flowrate.setEnabled(True)

                self.ui.pushButton_connect.setEnabled(False)
                time.sleep(3)
                print("Board has been connected")
            except:
                raise CannotConnectException
        except AttributeError:
            print("Please plug in the board and select a proper port, then press connect.")


    # Disconnect from the Arduino board
    def disconnect(self):
        print("Disconnecting from board..")
        time.sleep(3)
        self.serial.close()
        print("Board has been disconnected")

        self.ui.pushButton_connect.setEnabled(True)
        #Gray out buttons
        self.ui.pushButton_disconnect.setEnabled(False)
        self.ui.pushButton_start.setEnabled(False)
        self.ui.pushButton_stop.setEnabled(False)
        self.ui.doubleSpinBox_flowrate.setEnabled(False)
        self.ui.doubleSpinBox_volume.setEnabled(False)
        self.ui.spinBox_calibration.setEnabled(False)

    def start(self):
        self.flowrate = self.ui.doubleSpinBox_flowrate.value()
        self.volume = self.ui.doubleSpinBox_volume.value()
        self.conversion_mLTOsteps = self.ui.spinBox_calibration.value()
        steps_per_second = round((self.flowrate / 60) * self.conversion_mLTOsteps)
        total_steps = round(self.volume * self.conversion_mLTOsteps)
        data = str(steps_per_second) + '_' + str(total_steps)
        self.serial.write(data.encode())
        self.ui.pushButton_start.setEnabled(False)
        self.ui.pushButton_stop.setEnabled(True)
        self.ui.doubleSpinBox_flowrate.setEnabled(False)
        self.ui.doubleSpinBox_volume.setEnabled(False)
        self.ui.spinBox_calibration.setEnabled(False)
        print(data)
        #Animate ProgressBar
        self.ui.progressBar.setRange(0, 100*total_steps/self.conversion_mLTOsteps)
        self.timeLine = QtCore.QTimeLine(1000*(total_steps/steps_per_second), self) #Construct a timeline by passing its duration in milliseconds
        self.timeLine.setFrameRange(0, 100*total_steps/self.conversion_mLTOsteps)
        self.timeLine.setEasingCurve(QtCore.QEasingCurve.Linear)                    #Sets a LINEAR interpolation of the progress bar
        self.timeLine.frameChanged[int].connect(self.ui.progressBar.setValue)
        self.timeLine.start()

    def stop(self):
        data = '0_0'
        self.serial.write(data.encode())
        self.ui.pushButton_start.setEnabled(True)
        self.ui.pushButton_stop.setEnabled(False)
        self.ui.doubleSpinBox_flowrate.setEnabled(True)
        self.ui.doubleSpinBox_volume.setEnabled(True)
        self.ui.spinBox_calibration.setEnabled(True)
        self.ui.progressBar.reset()
        self.timeLine.stop()

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)

    #Set Stylesheet
    file = QtCore.QFile(":/dark.qss")
    file.open(QtCore.QFile.ReadOnly | QtCore.QFile.Text)
    stream = QtCore.QTextStream(file)
    app.setStyleSheet(stream.readAll())

    window = main()
    window.show()
    sys.exit(app.exec_())
