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
    public partial class Alice : Window
    {
        public Alice()
        {
            InitializeComponent();

            new Thread(doSome).Start();

            //doSome();


        }
        
        public void doSome()
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
            
        }

        

        private void BttnTCP_Click(object sender, RoutedEventArgs e)
        {
            TCPconfig winTCP = new TCPconfig(this);
            winTCP.Show();
            this.Hide();
        }

        private void BttnProtocol_Click(object sender, RoutedEventArgs e)
        {
            ProtocolConfig winProtocol = new ProtocolConfig(this);
            winProtocol.Show();
            this.Hide();
        }

        private void BttnStart_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
