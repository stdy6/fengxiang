// 全局变量
let tempGaugeChart, humidityGaugeChart
let tempChart, humidityChart, weightChart
let currentData = {
  temperature: 25,
  humidity: 60,
  weight: 15.2,
  beeCount: 5000,
  light: 800,
  co2: 400,
  tiltStatus: 'normal',
  honeyStatus: 'warning',
  deviceOnline: false, // 新增：设备在线状态
}

// 1. 定时请求接口，获取数据
const API_URL1 = 'https://iot-api.heclouds.com/thingmodel/query-device-property'
const API_URL2 =
  'https://iot-api.heclouds.com/thingmodel/query-device-property-detail'
const API_URL3 =
  'https://iot-api.heclouds.com/thingmodel/query-device-desired-property'
const API_URL4 =
  'https://iot-api.heclouds.com/thingmodel/set-device-desired-property'
const AUTH =
  'version=2022-05-01&res=userid%2F445428&et=2035444869&method=sha1&sign=FMUUFgzXk8igx%2B%2BkAGV31hzMBhE%3D'
const PRODUCT_ID = '6N5F59AL4m'
const DEVICE_NAME = 'test'

// 初始化所有图表
function initCharts() {
  initGaugeCharts()
  initLineCharts()
  updateDataDisplay()
  startDataPolling()
}

// 初始化仪表盘
function initGaugeCharts() {
  // 温度仪表盘
  tempGaugeChart = echarts.init(document.getElementById('tempGauge'))
  const tempGaugeOption = {
    series: [
      {
        type: 'gauge',
        startAngle: 180,
        endAngle: 0,
        min: 0,
        max: 100,
        center: ['50%', '70%'],
        splitNumber: 8,
        axisLine: {
          lineStyle: {
            width: 6,
            color: [
              [0.25, '#FF6E76'],
              [0.5, '#FDDD60'],
              [0.75, '#58D9F9'],
              [1, '#7CFFB2'],
            ],
          },
        },
        pointer: {
          icon: 'path://M12.8,0.7l12,40.1H0.7L12.8,0.7z',
          length: '12%',
          width: 20,
          offsetCenter: [0, '-60%'],
          itemStyle: {
            color: 'auto',
          },
        },
        axisTick: {
          length: 12,
          lineStyle: {
            color: 'auto',
            width: 2,
          },
        },
        splitLine: {
          length: 20,
          lineStyle: {
            color: 'auto',
            width: 5,
          },
        },
        axisLabel: {
          color: '#fff',
          fontSize: 12,
          distance: -60,
          formatter: function (value) {
            if (value === 15 || value === 35) {
              return ''
            }
            return value
          },
        },
        title: {
          offsetCenter: [0, '-30%'],
          fontSize: 14,
          color: '#00ffff',
        },
        detail: {
          fontSize: 24,
          offsetCenter: [0, '0%'],
          valueAnimation: true,
          formatter: function (value) {
            return Math.round(value) + '°C'
          },
          color: 'auto',
        },
        data: [
          {
            value: currentData.temperature,
            name: '温度',
          },
        ],
      },
    ],
  }
  tempGaugeChart.setOption(tempGaugeOption)

  // 湿度仪表盘
  humidityGaugeChart = echarts.init(document.getElementById('humidityGauge'))
  const humidityGaugeOption = {
    series: [
      {
        type: 'gauge',
        startAngle: 180,
        endAngle: 0,
        min: 0,
        max: 100,
        center: ['50%', '70%'],
        splitNumber: 8,
        axisLine: {
          lineStyle: {
            width: 6,
            color: [
              [0.25, '#FF6E76'],
              [0.5, '#FDDD60'],
              [0.75, '#58D9F9'],
              [1, '#7CFFB2'],
            ],
          },
        },
        pointer: {
          icon: 'path://M12.8,0.7l12,40.1H0.7L12.8,0.7z',
          length: '12%',
          width: 20,
          offsetCenter: [0, '-60%'],
          itemStyle: {
            color: 'auto',
          },
        },
        axisTick: {
          length: 12,
          lineStyle: {
            color: 'auto',
            width: 2,
          },
        },
        splitLine: {
          length: 20,
          lineStyle: {
            color: 'auto',
            width: 5,
          },
        },
        axisLabel: {
          color: '#fff',
          fontSize: 12,
          distance: -60,
          formatter: function (value) {
            if (value === 40 || value === 80) {
              return ''
            }
            return value
          },
        },
        title: {
          offsetCenter: [0, '-30%'],
          fontSize: 14,
          color: '#00ffff',
        },
        detail: {
          fontSize: 24,
          offsetCenter: [0, '0%'],
          valueAnimation: true,
          formatter: function (value) {
            return Math.round(value) + '%'
          },
          color: 'auto',
        },
        data: [
          {
            value: currentData.humidity,
            name: '湿度',
          },
        ],
      },
    ],
  }
  humidityGaugeChart.setOption(humidityGaugeOption)
}

