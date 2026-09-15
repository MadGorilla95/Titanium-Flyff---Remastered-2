#define __AEGON_ALLOW_ALL_RESOLUTIONS

using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;


namespace PatchClient
{
    /// <summary>
    /// Interaction logic for Options.xaml
    /// </summary>
    public partial class Options : Window
    {
        string currentPath;
        string fileName = MainWnd.configFileName;
        string configFileName = MainWnd.configFileName;

        string start_32bit = "0";

        public Options(string currentPath)
        {
            this.currentPath = currentPath;
            InitializeComponent();
            PopulateResolutions();
            GetValues();

            ButtonSave.Content = FindResource("saveNormal");
            ButtonCancel.Content = FindResource("cancelNormal");

        }

        public bool Start32BitNeuz() 
        {
            if (start_32bit == "0")
                return false;
            else
                return true;
        }

        public void GetValues()
        {

            string detail = "2";
            string distant = "2";
            string view = "2";
            string shadow = "2";
            string fullscreen = "0";
            string resolution = "800 600";

            string ani_filter = "0";
            string antialiasing = "0";
            string low_render = "0";

            start_32bit = "0";

            FileInfo fi1 = new FileInfo(currentPath + configFileName);
            if (fi1.Exists)
            {
                using (StreamReader fp = File.OpenText(currentPath + configFileName))
                {
                    while (fp.EndOfStream == false)
                    {
                        string szBuffer = fp.ReadLine();
                        string[] splitBuffer = szBuffer.Split(null);
                        switch(splitBuffer[0].ToLower())
                        {
                            case "detail":
                                detail = splitBuffer[1];
                                break;
                            case "distant":
                                distant = splitBuffer[1];
                                break;
                            case "view":
                                view = splitBuffer[1];
                                break;
                            case "shadow":
                                shadow = splitBuffer[1];
                                break;
                            case "fullscreen":
                                fullscreen = splitBuffer[1];
                                break;
                            case "resolution":
                                resolution = splitBuffer[1] + " " + splitBuffer[2];
                                break;
                            case "anisotropic":
                                ani_filter = splitBuffer[1];
                                break;
                            case "antialiasing":
                                antialiasing = splitBuffer[1];
                                break;
                            case "inactiverender":
                                low_render = splitBuffer[1];
                                break;
                            case "32bitMode":
                                start_32bit = splitBuffer[1];
                                break;
                        }
                    }
                }
            }
            

            switch (detail)
            {
                case "0":
                    this.ObjectHigh.IsChecked = true;
                    break;
                case "1":
                    this.ObjectMid.IsChecked = true;
                    break;
                case "2":
                    this.ObjectLow.IsChecked = true;
                    break;
                default:
                    this.ObjectLow.IsChecked = true;
                    break;
            }

            switch (distant)
            {
                case "0":
                    this.RangeFar.IsChecked = true;
                    break;
                case "1":
                    this.RangeMid.IsChecked = true;
                    break;
                case "2":
                    this.RangeLow.IsChecked = true;
                    break;
                default:
                    this.RangeLow.IsChecked = true;
                    break;
            }

            switch (view)
            {
                case "0":
                    this.TextureHigh.IsChecked = true;
                    break;
                case "1":
                    this.TextureMid.IsChecked = true;
                    break;
                case "2":
                    this.TextureLow.IsChecked = true;
                    break;
                default:
                    this.TextureLow.IsChecked = true;
                    break;
            }

            switch (shadow)
            {
                case "0":
                    this.ShadowHigh.IsChecked = true;
                    break;
                case "1":
                    this.ShadowMid.IsChecked = true;
                    break;
                case "2":
                    this.ShadowLow.IsChecked = true;
                    break;
                default:
                    this.ShadowLow.IsChecked = true;
                    break;
            }


            string[] resSplit = resolution.Split(" ".ToCharArray());
            if (resSplit.Length > 1)
            {
                for (int i = 0; i < Resolution.Items.Count; i++)
                {
#if __AEGON_ALLOW_ALL_RESOLUTIONS
                    if (Resolution.Items.Contains(resSplit[0] + "x" + resSplit[1]) == false)
                    {
                        Resolution.Items.Add(resSplit[0] + "x" + resSplit[1]);
                    }
#endif // __AEGON_ALLOW_ALL_RESOLUTIONS

                    if (Resolution.Items.GetItemAt(i).Equals(resSplit[0] + "x" + resSplit[1]))
                    {
                        Resolution.SelectedIndex = i;
                        break;
                    }

                }
            }
            else
            {
                Resolution.SelectedIndex = 0;
            }

            FullScreen.IsChecked = fullscreen == "0" ? false : true;
            Ani_filter_chkbox.IsChecked = ani_filter == "0" ? false : true;
            antialiasing_chkbox.IsChecked = antialiasing == "0" ? false : true;
            low_render_chkbox.IsChecked = low_render == "0" ? false : true;
            start_32bit_filter_chkbox.IsChecked = start_32bit == "0" ? false : true;

        }

