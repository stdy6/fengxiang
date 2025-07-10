// app.js - 小程序全局逻辑脚本
App({
  // 全局数据对象（可在所有页面通过 getApp().globalData 访问）
  globalData: {
    userInfo: null,                 // 用户信息
    systemInfo: null,               // 设备系统信息
    apiBaseUrl: "https://iot-api.heclouds.com" , // 全局API基础URL
    apiToken: "version=2022-05-01&res=userid%2F445428&et=2035444869&method=sha1&sign=FMUUFgzXk8igx%2B%2BkAGV31hzMBhE%3D",                   // 全局API令牌
    appVersion: "1.0.0",            // 应用版本号
    lastLaunchTime: 0,              // 上次启动时间戳
    settings: {                     // 全局设置
      theme: "light",               // 主题模式：light/dark
      fontSize: 16,                 // 默认字体大小
      notification: true            // 是否接收通知
    }
  },
  
  // 生命周期函数：小程序初始化时执行
  onLaunch(options) {
    console.log('小程序初始化完成', options);
    
    // 获取设备系统信息
    try {
      const systemInfo = wx.getSystemInfoSync();
      this.globalData.systemInfo = systemInfo;
      console.log('设备信息:', systemInfo);
    } catch (e) {
      console.error('获取设备信息失败:', e);
    }
    
    // 检查更新
    if (wx.canIUse('getUpdateManager')) {
      this.checkForUpdates();
    }
    
    // 设置全局样式
    this.setGlobalStyle();
    
    // 记录启动时间
    this.globalData.lastLaunchTime = Date.now();
  },

  // 生命周期函数：小程序显示时执行
  onShow(options) {
    console.log('小程序显示', options);
  },

  // 生命周期函数：小程序隐藏时执行
  onHide() {
    console.log('小程序隐藏');
  },

  // 生命周期函数：全局错误处理
  onError(msg) {
    console.error('发生全局错误:', msg);
    // 这里可以添加错误上报逻辑
  },

  // 检查应用更新
  checkForUpdates() {
    const updateManager = wx.getUpdateManager();
    
    updateManager.onCheckForUpdate(res => {
      if (res.hasUpdate) {
        wx.showLoading({ title: '下载更新中...' });
      }
    });

    updateManager.onUpdateReady(() => {
      wx.hideLoading();
      wx.showModal({
        title: '更新提示',
        content: '新版本已经准备好，是否重启应用？',
        success(res) {
          if (res.confirm) {
            updateManager.applyUpdate();
          }
        }
      });
    });

    updateManager.onUpdateFailed(() => {
      wx.showToast({
        title: '更新失败',
        icon: 'none'
      });
    });
  },

  // 设置全局样式
  setGlobalStyle() {
    const { globalData } = this;
    
    // 根据系统状态栏高度设置导航栏位置
    if (globalData.systemInfo) {
      wx.setNavigationBarColor({
        frontColor: '#ffffff',
        backgroundColor: globalData.settings.theme === 'dark' ? '#2c2c2c' : '#007AFF'
      });
      
      // 设置全局默认字体大小
      const fontSizeMap = {
        small: 14,
        normal: 16,
        large: 18
      };
      
      const fontSize = fontSizeMap[globalData.systemInfo.fontSizeSetting] || 16;
      globalData.settings.fontSize = fontSize;
    }
  },

  // 自定义全局方法：登录
  login(callback) {
    wx.login({
      success: res => {
        if (res.code) {
          // 这里实际项目中应该将code发送到后端换取session
          console.log('登录凭证:', res.code);
          if (callback && typeof callback === 'function') {
            callback(true);
          }
          
          // 获取用户信息
          this.getUserInfo();
        } else {
          console.error('登录失败:', res.errMsg);
          if (callback && typeof callback === 'function') {
            callback(false);
          }
        }
      }
    });
  },

  // 自定义全局方法：获取用户信息
  getUserInfo() {
    wx.getSetting({
      success: res => {
        if (res.authSetting['scope.userInfo']) {
          // 已经授权，可以直接获取用户信息
          wx.getUserInfo({
            success: res => {
              this.globalData.userInfo = res.userInfo;
              console.log('用户信息:', res.userInfo);
            }
          });
        }
      }
    });
  },

  // 自定义全局方法：显示提示
  showToast(title, icon = 'none', duration = 2000) {
    wx.showToast({ title, icon, duration });
  },

  // 自定义全局方法：网络请求
  request(url, method = 'GET', data = {}, contentType = 'application/json') {
    return new Promise((resolve, reject) => {
      wx.request({
        url: `${this.globalData.apiBaseUrl}${url}`,
        method,
        data,
        header: {
          'Content-Type': contentType,
          'Authorization': this.globalData.apiToken
        },
        success: (res) => {
          if (res.statusCode === 200 && res.data.success) {
            resolve(res.data);
          } else {
            reject(res.data);
          }
        },
        fail: (err) => {
          reject(err);
        }
      });
    });
  },

  // 自定义全局方法：切换主题
  switchTheme() {
    const theme = this.globalData.settings.theme === 'light' ? 'dark' : 'light';
    this.globalData.settings.theme = theme;
    this.setGlobalStyle();
    
    // 通知所有页面主题改变
    wx.setNavigationBarColor({
      frontColor: theme === 'light' ? '#000000' : '#ffffff',
      backgroundColor: theme === 'light' ? '#f8f8f8' : '#2c2c2c'
    });
    
    // 可以在这里添加通知所有页面刷新的逻辑
  }
});