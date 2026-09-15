using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace PatchClient
{
    /// <summary>
    /// Displays a WebBrowser control over a given placement target element in a WPF Window.
    /// The owner window can be transparent, but not this one, due mixing DirectX and GDI drawing. 
    /// WebBrowserOverlayWF uses WinForms to avoid this limitation.
    /// </summary>
    public partial class Webbox : Window
    {
        FrameworkElement _placementTarget;
        public WebBrowser WebBrowser { get { return _wb; } }

        [DllImport("user32.dll")]
        static extern bool SetWindowPos(
            IntPtr hWnd,
            IntPtr hWndInsertAfter,
            int X,
            int Y,
            int cx,
            int cy,
            uint uFlags);

        const UInt32 SWP_NOSIZE = 0x0001;
        const UInt32 SWP_NOMOVE = 0x0002;
        const UInt32 SWP_NOACTIVATE = 0x0010;

        static int _parentHandle=0;

        static void SendWpfWindowBack(Window window)
        {
            var hWnd = new WindowInteropHelper(window).Handle;
            SetWindowPos(hWnd, new IntPtr(_parentHandle), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
        }

        public Webbox(FrameworkElement placementTarget, IntPtr parentHandle)
        {
            _parentHandle = parentHandle.ToInt32();

            InitializeComponent();

            _placementTarget = placementTarget;
            MainWnd.webbox = Window.GetWindow(placementTarget);
            Debug.Assert(MainWnd.webbox != null);

            //owner.SizeChanged += delegate { OnSizeLocationChanged(); };
            MainWnd.webbox.LocationChanged += delegate { OnSizeLocationChanged(); };
            _placementTarget.SizeChanged += delegate { OnSizeLocationChanged(); };
            //_placementTarget.MouseDown += delegate { _wb_MouseDown(); };
            //MainWnd.webbox.MouseDown += delegate { _wb_MouseDown(); };

            if (MainWnd.webbox.IsVisible)
            {
                Owner = MainWnd.webbox;
                Show();
            }
            else
            {
                MainWnd.webbox.IsVisibleChanged += delegate
                {
                    if (MainWnd.webbox.IsVisible)
                    {
                        Owner = MainWnd.webbox;
                        Show();
                    }
                    else
                    {
                        Owner = null;
                        Hide();
                    }
                };
            }
            
            //owner.LayoutUpdated += new EventHandler(OnOwnerLayoutUpdated);
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            base.OnClosing(e);
            if (!e.Cancel)
                // Delayed call to avoid crash due to Window bug.
                Dispatcher.BeginInvoke((Action)delegate
                {
                    Owner.Close();
                });
        }

        void OnSizeLocationChanged()
        {
            try
            {
                Point offset = _placementTarget.TranslatePoint(new Point(), Owner);
                Point size = new Point(_placementTarget.ActualWidth, _placementTarget.ActualHeight);
                HwndSource hwndSource = (HwndSource)HwndSource.FromVisual(Owner);
                CompositionTarget ct = hwndSource.CompositionTarget;
                offset = ct.TransformToDevice.Transform(offset);
                size = ct.TransformToDevice.Transform(size);

                MainWnd.POINT screenLocation = new MainWnd.POINT(offset);
                MainWnd.ClientToScreen(hwndSource.Handle, ref screenLocation);
                MainWnd.POINT screenSize = new MainWnd.POINT(size);

                MainWnd.MoveWindow(((HwndSource)HwndSource.FromVisual(this)).Handle, screenLocation.X, screenLocation.Y, screenSize.X, screenSize.Y, true);
            }
            catch
            {

            }
        }

        void OnMouseEnter()
        {

        }

        private void Window_Activated(object sender, EventArgs e)
        {
            SendWpfWindowBack(this);
        }

        private void _wb_MouseDown(object sender, MouseButtonEventArgs e)
        {

        }
    }
}
