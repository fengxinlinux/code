<template>
  <div>
    <Row style="padding:10px">
      <Card>
        <div slot="title">
          <span style="font-size: 18px;color:black;font-weight: bold;">数据表格</span> &nbsp;&nbsp;
        </div>
        <div slot="extra">
          <Button type="primary" size="small" icon="ios-briefcase" @click="showExport">数据导出</Button>
        </div>
        <Table border :columns="columns1" :data="scoreData1" ref="table"></Table>
        <!-- <div style="margin: 10px;overflow: hidden">
          <div style="float: right;">
            <Page :total="1000" :current="1" @on-change="changePage"></Page>
          </div>
        </div> -->
      </Card>
    </Row>
    <div>
      <Modal v-model="modalShowExport" title="数据导出" width="50%" ok-text="确定">
        <export-component :columns="columns1" :tableRef="tableRef" :title="'主动检测数据导出'" :tableData="scoreData1" />
      </Modal>
    </div>
  </div>
</template>

<script>
  /* eslint-disable */
  //import axios from '@/libs/api.request'
  import {
    randomNum
  } from '@/libs/random'
  import ExportComponent from '@/components/export'

  export default {
    name: 'visit-table',
    components: {

    },
    components: {
      ExportComponent
    },
    data() {
      return {
        columns1: [{
            key: "day",
            title: '数据日期',
            "sortable": true,
            minWidth: 100
          },
          {
            key: "nbPageviews",
            title: '访问次数',
            "sortable": true,
            minWidth: 100
          },
          {
            key: "nbDownloads",
            title: '下载次数',
            "sortable": true,
            minWidth: 100
          },
          {
            key: "nbSearches",
            title: '站内搜索数',
            "sortable": true,
            minWidth: 110
          },
          {
            key: "nbUsers",
            title: '活动用户数',
            "sortable": true,
            minWidth: 110
          },
          {
            key: "avgTimeOnSite",
            title: '页面停留平均时间',
            "sortable": true,
            minWidth: 140
          },
          {
            key: "nbActions",
            title: '操作数量',
            "sortable": true,
            minWidth: 100
          },
          {
            key: "normalVisit",
            title: '正常访问',
            "sortable": true,
            minWidth: 100
          },
          {
            key: "bounceCount",
            title: '直接跳出',
            "sortable": true,
            minWidth: 100
          },
          {
            key: "newUser",
            title: '新用户访问',
            "sortable": true,
            minWidth: 110
          },
          {
            key: "nbVisitsReturning",
            title: '老用户访问',
            "sortable": true,
            minWidth: 110
          },
        ],
        scoreData1: [{
          day: '',
          nbPageviews: 0,
          nbDownloads: 0,
          nbSearches: 0,
          nbUsers: 0,
          avgTimeOnSite: 0,
          nbActions: 0,
          normalVisit: 0,
          bounceCount: 0,
          newUser: 0,
          nbVisitsReturning: 0
        }],
        tableRef: null, //数据导出用
        objList: [],
        modalShowExport: false,
      }
    },
    props: {
      "alldata": {
        type: Array
      }
    },
    mounted() {
      //this.init()
    },
    methods: {
      showExport() {
        this.modalShowExport = true;
        this.tableRef = this.$refs.table;
      },
      updateData() {
        var dataList = {};
        this.scoreData1 = [];
        for (let i = 0; i < this.alldata.length; i++) {
          dataList.day = this.alldata[i].day;
          dataList.nbPageviews = this.alldata[i].nbPageviews;
          dataList.nbDownloads = this.alldata[i].nbDownloads;
          dataList.nbSearches = this.alldata[i].nbSearches;
          dataList.nbUsers = this.alldata[i].nbUsers;
          dataList.avgTimeOnSite = this.alldata[i].avgTimeOnSite;
          dataList.nbActions = this.alldata[i].nbActions;
          dataList.normalVisit = this.alldata[i].nbPageviews - this.alldata[i].bounceCount;
          dataList.bounceCount = this.alldata[i].bounceCount;
          dataList.newUser = this.alldata[i].nbUniqVisitors - this.alldata[i].nbVisitsReturning;
          dataList.nbVisitsReturning = this.alldata[i].nbVisitsReturning;
          this.scoreData1[i]=JSON.parse(JSON.stringify(dataList));
        }
      },
      exportData(type) {
        if (type === 1) {
          this.$refs.table.exportCsv({
            filename: 'The original data'
          });
        } else if (type === 2) {
          this.$refs.table.exportCsv({
            filename: 'Sorting and filtering data',
            original: false
          });
        }
      },
    },
    watch: {
      alldata: {
        handler(n, o) {
          this.updateData();
        },
        deep: true
      }
    }
  }
</script>

<style scoped>

</style>
