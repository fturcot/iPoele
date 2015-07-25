<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="Website.Default" %>


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
        var series_1_field_number = 1;
        var series_1_read_api_key = 'T8MLPAM6JIVY86V5';
        var series_1_results = 400;
        var series_1_color = '#d62020';
        var series_1_average = 5;

        // variables for the second series
        var series_2_channel_id = 24298;
        var series_2_field_number = 2;
        var series_2_read_api_key = 'T8MLPAM6JIVY86V5';
        var series_2_results = 400;
        var series_2_color = '#00aaff';
        var series_2_average = 5;

        // variables for the third series
        var series_3_channel_id = 24298;
        var series_3_field_number = 3;
        var series_3_read_api_key = 'T8MLPAM6JIVY86V5';
        var series_3_results = 400;
        var series_3_color = '#00aaff';
        var series_3_average = 5;

        // chart title
        var chart_title = 'FT Temperature Stove & Ambient & Damper';
        // y axis title
        var y_axis_title = 'Degrees C';

        // user's timezone offset
        var my_offset = new Date().getTimezoneOffset();
        // chart variable
        var my_chart;

        // when the document is ready
        $(document).on('ready', function () {
            // add a blank chart
            addChart();
            // add the first series
            addSeries(series_1_channel_id, series_1_field_number, series_1_read_api_key, series_1_results, series_1_color, 0);
            // add the second series
            //addSeries(series_2_channel_id, series_2_field_number, series_2_read_api_key, series_2_results, series_2_color, 1);
            // add the third series
            addSeries(series_3_channel_id, series_3_field_number, series_3_read_api_key, series_3_results, series_3_color, 1);
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
                    enabled: false
                },
                xAxis: {
                    type: 'datetime',
                    title: { text: 'Time' }
                },
                yAxis: [
                {
                    labels: { format: '{value}°C', style: { color: '#d62020' } },
                    title: { text: 'Stove Temp', style: { color: '#d62020' } }, opposite: true
                }/*,
                { labels: { format: '{value}°C', style: { color: '#00ff00'} },
                    title: { text: 'Amb Temp', style: { color: '#00ff00'} }, opposite: true
                }*/,
                {
                    labels: { format: '{value}%', style: { color: '#00aaff' } },
                    title: { text: 'Damper %', style: { color: '#00aaff' } }, opposite: false
                }
                ],
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
        function addSeries(channel_id, field_number, api_key, results, color, yAxis) {
            var field_name = 'field' + field_number;

            // get the data with a webservice call
            $.getJSON('https://api.thingspeak.com/channels/' + channel_id + '/fields/' + field_number + '.json?offset=0&round=2&results=' + results + '&api_key=' + api_key, function (data) {

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
                my_chart.addSeries({ data: chart_data, name: data.channel[field_name], color: color, yAxis: yAxis });
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
                        <asp:Label ID="Label4" runat="server" Font-Size="X-Large" Text="Stove Temp :"></asp:Label>
                        <asp:Label ID="lblStoveTemp" runat="server" Font-Bold="True" 
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
                        <asp:Label ID="label6" runat="server" Font-Size="X-Large" Text="Damper :"></asp:Label>
                        <asp:Label ID="lblDamper" runat="server" Font-Bold="True" Font-Size="X-Large" 
                            ForeColor="#00AAFF" Text="0"></asp:Label>
                    </td>
                </tr>
            </table>
            &nbsp;<br />
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
