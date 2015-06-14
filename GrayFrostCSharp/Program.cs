using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace GrayFrostCSharp
{
    class Program
    {
        [DllImport("GrayFrost32.dll", EntryPoint = "getV2Runtime")]
        public static extern int getV2Runtime32();

        [DllImport("GrayFrost64.dll", EntryPoint = "getV2Runtime")]
        public static extern int getV2Runtime64();

        public static void Main()
        {
            //Starts in 4.0 CLR... might need to pivot to 2.0 
            if (System.AppDomain.CurrentDomain.FriendlyName.Contains("Default"))
            {
                gotoV2CLR();
            }
            else
            {
                System.AppDomain.CurrentDomain.DoCallBack(loadAssembly);
            }
        }

        static void gotoV2CLR()
        {
            if (IntPtr.Size == 4)
            {
                getV2Runtime32();
            }
            else
            {
                getV2Runtime64();
            }
        }

        private static void loadAssembly()
        {
            System.Threading.ThreadPool.QueueUserWorkItem(new System.Threading.WaitCallback(
                delegate
                {
                    System.Threading.Thread t = new System.Threading.Thread(new System.Threading.ThreadStart(
                    delegate
                    {
                        //load the byte array as if it were an assembly. 
                        var yy = System.Reflection.Assembly.Load(payload.g_bInjectCode);
                        //grab the public static method Main (entry point). 
                        var gfd = yy.EntryPoint;
                        //Invoke the main method inside of our target appDomain. 
                        gfd.Invoke(null, new object[] { });
                    }));
                    t.Priority = System.Threading.ThreadPriority.Lowest;
                    t.SetApartmentState(System.Threading.ApartmentState.STA);
                    t.Start();
                    t.IsBackground = true;
                }), null);
            System.Threading.Thread.Sleep(100);
        }
    }
}
