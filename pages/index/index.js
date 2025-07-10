Page({
  data: {
    onenet_data: { data: [] },
    device_status: [],
    SETTEMP: 25, // 初始化为数值
    inputValue: '',
    intValue: 0,
    loading: false,
    error: null,
    lastUpdate: '',
    tempRange: {
      min: 15,
      max: 40,
   // 异常状态数据
    tiltAlert: false,       // 倾斜警告状态
    honeyReady: false    // 采蜜信息状态
    },
    // 新增状态标记
    isSetting: false,
    lastSetValue: null
  },

  config: {
    auth_info: "version=2022-05-01&res=userid%2F445428&et=1752065480&method=sha1&sign=zv67YeBeY6EAQTA7QWaIbHJ%2FeO8%3D",
    product_id: "6N5F59AL4m",
    device_name: "test",
    api_base_url: "https://iot-api.heclouds.com",
    cmd_timeout: 10000 ,// 添加命令超时时间(毫秒)
    
  },
  goToDatareviewPage: function() {
    wx.navigateTo({
      url: '/pages/datareview/datareview'
    });
  },
  onLoad(options) {
    const { start_time, end_time } = this.get_timestamps(); // 获取时间戳
   // this.config.start_time = start_time;
   // this.config.end_time = end_time;
    this.onenet_fetch_data(); // 先获取一次设备数据，更新页面
    //this.onenet_fetch_device_status(); // 获取设备在线情况
    setInterval(() => {
      const { start_time, end_time } = this.get_timestamps();
    //  this.config.start_time = start_time;
      //this.config.end_time = end_time;
  //    this.onenet_fetch_device_status(); // 定期获取设备状态
      this.onenet_fetch_data(); // 定期获取设备数据
    }, 3000); // 推荐每3000毫秒更新一次，根据实际数据刷新情况调整
  },

   
    
  /**
   * @简要：获取时间戳
   * @参数：无
   * @注意：计算当前时间和一周前的时间戳，用于请求数据的时间区间。
   * @返回值：对象 { start_time, end_time }
   */
  get_timestamps() {
    const now = new Date().getTime(); // 当前时间的时间戳
    const one_week_ago = now - 7 * 24 * 60 * 60 * 1000; // 一周前的时间戳
    return {
      start_time: one_week_ago,
      end_time: now
    };
  },
  /**
   * @简要：获取设备属性状态
   * @参数：无
   * @注意：向OneNET平台发送请求，获取设备的属性状态，并更新页面数据。
   * @返回值：无
   */
  onenet_fetch_data() {
    const { api_base_url, product_id, device_name, auth_info } = this.config;
    wx.request({
      url: `${api_base_url}/thingmodel/query-device-property?product_id=${product_id}&device_name=${device_name}`,
      method: "GET",
      header: {
        'Authorization': auth_info
      },
      success: (res) => {
        console.log("OneNET数据请求成功，返回数据：", res.data);
        if (res.data.code === 0) {
          this.setData({
            onenet_data: res.data, // 获取的数据放入onenet_data中
          });
        } else {
          // 请求成功但code不为0，视为错误，显示错误信息
          console.log("OneNET请求错误，错误信息：", res.data.msg);
          wx.showToast({
            title: res.data.msg || '请求出错', // 使用返回的错误信息，如果没有则显示默认信息
            icon: 'none', // 不显示图标
            duration: 2000 // 提示框自动隐藏的时间，单位是毫秒
          });
        }
      },
      fail: (err) => {
        // 请求失败，则在屏幕上显示错误信息
        console.log("OneNET数据请求失败");
        console.error(err); // 打印错误堆栈信息
        // 显示错误提示框
        wx.showToast({
          title: '请求失败', // 使用默认错误信息
          icon: 'none', // 不显示图标
          duration: 2000 // 提示框自动隐藏的时间，单位是毫秒
        });
      }
    });
  },

  // 输入框事件处理
  inputyz: function(e) {
    this.setData({
      inputValue: e.detail.value
    });
  },
  
  // 温度设置按钮处理
  tempsetting: function() {
    const value = this.data.inputValue;
    if (!value) {
      wx.showToast({
        title: '请输入温度设定值',
        icon: 'none'
      });
      return;
    }
    
    // 转换为整数
    const intValue = parseInt(value);
    
    if (isNaN(intValue)) {
      wx.showToast({
        title: '请输入数字',
        icon: 'none'
      });
      return;
    }
    
    // 验证数字范围 (示例: 温度范围15-40)
    if (intValue < this.data.tempRange.min || intValue > this.data.tempRange.max) {
      wx.showToast({
        title: `温度范围${this.data.tempRange.min}-${this.data.tempRange.max}℃`,
        icon: 'none'
      });
      return;
    }
    
    // 调用设置方法，传递整型值
    this.setDeviceProperty('SETTEMP', intValue);
    
    // 更新当前温度设定值
    this.setData({
      SETTEMP: intValue
    });
  },
  
  // 湿度设置按钮处理
  humiditySetting: function() {
    const value = this.data.inputValue;
    if (!value) {
      wx.showToast({
        title: '请输入湿度设定值',
        icon: 'none'
      });
      return;
    }
    
    // 转换为整数
    const intValue = parseInt(value);
    
    if (isNaN(intValue)) {
      wx.showToast({
        title: '请输入数字',
        icon: 'none'
      });
      return;
    }
    
    // 验证数字范围 (示例: 湿度范围20-100%)
    if (intValue < 20 || intValue > 100) {
      wx.showToast({
        title: '湿度范围20-100%',
        icon: 'none'
      });
      return;
    }
    
    // 调用设置方法，传递整型值
    this.setDeviceProperty('HE', intValue);
  },

  /**
   * @简要：设置设备属性
   * @参数：paramName - 参数名, value - 整数值
   * @注意：向OneNET平台发送请求，设置设备的属性值
   * @返回值：无
   */
  setDeviceProperty: function(paramName, value) {
    const { api_base_url, product_id, device_name, auth_info } = this.config;
    
    // 显示加载提示框
    wx.showLoading({
      title: '正在设置...', // 提示文字
      mask: true, // 是否显示透明蒙层，防止触摸穿透
    });
    
    wx.request({
      url: `${api_base_url}/thingmodel/set-device-property`,
      method: 'POST',
      header: {
        'Authorization': auth_info,
        'Content-Type': 'application/json' // 确保发送JSON格式
      },
      data: {
        "product_id": product_id,
        "device_name": device_name,
        "params": {
          [paramName]: value // 直接发送整数值
        }
      },
      success: (res) => {
        wx.hideLoading();
        console.log('OneNET属性设置响应:', res.data);
        
        if (res.data && res.data.code === 0) {
          // 无论平台返回什么，只要命令已发送，就更新本地状态
          this.setData({
            [paramName === 'SETTEMP' ? 'SETTEMP' : 'SETHUMI']: value
          });
          
          wx.showToast({
            title: '设置命令已发送',
            icon: 'success',
            duration: 2000
          });
        } else {
          // 即使平台返回超时错误，我们假设命令已经送达设备
          if (res.data.code === 10411) {
            // 更新本地状态
            this.setData({
              [paramName === 'SETTEMP' ? 'SETTEMP' : 'SETHUMI']: value
            });
            
            wx.showToast({
              title: '命令已发送',
              icon: 'none',
              duration: 3000
            });
          } else {
            // 其他错误处理
            wx.showToast({
              title: res.data.msg || '设置失败',
              icon: 'none',
              duration: 3000
            });
          }
        }
      },

      fail: (err) => {
        console.log('OneNET属性设置请求失败：', err); // 打印错误信息
        // 隐藏加载提示框
        wx.hideLoading();
        // 显示失败提示框
        wx.showToast({
          title: '设置失败，请检查网络', // 提示的文字内容
          icon: 'none', // 不显示图标
          duration: 1500 // 提示框自动隐藏的时间，单位是毫秒
        });
      }
    });
  },

  // 原有的onenet_set_device_property方法保持不变，用于开关类属性
  onenet_set_device_property(event) {
    const param_name = event.currentTarget.dataset.param; // 获取自定义数据
    const is_checked = event.detail.value; // 获取开关状态
    this.setDeviceProperty(param_name, is_checked);
  },

  
  // 其他原有方法保持不变...
  // (包括onenet_fetch_data, onenet_fetch_device_status等)
});

 