#define __AEGON_THREADED_DECOMPRESS
#define __AEGON_THREADED_DOWNLOAD
#define __AEGON_FAST_COMPARE


using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Net;
using System.IO;
using System.Globalization;
using System.IO.Compression;
using System.ComponentModel;
using System.Threading;
using System.Windows.Media.Effects;



namespace PatchClient
{
    /// <summary>
    /// Interaction logic for PictureBasedWindow.xaml
    /// </summary>
    /// 

   
    public partial class MainWnd : Window
    {
        string currentPath;

        public static string SERVER_ROOT = "C:/Users/Administrator/Desktop/Immortal flyff/Patch/";

        string homepageUrl = "https://titanium-mmo.com/";
        string facebookUrl = "https://www.facebook.com/Titanium-MMO-106162314653143";
        string discordUrl = "https://discord.com/invite/KZJAzQ9Xdw";

        public static string startUpSalt = "TitClient!123";
        public static string startUpSaltUpdater = ""; //Leave empty if your updater.exe has no start up salt.
        public static string configFileName = "neuz.ini";

        public static int currentVersion = 38;
        public int currentFile = 0;
        public int totalFiles = 0;
        private Int64 bytesReceived = 0;
        public bool completedWork = false;
        Stopwatch sw = new Stopwatch();
        public Mutex mainMutex;
        public static string patcherMutexName = "TitaniumPatcher";
        public static string gameMutexName = "neuz";
        public static string gameMutexName64 = "neuz-x64";

        public static string exeName = "Neuz.exe";
        public static string exeName64 = "Neuz-x64.exe";
        bool updateRestart = false;

        List<Thread> decompressThreadList = new List<Thread>();

#if __AEGON_THREADED_DOWNLOAD
        List<Thread> downloadThreadList = new List<Thread>();
        public bool boostDownload = true;
        public static int concurrentDownloads = 1;
        public int activeDownloadThreads = 0;
        bool downloadFailed = false;
        public int downloadedCount = 0;
        public int lastDownloadCount = 0;
#endif // __AEGON_THREADED_DOWNLOAD

        public UInt64 totalDownloadSize = 0;
        public UInt64 totalBytesDownloaded = 0;

        Options optionWindow;

        private AbortableBackgroundWorker worker;  //= new AbortableBackgroundWorker();
        private bool programIsExiting = false;

        [StructLayout(LayoutKind.Sequential)]
        public struct POINT
        {
            public int X;
            public int Y;

