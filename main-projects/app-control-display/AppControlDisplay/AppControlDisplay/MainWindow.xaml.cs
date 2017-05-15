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
        
        public Label label1;
        
        public BROSDisplay display1 = new BROSDisplay();

        private TcAdsClient tcClient;
        public const int ADSPORT = 350;
        private int hVarTcADS_Enable;
        private int hVarTcADS_Disable;
        private int hVarTcADS_StateRequest;
        private int hVarTcADS_Errors;
        private int hVarTcADS_SystemStates;
        private int hVarTcADS_OpsEnabled;

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
                hVarTcADS_Enable = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).ModelParameters.EnableDrives_Value");
                hVarTcADS_Disable = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).ModelParameters.DisableDrives_Value");
                hVarTcADS_StateRequest = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).ModelParameters.Requestedstate_Value");
                hVarTcADS_Errors = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).BlockIO.VecCon_Errors");
                hVarTcADS_SystemStates = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).BlockIO.VecCon_SystemStates");
                hVarTcADS_OpsEnabled = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).BlockIO.VecCon_OpsEnabled");
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
                tcClient.DeleteVariableHandle(hVarTcADS_Disable);
                tcClient.DeleteVariableHandle(hVarTcADS_StateRequest);
                tcClient.DeleteVariableHandle(hVarTcADS_Errors);
                tcClient.DeleteVariableHandle(hVarTcADS_SystemStates);
                tcClient.DeleteVariableHandle(hVarTcADS_OpsEnabled);
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
            int varHandleOut1 = tcThrClient.CreateVariableHandle("Object1 (ModelBaseBROS).Output.ControlWord_joint1");

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

            WriteTCClientShort(client, hVar, 1);
            WriteTCClientShort(client, hVar, 0);
        }

        void WriteTCClientDouble(TcAdsClient client, int hVar, double val)
        {
            // Send pulse to model through ADS client (send 1 and 0 consecutively)
            AdsStream dataStream = new AdsStream(sizeof(double));
            BinaryWriter binWrite = new BinaryWriter(dataStream);
            dataStream.Position = 0;

            try
            {
                binWrite.Write(val);
                client.Write(hVar, dataStream);
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }
        }

        void WriteTCClientShort(TcAdsClient client, int hVar, short val)
        {
            // Send pulse to model through ADS client (send 1 and 0 consecutively)
            AdsStream dataStream = new AdsStream(sizeof(short));
            BinaryWriter binWrite = new BinaryWriter(dataStream);
            dataStream.Position = 0;

            try
            {
                binWrite.Write(val);
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

        private void ButtonStateReq_Idle_Click(object sender, RoutedEventArgs e)
        {
            WriteTCClientDouble(tcClient, hVarTcADS_StateRequest, 0);
        }

        private void ButtonStateReq_Calibrate_Click(object sender, RoutedEventArgs e)
        {
            WriteTCClientDouble(tcClient, hVarTcADS_StateRequest, 2);
        }

        private void ButtonStateReq_HomingManual_Click(object sender, RoutedEventArgs e)
        {
            WriteTCClientDouble(tcClient, hVarTcADS_StateRequest, 301);
        }

        private void ButtonStateReq_HomingAuto_Click(object sender, RoutedEventArgs e)
        {
            WriteTCClientDouble(tcClient, hVarTcADS_StateRequest, 302);
        }

        private void ButtonStateReq_Run_Click(object sender, RoutedEventArgs e)
        {
            WriteTCClientDouble(tcClient, hVarTcADS_StateRequest, 4);
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
