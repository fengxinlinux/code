<template>
  <div>
    <Card shadow>
      <Row>
        <Col span="13">
        <visit-histogram :alldata="allData"></visit-histogram>
        </Col>
        <Col span="11">
        <visit-pie-chart :alldata="allData"></visit-pie-chart>
        </Col>
      </Row>
    </Card>
    <br>
    <Card shadow v-if="isPie">
      <Row>
        <visit-line-chart :alldata="allData"></visit-line-chart>
      </Row>
    </Card>
    <visit-table :alldata="allData"></visit-table>
  </div>
</template>

<script>
  /* eslint-disable */
  import visitHistogram from './visit-histogram'
  import visitLineChart from './visit-line-chart'
  import visitPieChart from './visit-pie-chart'
  import visitTable from './visit-table'
  import {
    mapGetters
  } from 'vuex'
  import {
    getYesterday,
    getYesterdayByDate
  } from '@/libs/date'
  import * as OrgPiwikStatisticAPI from "@/api/statistics/org-piwik-statistic";
  export default {
    name: 'user-analyze-chart',
    data() {
      return {
        isPie: false,
        isStartDate:false,
        allData: [],
        sys_monitor_site_super_btn_perm: false,
        sys_monitor_site_org_parent_btn_perm: false,
        sys_monitor_site_org_btn_perm: false,
      }
    },
    methods: {
      getAllData() {
        let _this = this;
        let yesterday = _this.yesterday;
        if (this.sys_monitor_site_org_btn_perm) //机构用户权限
        {
          OrgPiwikStatisticAPI.fetchOrgList({
            dayStr: yesterday,
          }).then(resp => {
            _this.allData = resp.data.data;
            if (_this.allData.length > 1) { //开启折线图
              this.isPie = true;
            }
            else
            {
              this.isPie=false;
            }
          }).catch(err => {
            _this.$Message.error("获取列表信息失败");
          })
        }
      },
      reGetAllData() {
        let _this = this;
        let yesterday = _this.yesterday,
          startDate = _this.startDate,
          endDate = _this.endDate;
        if (!startDate || !endDate) {
          return false;
        }
        if (startDate > yesterday) {
          this.$Message.warning({
            content: "开始时间选择错误，请选择过去的时间",
            duration: 2,
          });
          return false;
        }
        if (endDate > yesterday) {
          this.$Message.warning({
            content: "结束时间错误，请选择过去的时间",
            duration: 2
          });
          return false;
        }
        if (this.sys_monitor_site_org_btn_perm) //机构用户权限
        {
          OrgPiwikStatisticAPI.fetchOrgList({
            flag: true,
            start: startDate,
            end: endDate
            // orderBy:"day ASC"
          }).then(resp => {
            _this.allData = resp.data.data;
            //_this.allData.reverse();
            if (_this.allData.length > 1) { //开启折线图
              this.isPie = true;
            }
            else
            {
              this.isPie=false;
            }
          }).catch(err => {
            _this.$Message.error("获取列表信息失败");
          })
        }
      }
    },
    computed: {
      ...mapGetters(['access', 'startDate', 'endDate']),
      yesterday() {
        return getYesterday('yyyy-MM-dd')
      },
    },
    components: {
      visitHistogram,
      visitLineChart,
      visitPieChart,
      visitTable,
    },
    mounted() {
      let _this = this;
      this.sys_monitor_site_super_btn_perm = this.access['monitor_site_sys'];
      this.sys_monitor_site_org_parent_btn_perm = this.access['monitor_site_parent'];
      this.sys_monitor_site_org_btn_perm = this.access['monitor_site_org'];
      _this.getAllData();
    },
    watch: {
      startDate() {
        this.isStartDate=true;
        this.reGetAllData();
        this.isStartDate=false;
      },
      endDate(){
        if(!this.isStartDate)
          this.reGetAllData();
      }
    }

  }
</script>

<style>
</style>