            public POINT(int x, int y)
            {
                this.X = x;
                this.Y = y;
            }
            public POINT(Point pt)
            {
                X = Convert.ToInt32(pt.X);
                Y = Convert.ToInt32(pt.Y);
            }
        };

        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "4.0.0.0")]
        public MainWnd()
        {

            System.Net.ServicePointManager.DefaultConnectionLimit = 6;
            System.Net.ServicePointManager.Expect100Continue = false;

            Process[] pname = Process.GetProcessesByName(gameMutexName);
            Process[] pname64 = Process.GetProcessesByName(gameMutexName64);
            if (pname64.Length != 0) 
            {
                Start_Neuz64();
            }
            else if (pname.Length != 0)
            {
                Start_Neuz32();
            }
            System.Uri resourceLocater = new System.Uri("/Titanium-MMO;component/mainwnd.xaml", System.UriKind.Relative);

            System.Windows.Application.LoadComponent(this, resourceLocater);

            mainMutex = new Mutex(true, patcherMutexName);
            currentPath = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath) + "\\";

            optionWindow = new Options(currentPath);
            optionWindow.WindowStartupLocation = System.Windows.WindowStartupLocation.Manual;


            this.Hide();

            ButtonPlay.Opacity = 0.8;

            ButtonPlay.IsEnabled = false;

            ButtonPlay.Content = FindResource("playNormal");
            ButtonSettings.Content = FindResource("settNormal");
            ButtonClose.Content = FindResource("closeNormal");
            ButtonMinimize.Content = FindResource("minimizeNormal");
            ButtonHomepage.Content = FindResource("homepageNormal");
            ButtonDiscord.Content = FindResource("discordNormal");
            ButtonFacebook.Content = FindResource("facebookNormal");

            ButtonNews0.Content = FindResource("ButtonNewsClicked");
            ButtonNews1.Content = FindResource("ButtonNewsNormal1");
            ButtonNews2.Content = FindResource("ButtonNewsNormal2");
            ButtonNews3.Content = FindResource("ButtonNewsNormal3");
            ButtonNews4.Content = FindResource("ButtonNewsNormal4");

            NewsImage.Source = ((Image)FindResource("news0")).Source;


            concurrentDownloads = (int)(Environment.ProcessorCount / 2);
            if (concurrentDownloads < 1)
                concurrentDownloads = 1;

            //SetSelected();

            ImageDark.Visibility = Visibility.Collapsed;

            this.Activated += MainWnd_Activated;
            this.Show();
            worker = new AbortableBackgroundWorker();

            worker.DoWork += worker_DoWork;
            worker.RunWorkerCompleted += worker_RunWorkerCompleted;
            worker.WorkerReportsProgress = true;
            worker.ProgressChanged += worker_ProgressChanged;
            

            worker.RunWorkerAsync();
        }

        private void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            // run all background tasks here
            try
            {
                int processReturn = 0;
                while (processReturn == 0)
                {
#if __AEGON_THREADED_DOWNLOAD
                    processReturn = ProcessFiles(true);
#else // __AEGON_THREADED_DOWNLOAD
                    processReturn = ProcessFiles();
#endif // __AEGON_THREADED_DOWNLOAD
                }
                if (processReturn == 2)
                {
                    updateRestart = true;
                    throw new Exception("Program Close");
                }
#if __AEGON_THREADED_DOWNLOAD
                if (downloadFailed == true)
                {
                    while (processReturn == 0)
                    {
                        processReturn = ProcessFiles(false);
                    }

                }
#endif // __AEGON_THREADED_DOWNLOAD
            }
            catch (Exception ex)
            {
                if(ex.Message.Contains("Program Close"))
                    ProgramClose(true);
                if (ex.Message.Contains("being used"))
                {
                    MessageBox.Show("Client already open, you can't overwrite this file whilst it's open.\nProgram now closing!");
                    ProgramClose(true);
                }
            }
        }

        private void worker_RunWorkerCompleted(object sender,
                                               RunWorkerCompletedEventArgs e)
        {
#if __AEGON_THREADED_DECOMPRESS
            if (e.Error != null)
            {
                if(!updateRestart)
                    MessageBox.Show(e.Error.Message);
                ProgramClose(true);
            }
#endif // __AEGON_THREADED_DECOMPRESS
            //update ui once worker complete his work
            if (completedWork == true)
            {
                ButtonPlay.IsEnabled = true;
                ButtonPlay.Opacity = 1;
            }
            else
                ProgramClose(true);
        }

        private void worker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            // Change the value of the ProgressBar to the BackgroundWorker progress.
            Total.Value = e.ProgressPercentage;
            CurrentFileText.Text = string.Format("{0}%", e.ProgressPercentage);

            if (bytesReceived > 0 && sw.Elapsed.TotalMilliseconds > 200)
                downloadSpeed.Text = string.Format("{0} MB/s", (Math.Abs(((bytesReceived / 1024d)) / (sw.Elapsed.TotalMilliseconds))).ToString("0.00"));


            if (e.ProgressPercentage == 100)
                totalBytesDownloaded = totalDownloadSize;

            if (totalBytesDownloaded == 0 || totalDownloadSize == 0)
            {
                downloadProgress.Text = string.Format("0,00/0,00MB");
            }
            else if (totalDownloadSize > 1 * 1024 * 1024 * 1024)
            {
                downloadProgress.Text = string.Format("{0}/{1}GB", (totalBytesDownloaded / 1024d / 1024d / 1024d).ToString("0.00"), (totalDownloadSize / 1024d / 1024d / 1024d).ToString("0.00"));
            }
            else if (totalDownloadSize > 1 * 1024 * 1024)
            {
                downloadProgress.Text = string.Format("{0}/{1}MB", (totalBytesDownloaded / 1024d / 1024d).ToString("0.00"), (totalDownloadSize / 1024d / 1024d).ToString("0.00"));
            }
            else if (totalDownloadSize > 1 * 1024)
            {
                downloadProgress.Text = string.Format("{0}/{1}kB", (totalBytesDownloaded / 1024d).ToString("0.00"), (totalDownloadSize / 1024d).ToString("0.00"));
            }
            else
            {
                downloadProgress.Text = string.Format("{0}/{1}byte", totalBytesDownloaded.ToString("0.00"), totalDownloadSize.ToString("0.00"));
            }

           
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (programIsExiting == false) 
            {

                this.DragMove();
                MoveOptionWindow();
              
            }
                
            programIsExiting = false;
        }

        private void Start_Neuz32()
        {
            try
            {
                ProcessStartInfo startInfo = new ProcessStartInfo();
                string path = System.IO.Path.GetDirectoryName(
                      System.Windows.Forms.Application.ExecutablePath);

                startInfo.FileName = path + "\\" + exeName;

                startInfo.Arguments = startUpSalt;
                Process.Start(startInfo);
                ProgramClose(false);
            }
            catch (Exception)
            {
                MessageBox.Show(exeName + " file not found\n");
            }
        }
        private void Start_Neuz64()
        {
            try
            {
                ProcessStartInfo startInfo = new ProcessStartInfo();
                string path = System.IO.Path.GetDirectoryName(
                      System.Windows.Forms.Application.ExecutablePath);

                startInfo.FileName = path + "\\" + exeName64;

                startInfo.Arguments = startUpSalt;
                Process.Start(startInfo);
                ProgramClose(false);
            }
            catch (Exception)
            {
                MessageBox.Show(exeName64 + " file not found\n");
            }
        }
        private void Start_Neuz() 
        {
            bool bStart32 = false;
            if (optionWindow != null) 
            {
                if (optionWindow.Start32BitNeuz())
                    bStart32 = true;
            }

            if (Environment.Is64BitOperatingSystem && !bStart32)
            {
                Start_Neuz64();
            }
            else
            {
                Start_Neuz32();
            }

        }

        public bool SkipPatch(string mutexId)
        {
            bool created = false;

            var mutex = new Mutex(false, mutexId, out created);

            if (created == false)
            {
                return true;
            }

            return false;
        }

        private void ButtonPlay_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            if (!completedWork) 
            {
                return;
            }

            Start_Neuz();
        }

        private void ButtonClose_Click(object sender, RoutedEventArgs e)
        {
            ProgramClose(false);
        }
        private void ButtonClose_MouseEnter(object sender, MouseEventArgs e)
        {
            ButtonClose.Content = FindResource("closeHover");
        }

        private void ButtonClose_MouseLeave(object sender, MouseEventArgs e)
        {
            ButtonClose.Content = FindResource("closeNormal");
        }

        private void ButtonMinimize_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Minimized;
        }
        private void ButtonMinimize_MouseEnter(object sender, MouseEventArgs e)
        {
            ButtonMinimize.Content = FindResource("minimizeHover");
        }

        private void ButtonMinimize_MouseLeave(object sender, MouseEventArgs e)
        {
            ButtonMinimize.Content = FindResource("minimizeNormal");
        }

        private void ButtonPlay_MouseEnter(object sender, MouseEventArgs e)
        {
            ButtonPlay.Content = FindResource("playHover");
        }

        private void ButtonPlay_MouseLeave(object sender, MouseEventArgs e)
        {
            ButtonPlay.Content = FindResource("playNormal");
        }

        private void ButtonSettings_MouseEnter(object sender, MouseEventArgs e)
        {
            ButtonSettings.Content = FindResource("settHover");
        }

        private void ButtonSettings_MouseLeave(object sender, MouseEventArgs e)
        {
            ButtonSettings.Content = FindResource("settNormal");
        }

        private void ButtonHomepage_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start(homepageUrl);
        }
        private void ButtonHomepage_MouseEnter(object sender, MouseEventArgs e)
        {
            ButtonHomepage.Content = FindResource("homepageHover");
        }

        private void ButtonHomepage_MouseLeave(object sender, MouseEventArgs e)
        {
            ButtonHomepage.Content = FindResource("homepageNormal");
        }
        private void ButtonFacebook_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start(facebookUrl);
        }
        private void ButtonFacebook_MouseEnter(object sender, MouseEventArgs e)
        {
            ButtonFacebook.Content = FindResource("facebookHover");
        }

        private void ButtonFacebook_MouseLeave(object sender, MouseEventArgs e)
        {
            ButtonFacebook.Content = FindResource("facebookNormal");
        }
        private void ButtonDiscord_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start(discordUrl);
        }
        private void ButtonDiscord_MouseEnter(object sender, MouseEventArgs e)
        {
            ButtonDiscord.Content = FindResource("discordHover");
        }

        private void ButtonDiscord_MouseLeave(object sender, MouseEventArgs e)
        {
            ButtonDiscord.Content = FindResource("discordNormal");
        }

        private void ButtonNews_Click(object sender, RoutedEventArgs e)
        {
            ButtonNews0.Content = FindResource("ButtonNewsNormal0");
            ButtonNews1.Content = FindResource("ButtonNewsNormal1");
            ButtonNews2.Content = FindResource("ButtonNewsNormal2");
            ButtonNews3.Content = FindResource("ButtonNewsNormal3");
            ButtonNews4.Content = FindResource("ButtonNewsNormal4");

            ((Button)sender).Content = FindResource("ButtonNewsClicked");

            string s = (sender as Button).Name;
            if (s.CompareTo("ButtonNews0") == 0) 
            {
                NewsImage.Source = ((Image)FindResource("news0")).Source;
            }
            else if (s.CompareTo("ButtonNews1") == 0)
            {
                NewsImage.Source = ((Image)FindResource("news1")).Source;
            }
            else if (s.CompareTo("ButtonNews2") == 0)
            {
                NewsImage.Source = ((Image)FindResource("news2")).Source;
            }
            else if (s.CompareTo("ButtonNews3") == 0)
            {
                NewsImage.Source = ((Image)FindResource("news3")).Source;
            }
            else if (s.CompareTo("ButtonNews4") == 0)
            {
                NewsImage.Source = ((Image)FindResource("news4")).Source;
            }


        }
        private void ButtonNews_MouseEnter(object sender, MouseEventArgs e)
        {
        }

        private void ButtonNews_MouseLeave(object sender, MouseEventArgs e)
        {

        }

        private bool Download(string inputFileName, string outputFileName, DateTime dateTime, int current, int max, long fileOriginalSize, bool multithreaded)
        {
            currentFile++;

            HttpWebRequest httpRequest = (HttpWebRequest)WebRequest.Create(inputFileName);
            httpRequest.Method = WebRequestMethods.Http.Get;
            httpRequest.Proxy = null;
            httpRequest.AllowAutoRedirect = true;

            HttpWebResponse httpResponse;

            try
            {
                httpResponse = (HttpWebResponse)httpRequest.GetResponse();
            }
            catch(Exception exp)
            {
                if (exp.ToString().Contains("(404)"))
                {
                    string[] split = inputFileName.Split("/".ToCharArray());
                    MessageBox.Show("Failed to find file " + split[split.Length - 1] + " !\nPlease try again soon.\nAlternately post on the forums with the file name.");
                    MessageBox.Show("Closing Program!");
                }
                else
                {
                    MessageBox.Show("Failed to connect to server!\nPlease try again soon.\nAlternatevly check your network connection.");
                    MessageBox.Show("Closing Program!");
                }

                throw exp;
            }
            using (Stream httpResponseStream = httpResponse.GetResponseStream())
            {

                int bufferSize = 1024;
                byte[] buffer = new byte[bufferSize];
                int bytesRead = 0;
                long length = httpResponse.ContentLength;
                long totalbytesread = 0;

                string[] outputSplit = outputFileName.Split("\\".ToCharArray());
                DriveInfo driveInfo = new DriveInfo(@outputSplit[0]);
                long FreeSpace = driveInfo.AvailableFreeSpace;
                if (FreeSpace < length)
                {
                    MessageBox.Show("Not enough space on Disk " + outputSplit[0] + "\nPlease consider removing some things.\nThen simply re-open this program");
                    MessageBox.Show("Closing Program!");
                    throw new Exception();
                }

                using (FileStream fileStreamCompressed = File.Create(outputFileName + ".gz"))
                {
                    
                    this.Dispatcher.Invoke((Action)(() =>
                    {
                        downloadType.Text = "Downloading";
                    }));
                    
                    sw.Start();
                    while ((bytesRead = httpResponseStream.Read(buffer, 0, bufferSize)) != 0)
                    {
                        if (worker.restartDownload == true)
                        {
                            sw.Stop();
                            return false;
                        }

                        bytesReceived += bytesRead;
                        while (worker.sleeping == true)
                        {
                            sw.Stop();
                            Thread.Sleep(100);
                            sw.Start();
                        }
                        totalbytesread += bytesRead;
                        
                        fileStreamCompressed.Write(buffer, 0, bytesRead);

                        Debug.Assert(max != 0);

                        decimal step = (100m / (decimal)max);

                        Debug.Assert(step != 0);
#if __AEGON_THREADED_DECOMPRESS
                        decimal currentPercent = (totalbytesread * 100m) / length;
#else // __AEGON_THREADED_DECOMPRESS
                        decimal currentPercent = (totalbytesread * 50m) / length;
#endif // __AEGON_THREADED_DECOMPRESS

                        decimal valuePercent = (current * step) + ((currentPercent / 100) * step);

                        worker.ReportProgress((int)valuePercent);

                    } // end while
                    sw.Stop();
                }


#if __AEGON_THREADED_DECOMPRESS
                if (multithreaded)
                {
                    Thread t = new Thread(
                       () => AegonThreadedDecompress(outputFileName, current, max, fileOriginalSize, outputSplit, dateTime)
                       );
                    decompressThreadList.Add(t);
                    t.Start();
                }
                else 
                {
                    AegonThreadedDecompress(outputFileName, current, max, fileOriginalSize, outputSplit, dateTime);
                }

#else // __AEGON_THREADED_DECOMPRESS
                byte[] file = File.ReadAllBytes(outputFileName + ".gz");
                byte[] decompressed = Decompress(file, current, max, fileOriginalSize);
                driveInfo = new DriveInfo(@outputSplit[0]);
                FreeSpace = driveInfo.AvailableFreeSpace;
                if (FreeSpace < decompressed.Length)
                {
                    MessageBox.Show("Not enough space on Disk " + outputSplit[0] + "\nPlease consider removing some things.\nThen simply re-open this program");
                    MessageBox.Show("Closing Program!");

                    throw new Exception();
                }

                using (FileStream fileStream = File.Create(outputFileName))
                {
                    fileStream.Write(decompressed, 0, decompressed.Length);
                }
                FileInfo fi1 = new FileInfo(outputFileName);
                fi1.CreationTime = dateTime;
                fi1.CreationTimeUtc = dateTime;

                File.Delete(outputFileName + ".gz");
#endif // __AEGON_THREADED_DECOMPRESS
            }
            if (worker.restartDownload)
            {
                return false;
            }
            return true;
        }

