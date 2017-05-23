using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.IO;


namespace AppControlDisplay
{
    public class TCADSClientWorker : IDisposable
    {
        EventWaitHandle _wh = new AutoResetEvent(false);
        Thread _worker;
        readonly object _locker = new object();
        Queue<TCADSTask> _tasks = new Queue<TCADSTask>();

        public TCADSClientWorker()
        {
            _worker = new Thread(Work);
            //_worker.Start();
        }

        public void Dispose()
        {
            EnqueueTask(null);      // Signal the consumer to exit.
            _worker.Join();         // Wait for the consumer's thread to finish.
            _wh.Close();            // Release any OS resources.
        }

        public void EnqueueTask(TCADSTask task)
        {
            lock (_locker) _tasks.Enqueue(task);
            _wh.Set();
        }

        void Work()
        {
            while (true)
            {
                TCADSTask task = null;

                lock (_locker)
                    if (_tasks.Count > 0)
                    {
                        task = _tasks.Dequeue();
                        if (task == null) return;
                    }
                if (task != null)
                {

                    task.Work();        // Perform TcAdsClient action (read/write)

                    task = null;        // Set task to null (clean-up?)

                    //Thread.Sleep(1);    // simulate work...
                }
                else
                    _wh.WaitOne();      // No more tasks - wait for a signal
            }
        }


    }
}
