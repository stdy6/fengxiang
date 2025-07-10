const echarts = require('../../ec-canvas/echarts.js'); // 确保路径正确

Page({
  data: {
    startDate: '2025-07-05',
    endDate: '2025-07-09',
    chart: null, // 保留 chart 属性，但不再通过 setData 存储
    options: null,
    loading: false,
    error: null,
    lastUpdate: null,
  },

  onLoad() {
    this.initChart();
    this.queryData();
  },

  initChart() {
    const ctx = this.selectComponent('#chart'); // 获取 ec-canvas 组件
    ctx.init((canvas, width, height, dpr) => {
      const chart = echarts.init(canvas, null, {
        width: width,
        height: height,
        devicePixelRatio: dpr // 新增 dpr
      });
      canvas.setChart(chart); // 将 chart 绑定到 canvas
      this.chart = chart; // 直接保存到页面上下文
      this.updateChart([], [], []); // 初始化图表
    });
  },

  onStartDateChange(e) {
    this.setData({ startDate: e.detail.value });
  },

  onEndDateChange(e) {
    this.setData({ endDate: e.detail.value });
  },

  queryData() {
    const { startDate, endDate } = this.data;
    const startTime = Date.parse(startDate);
    const endTime = Date.parse(endDate);

    this.setData({ loading: true, error: null });

    // 使用 Promise.all 并行获取数据
    Promise.all([
      this.fetchDataFromOneNET('Weight', startTime, endTime),
      this.fetchDataFromOneNET('Humidity', startTime, endTime),
      this.fetchDataFromOneNET('Temperature', startTime, endTime)
    ])
    .then(([lightData, humidityData, temperatureData]) => {
      this.updateChart(lightData, humidityData, temperatureData);
    })
    .catch((error) => {
      this.handleError(error);
    });
  },

  fetchDataFromOneNET(identifier, startTime, endTime) {
    const apiConfig = {
      auth_info: 'version=2022-05-01&res=userid%2F445428&et=1752065480&method=sha1&sign=zv67YeBeY6EAQTA7QWaIbHJ%2FeO8%3D', // 替换为你的授权信息
      productId: '6N5F59AL4m', // 替换为你的产品ID
      deviceName: 'test', // 替换为你的设备名称
    };

    return new Promise((resolve, reject) => {
      wx.request({
        url: 'https://iot-api.heclouds.com/thingmodel/query-device-property-history', // 修复 URL
        method: 'GET',
        header: {
          'Authorization': apiConfig.auth_info,
          'Content-Type': 'application/json'
        },
        data: {
          product_id: apiConfig.productId,
          device_name: apiConfig.deviceName,
          identifier: identifier,
          start_time: startTime,
          end_time: endTime,
          page_size: 100 // 获取最多100条数据
        },
        success: (res) => {
          this.handleApiResponse(res.data, identifier, resolve);
        },
        fail: (err) => {
          reject(err);
        }
      });
    });
  },

  handleApiResponse(response, identifier, resolve) {
    if (response.code === 0 && response.data && response.data.list) {
      const properties = response.data.list;
      const processedData = this.processData(properties);
      resolve(processedData);
    } else {
      this.handleError(response.msg || '获取数据失败');
      resolve([]); // 如果失败，返回空数组
    }
  },

  processData(properties) {
    return properties.map(property => ({
      time: property.time, // 时间戳
      value: parseFloat(property.value) // 转换为数字
    }));
  },

  handleApiError(err) {
    let errorMsg = '网络请求失败';
    if (err && err.errMsg) {
      errorMsg += `: ${err.errMsg}`;
    }
    this.handleError(errorMsg);
  },

  handleError(message) {
    this.setData({
      loading: false,
      error: message
    });
  },

  updateChart(lightData, humidityData, temperatureData) {
    console.log('Light Data:', lightData);
    console.log('Humidity Data:', humidityData);
    console.log('Temperature Data:', temperatureData);

    const option = {
      tooltip: {
        trigger: 'axis',
        formatter: function (params) {
          let time = new Date(params[0].data.time).toLocaleString();
          return `${time}<br>${params[0].seriesName}: ${params[0].data.value}`;
        },
      },
      xAxis: {
        type: 'time',
        boundaryGap: false,
        axisLabel: {
          rotate: -45, // 设置 X 轴标签旋转角度
          formatter: function (value) {
            return new Date(value).toLocaleString(); // 格式化时间显示
          }
        }
      },
      yAxis: {
        type: 'value',
      },
      series: [
        {
          name: '光照强度',
          type: 'line',
          data: lightData.map(item => [item.time, item.value]), // 使用时间戳
        },
        {
          name: '湿度',
          type: 'line',
          data: humidityData.map(item => [item.time, item.value]), // 使用时间戳
        },
        {
          name: '温度',
          type: 'line',
          data: temperatureData.map(item => [item.time, item.value]), // 使用时间戳
        },
      ],
    };

    if (this.chart) {
      this.chart.setOption(option);
    } else {
      console.error('ECharts instance is not initialized');
    }

    this.setData({
      loading: false,
      lastUpdate: formatTime(new Date(), 'hh:mm:ss')
    });
  },
});

// 格式化时间函数
function formatTime(date, format) {
  const o = {
    'M+': date.getMonth() + 1,
    'd+': date.getDate(),
    'h+': date.getHours(),
    'm+': date.getMinutes(),
    's+': date.getSeconds(),
  };
  if (/(y+)/.test(format)) {
    format = format.replace(RegExp.$1, (date.getFullYear() + '').substr(4 - RegExp.$1.length));
  }
  for (let k in o) {
    if (new RegExp(`(${k})`).test(format)) {
      format = format.replace(RegExp.$1, (RegExp.$1.length === 1) ? (o[k]) : (('00' + o[k]).substr(('' + o[k]).length)));
    }
  }
  return format;
}