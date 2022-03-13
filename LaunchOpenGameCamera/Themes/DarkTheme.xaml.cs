﻿using System.Diagnostics;
using System.Windows;

namespace LaunchOpenGameCamera.Themes
{
    public partial class DarkTheme
    {
        private void CloseWindow_Event(object sender, RoutedEventArgs e)
        {
            if (e.Source != null)
                try { CloseWind(Window.GetWindow((FrameworkElement)e.Source)); } catch { }
        }
        private void AutoMinimize_Event(object sender, RoutedEventArgs e)
        {
            if (e.Source != null)
                try { MaximizeRestore(Window.GetWindow((FrameworkElement)e.Source)); } catch { }
        }
        private void Minimize_Event(object sender, RoutedEventArgs e)
        {
            if (e.Source != null)
                try { MinimizeWind(Window.GetWindow((FrameworkElement)e.Source)); } catch { }
        }

        public void CloseWind(Window window) {
            if (window.Title == "OpenGameCamera") {
                //Environment.Exit(0);
                //Process.GetCurrentProcess().Kill();
                Application.Current.Shutdown();
            }
            else window.Hide();
        }

        public void MaximizeRestore(Window window)
        {
            if (window.WindowState == WindowState.Maximized) {
                window.WindowState = WindowState.Normal;
            }
                
            else if (window.WindowState == WindowState.Normal) {
                window.WindowState = WindowState.Maximized;
            }
                
        }

        public const string WrapperGridName = "WrapperGrid";

        public void MinimizeWind(Window window) => window.WindowState = WindowState.Minimized;
    }
}
