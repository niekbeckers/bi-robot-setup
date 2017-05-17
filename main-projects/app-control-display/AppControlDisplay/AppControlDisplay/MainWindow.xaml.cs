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
        public static double x = 0;

       
        
        public BROSDisplay display1 = new BROSDisplay();

        private TcAdsClient tcClient;
        public const int ADSPORT = 350;
       
        private int hVarTcADS_Write_Enable;
        private int hVarTcADS_Write_Disable;
        private int hVarTcADS_Write_StateRequest;
        private int hVarTcADS_Read_Errors;
        private int hVarTcADS_Read_SystemStates;
        private int hVarTcADS_Read_OpsEnabled;
        private int hVarTcADS_Read_LoggerPaused;

        private TCADSClientWorker tcWorker = new TCADSClientWorker();

        public MainWindow()
        {
            InitializeComponent();

            // Create display1
            display1.Show();

            // Create TcAdsClient, set up variable handles
            CreateSetupTcADSClient();


            Console.WriteLine(typeof(double).ToString());
            

            //short myShort = 1;
            //tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Enable, myShort));

            //double[] myDoubleArray = { 1.0, 2.0, 3.0 };
            //tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Enable, myDoubleArray));
        }

        void CreateSetupTcADSClient()
        {
            // Create and connect TcAdsClient to port
            tcClient = new TcAdsClient();
            tcClient.Connect(ADSPORT);

            // Create variable handles
            try
            {
                hVarTcADS_Write_Enable = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).ModelParameters.EnableDrives_Value");
                hVarTcADS_Write_Disable = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).ModelParameters.DisableDrives_Value");
                hVarTcADS_Write_StateRequest = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).ModelParameters.Requestedstate_Value");
                hVarTcADS_Read_Errors = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).BlockIO.VecCon_Errors");
                hVarTcADS_Read_SystemStates = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).BlockIO.VecCon_SystemStates");
                hVarTcADS_Read_OpsEnabled = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).BlockIO.VecCon_OpsEnabled");
                hVarTcADS_Read_LoggerPaused = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).Output.pause_logger");
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
                tcClient.DeleteVariableHandle(hVarTcADS_Write_Enable);
                tcClient.DeleteVariableHandle(hVarTcADS_Write_Disable);
                tcClient.DeleteVariableHandle(hVarTcADS_Write_StateRequest);
                tcClient.DeleteVariableHandle(hVarTcADS_Read_Errors);
                tcClient.DeleteVariableHandle(hVarTcADS_Read_SystemStates);
                tcClient.DeleteVariableHandle(hVarTcADS_Read_OpsEnabled);
                tcClient.DeleteVariableHandle(hVarTcADS_Read_LoggerPaused);
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }
            tcClient.Dispose();
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            // Ctrl+T: terminate program
            if ((e.Key == Key.LeftCtrl) && (e.Key == Key.T))
            {
                Application.Current.Shutdown();
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {

            //
            tcWorker.Dispose();

            // Dispose TcADSClient
            DisposeTcADSClient();

            // Close other windows
            display1.Close();
        }

        private void ButtonEnable_Click(object sender, RoutedEventArgs e)
        {
            // Send Enable Drive command to Simulink model
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_Enable, (short)1));
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_Enable, (short)0));
        }

        private void ButtonDisable_Click(object sender, RoutedEventArgs e)
        {
            // Send Disable Drive command to Simulink model
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_Disable, (short)1));
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_Disable, (short)0));
        }

        private void ButtonStateReq_Idle_Click(object sender, RoutedEventArgs e)
        {
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_StateRequest, 0.0));
        }

        private void ButtonStateReq_Calibrate_Click(object sender, RoutedEventArgs e)
        {
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_StateRequest, 2.0));
        }

        private void ButtonStateReq_HomingManual_Click(object sender, RoutedEventArgs e)
        {
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_StateRequest, 301.0));
        }

        private void ButtonStateReq_HomingAuto_Click(object sender, RoutedEventArgs e)
        {
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_StateRequest, 302.0));
        }

        private void ButtonStateReq_Run_Click(object sender, RoutedEventArgs e)
        {
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_StateRequest, 4.0));
        }
        
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            display1.Owner = this;
        }


        ///// <summary>
        ///// Function to continuously receive data from the ADS and perform calculations
        ///// </summary>
        //void ThreadContReceiveCalculate()
        //{
        //    // Thread is now running
        //    thrRunning = true;

        //    AdsStream dataStream = new AdsStream(8);
        //    BinaryReader binRead = new BinaryReader(dataStream);

        //    // Create and connect to ads client
        //    TcAdsClient tcThrClient = new TcAdsClient();
        //    tcThrClient.Connect(ADSPORT);

        //    // Connect to TMC output
        //    int varHandleOut1 = tcThrClient.CreateVariableHandle("Object1 (ModelBaseBROS).Output.ControlWord_joint1");

        //    double data;

        //    while (thrRunning)
        //    {
        //        try
        //        {
        //            // Read varHandleOut1 (array)
        //            tcThrClient.Read(varHandleOut1, dataStream);
        //            data = binRead.ReadDouble();
        //            dataStream.Position = 0;

        //            //binRead.Read
        //            // Save data in global variable
        //            x = data;
        //        }
        //        catch //(Exception err)
        //        {
        //            //MessageBox.Show(err.Message);
        //        }

        //        // Update label text
        //        // App.Current.Dispatcher.Invoke((Action)delegate { this.label1.Content = x.ToString(); });

        //        // Reduce processor load, 200Hz
        //        Thread.Sleep(5);
        //    }

        //    // Thread done. Release variable handle(s), dispose of tcClient
        //    try
        //    {
        //        tcThrClient.DeleteVariableHandle(varHandleOut1);
        //    }
        //    catch (Exception err)
        //    {
        //        MessageBox.Show(err.Message);
        //    }
        //    tcThrClient.Dispose();
        //    dataStream.Dispose();
        //    binRead.Dispose();
        //} 

    }
}
