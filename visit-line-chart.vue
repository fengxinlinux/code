<template>
  <div id="visitLineChart" style="width:100%;height:400px;"></div>

</template>

<script>
  /* eslint-disable */
  var echarts = require('echarts');

  export default {
    name: 'visit-line-chart',
    data() {
      return {
        myChart: {},
        dayList:[],
        dataList1:[],
        dataList2:[],
        dataList3:[],
        dataList4:[],
        dataList5:[],
        dataList6:[],
      }
    },
    props: {
      "alldata": {
        type: Array
      }
    },
    methods: {
      initCharts() {
        let _this = this;
        _this.myChart = echarts.init(document.getElementById("visitLineChart"));
        var option = {
          title: {
            text: '访问变化趋势'
          },
          tooltip: {
            trigger: 'axis'
          },
          legend: {
            data: ['访问次数', '下载次数', '站内搜索数', '活动用户数', '页面停留平均时间', '操作数量'],
            selected: {
              "页面停留平均时间": false,
              "操作数量": false
            }
          },
          grid: {
            left: '3%',
            right: '4%',
            bottom: '3%',
            containLabel: true
          },
          toolbox: {
            feature: {
              saveAsImage: {}
            }
          },
          xAxis: {
            type: 'category',
            boundaryGap: false,
            data: this.dayList
          },
          yAxis: {
            type: 'value'
          },
          series: [{
              name: '访问次数',
              type: 'line',
              stack: '总量1',
              data: this.dataList1
            },
            {
              name: '下载次数',
              type: 'line',
              stack: '总量2',
              data: this.dataList2
            },
            {
              name: '站内搜索数',
              type: 'line',
              stack: '总量3',
              data: this.dataList3
            },
            {
              name: '活动用户数',
              type: 'line',
              stack: '总量4',
              data: this.dataList4
            },
            {
              name: '页面停留平均时间',
              type: 'line',
              stack: '总量4',
              data: this.dataList5
            },
            {
              name: '操作数量',
              type: 'line',
              stack: '总量4',
              data: this.dataList6
            }
          ]
        };
        _this.myChart.setOption(option);
      },
      updateCharts(){
        let _this=this;
        _this.getAllData();
       var option = {
         title: {
           text: '访问变化趋势'
         },
         tooltip: {
           trigger: 'axis'
         },
         legend: {
           data: ['访问次数', '下载次数', '站内搜索数', '活动用户数', '页面停留平均时间', '操作数量'],
           selected: {
             "页面停留平均时间": false,
             "操作数量": false
           }
         },
         grid: {
           left: '3%',
           right: '4%',
           bottom: '3%',
           containLabel: true
         },
         toolbox: {
           feature: {
             saveAsImage: {}
           }
         },
         xAxis: {
           type: 'category',
           boundaryGap: false,
           data: this.dayList
         },
         yAxis: {
           type: 'value'
         },
         series: [{
             name: '访问次数',
             type: 'line',
             stack: '总量1',
             data: this.dataList1
           },
           {
             name: '下载次数',
             type: 'line',
             stack: '总量2',
             data: this.dataList2
           },
           {
             name: '站内搜索数',
             type: 'line',
             stack: '总量3',
             data: this.dataList3
           },
           {
             name: '活动用户数',
             type: 'line',
             stack: '总量4',
             data: this.dataList4
           },
           {
             name: '页面停留平均时间',
             type: 'line',
             stack: '总量4',
             data: this.dataList5
           },
           {
             name: '操作数量',
             type: 'line',
             stack: '总量4',
             data: this.dataList6
           }
         ]
       };
       _this.myChart.clear();
       _this.myChart.setOption(option);
      },
      getAllData(){
        for(let i=0;i<this.alldata[i].length;i++)
        {
          this.dayList.push(this.alldata[i].day);
          this.dataList1.push(this.alldata[i].nbPageviews);
          this.dayList2.push(this.alldata[i].nbDownloads);
          this.dayList3.push(this.alldata[i].nbSearches);
          this.dayList4.push(this.alldata[i].nbUsers);
          this.dayList5.push(this.alldata[i].avgTimeOnSite);
          this.dayList6.push(this.alldata[i].nbActions);
        }
      }
    },
    mounted() {
      this.initCharts();
    },
    watch: {
      alldata: {
        handler(n, o) {
          this.updateCharts();
        },
        deep: true
      }
    }
  }
</script>

<style>
</style>