// 初始化折线图
function initLineCharts() {
  // 生成时间数据
  const now = new Date()
  const timeData = []
  for (let i = 23; i >= 0; i--) {
    const time = new Date(now.getTime() - i * 60 * 60 * 1000)
    timeData.push(time.getHours() + ':00')
  }

  // 温度折线图
  tempChart = echarts.init(document.getElementById('tempChart'))
  const tempLineOption = {
    backgroundColor: 'transparent',
    grid: {
      left: '10%',
      right: '10%',
      top: '15%',
      bottom: '15%',
    },
    xAxis: {
      type: 'category',
      data: timeData,
      axisLine: { lineStyle: { color: '#00ffff' } },
      axisLabel: { color: '#00ffff', fontSize: 10 },
    },
    yAxis: {
      type: 'value',
      min: 15,
      max: 35,
      axisLine: { lineStyle: { color: '#00ffff' } },
      axisLabel: { color: '#00ffff', fontSize: 10 },
      splitLine: { lineStyle: { color: 'rgba(0,255,255,0.2)' } },
    },
    series: [
      {
        data: generateRandomData(24, 20, 30),
        type: 'line',
        smooth: true,
        lineStyle: { color: '#ff6b6b', width: 3 },
        itemStyle: { color: '#ff6b6b' },
        areaStyle: {
          color: {
            type: 'linear',
            x: 0,
            y: 0,
            x2: 0,
            y2: 1,
            colorStops: [
              { offset: 0, color: 'rgba(255,107,107,0.3)' },
              { offset: 1, color: 'rgba(255,107,107,0.1)' },
            ],
          },
        },
      },
    ],
  }
  tempChart.setOption(tempLineOption)

  // 湿度折线图
  humidityChart = echarts.init(document.getElementById('humidityChart'))
  const humidityLineOption = {
    backgroundColor: 'transparent',
    grid: {
      left: '10%',
      right: '10%',
      top: '15%',
      bottom: '15%',
    },
    xAxis: {
      type: 'category',
      data: timeData,
      axisLine: { lineStyle: { color: '#00ffff' } },
      axisLabel: { color: '#00ffff', fontSize: 10 },
    },
    yAxis: {
      type: 'value',
      min: 40,
      max: 80,
      axisLine: { lineStyle: { color: '#00ffff' } },
      axisLabel: { color: '#00ffff', fontSize: 10 },
      splitLine: { lineStyle: { color: 'rgba(0,255,255,0.2)' } },
    },
    series: [
      {
        data: generateRandomData(24, 50, 70),
        type: 'line',
        smooth: true,
        lineStyle: { color: '#4ecdc4', width: 3 },
        itemStyle: { color: '#4ecdc4' },
        areaStyle: {
          color: {
            type: 'linear',
            x: 0,
            y: 0,
            x2: 0,
            y2: 1,
            colorStops: [
              { offset: 0, color: 'rgba(78,205,196,0.3)' },
              { offset: 1, color: 'rgba(78,205,196,0.1)' },
            ],
          },
        },
      },
    ],
  }
  humidityChart.setOption(humidityLineOption)

  // 重量折线图
  weightChart = echarts.init(document.getElementById('weightChart'))
  const weightLineOption = {
    backgroundColor: 'transparent',
    grid: {
      left: '10%',
      right: '10%',
      top: '15%',
      bottom: '15%',
    },
    xAxis: {
      type: 'category',
      data: timeData,
      axisLine: { lineStyle: { color: '#00ffff' } },
      axisLabel: { color: '#00ffff', fontSize: 10 },
    },
    yAxis: {
      type: 'value',
      axisLine: { lineStyle: { color: '#00ffff' } },
      axisLabel: { color: '#00ffff', fontSize: 10 },
      splitLine: { lineStyle: { color: 'rgba(0,255,255,0.2)' } },
    },
    series: [
      {
        data: generateRandomData(24, 12, 18),
        type: 'line',
        smooth: true,
        lineStyle: { color: '#f9ca24', width: 3 },
        itemStyle: { color: '#f9ca24' },
        areaStyle: {
          color: {
            type: 'linear',
            x: 0,
            y: 0,
            x2: 0,
            y2: 1,
            colorStops: [
              { offset: 0, color: 'rgba(249,202,36,0.3)' },
              { offset: 1, color: 'rgba(249,202,36,0.1)' },
            ],
          },
        },
      },
    ],
  }
  weightChart.setOption(weightLineOption)
}