        public void SaveValues()
        {
            int detail = SelectedRadioValue<int>(0, ObjectHigh, ObjectMid, ObjectLow);
            int distant = SelectedRadioValue<int>(0, RangeFar, RangeMid, RangeLow);
            int view = SelectedRadioValue<int>(0, TextureHigh, TextureMid, TextureLow);
            int shadow = SelectedRadioValue<int>(0, ShadowHigh, ShadowMid, ShadowLow);
            string resolution = Resolution.SelectedValue.ToString();
            string[] res = resolution.Split("x".ToCharArray());
            string fullscreen = FullScreen.IsChecked == true ? "1" : "0";

            string ani_filter = Ani_filter_chkbox.IsChecked == true ? "1" : "0";
            string antialiasing = antialiasing_chkbox.IsChecked == true ? "1" : "0";
            string low_render = low_render_chkbox.IsChecked == true ? "1" : "0";
            start_32bit = start_32bit_filter_chkbox.IsChecked == true ? "1" : "0";


            string newBuffer = "";

            if (!File.Exists(currentPath + configFileName)) 
                return;
           
            using (StreamReader fp = File.OpenText(currentPath + configFileName))
            {
                while (fp.EndOfStream == false)
                {
                    string szBuffer = fp.ReadLine();
                    string[] splitBuffer = szBuffer.Split(null);
                    switch (splitBuffer[0].ToLower())
                    {
                        case "detail":
                            szBuffer = "detail " + detail.ToString();
                            break;
                        case "distant":
                            szBuffer = "distant " + distant.ToString();
                            break;
                        case "view":
                            szBuffer = "view " + view.ToString();
                            break;
                        case "shadow":
                            szBuffer = "shadow " + shadow.ToString();
                            break;
                        case "fullscreen":
                            szBuffer = "fullscreen " + fullscreen.ToString();
                            break;
                        case "resolution":
                            szBuffer = "resolution " + res[0] + " " + res[1];
                            break;
                        case "anisotropic":
                            szBuffer = "Anisotropic " + ani_filter.ToString();
                            break;
                        case "antialiasing":
                            szBuffer = "Antialiasing " + antialiasing.ToString();
                            break;
                        case "inactiverender":
                            szBuffer = "inactiveRender " + low_render.ToString();
                            break;
                        case "32bitmode":
                            szBuffer = "32bitMode " + low_render.ToString();
                            break;
                    }
                    newBuffer += szBuffer + "\n";
                }
            }
            File.Delete(currentPath + configFileName);
            using(StreamWriter fp = File.CreateText(currentPath + configFileName))
            {
                string[] splitString = newBuffer.Split('\n');
                foreach (string strInSplit in splitString)
                {
                    fp.WriteLine(strInSplit);
                }
                fp.Close();
            }
            
        }

