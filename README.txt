干掉屑 IE, 让 Linux 也能玩上 B 服原神 !!!

特别声明: 本项目无任何窃取用户隐私行为, 所有代码公开透明, 提供编译好的文件仅为方便使用, 如果不放心, 可阅读代码自行编译

食用方法：
    0. 准备好运行环境 (Wine, DXVK, Patch, etc)
    1. 把 bin 目录下的 hook.exe 和 main.dll 放到 YuanShen.exe 的上级目录 (尽量不要放在同级或子目录)
    3. 打开 https://sdk.biligame.com/login/?gameId=4963&appKey=fd1098c0489c4d00a08aa8a15e484d6c&sdk_ver=3.5.0
    4. 按 F12 打开开发者工具 (Dev Tools) 切到控制台 (Console) 
    5. 控制台输入 loginSuccess=(data)=>{console.log(JSON.parse(data))} 回车
    6. 登录，看控制台输出, 把 access_key, uid, unmae 的内容替换到 login.json 指定位置
    7. 把 login.json 复制到 YuanShen.exe 所在目录
    8. 修改 launch.bat 最后一行为 start ..\hook.exe "YuanShen.exe %*" ..\main.dll
    9. 享受游戏时光~~~

特殊注意:
    1. login.json 务必是 UTF-8 编码, 且所有内容在第一行 
        (只能读到第一行的内容, 不要格式化 !!!)
        (虽然是因为摆烂不想也不太会写多行读取)
    2. 每次进游戏时都需要去第 4 步的网址登录一下, 不然会报 "渠道错误"
        (通常点了登录了就行, 正常登录成功不会有任何提示, 一般而言不需要再改 json)
        (大约一个月内 access_key 轻易是不会变的, 但是每次登录行为的有效期很短, 所以要手动去触发一下)

十分感谢:
    MinHook:    https://github.com/TsudaKageyu/minhook
    m**n**2:    为避免 takedown, 不提供地址
    dawn:       为避免 takedown, 不提供地址
    A****-GC:   为避免 takedown, 不提供地址
    PCGameSDK:  http://open.biligame.com/wiki/bili_pc_game/