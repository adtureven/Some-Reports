import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import TextBox

pH = 0.3
pE = 0.7
LN = 10.0
LS = 0.2


def EH(pE_S):
    pH_E = (LS * pH) / ((LS - 1) * pH + 1)
    pH_nE = (LN * pH) / ((LN - 1) * pH + 1)
    if 0 <= pE_S < pE:
        pH_S = pH_nE + (pH - pH_nE) / pE * pE_S
    elif pE <= pE_S <= 1:
        pH_S = pH + (pH_E - pH) / (1 - pE) * (pE_S - pE)
    return pH_S


func = np.vectorize(EH)
x = np.linspace(0, 1, 1000)
y = func(x)
fig, ax = plt.subplots()
ax.set_title('Bayes')
l, = plt.plot(x, y)
plt.xlim(0, 1)
plt.ylim(0, 1)
plt.xlabel('P(E|S)', x=1)
plt.ylabel('P(H|S)', y=1.05, rotation=90)
plt.subplots_adjust(bottom=0.1)
plt.show()

# def draw():
#     y = func(x)
#     l.set_ydata(y)
#     plt.draw()
#
#
# def submit1(expression):
#     global pH
#     pH = float(expression)
#     draw()
#
#
# def submit2(expression):
#     global pE
#     pE = float(expression)
#     draw()
#
#
# def submit3(expression):
#     global LS
#     LS = float(expression)
#     draw()
#
#
# def submit4(expression):
#     global LN
#     LN = float(expression)
#     draw()
#
#
# pH_box = fig.add_axes([0.15, 0.1, 0.3, 0.04])
# pH_text_box = TextBox(pH_box, "p(H)", textalignment="center")
# pH_text_box.set_val("0.5")
# pH_text_box.on_submit(submit1)
#
# pE_box = fig.add_axes([0.15, 0.05, 0.3, 0.04])
# pE_text_box = TextBox(pE_box, "p(E)", textalignment="center")
# pE_text_box.set_val("0.3")
# pE_text_box.on_submit(submit2)
#
# LS_box = fig.add_axes([0.55, 0.1, 0.3, 0.04])
# LS_text_box = TextBox(LS_box, "LS", textalignment="center")
# LS_text_box.set_val("10.0")
# LS_text_box.on_submit(submit3)
#
# LN_box = fig.add_axes([0.55, 0.05, 0.3, 0.04])
# LN_text_box = TextBox(LN_box, "LN", textalignment="center")
# LN_text_box.set_val("0.2")
# LN_text_box.on_submit(submit4)


