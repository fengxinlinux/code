<template>
  <div>
    <div id="visitPieChart" style="width:100%;height:400px;"></div>
   <!-- <p>{{alldata}}</p> -->
  </div>
</template>

<script>
  /* eslint-disable */
  var echarts = require('echarts');


  export default {
    name: 'visit-line-chart',
    data() {
      return {
        myChart: {},
        pieData: {
          "bounceCount": 0,
          "nb_pageviews": 0,
          "nbUniqVisitors": 0,
          "nbVisitsReturning": 0
        }
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
        _this.myChart = echarts.init(document.getElementById("visitPieChart"));
        var option = {
          title: [{
              text: '访问质量',
              subtext: '跳出率分析',
              x: '21.5%',
              y: '25%'
            },
            {
              text: '访问用户类型',
              subtext: '新老用户访问次数分析',
              x: '70%',
              y: '25%'
            }
          ],
          tooltip: {
            trigger: 'item',
            formatter: "{a} <br/>{b} : {c} ({d}%)"
          },
          legend: [{
              orient: 'vertical',
              left: '5%',
              bottom: '63%',
              data: ['直接跳出', '正常访问']
            },
            {
              orient: 'vertical',
              left: '55%',
              bottom: '63%',
              data: ['新用户访问', '老用户访问']
            }
          ],
          series: [{
              name: '访问质量',
              type: 'pie',
              radius: '55%',
              center: ['30%', '65%'],
              data: [{
                  value: this.pieData.bounceCount,
                  name: '直接跳出'
                },
                {
                  value: this.pieData.nbPageviews - this.pieData.bounceCount,
                  name: '正常访问'
                }
              ],
              itemStyle: {
                emphasis: {
                  shadowBlur: 10,
                  shadowOffsetX: 0,
                  shadowColor: 'rgba(0, 0, 0, 0.5)'
                }
              }
            },
            {
              name: '总访问次数',
              type: 'pie',
              radius: '55%',
              center: ['75%', '65%'],
              data: [{
                  value: this.pieData.nbUniqVisitors - this.pieData.nbVisitsReturning,
                  name: '新用户访问'
                },
                {
                  value: this.pieData.nbVisitsReturning,
                  name: '老用户访问'
                }
              ],
              itemStyle: {
                emphasis: {
                  shadowBlur: 10,
                  shadowOffsetX: 0,
                  shadowColor: 'rgba(0, 0, 0, 0.5)'
                }
              },

            }
          ]
        };
        _this.myChart.setOption(option);
      },
      getData() {
        this.pieData.bounceCount = this.pieData.nbPageviews = 0;
        this.pieData.nbUniqVisitors = this.pieData.nbVisitsReturning = 0;
        for (let i = 0; i < this.alldata.length; i++) {
          this.pieData.bounceCount += this.alldata[i].bounceCount;
          this.pieData.nbPageviews += this.alldata[i].nbPageviews;
          this.pieData.nbUniqVisitors += this.alldata[i].nbUniqVisitors;
          this.pieData.nbVisitsReturning += this.alldata[i].nbVisitsReturning;
        }
      },
      updateCharts() {
        let _this = this;
        _this.getData();
        _this.myChart.clear();
        var option = {
          title: [{
              text: '访问质量',
              subtext: '跳出率分析',
              x: '21.5%',
              y: '25%'
            },
            {
              text: '访问用户类型',
              subtext: '新老用户访问次数分析',
              x: '70%',
              y: '25%'
            }
          ],
          tooltip: {
            trigger: 'item',
            formatter: "{a} <br/>{b} : {c} ({d}%)"
          },
          legend: [{
              orient: 'vertical',
              left: '5%',
              bottom: '63%',
              data: ['直接跳出', '正常访问']
            },
            {
              orient: 'vertical',
              left: '55%',
              bottom: '63%',
              data: ['新用户访问', '老用户访问']
            }
          ],
          series: [{
              name: '访问质量',
              type: 'pie',
              radius: '55%',
              center: ['30%', '65%'],
              data: [{
                  value: this.pieData.bounceCount,
                  name: '直接跳出'
                },
                {
                  value: this.pieData.nbPageviews - this.pieData.bounceCount,
                  name: '正常访问'
                }
              ],
              itemStyle: {
                emphasis: {
                  shadowBlur: 10,
                  shadowOffsetX: 0,
                  shadowColor: 'rgba(0, 0, 0, 0.5)'
                }
              }
            },
            {
              name: '总访问次数',
              type: 'pie',
              radius: '55%',
              center: ['75%', '65%'],
              data: [{
                  value: this.pieData.nbUniqVisitors - this.pieData.nbVisitsReturning,
                  name: '新用户访问'
                },
                {
                  value: this.pieData.nbVisitsReturning,
                  name: '老用户访问'
                }
              ],
              itemStyle: {
                emphasis: {
                  shadowBlur: 10,
                  shadowOffsetX: 0,
                  shadowColor: 'rgba(0, 0, 0, 0.5)'
                }
              },

            }
          ]
        };
        _this.myChart.setOption(option);
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