#if __AEGON_THREADED_DECOMPRESS
        int AegonThreadedDecompress(string outputFileName, int current, int max, long fileOriginalSize, string[] outputSplit, DateTime dateTime) 
        {
            byte[] file = File.ReadAllBytes(outputFileName + ".gz");
            byte[] decompressed = Decompress(file, current, max, fileOriginalSize);
            DriveInfo driveInfo = new DriveInfo(@outputSplit[0]);
            long FreeSpace = driveInfo.AvailableFreeSpace;
            if (FreeSpace < decompressed.Length)
            {
                MessageBox.Show("Not enough space on Disk " + outputSplit[0] + "\nPlease consider removing some things.\nThen simply re-open this program");
                MessageBox.Show("Closing Program!");
                throw new Exception();
            }

            using (FileStream fileStream = File.Create(outputFileName))
            {
                fileStream.Write(decompressed, 0, decompressed.Length);
                
            }

            FileInfo fi1 = new FileInfo(outputFileName);
            fi1.CreationTime = dateTime;
            fi1.CreationTimeUtc = dateTime;

            File.Delete(outputFileName + ".gz");

            return 0;
        }
#endif // __AEGON_THREADED_DECOMPRESS

#if __AEGON_THREADED_DOWNLOAD
        bool AegonThreadedDownload(string inputFileName, string outputFileName, DateTime dateTime, int current, int max, long fileOriginalSize, bool multithreaded, long fileCompressedSize) 
        {
            currentFile++;

            HttpWebRequest httpRequest = (HttpWebRequest)WebRequest.Create(inputFileName);
            httpRequest.Method = WebRequestMethods.Http.Get;
            httpRequest.Proxy = null;
            httpRequest.AllowAutoRedirect = true;
            
            HttpWebResponse httpResponse;
            try
            {
                httpResponse = (HttpWebResponse)httpRequest.GetResponse();
            }
            catch (Exception)
            {
                downloadFailed = true;
                activeDownloadThreads--;
                return false;
            }
            using (Stream httpResponseStream = httpResponse.GetResponseStream())
            {

                int bufferSize = 1024;
                byte[] buffer = new byte[bufferSize];
                int bytesRead = 0;
                long length = httpResponse.ContentLength;
                long totalbytesread = 0;

                string[] outputSplit = outputFileName.Split("\\".ToCharArray());
                DriveInfo driveInfo = new DriveInfo(@outputSplit[0]);
                long FreeSpace = driveInfo.AvailableFreeSpace;
                if (FreeSpace < length)
                {
                    MessageBox.Show("Not enough space on Disk " + outputSplit[0] + "\nPlease consider removing some things.\nThen simply re-open this program");
                    MessageBox.Show("Closing Program!");
#if __AEGON_THREADED_DOWNLOAD
                    downloadFailed = true;
                    activeDownloadThreads--;
#endif // __AEGON_THREADED_DOWNLOAD
                    throw new Exception();
                }

                using (FileStream fileStreamCompressed = File.Create(outputFileName + ".gz"))
                {


                    try
                    {
                        while ((bytesRead = httpResponseStream.Read(buffer, 0, bufferSize)) != 0)
                        {
                            if (programIsExiting == true) 
                            {
                                downloadFailed = true;
                                activeDownloadThreads--;
                                return false;
                            }

                            if (worker.restartDownload == true)
                            {

                                downloadFailed = true;
                                activeDownloadThreads--;
                                return false;
                            }

                            bytesReceived += bytesRead;
                            while (worker.sleeping == true)
                            {
                                Thread.Sleep(100);
                            }
                            totalbytesread += bytesRead;
                            totalBytesDownloaded += (ulong)bytesRead;
                            fileStreamCompressed.Write(buffer, 0, bytesRead);

                            worker.ReportProgress((int)(totalBytesDownloaded * 100 / totalDownloadSize));

                        } // end while
                        if (totalBytesDownloaded > totalDownloadSize)
                            totalBytesDownloaded = totalDownloadSize;
                    }
                    catch (Exception) 
                    {
                        downloadFailed = true;
                        activeDownloadThreads--;
                        return false;
                    }        
                }
               

#if __AEGON_THREADED_DECOMPRESS
                if (multithreaded)
                {
                    Thread t = new Thread(
                       () => AegonThreadedDecompress(outputFileName, current, max, fileOriginalSize, outputSplit, dateTime)
                       );
                    decompressThreadList.Add(t);
                    t.Start();
                }
                else
                {
                    AegonThreadedDecompress(outputFileName, current, max, fileOriginalSize, outputSplit, dateTime);
                }

#else // __AEGON_THREADED_DECOMPRESS
                byte[] file = File.ReadAllBytes(outputFileName + ".gz");
                byte[] decompressed = Decompress(file, current, max, fileOriginalSize);
                driveInfo = new DriveInfo(@outputSplit[0]);
                FreeSpace = driveInfo.AvailableFreeSpace;
                if (FreeSpace < decompressed.Length)
                {
                    MessageBox.Show("Not enough space on Disk " + outputSplit[0] + "\nPlease consider removing some things.\nThen simply re-open this program");
                    MessageBox.Show("Closing Program!");

                    throw new Exception();
                }

                using (FileStream fileStream = File.Create(outputFileName))
                {
                    fileStream.Write(decompressed, 0, decompressed.Length);
                }
                FileInfo fi1 = new FileInfo(outputFileName);
                fi1.CreationTime = dateTime;
                fi1.CreationTimeUtc = dateTime;

                File.Delete(outputFileName + ".gz");
#endif // __AEGON_THREADED_DECOMPRESS
            }
            if (worker.restartDownload)
            {
                downloadFailed = true;
                activeDownloadThreads--;
                return false;
            }
            downloadedCount++;
            activeDownloadThreads--;
            return true;
        }
