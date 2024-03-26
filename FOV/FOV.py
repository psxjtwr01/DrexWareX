
import tkinter as tk
from tkinter import *
import os
import json
import fade
import threading
from injector import Injector

r = tk.Tk()
r.wm_attributes("-topmost", True)
r.wm_attributes("-disabled", True)
r.wm_attributes("-transparentcolor", "SystemButtonFace")
r.configure(background="SystemButtonFace")
r.overrideredirect(True)
r.lift()


def center(win):
    win.update_idletasks()
    width = win.winfo_width()
    frm_width = win.winfo_rootx() - win.winfo_x()
    win_width = width + 2 * frm_width
    height = win.winfo_height()
    titlebar_height = win.winfo_rooty() - win.winfo_y()
    win_height = height + titlebar_height + frm_width
    x = win.winfo_screenwidth() // 2 - win_width // 2
    y = win.winfo_screenheight() // 2 - win_height // 2
    win.geometry('{}x{}+{}+{}'.format(width, height, x, y))
    win.deiconify()

def create_circle(x, y, r, canvas): #center coordinates, radius
    x0 = x - r
    y0 = y - r
    x1 = x + r
    y1 = y + r
    return canvas.create_oval(x0, y0, x1, y1, outline ='white', width=1)

def create(sizex, sizey):
    width = sizex
    height = sizey
    
    w = Canvas(r, width=width, height=height)
    
    create_circle(width / 2, height / 2, width / 2 - 2, w)
    w.quit()
    w.pack()
    center(r)
    r.mainloop()

logo = f"""
                            Crosshair       
                        made by drexxy :)                         
"""


print(fade.greenblue(logo))
os.system("title drexxy")
create(400,400)