using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading;
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
using System.Windows.Threading;

namespace WpfApp1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class ProtocolConfig : Window
    {
        private Alice alice;
        public ProtocolConfig(Alice alice)
        {
            this.alice = alice;
            InitializeComponent();

            //new Thread(doSome).Start();

            //doSome();


        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            alice.Show();
            this.Close();
        }

        /* public void doSome()
         {
             for (int i = 0; i < 1000; i++)
             {
                 textBlock.Dispatcher.BeginInvoke(new Action(() =>
                 {
                     textBlock.Text = i.ToString();

                     //((Alice)System.Windows.Application.Current.MainWindow).UpdateLayout();
                 }));
                 Thread.Sleep(500);
             }

             Thread.Sleep(1000);
             Thread.Sleep(1000);
             textBlock.Dispatcher.BeginInvoke(new Action(() =>
             {
                 textBlock.Text = "3141351435345423534534531341341412";
             }));

         }*/


    }
}
