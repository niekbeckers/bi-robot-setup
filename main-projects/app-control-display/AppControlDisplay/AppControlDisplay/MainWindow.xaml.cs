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

        public MainWindow()
        {
            InitializeComponent();

            // create display1
            
            display1.Show();
            if (!display1.IsLoaded) {
                display1.Owner = this;
            }
            

            // create threads
            Thread thrContRecCalc = new Thread(new ThreadStart(ThreadContReceiveCalculate));

            // start threads
            thrContRecCalc.Start();
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
            TcAdsClient tcClient = new TcAdsClient();
            tcClient.Connect(350); // port 350

            // Connect to TMC output
            int varHandleOut1 = tcClient.CreateVariableHandle("Object1 (ModelSine).Output.Out1");

            int length;
            double data;

            while (thrRunning)
            {
                // Read varHandleOut1
                length = tcClient.Read(varHandleOut1, dataStream);
                data = binRead.ReadDouble();
                dataStream.Position = 0;
                x = data;

                // Update label text
                App.Current.Dispatcher.Invoke((Action)delegate { this.label1.Content = x.ToString(); });

                // Reduce processor load, 200Hz
                Thread.Sleep(5);
            }

            // Release variable handle(s), dispose of tcClient
            try
            {
                tcClient.DeleteVariableHandle(varHandleOut1);
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }
            tcClient.Dispose();
        } 

        private void Label_Loaded(object sender, RoutedEventArgs e)
        {
            // ... get the label
            label1 = sender as Label;
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

            // Close other windows
            display1.Close();
        }
    }
}
