using System;
using System.Diagnostics;
using System.Threading;
using System.IO;
using System.Net;
using System.Text;
using System.Collections.Generic;
using System.Web.Script.Serialization;

namespace Website
{
    public partial class Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!this.IsPostBack)
                UpdateReadings();
        }

        private void UpdateReadings()
        {
            // Create a request for the URL. 		
            WebRequest request = WebRequest.Create("http://api.thingspeak.com/channels/26429/feed/last.json");

            // Get the response.
            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            // Display the status.
            Console.WriteLine(response.StatusDescription);
            // Get the stream containing content returned by the server.
            Stream dataStream = response.GetResponseStream();
            // Open the stream using a StreamReader for easy access.
            StreamReader reader = new StreamReader(dataStream);
            // Read the content. 
            string responseFromServer = reader.ReadToEnd();
            // Display the content.
            JavaScriptSerializer deserializer = new JavaScriptSerializer();
            Dictionary<string, object> data = (Dictionary<string, object>)deserializer.DeserializeObject(responseFromServer);
            
            lblStoveTemp.Text = (String)data["field1"];
            lblAmbientTemp.Text = (String)data["field2"];
            lblDamper.Text = (String)data["field3"];

            DateTime toDateTime = DateTime.Parse((String)data["created_at"]);


            lblTime.Text = toDateTime.AddHours(1).ToShortTimeString();
            // Cleanup the streams and the response.
            reader.Close();
            dataStream.Close();
            response.Close();
        }
        protected void Timer1_Tick1(object sender, EventArgs e)
        {
            UpdateReadings();
        }

        protected void btnRefresh_Click(object sender, EventArgs e)
        {

        }
    }
}