// 生成随机数据
function generateRandomData(count, min, max) {
  const data = []
  for (let i = 0; i < count; i++) {
    data.push(Math.random() * (max - min) + min)
  }
  return data
}

// 更新数据显示
function updateDataDisplay() {
  document.getElementById('tempValue').textContent =
    currentData.temperature.toFixed(1) + '°C'
  document.getElementById('envHumidityValue').textContent =
    currentData.humidity.toFixed(0) + '%'
  document.getElementById('hiveWeightValue').textContent =
    currentData.weight.toFixed(1) + 'g'
  document.getElementById('beeCountValue').textContent =
    currentData.beeCount.toLocaleString()
  document.getElementById('lightValue').textContent = currentData.light + 'lux'
  document.getElementById('co2Value').textContent = currentData.co2 + 'ppm'
}

// 更新状态显示
function updateStatusDisplay() {
  const tiltStatus = document.getElementById('tiltStatus')
  const honeyStatus = document.getElementById('honeyStatus')

  // 倾斜状态：description为"正常"显示绿色，否则橙色
  const tiltGreen = currentData.tiltDesc === '正常'
  tiltStatus.innerHTML =
    `<span class="status-icon ${tiltGreen ? 'normal' : 'warning'}">●</span>` +
    `<span class="status-text">${currentData.tiltDesc}</span>`

  // 采蜜通知：description为"不需采蜜"显示绿色，否则橙色
  const honeyGreen = currentData.honeyDesc === '不需采蜜'
  honeyStatus.innerHTML =
    `<span class="status-icon ${honeyGreen ? 'normal' : 'warning'}">●</span>` +
    `<span class="status-text">${currentData.honeyDesc}</span>`
}

// 2. 启动定时器，每5秒请求一次
function startDataPolling() {
  fetchAndUpdateData()
  setInterval(fetchAndUpdateData, 5000)
}

// 更新图表数据
function updateCharts() {
  // 更新仪表盘
  tempGaugeChart.setOption({
    series: [
      {
        data: [
          {
            value: currentData.temperature,
            name: '温度',
          },
        ],
      },
    ],
  })

  humidityGaugeChart.setOption({
    series: [
      {
        data: [
          {
            value: currentData.humidity,
            name: '湿度',
          },
        ],
      },
    ],
  })

  // 更新折线图（添加新数据点）
  const now = new Date()
  const timeStr =
    now.getHours() + ':' + now.getMinutes().toString().padStart(2, '0')

  // 更新温度折线图
  const tempData = tempChart.getOption().series[0].data
  tempData.push(currentData.temperature)
  if (tempData.length > 24) tempData.shift()

  const tempTimeData = tempChart.getOption().xAxis[0].data
  tempTimeData.push(timeStr)
  if (tempTimeData.length > 24) tempTimeData.shift()

  tempChart.setOption({
    xAxis: { data: tempTimeData },
    series: [{ data: tempData }],
  })

  // 更新湿度折线图
  const humidityData = humidityChart.getOption().series[0].data
  humidityData.push(currentData.humidity)
  if (humidityData.length > 24) humidityData.shift()

  const humidityTimeData = humidityChart.getOption().xAxis[0].data
  humidityTimeData.push(timeStr)
  if (humidityTimeData.length > 24) humidityTimeData.shift()

  humidityChart.setOption({
    xAxis: { data: humidityTimeData },
    series: [{ data: humidityData }],
  })

  // 更新重量折线图
  const weightData = weightChart.getOption().series[0].data
  weightData.push(currentData.weight)
  if (weightData.length > 24) weightData.shift()

  const weightTimeData = weightChart.getOption().xAxis[0].data
  weightTimeData.push(timeStr)
  if (weightTimeData.length > 24) weightTimeData.shift()

  weightChart.setOption({
    xAxis: { data: weightTimeData },
    series: [{ data: weightData }],
  })
}

