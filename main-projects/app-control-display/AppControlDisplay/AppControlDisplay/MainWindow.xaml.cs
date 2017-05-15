using System;
using System.Collections.Generic;
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
using System.Threading;
using System.IO;
using System.IO.Ports;
using System.Diagnostics;
using System.Net;
using TwinCAT.Ads;

namespace AppControlDisplay
{

    

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        // declare variables
        public bool thrRunning = false;
        public static double x = 0;
        public const int ADSPORT = 350;

        public Label label1;
        
        public BROSDisplay display1 = new BROSDisplay();

        private TcAdsClient tcClient;
        private int hVarTcADS_Enable;
        private int hVarTcADS_Disable;

        public MainWindow()
        {
            InitializeComponent();

            // Create display1
            display1.Show();

            // Create TcAdsClient, set up variable handles
            CreateSetupTcADSClient();

            // Threads
            //Thread thrContRecCalc = new Thread(new ThreadStart(ThreadContReceiveCalculate));

            // Start threads
            //thrContRecCalc.Start();
        }

        void CreateSetupTcADSClient()
        {
            // Create and connect TcAdsClient to port
            tcClient = new TcAdsClient();
            tcClient.Connect(ADSPORT);

            // Create variable handles
            try
            {
                hVarTcADS_Enable = tcClient.CreateVariableHandle("Object1 (ModelSine).Input.In1");
                //hVarTcADS_Disable = tcClient.CreateVariableHandle("MAIN.PLCVar");
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }

        }

        void DisposeTcADSClient()
        {
            try
            {
                tcClient.DeleteVariableHandle(hVarTcADS_Enable);
                //tcClient.DeleteVariableHandle(hVarTcADS_Disable);
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }
            tcClient.Dispose();
        }

        /// <summary>
        /// Function to continuously receive data from the ADS and perform calculations
        /// </summary>
        void ThreadContReceiveCalculate()
        {
            // Thread is now running
            thrRunning = true;

            AdsStream dataStream = new AdsStream(8);
            BinaryReader binRead = new BinaryReader(dataStream);

            // Create and connect to ads client
            TcAdsClient tcThrClient = new TcAdsClient();
            tcThrClient.Connect(ADSPORT);

            // Connect to TMC output
            int varHandleOut1 = tcThrClient.CreateVariableHandle("Object1 (ModelSine).Output.Out1");

            double data;

            while (thrRunning)
            {
                try
                {
                    // Read varHandleOut1 (array)
                    tcThrClient.Read(varHandleOut1, dataStream);
                    data = binRead.ReadDouble();
                    dataStream.Position = 0;

                    // Save data in global variable
                    x = data;
                }
                catch //(Exception err)
                {
                    //MessageBox.Show(err.Message);
                }

                // Update label text
                App.Current.Dispatcher.Invoke((Action)delegate { this.label1.Content = x.ToString(); });

                // Reduce processor load, 200Hz
                Thread.Sleep(5);
            }

            // Thread done. Release variable handle(s), dispose of tcClient
            try
            {
                tcThrClient.DeleteVariableHandle(varHandleOut1);
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }
            tcThrClient.Dispose();
            dataStream.Dispose();
            binRead.Dispose();
        } 

        void SendPulseToTC(TcAdsClient client, int hVar)
        {
            // Send pulse to model through ADS client (send 1 and 0 consecutively)
            AdsStream dataStream = new AdsStream(2);
            BinaryWriter binWrite = new BinaryWriter(dataStream);
            dataStream.Position = 0;

            try
            {
                // Write 1
                binWrite.Write((short)1);
                client.Write(hVar, dataStream);

                // Write 0
                binWrite.Flush();
                dataStream.Flush();
                dataStream.Position = 0;
                binWrite.Write((short)0);
                client.Write(hVar, dataStream);
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }
        }

        
        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            // Ctrl+T: terminate program
            if ((e.Key == Key.LeftCtrl) && (e.Key == Key.T))
            {
                thrRunning = false;
                Thread.Sleep(100);
                Application.Current.Shutdown();
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            // stop thread
            thrRunning = false;
            Thread.Sleep(100);

            // Dispose TcADSClient
            DisposeTcADSClient();

            // Close other windows
            display1.Close();
        }

        private void ButtonEnable_Click(object sender, RoutedEventArgs e)
        {
            // Send Enable Drive command to Simulink model
            SendPulseToTC(tcClient, hVarTcADS_Enable);
        }

        private void ButtonDisable_Click(object sender, RoutedEventArgs e)
        {
            // Send Disable Drive command to Simulink model
            SendPulseToTC(tcClient, hVarTcADS_Disable);
        }

        
        // Load all 
        private void Label_Loaded(object sender, RoutedEventArgs e)
        {
            // ... get the label
            label1 = sender as Label;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            display1.Owner = this;
        }

    }
}
