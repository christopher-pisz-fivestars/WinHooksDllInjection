import os.path
import threading
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
        self.consuming = False
        self.called_back_count = 0

        wx.Panel.__init__(self, parent)
        self.textbox = wx.TextCtrl(self, style=wx.TE_MULTILINE | wx.TE_READONLY)

        self.horizontal = wx.BoxSizer()
        self.horizontal.Add(self.textbox, proportion=1, flag=wx.EXPAND)

        self.sizer_vertical = wx.BoxSizer(wx.VERTICAL)
        self.sizer_vertical.Add(self.horizontal, proportion=1, flag=wx.EXPAND)
        self.SetSizerAndFit(self.sizer_vertical)

        # Set callback from windows hook procedure to our python code
        dll_name = "FSHooks.dll"
        dll_abspath = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'Win32',
                                                   'Debug', dll_name))
        dll_handle = cdll.LoadLibrary(dll_abspath)
        self.callback_type = CFUNCTYPE(None, c_int)
        self.callback = self.callback_type(self.callback_from_c)

        dll_handle.Init()
        dll_handle.SetCallback(self.callback)
        # TODO - Release the hooks when window closes

        # Register the hook
        # We want to pass dll_handle.HookProc
        # which looks like:
        # extern "C" FSHOOKS_API LRESULT HookProc(int code, WPARAM wParam, LPARAM lParam)
        dll_handle.HookProc.argtypes = (c_int, wintypes.WPARAM, wintypes.LPARAM)
        hook_proc_pointer = dll_handle.HookProc
        self.hook_id = windll.user32.SetWindowsHookExA(win32con.WH_MOUSE_LL, hook_proc_pointer,
                                                       win32api.GetModuleHandle(None), 0)

    def callback_from_c(self, number):
        self.textbox.AppendText('Called back from C: {}\n'.format(number))

    def print_to_text_box(self, event):
        self.called_back_count += 1
        print "Printing message {} on Thread with Id {}".format(self.called_back_count,
                                                                threading.current_thread().ident)
        self.textbox.AppendText('MessageName: {}\n'.format(event.MessageName))
        self.textbox.AppendText('Message: {}\n'.format(event.Message))
        self.textbox.AppendText('Time: {}\n'.format(event.Time))
        self.textbox.AppendText('Window: {}\n'.format(event.Window))
        self.textbox.AppendText('WindowName: {}\n'.format(event.WindowName))
        self.textbox.AppendText('Position: {}\n'.format(event.Position))
        self.textbox.AppendText('Wheel: {}\n'.format(event.Wheel))
        self.textbox.AppendText('Injected: {}\n'.format(event.Injected))
        self.textbox.AppendText('---\n')