// 1. 定时请求接口，获取数据
function fetchAndUpdateData() {
  fetch(API_URL1 + `?product_id=${PRODUCT_ID}&device_name=${DEVICE_NAME}`, {
    method: 'GET',
    headers: {
      Authorization: AUTH,
    },
  })
    .then((res) => res.json())
    .then((res) => {
      if (res.code === 0 && Array.isArray(res.data)) {
        const map = {}
        res.data.forEach((item) => {
          map[item.identifier] = item
        })
        // 更新全局数据
        currentData.temperature = Number(map.Temperature?.value) || 0
        currentData.humidity = Number(map.Humidity?.value) || 0
        currentData.weight = Number(map.Weight?.value) || 0
        currentData.beeCount = Number(map.Count?.value) || 0
        currentData.light = Number(map.LIGHT?.value) || 0
        currentData.co2 = Number(map.CO2?.value) || 0
        currentData.tiltStatus =
          map.WARN?.value === 'true' || map.WARN?.value === true
            ? 'danger'
            : 'normal'
        currentData.honeyStatus =
          map.notice?.value === 'true' || map.notice?.value === true
            ? 'warning'
            : 'normal'
        // 新增：保存description
        currentData.tiltDesc = map.WARN?.description || ''
        currentData.honeyDesc = map.notice?.description || ''
        // 更新图表和页面
        updateCharts()
        updateDataDisplay()
        updateStatusDisplay()
      }
    })
    .catch((err) => {
      console.error('数据请求失败', err)
    })
}

// 保存温度设置
function saveTempSettings() {
  const targetTemp = Number(document.getElementById('targetTemp').value)
  fetch(API_URL4, {
    method: 'POST',
    headers: {
      'Content-type': 'application/json',
      Authorization: AUTH,
    },
    body: JSON.stringify({
      product_id: PRODUCT_ID,
      device_name: DEVICE_NAME,
      params: { SETTEMP: targetTemp },
    }),
  })
    .then((res) => res.json())
    .then((res) => {
      if (res.code === 0) {
        alert('目标温度设置成功！')
      } else {
        alert('目标温度设置失败！')
      }
    })
    .catch(() => alert('目标温度设置失败！'))
}

// 保存湿度设置
function saveHumiditySettings() {
  const targetHumidity = Number(document.getElementById('targetHumidity').value)
  fetch(API_URL4, {
    method: 'POST',
    headers: {
      'Content-type': 'application/json',
      Authorization: AUTH,
    },
    body: JSON.stringify({
      product_id: PRODUCT_ID,
      device_name: DEVICE_NAME,
      params: { HE: targetHumidity },
    }),
  })
    .then((res) => res.json())
    .then((res) => {
      if (res.code === 0) {
        alert('目标湿度设置成功！')
      } else {
        alert('目标湿度设置失败！')
      }
    })
    .catch(() => alert('目标湿度设置失败！'))
}

// 窗口大小改变时重新调整图表
window.addEventListener('resize', function () {
  if (tempGaugeChart) tempGaugeChart.resize()
  if (humidityGaugeChart) humidityGaugeChart.resize()
  if (tempChart) tempChart.resize()
  if (humidityChart) humidityChart.resize()
  if (weightChart) weightChart.resize()
})

// 设备在线状态请求
function fetchDeviceStatus() {
  fetch(API_URL2, {
    method: 'POST',
    headers: {
      'Content-type': 'application/json',
      Authorization: AUTH,
    },
    body: JSON.stringify({
      product_id: PRODUCT_ID,
      device_name: DEVICE_NAME,
      params: [],
    }),
  })
    .then((res) => res.json())
    .then((res) => {
      currentData.deviceOnline = res.msg !== '获取属性失败:设备不在线'
      updateDeviceStatusDisplay()
    })
    .catch(() => {
      currentData.deviceOnline = false
      updateDeviceStatusDisplay()
    })
}

function startDeviceStatusPolling() {
  fetchDeviceStatus()
  setInterval(fetchDeviceStatus, 5000)
}

function updateDeviceStatusDisplay() {
  const statusSpan = document.querySelector('.con .status')
  if (currentData.deviceOnline) {
    statusSpan.textContent = '在线'
    statusSpan.className = 'status online'
  } else {
    statusSpan.textContent = '离线'
    statusSpan.className = 'status offline'
  }
}

// 页面加载时请求目标温度/湿度
function fetchDesiredTempHumidity() {
  fetch(API_URL3, {
    method: 'POST',
    headers: {
      'Content-type': 'application/json',
      Authorization: AUTH,
    },
    body: JSON.stringify({
      product_id: PRODUCT_ID,
      device_name: DEVICE_NAME,
      params: ['SETTEMP', 'HE'],
    }),
  })
    .then((res) => res.json())
    .then((res) => {
      if (res.code === 0 && res.data) {
        if (res.data.SETTEMP && typeof res.data.SETTEMP.value !== 'undefined') {
          document.getElementById('targetTemp').value = res.data.SETTEMP.value
        }
        if (res.data.HE && typeof res.data.HE.value !== 'undefined') {
          document.getElementById('targetHumidity').value = res.data.HE.value
        }
      }
    })
    .catch(() => {})
}

// 3. 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', function () {
  initCharts()
  startDataPolling()
  startDeviceStatusPolling()
  fetchDesiredTempHumidity() // 新增：加载目标温湿度
})
