using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Diagnostics;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using SharpGL;
using SharpGL.SceneGraph;
using SharpGL.Enumerations;

namespace AppControlDisplay
{
    /// <summary>
    /// Interaction logic for Display1.xaml
    /// </summary>
    public partial class BROSDisplay : Window
    {

        private double[] data = new double[8];

        public static double scrHeight;
        public static double scrWidth;

        public const double inch_on_screen = 27; // inch
        public static double dots_per_meter; //dots per meter

        private static double time = 0.0;

        private double x1old = 0.0;
        private double x2old = 0.0;

        private Stopwatch sw = new Stopwatch();

        public BROSDisplay(int scrNum)
        {
            InitializeComponent();

            scrHeight = SystemParameters.PrimaryScreenHeight;
            scrWidth = SystemParameters.PrimaryScreenWidth;

            Console.WriteLine("height " + scrHeight.ToString() + " width " + scrWidth.ToString());
            dots_per_meter =  Math.Sqrt(scrHeight * scrHeight + scrWidth * scrWidth) / inch_on_screen * 100.0 / 2.54;
            sw.Start();
        }

        private void OpenGLControl_OpenGLInitialized(object sender, SharpGL.SceneGraph.OpenGLEventArgs args)
        {
            //  Get the OpenGL object.
            OpenGL gl = openGLControl.OpenGL;

            //  Set the clear color.
            gl.ClearColor(0, 0, 0, 0);

            openGLControl.FrameRate = 2400;
            //gl.DrawBuffer(OpenGL.GL_FRONT);
        }

        private void OpenGLControl_Resized(object sender, SharpGL.SceneGraph.OpenGLEventArgs args)
        {


            //openGLControl.FrameRate = 60; 

            //  Get the OpenGL object.
            OpenGL gl = openGLControl.OpenGL;

            //  Create an orthographic projection.
            gl.MatrixMode(MatrixMode.Projection);
            gl.LoadIdentity();
            gl.Ortho(-openGLControl.ActualWidth / 2, openGLControl.ActualWidth / 2, -openGLControl.ActualHeight / 2, openGLControl.ActualHeight / 2,-10,10);
            //  Back to the modelview.
            gl.MatrixMode(MatrixMode.Modelview);

        }

        private void OpenGLControl_OpenGLDraw(object sender, SharpGL.SceneGraph.OpenGLEventArgs args)
        {
            time = sw.ElapsedMilliseconds / 1000.0;

            //data = ((MainWindow)this.Owner).x;

            //double x1 = -data[0] * dots_per_meter;
            //double x2 = -(data[1] - 0.25) * dots_per_meter;

            //if (Math.Abs(x1-x1old) < 1e-6) {
            //    Console.WriteLine("{0} x1 same values", time);
            //}

            //if (Math.Abs(x2 - x2old) < 1e-6)
            //{
            //    Console.WriteLine("{0} x2 same values",time);
            //}

            //x1old = x1;
            //x2old = x2;
            double x1 = 0.0;
            double x2 = 100.0;
            //double x1 = 0.0;
            //double x2 = 0.0;
            //  Get the OpenGL instance.
            var gl = args.OpenGL;

            gl.Enable(OpenGL.GL_POLYGON_SMOOTH);

            gl.Clear(OpenGL.GL_COLOR_BUFFER_BIT | OpenGL.GL_DEPTH_BUFFER_BIT);
            gl.LoadIdentity();

            gl.PushMatrix();
            //  Draw a coloured pyramid.
            gl.Translate(x1, x2, 0.0f);
            gl.Scale(20.0, 20.0, 20.0);
            gl.Begin(OpenGL.GL_TRIANGLES);
                gl.Color(0.0f, 0.0f, 1.0f);
                gl.Vertex(0.0f, 1.0f, 0.0f);
                gl.Vertex(-1.0f, -1.0f, 0.0f);
                gl.Vertex(1.0f, -1.0f, 0.0f);
            gl.End();

            gl.PopMatrix();

            gl.PushMatrix();
            gl.Translate(500.0*Math.Sin(2.0*time), 0.0, 0.0f);
            gl.Scale(20.0, 20.0, 20.0);

            gl.Begin(OpenGL.GL_TRIANGLES);
            gl.Color(1.0f, 1.0f, 0.0f);
            gl.Vertex(0.0f, 1.0f, 0.0f);
            gl.Vertex(-1.0f, -1.0f, 0.0f);
            gl.Vertex(1.0f, -1.0f, 0.0f);
            gl.End();
            gl.PopMatrix();
            gl.Flush();
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            // Toggle fullscreen
            if (e.Key == Key.F )
            {
                if (this.WindowState == WindowState.Maximized)
                {
                    this.WindowState = WindowState.Normal;
                    this.WindowStyle = WindowStyle.SingleBorderWindow;
                }
                else
                {
                    this.WindowState = WindowState.Maximized;
                    this.WindowStyle = WindowStyle.None;
                }
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {

        }
    }
}
