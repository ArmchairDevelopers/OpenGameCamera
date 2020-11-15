using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Metadata.W3cXsd2001;
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
    public static class DllInjector
    {
        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        [DllImport("user32.dll", SetLastError = true)]
        static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr GetModuleHandle(string lpModuleName);

        [DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
        static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress,
            uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out UIntPtr lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr CreateRemoteThread(
            IntPtr hProcess,
            IntPtr lpThreadAttributes, 
            uint dwStackSize, 
            IntPtr lpStartAddress, 
            IntPtr lpParameter, 
            uint dwCreationFlags, 
            out IntPtr lpThreadId
        );

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool CloseHandle(IntPtr hObject);

        const int PROCESS_CREATE_THREAD = 0x0002;
        const int PROCESS_QUERY_INFORMATION = 0x0400;
        const int PROCESS_VM_OPERATION = 0x0008;
        const int PROCESS_VM_WRITE = 0x0020;
        const int PROCESS_VM_READ = 0x0010;

        const uint MEM_COMMIT = 0x00001000;
        const uint MEM_RESERVE = 0x00002000;
        const uint PAGE_READWRITE = 4;

        public static bool Inject(ConsoleContent console, string dll_path)
        {
            IntPtr wnd = FindWindow(null, "STAR WARS Battlefront II");
            if (wnd == null)
            {
                console.Error("failed to find an active Star Wars Battlefront II game: code=" + Marshal.GetLastWin32Error());
                return false;
            }

            uint processId = 0;
            GetWindowThreadProcessId(wnd, out processId);

            if (processId == 0)
            {
                console.Error("failed to obtain process ID for Star Wars Battlefront II game: code=" + Marshal.GetLastWin32Error());
                return false;
            }

            console.Info("found SWBF2 at pid " + processId);

            IntPtr procHandle = OpenProcess(
                PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 
                false, 
                (int) processId
            );
            if (procHandle == null)
            {
                console.Error("failed to open pid=" + processId + ": code=" + Marshal.GetLastWin32Error());
                return false;
            }

            try
            {
                IntPtr loadLibraryPtr = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryW");

                console.Info("LoadLibraryW found at " + loadLibraryPtr.ToInt64().ToString("X16"));

                byte[] pathBuf = Encoding.Unicode.GetBytes(dll_path);
                
                IntPtr targetAddr = VirtualAllocEx(
                    procHandle, 
                    IntPtr.Zero, 
                    (uint)(pathBuf.Length + Marshal.SizeOf(typeof(ushort))), 
                    MEM_COMMIT | MEM_RESERVE, 
                    PAGE_READWRITE
                );
                if (targetAddr == null)
                {
                    console.Error("failed to allocate memory in target process: code=" + Marshal.GetLastWin32Error());
                    return false;
                }

                console.Info("memory allocated in SWBF2: 0x" + targetAddr.ToInt64().ToString("X16"));

                UIntPtr bytesWritten;

                if (!WriteProcessMemory(procHandle, targetAddr, pathBuf, (uint) pathBuf.Length, out bytesWritten))
                {
                    console.Error("failed to write memory in target process: code=" + Marshal.GetLastWin32Error());
                    return false;
                }

                console.Info("wrote " + bytesWritten.ToUInt64() + " bytes to allocated memory");

                IntPtr threadId;

                if (CreateRemoteThread(procHandle, IntPtr.Zero, 0, loadLibraryPtr, targetAddr, 0, out threadId) == null)
                {
                    console.Error("failed to launch remote thread: code=" + Marshal.GetLastWin32Error());
                    return false;
                }

                console.Info("OGC injected into SWBF2: threadId=" + threadId.ToInt64());
            } 
            finally
            {
                CloseHandle(procHandle);
            }

            return true;
        }
    }

    public static class ResourceExtractor
    {
        public static bool ExtractResourceToFile(ConsoleContent console, string resourceName, string filename)
        {
            using (Stream stream = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream(resourceName))
            {
                if (stream == null)
                {
                    console.Error("cannot find resource " + resourceName);
                    return false;
                }

                using (FileStream fs = new FileStream(filename, FileMode.Create))
                {
                    byte[] buf = new byte[stream.Length];
                    stream.Read(buf, 0, buf.Length);

                    try
                    {
                        fs.Write(buf, 0, buf.Length);
                    }
                    catch (IOException err)
                    {
                        console.Error("cannot write to " + filename + ": " + err);
                        return false;
                    }
                }
            }

            console.Info("OpenGameCamera extracted to " + filename);
            return true;
        }

        public static Nullable<DateTime> GetBuildUTCTimestamp(ConsoleContent console)
        {
            using (Stream stream = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream("LaunchOpenGameCamera.BuildTimestamp.txt"))
            {
                if (stream == null)
                {
                    console.Error("Cannot find UTC timestamp for current build");
                    return null;
                }

                byte[] buf = new byte[stream.Length];
                stream.Read(buf, 0, buf.Length);

                string rawData = Encoding.Unicode.GetString(buf);
                if (rawData.Length <= 50)
                {
                    console.Error("Invalid timestamp format (too short): " + rawData.Replace("\n", "\\n"));
                    return null;
                }

                int rawTimestampStart = rawData.LastIndexOf("\n", rawData.Length - 2);
                if (rawTimestampStart < 0)
                {
                    console.Error("Invalid timestamp format: " + rawData.Replace("\n", "\\n"));
                    return null;
                }
                
                string rawTimestamp = rawData.Substring(rawTimestampStart + 1);
                if (rawTimestamp.Length < 24)
                {
                    console.Error("Bad build timestamp format: " + rawTimestamp);
                    return null;
                }

                string rawTimezone = rawTimestamp.Substring(22);

                int minOffset;

                try
                {
                    minOffset = Int32.Parse(rawTimezone, NumberStyles.AllowTrailingWhite);
                } 
                catch (FormatException)
                {
                    console.Error("Timezone modifier not a number: " + rawTimezone);
                    return null;
                }
                catch (OverflowException)
                {
                    console.Error("Timezone modifier too big: " + rawTimezone);
                    return null;
                }

                int offsetHours = minOffset / 60;
                int offsetMins = minOffset % 60;

                string utcStamp = String.Format("{0}{1:00}:{2:00}", rawTimestamp.Substring(0, 22), offsetHours, offsetMins);
                return DateTime.ParseExact(utcStamp, "yyyyMMddHHmmss.ffffffzzz", CultureInfo.InvariantCulture, DateTimeStyles.AdjustToUniversal);
            }
        }
    }

    public partial class MainWindow : Window
    {
        ConsoleContent console = new ConsoleContent();
        DateTime buildTimestampUTC;
        string buildVersion = null;

        public MainWindow()
        {
            InitializeComponent();

            DataContext = console;
            console.SetScrollViewer(Scroller);

            DateTime? buildDate = ResourceExtractor.GetBuildUTCTimestamp(console);
            if (buildDate != null)
            {
                buildTimestampUTC = buildDate.Value;
                buildVersion = buildDate.Value.ToString("yyyy.MM.dd-HHmmss");

                CheckForLatestRelease();
            }

            console.Info("Welcome to OpenGameCamera. To begin, ensure your game is running before starting OGC.\n\n");
        }

        private void CheckForLatestRelease()
        {
            // TODO: For the future when I figure out how to merge references into one EXE correctly...
        }

        private void InjectButton_Click(object sender, RoutedEventArgs e)
        {
            string rootDir = System.IO.Path.Combine(System.IO.Path.GetTempPath(), "OpenGameCamera", buildVersion ?? "default");

            if (!Directory.Exists(rootDir)) {
                try
                {
                    Directory.CreateDirectory(rootDir);
                    console.Info("Creating directory for OGC: path=" + rootDir);
                }
                catch (IOException)
                {
                    console.Error("Path already exists as file: path=" + rootDir);
                    return;
                }
                catch (UnauthorizedAccessException)
                {
                    console.Error("Permission error trying to create: path=" + rootDir);
                    return;
                }
            } 
            else
            {
                console.Info("Directory already exists at " + rootDir);
            }

            string dllPath = System.IO.Path.Combine(rootDir, "OpenGameCamera.dll");
            if (!File.Exists(dllPath))
            {
                if (!ResourceExtractor.ExtractResourceToFile(console, "LaunchOpenGameCamera.OpenGameCamera.dll", dllPath))
                {
                    console.Error("Failed to extract DLL -- if this happens consistently, paste the Log Output in #log-outputs in the OpenGameCamera Discord (https://discord.gg/HZ676Ff)");
                    return;
                }

                console.Info("Extracted OGC to " + dllPath);
            }

            if (!DllInjector.Inject(console, dllPath))
            {
                console.Error("Failed to inject -- if this happens consistently, paste the Log Output in #log-outputs in the OpenGameCamera Discord (https://discord.gg/HZ676Ff)");
                return;
            }

            console.Info("");
        }

        private void CopyTextButton_Click(object sender, RoutedEventArgs e)
        {
            Clipboard.SetDataObject(console.GetText());
        }
    }

    public class ConsoleContent : INotifyPropertyChanged
    {
        ObservableCollection<string> consoleOutput = new ObservableCollection<string>();
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

        public string GetText()
        {
            return String.Join("\n", consoleOutput);
        }

        public event PropertyChangedEventHandler PropertyChanged;
        void OnPropertyChanged(string propertyName)
        {
            if (null != PropertyChanged)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
