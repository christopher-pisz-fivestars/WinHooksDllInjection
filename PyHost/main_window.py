import os.path
import threading
import time
import win32con
import win32api
import wx

from ctypes import wintypes
from ctypes import *


class MainWindow(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title, size=(800, 600))
        self.CreateStatusBar()

        menu_file = wx.Menu()
        menu_item_exit = menu_file.Append(wx.ID_EXIT, "E&xit", " Terminate the program")

        menu_help = wx.Menu()
        menu_item_about = menu_help.Append(wx.ID_ABOUT, "&About", " Information about this program")

        menu_bar = wx.MenuBar()
        menu_bar.Append(menu_file, "&File")
        menu_bar.Append(menu_help, "&Help")
        self.SetMenuBar(menu_bar)

        self.panel = MainPanel(self)

        self.Bind(wx.EVT_MENU, self.on_about, menu_item_about)
        self.Bind(wx.EVT_MENU, self.on_exit, menu_item_exit)

        self.Show(True)

    def on_about(self, e):
        dlg = wx.MessageDialog(self, "A window to test Windows Hooks", "About Test Windows Hooks",
                               wx.OK)
        dlg.ShowModal()
        dlg.Destroy()

    def on_exit(self, e):
        self.Close(True)


class MainPanel(wx.Panel):
    def __init__(self, parent):
        self.doing_task_event_loop = False
        self.doing_task_callback = False
        self.called_back_count = 0

        wx.Panel.__init__(self, parent)
        self.textbox = wx.TextCtrl(self, style=wx.TE_MULTILINE | wx.TE_READONLY)
        self.button_task_event_loop = wx.Button(self, label="Start task - event loop")
        self.button_task_callback = wx.Button(self, label="Start task - callback")

        self.horizontal = wx.BoxSizer()
        self.horizontal.Add(self.textbox, proportion=1, flag=wx.EXPAND)

        self.horizontal2 = wx.BoxSizer()
        self.horizontal2.Add(self.button_task_event_loop)
        self.horizontal2.Add(self.button_task_callback)

        self.sizer_vertical = wx.BoxSizer(wx.VERTICAL)
        self.sizer_vertical.Add(self.horizontal, proportion=1, flag=wx.EXPAND)
        self.sizer_vertical.Add(self.horizontal2, proportion=1, flag=wx.CENTER)
        self.SetSizerAndFit(self.sizer_vertical)

        self.Bind(wx.EVT_BUTTON, self.on_click_button_task_event_loop, self.button_task_event_loop)
        self.Bind(wx.EVT_BUTTON, self.on_click_button_task_callback, self.button_task_callback)

        # Set callback from windows hook procedure to our python code
        dll_name = "FSHooks.dll"
        dll_abspath = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'Win32',
                                                   'Debug', dll_name))
        dll_handle = cdll.LoadLibrary(dll_abspath)
        self.callback_type = CFUNCTYPE(None, c_int, c_int, c_int, c_bool)
        self.callback = self.callback_type(self.callback_from_c)

        dll_handle.Init()
        dll_handle.SetCallback(self.callback)
        # TODO - Release the hooks when window closes

        # Register the hook
        dll_handle.HookProc.argtypes = (c_int, wintypes.WPARAM, wintypes.LPARAM)
        hook_proc_pointer = dll_handle.HookProc
        self.hook_id = windll.user32.SetWindowsHookExA(win32con.WH_MOUSE_LL, hook_proc_pointer,
                                                       win32api.GetModuleHandle(None), 0)

        self.textbox.AppendText('Panel created on thread: {}\n'.format(
            threading.current_thread().ident))

    def callback_from_c(self, message_type, x, y, injected):
        self.textbox.AppendText(
            'Message: {} X: {} Y: {} Injected: {} Thread: {}\n'.format(
                message_type, x, y, injected, threading.current_thread().ident))
        if self.doing_task_callback:
            time.sleep(3)

    def on_click_button_task_event_loop(self, event):
        if self.doing_task_event_loop:
            self.doing_task_event_loop = False
            self.button_task_event_loop.SetLabel('Start task - event loop')
            self.textbox.AppendText('Stopping long task in the event loop...\n')
        else:
            self.doing_task_event_loop = True
            self.button_task_event_loop.SetLabel('Stop task - event loop')
            self.textbox.AppendText('Starting long task in the event loop...\n')
            from twisted.internet import reactor
            reactor.callLater(0.25, self.long_task_event_loop)

    def on_click_button_task_callback(self, event):
        if self.doing_task_callback:
            self.doing_task_callback = False
            self.button_task_callback.SetLabel('Start task - callback')
            self.textbox.AppendText('Stopping long task in the callback...\n')
        else:
            self.doing_task_callback = True
            self.button_task_callback.SetLabel('Stop task - callback')
            self.textbox.AppendText('Starting long task in callback...\n')

    def long_task_event_loop(self):
        if self.doing_task_event_loop:
            time.sleep(3)
            from twisted.internet import reactor
            reactor.callLater(0.25, self.long_task_event_loop)

