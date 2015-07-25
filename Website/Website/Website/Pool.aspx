<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Pool.aspx.cs" Inherits="Website.Pool" %>

<!DOCTYPE html>
<html style="height: 100%;">
  <head>
    <script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>
    <script type="text/javascript" src="//thingspeak.com/highcharts-3.0.8.js"></script>
    <script type="text/javascript" src="//thingspeak.com/exporting.js"></script>

    <style type="text/css">
  body { background-color: white; height: 100%; margin: 0; padding: 0; }
  #chart-container { width: 712px; 
height: 405px; 
display: block; position:absolute; bottom:-78; 
top:138; 
left:5; 
right:78; 
margin: 5px 15px 15px 0; overflow: hidden; }
        .style1
        {
            width: 337px;
        }
    </style>


    <script type="text/javascript">

        

        // variables for the first series
        var series_1_channel_id = 24298;
        var series_1_field_number = 6;
        var series_1_read_api_key = 'T8MLPAM6JIVY86V5';
        var series_1_results = 1000;
        var series_1_color = '#d62020';
        var series_1_average = 10;

           
        

        // chart title
        var chart_title = 'FT Pool Temperature';
        // y axis title
        var y_axis_title = 'Degrees F';

        // user's timezone offset
        var my_offset = new Date().getTimezoneOffset();
        // chart variable
        var my_chart;

        // when the document is ready
        $(document).on('ready', function () {
            // add a blank chart
            addChart();
            var TimeList = document.getElementById('<%= ddTime.ClientID %>');
            var TimeValue = TimeList.options[TimeList.selectedIndex].value;
            var d = new Date();
            d.setHours(d.getHours() - TimeValue);
            //YYYY-MM-DD%20HH:NN:SS
            var series_1_start_date = d.toDateString() + '%20' + d.toTimeString();

            // add the first series
            addSeries(series_1_channel_id, series_1_field_number, series_1_read_api_key, series_1_results, series_1_color, series_1_start_date,series_1_average);
            
        });


        // add the base chart
        function addChart() {
            // variable for the local date in milliseconds
            var localDate;

            // specify the chart options
            var chartOptions = {
                chart: {
                    renderTo: 'chart-container',
                    defaultSeriesType: 'line',
                    backgroundColor: '#ffffff',
                    events: {}
                },
                title: { text: chart_title },
                plotOptions: {
                    series: {
                        marker: { radius: 3 },
                        animation: true,
                        step: false,
                        borderWidth: 0,
                        turboThreshold: 0
                    }
                },
                tooltip: {
                    // reformat the tooltips so that local times are displayed
                    enabled : false
                },
                xAxis: {
                    type: 'datetime',
                    title: { text: 'Time' }
                },
                yAxis:
                { labels: { format: '{value}°F', style: { color: '#d62020'} },
                    title: { text: 'Pool Temp', style: { color: '#d62020'} }, opposite: true
                         },
                exporting: { enabled: false },
                legend: { enabled: true },
                credits: {
                    text: 'Provided by : FT Conseil',
                    href: '',
                    style: { color: '#D62020' }
                }
            };

            // draw the chart
            my_chart = new Highcharts.Chart(chartOptions);
        }
        

        // add a series to the chart
        function addSeries(channel_id, field_number, api_key, results, color, startdate, average) {
            var field_name = 'field' + field_number;

            // get the data with a webservice call
            $.getJSON('https://api.thingspeak.com/channels/' + channel_id + '/fields/' + field_number + '.json?offset=0&round=2&min=55&start=' + startdate + '&average=' + average  +'&api_key=' + api_key, function (data) {

                // blank array for holding chart data
                var chart_data = [];

                // iterate through each feed
                $.each(data.feeds, function () {
                    var point = new Highcharts.Point();
                    // set the proper values
                    var value = this[field_name];
                    point.x = getChartDate(this.created_at);
                    point.y = parseFloat(value);
                    // add location if possible
                    if (this.location) { point.name = this.location; }
                    // if a numerical value exists add it
                    if (!isNaN(parseInt(value))) { chart_data.push(point); }
                });

                // add the chart data
                my_chart.addSeries({ data: chart_data, name: data.channel[field_name], color: color});
            });
        }

        // converts date format from JSON
        function getChartDate(d) {
            // get the data using javascript's date object (year, month, day, hour, minute, second)
            // months in javascript start at 0, so remember to subtract 1 when specifying the month
            // offset in minutes is converted to milliseconds and subtracted so that chart's x-axis is correct
            return Date.UTC(d.substring(0, 4), d.substring(5, 7) - 1, d.substring(8, 10), d.substring(11, 13), d.substring(14, 16), d.substring(17, 19)) - (my_offset * 60000);
        }

</script>



</head>
<body>
    <form id="form1" runat="server">
    
        <ContentTemplate>
          
            <table style="width:100%;">
                <tr>
                    <td class="style1">
                        <asp:Label ID="Label4" runat="server" Font-Size="X-Large" Text="Pool Temp :"></asp:Label>
                        <asp:Label ID="lblPoolTemp" runat="server" Font-Bold="True" 
                            Font-Size="X-Large" ForeColor="#D62020" Text="0"></asp:Label>
                        <asp:Label ID="lblTime" runat="server" Text="--:--:--"></asp:Label>
                    </td>
                    <td rowspan="3">
                        <asp:Button ID="btnRefresh" runat="server" Font-Bold="True" 
                            Font-Size="XX-Large" Height="73px" onclick="btnRefresh_Click" 
                            Text="Refresh" Width="451px" />
                    </td>
                </tr>
                <tr>
                    <td class="style1">
                        <asp:Label ID="Label5" runat="server" Font-Size="X-Large" Text="Ambient Temp :"></asp:Label>
                        <asp:Label ID="lblAmbientTemp" runat="server" Font-Bold="True" 
                            Font-Size="X-Large" ForeColor="Lime" Text="0"></asp:Label>
                    </td>
                </tr>
                <tr>
                    <td class="style1">
                        &nbsp;</td>
                </tr>
            </table>
            &nbsp;<asp:DropDownList 
            ID="ddTime" runat="server" Font-Size="X-Large" Height="41px" Width="143px" 
            AutoPostBack="True" >
            <asp:ListItem Value="12">12 Hours</asp:ListItem>
            <asp:ListItem Value="24">1 Day</asp:ListItem>
            <asp:ListItem Value="120">5 Days</asp:ListItem>
        </asp:DropDownList>
        <br />
        <br />
            <br />
            <br />
            <br />
            <br />
            <div ID="chart-container">
                &nbsp;</div>
        </ContentTemplate>
    
    </form>
</body>
</html>