        public T SelectedRadioValue<T>(T defaultValue, params RadioButton[] buttons)
        {
            foreach (RadioButton button in buttons)
            {
                if (button.IsChecked == true)
                {
                    if (button.Tag is string && typeof(T) != typeof(string))
                    {
                        string value = (string)button.Tag;
                        return (T)Convert.ChangeType(value, typeof(T));
                    }

                    return (T)button.Tag;
                }
            }

            return defaultValue;
        }

        private void ButtonSave_MouseEnter(object sender, MouseEventArgs e)
        {
            ButtonSave.Content = FindResource("saveHover");
        }

        private void ButtonSave_MouseLeave(object sender, MouseEventArgs e)
        {
            ButtonSave.Content = FindResource("saveNormal");
        }

        private void ButtonSave_Click(object sender, RoutedEventArgs e)
        {
            SaveValues();
            //this.Close();
            this.Hide();
        }

        private void ButtonCancel_MouseEnter(object sender, MouseEventArgs e)
        {
            ButtonCancel.Content = FindResource("cancelHover");
        }

        private void ButtonCancel_MouseLeave(object sender, MouseEventArgs e)
        {
            ButtonCancel.Content = FindResource("cancelNormal");
        }

        private void ButtonCancel_Click(object sender, RoutedEventArgs e)
        {
            //this.Close();
            this.Hide();
        }


        [StructLayout(LayoutKind.Sequential)]
        public struct DEVMODE
        {

