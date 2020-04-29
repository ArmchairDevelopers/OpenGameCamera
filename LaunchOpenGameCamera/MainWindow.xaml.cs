using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace LaunchOpenGameCamera
{
    public static class ResourceExtractor
    {
        public static void ExtractResourceToFile(string resourceName, string filename)
        {
            // TODO: FIXME
            if (!System.IO.File.Exists(filename))
                using (System.IO.Stream s = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream(resourceName))
                using (System.IO.FileStream fs = new System.IO.FileStream(filename, System.IO.FileMode.Create))
                {
                    byte[] b = new byte[s.Length];
                    s.Read(b, 0, b.Length);
                    fs.Write(b, 0, b.Length);
                }
        }
    }

    public partial class MainWindow : Window
    {
        ObservableCollection<string> consoleOutput = new ObservableCollection<string>() { "Console Emulation Sample..." };
        ConsoleContent console = new ConsoleContent();

        public MainWindow()
        {
            InitializeComponent();
            DataContext = console;
            console.SetScrollViewer(Scroller);
        }

        private void InjectButton_Click(object sender, RoutedEventArgs e)
        {
            console.Info("Extracting");
            ResourceExtractor.ExtractResourceToFile("LaunchOpenGameCamera.OpenGameCamera.dll", "OpenGameCamera.dll");
            console.Info("Done");
        }

        private void CopyTextButton_Click(object sender, RoutedEventArgs e)
        {
            
        }
    }

    public class ConsoleContent : INotifyPropertyChanged
    {
        ObservableCollection<string> consoleOutput = new ObservableCollection<string>() { "Console Emulation Sample..." };
        ScrollViewer scroller = null;

        public ObservableCollection<string> ConsoleOutput
        {
            get
            {
                return consoleOutput;
            }
            set
            {
                consoleOutput = value;
                OnPropertyChanged("ConsoleOutput");
            }
        }

        public void SetScrollViewer(ScrollViewer view)
        {
            scroller = view;
        }

        public void Info(string msg)
        {
            Log("INFO", msg);
        }

        public void Warning(string msg)
        {
            Log("WARNING", msg);
        }

        public void Error(string msg)
        {
            Log("ERROR", msg);
        }

        private void Log(string prefix, string msg)
        {
            ConsoleOutput.Add("[" + prefix + "] " + msg);
            if (scroller != null)
            {
                scroller.ScrollToBottom();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        void OnPropertyChanged(string propertyName)
        {
            if (null != PropertyChanged)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
