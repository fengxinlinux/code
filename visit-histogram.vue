<template>
  <div>
    <Row style="padding:10px">
      <i-col :md="24" :lg="screenWidth>1500?16:24">
        <div style="border:1px;">
          <div :style="{height:'400px',width:'100%','border-right':'1px #DCDCDC solid'}" id="visitHistogram"></div>
        </div>
        <Modal v-model="modal" title="选择显示的指标" :scrollable="true" @on-ok="settingOk" :loading="loading" width="50%">
          <Select v-model="selectedLabels" multiple style="width:100%;">
            <Option v-for="item in labelsList" :value="item.value" :key="item.value">{{ item.label }}</Option>
          </Select>
        </Modal>
      </i-col>

    </Row>
  </div>
</template>

<script>
  /* eslint-disable */
  var echarts = require('echarts');
  export default {
    name: 'visit-histogram',
    props: {
      "alldata": {
        type: Array
      }
    },
    data() {
      return {
        myChart: {},
        modal: false,
        screenWidth: window.screen.width * 0.57,
        loading: true,
        selectedLabels: ["nbPageviews", "nbDownloads", "nbSearches", "nbUsers"],
        chartDataSet: {},
        chartData: [],
        xData: [],
        labelsObj: {
          "nbPageviews": '访问次数',
          'nbDownloads': '下载次数',
          'nbSearches': '站内搜索数',
          'nbUsers': '活动用户数',
          'avgTimeOnSite': '页面停留平均时间',
          'nbActions': '操作数量'
        },

      }
    },
    computed: {
      labelsList() {
        let info = [];
        for (let key in this.labelsObj) {
          info.push({
            label: this.labelsObj[key],
            value: key
          });
        }
        return info;
      },
      getalldata() {
        return this.alldata;
      }
    },
    methods: {
      initCharts() {
        let _this = this;
        _this.myChart = echarts.init(document.getElementById("visitHistogram"));
        this.getChartDateSet();
        this.getChartDate();
        let option = {
          color: ['#3398DB'],
          title: {
            text: '总访问统计'
          },
          tooltip: {
            trigger: 'axis',
            axisPointer: { // 坐标轴指示器，坐标轴触发有效
              type: 'shadow' // 默认为直线，可选为：'line' | 'shadow'
            }
          },
          grid: {
            left: '3%',
            right: '4%',
            bottom: '3%',
            containLabel: true
          },
          xAxis: [{
            type: 'category',
            data: this.xData,
            axisTick: {
              alignWithLabel: true
            },
          }],
          yAxis: [{
            type: 'value'
          }],
          series: [{
            name: '次数',
            type: 'bar',
            barWidth: '60%',
            data: this.chartData
          }],
          toolbox: {
            show: true,
            x: 'right',
            y: 'top',
            feature: {
              dataView: {
                readOnly: false
              },
              //saveAsImage: {},
              myLableAdd: {
                show: true,
                title: '选择显示指标',
                icon: 'image://https://cdn.onlinewebfonts.com/svg/img_101423.png',
                onclick: function() {
                  _this.modal = true;
                }
              }
            }
          }
        };
        this.myChart.setOption(option);
      },
      settingOk() {
        let _this = this;
        if (!_this.selectedLabels || _this.selectedLabels.length == 0) {
          _this.warning("请至少选择一个指标展示!");
          setTimeout(() => {
            _this.loading = false;
            _this.$nextTick(() => {
              _this.loading = true;
            });
          }, 0);
          return;
        }
        _this.loading = false;
        this.updateChart();
      },
      warning(nodesc) {
        this.$Notice.warning({
          title: '共享网后台警告信息',
          desc: nodesc ? nodesc : '共享网警告信息 ',
          duration: 5
        });
      },
      getChartDate() {
        this.chartData = [];
        this.xData = [];
        for (let it in this.selectedLabels) {
          let label = this.selectedLabels[it];
          this.chartData.push(this.chartDataSet[label]);
          this.xData.push(this.labelsObj[label]);
        }
      },
      getChartDateSet() {
        var alldata2={"nbPageviews":0,"nbDownloads":0,"nbSearches":0,"nbUsers":0,"avgTimeOnSite":0,"nbActions":0};
        for(let i=0;i<this.alldata.length;i++)
        {
          alldata2.nbPageviews+=this.alldata[i].nbPageviews;
          alldata2.nbDownloads+=this.alldata[i].nbDownloads;
          alldata2.nbSearches+=this.alldata[i].nbSearches;
          alldata2.nbUsers+=this.alldata[i].nbUsers;
          alldata2.avgTimeOnSite+=this.alldata[i].avgTimeOnSite;
          alldata2.nbActions+=this.alldata[i].nbActions;
        }
        this.chartDataSet = JSON.parse(JSON.stringify(alldata2));
      },
      updateChart() {
        let _this = this;
        this.getChartDateSet();
        this.getChartDate();
        var option = {
          color: ['#3398DB'],
          title: {
            text: '总访问统计'
          },
          tooltip: {
            trigger: 'axis',
            axisPointer: { // 坐标轴指示器，坐标轴触发有效
              type: 'shadow' // 默认为直线，可选为：'line' | 'shadow'
            }
          },
          grid: {
            left: '3%',
            right: '4%',
            bottom: '3%',
            containLabel: true
          },
          xAxis: [{
            type: 'category',
            data: this.xData,
            axisTick: {
              alignWithLabel: true
            },
          }],
          yAxis: [{
            type: 'value'
          }],
          series: [{
            name: '次数',
            type: 'bar',
            barWidth: '60%',
            data: this.chartData
          }],
          toolbox: {
            show: true,
            x: 'right',
            y: 'top',
            feature: {
              dataView: {
                readOnly: false
              },
              myLableAdd: {
                show: true,
                title: '选择显示指标',
                icon: 'image://https://cdn.onlinewebfonts.com/svg/img_101423.png',
                onclick: function() {
                  _this.modal = true;
                }
              }
            }
          }
        };
        _this.modal = false;
        _this.myChart.clear();
        _this.myChart.setOption(option);
      }
    },
    mounted()
    {
      this.initCharts();
    },
    watch: {
      alldata: {
        handler(n, o) {
          this.updateChart();
        },
        deep: true
      }
    }
  }
</script>

<style>
</style>