            private const int CCHDEVICENAME = 0x20;
            private const int CCHFORMNAME = 0x20;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 0x20)]
            public string dmDeviceName;
            public short dmSpecVersion;
            public short dmDriverVersion;
            public short dmSize;
            public short dmDriverExtra;
            public int dmFields;
            public int dmPositionX;
            public int dmPositionY;
            public System.Windows.Forms.ScreenOrientation dmDisplayOrientation;
            public int dmDisplayFixedOutput;
            public short dmColor;
            public short dmDuplex;
            public short dmYResolution;
            public short dmTTOption;
            public short dmCollate;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 0x20)]
            public string dmFormName;
            public short dmLogPixels;
            public int dmBitsPerPel;
            public int dmPelsWidth;
            public int dmPelsHeight;
            public int dmDisplayFlags;
            public int dmDisplayFrequency;
            public int dmICMMethod;
            public int dmICMIntent;
            public int dmMediaType;
            public int dmDitherType;
            public int dmReserved1;
            public int dmReserved2;
            public int dmPanningWidth;
            public int dmPanningHeight;

        }

        [DllImport("user32.dll")]
        public static extern bool EnumDisplaySettings(
              string deviceName, int modeNum, ref DEVMODE devMode);
        const int ENUM_CURRENT_SETTINGS = -1;

        const int ENUM_REGISTRY_SETTINGS = -2;

        void PopulateResolutions()
        {
            bool[] checkRes = { false, false, false, false, false, false, false, false, false, false, false, false, false, false };
            
            DEVMODE vDevMode = new DEVMODE();
            int i = 0;
            while (EnumDisplaySettings(null, i, ref vDevMode))
            {

                if (!checkRes[0] && vDevMode.dmPelsWidth == 800 && vDevMode.dmPelsHeight == 600)
				{
					if(Resolution.Items.Contains("800x600") == false)
					{
						Resolution.Items.Add("800x600" );
						checkRes[0] = true;
					}
				}
				else if( !checkRes[1] && vDevMode.dmPelsWidth == 1024 && vDevMode.dmPelsHeight == 768 )
				{
					if(Resolution.Items.Contains("1024x768") == false)
					{
						Resolution.Items.Add("1024x768" );
						checkRes[1] = true;
					}
				}
		        else if( !checkRes[2] && vDevMode.dmPelsWidth == 1280 && vDevMode.dmPelsHeight == 720 )
		        {
			        if(Resolution.Items.Contains("1280x720") == false)
			        {
				        Resolution.Items.Add("1280x720" );
				        checkRes[2] = true;
			        }
		        }
		        else if( !checkRes[3] && vDevMode.dmPelsWidth == 1280 && vDevMode.dmPelsHeight == 768 )
		        {
			        if(Resolution.Items.Contains("1280x768") == false)
			        {
				        Resolution.Items.Add("1280x768" );
				        checkRes[3] = true;
			        }
		        }
		        else if( !checkRes[4] && vDevMode.dmPelsWidth == 1280 && vDevMode.dmPelsHeight == 800 )
		        {
			        if(Resolution.Items.Contains("1280x800") == false)
			        {
				        Resolution.Items.Add("1280x800" );
				        checkRes[4] = true;
			        }
		        }
		        else if( !checkRes[5] && vDevMode.dmPelsWidth == 1280 && vDevMode.dmPelsHeight == 1024 )
		        {
			        if(Resolution.Items.Contains("1280x1024") == false)
			        {
				        Resolution.Items.Add("1280x1024" );
				        checkRes[5] = true;
			        }
		        }
		        else if( !checkRes[6] && vDevMode.dmPelsWidth == 1360 && vDevMode.dmPelsHeight == 768 )
		        {
			        if(Resolution.Items.Contains("1360x768") == false)
			        {
				        Resolution.Items.Add("1360x768" );
				        checkRes[6] = true;
			        }
		        }
		        else if( !checkRes[7] && vDevMode.dmPelsWidth == 1440 && vDevMode.dmPelsHeight == 900 )
		        {
			        if(Resolution.Items.Contains("1440x900") == false)
			        {
				        Resolution.Items.Add("1440x900" );
				        checkRes[7] = true;
			        }
		        }
		        else if( !checkRes[8] && vDevMode.dmPelsWidth == 1400 && vDevMode.dmPelsHeight == 1050 )
		        {
			        if(Resolution.Items.Contains("1400x1050") == false)
			        {
				        Resolution.Items.Add("1400x1050" );
				        checkRes[8] = true;
			        }
                }
                else if (!checkRes[9] && vDevMode.dmPelsWidth == 1600 && vDevMode.dmPelsHeight == 900)
                {
                    if (Resolution.Items.Contains("1600x900") == false)
                    {
                        Resolution.Items.Add("1600x900");
                        checkRes[9] = true;
                    }
                }
                else if (!checkRes[9] && vDevMode.dmPelsWidth == 1600 && vDevMode.dmPelsHeight == 1200)
                {
                    if (Resolution.Items.Contains("1600x1200") == false)
                    {
                        Resolution.Items.Add("1600x1200");
                        checkRes[9] = true;
                    }
                }
		        else if( !checkRes[10] && vDevMode.dmPelsWidth == 1680 && vDevMode.dmPelsHeight == 1050 )
		        {
			        if(Resolution.Items.Contains("1680x1050") == false)
			        {
				        Resolution.Items.Add("1680x1050" );
				        checkRes[10] = true;
			        }
		        }
		        else if( !checkRes[11] && vDevMode.dmPelsWidth == 1920 && vDevMode.dmPelsHeight == 1080 )
		        {
			        if(Resolution.Items.Contains("1920x1080") == false)
			        {
				        Resolution.Items.Add("1920x1080" );
				        checkRes[11] = true;
			        }
		        }
		        else if( !checkRes[12] && vDevMode.dmPelsWidth == 2560 && vDevMode.dmPelsHeight == 1440 )
		        {
			        if(Resolution.Items.Contains("2560x1440") == false)
			        {
				        Resolution.Items.Add("2560x1440" );
				        checkRes[12] = true;
			        }
		        }
                else if (!checkRes[13] && vDevMode.dmPelsWidth == 3840 && vDevMode.dmPelsHeight == 2160)
                {
                    if (Resolution.Items.Contains("3840x2160") == false)
                    {
                        Resolution.Items.Add("3840x2160");
                        checkRes[13] = true;
                    }
                }
                i++;
                
	        }
        }
        
    }
}
