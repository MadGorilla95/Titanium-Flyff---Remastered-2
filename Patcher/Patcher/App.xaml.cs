using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Security.AccessControl;
using System.Security.Principal;
using System.Threading;
using System.Windows;
using System;
using System.IO.Compression;
using System.Net;

namespace PatchClient
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
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

        public bool IsUserAdministrator()
        {
            bool isAdmin;
            try
            {
                WindowsIdentity user = WindowsIdentity.GetCurrent();
                WindowsPrincipal principal = new WindowsPrincipal(user);
                isAdmin = principal.IsInRole(WindowsBuiltInRole.Administrator);
            }
            catch (UnauthorizedAccessException)
            {
                isAdmin = false;
            }
            catch (Exception)
            {
                isAdmin = false;
            }
            return isAdmin;
        }

        private void Start_Neuz32()
        {
            try
            {
                ProcessStartInfo startInfo = new ProcessStartInfo();
                string path = System.IO.Path.GetDirectoryName(
                      System.Windows.Forms.Application.ExecutablePath);

                startInfo.FileName = path + "\\" + MainWnd.exeName;

                startInfo.Arguments = MainWnd.startUpSalt;
                Process.Start(startInfo);
                this.Shutdown(0);
                return;
            }
            catch (Exception)
            {
                MessageBox.Show(MainWnd.exeName + " file not found\n");
                this.Shutdown(0);
                return;
            }
        }
        private void Start_Neuz64()
        {
            try
            {
                ProcessStartInfo startInfo = new ProcessStartInfo();
                string path = System.IO.Path.GetDirectoryName(
                      System.Windows.Forms.Application.ExecutablePath);

                startInfo.FileName = path + "\\" + MainWnd.exeName64;

                startInfo.Arguments = MainWnd.startUpSalt;
                Process.Start(startInfo);
                this.Shutdown(0);
                return;
            }
            catch (Exception)
            {
                MessageBox.Show(MainWnd.exeName64 + " file not found\n");
                this.Shutdown(0);
                return;
            }
        }

        App()
        {
            string currentPath = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath) + "\\";

            Process[] pname = Process.GetProcessesByName(MainWnd.gameMutexName);
            Process[] pname64 = Process.GetProcessesByName(MainWnd.gameMutexName64);
            if (pname64.Length != 0)
            {
                Start_Neuz64();
            }
            else if (pname.Length != 0)
            {
                Start_Neuz32();
            }

            if (SkipPatch(MainWnd.patcherMutexName))
            {
                MessageBox.Show("Patch client already running.\nNow closing.");
                this.Shutdown(0);
                return;
            }
        }
    }
}
