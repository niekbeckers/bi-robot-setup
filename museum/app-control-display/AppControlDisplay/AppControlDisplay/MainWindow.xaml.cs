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
        public double[] x = new double[8];
        private bool thrRunning = true;
        private Thread ThreadCyclicTask;

        static readonly object locker = new object();

        public BROSDisplay display1 = new BROSDisplay(1);

        private TcAdsClient tcClient;
        public const int ADSPORT = 350;
       
        private int hVarTcADS_Write_Enable;
        private int hVarTcADS_Write_Disable;
        private int hVarTcADS_Write_StateRequest;
        private int hVarTcADS_Read_Errors;
        private int hVarTcADS_Read_SystemStates;
        private int hVarTcADS_Read_OpsEnabled;
        private int hVarTcADS_Read_LoggerPaused;
        private int hVarTcADS_Read_DataToADS;

        private TCADSClientWorker tcWorker = new TCADSClientWorker();

        public MainWindow()
        {
            InitializeComponent();

            // Create display1
            display1.Show();

            // Create TcAdsClient, set up variable handles
            CreateSetupTcADSClient();

            ThreadCyclicTask = new Thread(AddCyclicTasks);
            //ThreadCyclicTask.Start();
        }


        public void AddCyclicTasks()
        {
            while(thrRunning)
            {
                //lock (locker)
                //{
                    tcWorker.EnqueueTask(new TCADSTaskRead(tcClient, hVarTcADS_Read_DataToADS, ref x));
                //}
                Thread.Sleep(5);
            }
            

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
                hVarTcADS_Read_DataToADS = tcClient.CreateVariableHandle("Object1 (ModelBaseBROS).Output.DataToADS");
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
                tcClient.DeleteVariableHandle(hVarTcADS_Read_DataToADS);
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
            // Close cyclic task thread
            thrRunning = false;
            ThreadCyclicTask.Join();

            // Dispose of worker
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
            Console.WriteLine("x1: " + x[0].ToString() + " y1: " + x[1].ToString());
        }

        private void ButtonStateReq_Calibrate_Click(object sender, RoutedEventArgs e)
        {
            tcWorker.EnqueueTask(new TCADSTaskWrite(tcClient, hVarTcADS_Write_StateRequest, 2.0));
            Console.WriteLine("x1: " + x[0].ToString() + " y1: " + x[1].ToString());
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
    }
}
