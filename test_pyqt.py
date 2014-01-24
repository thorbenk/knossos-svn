from PyQt5.QtWidgets import QWidget, QApplication
import sys
from pyknossos import TestingWidget

app = QApplication(sys.argv)

#t = TestingWidget(None)
#t.show()

t = QWidget()
t.show()

app.exec_()