#endif // __AEGON_THREADED_DOWNLOAD

        byte[] Decompress(byte[] gzip, int current, int max, long fileOriginalSize)
        {
            // Create a GZIP stream with decompression mode.
            // ... Then create a buffer and write into while reading from the GZIP stream.
            long total = fileOriginalSize == 0 ? gzip.Length : fileOriginalSize;
            long totalcount = 0;

            using (GZipStream stream = new GZipStream(new MemoryStream(gzip), CompressionMode.Decompress))
            {
                const int size = 4096;
                byte[] buffer = new byte[size];
                using (MemoryStream memory = new MemoryStream())
                {
#if !__AEGON_THREADED_DECOMPRESS
                    this.Dispatcher.Invoke((Action)(() =>
                    {
                        downloadType.Text = "Decompressing";
                    }));
#endif // __AEGON_THREADED_DECOMPRESS
                    int count = 0;
                    do
                    {
                        while (worker.sleeping == true)
                        {
                            Thread.Sleep(100);
                        }

                        count = stream.Read(buffer, 0, size);

                        if (count > 0)
                        {

                            totalcount += count;
                            memory.Write(buffer, 0, count);
#if !__AEGON_THREADED_DECOMPRESS
                            decimal step = (100m / (decimal) max);

                            Debug.Assert(step != 0);

                            decimal currentPercent = 50m + ((totalcount * 50.0m) / total);

                            decimal valuePercent = (current * step) + ((currentPercent/100) * step);

                            worker.ReportProgress((int)valuePercent);
#endif // __AEGON_THREADED_DECOMPRESS

                        }

                    }
                    while (count > 0);
                    return memory.ToArray();
                }
            }
        }

        void AegonTerminateThreads() 
        {
            worker.Pause();
#if __AEGON_THREADED_DECOMPRESS
            foreach (Thread t in decompressThreadList)
            {
                if (t.IsAlive)
                    t.Abort();
            }
            


#endif // __AEGON_THREADED_DECOMPRESS
#if __AEGON_THREADED_DOWNLOAD
            foreach (Thread t in downloadThreadList)
            {
                if (t.IsAlive) 
                    t.Abort();
            }
#endif // __AEGON_THREADED_DOWNLOAD
            worker.Resume();

        }

        private int ProcessFiles(bool boostable)
        {
            //try
            {
                worker.restartDownload = false;

                currentFile = 0;
                totalFiles = 1;
                this.Dispatcher.Invoke((Action)(() =>
                {
                   downloadType.Text = "Grabbing Patch List";
                }));
                if (!Download(SERVER_ROOT + "list.txt.gz", currentPath + "list.txt", DateTime.Now,0,1,0, false))
                {
                    return 0;
                }

                List<FILE_INFO> files = new List<FILE_INFO>();
                using (StreamReader fp = File.OpenText(currentPath + "list.txt"))
                {
                        /*	            SYSTEMTIME	sysTime;
                                        char	szTokens[4][256];
                                        char	szBuffer[256], szDir[256], szFile[256];
                                        char	szRoot[256] = {0, };
                                        char	cbFlag;
                                        int		nYY, nMM, nDD;
                                        int		nHour, nMin;
                         */
                    int line = 0;
                    try
                    {
                        string szRoot = "";
                        string szDir = "";
                        while (fp.EndOfStream == false)
                        {
                            line++;
                            string szBuffer = fp.ReadLine();
                            /*                        if (szBuffer == null || szBuffer.Length == 0)
                                                    {
                                                        break;
                                                    }
                              */

                            if (szBuffer.Length < 6)
                                continue;

                            if (szBuffer[0] == 'v')
                            {
                                if (szBuffer.Substring(0, 4) == "ver:")
                                {
                                    int nVersion = int.Parse(szBuffer.Substring(4));
                                    if (nVersion > currentVersion)
                                    {
                                        while (true)
                                        {
                                            totalFiles = 2;
                                            currentFile = 0;
                                            this.Dispatcher.Invoke((Action)(() =>
                                            {
                                                downloadType.Text = "Updating Patcher";
                                            }));
                                            if (File.Exists(currentPath + "NewTitanium-MMO.exe"))
                                            {
                                                try
                                                {
                                                    File.Delete(currentPath + "NewTitanium-MMO.exe");
                                                }
                                                catch (Exception e)
                                                {
                                                    MessageBox.Show(e.Message);
                                                }
                                            }
                                            if (Download(SERVER_ROOT + "NewTitanium-MMO.exe.gz", currentPath + "NewTitanium-MMO.exe", DateTime.Now, 0, 2, 0, false))
                                            {
                                                if (File.Exists(currentPath + "Updater.exe")) 
                                                {
                                                    try
                                                    {
                                                        File.Delete(currentPath + "Updater.exe");
                                                    }
                                                    catch (Exception e) 
                                                    {
                                                        MessageBox.Show(e.Message);
                                                    }
                                                   
                                                }
                                               
                                                worker.ReportProgress(50);
                                                if (Download(SERVER_ROOT + "Updater.exe.gz", currentPath + "Updater.exe", DateTime.Now, 1, 2, 0, false))
                                                {
                                                    worker.ReportProgress(100);
                                                    worker.Dispose();
                                                    try
                                                    {
                                                        ProcessStartInfo startInfo = new ProcessStartInfo();
                                                        string path = System.IO.Path.GetDirectoryName(
                                                              System.Windows.Forms.Application.ExecutablePath);
                                                        startInfo.FileName = path + "\\Updater.exe";
                                                        startInfo.Arguments = startUpSaltUpdater;
                                                        //startInfo.Verb = "runas";
                                                        MessageBox.Show("Patcher Updated available, Click to Restart!");
                                                        Process.Start(startInfo);
                                                        Thread.Sleep(1000);
                                                        return 2;
                                                    }
                                                    catch (Exception Ex)
                                                    {
                                                        string tmp = Ex.Message;
                                                        MessageBox.Show("Updater.exe file not found, Trying again!");
                                                        return 0;

                                                    }
                                                }
                                                else
                                                {
                                                    return 0;
                                                }
                                            }
                                            else
                                            {
                                                return 0;
                                            }
                                        }

                                    }
                                    continue;
                                }
                                else
                                {
                                    throw new Exception();
                                }
                            }
                            else if (szBuffer[2] == ':')
                            {
                                string[] szTokens = szBuffer.Split(null);

                                if (szRoot.Length == 0)
                                {
                                    szRoot = szTokens[1];
                                    szDir = "";
                                }
                                else
                                {
                                    szDir = "";
                                    szDir = szTokens[1].Substring(szRoot.Length);
                                }
                            }
                            else if (szBuffer[4] == '-')
                            {
                                string[] szTokens = szBuffer.Split(new char[0], StringSplitOptions.RemoveEmptyEntries);

                                if (szTokens[2] != "<DIR>")
                                {

                                    string format = "yyyy-MM-dd hh:mm";

                                    string tmp = szTokens[0] + " " + szTokens[1];

                                    string cbFlag = tmp.Substring(tmp.Length - 1);
                                    tmp = tmp.Substring(0, tmp.Length - 1);

                                    DateTime date = DateTime.ParseExact(tmp, format,
                                        CultureInfo.InvariantCulture);


                                    int nYY = date.Year;
                                    int nMM = date.Month;
                                    int nDD = date.Day;

                                    int nHour = date.Hour;
                                    int nMin = date.Minute;

                                    if (date.Hour == 12)
                                        date = date.AddHours(-12.0d);
                                    if (cbFlag == "p")
                                        date = date.AddHours(12.0d);

                                    long nCompressedSize = 0;
                                    try
                                    {
                                        nCompressedSize = long.Parse(szTokens[3]);
                                    }
                                    catch (Exception) 
                                    {
                                        nCompressedSize = long.Parse(szTokens[2]);
                                    }
                                    


                                    string szFile = szDir + "\\" + szBuffer.Substring(39);                                    


                                    //Add me to get working for titanium users.
                                    if (szFile.Contains("\\"))
                                    {
                                        string[] newFile = szFile.Split("\\".ToCharArray());
                                        szFile = "";
                                        for (int i = 0; i < newFile.Length - 1; i++)
                                        {
                                            if (newFile[0] == "")
                                                szFile += "\\" + newFile[i + 1];
                                            else
                                                szFile += newFile[i] + "\\" + newFile[i + 1];
                                        }

                                    }

                                  
                                    EnqueueFILE(ref files, szFile, false, long.Parse(szTokens[2]), date, nCompressedSize); //, &sysTime, cB);
                                }
                            }
                        }
                    }
                    catch (Exception exc)
                    {
                        if (exc.ToString().Contains("DateTime"))
                            MessageBox.Show("Invalid date time in list.txt. Please notify an admin about the issue.\nLine number: " + line);
                        throw exc;
                    }
                }
                
                this.Dispatcher.Invoke((Action)(() =>
                {
                    downloadType.Text = "Comparing Files";                    
                    worker.ReportProgress(0);
                }));
                for (int i = files.Count - 1; i >= 0; i--)
                {
                    FileInfo fi1 = new FileInfo(files[i].szPath);
                    if (fi1.Exists)
                    {
                        if (fi1.Length == files[i].nFileSize
                             && (fi1.CreationTime.Equals(files[i].dateTime) || fi1.CreationTimeUtc.Equals(files[i].dateTime)))
                        {
#if __AEGON_FAST_COMPARE
                            FILE_INFO tmpFile = files[i];
                            tmpFile.bUpToDate = true;
                            files[i] = tmpFile;
#else // __AEGON_FAST_COMPARE
                            files.Remove(files[i]);
#endif // __AEGON_FAST_COMPARE
                        }
                    }
                }
                foreach (FILE_INFO file in files)
                {
#if __AEGON_FAST_COMPARE
                    if (file.bUpToDate)
                        continue;
#endif // __AEGON_FAST_COMPARE
                    //get total download size
                    totalDownloadSize += (ulong)file.nCompressedFileSize;
                }

                int max = files.Count;
                totalFiles = max == 0 ? 1 : max;
                currentFile = max == 0 ? 1 : 0;
                worker.ReportProgress(0);
                int count = 0;
                sw.Restart();

                foreach (FILE_INFO file in files)
                {
                    if (file.bUpToDate)
                        continue;
                   
                    if (!Directory.Exists(Path.GetDirectoryName(file.szPath)))
                    {
                        // Try to create the directory.
                        try
                        {
                            DirectoryInfo di = Directory.CreateDirectory(Path.GetDirectoryName(file.szPath));

                        }
                        catch (UnauthorizedAccessException e) 
                        {
                            MessageBox.Show(e.Message + "\nFailed to create Folder: " + Path.GetDirectoryName(file.szPath) + ". Create the folder manually or start the patcher with Administrator privileges.", "Error");
                        }
                        catch (DirectoryNotFoundException e)
                        {
                            MessageBox.Show(e.Message + "\nInvalid Directory: " + Path.GetDirectoryName(file.szPath), "Error");
                        }

                    }

#if __AEGON_THREADED_DECOMPRESS
                    decompressThreadList.Clear();
#endif // __AEGON_THREADED_DECOMPRESS

#if __AEGON_THREADED_DOWNLOAD
                    if (boostable && boostDownload && concurrentDownloads > 1) //multithreaded download
                    {
                        this.Dispatcher.Invoke((Action)(() =>
                        {
                            downloadType.Text = "Downloading";
                        }));

                        if (lastDownloadCount != downloadedCount)
                        {
                            int fileProgress = downloadedCount * 100 / max;

                            if (sw.ElapsedMilliseconds > 5000) 
                            {
                                sw.Restart();
                                bytesReceived = 0;
                            }
                        }

                        lastDownloadCount = downloadedCount;


                        while (activeDownloadThreads >= concurrentDownloads)
                        {
                            Thread.Sleep(100);
                        }

                        if (programIsExiting == true)
                            return 0;

                        Thread t = new Thread(
                            () => AegonThreadedDownload(file.szServerPath, file.szPath, file.dateTime, count, max, file.nFileSize, true, file.nCompressedFileSize)
                            );
                        downloadThreadList.Add(t);
                        activeDownloadThreads++;
                        
                        t.Start();
                    }
                    else
                    {
                        if (Download(file.szServerPath, file.szPath, file.dateTime, count, max, file.nFileSize, true))
                        {
                            ++count;
                            if (sw.ElapsedMilliseconds > 5000)
                            {
                                sw.Restart();
                                bytesReceived = 0;
                            }

                        }
                        else
                        {
                            return 0;
                        }
                    }
#else // __AEGON_THREADED_DOWNLOAD
                        if (Download(file.szServerPath, file.szPath, file.dateTime, count, max, file.nFileSize, true))
                        {
                            ++count;
                            if (count % 3 == 0)
                            {
                                bytesReceived = 0;
                                sw.Reset();
                            }

                        }
                        else
                        {
                            return 0;
                        }
#endif // __AEGON_THREADED_DOWNLOAD

                }

#if __AEGON_THREADED_DECOMPRESS
                //wait for download threads to finish
                foreach (Thread t in downloadThreadList)
                {
                    t.Join();                    
                }
                this.Dispatcher.Invoke((Action)(() =>
                {
                    downloadType.Text = "Decompressing";
                }));
               
                //wait for decompress threads to finish
                foreach (Thread t in decompressThreadList)
                {
                    t.Join();
                }
#endif // __AEGON_THREADED_DECOMPRESS

                worker.ReportProgress(100);
                this.Dispatcher.Invoke((Action)(() =>
                {
                    downloadType.Text = "Update Completed";
                }));
                completedWork = true;

                return 1;
            }
        }

        void EnqueueFILE(ref List<FILE_INFO> files, string szFile, bool bDir, long nSize, DateTime dateTime, long compressedSize) //, SYSTEMTIME* pTime, CBetaPatchClientDlg* cB)
        {
            
            string  s_cdn = SERVER_ROOT;
            /* if (cB->m_Server_Combo.GetCurSel() == 1)
             {
                 s_cdn = "ShadowFlyff/Patcher-Void";
             }
            */

            FILE_INFO info = new FILE_INFO();
            /*
            if( pTime )
                SystemTimeToFileTime( pTime, &info.ft );
            */
            info.bDir = bDir;
            info.nFileSize = nSize;

            //GetCurrentDirectoryA(MAX_PATH, m_szCurrentDirectory);
            info.szPath = this.currentPath + szFile;
            //char szServerPath[MAX_PATH];

            //sprintf( szServerPath, "%s%s.gz", s_cdn, szFile );
            string str = s_cdn + szFile + ".gz";
            str.Replace( "\\", "/" );
            info.szServerPath = str;
            info.dateTime = dateTime;
            info.nCompressedFileSize = compressedSize;

#if __AEGON_FAST_COMPARE
            info.bUpToDate = false;
#endif // __AEGON_FAST_COMPARE


            files.Add(info);
        }

        private void ButtonSettings_Click(object sender, RoutedEventArgs e)
        {

            MoveOptionWindow();

            double blurEffect = 4;

            this.Effect = new BlurEffect() { Radius = blurEffect, };
            ImageDark.Visibility = Visibility.Visible;
            ImageDark.Opacity = 0.5;


            optionWindow.Visibility = Visibility.Visible;
            optionWindow.Opacity = 1;

            optionWindow.ShowDialog();

            this.Effect = null;
            
            ImageDark.Visibility = Visibility.Collapsed;
            ImageDark.Opacity = 0;

            MoveOptionWindow();

        }

        private void MoveOptionWindow() 
        {
            if (optionWindow == null)
                return;
            optionWindow.Left = this.Left + 523; //523
            optionWindow.Top = this.Top + 135;  //135

            if (optionWindow.Visibility == Visibility.Visible)
            {
                optionWindow.Focus();
                optionWindow.Opacity = 1;
            }

        }
        private void MainWnd_Activated(object sender, EventArgs e)
        {
            MoveOptionWindow();
        }
        public void ProgramClose(bool forced)
        {
            if (worker == null) 
            {
                programIsExiting = true;
                this.Close();
                Application.Current.Shutdown();
            }
            else if (worker.IsBusy && forced == false)
            {
                // Display message box
                MessageBoxResult result = MessageBox.Show("Patcher is still downloading. Press Ok to stop download and exit program", "Exit", MessageBoxButton.OKCancel);
                //worker.Resume();
                // Process message box results
                switch (result)
                {
                    case MessageBoxResult.OK:
                        // User pressed Yes button
                        // ...
                        //worker.Abort();
                        //worker.Dispose();
                        programIsExiting = true;
                        AegonTerminateThreads();
                        this.Close();
                        Application.Current.Shutdown();
                        break;
                    case MessageBoxResult.Cancel:
                        // User pressed Cancel button
                        // ...
                        programIsExiting = false;
                        break;
                }
            }
            else
            {
                programIsExiting = true;
                AegonTerminateThreads();
                this.Close();
                Application.Current.Shutdown();
            }
        }
    }

    struct FILE_INFO
    {
        public bool bDir;					//µð·ºÅä¸®ÀÎÁö?	
        public long nFileSize;				//ÆÄÀÏ Å©±â	
        public DateTime dateTime;						//ÆÄÀÏ ÃÖÁ¾ ¼öÁ¤½Ã°£	
        public string szPath;		//ÆÐ½º	
        public string szServerPath;
        public long nCompressedFileSize;
#if __AEGON_FAST_COMPARE
        public bool bUpToDate;
#endif // __AEGON_FAST_COMPARE
    }

    public class AbortableBackgroundWorker : BackgroundWorker
    {
        public Thread workerThread;

        public bool sleeping = false;
        public bool restartDownload = false;

        protected override void OnDoWork(DoWorkEventArgs e)
        {
            workerThread = Thread.CurrentThread;
            try
            {
                base.OnDoWork(e);
            }
            catch (ThreadAbortException)
            {
                e.Cancel = true; //We must set Cancel property to true!
                Thread.ResetAbort(); //Prevents ThreadAbortException propagation
            }
        }

        public void ProgramExit(bool forced)
        {
            if(forced == true)
            {
                Application.Current.Shutdown(0);
            }
        }

        public void RestartDownload()
        {
            restartDownload = true;
        }


        public void Abort()
        {
            if (workerThread != null)
            {
                workerThread.Abort();
                workerThread = null;
            }
        }

        public void Resume()
        {
            sleeping = false;
        }

        public void Pause()
        {
            sleeping = true;
        }
    }
}